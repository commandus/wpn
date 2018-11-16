#include "utilfile.h"

#include <sstream>
#include <fstream>
#include "nlohmann/json.hpp"
#include "utilrecv.h"
#include "endpoint.h"

using json = nlohmann::json;

static const std::string KEY_PROVIDER = "provider";
static const std::string PROVIDER_VALUE_FIREFOX = "firefox";
static const std::string PROVIDER_VALUE_CHROME = "chrome";
static const std::string KEY_APPID = "appId";
static const std::string KEY_REGISTRATIONID = "registrationId";
static const std::string KEY_PRIVATEKEY = "privateKey";
static const std::string KEY_PUBLICKEY = "publicKey";
static const std::string KEY_AUTHSECRET = "authSecret";
static const std::string KEY_ANDROIDID = "androidId";
static const std::string KEY_SECURITYTOKEN = "securityToken";

std::string jsonConfig
(
	enum VAPID_PROVIDER provider,
 	const char* registrationIdC,
	const char* privateKeyC,
	const char* publicKeyC,
	const char* authSecretC,
	uint64_t androidId,
	uint64_t securityToken,
	const std::string &appId
)
{
	json json = {
		{ KEY_PROVIDER, provider == PROVIDER_FIREFOX ? PROVIDER_VALUE_FIREFOX : PROVIDER_VALUE_CHROME },
		{ KEY_APPID , appId},
		{ KEY_REGISTRATIONID, registrationIdC},
		{ KEY_PRIVATEKEY, privateKeyC },
		{ KEY_PUBLICKEY, publicKeyC },
		{ KEY_AUTHSECRET, authSecretC },
		{ KEY_ANDROIDID, androidId },
		{ KEY_SECURITYTOKEN, securityToken }
	};
	return json.dump(2);
}

std::string tabConfig
(
	enum VAPID_PROVIDER provider,
 	const char* registrationIdC,
	const char* privateKeyC,
	const char* publicKeyC,
	const char* authSecretC,
	uint64_t androidId,
	uint64_t securityToken,
	const std::string &appId
)
{
	std::stringstream ss;
	std::string e = endpoint(registrationIdC, 0, (int) provider);	///< 0- Chrome, 1- Firefox
	ss << (provider == PROVIDER_FIREFOX ? "firefox" : "chrome") 
	<< "\t" << appId
	<< "\t" << registrationIdC
	<< "\t" << privateKeyC
	<< "\t" << publicKeyC
	<< "\t" << authSecretC
	<< "\t" << androidId
	<< "\t" << securityToken
	<< "\t" << e;
	return ss.str();
}

int writeConfig
(
	const std::string &filename,
	enum VAPID_PROVIDER provider,
 	const char* registrationIdC,
	const char* privateKeyC,
	const char* publicKeyC,
	const char* authSecretC,
	uint64_t androidId,
	uint64_t securityToken,
	const std::string &appId
)
{
	int r = 0;
	std::string d = jsonConfig(
		provider,
		registrationIdC,
		privateKeyC,
		publicKeyC,
		authSecretC,
		androidId,
		securityToken,
		appId
	);
	std::ofstream ostrm(filename);
	try {
		ostrm << d;
	}
	catch (...) {
		r = -1;
	}
	ostrm.close();
	return r;
}

/**
 * Parse config file
 * @return 0- success, -1: Invalid JSON, -2: Invalid config
 */
