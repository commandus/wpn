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
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#include <functional>
#include <stdint.h>
#include <unistd.h>
#include <sstream>
#include <curl/curl.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>

#include <ece.h>

#include "mcsclient.h"

#include "utilstring.h"
#include "sslfactory.h"
#include "android_checkin.pb.h"
#include "checkin.pb.h"
#include "mcs.pb.h"

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
	kNumProtoTypes          = 16,
	kDontKnow				= 17,
	kHeaders				= 18
};

using namespace checkin_proto;
using namespace mcs_proto;
using namespace google::protobuf::io;

#define MCS_PORT		443

static const std::string REGISTER_URL("https://android.clients.google.com/c2dm/register3");
static const std::string CHECKIN_URL("https://android.clients.google.com/checkin");
static const std::string MCS_HOST("mtalk.google.com");
static const std::string DEF_CHROME_VER("63.0.3234.0");

static const u_int8_t kMCSVersion = 41;

static const std::string HDR_CONTENT_TYPE("Content-Type: ");
static const std::string HDR_AUTHORIZATION("Authorization : ");


static const char *IQTYPE_NAMES[] = 
{
	"GET", "SET", "RESULT", "IQ_ERROR"
};

//----------------------------- MCSReceiveBuffer -----------------------------

MCSReceiveBuffer::MCSReceiveBuffer()
	: mVersion(0), state(STATE_VERSION), buffer("")
{
}

void MCSReceiveBuffer::setClient(MCSClient *client)
{
	mClient = client;
}

int MCSReceiveBuffer::process()
{
	int sz = buffer.size();
	int count = 0;
	while (sz > 0)
	{
		if (state == STATE_VERSION)
		{
			mVersion = (uint8_t) buffer[0];
			std::cerr << "MCS version: " << (int) mVersion << std::endl;
			buffer.erase(0, 1);
			sz--;
			state = STATE_TAG;
		} else {
			sz = parse();
			if (sz <= 0)
				break;
			count++;
		}
	}
	return count;
}

