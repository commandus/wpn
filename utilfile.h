#ifndef UTILFILE_H_
#define UTILFILE_H_	1

#include <string>
#include "wpnapi.h"

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

#endif
