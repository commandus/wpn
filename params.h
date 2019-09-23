#ifndef PARAMS_H
#define PARAMS_H	1

#include <stddef.h>
#include <inttypes.h>

int ece_webpush_aesgcm_headers_extract_params1
(
	const char* cryptoKeyHeader,
	const char* encryptionHeader,
	uint8_t* salt, size_t saltLen,
	uint8_t* rawSenderPubKey,
	size_t rawSenderPubKeyLen,
	uint32_t* rs
);

#endif