MessageLite *MCSReceiveBuffer::createMessage(uint8_t tag)
{
	MessageLite *r = NULL;
	switch(tag)
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

int MCSReceiveBuffer::parse()
{
	std::stringstream ss(buffer);
	IstreamInputStream rawInput(&ss);
	CodedInputStream codedInput(&rawInput);
	int c = 0;
	int sz = 0;
	while (true)
	{
		// tag number, size, message
		uint8_t tag;
		bool r = codedInput.ReadRaw(&tag, 1);	// 1 byte long
		if (!r)
			break;
		uint32_t msgSize;
		r = codedInput.ReadVarint32(&msgSize);
		if (!r)
			break;

		
		std::cerr << "<<<  Tag " << (int) tag << " size " << msgSize << "  >>>" << std::endl;

		if (tag == kHeaders)
		{
			std::cerr << "Headers size " << msgSize << std::endl;
			std::string h(msgSize, '\0'); 
			codedInput.ReadRaw((void *) h.c_str(), msgSize);
			std::cerr << h << std::endl;
			continue;
		}

		google::protobuf::io::CodedInputStream::Limit limit = codedInput.PushLimit(msgSize);
		MessageLite *message = createMessage(tag);
		if (message)
		{
			r = message->ParsePartialFromCodedStream(&codedInput);
			if (!r)
				break;
			r = codedInput.ConsumedEntireMessage();
			std::string d;
			message->SerializeToString(&d);
			std::cerr << "Tag: " << (int) tag << " size: " << msgSize << ": " << hexString(d) << std::endl;
			switch (tag)
			{
				case kLoginResponseTag:
				{
					LoginResponse* r = (LoginResponse*) message;
					std::cerr << "Login " << r->id() << " ";
					if (r->has_error())
					{
						std::cerr << "error " << r->error().code();
						if (r->error().has_message())
						{
							std::cerr << ": " << r->error().message();
						}
						std::cerr << " ";
					}
					for (int i = 0; i < r->setting_size(); i++)
					{
						std::cerr << "setting " << i << ":" << r->setting(i).name() << r->setting(i).value() << std::endl;
					}
					if (r->has_server_timestamp())
					{
						time_t t = r->server_timestamp()/1000;
						struct tm *tm = localtime(&t);
						std::cerr << "server time " <<  std::asctime(tm) << std::endl;
					}
					
					if (r->has_heartbeat_config())
					{
						std::cerr << " has heart beat config";
					}
					std::cerr << std::endl;
				}
					break;
				case kHeartbeatAckTag:
				{
					HeartbeatAck* r = (HeartbeatAck*) message;
					std::cerr << "HeartbeatAck " ;
					if (r->has_last_stream_id_received())
					{
						std::cerr << " last_stream_id_received: " << r->last_stream_id_received() << " ";
					}
					if (r->has_status())
					{
						std::cerr << " status: " << r->status() << " ";
					}
					if (r->has_stream_id())
					{
						std::cerr << " stream_id: " << r->stream_id() << " ";
					}
					std::cerr << std::endl;
				}
					break;
				case kBindAccountResponseTag:
					{
					}
					break;
				case kIqStanzaTag:
				{
					IqStanza* r = (IqStanza*) message;
					std::cerr << "IqStanza " << IQTYPE_NAMES[r->type()] << " " << r->id() << " ";
					if (r->has_rmq_id())
						std::cerr << " rmq_id: " << r->rmq_id();
					if (r->has_from())
						std::cerr << " from: " << r->from();
					if (r->has_to())
						std::cerr << " to: " << r->to();
					if (r->has_error())
					{
						std::cerr << " error: ";
						if (r->error().has_code())
							std::cerr << " code: " << r->error().code();
						if (r->error().has_extension())
						{
							std::cerr << " extension: ";
							if (r->error().extension().has_id())
								std::cerr << " id: " << r->error().extension().id();
							if (r->error().extension().has_data())
								std::cerr << " data: " << r->error().extension().data();
						}
					}
					if (r->has_extension())
					{
						std::cerr << " extension: ";
						if (r->error().extension().has_id())
							std::cerr << " id: " << r->error().extension().id();
						if (r->error().extension().has_data())
							std::cerr << " data " << r->error().extension().data();
					}
					if (r->has_persistent_id())
						std::cerr << " persistent_id: " << r->persistent_id();
					if (r->has_stream_id())
						std::cerr << " stream_id: " << r->stream_id();
					if (r->has_last_stream_id_received())
						std::cerr << " last_stream_id_received: " << r->last_stream_id_received();
					if (r->has_account_id())
						std::cerr << " account_id: " << r->account_id();
  					if (r->has_status())
						std::cerr << " status: " << r->status();
					std::cerr << std::endl;
				}
					break;
				case kDataMessageStanzaTag:
				{
					DataMessageStanza* r = (DataMessageStanza*) message;
					std::string cryptoKeyHeader;
					std::string encryptionHeader;
					std::cerr << "DataMessageStanza ";
					for (int a = 0; a < r->app_data_size(); a++)
					{
						std::cerr << " app_data key: " << r->app_data(a).key() 
						<< " data: " << r->app_data(a).value() << std::endl;
						if (r->app_data(a).key() == "crypto-key")
							cryptoKeyHeader = r->app_data(a).value();
						if (r->app_data(a).key() == "encryption")
							encryptionHeader = r->app_data(a).value();
					}
					if (r->has_persistent_id())
						std::cerr << " persistent_id: " << r->persistent_id();
					if (r->has_id())
						std::cerr << " id: " << r->id();
					if (r->has_category())
						std::cerr << " category: " << r->category();
					if (r->has_device_user_id())
						std::cerr << " device_user_id: " << r->device_user_id();
					if (r->has_from())
						std::cerr << " from: " << r->from();
					if (r->has_from_trusted_server())
						std::cerr << " from_trusted_server: " << r->from_trusted_server();
					if (r->has_immediate_ack())
						std::cerr << " immediate_ack: " << r->immediate_ack();
					if (r->has_last_stream_id_received())
						std::cerr << " last_stream_id_received: " << r->last_stream_id_received();
					if (r->has_queued())
						std::cerr << " queued: " << r->queued();
					if (r->has_raw_data())
					{
						std::cerr << " raw_data: " << hexString(r->raw_data());
						if (mClient)
						{
							std::string d = mClient->decode(r->raw_data(), cryptoKeyHeader, encryptionHeader);
							std::cerr << " data: " << d;
						}
					}
					if (r->has_reg_id())
						std::cerr << " reg_id: " << r->reg_id();
					if (r->has_sent())
						std::cerr << " sent: " << r->sent();
					if (r->has_status())
						std::cerr << " status: " << r->status();
					if (r->has_stream_id())
						std::cerr << " stream_id: " << r->stream_id();
					if (r->has_to())
						std::cerr << " to: " << r->to();
					if (r->has_token())
						std::cerr << " token: " << r->token();
					if (r->has_ttl())
						std::cerr << " ttl: " << r->ttl();
					std::cerr << std::endl;
				}
					break;
				default:
					break;
			}
		}
		else
		{
			r = codedInput.Skip(msgSize);
		}
		sz = codedInput.CurrentPosition();
		codedInput.ConsumedEntireMessage();
		codedInput.PopLimit(limit);
	}
	if (sz)
	{
		buffer.erase(0, sz);
	}
	return c;
}

void MCSReceiveBuffer::put(const void *buf, int size)
{
	buffer.append(std::string((char*) buf, size));
}

uint8_t MCSReceiveBuffer::getVersion()
{
	return mVersion;
}

//----------------------------- MCSClient helpers -----------------------------

/**
 * @param androidId 0- before register
 * @param securityToken 0- before register
 */
static std::string mkCheckinRequest(
	uint64_t androidId, 
	uint64_t securityToken
)
{
	AndroidCheckinRequest req;
	req.set_user_serial_number(0);
	req.set_version(3);
	if (androidId)
		req.set_id(androidId);
	if (securityToken)
		req.set_security_token(securityToken);
	AndroidCheckinProto* checkIn = req.mutable_checkin();
	ChromeBuildProto *chrome = checkIn->mutable_chrome_build();
	checkIn->set_type(DEVICE_CHROME_BROWSER);
	chrome->set_platform(ChromeBuildProto_Platform_PLATFORM_LINUX);
	*chrome->mutable_chrome_version() = DEF_CHROME_VER;
	chrome->set_channel(ChromeBuildProto_Channel_CHANNEL_DEV);
	
	std::string r;
	req.SerializeToString(&r);
	return r;
}

static MessageLite * mkPing()
{
	HeartbeatPing *req = new HeartbeatPing();
	// req->set_status(0);
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

/**
  * @brief CURL write callback
  */
static size_t write_string(void *contents, size_t size, size_t nmemb, void *userp)
{
	if (userp)
		((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

static size_t write_header(char* buffer, size_t size, size_t nitems, void *userp) {
	size_t sz = size * nitems;
	((std::string*)userp)->append((char*)buffer, sz);
	return sz;
}

static const char *CURL_TYP_NAMES[7] = {
	"INFO",
	"HEADER IN",
	"HEADER OUT",
	"DATA IN",
	"DATA OUT",
	"SSL DATA IN",
	"SSL DATA OUT"
};

static int curl_trace
(
	CURL *handle, 
	curl_infotype typ,
	char *data, 
	size_t size,
	void *userp
)
{
	if (typ >= 0 && typ < 7)
		std::cerr << CURL_TYP_NAMES[typ] << ": " << std::endl;
	std::string s(data, size);
	if (typ > 2)
		s = hexString(s);
	std::cerr << s << std::endl;
	return 0;
}

//----------------------------- MCSClient -----------------------------

/**
  * POST data, return received data in retval
  * @return 200-299 success, otherwise error code. retval contains error description
  */
int MCSClient::curlPost
(
	const std::string &url,
	const std::string &contentType,
	const std::string &content,
	std::string *retval
)
{
	CURL *curl = curl_easy_init();
	if (!curl)
		return CURLE_FAILED_INIT; 
	CURLcode res;
	
	
	struct curl_slist *chunk = NULL;
	chunk = curl_slist_append(chunk, (HDR_CONTENT_TYPE + contentType).c_str());
	std::stringstream ss;
	ss << HDR_AUTHORIZATION << "AidLogin " << mCredentials->getAndroidId() << ":" << mCredentials->getSecurityToken();
	if (hasIdNToken())
		curl_slist_append(chunk, ss.str().c_str());
	
	curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, content.size());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, content.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_string);
	if (retval)
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, retval);

	std::string debugHeaders;
	
	if (mConfig->verbosity > 2) {
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, curl_trace);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, &debugHeaders);
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &write_header);
	}

    res = curl_easy_perform(curl);

	if (mConfig->verbosity > 2) {
		std::cerr << debugHeaders << std::endl;
	}

	int http_code;

    if (res != CURLE_OK)
	{
		if (retval)
			*retval = curl_easy_strerror(res);
		http_code = - res;
	}
	else
	{
		curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
	}
	curl_easy_cleanup(curl);
	return http_code;
}

