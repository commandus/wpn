#ifndef UTILRECV_H_
#define UTILRECV_H_

#include "utilrecv.h"
#include <sstream>

#include <curl/curl.h>
#include <google/protobuf/message.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>

#include "android_checkin.pb.h"
#include "checkin.pb.h"

#include "utilvapid.h"
#include "utilstring.h"
#include "wp-storage-file.h"

static const std::string DEF_CHROME_VER("63.0.3234.0");

using namespace checkin_proto;

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
  * @brief CURL write callback
  */
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

static const std::string HDR_CONTENT_TYPE("Content-Type: ");

/**
  * @brief CURL write callback
  */
static size_t write_string(void *contents, size_t size, size_t nmemb, void *userp)
{
	if (userp)
		((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

/**
  * POST data, return received data in retval
  * @return 200-299 success, otherwise error code. retval contains error description
  */
int curlPost
(
	std::string *retval,
	std::string *debugHeaders,
	const std::string &url,
	const std::string &contentType,
	const std::string &content,
	const std::string &extraHeader,
	int verbosity
)
{
	CURL *curl = curl_easy_init();
	if (!curl)
		return CURLE_FAILED_INIT;
	CURLcode res;
	struct curl_slist *chunk = NULL;
	chunk = curl_slist_append(chunk, (HDR_CONTENT_TYPE + contentType).c_str());
	if (!extraHeader.empty())
		curl_slist_append(chunk, extraHeader.c_str());
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
	if (verbosity > 3)
	{
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, curl_trace);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

		if (debugHeaders)
			curl_easy_setopt(curl, CURLOPT_HEADERDATA, debugHeaders);
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &write_header);
	}

	res = curl_easy_perform(curl);

	long http_code;
	if (res != CURLE_OK)
	{
		if (retval)
			*retval = curl_easy_strerror(res);
		http_code = -res;
	}
	else
	{
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
	}
	curl_easy_cleanup(curl);
	return http_code;
}

static const std::string CHECKIN_URL("https://android.clients.google.com/checkin");

int checkIn(
	uint64_t *androidId,
	uint64_t *securityToken,
	int verbosity
)
{
	if ((!androidId) || (!securityToken))
		return ERR_WRONG_PARAM;
	std::string protobuf = mkCheckinRequest(*androidId, *securityToken);
	std::string retval;
	int r = curlPost(&retval, NULL, CHECKIN_URL, "application/x-protobuffer", protobuf, "", verbosity);
	if (r < 200 || r >= 300)
		return r;

	AndroidCheckinResponse resp;
	bool cr = resp.ParseFromString(retval);
	// Whether statistics were recorded properly.
	if (!cr)
		return -r;
	*androidId = resp.android_id();
	*securityToken = resp.security_token();
	return r;
}

static const std::string REGISTER_URL("https://android.clients.google.com/c2dm/register3");

static const std::string HDR_AUTHORIZATION("Authorization : ");

static const uint8_t REGISTER_SERVER_KEY[] =	// also default VAPID key
{
	0x04, 0x33, 0x94, 0xf7, 0xdf, 0xa1, 0xeb, 0xb1, 0xdc, 0x03,
	0xa2, 0x5e, 0x15, 0x71, 0xdb, 0x48, 0xd3, 0x2e, 0xed, 0xed,
	0xb2, 0x34, 0xdb, 0xb7, 0x47, 0x3a, 0x0c, 0x8f, 0xc4, 0xcc,
	0xe1, 0x6f, 0x3c, 0x8c, 0x84, 0xdf, 0xab, 0xb6, 0x66, 0x3e,
	0xf2, 0x0c, 0xd4, 0x8b, 0xfe, 0xe3, 0xf9, 0x76, 0x2f, 0x14,
	0x1c, 0x63, 0x08, 0x6a, 0x6f, 0x2d, 0xb1, 0x1a, 0x95, 0xb0,
	0xce, 0x37, 0xc0, 0x9c, 0x6e
};

/**
 * Register device and obtain GCM token
 */
int registerDevice
(
	std::string *retGCMToken,
	uint64_t androidId,
	uint64_t securityToken,
	const std::string &appId,
	int verbosity
)
{
	std::stringstream formData;
	std::string rkb64 = base64UrlEncode(REGISTER_SERVER_KEY, sizeof(REGISTER_SERVER_KEY));
	formData << "app=" << APP_CATEGORY
		<< "&X-subtype=" << escapeURLString(appId)
		<< "&device=" << androidId
		<< "&sender=" << rkb64;
	std::stringstream ss;
	ss << HDR_AUTHORIZATION << "AidLogin "
		<< androidId << ":"
		<< securityToken;
	std::string aidlogin = ss.str();
	std::string retval;
	int r = curlPost(&retval, NULL, REGISTER_URL, "application/x-www-form-urlencoded", formData.str(), aidlogin, verbosity);
	// retval: token=xxx
	if (r < 200 || r >= 300)
		return r;

	std::size_t p = retval.find("=", 0);
	if (p == std::string::npos)
		return ERR_REGISTER_VAL;
	std::string k = retval.substr(0, p);
	std::string v = retval.substr(p + 1);
	if (k == "token")
	{
		if (retGCMToken)
			*retGCMToken = v;
	}
	else
	{
		r = ERR_REGISTER_FAIL;
	}

	return r;
}

std::string tagNmessageToString
(
	uint8_t tag,
	const void *msg
)
{
	std::string s;
	google::protobuf::io::StringOutputStream ss(&s);
	google::protobuf::io::CodedOutputStream *codedOutput = new google::protobuf::io::CodedOutputStream(&ss);
	codedOutput->WriteRaw(&tag, 1);
	int sz = ((google::protobuf::MessageLite*) msg)->ByteSize();
	codedOutput->WriteVarint32(sz);
	((google::protobuf::MessageLite*) msg)->SerializeToCodedStream(codedOutput);
	delete codedOutput;
	return s;
}

#define TRIES	5

/**
 * Calls generateVAPIDKeys() and checkIn()
 * @return from checkIn()
 */
int initClient
(
	std::string &retRegistrationId,
	std::string &privateKey,
	std::string &publicKey,
	std::string &authSecret,
	uint64_t *androidId,
	uint64_t *securityToken,
	std::string &appId,
	int verbosity
)
{
	generateVAPIDKeys(
		privateKey,
		publicKey,
		authSecret
	);
	*androidId = 0;
	*securityToken = 0;
	int r = checkIn(
		androidId,
		securityToken,
		verbosity
	);
	if (r < 200 || r >= 300)
		return r;

	for (int i = 0; i < TRIES; i++)
	{
		r = registerDevice(
			&retRegistrationId,
			*androidId,
			*securityToken,
			appId,
			verbosity
		);
		if (r >= 200 && r < 300)
			break;
	}

	return r;
}

#endif
