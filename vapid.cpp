#include "vapid.h"

#include <cstring>
#include <openssl/sha.h>
#include <ece.h>
#include <ece/keys.h>

std::string base64UrlEncode(
	const void *data,
	size_t size
) {
	size_t sz = ece_base64url_encode(data, size, ECE_BASE64URL_OMIT_PADDING, NULL, 0);
	std::string r(sz, '\0');
	ece_base64url_encode(data, size, ECE_BASE64URL_OMIT_PADDING, (char*) r.c_str(), r.size());
	return r;
}

/**
 * @return 0- success
 */
int generateKeys(
	std::string &retPrivateKey,
	std::string  &retPublicKey,
	std::string &retAuthSecret
) {
	uint8_t rawRecvPrivKey[ECE_WEBPUSH_PRIVATE_KEY_LENGTH];
	uint8_t rawRecvPubKey[ECE_WEBPUSH_PUBLIC_KEY_LENGTH];
	uint8_t authSecret[ECE_WEBPUSH_AUTH_SECRET_LENGTH];
	int err = ece_webpush_generate_keys(
		rawRecvPrivKey, ECE_WEBPUSH_PRIVATE_KEY_LENGTH, rawRecvPubKey,
		ECE_WEBPUSH_PUBLIC_KEY_LENGTH, authSecret, ECE_WEBPUSH_AUTH_SECRET_LENGTH);
	retPrivateKey = base64UrlEncode(rawRecvPrivKey, ECE_WEBPUSH_PRIVATE_KEY_LENGTH);
	retPublicKey = base64UrlEncode(rawRecvPubKey, ECE_WEBPUSH_PUBLIC_KEY_LENGTH);
	retAuthSecret = base64UrlEncode(authSecret, ECE_WEBPUSH_AUTH_SECRET_LENGTH);
	return err;
}

int encryptEC (
	std::string &retVal,
	const std::string &val,
	const std::string &publicKey,
	const std::string &authSecret
) {
	size_t padLen = 0;

	uint8_t rawRecvPubKey[ECE_WEBPUSH_PUBLIC_KEY_LENGTH];
		size_t rawRecvPubKeyLen = ece_base64url_decode(publicKey.c_str(), publicKey.length(), ECE_BASE64URL_REJECT_PADDING,
	rawRecvPubKey, ECE_WEBPUSH_PUBLIC_KEY_LENGTH);
	if (rawRecvPubKeyLen == 0)
		return -1;
	uint8_t rawAuthSecret[ECE_WEBPUSH_AUTH_SECRET_LENGTH];
	if (authSecret.empty()) {
		memset(rawAuthSecret, 0, ECE_WEBPUSH_AUTH_SECRET_LENGTH);
	} else {
		size_t authSecretLen = ece_base64url_decode(authSecret.c_str(), authSecret.length(), ECE_BASE64URL_REJECT_PADDING,
			rawAuthSecret, ECE_WEBPUSH_AUTH_SECRET_LENGTH);
		if (authSecretLen != ECE_WEBPUSH_AUTH_SECRET_LENGTH)
			return -2;
	}
	size_t payloadLen = ece_aes128gcm_payload_max_length(ECE_WEBPUSH_DEFAULT_RS,
		padLen, val.length());
	if (payloadLen == 0)
		return -3;
	retVal = std::string(payloadLen, '\0');
	int err = ece_webpush_aes128gcm_encrypt(rawRecvPubKey, rawRecvPubKeyLen, rawAuthSecret, ECE_WEBPUSH_AUTH_SECRET_LENGTH,
		ECE_WEBPUSH_DEFAULT_RS, padLen, (const uint8_t *) val.c_str(), val.length(),
		(uint8_t *) retVal.c_str(), &payloadLen);
	if (err != ECE_OK)
		return err;
	retVal.resize(payloadLen);
	return ECE_OK;
}

int decryptEC(
	std::string &retVal,
	const std::string &val,
	const std::string &privateKey,
	const std::string &authSecret
) {
	// Assume `rawSubPrivKey` and `authSecret` contain the subscription private key
	// and auth secret.
	uint8_t rawSubPrivKey[ECE_WEBPUSH_PRIVATE_KEY_LENGTH];
	size_t rawRecvPrivKeyLen = ece_base64url_decode(privateKey.c_str(), privateKey.length(), ECE_BASE64URL_REJECT_PADDING,
		rawSubPrivKey, ECE_WEBPUSH_PRIVATE_KEY_LENGTH);
	if (rawRecvPrivKeyLen == 0)
		return -1;
	uint8_t rawAuthSecret[ECE_WEBPUSH_AUTH_SECRET_LENGTH];
	if (authSecret.empty()) {
		memset(rawAuthSecret, 0, ECE_WEBPUSH_AUTH_SECRET_LENGTH);
	} else {
		size_t authSecretLen = ece_base64url_decode(authSecret.c_str(), authSecret.length(), ECE_BASE64URL_REJECT_PADDING,
			rawAuthSecret, ECE_WEBPUSH_AUTH_SECRET_LENGTH);
		if (authSecretLen == 0)
			return -2;
	}
	size_t decryptedLen = ece_aes128gcm_plaintext_max_length((const uint8_t *) val.c_str(), val.length());
	if (decryptedLen == 0)
		return -3;
	retVal = std::string(decryptedLen, '\0');

	int err = ece_webpush_aes128gcm_decrypt(rawSubPrivKey, ECE_WEBPUSH_PRIVATE_KEY_LENGTH, rawAuthSecret, ECE_WEBPUSH_AUTH_SECRET_LENGTH,
		(const uint8_t *) val.c_str(), val.length(), (uint8_t *) retVal.c_str(), &decryptedLen);

	if (err != ECE_OK)
		return err;
	retVal.resize(decryptedLen);
	return ECE_OK;
}