MCSClient::MCSClient()
	: mStream(&mBuffer), mConfig(NULL), mKeys(NULL), mCredentials(NULL)
{
	mStream->setClient(this);
}

MCSClient::MCSClient(
	const WpnConfig *config, 
	const WpnKeys* keys,
	AndroidCredentials *androidCredentials
)
	: mStream(&mBuffer), mConfig(config), mKeys(keys), mCredentials(androidCredentials)
{
}

void MCSClient::setConfig(const WpnConfig *config)
{
	mConfig = config;
}

void MCSClient::setKeys(const WpnKeys* keys)
{
	mKeys = keys;
}

MCSClient::MCSClient(const MCSClient& other)
{
	mConfig = other.mConfig;
}

MCSClient::~MCSClient()
{
	stop();
}

int readLoop(MCSClient *client)
{
	unsigned char buffer[4096];
	client->log(0, 1, "Start");
	int r = client->logIn();
	while (!client->mStop)
	{
		int r = SSL_read(client->mSsl, buffer, sizeof(buffer));
		if (r > 0) 
		{
			std::cerr << "Received " << r << " bytes: " << std::endl << std::endl
				<< std::string((char *) buffer, r) << std::endl << std::endl
				<< hexString(std::string((char *) buffer, r)) << std::endl << std::endl;
			client->mStream->put(buffer, r);
			if (size_t c = client->mStream->process())
			{
				std::cerr << "Processed " << c << " messages" << std::endl;
			}
		}
		sleep(0);
	}
	client->log(0, 1, "End");
	return 0;
}

