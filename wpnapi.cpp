#include <string>
#include <string.h>
#include "wpnapi.h"
#include "utilvapid.h"
#include "utilrecv.h"
#include "utilqr.h"
#include "wp-storage-file.h"
#include "mcs/mcsclient.h"

#define TRIES	5

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
	const char* publicKey,
	const char* privateKey,
	const char* filename,
	const char* endpoint,
	const char* p256dh,
	const char* auth,
	const char* body,
	const char* contact,
	int contentEncoding,
	time_t expiration
)
{
	std::string r = webpushVapidCmd(
		std::string(publicKey),
		std::string(privateKey),
		std::string(filename),
		std::string(endpoint),
		std::string(p256dh),
		std::string(auth),
		std::string(body),
		std::string(contact),
		contentEncoding,
		expiration
	);

	if ((retvalsize > 0) && retval) {
		size_t sz = (r.size() < retvalsize) ? r.size() : retvalsize;
		memmove(retval, r.c_str(), sz);
		if (sz < retvalsize) {
			// add terminal
			retval[sz] = '\0';
		}
	}
	return r.size();
}

/**
 * Send VAPID web push using CURL library
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
	char* retval,
	size_t retvalsize,
	const char *publicKey,
	const char *privateKey,
	const char *endpoint,
	const char *p256dh,
	const char *auth,
	const char *body,
	const char *contact,
	int contentEncoding,
	time_t expiration
)
{
	std::string r;
	int c = webpushVapid(
		r,
		std::string(publicKey),
		std::string(privateKey),
		std::string(endpoint),
		std::string(p256dh),
		std::string(auth),
		std::string(body),
		std::string(contact),
		contentEncoding,
		expiration
	);
	if (retvalsize > 0) {
		size_t sz = (r.size() < retvalsize) ? r.size() : retvalsize;
		memmove(retval, r.c_str(), sz);
		if (sz < retvalsize) {
			// add terminal
			retval[sz] = '\0';
		}
	}
	return c;
}

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
	const char *publicKey,
	const char *privateKey,
	const char *endpoint,
	const char *p256dh,
	const char *auth,

	const char *persistent_id,
	const char *command,
	int code,
	const char *output,
	int verbosity,
  	const char *contact,
	int contentEncoding,
	time_t expiration
)
{
	std::string r;
	int c = webpushVapidData(
		r,
		std::string(publicKey),
		std::string(privateKey),
		std::string(endpoint),
		std::string(p256dh),
		std::string(auth),
		std::string(persistent_id),
		std::string(command),
		code,
		std::string(output),
		verbosity,
		std::string(contact),
		contentEncoding,
		expiration
	);
	if (retvalsize > 0) {
		size_t sz = (r.size() < retvalsize) ? r.size() : retvalsize;
		memmove(retval, r.c_str(), sz);
		if (sz < retvalsize) {
			// add terminal
			retval[sz] = '\0';
		}
	}
	return c;
}

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
)
{
	WpnKeys k;
	k.generate();
	std::string p = k.getPrivateKey();
	size_t sz = p.size();
	if (privateKey && (privateKeySize >= sz)) {
		memmove(privateKey, p.c_str(), sz);
		if (privateKeySize > sz) {
			privateKey[sz] = '\0';
		}
	}

	p = k.getPublicKey();
	sz = p.size();
	if (publicKey && (publicKeySize >= sz)) {
		memmove(publicKey, p.c_str(), sz);
		if (publicKeySize > sz) {
			publicKey[sz] = '\0';
		}
	}

	p = k.getAuthSecret();
	sz = p.size();
	if (authSecret && (authSecretSize >= sz)) {
		memmove(authSecret, p.c_str(), sz);
		if (authSecretSize > sz) {
			authSecret[sz] = '\0';
		}
	}
}

EXPORTDLL int checkInC(
	uint64_t *androidId,
	uint64_t *securityToken,
	int verbosity
) {
	return checkIn(androidId,
		securityToken,
		verbosity
	);
}

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
)
{
	generateVAPIDKeysC(
		privateKey,
		privateKeySize,
		publicKey,
		publicKeySize,
		authSecret,
		authSecretSize
	);
	int r = checkIn(
		androidId,
		securityToken,
		verbosity
	);
	if (r < 200 || r >= 300)
		return r;

	std::string retGCMToken;
	for (int i = 0; i < TRIES; i++)
	{
		r = registerDevice(
			&retGCMToken,
			*androidId,
			*securityToken,
			appId,
			verbosity
		);
		if (r >= 200 && r < 300)
			break;
	}
	
	size_t sz = retGCMToken.size();

	if (retRegistrationId && (retsize >= sz)) {
		memmove(retRegistrationId, retGCMToken.c_str(), sz);
		if (retsize > sz) {
			retRegistrationId[sz] = '\0';
		}
	}
	return r;
}

/**
 * Register device and obtain GCM token
 */
