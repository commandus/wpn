#include <inttypes.h>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <curl/curl.h>
#include "nlohmann/json.hpp"
#include <ece.h>
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

static const std::string HDR_AUTHORIZATION("Authorization : ");

std::string encodeBase64Uint64(uint64_t value)
{
	int len = ece_base64url_encode(&value, sizeof(value), ECE_BASE64URL_OMIT_PADDING, NULL, 0);
	std::string an64(len, '\0');
	ece_base64url_encode(&value, sizeof(value), ECE_BASE64URL_OMIT_PADDING, (char *) an64.c_str(), len);
	return an64;
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
	uint64_t androidId,
	uint64_t securityToken,
	int verbosity
)
{
	CURL *curl = curl_easy_init();
	if (!curl)
		return CURLE_FAILED_INIT; 
	CURLcode res;
	
	
	struct curl_slist *chunk = NULL;
	chunk = curl_slist_append(chunk, ("Content-Type: " + contentType).c_str());
/*
	std::stringstream ss;
	ss << "Authorization: Bearer eJpuAioExgY:APA91bEehiEOf2UfagQGk9hJGBQOgUAkbXWQAu2NJAETofuCcB8mYhL5jDKD_qrcDcR3kAPUJ3YBOH9r8GddFXYDo4q3Ze7XG6KBx4mKp7tdSPDlbIJ6w_sh6RpDvDJKyg7flTiEg47i";
	// ss << HDR_AUTHORIZATION << "AidLogin " << androidId << ":" << securityToken;
	curl_slist_append(chunk, ss.str().c_str());
*/
	std::string s;
	std::string an;
	an = "246829423295:AIzaSyBfUt1N5aabh8pubYiBPKOq9OcIoHv_41I";
	// an = "MjQ2ODI5NDIzMjk1:AIzaSyBfUt1N5aabh8pubYiBPKOq9OcIoHv_41I";
	int len = ece_base64url_encode(an.c_str(), an.size(), ECE_BASE64URL_OMIT_PADDING, NULL, 0);
	std::string an64(len, '\0');
	ece_base64url_encode(an.c_str(), an.size(), ECE_BASE64URL_OMIT_PADDING, (char *) an64.c_str(), len);
	
	std::stringstream ss;
	
	ss << "Authorization: Bearer " << an;
	curl_slist_append(chunk, ss.str().c_str());
	
std::cerr << "============>" << ss.str().c_str() << std::endl;

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

	if (verbosity && (headers != NULL)) {
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
 * @param authorizedEntity usual decimal number string
 * @param retVal can be NULL
 * @param retHeaders can be NULL
 * @param androidId number
 * @param securityToken number
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
	std::string *retVal,
	std::string *retHeaders,
 	uint64_t androidId,
	uint64_t securityToken,
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
	subscription.setAuthorizedEntity(authorizedEntity);

	switch (subscribeMode) {
		case SUBSCRIBE_FIREBASE:
		{
			std::string key = wpnKeys.getPublicKey();
			std::string auth = wpnKeys.getAuthSecret();
/*			
std::cerr 
	<< "key before: " << std::endl
	<< key << std::endl;
	key.erase(std::remove(key.begin(), key.end(), '='), key.end());
	std::replace(key.begin(), key.end(), '+', '-');
	std::replace(key.begin(), key.end(), '/', '_');

std::cerr 	
	<< "key after: " << std::endl
	<< key << std::endl;

std::cerr 
	<< "auth before: " << std::endl
	<< auth << std::endl;
	auth.erase(std::remove(auth.begin(), auth.end(), '='), auth.end());
	std::replace(auth.begin(), auth.end(), '+', '-');
	std::replace(auth.begin(), auth.end(), '/', '_');

std::cerr 	
	<< "auth after: " << std::endl
	<< auth << std::endl;

	
	*/	
			json j = {
				{"endpoint", endPoint},
				{"encryption_key", key},
				{"encryption_auth", auth},
				{"authorized_entity", authorizedEntity}
			};
			std::string s(j.dump());

			if (verbosity > 2)
				std::cerr << "Send: " << s << " to " << subscribeUrl << std::endl;
			signupNewUser
			r = curlPost(subscribeUrl, "application/json", s, retHeaders,  retVal, 
						 androidId, securityToken, verbosity);
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
