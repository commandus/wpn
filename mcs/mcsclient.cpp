/*
 * Web push notification command line client
 * Copyright (C) 2018  Andrei Ivanov andrei.i.ivanov@commandus.com * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more $$details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#include <functional>
#include <stdint.h>
#include <fstream>
#include <sstream>
#include <ece.h>

#include "platform.h"
#include "nlohmann/json.hpp"

#include <google/protobuf/message.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>

#include "mcs.pb.h"
#include "mcsclient.h"
#include "utilstring.h"
#include "sslfactory.h"
#include "commandoutput.h"
#include "wp-push.h"

#include "utilvapid.h"
#include "utilrecv.h"
#include "params.h"

#ifdef _MSC_VER
#else
#endif

using json = nlohmann::json;

// MCS Message tags.
enum MCSProtoTag
{
	kHeartbeatPingTag       = 0,
	kHeartbeatAckTag        = 1,
	kLoginRequestTag        = 2,
	kLoginResponseTag       = 3,
	kCloseTag               = 4,
	kMessageStanzaTag       = 5,
	kPresenceStanzaTag      = 6,
	kIqStanzaTag            = 7,
	kDataMessageStanzaTag   = 8,
	kBatchPresenceStanzaTag = 9,
	kStreamErrorStanzaTag   = 10,
	kHttpRequestTag         = 11,
	kHttpResponseTag        = 12,
	kBindAccountRequestTag  = 13,
	kBindAccountResponseTag = 14,
	kTalkMetadataTag        = 15,
	kNumProtoTypes          = 16
};

enum MCSIqStanzaExtension {
  kSelectiveAck = 12,
  kStreamAck = 13,
};

using namespace mcs_proto;
using namespace google::protobuf::io;

#define MCS_PORT		443

static const std::string MCS_HOST("mtalk.google.com");
static const std::string DEF_CHROME_VER("63.0.3234.0");

static const uint8_t kMCSVersion = 41;

static const char *IQTYPE_NAMES[] = 
{
	"GET", "SET", "RESULT", "IQ_ERROR"
};

static MessageLite *mkPing()
{
	HeartbeatPing *req = new HeartbeatPing();
	// req->set_status(0);
	return req;
}

static MessageLite *mkAck
(
	const std::string &persistent_id
)
{
	IqStanza *req = new IqStanza();
	if (!req)
		return NULL;
	req->set_type(mcs_proto::IqStanza::SET);
	req->set_id("");
	req->set_persistent_id(persistent_id);
	req->mutable_extension()->set_id(kStreamAck);
	req->mutable_extension()->set_data("");
	return req;
}

/**
 * @param androidId 0- before register
 * @param securityToken 0- before register
 */
static MessageLite *mkLoginRequest
(
	uint64_t androidId,
	uint64_t securityToken,
	const std::vector<std::string> &persistentIds
)
{
	LoginRequest *req = new LoginRequest();
	req->set_adaptive_heartbeat(false);
	req->set_auth_service(LoginRequest_AuthService_ANDROID_ID);
	std::stringstream st;
	st << securityToken;
	req->set_auth_token(st.str());

	req->set_id("chrome-" + DEF_CHROME_VER);
	req->set_domain("mcs.android.com");

	std::stringstream sh;
	sh << std::hex << androidId;
	std::string haid = sh.str();
	std::stringstream ss;
	ss << androidId;
	std::string said = ss.str();

	req->set_device_id("android-" + haid);
	req->set_network_type(1);
	req->set_resource(said);
	req->set_user(said);
	req->set_use_rmq2(true);
	auto sc = req->mutable_setting();
	Setting *s = sc->Add();
	*s->mutable_name() = "new_vc";
	*s->mutable_value() = "1";
	for (std::vector<std::string>::const_iterator it(persistentIds.begin()); it != persistentIds.end(); ++it)
	{
		*req->add_received_persistent_id() = *it;
	}
	return req;
}