EXPORTDLL int registerDeviceC(
	char* retGCMToken,
	size_t GCMTokenSize,
	uint64_t androidId,
	uint64_t securityToken,
	const char* appId,
	int verbosity
)
{
	std::string t;
	int r = registerDevice(&t, androidId, securityToken, std::string(appId), verbosity);
	if (r < 200 || r >= 300) {
		if (retGCMToken)
			retGCMToken[0] = '\0';
		return r;
	}
	size_t sz = t.size();
	if (retGCMToken && (GCMTokenSize >= sz)) {
		memmove(retGCMToken, t.c_str(), sz);
		if (GCMTokenSize > sz) {
			retGCMToken[sz] = '\0';
		}
	}
	return r;
}

/**
 * Return QR lines using two pseudographics symbols full block (\u2588\u2588).
 * If retval is NULL, return required size
 * @param retval return buffer. Can be NULL
 * @param retsize return buffer size
 * @param value string to conversion
 * @param mode 0- pseudo graphics
 */
EXPORTDLL size_t qr2pchar
(
	char *retval,
	size_t retsize,
	const char *value,
	const int mode,
	const char *foreground,
	const char *background
)
{
	std::string r = qr2string(value, foreground, background);
	size_t rs = r.size();
	if (retval)
	{
		size_t csz = retsize >= rs ? rs : retsize;
		if (csz > 0) 
		{
			memmove(retval, r.c_str(), csz);
			if (retsize > csz)
			{
				retval[csz] = '\0';	// add trailing zero
			}
		}
	}
	return rs;
}

/**
 * Start client
 */
EXPORTDLL void *startClient
(
	const char *privateKey,
	const char *authSecret,
	uint64_t androidId,
	uint64_t securityToken,
	OnNotifyC onNotify,
	void *onNotifyEnv,
	OnLogC onLog,
	void *onLogEnv,
	int verbosity
)
{
	MCSClient *client = new MCSClient(
		std::string(privateKey),
		std::string(authSecret),
		androidId,
		securityToken,
		onNotify, onNotifyEnv, onLog, onLogEnv,
		verbosity
	);
	client->connect();
	return client;
}

/**
 * Stop client
 */
EXPORTDLL void stopClient
(
	void *client
)
{
	if (client) {
		delete ((MCSClient*)client);
	}
}

/**
 * VAPID Endpoint
 * @see https://github.com/web-push-libs/webpush-java/wiki/Endpoints
 */
EXPORTDLL size_t endpointC(
	char* retval,
	size_t retsize,
	const char *registrationId,			///< GCMToken
	const int browser					///< 0- Chrome, 1- Firefox
)
{
	std::string r = endpoint(std::string(registrationId));
	size_t rs = r.size();
	if (retval)
	{
		size_t csz = retsize >= rs ? rs : retsize;
		if (csz > 0)
		{
			memmove(retval, r.c_str(), csz);
			if (retsize > csz)
			{
				retval[csz] = '\0';	// add trailing zero
			}
		}
	}
	return rs;
}
