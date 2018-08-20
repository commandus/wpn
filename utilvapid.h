#ifndef UTILVAPID_H
#define UTILVAPID_H 1

#include <string>
#include <ece/keys.h>

#define AESGCM 		0
#define AES128GCM	1

// MathFuncsDll.h
#ifdef _MSC_VER
#ifdef EXPORT_CPP_DLL
#define EXPORTDLL extern "C" __declspec(dllexport) 
#else
#define EXPORTDLL
#endif
#else
#define EXPORTDLL
#endif

#define ERR_WRONG_PARAM				-1

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

EXPORTDLL int checkIn(
	uint64_t *androidId,
	uint64_t *securityToken,
	int verbosity
);

EXPORTDLL std::string base64UrlEncode(
	const void *data,
	size_t size
);

/**
 * Helper function for testing
 * Print out "curl ..."  command line string
 * @param publicKey e.g. "BM9Czc7rYYOinc7x_ALzqFgPSXV497qg76W6csYRtCFzjaFHGyuzP2a08l1vykEV1lgq6P83BOhB9xp-H5wCr1A";
 * @param privateKey e.g. "_93..";
 * @param filename	temporary file keeping AES GCM ciphered data
 * @param endpoint recipient endpoint
 * @param p256dh recipient key 
 * @param auth recipient key auth 
 * @param body JSON string message
 * @param contact mailto:
 * @param contentEncoding AESGCM or AES128GCM
 */
EXPORTDLL std::string webpushVapidCmd(
	const std::string &publicKey,
	const std::string &privateKey,
	const std::string &filename,
	const std::string &endpoint,
	const std::string &p256dh,
	const std::string &auth,
	const std::string &body,
	const std::string &contact,
	int contentEncoding,
	time_t expiration = 0
);

/**
 * Send VAPID web push using CURL library
 * @param retval return string
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
EXPORTDLL int webpushVapid(
	std::string &retval,
	const std::string &publicKey,
	const std::string &privateKey,
	const std::string &endpoint,
	const std::string &p256dh,
	const std::string &auth,
	const std::string &body,
	const std::string &contact,
	int contentEncoding,
	time_t expiration = 0
);

/**
 * Push "command output" to device
 * @param retval return string
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
EXPORTDLL int webpushVapidData
(
	std::string &retval,
	const std::string &publicKey,
	const std::string &privateKey,
	const std::string &endpoint,
	const std::string &p256dh,
	const std::string &auth,

	const std::string &persistent_id,
	const std::string &command,
	int code,
	const std::string &output,
	int verbosity,
 
 	const std::string &contact,
	int contentEncoding,
	time_t expiration = 0
);
#endif