static void logMessage
(
	enum MCSProtoTag tag,
	MessageLite *message,
	int verbosity,
	std::ostream *log
) {
	switch (tag)
	{
	case kLoginResponseTag:
	{
		LoginResponse* r = (LoginResponse*)message;
		if (log && (verbosity >= 1))
			*log << "Login " << r->id() << " ";
		if (r->has_error())
		{
			if (log && (verbosity >= 0))
				*log << "error " << r->error().code();
			if (r->error().has_message())
			{
				if (log && (verbosity >= 0))
					*log << ": " << r->error().message() << " ";
			}
		}
		for (int i = 0; i < r->setting_size(); i++)
		{
			if (log && (verbosity >= 2))
				*log << "setting " << i << ":" << r->setting(i).name() << r->setting(i).value() << std::endl;
		}
		if (r->has_server_timestamp())
		{
			time_t t = r->server_timestamp() / 1000;
			struct tm *tm = localtime(&t);
			if (log && (verbosity >= 2))
				*log << "server time " << std::asctime(tm) << std::endl;
		}

		if (r->has_heartbeat_config())
		{
			if (log && (verbosity >= 2))
				*log << " has heart beat config" << std::endl;
		}
	}
	break;
	case kHeartbeatAckTag:
	{
		HeartbeatAck* r = (HeartbeatAck*)message;
		if (log && (verbosity >= 2))
			*log << " HeartbeatAck " << std::endl;
		if (r->has_last_stream_id_received())
		{
			if (log && (verbosity >= 2))
				*log << " last_stream_id_received: " << r->last_stream_id_received() << " ";
		}
		if (r->has_status())
		{
			if (log && (verbosity >= 2))
				*log << " status: " << r->status() << " ";
		}
		if (r->has_stream_id())
		{
			if (log && (verbosity >= 2))
				*log << " stream_id: " << r->stream_id() << " ";
		}
	}
	break;
	case kBindAccountResponseTag:
	{
	}
	break;
	case kIqStanzaTag:
	{
		IqStanza* r = (IqStanza*)message;
		if (log && (verbosity >= 3))
			*log << "IqStanza " << IQTYPE_NAMES[r->type()] << " " << r->id() << " ";
		if (r->has_rmq_id())
			if (log && (verbosity >= 3))
				*log << " rmq_id: " << r->rmq_id();
		if (r->has_from())
			if (log && (verbosity >= 3))
				*log << " from: " << r->from();
		if (r->has_to())
			if (log && (verbosity >= 3))
				*log << " to: " << r->to();
		if (r->has_error())
		{
			if (log && (verbosity >= 3))
				*log << " error: ";
			if (r->error().has_code())
				if (log && (verbosity >= 3))
					*log << " code: " << r->error().code();
			if (r->error().has_extension())
			{
				if (log && (verbosity >= 3))
					*log << " extension: ";
				if (r->error().extension().has_id())
					if (log && (verbosity >= 3))
						*log << " id: " << r->error().extension().id();
				if (r->error().extension().has_data())
					if (log && (verbosity >= 3))
						*log << " data: " << r->error().extension().data();
			}
		}
		if (r->has_extension())
		{
			if (log && (verbosity >= 3))
				*log << " extension: ";
			if (r->error().extension().has_id())
				if (log && (verbosity >= 3))
					*log << " id: " << r->error().extension().id();
			if (r->error().extension().has_data())
				if (log && (verbosity >= 3))
					*log << " data " << r->error().extension().data();
		}
		if (r->has_persistent_id())
			if (log && (verbosity >= 3))
				*log << " persistent_id: " << r->persistent_id();
		if (r->has_stream_id())
			if (log && (verbosity >= 3))
				*log << " stream_id: " << r->stream_id();
		if (r->has_last_stream_id_received())
			if (log && (verbosity >= 3))
				*log << " last_stream_id_received: " << r->last_stream_id_received();
		if (r->has_account_id())
			if (log && (verbosity >= 3))
				*log << " account_id: " << r->account_id();
		if (r->has_status())
			if (log && (verbosity >= 3))
				*log << " status: " << r->status();
		if (log && (verbosity >= 3))
			*log << std::endl;
	}
	break;
	case kDataMessageStanzaTag:
	{
		DataMessageStanza* r = (DataMessageStanza*)message;
		std::string cryptoKeyHeader;
		std::string encryptionHeader;
		std::string persistent_id;
		std::string from;
		std::string subtype;
		int64_t sent;

		if (log && (verbosity >= 3))
			*log << "DataMessageStanza" << std::endl;
		for (int a = 0; a < r->app_data_size(); a++)
		{
			if (log && (verbosity >= 3))
				*log << " app_data key: " << r->app_data(a).key()
				<< " data: " << r->app_data(a).value() << std::endl;
			if (r->app_data(a).key() == "crypto-key")
				cryptoKeyHeader = r->app_data(a).value();
			if (r->app_data(a).key() == "encryption")
				encryptionHeader = r->app_data(a).value();
			if (r->app_data(a).key() == "subtype")
				subtype = r->app_data(a).value();
		}
		if (r->has_id())
			if (log && (verbosity >= 3))
				*log << " id: " << r->id();
		if (r->has_category())
			if (log && (verbosity >= 3))
				*log << " category: " << r->category();
		if (r->has_device_user_id())
			if (log && (verbosity >= 3))
				*log << " device_user_id: " << r->device_user_id();
		if (r->has_from())
		{
			from = r->from();
			if (log && (verbosity >= 3))
				*log << " from: " << r->from();
		}
		else
		{
			from = "";
		}
		if (r->has_persistent_id())
		{
			persistent_id = r->persistent_id();
			if (log && (verbosity >= 3))
				*log << " persistent_id: " << r->persistent_id();
		}
		else
		{
			persistent_id = "";
		}
		if (r->has_token()) {
			if (log && (verbosity >= 3))
				*log << " token: " << r->token();
		}
		if (r->has_from_trusted_server())
			if (log && (verbosity >= 3))
				*log << " from_trusted_server: " << r->from_trusted_server();
		if (r->has_immediate_ack())
			if (log && (verbosity >= 3))
				*log << " immediate_ack: " << r->immediate_ack();
		if (r->has_last_stream_id_received())
			if (log && (verbosity >= 3))
				*log << " last_stream_id_received: " << r->last_stream_id_received();
		if (r->has_queued())
			if (log && (verbosity >= 3))
				*log << " queued: " << r->queued();
		if (log && (verbosity >= 3))
			*log << std::endl;
		if (r->has_raw_data())
		{
			if (log && (verbosity >= 3))
				*log << " raw_data: " << hexString(r->raw_data());
			std::string d;
		}
		if (r->has_reg_id())
			if (log && (verbosity >= 3))
				*log << " reg_id: " << r->reg_id();
		if (r->has_sent())
		{
			sent = r->sent();
			if (log && (verbosity >= 3))
				*log << " sent: " << r->sent();
		}
		if (r->has_status())
			if (log && (verbosity >= 3))
				*log << " status: " << r->status();
		if (r->has_stream_id())
			if (log && (verbosity >= 3))
				*log << " stream_id: " << r->stream_id();
		if (r->has_to())
			if (log && (verbosity >= 3))
				*log << " to: " << r->to();
		if (r->has_token())
			if (log && (verbosity >= 3))
				*log << " token: " << r->token();
		if (r->has_ttl())
			if (log && (verbosity >= 3))
				*log << " ttl: " << r->ttl();
		if (log && (verbosity >= 3))
			*log << std::endl;
	}
	break;
	default:
		break;
	}
}

