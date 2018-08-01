#include <openssl/ossl_typ.h>
#include <openssl/ossl_typ.h>
#include <openssl/ossl_typ.h>
#include <openssl/ossl_typ.h>
#include <iostream>
#include <sstream>
#include "wp-push.h"

#include <ece.h>
#include <ece/keys.h>
#include <curl/curl.h>

#include "nlohmann/json.hpp"

#include "utilvapid.h"

using json = nlohmann::json;

#define FCM_SEND "https://fcm.googleapis.com/fcm/send"

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
* Push raw JSON to device FCM
* @return 200-299- success, <0- error
*/
int push2ClientJSON
(
	std::string *retval,
	const std::string &server_key,
	const std::string &client_token,
	const std::string &value
)
{
	if (server_key.empty())
		return ERR_PARAM_SERVER_KEY;
	if (client_token.empty())
		return ERR_PARAM_CLIENT_TOKEN;
	CURL *curl = curl_easy_init();
	if (!curl)
		return CURLE_FAILED_INIT; 
	CURLcode res;
	
	struct curl_slist *chunk = NULL;
	chunk = curl_slist_append(chunk, ("Content-Type: application/json"));
	chunk = curl_slist_append(chunk, ("Authorization: key=" + server_key).c_str());

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

	curl_easy_setopt(curl, CURLOPT_URL, FCM_SEND);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, value.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, value.size());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_string);
	std::string r;
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &r);
// std::cerr << "Send to " << FCM_SEND << ": " << value<< std::endl;		
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
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
		if ((http_code >= 200) && (http_code < 300))
		{
			try 
			{
				json response = json::parse(r);
				if (retval)
					*retval = r;
			}
			catch(...) 
			{
				if (retval)
					*retval = "Parse error of: "  + r;
				if (client_token.empty())
					return ERR_PARSE_RESPONSE;	
			}
		}
		else
		{
			// Error
			if (retval)
				*retval = r;
		}
	}
	curl_easy_cleanup(curl);
	return http_code;
}

std::string mkJWTHeader
(
	const std::string &aud,
	const std::string &sub,
	const std::string &privateKey
)
{
	// Builds a signed Vapid token to include in the `Authorization` header. 
	uint8_t pk[ECE_WEBPUSH_PRIVATE_KEY_LENGTH];
	ece_base64url_decode(privateKey.c_str(), privateKey.size(), ECE_BASE64URL_REJECT_PADDING, pk, ECE_WEBPUSH_PRIVATE_KEY_LENGTH);
	EC_KEY *key = ece_import_private_key(pk, ECE_WEBPUSH_PRIVATE_KEY_LENGTH);
	time_t exp = time(NULL) + 60 * 60 * 12;
	return vapid_build_token(key, aud, exp, sub);
}

/**
* Push raw JSON to device VAPID
* @param aud e.g. "http://acme.com" 
* @param sub e.g. "mailto: wile@acme.com" 
* @return 200-299- success, <0- error
*/
int push2ClientJSON_VAPID
(
	std::string *retval,
	const std::string &privateKey,
	const std::string &publicKey,
	const std::string aud,
    const std::string sub,
	const std::string &client_token,
	const std::string &value
)
{
	if (privateKey.empty())
		return ERR_PARAM_SERVER_KEY;
	if (publicKey.empty())
		return ERR_PARAM_SERVER_KEY;
	if (client_token.empty())
		return ERR_PARAM_CLIENT_TOKEN;
	CURL *curl = curl_easy_init();
	if (!curl)
		return CURLE_FAILED_INIT; 
	CURLcode res;
	
	struct curl_slist *chunk = NULL;
	chunk = curl_slist_append(chunk, ("Content-Type: application/json"));
	chunk = curl_slist_append(chunk, ("Authorization: WebPush " 
		+ mkJWTHeader(aud, sub, privateKey)).c_str());
	chunk = curl_slist_append(chunk, ("Crypto-Key: p256ecdsa=" 
		+ publicKey).c_str());

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

	curl_easy_setopt(curl, CURLOPT_URL, FCM_SEND);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, value.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, value.size());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_string);
	std::string r;
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &r);
	// std::cerr << "Send to " << FCM_SEND << ": " << value<< std::endl;		
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
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
		if ((http_code >= 200) && (http_code < 300))
		{
			try 
			{
				json response = json::parse(r);
				if (retval)
					*retval = r;
			}
			catch(...) 
			{
				if (retval)
					*retval = "Parse error of: "  + r;
				if (client_token.empty())
					return ERR_PARSE_RESPONSE;	
			}
		}
		else
		{
			// Error
			if (retval)
				*retval = r;
		}
	}
	curl_easy_cleanup(curl);
	return http_code;
}

