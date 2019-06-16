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

#include "utilvapid.h"

static const char* progname = "ec";

int main(int argc, char **argv) 
{
	struct arg_str *a_key = arg_str0(NULL, NULL, "<base64>", "Public(encrypt) or private key(decrypt)");
	struct arg_str *a_auth = arg_str0("a", "auth", "<base64>", "Auth");
	struct arg_lit *a_generate = arg_lit0("g", "keys", "Generate public/private keys");
	struct arg_lit *a_help = arg_lit0("h", "help", "Show this help");
	struct arg_end *a_end = arg_end(20);

	void* argtable[] = { 
		a_key, a_auth, a_generate,
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

	std::string auth;
	if (a_auth->count)
		auth = *a_auth->sval;
	else {
		auth = "";
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
		r = generateKeys(priv, pub, auth);
		if (r) {
			std::cerr << "Error generate keys" << std::endl;
		} else {
			std::cout << pub << "\t" << priv << "\t" << auth << std::endl;
		}
	} else {
		std::string s;
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