/**
 * Decode string
 * @see https://tools.ietf.org/html/draft-ietf-webpush-encryption-03
 */
static int decode
(
	std::string &retval,
	const uint8_t *privateKeyArray,
	const uint8_t *authSecretArray,
	const std::string &source,
	const std::string &cryptoKeyHeader,
	const std::string &encryptionHeader
)
{
	uint32_t rs = 0;
	uint8_t salt[ECE_SALT_LENGTH];
	uint8_t rawSenderPubKey[ECE_WEBPUSH_PUBLIC_KEY_LENGTH];
	int err = ece_webpush_aesgcm_headers_extract_params1(cryptoKeyHeader.c_str(), encryptionHeader.c_str(),
		salt, ECE_SALT_LENGTH, rawSenderPubKey, ECE_WEBPUSH_PUBLIC_KEY_LENGTH, &rs);
	if (err != ECE_OK)
	{
		std::string m;
		switch (err) {
		case ECE_ERROR_INVALID_ENCRYPTION_HEADER:
			m = "Invalid encryption: " + encryptionHeader;
			break;
		case ECE_ERROR_INVALID_CRYPTO_KEY_HEADER:
			m = "Invalid crypto-key: " + cryptoKeyHeader;
			break;
		default:
			m = "";
			break;
		}
		return err;
	}
	size_t outSize = ece_aes128gcm_plaintext_max_length((const uint8_t*)source.c_str(), source.size());
	if (outSize == 0)
	{
		return ERR_MEM;
	}
	else
	{
		if (outSize < 4096)
			outSize = 4096;
		retval = std::string(outSize, '\0');
		ece_webpush_aesgcm_decrypt(
			privateKeyArray, ECE_WEBPUSH_PRIVATE_KEY_LENGTH,
			authSecretArray, ECE_WEBPUSH_AUTH_SECRET_LENGTH,
			(const uint8_t *)&salt, ECE_SALT_LENGTH,
			(const uint8_t *)&rawSenderPubKey, ECE_WEBPUSH_PUBLIC_KEY_LENGTH,
			rs,
			(const uint8_t *)source.c_str(), source.size(),
			(uint8_t *)retval.c_str(), &outSize);
		retval.resize(outSize);
	}
	return 0;
}

