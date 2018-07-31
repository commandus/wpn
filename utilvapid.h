#include <openssl/bn.h>
#include <openssl/ec.h>
#include <openssl/obj_mac.h>
#include <openssl/sha.h>

#include <ece.h>
#include <ece/keys.h>

// Builds a signed Vapid token to include in the `Authorization` header. The token is null-terminated.
char* vapid_build_token(
	EC_KEY* key, 
	const char* aud, 
	size_t audLen, 
	uint32_t exp,
	const char* sub, 
	size_t subLen
);
