#include "utilvapid.h"
#include <fstream>
#include <sstream>

#include <curl/curl.h>
#include <openssl/sha.h>
#include <ece.h>
#include <ece/keys.h>

#include "utiljson.h"

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
static inline int vapid_json_escape_unicode(
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

// Signs a signature base string with the given `key`, and returns the raw signature.
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
  * Builds JWT: a signed Vapid token to include in the `Authorization` header.
  */
static std::string vapid_build_token(
	EC_KEY* key, 
	const std::string &aud, 
	time_t exp,
	const std::string &sub
) {
	// get JWT base 
	std::stringstream opayload;
	opayload << "{\"aud\":" << vapid_json_quote(aud) 
		<< ",\"exp\":" << exp 
		<< ",\"sub\":" << vapid_json_quote(sub) << "}";
	std::string payload(opayload.str());
	std::string sigBase = base64UrlEncode(VAPID_HEADER.c_str(), VAPID_HEADER.size())
		+ "." + base64UrlEncode(payload.c_str(), (size_t) payload.size()) ;
	// add signature
	size_t sigLen;
	uint8_t* sig = vapid_sign(key, sigBase.c_str(), sigBase.size(), &sigLen);
	std::string token = sigBase + "." + base64UrlEncode(sig, sigLen);
	free(sig);
	return token;
}

static std::string extractURLProtoAddress(
	const std::string &endpoint
)
{
	size_t p = 0;
	p = endpoint.find(':', 0);
	if (p != std::string::npos) {
		p = endpoint.find('/', p);
		if (p != std::string::npos) {
			p++;
			p = endpoint.find('/', p);
			if (p != std::string::npos) {
				p++;
				p = endpoint.find('/', p);
				if (p != std::string::npos) {
					return endpoint.substr(0, p);
				}
			}
		}
	}
	return endpoint;
}

static std::string mkJWTHeader
(
	const std::string &aud,
	const std::string &sub,	///< contact
	const std::string &privateKey,
	time_t exp
)
{
	// Builds a signed Vapid token to include in the `Authorization` header. 
	uint8_t pk[ECE_WEBPUSH_PRIVATE_KEY_LENGTH];
	ece_base64url_decode(privateKey.c_str(), privateKey.size(), ECE_BASE64URL_REJECT_PADDING, pk, ECE_WEBPUSH_PRIVATE_KEY_LENGTH);
	EC_KEY *key = ece_import_private_key(pk, ECE_WEBPUSH_PRIVATE_KEY_LENGTH);

	std::string r = vapid_build_token(key, aud, exp, sub);
	// logPEMForKey(key);
	EC_KEY_free(key);
	return r;
}

/**
 * Build the `Crypto-Key` and `Encryption` HTTP headers. First, we pass
 * `NULL`s for `cryptoKeyHeader` and `encryptionHeader`, and 0 for their
 * lengths, to calculate the lengths of the buffers we need. Then, we
 * allocate, write out, and null-terminate the headers.
 * @param retval return cipher as string
 * @param cryptoKeyHeader return Crypto
 * @param encryptionHeader return Encrypt
 * @param p256dh recipient key 
 * @param auth recipient key auth 
 * @param body JSON string message
 */
static int WPCipher(
	std::string &retval,
	std::string &cryptoKeyHeader,
	std::string &encryptionHeader,

	const std::string &p256dh,
	const std::string &auth,
	const std::string &body
)
{
	uint8_t rawRecvPubKey[ECE_WEBPUSH_PUBLIC_KEY_LENGTH];
	size_t rawRecvPubKeyLen = ece_base64url_decode(p256dh.c_str(), p256dh.size(), ECE_BASE64URL_REJECT_PADDING, rawRecvPubKey, ECE_WEBPUSH_PUBLIC_KEY_LENGTH);
	
	uint8_t authSecret[ECE_WEBPUSH_AUTH_SECRET_LENGTH];
	size_t authSecretLen = ece_base64url_decode(auth.c_str(), auth.size(), ECE_BASE64URL_REJECT_PADDING, authSecret, ECE_WEBPUSH_AUTH_SECRET_LENGTH);

	size_t cipherSize = ece_aesgcm_ciphertext_max_length(ECE_WEBPUSH_DEFAULT_RS, 0, body.size());
	retval = std::string(cipherSize, '\0');

	// Encrypt the body and fetch encryption parameters for the headers:
	// - salt holds the encryption salt, which we include in the `Encryption` header.
	// - rawSenderPubKey holds the ephemeral sender, or app server, public key, which we include as the `dh` parameter in the `Crypto-Key` header.
	uint8_t rawSenderPubKey[ECE_WEBPUSH_PUBLIC_KEY_LENGTH];
	uint8_t salt[ECE_SALT_LENGTH];

	int err = ece_webpush_aesgcm_encrypt(
		rawRecvPubKey, rawRecvPubKeyLen, authSecret, authSecretLen,
		ECE_WEBPUSH_DEFAULT_RS, 0, (const uint8_t*)body.c_str(), body.size(), salt,
		ECE_SALT_LENGTH, rawSenderPubKey, ECE_WEBPUSH_PUBLIC_KEY_LENGTH, (uint8_t *) retval.c_str(),
		&cipherSize
	);
	if (err != ECE_OK)
		return err;

	retval.resize(cipherSize);
	
	size_t cryptoKeyHeaderLen = 0;
	size_t encryptionHeaderLen = 0;
	err = ece_webpush_aesgcm_headers_from_params(
		salt, ECE_SALT_LENGTH, rawSenderPubKey, ECE_WEBPUSH_PUBLIC_KEY_LENGTH,
		ECE_WEBPUSH_DEFAULT_RS, NULL, &cryptoKeyHeaderLen, NULL,
		&encryptionHeaderLen
	);
	if (err != ECE_OK)
		return err;

	// Allocate an extra byte for the null terminator.
	cryptoKeyHeader = std::string(cryptoKeyHeaderLen, '\0');
	encryptionHeader = std::string(encryptionHeaderLen, '\0');

	err = ece_webpush_aesgcm_headers_from_params(
		salt, ECE_SALT_LENGTH, rawSenderPubKey, ECE_WEBPUSH_PUBLIC_KEY_LENGTH,
		ECE_WEBPUSH_DEFAULT_RS, (char *) cryptoKeyHeader.c_str(), &cryptoKeyHeaderLen,
		(char *) encryptionHeader.c_str(), &encryptionHeaderLen
	);
	return err;
}

/**
 * Build the `Crypto-Key` and `Encryption` HTTP headers. First, we pass
 * `NULL`s for `cryptoKeyHeader` and `encryptionHeader`, and 0 for their
 * lengths, to calculate the lengths of the buffers we need. Then, we
 * allocate, write out, and null-terminate the headers.
 * @param retval return cipher as string
 * @param cryptoKeyHeader return Crypto
 * @param encryptionHeader return Encrypt
 * @param p256dh recipient key 
 * @param auth recipient key auth 
 * @param body JSON string message
 */
static int WPCipher128(
	std::string &retval,
	const std::string &p256dh,
	const std::string &auth,
	const std::string &body
)
{
	uint8_t rawRecvPubKey[ECE_WEBPUSH_PUBLIC_KEY_LENGTH];
	size_t rawRecvPubKeyLen = ece_base64url_decode(p256dh.c_str(), p256dh.size(), ECE_BASE64URL_REJECT_PADDING, rawRecvPubKey, ECE_WEBPUSH_PUBLIC_KEY_LENGTH);
	
	uint8_t authSecret[ECE_WEBPUSH_AUTH_SECRET_LENGTH];
	size_t authSecretLen = ece_base64url_decode(auth.c_str(), auth.size(), ECE_BASE64URL_REJECT_PADDING, authSecret, ECE_WEBPUSH_AUTH_SECRET_LENGTH);
	size_t cipherSize = ece_aes128gcm_payload_max_length(ECE_WEBPUSH_DEFAULT_RS, 0, body.size());
	retval = std::string(cipherSize, '\0');
	// Encrypt the body and fetch encryption parameters for the headers:
	// - salt holds the encryption salt, which we include in the `Encryption` header.
	// - rawSenderPubKey holds the ephemeral sender, or app server, public key, which we include as the `dh` parameter in the `Crypto-Key` header.

	int err = ece_webpush_aes128gcm_encrypt(
		rawRecvPubKey, rawRecvPubKeyLen, authSecret, authSecretLen,
		ECE_WEBPUSH_DEFAULT_RS, 0, (const uint8_t*) body.c_str(), body.size(), 
		(uint8_t *) retval.c_str(), &cipherSize
	);
	retval.resize(cipherSize);
	return err;
}

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
 * @param expiration 0- 12 hours from now
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
	time_t expiration
) {
	std::string cipherString;
	std::string cryptoKeyHeader;
	std::string encryptionHeader;
	int code;
	if (contentEncoding == AES128GCM) {
		code = WPCipher128(cipherString, p256dh, auth, body);
	} else {
		code = WPCipher(cipherString, cryptoKeyHeader, encryptionHeader, p256dh, auth, body);
	}

	if (code)
		return "";

	std::ofstream cipherFile(filename, std::ios::out | std::ios::binary);
	cipherFile.write(cipherString.c_str(), cipherString.size());
	cipherFile.close();

	if (expiration == 0)
		expiration = time(NULL) + (12 * 60 * 60);
	std::stringstream r;
	if (contentEncoding == AES128GCM) {
		r << "curl -v -X POST -H \"Content-Type: application/octet-stream\" -H \"Content-Encoding: aes128gcm\" -H \"TTL: 2419200\" "
			<< "-H \"Authorization: vapid t=" << mkJWTHeader(extractURLProtoAddress(endpoint), contact, privateKey, expiration) << ", k=" << publicKey 
			<< "\"  --data-binary @" << filename
			<< " " << endpoint << std::endl;
	} else {
		r << "curl -v -X POST -H \"Content-Type: application/octet-stream\" -H \"Content-Encoding: aesgcm\" -H \"TTL: 2419200\" -H \"Crypto-Key: "
			<< cryptoKeyHeader
			<< ";p256ecdsa=" << publicKey
			<< "\" -H \"Encryption: " << encryptionHeader
			<< "\" -H \"Authorization: WebPush " << mkJWTHeader(extractURLProtoAddress(endpoint), contact, privateKey, expiration)
			<< "\"  --data-binary @" << filename
			<< " " << endpoint << std::endl;
	}

	return r.str();
}

