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
	kNumProtoTypes          = 16
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

//----------------------------- MCSReceiveBuffer -----------------------------

MCSReceiveBuffer::MCSReceiveBuffer()
	: mVersion(0), state(STATE_VERSION), buffer("")
{
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

MessageLite *MCSReceiveBuffer::createMessage(int tag)
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
		uint32_t tag;
		bool r = codedInput.ReadVarint32(&tag);	// 1 byte long
		if (!r)
			break;
		uint32_t tagSize;
		r = codedInput.ReadVarint32(&tagSize);
		if (!r)
			break;
		google::protobuf::io::CodedInputStream::Limit limit = codedInput.PushLimit(tagSize);
		MessageLite *message = createMessage(tag);
		if (message)
		{
			r = message->ParsePartialFromCodedStream(&codedInput);
			if (!r)
				break;
			r = codedInput.ConsumedEntireMessage();
		}
		else
		{
			r = codedInput.Skip(tagSize);
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

/**
 * @param androidId 0- before register
 * @param securityToken 0- before register
 */
static std::string mkLoginRequest
(
	uint64_t androidId,
	uint64_t securityToken,
	const std::string &gcmSecurityToken,
	const std::vector<std::string> &persistentIds
)
{
	LoginRequest req;
	req.set_adaptive_heartbeat(false);
	req.set_auth_service(LoginRequest_AuthService_ANDROID_ID);
	req.set_auth_token(gcmSecurityToken);
	req.set_id("chrome-" + DEF_CHROME_VER);
	req.set_domain("mcs.android.com");

	std::string said = std::to_string(androidId);
	req.set_device_id("android-" + said);
	req.set_network_type(1);
	req.set_resource(said);
	req.set_user(said);
	req.set_use_rmq2(true);
	auto sc = req.mutable_setting();
	Setting *s = sc->Add();
	*s->mutable_name() = "new_vc";
	*s->mutable_value() = "1";
	for (std::vector<std::string>::const_iterator it(persistentIds.begin()); it != persistentIds.end(); ++it)
	{
		*req.add_received_persistent_id() = *it;
	}

	std::string r;
	req.SerializeToString(&r);
	return r;
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

int readLoop(MCSClient *client, MCSReceiveBuffer *strm, SSL *ssl, bool &stop)
{
	unsigned char buffer[4096];
	client->log(0, 1, "Start");
	while (!stop)
	{
		int r = SSL_read(ssl, buffer, sizeof(buffer));
		if (r > 0) 
		{
			std::cerr << "Received " << r << " bytes:" << std::endl << hexString(std::string((char *) buffer, r)) << std::endl;
			strm->put(buffer, r);
			if (size_t c = strm->process())
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
	if (mCredentials->getFCMToken().empty())
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
	
	mListenerThread = new std::thread(readLoop, 
		this,
		std::ref(mStream), std::ref(mSsl), std::ref(mStop)
	);
	mListenerThread->detach();
	r = logIn();
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

int MCSClient::registerDevice()
{
	std::string retval;
	std::stringstream formData;
	formData << "app=org.chromium.linux&X-subtype=" << getAppId() 
		<< "&device=" << mCredentials->getAndroidId()
		<< "&sender=" << mKeys->getPublicKey();

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
		mCredentials->setFCMToken(v);
		if (mConfig->verbosity > 1)
			std::cerr << "FCM token: " << v << std::endl;
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

	std::string gcmToken = mCredentials->getFCMToken();
	std::string protobuf = mkLoginRequest(androidId, securityToken, gcmToken, mPersistentIds);
	if (mConfig->verbosity > 2)
	{
		std::cerr << "Login to " << MCS_HOST << std::endl;
	}

	send(kLoginRequestTag, protobuf);
	int r = 0;
	return r;
}

int MCSClient::send
(
	uint8_t tag,
	const std::string &protobuf
)
{
	std::stringstream ss;
	ss << kMCSVersion << tag << protobuf;
	std::string r = ss.str();
	return SSL_write(mSsl, r.c_str(), r.size());
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
	}
}
