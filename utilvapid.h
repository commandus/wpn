#include <string>
#include <ece/keys.h>

#define AESGCM 		0
#define AES128GCM	1

std::string base64UrlEncode(
	const void *data,
	size_t size
);

/**
 * Builds a signed Vapid token to include in the `Authorization` header. The token is null-terminated.
 */
std::string vapid_build_token(
	EC_KEY* key, 
	const std::string &aud, 
	time_t exp,
	const std::string &sub
);

std::string extractURLProtoAddress(
	const std::string &endpoint
);

std::string mkJWTHeader
(
	const std::string &aud,
	const std::string &sub,
	const std::string &privateKey,
	time_t exp
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
std::string webpushVapidCmd(
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
int webpushVapid(
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
