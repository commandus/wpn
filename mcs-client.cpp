#include <stddef.h>
#include <set>
#include <iostream>
#include <utility>
#include <stdint.h>
#include <curl/curl.h>
#include <google/protobuf/message.h>
#include <google/protobuf/text_format.h>

#include "mcs-client.h"
#include "android_checkin.pb.h"
#include "checkin.pb.h"

static const std::string REGISTER_URL("https://android.clients.google.com/c2dm/register3");
static const std::string CHECKIN_URL("https://android.clients.google.com/checkin");

using namespace checkin_proto;

static std::string mkCheckinRequest(
	uint64_t androidId, 
	uint64_t securityToken
)
{
	AndroidCheckinRequest req;
	req.set_user_serial_number(0);
	req.set_version(3);
	req.set_id(androidId);
	req.set_security_token(securityToken);
	AndroidCheckinProto* checkIn = req.mutable_checkin();
	ChromeBuildProto *chrome = checkIn->mutable_chrome_build();
	checkIn->set_type(DEVICE_CHROME_BROWSER);
	chrome->set_platform(ChromeBuildProto_Platform_PLATFORM_LINUX);
	*chrome->mutable_chrome_version() = "63.0.3234.0";
	chrome->set_channel(ChromeBuildProto_Channel_CHANNEL_DEV);
	
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

/**
  * POST data, return received data in retval
  * @return 200-299 success, otherwise error code. retval contains error description
  */
static int curlPost
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
	chunk = curl_slist_append(chunk, ("Content-Type: " + contentType).c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, content.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_string);
	if (retval)
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, retval);
    res = curl_easy_perform(curl);
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
{
}

MCSClient::~MCSClient()
{
	
}

int MCSClient::checkIn(
	uint64_t androidId, 
	uint64_t securityToken
)
{
	std::string protobuf = mkCheckinRequest(androidId, securityToken);
	std::string retval;
	int r = curlPost(CHECKIN_URL, "application/x-protobuf", protobuf, &retval);
std::cerr << "Code: " << r	<< "\nRet:\n" << retval << std::endl;
	if (r < 200 || r >= 300)
		return r;
	
	AndroidCheckinResponse resp;
	bool cr = resp.ParseFromString(retval);
	// Whether statistics were recorded properly.
	if (!cr)
		return -r;
	
	return r;
}

