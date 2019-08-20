#include <iostream>
#include <sstream>
#include <curl/curl.h>
#include "utiljson.h"
#include "platform.h"

#include "endpoint.h"
#include "utilstring.h"
#include "utilrecv.h"
#include "wp-storage-file.h"
#include "wp-registry.h"
#include "wp-subscribe.h"

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

static size_t write_header(char* buffer, size_t size, size_t nitems, void *userp) {
	size_t sz = size * nitems;
	((std::string*)userp)->append((char*)buffer, sz);
    return sz;
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
	const std::string &value
)
{
	int verbosity = config ? config->clientOptions->getVerbosity() : 0;
	bool c = false;
	CURL *curl = curl_easy_init();
	if (!curl)
		errorCode = - CURLE_FAILED_INIT;
	CURLcode res;

	std::string headers;
	if (verbosity > 2) {
		curl_easy_setopt(curl, CURLOPT_VERBOSE, true);	
		std::cerr << value << std::endl;
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, &headers);
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &write_header);
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

	if (verbosity > 2) 
	{
		std::cerr << headers << std::endl;
	}

	if (res != CURLE_OK)
	{
		errorDescription = curl_easy_strerror(res);
		errorCode = - res;
	}
	else
	{
		errorDescription = "";
		long errc;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &errc);
		errorCode = errc;
		if ((errorCode >= 200) && (errorCode < 300))
		{
			c = true;
			if (retval)
				*retval = r;
		}
	}
	curl_slist_free_all(chunk);
	curl_easy_cleanup(curl);
	if (verbosity > 0)
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
)
{
	// check-in and register if needed
	int r = 200;
	int verbosity = config ? config->clientOptions->getVerbosity() : 0;
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

/**
 * @param retval Return identifier number
 * @return identifier number
 */
bool RegistryClient::add(
	uint64_t *retval
)
{
	bool c = false;
	std::string r;
	if (rpc(&r, METHOD_POST, PATH_KEY, 0, config->wpnKeys->getPublicKey())) {
		c = true;
		uint64_t id = string2id(r);	// contains trailing "\n"
		config->wpnKeys->id = id;
		if (retval) {
			*retval = id;
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
	if (rpc(&v, METHOD_GET, PATH_KEY, id, "")) {
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
	int verbosity = config ? config->clientOptions->getVerbosity() : 0;
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
	// TODO test server implementation
	uint64_t id = config->wpnKeys->id;
	if (!id)
		return false;
	bool c = false;
	std::string v;
	bool verbosity = (config && config->clientOptions->getVerbosity() > 0);
	if (rpc(&v, METHOD_DELETE, PATH_SUBSCRIPTION, id, "")) {
		if (rpc(&v, METHOD_DELETE, PATH_KEY, id, "")) {
			c = true;
		}
	}
	return c;
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

	std::string json = jsAddSubscription(config->wpnKeys->getPublicKey(), config->wpnKeys->getAuthSecret(), s->getSentToken());
	bool verbosity = (config && config->clientOptions->getVerbosity() > 0);
	if (rpc(&v, METHOD_POST, PATH_SUBSCRIPTION, id2, json)) {
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
	if (!rpc(&v, METHOD_GET, PATH_SUBSCRIPTION, id2, "")) {
		return false;
	}
	std::string publicKey;
	std::string token;
	std::string authSecret;
	if (!jsGetSubscription(v, publicKey, token, authSecret))
		return false;
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
	// TODO test server impementation
	Subscription *s = config->subscriptions->getById(id2);
	if (!s)
		return false;
	bool c = false;
	bool verbosity = (config && config->clientOptions->getVerbosity() > 0);
	if (rpc(&retval, METHOD_DELETE, PATH_SUBSCRIPTION, id2, s->toJson())) {
		c = true;
	}
	return c;
}