std::string getECECErrorString
(
	int code
)
{
	std::string r;
	switch (code) {
	case ECE_ERROR_OUT_OF_MEMORY:
			r = "Out of memory";
			break;
	case ECE_ERROR_INVALID_PRIVATE_KEY:
			r = "Invalid private key";
			break;
	case ECE_ERROR_INVALID_PUBLIC_KEY:
			r = "Invalid public key";
			break;
	case ECE_ERROR_COMPUTE_SECRET:
			r = "Error compute secret";
			break;
	case ECE_ERROR_ENCODE_PUBLIC_KEY:
			r = "Error encode public key";
			break;
	case ECE_ERROR_DECRYPT:
			r = "Error decrypt";
			break;
	case ECE_ERROR_DECRYPT_PADDING:
			r = "Error decrypt padding";
			break;
	case ECE_ERROR_ZERO_PLAINTEXT:
			r = "Error zero plain text";
			break;
	case ECE_ERROR_SHORT_BLOCK:
			r = "Too short block";
			break;
	case ECE_ERROR_SHORT_HEADER:
			r = "Too short header";
			break;
	case ECE_ERROR_ZERO_CIPHERTEXT:
			r = "Zero cipker text";
			break;
	case ECE_ERROR_HKDF:
			r = "Error HKDF";
			break;
	case ECE_ERROR_INVALID_ENCRYPTION_HEADER:
			r = "Invalid encryption header";
			break;
	case ECE_ERROR_INVALID_CRYPTO_KEY_HEADER:
			r = "Invalid Crypto-key header";
			break;
	case ECE_ERROR_INVALID_RS:
			r = "Invalid rs";
			break;
	case ECE_ERROR_INVALID_SALT:
			r = "Invalid salt";
			break;
	case ECE_ERROR_INVALID_DH:
			r = "Invalid dh";
			break;
	case ECE_ERROR_ENCRYPT:
			r = "Error encrypt";
			break;
	case ECE_ERROR_ENCRYPT_PADDING:
			r = "Error encrypt padding";
			break;
	case ECE_ERROR_INVALID_AUTH_SECRET:
			r = "Invalid auth secret";
			break;
	case ECE_ERROR_GENERATE_KEYS:
			r = "Error generate keys";
			break;
	case ECE_ERROR_DECRYPT_TRUNCATED:
			r = "Error decrypt truncated";
			break;
	default:
		r = "";
	}
	return r;
}