static void doSmth
(
	enum MCSProtoTag tag,
	MessageLite *message,
	MCSClient *client
) {
	switch (tag)
	{
	case kLoginResponseTag:
		break;
	case kHeartbeatAckTag:
		break;
	case kBindAccountResponseTag:
		break;
	case kIqStanzaTag:
		break;
	case kDataMessageStanzaTag:
	{
		DataMessageStanza* r = (DataMessageStanza*)message;
		std::string cryptoKeyHeader;
		std::string encryptionHeader;
		std::string persistent_id;
		std::string from;
		std::string subtype;
		int64_t sent;
		MessageLite *messageAck = mkAck(persistent_id);
		if (messageAck) {
			int r = client->sendTag(kIqStanzaTag, messageAck);
			delete messageAck;
		}
		if (r->has_raw_data())
		{
			std::string d;
			int dr = decode(d, 
				client->privateKey, client->authSecret, r->raw_data(), cryptoKeyHeader, encryptionHeader);
			if (dr == 0)
			{
				std::string appName;
				std::string appId;
				size_t start_pos = subtype.find("#");
				if (start_pos == std::string::npos)
				{
					appName = subtype;
					appId = "";
				}
				else
				{
					appName = subtype.substr(0, start_pos);
					appId = subtype.substr(start_pos + 1);
				}
				NotifyMessage notification;
				if (!persistent_id.empty())
				{
					// TODO save persistent
					// if (!client->getConfig()->setPersistentId(notification.authorizedEntity, persistent_id))
				}
				int mt = client->parseJSONNotifyMessage(notification, d);
				switch (mt) {
					case 0:
						sent = 0;
						client->notify(persistent_id, from, appName, appId, sent, notification);
						break;
					case 1:
					{
						std::string serverKey;
						std::string token;
						std::string persistent_id;
						std::string command;
						int code;
						std::string output;

						if (client->parseJSONCommandOutput(serverKey, token, persistent_id, command, &code, output, notification.data))
						{
							if (persistent_id.empty())
							{
								CommandOutput co;
								std::stringstream ss;
								int retcode = co.exec(&ss, command);
								if (retcode)
								{
								}
								std::string r = ss.str();
								if (r.size() == 0)
								{
								}
								// send response
								std::string output;
								int rp;
								if (true)
									rp = push2ClientDataFCM(&output, serverKey, token, from, persistent_id, command, retcode, r, 0);
								else {
									std::string p256dh = "";
									std::string auth = "";
									std::string sub = "";
									std::string contact = "";
									rp = webpushVapidData(output, serverKey, serverKey, token,
										p256dh, auth, persistent_id, command, retcode, r, 0, contact, AES128GCM, 0);
								}
								if ((rp >= 200) && (rp <= 399))
								{
								}
							}
						}
					}
					break;
				default:
					break;
				}
			}
		}
	}
	break;
	default:
		break;
	}
}

