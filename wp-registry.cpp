#include <iostream>
#include <sstream>
#include <curl/curl.h>
#include "nlohmann/json.hpp"
#include "platform.h"

#include "endpoint.h"
#include "utilstring.h"
#include "utilrecv.h"
#include "wp-storage-file.h"
#include "wp-registry.h"
#include "wp-subscribe.h"

using json = nlohmann::json;

#define ENDPOINT "https://surephone.commandus.com/wpn-registry/"
#define HEADER_AUTH	"Authorization: U "
#define METHOD_GET "GET"
#define METHOD_POST "POST"
#define METHOD_DELETE "DELETE"
#define PATH_KEY "key"
#define PATH_SUBSCRIPTION "subscription"

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
* Remote call
* @param retval return value
* @param method "POST", "GET", "DELETE"
* @param path "key", "subscription"
* @param value JSON serialized message.  See push2ClientNotificationFCM()
* @return true- success
*/
bool RegistryClient::rpc
(
	std::string *retval,
	const std::string &method,
	const std::string &path,
	uint64_t id,
	const std::string &value,
	bool debug
)
{
	bool c = false;
	CURL *curl = curl_easy_init();
	if (!curl)
		errorCode = - CURLE_FAILED_INIT;
	CURLcode res;
	
	if (debug) {
		curl_easy_setopt(curl, CURLOPT_VERBOSE, true);	
		std::cerr << value << std::endl;
	}

	struct curl_slist *chunk = NULL;
	std::stringstream ss;
	ss << HEADER_AUTH;
	if (config && config->wpnKeys)
		ss << this->config->wpnKeys->id;
	ss << ":";
	if (config && config->wpnKeys)
		ss << this->config->wpnKeys->secret;
	chunk = curl_slist_append(chunk, ss.str().c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
	std::stringstream pp;
	pp << ENDPOINT << path;
	if (id)
		pp << "?id=" << id;

	curl_easy_setopt(curl, CURLOPT_URL, pp.str().c_str());
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, value.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, value.size());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_string);

	if (method != "POST") {
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str());
	}

	std::string r;
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &r);
	res = curl_easy_perform(curl);
	if (res != CURLE_OK)
	{
		errorDescription = curl_easy_strerror(res);
		errorCode = - res;
	}
	else
	{
		errorDescription = "";
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &errorCode);
		if ((errorCode >= 200) && (errorCode < 300))
		{
			c = true;
			if (retval)
				*retval = r;
		}
	}
	curl_slist_free_all(chunk);
	curl_easy_cleanup(curl);
	if (debug)
		std::cerr << r << std::endl;
	return c;
}

RegistryClient::RegistryClient(
	ConfigFile *a_config
)
	: config(a_config), errorCode(0), errorDescription("")
{
	
}

/**
 * Invalidate device registration, check-in and register if needed
 * @param verbosity 0..3
 */
bool RegistryClient::validate(
	int verbosity
)
{
	// check-in and register if needed
	int r = 200;

	// check in
	if (config->androidCredentials->getAndroidId() == 0)
	{
		uint64_t androidId = config->androidCredentials->getAndroidId();
		uint64_t securityToken = config->androidCredentials->getSecurityToken();
		int r = checkIn(&androidId, &securityToken, verbosity);
		if (r < 200 || r >= 300) {
			return false;
		}
		config->androidCredentials->setAndroidId(androidId);
		config->androidCredentials->setSecurityToken(securityToken);
		if (!config->fileName.empty())
			config->save();
	}

	// register
	if (config->androidCredentials->getGCMToken().empty()) {
		for (int i = 0; i < 5; i++) {
			std::string gcmToken;
			r = registerDevice(&gcmToken,
				config->androidCredentials->getAndroidId(),
				config->androidCredentials->getSecurityToken(),
				config->androidCredentials->getAppId(),
				verbosity
			);
			if (r >= 200 && r < 300) {
				config->androidCredentials->setGCMToken(gcmToken);
				if (!config->fileName.empty())
					config->save();
				break;
			}
			sleep(1);
		}
	}
	return (r >= 200 && r < 300);
}

RegistryClient::~RegistryClient()
{

}

