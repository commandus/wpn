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

static MessageLite *mkHeartbeatAck()
{
	HeartbeatAck *req = new HeartbeatAck();
	// req->set_status(0);
	return req;
}

static MessageLite *mkStreamAck()
{
	IqStanza *req = new IqStanza();
	if (!req)
		return NULL;
	req->set_type(mcs_proto::IqStanza::SET);
	req->set_id("");
	req->mutable_extension()->set_id(kStreamAck);
	req->mutable_extension()->set_data("");
	return req;
}

static MessageLite *mkSelectiveAck
(
	const std::vector<std::string> &ids
) 
{
	IqStanza *req = new IqStanza();
	req->set_type(mcs_proto::IqStanza::SET);
	req->set_id("");
	req->mutable_extension()->set_id(kSelectiveAck);
	SelectiveAck ack;
	for (size_t i = 0; i < ids.size(); ++i)
	{
		ack.add_id(ids[i]);
	}
	req->mutable_extension()->set_data(ack.SerializeAsString());
	return req;
}

static MessageLite *mkSelectiveAck1
(
	const std::string &id
) 
{
	IqStanza *req = new IqStanza();
	req->set_type(mcs_proto::IqStanza::SET);
	req->set_id("");
	req->mutable_extension()->set_id(kSelectiveAck);
	SelectiveAck ack;
	ack.add_id(id);
	req->mutable_extension()->set_data(ack.SerializeAsString());
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

CallbackLogger::CallbackLogger(
)
	: verbosity(0), onLog(NULL), onLogEnv(NULL)
{
}

void CallbackLogger::setCallback(
	OnLogC aonLog,
	void *aonLogEnv
)
{
	this->onLog = aonLog;
	this->onLogEnv = aonLogEnv;
}

int CallbackLogger::overflow(int c)
{
	if (c != EOF)
	{
		if (c != '\n')
		{
			buffer << c;
		}
		else
		{
			if (onLog)
				onLog(onLogEnv, verbosity, buffer.str().c_str());
			buffer.clear();
		}
	}
	return c;
}

CallbackLogger &CallbackLogger::flush()
{
	if (onLog)
		onLog(onLogEnv, verbosity, buffer.str().c_str());
	buffer.clear();
	return *this;
}

static std::string getExtensionName(int extensionId)
{
	if (extensionId == 12)
		return "SelectiveAck";
	else
		if (extensionId == 12)
			return "StreamAck";
		else
			return "Unknown";
}

static bool BuildPersistentIdListFromProto(const google::protobuf::string& bytes, std::vector<std::string>* id_list) 
{
	mcs_proto::SelectiveAck selective_ack;
	if (!selective_ack.ParseFromString(bytes))
		return false;
	std::vector<std::string> new_list;
	for (int i = 0; i < selective_ack.id_size(); ++i) {
		new_list.push_back(selective_ack.id(i));
	}
	id_list->swap(new_list);
	return true;
}

static void logMessage
(
	enum MCSProtoTag tag,
	MessageLite *message,
	int verbosity,
	CallbackLogger *log
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
				*log << "setting " << i << ":" << r->setting(i).name() << r->setting(i).value() << "\n";
		}
		if (r->has_server_timestamp())
		{
			time_t t = r->server_timestamp() / 1000;
			struct tm *tm = localtime(&t);
			if (log && (verbosity >= 2))
				*log << "server time " << std::asctime(tm) << "\n";
		}

		if (r->has_heartbeat_config())
		{
			if (log && (verbosity >= 2))
				*log << " has heart beat config" << "\n";
		}
	}
	break;
	case kHeartbeatAckTag:
	{
		HeartbeatAck* r = (HeartbeatAck*)message;
		if (log && (verbosity >= 2))
			*log << " HeartbeatAck " << "\n";
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
		IqStanza* r = (IqStanza*) message;
		if (log && (verbosity >= 3)) {
			*log << "IqStanza " << IQTYPE_NAMES[r->type()] << " id: " << r->id() << " ";
			if (r->has_rmq_id())
				*log << " rmq_id: " << r->rmq_id();
			if (r->has_from())
				*log << " from: " << r->from();
			if (r->has_to())
				*log << " to: " << r->to();
			if (r->has_error())
			{
				*log << " error: {";
				if (r->error().has_code())
					*log << " code: " << r->error().code();
				if (r->error().has_extension())
				{
					*log << " extension: {";
					if (r->error().extension().has_id())
						*log << " id: " << r->error().extension().id();
					if (r->error().extension().has_data())
						*log << " data: " << r->error().extension().data();
					*log << "} ";
				}
				*log << "} ";
			}
			if (r->has_extension())
			{
				*log << " extension: {";
				if (r->extension().has_id())
					*log << " id: " << r->extension().id() << getExtensionName(r->extension().id());
				if (r->extension().has_data())
				{
					std::vector<std::string> ackedIds;
					if (BuildPersistentIdListFromProto(r->extension().data(), &ackedIds)) 
					{
						std::stringstream ss;
						for (std::vector<std::string>::const_iterator it = ackedIds.begin(); it != ackedIds.end(); ++it)
						{
							ss << *it << "; ";
						}
						*log << " list " << ss.str();	// \n#0:1541907792665672%7031b2e6f9fd7ecd
					}
					else
					{
						*log << " data " << r->extension().data();	// \n#0:1541907792665672%7031b2e6f9fd7ecd
					}
				}
				*log << "} ";
			}
			if (r->has_persistent_id())
				*log << " persistent_id: " << r->persistent_id() << " size: " << r->persistent_id().size();
			if (r->has_stream_id())
				*log << " stream_id: " << r->stream_id();
			if (r->has_last_stream_id_received())
				*log << " last_stream_id_received: " << r->last_stream_id_received();
			if (r->has_account_id())
				*log << " account_id: " << r->account_id();
			if (r->has_status())
					*log << " status: " << r->status();
			if (log && (verbosity >= 3))
				*log << "\n";
		}
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
		std::string content_encoding;
		int64_t sent;

		if (log && (verbosity >= 3))
			*log << "DataMessageStanza" << "\n";
		for (int a = 0; a < r->app_data_size(); a++)
		{
			if (log && (verbosity >= 3))
				*log << " app_data key: " << r->app_data(a).key()
				<< " data: " << r->app_data(a).value() << "\n";
			if (r->app_data(a).key() == "crypto-key")
				cryptoKeyHeader = r->app_data(a).value();
			if (r->app_data(a).key() == "encryption")
				encryptionHeader = r->app_data(a).value();
			if (r->app_data(a).key() == "subtype")
				subtype = r->app_data(a).value();
			if (r->app_data(a).key() == "content-encoding")
				content_encoding = r->app_data(a).value();	// aes128gcm
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
			*log << "\n";
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
			*log << "\n";
	}
	break;
	default:
		break;
	}
}