int MCSClient::connect()
{
	if (!mConfig)
		return ERR_NO_CONFIG;
	if (!mKeys)
		return ERR_NO_KEYS;
	if (!mCredentials)
		return ERR_NO_CREDS;
	int r;
	if (mCredentials->getAndroidId() == 0)
	{
		r = checkIn();
		if (r < 200 || r >= 300)
		{
			return ERR_NO_ANDROID_ID_N_TOKEN;
		}
	}
	if (mCredentials->getGCMToken().empty())
	{
		for (int i = 0; i < 5; i++) 
		{
			r = registerDevice();
			if (r >= 200 && r < 300)
			{
				break;
			}
			sleep(1);
		}
		if (r < 200 || r >= 300)
		{
			return ERR_NO_FCM_TOKEN;
		}
	}
	
	mSsl = mSSLFactory.open(&mSocket, MCS_HOST, MCS_PORT);
	if (!mSsl)
		return ERR_NO_CONNECT;
	mStop = false;
	
	if (mConfig->verbosity > 1)
		std::cerr << "Android id: " << mCredentials->getAndroidId() 
			<< " security token: " << mCredentials->getSecurityToken() << std::endl;
	mListenerThread = new std::thread(readLoop, this);
	mListenerThread->detach();
	return 0;
}

void MCSClient::stop()
{
	mStop = true;
	if (mListenerThread)
	{
		mListenerThread = NULL;
	}
}

bool MCSClient::hasIdNToken()
{
	if (!mCredentials)
		return false;
	return (mCredentials->getAndroidId() && mCredentials->getSecurityToken());
}