static MessageLite *createMessage
(
	uint8_t tag
)
{
	MessageLite *r = NULL;
	switch (tag)
	{
	case kHeartbeatPingTag:
		r = new HeartbeatPing();
		break;
	case kHeartbeatAckTag:
		r = new HeartbeatAck();
		break;
	case kLoginRequestTag:
		r = new LoginRequest();
		break;
	case kLoginResponseTag:
		r = new LoginResponse();
		break;
	case kCloseTag:
		r = new Close();
		break;
	case kMessageStanzaTag:
		break;
	case kPresenceStanzaTag:
		break;
	case kIqStanzaTag:
		r = new IqStanza();
		break;
	case kDataMessageStanzaTag:
		r = new DataMessageStanza();
		break;
	case kBatchPresenceStanzaTag:
		break;
	case kStreamErrorStanzaTag:
		break;
	case kHttpRequestTag:
		break;
	case kHttpResponseTag:
		break;
	case kBindAccountRequestTag:
		break;
	case kBindAccountResponseTag:
		break;
	case kTalkMetadataTag:
		break;
	case kNumProtoTypes:
		break;
	default:
		break;
	}
	return r;
}

static int nextMessage(
	enum MCSProtoTag *retTag,
	MessageLite *retMessage,
	std::string &buffer,
	int verbosity,
	std::ostream *log
)
{
	std::stringstream ss(buffer);
	IstreamInputStream rawInput(&ss);
	CodedInputStream codedInput(&rawInput);
	// tag number, size, message
	uint8_t tag;
	bool r = codedInput.ReadRaw(&tag, 1);	// 1 byte long
	if (!r) {
		retMessage = NULL;
		return 0;
	}

	uint32_t msgSize;
	r = codedInput.ReadVarint32(&msgSize);
	if (!r) {
		retMessage = NULL;
		return 0;
	}

	if (log && (verbosity >= 1))
		*log << "<<<  Tag " << (int)tag << " size " << msgSize << "  >>>" << std::endl;

	*retTag = (enum MCSProtoTag) tag;
	google::protobuf::io::CodedInputStream::Limit limit = codedInput.PushLimit(msgSize);
	retMessage = createMessage(tag);
	if (retMessage)
	{
		r = retMessage->ParsePartialFromCodedStream(&codedInput);
		if (!r)
			return 0;
		r = codedInput.ConsumedEntireMessage();
		std::string d;
		retMessage->SerializeToString(&d);
		if (log && (verbosity >= 1))
			*log << "Tag: " << (int) tag << " size: " << msgSize << ": " << hexString(d) << std::endl;
	}
	else
	{
		r = codedInput.Skip(msgSize);
	}
	int sz = codedInput.CurrentPosition();
	codedInput.ConsumedEntireMessage();
	codedInput.PopLimit(limit);
	if (sz)
	{
		buffer.erase(0, sz);
	}
	return sz;
}

//----------------------------- MCSClient -----------------------------

MCSClient::MCSClient(
	const uint8_t *privateKey,
	const uint8_t *authSecret,
	uint64_t androidId,
	uint64_t securityToken,
	OnNotifyFunc onNotify,
	void *onNotifyEnv,
	int verbosity
)
	: state(STATE_VERSION)
{
	this->privateKey = privateKey;
	this->authSecret = authSecret;
	this->androidId = androidId;
	this->securityToken = securityToken;
	this->onNotify = onNotify;
	this->onNotifyEnv = onNotifyEnv;
	this->verbosity = verbosity;
}

MCSClient::MCSClient
(
	const MCSClient& other
)
	: state(STATE_VERSION)
{
	this->privateKey = other.privateKey;
	this->authSecret = other.authSecret;
	this->androidId = other.androidId;
	this->securityToken = other.securityToken;
	this->onNotify = other.onNotify;
	this->onNotifyEnv = other.onNotifyEnv;
	this->verbosity = other.verbosity;
}

