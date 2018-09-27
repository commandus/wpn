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
 * @file wpnr.cpp
 * 
 */

#include <string>
#include <iostream>
#include <curl/curl.h>
#include <argtable3/argtable3.h>
#include <fstream>
#include "nlohmann/json.hpp"

#include "sole/sole.hpp"
#include "sslfactory.h"

#include "config-filename.h"
#include "wpnapi.h"

using json = nlohmann::json;

static const char* progname = "wpnr";
#define DEF_FILE_NAME			".wpn.js"

static int save
(
	const std::string &filename,
 	const char* registrationIdC,
	const char* privateKeyC,
	const char* publicKeyC,
	const char* authSecretC,
	uint64_t androidId,
	uint64_t securityToken,
	const std::string &appId
)
{
	int r = 0;
	json json = {
		{ "appId", appId},
		{ "registrationId", registrationIdC},
		{ "privateKey", privateKeyC },
		{ "publicKey", publicKeyC },
		{ "authSecret", authSecretC },
		{ "androidId", androidId },
		{ "securityToken", securityToken }
	};
	std::string d = json.dump(2);
	std::ofstream ostrm(filename);
	try {
		ostrm << d;
	}
	catch (...) {
		std::cerr << "Error write " << filename << std::endl;
		r = -1;
	}
	ostrm.close();
	return r;
}

int main(int argc, char **argv) 
{
	std::string filename;

	int contentEncoding; // AESGCM or AES128GCM

	struct arg_str *a_file_name = arg_str0("c", "config", "<file>", "Configuration file. Default ~/" DEF_FILE_NAME);
	struct arg_lit *a_verbosity = arg_litn("v", "verbose", 0, 4, "0- quiet (default), 1- errors, 2- warnings, 3- debug, 4- debug libs");
	struct arg_lit *a_help = arg_lit0("h", "help", "Show this help");
	struct arg_end *a_end = arg_end(20);

	void* argtable[] = { 
		a_file_name,
		a_verbosity,
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

	if (a_file_name->count)
		filename = *a_file_name->sval;
	else
		filename = getDefaultConfigFileName();
	int verbosity = a_verbosity->count;

	// special case: '--help' takes precedence over error reporting
	if ((a_help->count) || nerrors)
	{
		if (nerrors)
			arg_print_errors(stderr, a_end, progname);
		std::cerr << "Usage: " << progname << std::endl;
		arg_print_syntax(stderr, argtable, "\n");
		std::cerr << "Web push receiver" << std::endl;
		arg_print_glossary(stderr, argtable, "  %-27s %s\n");
		arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
		return 1;
	}
	arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));

	// load config file
	std::ifstream strm(filename);
	json j;
	try {
		strm >> j;
	}
	catch (...) {
		std::cerr << "Error parse " << filename << std::endl;
	}
	strm.close();

	char registrationIdC[256];
	char privateKeyC[96];
	char publicKeyC[240];
	char authSecretC[48];
	uint64_t androidId;
	uint64_t securityToken;
	std::string s;
	std::string appId;

	try {
		appId = j["appId"];
		s = j["registrationId"];
		strncpy(registrationIdC, s.c_str(), sizeof(registrationIdC));
		s = j["privateKey"];
		strncpy(privateKeyC, s.c_str(), sizeof(privateKeyC));
		s = j["publicKey"];
		strncpy(publicKeyC, s.c_str(), sizeof(publicKeyC));
		s = j["authSecret"];
		strncpy(authSecretC, s.c_str(), sizeof(authSecretC));
		androidId = j["androidId"];
		securityToken = j["securityToken"];
	} catch(...) {
	}
	strm.close();

	bool isNew = appId.empty();

	// In windows, this will init the winsock stuff
	curl_global_init(CURL_GLOBAL_ALL);
	OpenSSL_add_all_algorithms();

	int r  = 0;
	if (isNew) 
	{
		// generate a new application name. Is it requited?
		appId = "wp:com.commandus.wpnr#" + sole::uuid4().str();
		// Initialize client
		r = initClientC(
			registrationIdC, sizeof(registrationIdC),
			privateKeyC, sizeof(privateKeyC),
			publicKeyC, sizeof(publicKeyC),
			authSecretC, sizeof(authSecretC),
			&androidId,
			&securityToken,
			appId.c_str(),
			verbosity
		);
		if ((r < 200) || (r >= 300))
		{
			std::cerr << "Error " << r << " on client registration. Check Internet connection and try again." << std::endl;
			return r;
		} else {
			// HTTP 200 -> 0
			r = 0;
		}
		// save 
		r = save(
			filename,
			registrationIdC,
			privateKeyC,
			publicKeyC,
			authSecretC,
			androidId,
			securityToken,
			appId
		);
	}
	return r;
}