int MCSClient::checkIn()
{
	if (!mCredentials)
		return ERR_NO_CREDS;
	uint64_t androidId = mCredentials->getAndroidId();
	uint64_t securityToken = mCredentials->getSecurityToken();
	std::string protobuf = mkCheckinRequest(androidId, securityToken);
	if (mConfig->verbosity > 2)
	{
		std::cerr << "Send to " << CHECKIN_URL 
			<< " checkin: " << std::endl
			<< hexString(protobuf) << std::endl;
	}

	std::string retval;
	int r = curlPost(CHECKIN_URL, "application/x-protobuffer", protobuf, &retval);
	if (mConfig->verbosity > 1)
	{
		std::cerr << "Check in " << CHECKIN_URL 
			<< " android id: " << mCredentials->getAndroidId()
			<< " app id: " << mCredentials->getAppId()
			<< " security token: " << mCredentials->getSecurityToken()
			<< " security token: " << mCredentials->getSecurityToken() 
			<< " return code: " << r 
			<< " value: " << std::endl << std::endl << retval << std::endl;
	}
	if (r < 200 || r >= 300)
		return r;
	
	AndroidCheckinResponse resp;
	bool cr = resp.ParseFromString(retval);
	// Whether statistics were recorded properly.
	if (!cr)
		return -r;
	
	if (mCredentials)
	{
		uint64_t androidId = resp.android_id();
		uint64_t securityToken = resp.security_token();
		mCredentials->setAndroidId(androidId);
		mCredentials->setSecurityToken(securityToken);
		if (androidId && securityToken)
		{
			if (mConfig->verbosity > 2)
			{
				std::cerr << "Credentials assigned, android id:  " << androidId 
					<< ", security token: " << securityToken << std::endl;
			}
		}
	}
	return r;
}

std::string MCSClient::getAppId()
{
	std::stringstream r;
	r << "wp:com.commandus.wpn#" << mCredentials->getAppId();
	return r.str();
}

const uint8_t REGISTER_SERVER_KEY[] = 
{
	0x04,
	0x33,
	0x94,
	0xf7,
	0xdf,
	0xa1,
	0xeb,
	0xb1,
	0xdc,
	0x03,
	0xa2,
	0x5e,
	0x15,
	0x71,
	0xdb,
	0x48,
	0xd3,
	0x2e,
	0xed,
	0xed,
	0xb2,
	0x34,
	0xdb,
	0xb7,
	0x47,
	0x3a,
	0x0c,
	0x8f,
	0xc4,
	0xcc,
	0xe1,
	0x6f,
	0x3c,
	0x8c,
	0x84,
	0xdf,
	0xab,
	0xb6,
	0x66,
	0x3e,
	0xf2,
	0x0c,
	0xd4,
	0x8b,
	0xfe,
	0xe3,
	0xf9,
	0x76,
	0x2f,
	0x14,
	0x1c,
	0x63,
	0x08,
	0x6a,
	0x6f,
	0x2d,
	0xb1,
	0x1a,
	0x95,
	0xb0,
	0xce,
	0x37,
	0xc0,
	0x9c,
	0x6e
};

static std::string base64encode
(
	const void *source,
	size_t size
)
{
	size_t requiredSize =  ece_base64url_encode(source, size, ECE_BASE64URL_OMIT_PADDING, NULL, 0);
	std::string r(requiredSize, '\0');
	ece_base64url_encode(source, size, ECE_BASE64URL_OMIT_PADDING, (char *) r.c_str(), r.size());
	return r;
}

/**
 * obtain GCM token
 */
int MCSClient::registerDevice()
{
	std::string retval;
	std::stringstream formData;
	std::string rkb64 = base64encode(REGISTER_SERVER_KEY, sizeof(REGISTER_SERVER_KEY));
	formData << "app=org.chromium.linux" 
		<< "&X-subtype=" << escapeURLString(getAppId()) 
		<< "&device=" << mCredentials->getAndroidId()
		<< "&sender=" << rkb64;

	int r = curlPost(REGISTER_URL, "application/x-www-form-urlencoded", formData.str(), &retval);

	// retval: token=xxx

	if (mConfig->verbosity > 2)
	{
		std::cerr << "Register " << REGISTER_URL 
			<< " return code: " << r 
			<< " value: " << retval << std::endl;
	}
	if (r < 200 || r >= 300)
		return r;

	std::size_t p = retval.find("=", 0);
	if (p == std::string::npos)
		return ERR_REGISTER_VAL;
	std::string k = retval.substr(0, p);
	std::string v = retval.substr(p + 1);
	if (k == "token") 
	{
		mCredentials->setGCMToken(v);
		if (mConfig->verbosity > 1)
			std::cerr << "GCM token: " << v << std::endl;
	}
	else
	{
		if (mConfig->verbosity > 0)
		{
			std::cerr << k << ": " << v << std::endl;
		}
		r = ERR_REGISTER_FAIL;
	}
	return r;
}

