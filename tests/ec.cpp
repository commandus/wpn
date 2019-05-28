/**
 * Copyright (c) 2019 Andrei Ivanov <andrei.i.ivanov@commandus.com>
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 * 
 * @file ec.cpp
 * 
 * g++ -std=c++11 -o ec -I .. -I ../third_party ec.cpp argtable3.o base64url.o params.o keys.o trailer.o encrypt.o decrypt.o -L/usr/local/lib -lssl -lcrypto
 * 
 */

#include <string>
#include <iostream>
#include <istream>
#include <ostream>
#include <iterator>
#include <cstring>

#include <openssl/sha.h>
#include <openssl/ssl.h>

#include <ece.h>
#include <ece/keys.h>

#include <argtable3/argtable3.h>

static const char* progname = "ec";

static std::string base64UrlEncode(
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
static int generateKeys(
	std::string &retPrivateKey,
	std::string &retPublicKey,
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

static int encryptEC (
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

static int decryptEC(
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

int main(int argc, char **argv) 
{
	struct arg_str *a_key = arg_str0(NULL, NULL, "<key>", "Public(encrypt) or private key(decrypt)");
	struct arg_lit *a_generate = arg_lit0("g", "keys", "Generate public/private keys");
	struct arg_lit *a_help = arg_lit0("h", "help", "Show this help");
	struct arg_end *a_end = arg_end(20);

	void* argtable[] = { 
		a_key, a_generate,
		a_help, a_end 
	};

	// verify the argtable[] entries were allocated successfully
	if (arg_nullcheck(argtable) != 0)
	{
		arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
		return 1;
	}
	// Parse the command line as defined by argtable[]
	int nerrors = arg_parse(argc, argv, argtable);

	std::string key;
	if (a_key->count)
		key = *a_key->sval;
	else {
		if (a_generate->count == 0)
			nerrors++;
	}

	// special case: '--help' takes precedence over error reporting
	if ((a_help->count) || nerrors)
	{
		if (nerrors)
			arg_print_errors(stderr, a_end, progname);
		std::cerr << "Usage: " << progname << std::endl;
		arg_print_syntax(stderr, argtable, "\n");
		std::cerr << "encryption/decryption utility" << std::endl;
		arg_print_glossary(stderr, argtable, "  %-27s %s\n");
		arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
		return 1;
	}
	// length public: 87, private 43
	bool encrypt = key.length() > 43;
	bool generate = a_generate->count > 0;

	arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));

	OpenSSL_add_all_algorithms();

	int r;
	if (generate) {
		std::string pub;
		std::string priv;
		std::string auth;
		r = generateKeys(priv, pub, auth);
		if (r) {
			std::cerr << "Error generate keys" << std::endl;
		} else {
			std::cout << pub << "\t" << priv << std::endl;
		}
	} else {
		std::string s;
		std::string auth = "";
		// don't skip the whitespace while reading
		std::cin >> std::noskipws;
		// use stream iterators to copy the stream to a string
		std::istream_iterator<char> it(std::cin);
		std::istream_iterator<char> end;
		std::string v(it, end);
		if (encrypt) {
			int r = encryptEC(s, v, key, auth);
			if (r) {
				std::cerr << "Error: " << r << std::endl;
			} else {
				std::cout << s;
			}
		} else {
			int r = decryptEC(s, v, key, auth);
			if (r) {
				std::cerr << "Error: " << r << std::endl;
			} else {
				std::cout << s;
			}
		}
	}
	return 0;
}