MCSClient::~MCSClient()
{
	stop();
}

static int readLoop(MCSClient *client)
{
	unsigned char buffer[4096];
	client->log(3) << "Listen loop started" << std::endl;
	int r = client->logIn();
	while (!client->mStop)
	{
		int r = SSL_read(client->mSsl, buffer, sizeof(buffer));
		if (r > 0) 
		{
			// mClient->log(3) << "Received " << r << " bytes: " << std::endl << hexString(std::string((char *) buffer, r)) << std::endl;
			client->put(buffer, r);
			if (size_t c = client->process())
			{
				client->log(3) << "Processed " << c << " messages" << std::endl;
			}
		}
		sleep(0);
	}
	client->log(3) << "Listen loop stopped" << std::endl;
	return 0;
}

int MCSClient::connect()
{
	if ((!privateKey) || (!authSecret))
		return ERR_NO_KEYS;
	if ((!androidId) || (!securityToken))
		return ERR_NO_CREDS;
	
	mSsl = mSSLFactory.open(&mSocket, MCS_HOST, MCS_PORT);
	if (!mSsl)
		return ERR_NO_CONNECT;
	mStop = false;
	
	if (verbosity > 1)
		log(3) << "Android id: " << androidId 
			<< " security token: " << securityToken << std::endl;
	std::thread listenerThread(readLoop, this);
	listenerThread.detach();
	return 0;
}

void MCSClient::stop()
{
	mStop = true;
}

bool MCSClient::hasIdNToken()
{
	return ((!androidId) && (!securityToken));
}

std::ostream &MCSClient::log
(
	int level
)
{
	if (verbosity >= level)
	{
		return std::cerr;
	}
	else
	{
		return onullstrm;
	}
}

int MCSClient::logIn()
{
	if ((!androidId) || (!securityToken))
		return ERR_NO_CREDS;
	if (!mSsl)
		return ERR_NO_CONNECT;
	if (verbosity > 2)
	{
		log(3) << "Login to " << MCS_HOST << std::endl;
	}

	std::vector<std::string> persistentIds;
	// TODO get persistent ids
	// mConfig->getPersistentIds(persistentIds);
	
	if (verbosity >= 3)
	{
		log(3) << "Saved persistent ids:" << std::endl;
		for (std::vector<std::string>::const_iterator it(persistentIds.begin()); it != persistentIds.end(); ++it)
		{
			log(3) << *it << std::endl;
		}
	}
	MessageLite *messageLogin =  mkLoginRequest(androidId, securityToken, persistentIds);
	if (!messageLogin)
		return ERR_MEM;

	sendVersion();

	sendTag(kLoginRequestTag, messageLogin);

	delete messageLogin;
	int r = 0;
	return r;
}

int MCSClient::sendVersion()
{
	std::stringstream ss;
	ss << kMCSVersion;
	std::string r = ss.str();
	log(3) << "Send version: " << hexString(r) << std::endl;
	return SSL_write(mSsl, r.c_str(), r.size());
}

int MCSClient::sendTag
(
	uint8_t tag,
	const MessageLite *msg
)
{
	std::string s = tagNmessageToString(tag, msg);
	log(3) << "Send tag: " << (int) tag << hexString(s) << std::endl;
	return SSL_write(mSsl, s.c_str(), s.size());
}

int MCSClient::ping()
{
	log(3) << "ping.." << std::endl;
	MessageLite *l =  mkPing();
	if (!l)
		return -1;
	int r = sendTag(kHeartbeatPingTag, l);
	delete l;
	return r;
}

void MCSClient::writeStream
(
	std::istream &strm
)
{
	std::string s;
	while (!strm.eof())
	{
		strm >> s;
		if ((s == "q") || (s == "quit"))
			break;
		if ((s == "p") || (s == "ping"))
		{
			ping();
		}
	}
}

