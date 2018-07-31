#include <inttypes.h>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <curl/curl.h>
#include "ece.h"
#include "nlohmann/json.hpp"
#include "utilstring.h"
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
	chunk = curl_slist_append(chunk, ("Content-Type: " + contentType).c_str());
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
 * Make subscription
 * @param subscription return value
 * @param subscribeMode always 1
 * @param wpnKeys reserved
 * @param subscribeUrl URL e.g. https://fcm.googleapis.com/fcm/connect/subscribe
 * @param endPoint https URL e.g. https://sure-phone.firebaseio.com
 * @param serverKey optional key used to send messages
 * @param authorizedEntity usual decimal number string
 * @param token OAuth 2.0 oth other kind token 
 * @param retVal can be NULL
 * @param retHeaders can be NULL
 * @param verbosity default 0- none
 * @return 200-299 - OK (HTTP code), less than 0- fatal error (see ERR_*)
 */
int subscribe2
(
	Subscription &subscription, 
	int subscribeMode, 
	const WpnKeys &wpnKeys, 
	const std::string &subscribeUrl,
	const std::string &endPoint,
	const std::string &authorizedEntity,
	const std::string &serverKey,
	const std::string &token, 
	std::string *retVal,
	std::string *retHeaders,
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

	subscription.setSubscribeUrl(subscribeUrl);
	subscription.setSubscribeMode(subscribeMode);
	subscription.setEndpoint(endPoint);
	subscription.setServerKey(serverKey);
	subscription.setAuthorizedEntity(authorizedEntity);

	switch (subscribeMode) {
		case SUBSCRIBE_FIREBASE:
		{
			std::string key = wpnKeys.getPublicKey();
			std::string auth = wpnKeys.getAuthSecret();
			json j = {
				{"endpoint", endPoint},
				{"encryption_key", key},
				{"encryption_auth", auth},
				{"authorized_entity", authorizedEntity}
			};
			std::string s(j.dump());

			if (verbosity > 2)
				std::cerr << "Send: " << s << " to " << subscribeUrl << std::endl;
			r = curlPost(subscribeUrl, "application/json", s, retHeaders,  retVal, verbosity);
std::cerr << "Headers received: " << retHeaders << std::endl;			
			if (retVal) {
				if (verbosity > 2)
					std::cerr << "Receive response code: "<< r << ", body:" << *retVal << " from " << subscribeUrl << std::endl;
				if (r >= 200 && r < 300)
				{
					json js = json::parse(*retVal);
					subscription.setToken(js["token"]);
					subscription.setPushSet(js["pushSet"]);
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
			break;
		}
		default:
		{
			if (retVal)
				*retVal = "Unsupported mode";
			return ERR_MODE;
		}
	}
	return r;
}

/**
 * Make subscription
 * @param subscription return value
 * @param subscribeMode always 1
 * @param wpnKeys reserved
 * @param subscribeUrl URL e.g. https://fcm.googleapis.com/fcm/connect/subscribe
 * @param endPoint https URL e.g. https://sure-phone.firebaseio.com
 * @param authorizedEntity usual decimal number string
 * @param retVal can be NULL
 * @param retHeaders can be NULL
 * @param verbosity default 0- none
 * @return 200-299 - OK (HTTP code), less than 0- fatal error (see ERR_*)
 */
int subscribe
(
	Subscription &subscription, 
	int subscribeMode, 
	const WpnKeys &wpnKeys, 
	const std::string &subscribeUrl,
	const std::string &endPoint,
	const std::string &authorizedEntity,
	const std::string &serverKey,
	std::string *retVal,
	std::string *retHeaders,
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

	subscription.setSubscribeUrl(subscribeUrl);
	subscription.setSubscribeMode(subscribeMode);
	subscription.setEndpoint(endPoint);
	subscription.setServerKey(serverKey);
	subscription.setAuthorizedEntity(authorizedEntity);

	switch (subscribeMode) {
		case SUBSCRIBE_FIREBASE:
		{
			std::string key = wpnKeys.getPublicKey();
			std::string auth = wpnKeys.getAuthSecret();
			std::string s = 
				"authorized_entity=" + escapeURLString(authorizedEntity)
				+ "&endpoint=" + escapeURLString(endPoint)
				+ "&encryption_key=" + escapeURLString(key)
				+ "&encryption_auth=" + escapeURLString(auth)
			;
			if (verbosity > 2)
				std::cerr << "Send: " << s << " to " << subscribeUrl << std::endl;
			r = curlPost(subscribeUrl, "application/x-www-form-urlencoded", s, retHeaders,  retVal, verbosity);
			if (verbosity > 2)
				std::cerr << "Headers received: " << *retHeaders << std::endl;			
			if (retVal) {
				if (verbosity > 2)
					std::cerr << "Receive response code: "<< r << ", body:" << *retVal << " from " << subscribeUrl << std::endl;
				if (r >= 200 && r < 300)
				{
					json js = json::parse(*retVal);
					subscription.setToken(js["token"]);
					subscription.setPushSet(js["pushSet"]);
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
			break;
		}
		default:
		{
			if (retVal)
				*retVal = "Unsupported mode";
			return ERR_MODE;
		}
	}
	return r;
}