int parseConfig
(
	const std::string &value,
	enum VAPID_PROVIDER &provider,
 	std::string &registrationId,
	std::string &privateKey,
	std::string &publicKey,
	std::string &authSecret,
	uint64_t &androidId,
	uint64_t &securityToken,
	std::string &appId
)
{
	appId = "";
	json j;
	int r = 0;
	try {
		j = json::parse(value);
	}
	catch (...) {
		r = -1;
	}

	if (r == 0)
	{
		try {
			std::string s;
			if (j.count(KEY_PROVIDER))
				s = j.at(KEY_PROVIDER);
			else
				s = "";
			if (s == PROVIDER_VALUE_FIREFOX)
				provider = PROVIDER_FIREFOX;
			else
				provider = PROVIDER_CHROME;
			if (j.count(KEY_APPID))
				appId = j.at(KEY_APPID);
			else
				appId = "";
			if (j.count(KEY_REGISTRATIONID))
				registrationId = j.at(KEY_REGISTRATIONID);
			else
				registrationId = "";
			if (j.count(KEY_PRIVATEKEY))
				privateKey = j.at(KEY_PRIVATEKEY);
			else
				privateKey = "";
			if (j.count(KEY_PUBLICKEY))
				publicKey = j.at(KEY_PUBLICKEY);
			else
				publicKey = "";
			if (j.count(KEY_PUBLICKEY))
				authSecret = j.at(KEY_AUTHSECRET);
			else
				authSecret = "";
			if (j.count(KEY_ANDROIDID))
				androidId = j.at(KEY_ANDROIDID);
			else
				androidId = 0;
			if (j.count(KEY_SECURITYTOKEN))
				securityToken = j.at(KEY_SECURITYTOKEN);
			else
				securityToken = 0;
		} catch(...) {
			r = -2;
		}
	}
	return r;
}

/**
 * Load config file
 * @return 0- success, -1: Invalid JSON, -2: Invalid config
 */
int readConfig
(
	const std::string &filename,
	enum VAPID_PROVIDER &provider,
 	std::string &registrationId,
	std::string &privateKey,
	std::string &publicKey,
	std::string &authSecret,
	uint64_t &androidId,
	uint64_t &securityToken,
	std::string &appId
)
{
	std::ifstream strm(filename);
	std::string s((std::istreambuf_iterator<char>(strm)), std::istreambuf_iterator<char>());
	strm.close();
	return parseConfig(
		s,
		provider,
 		registrationId,
		privateKey,
		publicKey,
		authSecret,
		androidId,
		securityToken,
		appId
	);
}

static const std::string KEY_TO = "to";
static const std::string KEY_NOTIFICATION = "notification";
static const std::string KEY_TITLE = "title";
static const std::string KEY_BODY = "body";
static const std::string KEY_ICON = "icon";
static const std::string KEY_CLICK_ACTION = "click_action";

std::string mkNotificationJson
(
	const std::string &to,
	const std::string &subject,
	const std::string &body,
	const std::string &icon, 
	const std::string &link 
)
{
	json requestBody = {
		{ KEY_TO, to },
		{ KEY_NOTIFICATION, 
			{
				{ KEY_TITLE, subject },
				{ KEY_BODY, body },
				{ KEY_ICON, icon},
				{ KEY_CLICK_ACTION, link }
			}
		}
	};
	return requestBody.dump();
}

/**
 * Parse notification file
 * @return 0- success, -1: Invalid JSON, -2: Important information missed
 */
int parseNotificationJson
(
	const std::string &value,
	std::string &to,
	std::string &title,
	std::string &body,
	std::string &icon, 
	std::string &click_action
)
{
	json j;
	int r = 0;
	try {
		j = json::parse(value);
	}
	catch (...) {
		r = -1;
	}

	if (r == 0)
	{
		try {
			if (j.count(KEY_TO))
				to = j.at(KEY_TO);
			else
				to = "";
			json n = j.at(KEY_NOTIFICATION);
			if (n.count(KEY_TITLE))
				title = n.at(KEY_TITLE);
			else
				title = "";
			if (n.count(KEY_BODY))
				body = n.at(KEY_BODY);
			else
				body = "";
			if (n.count(KEY_ICON))
				icon = n.at(KEY_ICON);
			else
				icon = "";
			if (n.count(KEY_CLICK_ACTION))
				click_action = n.at(KEY_CLICK_ACTION);
			else
				click_action = "";
		} catch(...) {
			r = -2;
		}
	}
	return r;
}