/**
  * @brief CURL write callback
  */
static size_t write_header(char* buffer, size_t size, size_t nitems, void *userp) {
	size_t sz = size * nitems;
	((std::string*)userp)->append((char*)buffer, sz);
	return sz;
}

/**
  * @brief CURL write callback
  */
static size_t write_string(void *contents, size_t size, size_t nmemb, void *userp)
{
	if (userp)
		((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

/**
 * Send VAPID web push request using CURL library
 * @param reuseCurl may be NULL 
 * @param retval return string
 * @param publicKey e.g. "BM9Czc7rYYOinc7x_ALzqFgPSXV497qg76W6csYRtCFzjaFHGyuzP2a08l1vykEV1lgq6P83BOhB9xp-H5wCr1A";
 * @param privateKey e.g. "_93..";
 * @param endpoint recipient endpoint
 * @param p256dh recipient key 
 * @param auth recipient key auth 
 * @param body JSON string message
 * @param contact mailto:
 * @param contentEncoding AESGCM or AES128GCM
 * @param expiration 0- 12 hours from now
*/
int webpushVapid(
	void *reuseCurl,
	std::string &retval,
	const std::string &publicKey,
	const std::string &privateKey,
	const std::string &endpoint,
	const std::string &p256dh,
	const std::string &auth,
	const std::string &body,
	const std::string &contact,
	int contentEncoding,
	time_t expiration
) {
	retval = "";
	std::string cipherString;
	std::string cryptoKeyHeader;
	std::string encryptionHeader;
	int code;

	if (contentEncoding == AES128GCM) {
		code = WPCipher128(cipherString, p256dh, auth, body);
	} else {
		code = WPCipher(cipherString, cryptoKeyHeader, encryptionHeader, p256dh, auth, body);
	}

	if (code) {
		retval = "cipher, " + getECECErrorString(code) + "; p256dh: " + p256dh + "; auth: " + auth;
		return code;
	}
	if (expiration == 0)
		expiration = time(NULL) + (12 * 60 * 60);

	CURL *curl;
	if (reuseCurl)
		curl = (CURL *) reuseCurl;
	else 
		curl = curl_easy_init();

	if (!curl)
		return CURLE_FAILED_INIT; 

	CURLcode res;
	
	struct curl_slist *chunk = NULL;

	chunk = curl_slist_append(chunk, ("Content-Type: application/octet-stream"));
	chunk = curl_slist_append(chunk, ("TTL: 2419200"));
	
	if (contentEncoding == AES128GCM) {
		chunk = curl_slist_append(chunk, ("Content-Encoding: aes128gcm"));
		chunk = curl_slist_append(chunk, ("Authorization: vapid t=" 
			+ mkJWTHeader(extractURLProtoAddress(endpoint), contact, privateKey, expiration)
			+ ", k=" + publicKey).c_str());
	} else {
		chunk = curl_slist_append(chunk, ("Content-Encoding: aesgcm"));
		chunk = curl_slist_append(chunk, ("Crypto-Key: " + cryptoKeyHeader + ";p256ecdsa=" + publicKey).c_str());
		chunk = curl_slist_append(chunk, ("Encryption: " + encryptionHeader).c_str());
		chunk = curl_slist_append(chunk, ("Authorization: WebPush " + mkJWTHeader(extractURLProtoAddress(endpoint), contact, privateKey, expiration)).c_str());
	}
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

	curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, cipherString.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, cipherString.size());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_string);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &retval);
	res = curl_easy_perform(curl);

	long http_code;
    if (res != CURLE_OK)
	{
		retval = std::string(curl_easy_strerror(res));
		http_code = - res;
	}
	else
	{
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
	}

	if (!reuseCurl)
		curl_easy_cleanup(curl);
	curl_slist_free_all(chunk);
	return http_code;
}