void MCSClient::mkNotifyMessage
(
	NotifyMessage &retval,
	const std::string &authorizedEntity,	///< e.g. 246829423295
	const std::string &title,
	const std::string &body,
	const std::string &icon,
	const std::string &click_action,
	const std::string &data
)
{
	retval.authorizedEntity = authorizedEntity;
	retval.title = title;
	retval.body = body;
	retval.icon = icon;
	retval.link = click_action;
	retval.data = data;
}

/**
 * Parse command
 * @param retval return value. If it is data, return data JSON string in retval.data
 * @param value JSON data to be parsed
 * @return true
 */
bool MCSClient::parseJSONCommandOutput
(
	std::string &serverKey,
	std::string &token,
	std::string &persistent_id,
	std::string &command,
	int *code,
	std::string &output,
	const std::string &value
)
{
	bool r = true;
	try
	{
		json m = json::parse(value);
		try
		{
			command = m.at("command");
		}
		catch(...)
		{
			r = false;
		}
		try
		{
			serverKey = m.at("serverKey");
		}
		catch(...)
		{
			serverKey = "";
		}
		try
		{
			token = m.at("token");
		}
		catch(...)
		{
			token = "";
		}
		try
		{
			persistent_id = m.at("persistent_id");
		}
		catch(...)
		{
			persistent_id = "";
		}
		try
		{
			output = m.at("output");
		}
		catch(...)
		{
			output = "";
		}
		if (code)
		{
			try
			{
				*code = m.at("code");
			}
			catch(...)
			{
				*code = 0;
			}
		}
	}
	catch(...)
	{
		r = false;
	}
	return r;
}

/**
 * Parse FCM JSON message into notification structure or copy data 
 * @param retval return value. If it is data, return data JSON string in retval.data
 * @param value JSON data to be parsed
 * @return 0- notification, 1- data, less 0 error: -1 not notification or data found
 */
int MCSClient::parseJSONNotifyMessage
(
	NotifyMessage &retval,
	const std::string &value
)
{
	int r = -1;
	try
	{
		json m = json::parse(value);
		try
		{
			retval.authorizedEntity = m.at("from");
		}
		catch(...)
		{
		}
		try
		{
			json notification  = m.at("notification");
			r = 0;
			try
			{
				retval.title = notification.at("title");
			}
			catch(...)
			{
				retval.title = "";
			}
			try
			{
				retval.body = notification.at("body");
			}
			catch(...)
			{
				retval.body = "";
			}
			try
			{
				retval.icon = notification.at("icon");
			}
			catch(...)
			{
				retval.icon = "";
			}
			try
			{
				retval.link = notification.at("click_action");
			}
			catch(...)
			{
				retval.link = "";
			}
		}
		catch(...)
		{
		}

		try
		{
			json d = m.at("data");
			r = 1;
			retval.data = d.dump();
		}
		catch(...)
		{
		}
	}
	catch(...)
	{
	}
	return r;
}

void MCSClient::notify
(
	const std::string &persistent_id,
	const std::string &from,
	const std::string &appName,
	const std::string &appId,
	int64_t sent,
	const NotifyMessage &notification
) const
{
	if (onNotify) {
		NotifyMessage response;
		onNotify(onNotifyEnv, persistent_id, from, appName, appId, sent, &notification, &response);
	}
}

// Return 0 if incomplete and is not parcelable
void MCSClient::put(const void *buf, int size)
{
	mStream.append(std::string((char*)buf, size));
}

int MCSClient::process()
{
	int sz = mStream.size();
	int count = 0;
	while (sz > 0)
	{
		if (state == STATE_VERSION)
		{
			uint8_t version = (uint8_t) mStream[0]; // last known is 38
			log(3) << "MCS version: " << (int) version << std::endl;
			mStream.erase(0, 1);
			sz--;
			state = STATE_TAG;
		}
		else {
			MessageLite *m = NULL;
			enum MCSProtoTag tag;
			sz = nextMessage(&tag, m, mStream, verbosity, &std::cerr);
			if (!m)
				continue;
			logMessage(tag, m, verbosity, &std::cerr);
			doSmth(tag, m, this);
			delete m;
			count++;
		}
	}
	return count;
}
