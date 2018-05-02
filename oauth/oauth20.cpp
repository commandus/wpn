#include <iostream>
#include <curl/curl.h>
#include "oauth20.h"
#include "oauthrequest.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

static const std::string ENDPOINT_IDENTITY("https://www.googleapis.com/identitytoolkit/v3/relyingparty/");
static const std::string ENDPOINT_ANONYMOUS(ENDPOINT_IDENTITY + "signupNewUser");

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
	std::string &retval,
	const std::string &url,
	const std::string &data
)
{
	CURL *curl = curl_easy_init();
	if (!curl)
		return CURLE_FAILED_INIT; 
	CURLcode res;

	struct curl_slist *chunk = NULL;
	chunk = curl_slist_append(chunk, "Content-Type: application/json");
	chunk = curl_slist_append(chunk, "Accept: application/json");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_string);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &retval);
    res = curl_easy_perform(curl);
	int http_code;

    if (res != CURLE_OK)
	{
		retval = curl_easy_strerror(res);
		http_code = - res;
	}
	else
	{
		curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
	}
	curl_easy_cleanup(curl);
	return http_code;
}

const std::string &OAuth20Credentials::getBearerToken() const
{
	return tokenPair.idToken;
}

OAuth20Credentials::OAuth20Credentials
(
	enum AUTH_PROVIDER provider, 
	const std::string &projectKey,
	const std::string &identifier,
	const std::string &continueUri
)
{
	this->provider = provider;
	this->key = projectKey;
	this->identifier= identifier;
	this->continueUri = continueUri;
}

OAuth20Credentials::~OAuth20Credentials()
{
}

std::string OAuth20Credentials::endpoint()
{
	switch (provider) {
		case AUTH_EMAIL:
			break;
		case AUTH_PHONE:
			break;
		case AUTH_GOOGLE:
			break;
		case AUTH_PLAY:
			break;
		case AUTH_FACEBOOK:
			break;
		case AUTH_TWITTER:
			break;
		case AUTH_GITHUB:
			break;
		default:
			// AUTH_ANONYMOUS
			return ENDPOINT_ANONYMOUS + "?key=" + key; 
	}
	return "";
}
int OAuth20Credentials::authenticate()
{
	switch (provider) {
		case AUTH_EMAIL:
			break;
		case AUTH_PHONE:
			break;
		case AUTH_GOOGLE:
			break;
		case AUTH_PLAY:
			break;
		case AUTH_FACEBOOK:
			break;
		case AUTH_TWITTER:
			break;
		case AUTH_GITHUB:
			break;
		default:
			// AUTH_ANONYMOUS
			{
				OAuthRequest req(provider, identifier, "google.com", continueUri, "https://accounts.google.com/");
				std::string q = req.toString();
				std::string resp;
				int r = curlPost(resp, endpoint(), q);
				if (r >= 200 && r < 300)
				{
					// parse response
					if (!tokenPair.parse(provider, resp))
					{
						return ERR_PARSE_IDENTITY_RESPONSE;
					}
				}
				else
				{
					std::cerr << "Error authenticate user " << r << ": " << resp << std::endl;
				}
			}
	}
	return 200;
}

std::string OAuth20Credentials::toString()
{
	json r = {
		{ "provider", provider },
		{ "tokenPair", tokenPair.toJson() },
		{ "key", key},
		{ "identifier", identifier},
		{ "continueUri", continueUri}
	};
	return r.dump();
}