void MCSClient::log
(
	int level, 
	int tag, 
	const std::string &message
)
{
	std::cerr << ">" << message << std::endl;
}

int MCSClient::logIn()
{
	if (!mCredentials)
		return ERR_NO_CREDS;
	if (!mSsl)
		return ERR_NO_CONNECT;
	uint64_t androidId = mCredentials->getAndroidId();
	uint64_t securityToken = mCredentials->getSecurityToken();
	if (mConfig->verbosity > 2)
	{
		std::cerr << "Login to " << MCS_HOST << std::endl;
	}

	MessageLite *l =  mkLoginRequest(androidId, securityToken, mPersistentIds);
	if (!l)
		return ERR_MEM;

	sendVersion();

	sendTag(kLoginRequestTag, l);

	delete l;
	int r = 0;
	return r;
}

int MCSClient::sendVersion()
{
	std::stringstream ss;
	ss << kMCSVersion;
	std::string r = ss.str();
	std::cerr << "Send version: " << hexString(r) << std::endl;
	return SSL_write(mSsl, r.c_str(), r.size());
}

int MCSClient::sendTag
(
	uint8_t tag,
	const MessageLite *msg
)
{
	std::stringstream ss;
	OstreamOutputStream *rawOutput = new OstreamOutputStream(&ss);
	CodedOutputStream *codedOutput = new CodedOutputStream(rawOutput);
	codedOutput->WriteRaw(&tag, 1);
	int sz = msg->ByteSize();
	codedOutput->WriteVarint32(sz);
	msg->SerializeToCodedStream(codedOutput);
	delete codedOutput;
	delete rawOutput;
	std::string s(ss.str());
	std::cerr << "Send tag: " << (int) tag << " (" << sz << " bytes) : " << hexString(s) << std::endl;
	return SSL_write(mSsl, s.c_str(), s.size());
}

void MCSClient::ping()
{
	std::cerr << "ping.." << std::endl;
	MessageLite *l =  mkPing();
	if (!l)
		return;
	sendTag(kHeartbeatPingTag, l);
	delete l;
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
		if (s == "q")
			break;
		if (s == "p")
		{
			ping();
		}
	}
}

/**
 * Decode string
 * @see https://tools.ietf.org/html/draft-ietf-webpush-encryption-03
 */
std::string MCSClient::decode
(
	const std::string &source,
	const std::string &cryptoKeyHeader,
	const std::string &encryptionHeader
)
{
	uint32_t rs = 0;
	uint8_t salt[ECE_SALT_LENGTH];
	uint8_t rawSenderPubKey[ECE_WEBPUSH_PUBLIC_KEY_LENGTH];
	int err = ece_webpush_aesgcm_headers_extract_params(cryptoKeyHeader.c_str(), encryptionHeader.c_str(),
		salt, ECE_SALT_LENGTH, rawSenderPubKey, ECE_WEBPUSH_PUBLIC_KEY_LENGTH, &rs);
	if (err != ECE_OK)
		return "";
	size_t outSize = ece_aes128gcm_plaintext_max_length((const uint8_t*) source.c_str(), source.size());
	if (outSize > 0)
	{
		std::string r(outSize, '\0');
		ece_webpush_aesgcm_decrypt(
			mKeys->getPrivateKeyArray(), ECE_WEBPUSH_PRIVATE_KEY_LENGTH,
			mKeys->getAuthSecretArray(), ECE_WEBPUSH_AUTH_SECRET_LENGTH,
			salt, ECE_SALT_LENGTH,
			rawSenderPubKey, ECE_WEBPUSH_PUBLIC_KEY_LENGTH,
			rs,
			(const uint8_t *) source.c_str(), source.size(), 
			(uint8_t *) r.c_str(), &outSize);
		r.resize(outSize);
		return r;
	}
	return "";
}
