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
