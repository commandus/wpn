#include "utilfile.h"

#include <sstream>
#include <fstream>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

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
		{ "provider", provider == PROVIDER_FIREFOX ? "firefox" : "chrome" },
		{ "appId", appId},
		{ "registrationId", registrationIdC},
		{ "privateKey", privateKeyC },
		{ "publicKey", publicKeyC },
		{ "authSecret", authSecretC },
		{ "androidId", androidId },
		{ "securityToken", securityToken }
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
	char endpoint[255];
	endpointC(endpoint, sizeof(endpoint), registrationIdC, 0, (int) provider);	///< 0- Chrome, 1- Firefox
	ss << (provider == PROVIDER_FIREFOX ? "firefox" : "chrome") 
	<< "\t" << appId
	<< "\t" << registrationIdC
	<< "\t" << privateKeyC
	<< "\t" << publicKeyC
	<< "\t" << authSecretC
	<< "\t" << androidId
	<< "\t" << securityToken
	<< "\t" << endpoint;
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
 * load config file
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
	appId = "";
	
	std::ifstream strm(filename);
	json j;
	int r = 0;
	try {
		strm >> j;
	}
	catch (...) {
		r = -1;
	}
	strm.close();

	if (r == 0)
	{
		try {
			std::string  s = j["provider"];;
			if (s == "firefox")
				provider = PROVIDER_FIREFOX;
			else
				provider = PROVIDER_CHROME;
			appId = j["appId"];
			registrationId = j["registrationId"];
			privateKey = j["privateKey"];
			publicKey = j["publicKey"];
			authSecret = j["authSecret"];
			androidId = j["androidId"];
			securityToken = j["securityToken"];
		} catch(...) {
			r = -2;
		}
	}
	return r;
}

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
		{"to", to },
		{"notification", 
			{
				{"title", subject },
				{"body", body },
				{"icon", icon},
				{"click_action", link }
			}
		}
	};
	return requestBody.dump();
}
