#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include <ece.h>
#include <ece/keys.h>
#include <openssl/ec.h>
#include <openssl/sha.h>
#include <argtable3/argtable3.h>

#include "utilvapid.h"


int main() {
	const char* filename = "aesgcm.bin";
	std::string vapid_pk = "BM9Czc7rYYOinc7x_ALzqFgPSXV497qg76W6csYRtCFzjaFHGyuzP2a08l1vykEV1lgq6P83BOhB9xp-H5wCr1A";
	
	const char* endpoint = "https://updates.push.services.mozilla.com/wpush/v2/gAAAAABbZ7cIJuyrIqApNuZd0AVjSSrYk5Cef5cI29-g8iRpHvFZzvqO6bI0ymUcf1tJpvg0lCIF7GxAbU7yg7EMXUh6c4MKaFPsSEsLzC7Mlb1JyIAMz5Wf0orVg15A2OD9dBCCUwbol78DdinNpwz-ExA67dH7InfiUDeYZS6QmVNXaPhzpGo";
	const char* p256dh = "BBpYsgvCmjRZTlwQ__nWoeaLwuqxVc9Eg-GSloPxQdvVxapVybJKJMns8IMkYQUDiLBrnXp-qFugkPBq3fOncvY";
	const char* auth = "4SgZbJVmKUP56tJ39wcWPw";

	const char* body = "{\
	\"notification\": {\
		\"title\": \"Title\",\
		\"body\": \"Body\",\
		\"icon\": \"https://commandus.com/favicon.ico\",\
		\"click_action\": \"https://commandus.com/\"\
	} \
	}";

	size_t bodyLen = strlen(body);
	
	std::string aud = "https://commandus.com/";
	std::string sub = "mailto:andrei.i.ivanov@gmail.com";
	std::string publicKey = "BM9Czc7rYYOinc7x_ALzqFgPSXV497qg76W6csYRtCFzjaFHGyuzP2a08l1vykEV1lgq6P83BOhB9xp-H5wCr1A";
	std::string privateKey = "_93Jy3cT0SRuUA1B9-D8X_zfszukGUMjIcO5y44rqCk";
	time_t exp = time(NULL) + (60 * 60 * 24);

	std::string jwt = mkJWTHeader(aud, sub, privateKey, exp);

	size_t padLen = 0;

	uint8_t rawRecvPubKey[ECE_WEBPUSH_PUBLIC_KEY_LENGTH];
	size_t rawRecvPubKeyLen =
	ece_base64url_decode(p256dh, strlen(p256dh), ECE_BASE64URL_REJECT_PADDING,
							rawRecvPubKey, ECE_WEBPUSH_PUBLIC_KEY_LENGTH);
	assert(rawRecvPubKeyLen > 0);
	uint8_t authSecret[ECE_WEBPUSH_AUTH_SECRET_LENGTH];
	size_t authSecretLen =
	ece_base64url_decode(auth, strlen(auth), ECE_BASE64URL_REJECT_PADDING,
							authSecret, ECE_WEBPUSH_AUTH_SECRET_LENGTH);
	assert(authSecretLen > 0);

	size_t ciphertextLen = ece_aesgcm_ciphertext_max_length(
	ECE_WEBPUSH_DEFAULT_RS, padLen, bodyLen);
	assert(ciphertextLen > 0);
	uint8_t* ciphertext = (uint8_t*) calloc(ciphertextLen, sizeof(uint8_t));
	assert(ciphertext);

	// Encrypt the body and fetch encryption parameters for the headers.
	// `salt` holds the encryption salt, which we include in the `Encryption`
	// header. `rawSenderPubKey` holds the ephemeral sender, or app server,
	// public key, which we include as the `dh` parameter in the `Crypto-Key`
	// header. `ciphertextLen` is an in-out parameter set to the actual ciphertext
	// length.
	uint8_t salt[ECE_SALT_LENGTH];
	uint8_t rawSenderPubKey[ECE_WEBPUSH_PUBLIC_KEY_LENGTH];
	int err = ece_webpush_aesgcm_encrypt(
	rawRecvPubKey, rawRecvPubKeyLen, authSecret, authSecretLen,
	ECE_WEBPUSH_DEFAULT_RS, padLen, (const uint8_t*)body, bodyLen, salt,
	ECE_SALT_LENGTH, rawSenderPubKey, ECE_WEBPUSH_PUBLIC_KEY_LENGTH, ciphertext,
	&ciphertextLen);
	assert(err == ECE_OK);

	// Build the `Crypto-Key` and `Encryption` HTTP headers. First, we pass
	// `NULL`s for `cryptoKeyHeader` and `encryptionHeader`, and 0 for their
	// lengths, to calculate the lengths of the buffers we need. Then, we
	// allocate, write out, and null-terminate the headers.
	size_t cryptoKeyHeaderLen = 0;
	size_t encryptionHeaderLen = 0;
	err = ece_webpush_aesgcm_headers_from_params(
	salt, ECE_SALT_LENGTH, rawSenderPubKey, ECE_WEBPUSH_PUBLIC_KEY_LENGTH,
	ECE_WEBPUSH_DEFAULT_RS, NULL, &cryptoKeyHeaderLen, NULL,
	&encryptionHeaderLen);
	assert(err == ECE_OK);
	// Allocate an extra byte for the null terminator.
	char* cryptoKeyHeader = (char*) calloc(cryptoKeyHeaderLen + 1, 1);
	assert(cryptoKeyHeader);
	char* encryptionHeader = (char *)calloc(encryptionHeaderLen + 1, 1);
	assert(encryptionHeader);
	err = ece_webpush_aesgcm_headers_from_params(
	salt, ECE_SALT_LENGTH, rawSenderPubKey, ECE_WEBPUSH_PUBLIC_KEY_LENGTH,
	ECE_WEBPUSH_DEFAULT_RS, cryptoKeyHeader, &cryptoKeyHeaderLen,
	encryptionHeader, &encryptionHeaderLen);
	assert(err == ECE_OK);
	cryptoKeyHeader[cryptoKeyHeaderLen] = '\0';
	encryptionHeader[encryptionHeaderLen] = '\0';

  
	std::ofstream cipherFile(filename, std::ios::out | std::ios::binary);
	cipherFile.write((char*)ciphertext, ciphertextLen);
	cipherFile.close();

	std::cout << "curl -v -X POST -H \"Content-Type: application/octet-stream\" -H \"Content-Encoding: aesgcm\" -H \"TTL: 2419200\" -H \"Crypto-Key: "
		<< cryptoKeyHeader
		<< ";p256ecdsa=" << vapid_pk
		<< "\" -H \"Encryption: " << encryptionHeader
		<< "\" -H \"Authorization: WebPush " << jwt
		<< "\"  --data-binary @" << filename
		<< " " << endpoint << std::endl;

	free(ciphertext);
	free(cryptoKeyHeader);
	free(encryptionHeader);

	return 0;
}
