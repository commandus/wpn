#ifdef __cplusplus
extern "C" {
#endif
	
#include <ece/keys.h>

#define AESGCM 		0
#define AES128GCM	1

#ifdef _MSC_VER
#ifdef EXPORT_C_DLL
#define EXPORT __declspec(dllexport) 
#else
#define EXPORT __declspec(dllimport) 
#endif
#else
#define EXPORT
#endif

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
EXPORT size_t webpushVapidCmdC(
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
EXPORT int webpushVapidC(
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
EXPORT int webpushVapidDataC
(
	char* retval,
	size_t retvalsize,
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

#ifdef __cplusplus
}
#endif