/**
 * aesgcm Decode string
 * @see https://tools.ietf.org/html/draft-ietf-webpush-encryption-03
 */
static int decode_aesgcm
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

/**
 * aesgcm Decode string
 * @see https://tools.ietf.org/html/draft-ietf-webpush-encryption-03
 */
static int decode_aes128gcm
(
	std::string &retval,
	const uint8_t *privateKeyArray,
	const uint8_t *authSecretArray,
	const std::string &source
)
{
	size_t outSize = ece_aes128gcm_plaintext_max_length((const uint8_t*) source.c_str(), source.size());
	if (outSize == 0)
	{
		return ERR_MEM;
	}

	if (outSize < 4096)
		outSize = 4096;
	retval = std::string(outSize, '\0');

	int r = ece_webpush_aes128gcm_decrypt(
		privateKeyArray, ECE_WEBPUSH_PRIVATE_KEY_LENGTH, authSecretArray, ECE_WEBPUSH_AUTH_SECRET_LENGTH,
		(const uint8_t*) source.c_str(), source.size(), (uint8_t*) retval.c_str(), &outSize
	);
	retval.resize(outSize);
	return r;
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
	case kHeartbeatPingTag:
		{
			MessageLite *messageHeartbeatAck = mkHeartbeatAck();
			if (messageHeartbeatAck) 
			{
				int r = client->send(kHeartbeatAckTag, messageHeartbeatAck);
				if (r < 0)
					client->log << severity(3) << "Send heartbeat ACK with error " << r << "\n";
				else
					client->log << severity(0) << "Sent heartbeat ACK successfully " << r << " bytes\n";
				delete messageHeartbeatAck;
			}
		}
		break;
	case kBindAccountResponseTag:
		break;
	case kCloseTag:
		client->disconnect();
		// TODO reconnect?
		break;
	case kIqStanzaTag:
		{
		if (!((IqStanza*) message)->has_extension())
			break;
		const mcs_proto::Extension& iqExtension =  ((IqStanza*) message)->extension();
		switch (iqExtension.id()) 
		{
			case kSelectiveAck: 
			{
				std::vector<std::string> ackedIds;
				if (BuildPersistentIdListFromProto(iqExtension.data(), &ackedIds)) 
				{
					std::stringstream ss;
					for (std::vector<std::string>::const_iterator it = ackedIds.begin(); it != ackedIds.end(); ++it)
					{
						ss << *it << "; ";
					}
					client->log << severity(3) << "Send selective ACK to " << ss.str() << "\n";
					MessageLite *resp = mkSelectiveAck(ackedIds);
					if (resp) 
					{
						int r = client->send(kIqStanzaTag, resp);
						if (r < 0)
							client->log << severity(3) << "Send selective ACK with error " << r << "\n";
						else
							client->log << severity(0) << "Sent selective ACK successfully " << r << " bytes\n";
						delete resp;
					}
				}
				return;
			}
		}
		}
		break;
	case kDataMessageStanzaTag:
	{
		DataMessageStanza* r = (DataMessageStanza*) message;
		std::string cryptoKeyHeader;
		std::string encryptionHeader;
		std::string subtype;
		std::string content_encoding;

		DataMessageStanza* dms = (DataMessageStanza*) message;
		for (int a = 0; a < dms->app_data_size(); a++)
		{
			if (dms->app_data(a).key() == "crypto-key")
				cryptoKeyHeader = dms->app_data(a).value();
			if (dms->app_data(a).key() == "encryption")
				encryptionHeader = dms->app_data(a).value();
			if (dms->app_data(a).key() == "subtype")
				subtype = dms->app_data(a).value();
			if (dms->app_data(a).key() == "content-encoding")
				content_encoding = dms->app_data(a).value();	// "aes128gcm"
		}

		std::string persistent_id = r->persistent_id();
		std::string from = r->from();
		std::string appName = "";
		std::string appId = "";
		int64_t sent = r->sent();

		MessageLite *messageAck = mkStreamAck();
		if (messageAck) {
			int r = client->send(kIqStanzaTag, messageAck);
			if (r < 0)
				client->log << severity(3) << "Send ACK id: " << persistent_id << " with error " << r << "\n";
			else
				client->log << severity(0) << "Sent ACK id: " << persistent_id << " successfully " << r << " bytes\n";
			delete messageAck;
		}
		
		MessageLite *messageAck1 = mkSelectiveAck1(persistent_id);
		if (messageAck1) {
			int r = client->send(kIqStanzaTag, messageAck1);
			if (r < 0)
				client->log << severity(3) << "Send ACK id: " << persistent_id << " with error " << r << "\n";
			else
				client->log << severity(0) << "Sent ACK id: " << persistent_id << " successfully " << r << " bytes\n";
			delete messageAck;
		}
		
		if (r->has_raw_data())
		{
			std::string d;
			int dr;
			if (content_encoding == "aes128gcm") {
				dr = decode_aes128gcm(d, client->privateKey, client->authSecret, r->raw_data());
			} else {
				dr = decode_aesgcm(d, client->privateKey, client->authSecret, r->raw_data(), cryptoKeyHeader, encryptionHeader);
			}
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
						} else {
							notification.data = "Error parse " + notification.data;
							client->notify(persistent_id, from, appName, appId, sent, notification);
						}
					}
					break;
				default:
					break;
				}
			} else {
				client->log << severity(0) << "Decoding error "
				<< " private key: " << base64UrlEncode(client->privateKey, ECE_WEBPUSH_PRIVATE_KEY_LENGTH) << "\n"
				<< " auth secret: " << base64UrlEncode(client->authSecret, ECE_WEBPUSH_AUTH_SECRET_LENGTH) << "\n"
				<< " Header: cryptoKey: " << cryptoKeyHeader << "\n"
				<< " Header: encryptionHeader: " << encryptionHeader << "\n"
				<< "\n";
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
	MessageLite **retMessage,
	std::string &buffer,
	int verbosity,
	CallbackLogger *log
)
{
	std::stringstream ss(buffer);
	IstreamInputStream rawInput(&ss);
	CodedInputStream codedInput(&rawInput);
	// tag number, size, message
	uint8_t tag;
	bool r = codedInput.ReadRaw(&tag, 1);	// 1 byte long
	if (!r) {
		*retMessage = NULL;
		return 0;
	}

	uint32_t msgSize;
	r = codedInput.ReadVarint32(&msgSize);
	if (!r) {
		*retMessage = NULL;
		return 0;
	}

	int p = codedInput.CurrentPosition();
	if (msgSize + p > buffer.size()) {
		*retMessage = NULL;
		return 0;
	}

	if (log && (verbosity >= 1))
		*log << severity(1) << "Tag: " << (int) tag << " size: " << msgSize << ": " << hexString(buffer) << "\n";

	*retTag = (enum MCSProtoTag) tag;
	google::protobuf::io::CodedInputStream::Limit limit = codedInput.PushLimit(msgSize);
	*retMessage = createMessage(tag);
	if (*retMessage)
	{
		r = (*retMessage)->ParsePartialFromCodedStream(&codedInput);
		if (!r)
			return 0;
		r = codedInput.ConsumedEntireMessage();
		std::string d;
		(*retMessage)->SerializeToString(&d);
		/*
		if (log && (verbosity >= 1))
			*log << severity(1) << hexString(d) << "\n";
		*/
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
	const std::string &privateKey,
	const std::string &authSecret,
	uint64_t androidId,
	uint64_t securityToken,
	OnNotifyC onNotify,
	void *onNotifyEnv,
	OnLogC onLog,
	void *onLogEnv,
	int verbosity
)
	: state(STATE_VERSION), listenerThread(NULL)
{
	ece_base64url_decode(privateKey.c_str(), privateKey.size(), ECE_BASE64URL_REJECT_PADDING, this->privateKey, ECE_WEBPUSH_PRIVATE_KEY_LENGTH);
	ece_base64url_decode(authSecret.c_str(), authSecret.size(), ECE_BASE64URL_REJECT_PADDING, this->authSecret, ECE_WEBPUSH_AUTH_SECRET_LENGTH);
	this->androidId = androidId;
	this->securityToken = securityToken;
	this->onNotify = onNotify;
	this->onNotifyEnv = onNotifyEnv;
	this->onLog = onLog;
	this->onLogEnv = onLogEnv;
	this->verbosity = verbosity;
	log.setCallback(this->onLog, this->onLogEnv);
}

MCSClient::MCSClient
(
	const MCSClient& other
)
	: state(STATE_VERSION), listenerThread(NULL)
{
	memmove(this->privateKey, other.privateKey, sizeof(this->privateKey));
	memmove(this->authSecret, other.authSecret, sizeof(this->authSecret));
	this->androidId = other.androidId;
	this->securityToken = other.securityToken;
	this->onNotify = other.onNotify;
	this->onNotifyEnv = other.onNotifyEnv;
	this->onLog = other.onLog;
	this->onLogEnv = other.onLogEnv;
	this->verbosity = other.verbosity;
}

MCSClient::~MCSClient()
{
	disconnect();
}

bool MCSClient::ready()
{
	return (!mStop) && (mSsl != NULL);
}

static int readLoop(MCSClient *client)
{
	client->log << severity(3) << "Listen loop started" << "\n";
	int r;
	while ((r = client->read()) >= 0)
	{
		sleep(0);
	}
	client->log << severity(3) << "Listen loop stopped" << "\n";
	return 0;
}

int MCSClient::connect()
{
	/*
	 * Maybe later check are keys assigned
	if ((!privateKey) || (!authSecret))
		return ERR_NO_KEYS;
	*/
	if ((!androidId) || (!securityToken))
		return ERR_NO_CREDS;
	
	mSsl = mSSLFactory.connect(&mSocket, MCS_HOST, MCS_PORT);
	if (!mSsl)
		return ERR_NO_CONNECT;
	mStop = false;
	if (verbosity > 1)
		log << severity(3) << "Android id: " << androidId 
			<< " security token: " << securityToken << "\n";
	state = STATE_VERSION;
	int r = logIn();
	if (r == 0) {
		listenerThread = new std::thread(readLoop, this);
		listenerThread->detach();
	}
	return r;
}

void MCSClient::disconnect()
{
	mStop = true;
	if (mSocket && mSsl)
		mSSLFactory.disconnect(mSocket, mSsl);
	mSocket = INVALID_SOCKET;
	mSsl = NULL;

	if (listenerThread) {
		if (listenerThread->joinable()) {
			listenerThread->join();
		}
	}

	state = STATE_VERSION;
}

bool MCSClient::hasIdNToken()
{
	return ((!androidId) && (!securityToken));
}

/**
  * Return 0- success, <0- error
  */
int MCSClient::logIn()
{
	if ((!androidId) || (!securityToken))
		return ERR_NO_CREDS;
	if (!mSsl)
		return ERR_NO_CONNECT;
	if (verbosity > 2)
	{
		log << severity(3) << "Login to " << MCS_HOST << "\n";
	}

	std::vector<std::string> persistentIds;
	// TODO get persistent ids
	// mConfig->getPersistentIds(persistentIds);
	
	if (verbosity >= 3)
	{
		log << severity(3) << "Saved persistent ids:" << "\n";
		for (std::vector<std::string>::const_iterator it(persistentIds.begin()); it != persistentIds.end(); ++it)
		{
			log << severity(3) << *it << "\n";
		}
	}
	MessageLite *messageLogin =  mkLoginRequest(androidId, securityToken, persistentIds);
	if (!messageLogin)
		return ERR_MEM;

	sendVersion();

	send(kLoginRequestTag, messageLogin);

	delete messageLogin;
	return 0;
}

int MCSClient::sendVersion()
{
	std::stringstream ss;
	ss << kMCSVersion;
	std::string r = ss.str();
	log << severity(3) << "Send version: " << hexString(r) << "\n";
	return SSL_write(mSsl, r.c_str(), r.size());
}

int MCSClient::send
(
	uint8_t tag,
	const MessageLite *msg
)
{
	std::string s = tagNmessageToString(tag, msg);
	log << severity(3) << "Send tag: " << hexString(s) << "\n";
	if (mSsl)
		return SSL_write(mSsl, s.c_str(), s.size());
	else
		return ERR_NO_CONNECT;
}

int MCSClient::ping()
{
	log << severity(3) << "ping.." << "\n";
	MessageLite *l =  mkPing();
	if (!l)
		return ERR_MEM;
	int r = send(kHeartbeatPingTag, l);
	delete l;
	return r;
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

static void copyRefNotifyMessage(NotifyMessageC *retval, const NotifyMessage *value)
{
	retval->authorizedEntity = value->authorizedEntity.c_str();
	retval->title = value->title.c_str();
	retval->body = value->body.c_str();
	retval->icon = value->icon.c_str();
	retval->sound = value->sound.c_str();
	retval->link = value->link.c_str();
	retval->linkType = value->linkType.c_str();
	retval->urgency = value->urgency;
	retval->timeout = value->timeout;
	retval->category = value->category.c_str();
	retval->extra = value->extra.c_str();
	retval->data = value->data.c_str();
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
		NotifyMessageC notificationC;
		copyRefNotifyMessage(&notificationC, &notification);
		onNotify(onNotifyEnv, persistent_id.c_str(), from.c_str(), appName.c_str(), appId.c_str(), sent, &notificationC);
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
			log << severity(3) << "MCS version: " << (int) version << "\n";
			mStream.erase(0, 1);
			sz--;
			state = STATE_TAG;
		}
		else {
			MessageLite *m = NULL;
			enum MCSProtoTag tag;
			sz = nextMessage(&tag, &m, mStream, verbosity, &log);
			if (!m) {
				// That's ok
				continue;
			}
			logMessage(tag, m, verbosity, &log);
			doSmth(tag, m, this);
			delete m;
			count++;
		}
	}
	return count;
}

