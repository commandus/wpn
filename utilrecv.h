#ifndef UTILRECV_H
#define UTILRECV_H 1

#include <string>
#include <ece/keys.h>

#define AESGCM 		0
#define AES128GCM	1

// MathFuncsDll.h
#ifdef _MSC_VER
#ifdef EXPORT_C_DLL
#define EXPORTDLL extern "C" __declspec(dllexport) 
#else
#define EXPORTDLL
#endif
#else
#define EXPORTDLL
#endif

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
EXPORTDLL int checkIn(
	uint64_t *androidId,
	uint64_t *securityToken,
	int verbosity
);

/**
 * Register device and obtain GCM token
 */
EXPORTDLL int registerDevice(
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
 * VAPID Endpoint
 * @see https://github.com/web-push-libs/webpush-java/wiki/Endpoints
 */
EXPORTDLL std::string endpoint(
	const std::string &registrationId,			///< GCMToken
	const int browser = 0						///< 0- Chrome, 1- Firefox
);

#endif
