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

#include "utiljson.h"
#include "utilvapid.h"
#include "errlist.h"

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
* Push serialized JSON string to device FCM
* @param retval return error description if not null
* @param value JSON serialized message.  See push2ClientNotificationFCM()
* @return 200-299- success, negative- error code
* @see push2ClientNotificationFCM()
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

	long http_code;

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
			if (jsValid(r)) {
				if (retval)
					*retval = r;
			} else {
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

/**
* Push notification to device
* @return 200-299- success, negative- error code
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
	const std::string &data,
 	int verbosity
)
{
	std::string request = jsClientNotification(client_token, title, body, icon, click_action, data);
	return push2ClientJSON(retval, server_key, client_token, request, verbosity);
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
* @return 200-299- success, nefative- error code
* @see push2ClientJSON()
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
	std::string requestBody = jsClientCommand(client_token, command, persistent_id, code, output, server_key, token);
	return push2ClientJSON(retval, server_key, client_token, requestBody, verbosity);
}
