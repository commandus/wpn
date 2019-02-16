#ifndef UTILFILE_H_
#define UTILFILE_H_	1

#include <string>
#include "wpnapi.h"

std::string jsonConfig
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
);

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
);

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
	const std::string &appId,
	const std::string &lastPersistentId
);

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
	std::string &appId,
	std::string &lastPersistentId
);

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
	std::string &appId,
	std::string &lastPersistentId
);

#endif
