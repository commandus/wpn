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


#include <stdint.h>
#include <curl/curl.h>
#include <google/protobuf/message.h>
#include <google/protobuf/text_format.h>

#include "mcsclient.h"

#include "android_checkin.pb.h"
#include "checkin.pb.h"

static const std::string REGISTER_URL("https://android.clients.google.com/c2dm/register3");
static const std::string CHECKIN_URL("https://android.clients.google.com/checkin");
static const std::string DEF_CHROME_VER("63.0.3234.0");

static const std::string HDR_CONTENT_TYPE("Content-Type: ");


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
	*chrome->mutable_chrome_version() = DEF_CHROME_VER;
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
	chunk = curl_slist_append(chunk, (HDR_CONTENT_TYPE + contentType).c_str());
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
	: mConfig(NULL), mKeys(NULL), mCredentials(NULL)
{

}

MCSClient::MCSClient(
	const WpnConfig *config, 
	const WpnKeys* keys,
	AndroidCredentials *androidCredentials
)
	: mConfig(config), mKeys(keys), mCredentials(androidCredentials)
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

}

MCSClient& MCSClient::operator=(const MCSClient& other)
{

}

bool MCSClient::operator==(const MCSClient& other) const
{

}

int MCSClient::connect()
{
	if (!mConfig)
		return ERR_NO_CONFIG;
	if (!mKeys)
		return ERR_NO_KEYS;
	if (!mCredentials)
		return ERR_NO_CREDS;
	checkIn();
}

int MCSClient::checkIn()
{
	std::string protobuf = mkCheckinRequest(mCredentials->getAndroidId(), mCredentials->getSecurityToken());
	std::string retval;
	int r = curlPost(CHECKIN_URL, "application/x-protobuf", protobuf, &retval);
	if (mConfig->verbosity > 2)
	{
		std::cerr << "Checkin " << CHECKIN_URL 
			<< " return code: " << r 
			<< " value: " << retval << std::endl;
	}
	if (r < 200 || r >= 300)
		return r;
	
	AndroidCheckinResponse resp;
	bool cr = resp.ParseFromString(retval);
	// Whether statistics were recorded properly.
	if (!cr)
		return -r;
	
	return r;
}