/**
* Push notification to device
* @return 200-299- success, <0- error
*/
int push2ClientNotificationFCM
(
	std::string *retval,
	const std::string &server_key,
	const std::string &client_token,
	const std::string &title,
	const std::string &body,
	const std::string &icon,
	const std::string &click_action
)
{
	json requestBody = {
		{"to", client_token},
		{"notification", 
			{
				{"title", title},
				{"body", body},
				{"icon", icon},
				{"click_action", click_action}
			}
		}
	};
	return push2ClientJSON(retval, server_key, client_token, requestBody.dump());
}

/**
* Push notification to device
* @return 200-299- success, <0- error
*/
int push2ClientNotificationVAPID
(
	std::string *retval,
	const std::string &endpoint,
	const std::string &privateKey,
	const std::string &publicKey,
	const std::string &aud, 
	const std::string &sub,
	const std::string &title,
	const std::string &body,
	const std::string &icon,
	const std::string &click_action
)
{
	json requestBody = {
		{"to", endpoint},
		{"notification", 
			{
				{"title", title},
				{"body", body},
				{"icon", icon},
				{"click_action", click_action}
			}
		}
	};
	return push2ClientJSON_VAPID(retval, privateKey, publicKey, 
		aud, sub, endpoint, requestBody.dump());
}

/**
* Push "command output" to device
* @param server_key FCM subscription server key
* @param token FCM recipient token
* @param client_token FCM token
* @param persistent_id reference to request
* @param command command line
* @param code execution return code, usually 0
* @param output result from stdout
* @return 200-299- success, <0- error
*/
int push2ClientDataFCM
(
	std::string *retval,
	const std::string &server_key,
	const std::string &token,
	const std::string &client_token,
	const std::string &persistent_id,
	const std::string &command,
	int code,
	const std::string &output
)
{
	json requestBody = {
		{"to", client_token},
		{"data", 
			{
				{"command", command},
				{"persistent_id", persistent_id},
				{"code", code},
				{"output", output},
				{"serverKey", server_key},
				{"token", token}
			}
		}
	};
	return push2ClientJSON(retval, server_key, client_token, requestBody.dump());
}

/**
* Push "command output" to device
* @param endpoint endpoint
* @param privateKey PK
* @param publicKey public key
* @param authSecret secret
* @param persistent_id reference to request. If empty, it is request, otherwise response
* @param command command line
* @param code execution return code, usually 0
* @param output result from stdout
* @return 200-299- success, <0- error
*/
int push2ClientDataVAPID
(
	std::string *retval,
	const std::string &endpoint,
	const std::string &privateKey,
	const std::string &publicKey,
	const std::string &authSecret,
	const std::string &persistent_id,
	const std::string &command,
	int code,
	const std::string &output
)
{
	json requestBody = {
		{"to", endpoint},
		{"data", 
			{
				{"command", command},
				{"persistent_id", persistent_id},
				{"code", code},
				{"output", output},
				{"serverKey", privateKey},
				{"token", authSecret}
			}
		}
	};
	return push2ClientJSON(retval, authSecret, authSecret, requestBody.dump());
}
