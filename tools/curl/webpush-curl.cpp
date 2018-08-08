#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <iostream>
#include <sstream>

#include <ece.h>
#include <ece/keys.h>
#include <openssl/ec.h>
#include <openssl/sha.h>

#include "utilvapid.h"


std::string vapid_pk = "BM9Czc7rYYOinc7x_ALzqFgPSXV497qg76W6csYRtCFzjaFHGyuzP2a08l1vykEV1lgq6P83BOhB9xp-H5wCr1A";

static std::string mkJWTHeader
(
	const std::string &aud,
	const std::string &sub,
	const std::string &privateKey,
	time_t exp
)
{
	// Builds a signed Vapid token to include in the `Authorization` header. 
	uint8_t pk[ECE_WEBPUSH_PRIVATE_KEY_LENGTH];
	ece_base64url_decode(privateKey.c_str(), privateKey.size(), ECE_BASE64URL_REJECT_PADDING, pk, ECE_WEBPUSH_PRIVATE_KEY_LENGTH);
	EC_KEY *key = ece_import_private_key(pk, ECE_WEBPUSH_PRIVATE_KEY_LENGTH);
	std::string r = vapid_build_token(key, aud, exp, sub);
	EC_KEY_free(key);
	return r;
}

int main() {
  const char* endpoint = "https://updates.push.services.mozilla.com/wpush/v2/gAAAAABbZ7cIJuyrIqApNuZd0AVjSSrYk5Cef5cI29-g8iRpHvFZzvqO6bI0ymUcf1tJpvg0lCIF7GxAbU7yg7EMXUh6c4MKaFPsSEsLzC7Mlb1JyIAMz5Wf0orVg15A2OD9dBCCUwbol78DdinNpwz-ExA67dH7InfiUDeYZS6QmVNXaPhzpGo";
  const char* p256dh = "BBpYsgvCmjRZTlwQ__nWoeaLwuqxVc9Eg-GSloPxQdvVxapVybJKJMns8IMkYQUDiLBrnXp-qFugkPBq3fOncvY";
  const char* auth = "4SgZbJVmKUP56tJ39wcWPw";

  const char* plaintext = "{\
    \"notification\": {\
      \"title\": \"Title\",\
      \"body\": \"Body\",\
      \"icon\": \"https://commandus.com/favicon.ico\",\
      \"click_action\": \"https://commandus.com/\"\
    } \
  }";

  std::cout << plaintext << std::endl;

  size_t plaintextLen = strlen(plaintext);


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
    ECE_WEBPUSH_DEFAULT_RS, padLen, plaintextLen);
  assert(ciphertextLen > 0);
  uint8_t* ciphertext = (uint8_t*) calloc(ciphertextLen, sizeof(uint8_t));
  assert(ciphertext);

  // Encrypt the plaintext and fetch encryption parameters for the headers.
  // `salt` holds the encryption salt, which we include in the `Encryption`
  // header. `rawSenderPubKey` holds the ephemeral sender, or app server,
  // public key, which we include as the `dh` parameter in the `Crypto-Key`
  // header. `ciphertextLen` is an in-out parameter set to the actual ciphertext
  // length.
  uint8_t salt[ECE_SALT_LENGTH];
  uint8_t rawSenderPubKey[ECE_WEBPUSH_PUBLIC_KEY_LENGTH];
  int err = ece_webpush_aesgcm_encrypt(
    rawRecvPubKey, rawRecvPubKeyLen, authSecret, authSecretLen,
    ECE_WEBPUSH_DEFAULT_RS, padLen, (const uint8_t*)plaintext, plaintextLen, salt,
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

  const char* filename = "aesgcm.bin";
  FILE* ciphertextFile = fopen(filename, "wb");
  assert(ciphertextFile);
  size_t ciphertextFileLen =
    fwrite(ciphertext, sizeof(uint8_t), ciphertextLen, ciphertextFile);
  assert(ciphertextLen == ciphertextFileLen);
  fclose(ciphertextFile);

  printf("curl -v -X POST -H \"Content-Type: application/octet-stream\" -H \"Content-Encoding: aesgcm\" -H \"Crypto-Key: "
         "%s;p256ecdsa=%s\" -H \"Encryption: %s\" -H \"Authorization: WebPush %s\"  --data-binary @%s %s\n",
         cryptoKeyHeader, vapid_pk.c_str(), encryptionHeader, jwt.c_str(), filename, endpoint);

  free(ciphertext);
  free(cryptoKeyHeader);
  free(encryptionHeader);

  return 0;}