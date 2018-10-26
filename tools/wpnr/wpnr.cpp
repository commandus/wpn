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
#include <cstring>

#include "sslfactory.h"

#include "config-filename.h"
#include "wpnapi.h"
#include "utilfile.h"
#include "utilinstance.h"

static const char* progname = "wpnr";
#define DEF_FILE_NAME			".wpnr.js"
#undef SUPPORT_FIREFOX

void onNotify
(
	void *env,
	const char *persistent_id,
	const char *from,				///< e.g. BDOU99-h67HcA6JeFXHbSNMu7e2yNNu3RzoMj8TM4W88jITfq7ZmPvIM1Iv-4_l2LxQcYwhqby2xGpWwzjfAnG4
	const char *appName,
	const char *appId,
	int64_t sent,
	const NotifyMessageC *request
)
{
	std::cerr << "Notify " 
		<< "persistent_id: " << persistent_id
		<< std::endl;
}

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
	struct arg_str *a_file_name = arg_str0("c", "config", "<file>", "Configuration file. Default ~/" DEF_FILE_NAME);
	// TODO add Firefox read
#ifdef SUPPORT_FIREFOX	
	struct arg_str *a_provider = arg_str0("p", "provider", "chrome|firefox", "Re-init web push provider. Default chrome.");
#endif	
	struct arg_lit *a_verbosity = arg_litn("v", "verbose", 0, 4, "0- quiet (default), 1- errors, 2- warnings, 3- debug, 4- debug libs");
	struct arg_lit *a_help = arg_lit0("h", "help", "Show this help");
	struct arg_end *a_end = arg_end(20);

	void* argtable[] = { 
		a_file_name,
#ifdef SUPPORT_FIREFOX
		a_provider,
#endif
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

	std::string filename;
	if (a_file_name->count)
		filename = *a_file_name->sval;
	else
		filename = getDefaultConfigFileName(DEF_FILE_NAME);
	int verbosity = a_verbosity->count;

	enum VAPID_PROVIDER provider = PROVIDER_CHROME;
#ifdef SUPPORT_FIREFOX
	if (a_provider->count) {
		if ("firefox" == std::string(*a_provider->sval)) {
			provider = PROVIDER_FIREFOX;
		}
	}
#endif	

	bool isNew = 
#ifdef SUPPORT_FIREFOX	
	a_provider->count > 0;
#else
	false;
#endif	

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

	char registrationIdC[256];
	char privateKeyC[96];
	char publicKeyC[240];
	char authSecretC[48];
	uint64_t androidId;
	uint64_t securityToken;
	std::string appId;
	
	std::string registrationId;
	std::string privateKey;
	std::string publicKey;
	std::string authSecret;

	// load config file
	int r = readConfig(
		filename,
		provider,
		registrationId,
		privateKey,
		publicKey,
		authSecret,
		androidId,
		securityToken,
		appId
	);
	if (r)  {
		std::cerr << "Error parse " << filename << std::endl;
	} else {
		strncpy(registrationIdC, registrationId.c_str(), sizeof(registrationIdC));
		strncpy(privateKeyC, privateKey.c_str(), sizeof(privateKeyC));
		strncpy(publicKeyC, publicKey.c_str(), sizeof(publicKeyC));
		strncpy(authSecretC, authSecret.c_str(), sizeof(authSecretC));
	}

	// In windows, this will init the winsock stuff
	curl_global_init(CURL_GLOBAL_ALL);
	OpenSSL_add_all_algorithms();

	if (isNew || appId.empty()) 
	{
		// generate a new application name. Is it required?
		appId = mkInstanceId();
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
		r = writeConfig(
			filename,
			provider,
			registrationIdC,
			privateKeyC,
			publicKeyC,
			authSecretC,
			androidId,
			securityToken,
			appId
		);
	}
	if (r < 0)
		std::cerr << "Error write " << filename << std::endl;

	// read
	int retcode;
	void *client = startClient(
		&retcode,
		privateKeyC,
		authSecretC,
		androidId,
		securityToken,
		onNotify,
		NULL,
		onLog,
		NULL,
		verbosity
	);
	if (r) 
	{
		std::cerr << "Starting client error " << retcode << std::endl;
		return r;
	} 
	std::cout << "Enter q to quit" << std::endl;
	char endpoint[255];
	endpointC(endpoint, sizeof(endpoint), registrationIdC, 0, (int) provider);	///< 0- Chrome, 1- Firefox
	std::cout << endpoint << std::endl;
	std::cout << jsonConfig(
		provider,
		registrationIdC,
		privateKeyC,
		publicKeyC,
		authSecretC,
		androidId,
		securityToken,
		appId
	) << std::endl;

	// loop
	std::string l;
	do {
		std::cin >> l;
	} while (l != "q");
	
	stopClient(client);
	return r;
}
