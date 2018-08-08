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
  * Indicates whether `c` is an ASCII control character, and must be escaped to appear in a JSON string.
  */
static inline bool vapid_json_escape_is_control(char c) {
	return c >= '\0' && c <= '\x1f';
}

# define SHA256_DIGEST_LENGTH    32
static const std::string VAPID_HEADER("{\"alg\":\"ES256\",\"typ\":\"JWT\"}");

/**
  * Returns an escaped literal for a control character, double quote, or reverse solidus; `\0` otherwise.
  */
static inline char vapid_json_escape_literal(char c) {
	switch (c) {
	case '\b':
		return 'b';
	case '\n':
		return 'n';
	case '\f':
		return 'f';
	case '\r':
		return 'r';
	case '\t':
		return 't';
	case '"':
	case '\\':
		return c;
	}
	return '\0';
}

static const char vapid_hex_table[] = "0123456789abcdef";

/**
  * Writes a Unicode escape sequence for a control character.
  */
static inline size_t vapid_json_escape_unicode(
	char c,
	int position,
	std::string &result
) 
{
	result[position] = '\\';
	result[position + 1] = 'u';
	result[position + 2] = '0';
	result[position + 3] = '0';
	result[position + 4] = vapid_hex_table[(c >> 4) & 0xf];
	result[position + 5] = vapid_hex_table[c & 0xf];
	return 6;
}

/**
  * Returns the length of `str` as a JSON string, including room for double quotes and escape sequences for special characters.
  */
static size_t vapid_json_quoted_length(
	const std::string &str
)
{
	// 2 bytes for the opening and closing quotes.
	size_t len = 2;
	for (size_t i = 0; i < str.length(); i++) {
		if (vapid_json_escape_literal(str[i])) {
			// 2 bytes: "\", followed by the escaped literal.
			len += 2;
		} else if (vapid_json_escape_is_control(str[i])) {
			// 6 bytes: "\u", followed by a four-byte Unicode escape sequence.
			len += 6;
		} else {
			len++;
		}
	}
	return len;
}

/**
  * Converts `str` into a double-quoted JSON string and escapes all special
  * characters. This is the only JSON encoding we'll need to do, since our claims
  * object contains two strings and a number.
  */
static std::string vapid_json_quote(
	const std::string &str
) 
{
	size_t quotedLen = vapid_json_quoted_length(str);
	std::string r(quotedLen, '\0');
	r[0] = '"';
	r[quotedLen - 1] = '"';
	int p = 1;
	for (size_t i = 0; i < str.length(); i++) {
		char escLiteral = vapid_json_escape_literal(str[i]);
		if (escLiteral) {
			// Some special characters have escaped literal forms.
			r[p] = '\\';
			p++;
			r[p] = escLiteral;
			p++;
		} else {
			if (vapid_json_escape_is_control(str[i])) {
				// Other control characters need Unicode escape sequences.
				p += vapid_json_escape_unicode(str[i], p, r);
			} else {
				r[p] = str[i];
				p++;
			}
		}
	}
	return r;
}

/**
  * Builds and returns the signature base string. This is what we'll sign with
  * our private key. The base string is *not* null-terminated.
  */
static std::string vapid_build_signature_base(
	const std::string &aud, 
	time_t exp,
	const std::string & sub
) {
	std::stringstream opayload;
	opayload << "{\"aud\":" << vapid_json_quote(aud) 
		<< ",\"exp\":" << exp 
		<< ",\"sub\":" << vapid_json_quote(sub) << "}";
	std::string payload(opayload.str());
  return base64UrlEncode(VAPID_HEADER.c_str(), VAPID_HEADER.size())
		+ "."
		+ base64UrlEncode(payload.c_str(), (size_t) payload.size()) ;
}

