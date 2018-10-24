#include <inttypes.h>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <curl/curl.h>
#include "ece.h"
#include "nlohmann/json.hpp"
#include "utilstring.h"
#include "utilvapid.h"
#include "wp-subscribe.h"

using json = nlohmann::json;

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

/**
  * POST data, return received data in retval
  * @return 200-299 success, otherwise error code. retval contains error description
  */
static int curlPost
(
	const std::string &url,
	const std::string &contentType,
	const std::string &authorization,
	const std::string &data,
	const std::string *headers,
	std::string *retval,
	int verbosity
)
{
	CURL *curl = curl_easy_init();
	if (!curl)
		return CURLE_FAILED_INIT; 
	CURLcode res;

	struct curl_slist *chunk = NULL;
	if (!contentType.empty())
		chunk = curl_slist_append(chunk, ("Content-Type: " + contentType).c_str());
	if (!authorization.empty())
		chunk = curl_slist_append(chunk, ("Authorization: AidLogin " + authorization).c_str());

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_string);
	if (verbosity && (headers != NULL)) {
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, headers);
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &write_header);
	}
	if (retval)
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, retval);
    res = curl_easy_perform(curl);
	int http_code;

	if ((verbosity > 2) && (headers != NULL)) 
	{
		std::cerr << *headers << std::endl;
	}

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

/**
 * if vapid public key provided, and it is not default, add:
 * if (DEFAULT_PUBLIC_VAPID_KEY != vapidkey) application_pub_key = base64UrlEncode(vapidkey, sizeof(vapidkey))
 */
static const uint8_t DEFAULT_PUBLIC_VAPID_KEY[] =	// also REGISTER_SERVER_KEY(utilrecv.cpp) also default VAPID key
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
 * Make subscription
 * authorizedEntity MUST BE "103953800507"
 * @see https://firebase.google.com/docs/cloud-messaging/js/client
 * @param retVal can be NULL
 * @param retHeaders can be NULL
 * @param retToken return subscription token
 * @param retPushSet return subscription push set
 * @param receiverAndroidId receiver Android id
 * @param receiverSecurityToken receiver security number
 * @param wpnKeys reserved
 * @param subscribeUrl URL e.g. https://fcm.googleapis.com/fcm/connect/subscribe
 * @param endPoint https URL e.g. https://sure-phone.firebaseio.com
 * @param authorizedEntity usual decimal number string "103953800507"
 * @param verbosity default 0- none
 * @return 200-299 - OK (HTTP code), less than 0- fatal error (see ERR_*)
 */
int subscribeFCM
(
	std::string *retVal,
	std::string *retHeaders,
	std::string &retToken,
	std::string &retPushSet,
	const std::string &receiverAndroidId,
	const std::string &receiverSecurityToken,
	const std::string &subscribeUrl,
	const std::string &endPoint,
	const std::string &authorizedEntity,
	int verbosity
)
{
	int r = 0;

	if (endPoint.empty())
	{
		if (retVal)
			*retVal = "Endpoint is empty";
		return ERR_PARAM_ENDPOINT;
	}
	if (authorizedEntity.empty())
	{
		if (retVal)
			*retVal = "Authorized entity is empty";
		return ERR_PARAM_AUTH_ENTITY;
	}
#ifdef URLENCODED	
	std::string mimetype = "application/x-www-form-urlencoded";
	std::string s = 
		"authorized_entity=" + escapeURLString(authorizedEntity)
		+ "&endpoint=" + escapeURLString(endPoint)
		+ "&encryption_key=" + escapeURLString(receiverAndroidId)
		+ "&encryption_auth=" + escapeURLString(receiverSecurityToken)
	;
#else
	std::string mimetype = "application/json; charset=UTF-8";
	json j = {
		{ "authorized_entity", authorizedEntity },
		{ "endpoint", endPoint },
		{ "encryption_key", receiverAndroidId },
		{ "encryption_auth", receiverSecurityToken }
	};
	std::string s = j.dump();
#endif	
	if (verbosity > 2)
		std::cerr << "Send: " << s << " to " << subscribeUrl << std::endl;
	r = curlPost(subscribeUrl, mimetype, "", s, retHeaders,  retVal, verbosity);
	if (verbosity > 2)
		std::cerr << "Headers received: " << *retHeaders << std::endl;			
	if (retVal) {
		if (verbosity > 2)
			std::cerr << "Receive response code: "<< r << ", body:" << *retVal << " from " << subscribeUrl << std::endl;
		if (r >= 200 && r < 300)
		{
			json js = json::parse(*retVal);
			retToken = js["token"];
			retPushSet = js["pushSet"];
		}
		else
		{
			json js = json::parse(*retVal);
			if (retVal)
			{
				json e = js["error"];
				*retVal = e["message"];
			}
		}
	}
	return r;
}

/**
 * Make subscription
 * authorizedEntity MUST BE "103953800507"
 * @see https://firebase.google.com/docs/cloud-messaging/js/client
 * @param retVal can be NULL
 * @param retHeaders can be NULL
 * @param retToken return subscription token
 * @param retPushSet return subscription push set
 * @param receiverAndroidId receiver Android id
 * @param receiverSecurityToken receiver security number
 * @param wpnKeys reserved
 * @param subscribeUrl URL e.g. https://fcm.googleapis.com/fcm/connect/subscribe
 * @param endPoint https URL e.g. https://sure-phone.firebaseio.com
 * @param authorizedEntity usual decimal number string "103953800507"
 * @param verbosity default 0- none
 * @return 200-299 - OK (HTTP code), less than 0- fatal error (see ERR_*)
 */
int subscribe
(
	std::string *retVal,
	std::string *retHeaders,
	std::string &retToken,
	std::string &retPushSet,
	const std::string &receiverAndroidId,
	const std::string &receiverSecurityToken,
	const std::string &subscribeUrl,
	const std::string &endPoint,
	const std::string &authorizedEntity,
	int verbosity
)
{
	int r = 0;

	if (endPoint.empty())
	{
		if (retVal)
			*retVal = "Endpoint is empty";
		return ERR_PARAM_ENDPOINT;
	}
	if (authorizedEntity.empty())
	{
		if (retVal)
			*retVal = "Authorized entity is empty";
		return ERR_PARAM_AUTH_ENTITY;
	}
	std::string mimetype = "application/x-www-form-urlencoded";

	std::string s =
		"app=org.chrome.linux&X-subtype=wp:" + std::string(endPoint)
		+ "&device=" + escapeURLString(receiverAndroidId)
		+ "&sender=" + escapeURLString(authorizedEntity)
	;
	if (verbosity > 2)
		std::cerr << "Send: " << s << " to " << subscribeUrl << std::endl;
	r = curlPost(subscribeUrl, mimetype, receiverAndroidId + ":" + receiverSecurityToken, s, retHeaders,  retVal, verbosity);
	if (verbosity > 2)
		std::cerr << "Headers received: " << *retHeaders << std::endl;			
	if (retVal) {
		if (verbosity > 2)
			std::cerr << "Receive response code: "<< r << ", body:" << *retVal << " from " << subscribeUrl << std::endl;
		if (r >= 200 && r < 300)
		{
			json js = json::parse(*retVal);
			retToken = js["token"];
			retPushSet = js["pushSet"];
		}
		else
		{
			json js = json::parse(*retVal);
			if (retVal)
			{
				json e = js["error"];
				*retVal = e["message"];
			}
		}
	}
	return r;
}
