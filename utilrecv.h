#ifndef UTILRECV_H
#define UTILRECV_H 1

#include <string>
#include <ece/keys.h>

#define AESGCM 		0
#define AES128GCM	1

// MathFuncsDll.h

#define ERR_WRONG_PARAM				-31
#define ERR_REGISTER_VAL			-32	// Error registering
#define ERR_REGISTER_FAIL			-33

/**
  * POST data, return received data in retval
  * @return 200-299 success, otherwise error code. retval contains error description
  */
int curlPost
(
	std::string *retval,
	std::string *debugHeaders,
	const std::string &url,
	const std::string &contentType,
	const std::string &content,
	const std::string &extraHeader,
	int verbosity
);

/**
  * Obtain device identifer and "password"
  */
int checkIn(
	uint64_t *androidId,
	uint64_t *securityToken,
	int verbosity
);

/**
 * Register device and obtain GCM token
 */
int registerDevice(
	std::string *retGCMToken,
	uint64_t androidId,
	uint64_t securityToken,
	const std::string &appId,
	int verbosity
);

std::string tagNmessageToString
(
	uint8_t tag,
	const void *msg
);

/**
 * Calls generateVAPIDKeys() and checkIn()
 * @return from checkIn()
 */
int initClient
(
	std::string &retRegistrationId,
	std::string &privateKey,
	std::string &publicKey,
	std::string &authSecret,
	uint64_t *androidId,
	uint64_t *securityToken,
	std::string &appId,
	int verbosity
);

#endif
