#include <string>
#include <ece/keys.h>

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
	uint32_t exp,
	const std::string &sub
);