/**
 * Send VAPID web push using CURL library
 * @param reuseCurl may be NULL 
 * @param retval return error description string
 * @param subscriptionJSON: {public, private, endpoint, p256dh, auth, contact}
 * @param subject 
 * @param body message
 * @param icon
 * @param link 
 * @param optional originator contact mailto: or http[s]: URI
 * @param contentEncoding AESGCM or AES128GCM
 * @return 0 or positive- HTTP code(200..299- success), negative- error code
 */
int webpushVapidJSON(
	void *reuseCurl,
	std::string &retval,
	const std::string &subscriptionJSON,
	const std::string &subject,
	const std::string &body,
	const std::string &icon, 
	const std::string &link,
	int contentEncoding,
	time_t expiration
)
{
	std::string publicKey, privateKey, endpoint, p256dh, auth, contact;
	std::string jsonMsg = jsClientNotification(subscriptionJSON, subject, body, icon, link);
	return webpushVapid(reuseCurl, retval, publicKey, privateKey, endpoint, p256dh, auth, jsonMsg, contact, contentEncoding, expiration);
}

/**
 * Push "command output" to device
 * @param reuseCurl may be NULL 
 * @param retval return string
 * @param publicKey e.g. "BM9Czc7rYYOinc7x_ALzqFgPSXV497qg76W6csYRtCFzjaFHGyuzP2a08l1vykEV1lgq6P83BOhB9xp-H5wCr1A";
 * @param privateKey e.g. "_93..";
 * @param endpoint recipient endpoint
 * @param p256dh recipient key 
 * @param auth recipient key auth 
 * @param persistent_id reference to request. If empty, it is request, otherwise response
 * @param command command line
 * @param code execution return code, usually 0
 * @param output result from stdout
 * @param contact mailto:
 * @param contentEncoding AESGCM or AES128GCM
 * @param expiration expiration time unix epoch seconds, default 0- now + 12 hours
 * @return 200-299- success, <0- error
*/
int webpushVapidData
(
	void *reuseCurl,
	std::string &retval,
	const std::string &publicKey,
	const std::string &privateKey,
	const std::string &endpoint,
	const std::string &p256dh,
	const std::string &auth,

	const std::string &persistent_id,
	const std::string &command,
	int code,
	const std::string &output,
 	const std::string &contact,
	int contentEncoding,
	time_t expiration
)
{
	std::string requestBody = jsClientCommand(endpoint, command, persistent_id, code, output,"", "");
	return webpushVapid(reuseCurl, retval, publicKey, privateKey, endpoint, p256dh, auth,
		requestBody, contact, contentEncoding, expiration);
}
