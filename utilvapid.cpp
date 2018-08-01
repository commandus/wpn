/**
  */
#include <string>
#include <sstream>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <openssl/bn.h>
#include <openssl/ec.h>
#include <openssl/obj_mac.h>
#include <openssl/sha.h>

#include <ece.h>

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

static const std::string VAPID_HEADER("{\"alg\":\"ES256\",\"typ\":\"JWT\"}");

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

/**
  * Builds a signed Vapid token to include in the `Authorization` header. The token is null-terminated.
  */
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