// Signs a signature base string with the given `key`, and returns the raw
// signature.
static uint8_t* vapid_sign(
	EC_KEY* key,
	const void* sigBase,
	size_t sigBaseLen,
    size_t* sigLen
) 
{
	ECDSA_SIG* sig = NULL;
	const BIGNUM* r;
	const BIGNUM* s;
	uint8_t* rawSig = NULL;

	// Our algorithm is "ES256", so we compute the SHA-256 digest.
	uint8_t digest[SHA256_DIGEST_LENGTH];
	SHA256((const unsigned char *) sigBase, sigBaseLen, digest);

	// OpenSSL has an `ECDSA_sign` function that writes a DER-encoded ASN.1
	// structure. We use `ECDSA_do_sign` instead because we want to write
	// `s` and `r` directly.
	sig = ECDSA_do_sign(digest, SHA256_DIGEST_LENGTH, key);
	if (!sig)
		return NULL;
	ECDSA_SIG_get0(sig, &r, &s);

	size_t rLen = (size_t) BN_num_bytes(r);
	size_t sLen = (size_t) BN_num_bytes(s);
	*sigLen = rLen + sLen;
	rawSig = (uint8_t*) calloc(*sigLen, sizeof(uint8_t));
	if (!rawSig) {
		ECDSA_SIG_free(sig);
		return NULL;
	}

	BN_bn2bin(r, rawSig);
	BN_bn2bin(s, &rawSig[rLen]);

	ECDSA_SIG_free(sig);
	return rawSig;
}

std::string vapid_build_token(
	EC_KEY* key, 
	const std::string &aud, 
	time_t exp,
	const std::string &sub
) {
	std::string sigBase = vapid_build_signature_base(aud, exp, sub);
	size_t sigLen;
	uint8_t* sig = vapid_sign(key, sigBase.c_str(), sigBase.size(), &sigLen);
	std::string token = sigBase + "." + base64UrlEncode(sig, sigLen);
	free(sig);
	return token;
}

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

  // jwt = "eyJ0eXAiOiJKV1QiLCJhbGciOiJFUzI1NiJ9.eyJhdWQiOiJodHRwczovL2NvbW1hbmR1cy5jb20vIiwiZXhwIjoxNTMzNjYzNjE2LCJzdWIiOiJtYWlsdG86YW5kcmVpLmkuaXZhbm92QGdtYWlsLmNvbSJ9.a3xLpu-9BAFlfT_5-xqmXQJ1ZhoDHEsivHbBSAmQtf4i4P6IVkNzyP24QEsQBLERv8KTFs5uCYPfe_I3WnMGJA";
  jwt = "eyJ0eXAiOiJKV1QiLCJhbGciOiJFUzI1NiJ9.eyJhdWQiOiJodHRwczovL2NvbW1hbmR1cy5jb20vIiwiZXhwIjoxNTMzNzMzNzA4LCJzdWIiOiJtYWlsdG86YW5kcmVpLmkuaXZhbm92QGdtYWlsLmNvbSJ9.vVRtEVwvjjoq1kK0ISPntCF0fUSy4k3FQvS-8W44owE3Jdg_iKFU5EYTN-yySHYtSGH066Rii2wsdUVoEGw32A";
  // std::string cryptokey = "p256ecdsa=BM9Czc7rYYOinc7x_ALzqFgPSXV497qg76W6csYRtCFzjaFHGyuzP2a08l1vykEV1lgq6P83BOhB9xp-H5wCr1A";



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

  std::string vapid_pk = "BM9Czc7rYYOinc7x_ALzqFgPSXV497qg76W6csYRtCFzjaFHGyuzP2a08l1vykEV1lgq6P83BOhB9xp-H5wCr1A";
  printf("curl -v -X POST -H \"Content-Type: application/octet-stream\" -H \"Content-Encoding: aesgcm\" -H \"Crypto-Key: "
         "%s;p256ecdsa=%s\" -H \"Encryption: %s\" -H \"Authorization: WebPush %s\"  --data-binary @%s %s\n",
         cryptoKeyHeader, vapid_pk.c_str(), encryptionHeader, jwt.c_str(), filename, endpoint);

  free(ciphertext);
  free(cryptoKeyHeader);
  free(encryptionHeader);

  return 0;}
