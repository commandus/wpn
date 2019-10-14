#ifndef WPNAPI_H_
#define WPNAPI_H_	1

#ifdef __cplusplus
extern "C" {
#endif
	
#include <ece/keys.h>
#include "wpn-notify.h"

#define AESGCM 		0
#define AES128GCM	1

#ifdef _MSC_VER
#ifdef EXPORT_C_DLL
#define EXPORTDLL extern "C" __declspec(dllexport)
#else
#define EXPORTDLL
#endif
#else
#define EXPORTDLL
#endif

enum VAPID_PROVIDER {
	PROVIDER_CHROME = 0,
	PROVIDER_FIREFOX = 1
};

struct ClientConfig {
	enum VAPID_PROVIDER provider;
	std::string registrationId;
	std::string privateKey;
	std::string publicKey;
	std::string authSecret;
	uint64_t androidId;
	uint64_t securityToken;
	std::string appId;
	std::string lastPersistentId;
};

struct NotificationData {
	std::string to;
	std::string title;
	std::string body;
	std::string icon; 
	std::string click_action;
};

/**
 * Helper function for testing
 * Print out "curl ..."  command line string
 * @param retval return string
 * @param retvalsize can be 0
 * @param publicKey e.g. "BM9Czc7rYYOinc7x_ALzqFgPSXV497qg76W6csYRtCFzjaFHGyuzP2a08l1vykEV1lgq6P83BOhB9xp-H5wCr1A";
 * @param privateKey e.g. "_93..";
 * @param filename	temporary file keeping AES GCM ciphered data
 * @param endpoint recipient endpoint
 * @param p256dh recipient key 
 * @param auth recipient key auth 
 * @param body JSON string message
 * @param contact mailto:
 * @param contentEncoding AESGCM or AES128GCM
 * @return required buffer size
 */
EXPORTDLL size_t webpushVapidCmdC(
	char* retval,
	size_t retvalsize,
	const char*publicKey,
	const char*privateKey,
	const char*filename,
	const char*endpoint,
	const char*p256dh,
	const char*auth,
	const char*body,
	const char*contact,
	int contentEncoding,
	time_t expiration = 0
);

/**
 * Send VAPID web push using CURL library
 * @param reuseCurl may be NULL 
 * @param retval return string
 * @param retvalsize can be 0
 * @param publicKey e.g. "BM9Czc7rYYOinc7x_ALzqFgPSXV497qg76W6csYRtCFzjaFHGyuzP2a08l1vykEV1lgq6P83BOhB9xp-H5wCr1A";
 * @param privateKey e.g. "_93..";
 * @param endpoint recipient endpoint
 * @param p256dh recipient key 
 * @param auth recipient key auth 
 * @param body JSON string message
 * @param contact mailto:
 * @param contentEncoding AESGCM or AES128GCM
 * @return >0- HTTP code, <0- error code
 */
EXPORTDLL int webpushVapidC(
	void *reuseCurl,
	char* retval,
	size_t retvalsize,
	const char*publicKey,
	const char*privateKey,
	const char*endpoint,
	const char*p256dh,
	const char*auth,
	const char*body,
	const char*contact,
	int contentEncoding,
	time_t expiration = 0
);

/**
 * Push "command output" to device
 * @param retval return string
 * @param retvalsize can be 0
 * @param publicKey e.g. "BM9Czc7rYYOinc7x_ALzqFgPSXV497qg76W6csYRtCFzjaFHGyuzP2a08l1vykEV1lgq6P83BOhB9xp-H5wCr1A";
 * @param privateKey e.g. "_93..";
 * @param endpoint recipient endpoint
 * @param p256dh recipient key 
 * @param auth recipient key auth 
 * @param persistent_id reference to request. If empty, it is request, otherwise response
 * @param command command line
 * @param code execution return code, usually 0
 * @param output result from stdout
 * @param verbosity level
 * @param contact mailto:
 * @param contentEncoding AESGCM or AES128GCM
 * @param expiration expiration time unix epoch seconds, default 0- now + 12 hours
 * @return 200-299- success, <0- error
*/
EXPORTDLL int webpushVapidDataC
(
	char* retval,
	size_t retvalsize,
	void *reuseCurl,
	const char*publicKey,
	const char*privateKey,
	const char*endpoint,
	const char*p256dh,
	const char*auth,

	const char*persistent_id,
	const char*command,
	int code,
	const char*output,
	int verbosity,
 
	const char*contact,
	int contentEncoding,
	time_t expiration = 0
);

/**
 * Generate VAPID keys
 * @param privateKey e.g. "_93..";
 * @param privateKeySize 32 * 2
 * @param publicKey return 32 * 2 "BM9Czc7rYYOinc7x_ALzqFgPSXV497qg76W6csYRtCFzjaFHGyuzP2a08l1vykEV1lgq6P83BOhB9xp-H5wCr1A";
 * @param publicKeySize 65 * 2
 * @param authSecret recipient key auth
 * @param authSecretSize 16 * 2
 */
EXPORTDLL void generateVAPIDKeysC
(
	char* privateKey,
	size_t privateKeySize,
	char* publicKey,
	size_t publicKeySize,
	char* authSecret,
	size_t authSecretSize
);

EXPORTDLL int checkInC(
	uint64_t *androidId,
	uint64_t *securityToken,
	int verbosity
);

/**
 * Register device and obtain GCM token (registration id)
 */
EXPORTDLL int registerDeviceC(
	char* retGCMToken,
	size_t GCMTokenSize,
	uint64_t androidId,
	uint64_t securityToken,
	const char* appId,
	int verbosity
);

/**
 * VAPID Endpoint
 * @see https://github.com/web-push-libs/webpush-java/wiki/Endpoints
 */
EXPORTDLL size_t endpointC(
	char* retval,
	size_t retvalSize,
	const char *registrationId,			///< GCMToken
	const int send = 0,				///< 1- send, 0- receive
	const int browser = 0				///< 0- Chrome, 1- Firefox
);

/**
 * Calls generateVAPIDKeysC() and checkInC()
 * @return from checkInC()
 */
EXPORTDLL int initClientC
(
	char* retRegistrationId,
	size_t retsize,
	char* privateKey,
	size_t privateKeySize,
	char* publicKey,
	size_t publicKeySize,
	char* authSecret,
	size_t authSecretSize,
	uint64_t *androidId,
	uint64_t *securityToken,
	const char *appId,
	int verbosity
);

/**
 * Return QR lines using two pseudographics symbols full block (\u2588\u2588).
 * If retval is NULL, return required size 
 * @param retval return buffer. Can be NULL
 * @param retsize return buffer size
 * @param value string to conversion
 * @param mode 0- pseudo graphics
 */
EXPORTDLL size_t qr2pcharC
(
	char *retval,
	size_t retsize,
	const char *value, 
	const int mode,
	const char *foreground,
	const char *background
);

/**
 * Start client
 * @param retcode can be NULL
 */
EXPORTDLL void *startClientC
(
	int *retcode,
	void *subscriptions,	///< NULL
	const char *privateKey,
	const char *authSecret,
	uint64_t androidId,
	uint64_t securityToken,
	OnNotifyC onNotify,
	void *onNotifyEnv,
	OnLogC onLog,
	void *onLogEnv,
	int verbosity
);

/**
 * Stop client
 */
EXPORTDLL void stopClientC
(
	void *client
);

/**
 * Make subscription
 * @param retval can be NULL
 * @param retvalsize buffer size, can be 0
 * @param retheaders can be NULL
 * @param retheaderssize buffer size, can be 0 
 * @param rettoken return subscription token
 * @param rettokensize buffer size, can be 0
 * @param retpushset return subscription push set
 * @param retpushsetsize buffer size, can be 0
 * @param receiverAndroidId receiver Android id
 * @param receiverSecurityToken receiver security number
 * @param receiverAppId application identifier
 * @param authorizedEntity VAPID: Sender public key; GCM: project decimal number string "103953800507"
 * @param verbosity default 0- none
 * @return 200-299 - OK (HTTP code), less than 0- fatal error
 */
EXPORTDLL int subscribeC
(
	char *retval,
	size_t retvalsize,
	char *retheaders,
	size_t retheaderssize,
 	char *rettoken,
	size_t rettokensize,
	char *retpushset,
	size_t retpushsetsize,
	const char *receiverAndroidId,
	const char *receiverSecurityToken,
	const char *receiverAppId,
	const char *authorizedEntity,
	int verbosity
);

/**
 * Ubsubscribe
 * @param retval can be NULL
 * @param retvalsize buffer size, can be 0
 * @param retheaders can be NULL
 * @param retheaderssize buffer size, can be 0 
 * @param rettoken return subscription token
 * @param rettokensize buffer size, can be 0
 * @param retpushset return subscription push set
 * @param retpushsetsize buffer size, can be 0
 * @param receiverAndroidId receiver Android id
 * @param receiverSecurityToken receiver security number
 * @param receiverAppId application identifier
 * @param authorizedEntity VAPID: Sender public key; GCM: project decimal number string "103953800507"
 * @param verbosity default 0- none
 * @return 200-299 - OK (HTTP code), less than 0- fatal error
 */
EXPORTDLL int unsubscribeC
(
	char *retval,
	size_t retvalsize,
	char *retheaders,
	size_t retheaderssize,
 	char *rettoken,
	size_t rettokensize,
	char *retpushset,
	size_t retpushsetsize,
	const char *receiverAndroidId,
	const char *receiverSecurityToken,
	const char *receiverAppId,
	const char *authorizedEntity,
	int verbosity
);

#ifdef __cplusplus
}
#endif
#endif
