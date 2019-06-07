#include <iostream>
#include <sstream>
#include <curl/curl.h>
#include "nlohmann/json.hpp"

#include "utilstring.h"
#include "utilstring.h"
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
	
	if (debug)
		curl_easy_setopt(curl, CURLOPT_VERBOSE, true);	

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
			if (debug) {
				std::cerr << r << std::endl;
			}
			if (retval)
				*retval = r;
		}
	}
	curl_slist_free_all(chunk);
	curl_easy_cleanup(curl);
	return c;
}

RegistryClient::RegistryClient(
	ConfigFile *a_config
)
	: config(a_config), errorCode(0), errorDescription("")
{
	
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
	if (rpc(&r, METHOD_POST, PATH_KEY, 0, config->wpnKeys->getPublicKey(), false)) {
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
	if (rpc(&v, METHOD_GET, PATH_KEY, id, "", true)) {
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
 * Make subscription
 */
bool RegistryClient::subscribeById(
	uint64_t id
) 
{
	bool c = false;
	Subscription *s = config->subscriptions->getById(id);
	if (!s)
		return false;
	std::string retval;
	std::string retheaders;
	std::string rettoken;			///< returns subscription token
	std::string retpushset;		///< returns pushset. Not implemented. Returns empty string
	int r = subscribe(&retval, &retheaders, rettoken, retpushset, 
		std::to_string(config->androidCredentials->getAndroidId()),
		std::to_string(config->androidCredentials->getSecurityToken()),
		config->androidCredentials->getAppId(),
		s->getWpnKeys().getPublicKey(), 3
	);
	if ((r >= 200) && (r < 300)) {
		c = true;
		errorCode = 0;
		errorDescription = "";
		s->setToken(rettoken);
		s->setPushSet(retpushset);
	} else {
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
	std::stringstream js;
	js << s->toJson();
	if (rpc(&v, METHOD_POST, PATH_SUBSCRIPTION, id2, js.str(), true)) {
		c = true;
	}
	return c;
}

int RegistryClient::getSubscription(
	std::string &retval,
	uint64_t id2
)
{
	// std::stringstream strm(r);
	// retVal->fromStream(id, strm);
}

int RegistryClient::rmSubscription(
	std::string &retval,
	uint64_t id2
)
{
	
}
