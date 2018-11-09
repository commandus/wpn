/**
 * Copyright (c) 2018 Andrei Ivanov <andrei.i.ivanov@commandus.com>
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
 * @file wpnlinkj.cpp
 * 
 */

#include <string>
#include <vector>
#include <iostream>
#include <curl/curl.h>
#include <argtable3/argtable3.h>
#include <fstream>
#include "nlohmann/json.hpp"

#include "utilinstance.h"
#include "sslfactory.h"

#include "config-filename.h"
#include "utilfile.h"
#include "utilstring.h"
#include "wpnapi.h"

using json = nlohmann::json;

static const char* progname = "wpnlinkj";
#undef SUPPORT_FIREFOX

void onLog
(
	void *env,
	int severity,
	const char *message
)
{
	std::cerr << message;
}

int main(int argc, char **argv) 
{
	struct arg_str *a_pub = arg_str1(NULL, NULL, "<sender-file>", "Subscribe to ");
	struct arg_str *a_receivers = arg_strn(NULL, NULL, "<reciever-file>", 1, 100, "Subscriber's config file(s), up to 100");

	struct arg_lit *a_verbosity = arg_litn("v", "verbose", 0, 4, "0- quiet (default), 1- errors, 2- warnings, 3- debug, 4- debug libs");
	struct arg_lit *a_help = arg_lit0("h", "help", "Show this help");
	struct arg_end *a_end = arg_end(20);

	void* argtable[] = { 
		a_pub, a_receivers,
		a_verbosity,
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
	
	int r = 0;
	int verbosity;
	ClientConfig pub;
	std::vector<ClientConfig> tos;

	if (nerrors == 0)
	{
		verbosity = a_verbosity->count;

		std::string pubString = file2string(*a_pub->sval);
		// Check is file empty or not
		if (pubString.empty()) {
			std::cerr << "Publisher configuration file empty." << std::endl;
			nerrors++;
		} else {
			r = parseConfig(
				pubString,
				pub.provider,
				pub.registrationId,
				pub.privateKey,
				pub.publicKey,
				pub.authSecret,
				pub.androidId,
				pub.securityToken,
				pub.appId
			);
			if (r != 0) {
				std::cerr << "Publisher configuration file is invalid." << std::endl;
				nerrors++;
			} else {
				if (pub.publicKey.empty()) {
					nerrors++;
					std::cerr << "Publisher publicKey missed." << std::endl;
				}
			}
		}

		if (a_receivers->count > 0) {
			tos.reserve(a_receivers->count);
			for (int i = 0; i < a_receivers->count; i++) {
				ClientConfig c;
				std::string subscriberString = file2string(a_receivers->sval[i]);
				// Check is file empty or not
				if (subscriberString.empty()) {
					std::cerr << "Subscriber configuration file empty." << std::endl;
					nerrors++;
				} else {
					r = parseConfig(
						subscriberString,
						c.provider,
						c.registrationId,
						c.privateKey,
						c.publicKey,
						c.authSecret,
						c.androidId,
						c.securityToken,
						c.appId
					);
					if (r != 0) {
						std::cerr << "Subscriber configuration file " << a_receivers->sval[i] << " is invalid." << std::endl;
						nerrors++;
					} else {
						if (pub.androidId == 0) {
							nerrors++;
							std::cerr << "Subscriber Android id missed in " << a_receivers->sval[i] << " file." << std::endl;
						}
						if (pub.securityToken == 0) {
							nerrors++;
							std::cerr << "Subscriber security token missed " << a_receivers->sval[i] << " file." << std::endl;
						}
						if (pub.appId.empty()) {
							nerrors++;
							std::cerr << "Subscriber application instance id missed " << a_receivers->sval[i] << " file." << std::endl;
						}
					}
				}
				tos.push_back(c);
			}
		}
	}

	// special case: '--help' takes precedence over error reporting
	if ((a_help->count) || nerrors)
	{
		if (nerrors)
			arg_print_errors(stderr, a_end, progname);
		std::cerr << "Usage: " << progname << std::endl;
		arg_print_syntax(stderr, argtable, "\n");
		std::cerr << "Subscribe to VAPID provider" << std::endl;
		arg_print_glossary(stderr, argtable, "  %-27s %s\n");
		arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
		return 1;
	}
	arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));

	// In windows, this will init the winsock stuff
	curl_global_init(CURL_GLOBAL_ALL);
	OpenSSL_add_all_algorithms();

	// User can subscribe one or more subscribers to the publisher
	std::string authorizedEntity = pub.publicKey;

	for (std::vector<ClientConfig>::const_iterator it(tos.begin()); it != tos.end(); ++it) 
	{
		char retval[2048];
		char headers[2048];
		char token[255];		///< returns subscription token
		char pushset[255];		///< returns pushset. Not implemented. Returns empty string

		if (verbosity > 1) {
			std::cerr << "Subscribe Android id: " 
			<< it->androidId << ", security token: " << it->securityToken << ", application id: " << it->appId
			<< std::endl;
		}
		// Make subscription
		int r = subscribeC(retval, sizeof(retval), headers, sizeof(headers),
			token, sizeof(token), pushset, sizeof(pushset),
			std::to_string(it->androidId).c_str(),  std::to_string(it->securityToken).c_str(), it->appId.c_str(),
			authorizedEntity.c_str(),
			verbosity
		);
		if ((r >= 200) && (r < 300)) {
			std::cout << token << std::endl;
		} else {
			std::cerr << "Error " << r << ": " << retval << std::endl;
		}
		if (r)
			break;
	}
	return r;
}
