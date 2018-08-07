#include <openssl/ossl_typ.h>
#include <openssl/ossl_typ.h>
#include <openssl/ossl_typ.h>
#include <openssl/ossl_typ.h>
#include <openssl/bio.h>
#include <openssl/pem.h>

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

static size_t write_header(char* buffer, size_t size, size_t nitems, void *userp) {
	size_t sz = size * nitems;
	((std::string*)userp)->append((char*)buffer, sz);
    return sz;
}

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
		std::cerr << typ << ": " << std::endl;
	std::string s(data, size);
	std::cerr << s << std::endl;
	return 0;
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
	const std::string &value,
	int verbosity
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
	std::string headers;
	if (verbosity) {
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, &headers);
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &write_header);
	}

	std::string r;
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &r);
	if (verbosity > 2) 
	{
		std::cerr << "Send to " << FCM_SEND << ": " << value<< std::endl;		
	}
	res = curl_easy_perform(curl);

	if (verbosity > 2) 
	{
		std::cerr << headers << std::endl;
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

#define FORMAT_PEM	(5 | 1)

static void logPEMForKey(EC_KEY *key) {
    const EC_GROUP *group = EC_KEY_get0_group(key);
	BIO *out = BIO_new_fp(stdout, BIO_NOCLOSE);
	// auto out = bio_open_owner("kays.pem", FORMAT_PEM, 1);
    PEM_write_bio_ECPKParameters(out, group);
    PEM_write_bio_EC_PUBKEY(out, key);
    PEM_write_bio_ECPrivateKey(out, key, NULL, NULL, 0, NULL, NULL);
}

static std::string mkJWTHeader
(
	const std::string &aud,
	const std::string &sub,
	const std::string &privateKey,
	time_t exp
)
{
	// Builds a signed Vapid token to include in the `Authorization` header. 
	uint8_t pk[ECE_WEBPUSH_PRIVATE_KEY_LENGTH];
	ece_base64url_decode(privateKey.c_str(), privateKey.size(), ECE_BASE64URL_REJECT_PADDING, pk, ECE_WEBPUSH_PRIVATE_KEY_LENGTH);
	EC_KEY *key = ece_import_private_key(pk, ECE_WEBPUSH_PRIVATE_KEY_LENGTH);

	std::string r = vapid_build_token(key, aud, exp, sub);
	// logPEMForKey(key);
	EC_KEY_free(key);
	std::cerr << "mkJWTHeader" 
	<< " pk: " << privateKey
	<< " aud: " << aud
	<< " exp: " << exp
	<< " sub: " << sub
	<< std::endl
	<< " JWT: " << r
	<< std::endl;
	
	return r;
}

/**
* Push raw JSON to device VAPID
* @param aud e.g. "http://acme.com" 
* @param sub e.g. "mailto: wile@acme.com" 
* @return 200-299- success, <0- error
*/
static int push2ClientJSON_VAPID
(
	std::string *retval,
	const std::string &endpoint,
	const std::string &privateKey,
	const std::string &publicKey,
	const std::string &sub,
	const std::string &client_token,
	const std::string &value,
	int verbosity
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

	std::string aud = extractURLProtoAddress(endpoint);
	/*
	std::string jwt1 = mkJWTHeader("https://updates.push.services.mozilla.com", 
		"mailto:andrei.i.ivanov@gmail.com", 
		"_93Jy3cT0SRuUA1B9-D8X_zfszukGUMjIcO5y44rqCk",
		1533621648
	);
	std::cerr << std::endl << "JWT-1" << std::endl
	<< jwt1
	<< std::endl;
	*/
// const publicKey = 'BM9Czc7rYYOinc7x_ALzqFgPSXV497qg76W6csYRtCFzjaFHGyuzP2a08l1vykEV1lgq6P83BOhB9xp-H5wCr1A';

	time_t exp = time(NULL) + (60 * 60 * 24);
	std::string jwt = mkJWTHeader(aud, sub, privateKey, exp);
	
	struct curl_slist *chunk = NULL;
	chunk = curl_slist_append(chunk, ("Content-Type: application/json"));
	chunk = curl_slist_append(chunk, ("Authorization: WebPush " + jwt).c_str());
	chunk = curl_slist_append(chunk, ("Crypto-Key: p256ecdsa=" + publicKey).c_str());
	/*
	chunk = curl_slist_append(chunk, "Content-Type: application/octet-stream");
	chunk = curl_slist_append(chunk, "Content-Encoding: aes128gcm");
	chunk = curl_slist_append(chunk, ("Authorization: vapid t=" + jwt + ", k=" + publicKey).c_str());
	*/

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

	// curl_easy_setopt(curl, CURLOPT_URL, FCM_SEND);
	curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, value.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, value.size());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_string);
	
	std::string headers;
	if (verbosity > 2) {
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, &headers);
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &write_header);
	}
	if (verbosity > 3)
	{
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, curl_trace);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	}

	std::string r;
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &r);
	// std::cerr << "Send to " << FCM_SEND << ": " << value<< std::endl;
	res = curl_easy_perform(curl);

	if (verbosity > 2) 
	{
		std::cerr << headers << std::endl;
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
	const std::string &click_action,
 	int verbosity
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
	return push2ClientJSON(retval, server_key, client_token, requestBody.dump(), verbosity);
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
	const std::string &sub,
	const std::string &title,
	const std::string &body,
	const std::string &icon,
	const std::string &click_action,
	int verbosity
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
	return push2ClientJSON_VAPID(retval, endpoint, privateKey, publicKey, 
		sub, endpoint, requestBody.dump(), verbosity);
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
	const std::string &output,
	int verbosity
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
	return push2ClientJSON(retval, server_key, client_token, requestBody.dump(), verbosity);
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
	const std::string &output,
	int verbosity
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
	return push2ClientJSON(retval, authSecret, authSecret, requestBody.dump(), verbosity);
}
