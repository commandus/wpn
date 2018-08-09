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
#include "nlohmann/json.hpp"

#include "utilvapid.h"

using json = nlohmann::json;

static const char* progname = "wpn";

/**
 * @param publicKey = "BM9Czc7rYYOinc7x_ALzqFgPSXV497qg76W6csYRtCFzjaFHGyuzP2a08l1vykEV1lgq6P83BOhB9xp-H5wCr1A";
 * @param privateKey = "_93Jy3cT0SRuUA1B9-D8X_zfszukGUMjIcO5y44rqCk";
 * @param filename	temporary file keeping AES GCM ciphered data
 * @param endpoint recipient endpoint
 * @param p256dh recipient key 
 * @param auth recipient key auth 
 * @param body JSON string message
 * @param contentEncoding string message  
 */
std::string webpush2curl(
	const std::string &publicKey,
	const std::string &privateKey,
	const std::string &filename,
	const std::string &endpoint,
	const std::string &p256dh,
	const std::string &auth,
	const std::string &body,
	int contentEncoding
) {
	uint8_t rawRecvPubKey[ECE_WEBPUSH_PUBLIC_KEY_LENGTH];
	size_t rawRecvPubKeyLen = ece_base64url_decode(p256dh.c_str(), p256dh.size(), ECE_BASE64URL_REJECT_PADDING, rawRecvPubKey, ECE_WEBPUSH_PUBLIC_KEY_LENGTH);
	
	uint8_t authSecret[ECE_WEBPUSH_AUTH_SECRET_LENGTH];
	size_t authSecretLen = ece_base64url_decode(auth.c_str(), auth.size(), ECE_BASE64URL_REJECT_PADDING, authSecret, ECE_WEBPUSH_AUTH_SECRET_LENGTH);

	size_t cipherSize = ece_aesgcm_ciphertext_max_length(ECE_WEBPUSH_DEFAULT_RS, 0, body.size());
	std::string cipherString(cipherSize, '\0');

	// Encrypt the body and fetch encryption parameters for the headers:
	// - salt holds the encryption salt, which we include in the `Encryption` header.
	// - rawSenderPubKey holds the ephemeral sender, or app server, public key, which we include as the `dh` parameter in the `Crypto-Key` header.
	uint8_t salt[ECE_SALT_LENGTH];
	uint8_t rawSenderPubKey[ECE_WEBPUSH_PUBLIC_KEY_LENGTH];
	int err = ece_webpush_aesgcm_encrypt(
		rawRecvPubKey, rawRecvPubKeyLen, authSecret, authSecretLen,
		ECE_WEBPUSH_DEFAULT_RS, 0, (const uint8_t*)body.c_str(), body.size(), salt,
		ECE_SALT_LENGTH, rawSenderPubKey, ECE_WEBPUSH_PUBLIC_KEY_LENGTH, (uint8_t *) cipherString.c_str(),
		&cipherSize
	);
	if (err != ECE_OK)
		return "";

	// Build the `Crypto-Key` and `Encryption` HTTP headers. First, we pass
	// `NULL`s for `cryptoKeyHeader` and `encryptionHeader`, and 0 for their
	// lengths, to calculate the lengths of the buffers we need. Then, we
	// allocate, write out, and null-terminate the headers.
	size_t cryptoKeyHeaderLen = 0;
	size_t encryptionHeaderLen = 0;
	err = ece_webpush_aesgcm_headers_from_params(
		salt, ECE_SALT_LENGTH, rawSenderPubKey, ECE_WEBPUSH_PUBLIC_KEY_LENGTH,
		ECE_WEBPUSH_DEFAULT_RS, NULL, &cryptoKeyHeaderLen, NULL,
		&encryptionHeaderLen
	);
	if (err != ECE_OK)
		return "";

	// Allocate an extra byte for the null terminator.
	std::string cryptoKeyHeader(cryptoKeyHeaderLen, '\0');
	std::string encryptionHeader(encryptionHeaderLen, '\0');

	err = ece_webpush_aesgcm_headers_from_params(
		salt, ECE_SALT_LENGTH, rawSenderPubKey, ECE_WEBPUSH_PUBLIC_KEY_LENGTH,
		ECE_WEBPUSH_DEFAULT_RS, (char *) cryptoKeyHeader.c_str(), &cryptoKeyHeaderLen,
		(char *) encryptionHeader.c_str(), &encryptionHeaderLen
	);
	if (err != ECE_OK)
		return "";

	std::ofstream cipherFile(filename, std::ios::out | std::ios::binary);
	cipherFile.write(cipherString.c_str(), cipherSize);
	cipherFile.close();

	time_t expiration = time(NULL) + (24 * 60 * 60);
	std::stringstream r;
	if (contentEncoding == AES128GCM) {
		r << "curl -v -X POST -H \"Content-Type: application/octet-stream\" -H \"Content-Encoding: aes128gcm\" -H \"TTL: 2419200\" "
			<< " -H \"Encryption: " << encryptionHeader
			<< "\" -H \"Authorization: vapid t=" << mkJWTHeader(extractURLProtoAddress(endpoint), "", privateKey, expiration) << ", k=" << publicKey 
			<< "\"  --data-binary @" << filename
			<< " " << endpoint << std::endl;
	} else {
		r << "curl -v -X POST -H \"Content-Type: application/octet-stream\" -H \"Content-Encoding: aesgcm\" -H \"TTL: 2419200\" -H \"Crypto-Key: "
			<< cryptoKeyHeader
			<< ";p256ecdsa=" << publicKey
			<< "\" -H \"Encryption: " << encryptionHeader
			<< "\" -H \"Authorization: WebPush " << mkJWTHeader(extractURLProtoAddress(endpoint), "", privateKey, expiration)
			<< "\"  --data-binary @" << filename
			<< " " << endpoint << std::endl;
	}

	return r.str();
}