bool RegistryClient::add(
	uint64_t *retval
)
{
	bool c = false;
	std::string r;
	bool v = (config && config->clientOptions->getVerbosity() > 0);
	if (rpc(&r, METHOD_POST, PATH_KEY, 0, config->wpnKeys->getPublicKey(), v)) {
		c = true;
		uint64_t v = string2id(r);	// contains trailing "\n"
		config->wpnKeys->id = v;
		if (retval) {
			*retval = v;
		}
	}
	return c;
}
	
bool RegistryClient::get(
	uint64_t id,
	std::string *retVal
) 
{
	bool c = false;
	std::string v;
	bool verbosity = (config && config->clientOptions->getVerbosity() > 0);
	if (rpc(&v, METHOD_GET, PATH_KEY, id, "", verbosity)) {
		c = true;
		v = trim(v);
		config->subscriptions->putSubsciptionVapidPubKey(id, v);
		if (retVal) {
			*retVal = v;
		}
	}
	return c;
}

/**
 * Make subscription at the FCM, save subscription token 
 * in the sentToken
 */
bool RegistryClient::subscribeById(
	uint64_t id
) 
{
	bool c = false;
	Subscription *s = config->subscriptions->getById(id);
	if (!s) {
		errorDescription = "Subscription not found.";
		return c;
	}
	std::string retval;
	std::string retheaders;
	std::string rettoken;
	std::string retpushset;			///< returns pushset. Not implemented. Returns empty string
	std::string subscriptionVAPIDKey = s->getWpnKeys().getPublicKey();
	int verbosity = config->clientOptions->getVerbosity();
	int r = subscribe(&retval, &retheaders, rettoken, retpushset, 
		std::to_string(config->androidCredentials->getAndroidId()),
		std::to_string(config->androidCredentials->getSecurityToken()),
		config->androidCredentials->getAppId(),
		subscriptionVAPIDKey, verbosity
	);
	if ((r >= 200) && (r < 300)) {
		c = true;
		s->setSentToken(rettoken);
		errorCode = 0;
		errorDescription = "";
	} else {
		s->setSentToken("");
		errorCode = r;
		errorDescription = retval + "\n" + retheaders;
	}
	return c;
}

bool RegistryClient::rm()
{

}

bool RegistryClient::addSubscription(
	uint64_t id2
)
{
	Subscription *s = config->subscriptions->getById(id2);
	if (!s)
		return false;
	bool c = false;
	std::string v;
	json js = {
		{ "publicKey", config->wpnKeys->getPublicKey() },
		{ "authSecret", config->wpnKeys->getAuthSecret() },
		{ "token", s->getSentToken() }
	};
	bool verbosity = (config && config->clientOptions->getVerbosity() > 0);
	if (rpc(&v, METHOD_POST, PATH_SUBSCRIPTION, id2, js.dump(), verbosity)) {
		c = true;
	}
	return c;
}

int RegistryClient::getSubscription(
	uint64_t id2
)
{
	Subscription *s = config->subscriptions->getById(id2);
	if (!s)
		return false;
	bool c = false;

	std::string v;
	bool verbosity = (config && config->clientOptions->getVerbosity() > 0);
	if (!rpc(&v, METHOD_GET, PATH_SUBSCRIPTION, id2, "", verbosity)) {
		return false;
	}
	json j;
	try {
		std::stringstream(v) >> j;
	}
	catch (json::exception e) {
		return false;
	}
	catch (...) {
		return false;
	}
	std::string publicKey;
	std::string token;
	std::string authSecret;
	try {
		json::const_iterator f = j.find("publicKey");
		if (f != j.end())
			publicKey = f.value();
		f = j.find("token");
		if (f != j.end()) 
			token = f.value();
		f = j.find("authSecret");
		if (f != j.end())
			authSecret = f.value();
	} catch(...) {
		return false;
	}

	if (publicKey.empty() || token.empty() || authSecret.empty())
		return false;
	s->setToken(token);
	// s->setEndpoint(endpoint(publicKey));
	s->getWpnKeysPtr()->setPublicKey(publicKey);
	s->getWpnKeysPtr()->setAuthSecret(authSecret);
	return true;
}

int RegistryClient::rmSubscription(
	std::string &retval,
	uint64_t id2
)
{
	
}
