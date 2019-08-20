#include "utilfile.h"

#include <sstream>
#include <fstream>
#include "utiljson.h"
#include "utilrecv.h"
#include "endpoint.h"

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
static const std::string KEY_LASTPESISTENTID = "lastPersistentId";

std::string tabConfig
(
	enum VAPID_PROVIDER provider,
 	const char* registrationIdC,
	const char* privateKeyC,
	const char* publicKeyC,
	const char* authSecretC,
	uint64_t androidId,
	uint64_t securityToken,
	const std::string &appId,
	const std::string &lastPersistentId
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
	<< "\t" << e
	<< "\t" << lastPersistentId;
	return ss.str();
}