int main(int argc, char **argv) 
{
	std::string title;
	std::string body;
	std::string icon;
	std::string click_action;
	std::string aud;
	std::string sub;

	std::string publicKey;
	std::string privateKey;

	std::string endpoint;
	std::string p256dh;
	std::string auth;
	
	int contentEncoding; // AESGCM or AES128GCM

	struct arg_str *a_title = arg_str1("t", "title", "<title>", "Title of push notification");
	struct arg_str *a_body = arg_str1("b", "body", "<text>", "Push notification body");
	struct arg_str *a_icon = arg_str1("i", "icon", "<URL>", "Push notification icon URL");
	struct arg_str *a_action = arg_str1("c", "action", "<URL>", "Push notification action on click URL");
	struct arg_str *a_aud = arg_str1("s", "site", "<URL>", "Originator site URL");
	struct arg_str *a_sub = arg_str1("f", "from", "<URL>", "mailto: address of the contact");
	
	struct arg_str *a_public_key = arg_str1("k", "public", "<key>", "VAPID public key");
	struct arg_str *a_private_key = arg_str1("p", "private", "<key>", "VAPID private key");
	struct arg_str *a_endpoint = arg_str1("e", "endpoint", "<URL>", "Recipient's endpoint URL");
	struct arg_str *a_p256dh = arg_str1("d", "p256dh", "<key>", "Recipient's endpoint p256dh");
	struct arg_str *a_auth = arg_str1("a", "auth", "<key>", "Recipient's endpoint auth");

	struct arg_lit *a_aes128gcm = arg_lit0("1", "aes128gcm", "content encoding aes128gcm. Default aesgcm");
	struct arg_lit *a_help = arg_lit0("h", "help", "Show this help");
	struct arg_end *a_end = arg_end(20);

	void* argtable[] = { 
		a_title, a_body, a_icon, a_action, a_aud, a_sub, 
		a_public_key, a_private_key, a_endpoint, a_p256dh, a_auth,
		a_aes128gcm,
		a_help, a_end 
	};

	int nerrors;

	// verify the argtable[] entries were allocated successfully
	if (arg_nullcheck(argtable) != 0)
	{
		arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
		return 1;
	}
	// Parse the command line as defined by argtable[]
	nerrors = arg_parse(argc, argv, argtable);

	title = *a_title->sval;
	body = *a_body->sval;
	icon = *a_icon->sval;
	click_action = *a_action->sval;
	aud = *a_aud->sval;
	sub = *a_sub->sval;
	publicKey = *a_public_key->sval;
	privateKey = *a_private_key->sval;
	endpoint = *a_endpoint->sval;
	p256dh = *a_p256dh->sval;
	auth = *a_auth->sval;
	if (a_aes128gcm->count)
		contentEncoding = AES128GCM;
	else
		contentEncoding = AESGCM;
		
	// special case: '--help' takes precedence over error reporting
	if ((a_help->count) || nerrors)
	{
		if (nerrors)
			arg_print_errors(stderr, a_end, progname);
		std::cerr << "Usage: " << progname << std::endl;
		arg_print_syntax(stderr, argtable, "\n");
		std::cerr << "Web push message CURL printer" << std::endl;
		arg_print_glossary(stderr, argtable, "  %-25s %s\n");
		arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
		return 1;
	}

	arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));

	std::string filename = "aesgcm.bin";

	json requestBody = {
		{"notification", 
			{
				{"title", title},
				{"body", body},
				{"icon", icon},
				{"click_action", click_action}
			}
		}
	};

	std::string r = webpush2curl(
		publicKey, privateKey, filename, endpoint, p256dh, auth,
		requestBody.dump(), contentEncoding);
	std::cout << r << std::endl;
	return 0;
}