int MCSClient::read()
{
	if (!ready())
		return ERR_DISCONNECTED;
	unsigned char buffer[16384];
	int r = SSL_read(mSsl, buffer, sizeof(buffer));
	if (r > 0)
	{
		log << severity(3) << "Received " << r << " bytes\n";
		log << severity(3) << hexString(std::string((char *) buffer, r)) << "\n";
		put(buffer, r);
		if (size_t c = process())
		{
			log << severity(3) << "Total: " << c << " message(s)" << "\n\n";
		}
	}
	return r;
}

/**
 * Start client
 * @param retcode can be NULL
 */
void *startClient
(
	int *retcode,
	const std::string &privateKey,
	const std::string &authSecret,
	uint64_t androidId,
	uint64_t securityToken,
	OnNotifyC onNotify,
	void *onNotifyEnv,
	OnLogC onLog,
	void *onLogEnv,
	int verbosity
)
{
	MCSClient *client = new MCSClient(
		privateKey,
		authSecret,
		androidId,
		securityToken,
		onNotify, onNotifyEnv, onLog, onLogEnv,
		verbosity
	);
	int r = client->connect();
	if (retcode)
		*retcode = r;
	return client;
}

/**
 * Stop client
 */
void stopClient
(
	void *client
)
{
	if (client) {
		delete ((MCSClient*)client);
	}
}
