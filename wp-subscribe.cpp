#include <inttypes.h>
#include <fstream>
#include <sstream>
#include <curl/curl.h>

#include "wp-subscribe.h"

/**
  * @brief CURL write callback
  */
static size_t write_string(void *contents, size_t size, size_t nmemb, void *userp)
{
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
	const std::string &data,
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

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
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

/**
 * Make subscription
 * @param subscription return value
 * @param subscribeMode always 1
 * @param wpnKeys reserved
 * @param subscribeUrl URL e.g. https://fcm.googleapis.com/fcm/connect/subscribe
 * @param endPoint https URL e.g. https://sure-phone.firebaseio.com
 * @param authorizedEntity usual decimal number string
 * @param retVal can be NULL
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
	std::string *retVal
)
{
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
	
	switch (subscribeMode) {
		case SUBSCRIBE_FIREBASE:
		{
			std::stringstream q;
			q << "{\"endpoint\": \""
				<< endPoint << "\","
				<< "\"encryption_key\": \""
				<< wpnKeys.getPublicKey() << "\","
				<< "\"encryption_auth\": \""
				<< wpnKeys.getAuthSecret() << "\","
				<< "\"authorized_entity\": \""
				<< authorizedEntity << "\""
				<< "}";
			curlPost(subscribeUrl, "application/json", q.str(), retVal);
			break;
		}
		default:
		{
			if (retVal)
				*retVal = "Unsupported mode";
			return ERR_MODE;
		}
	}
}