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
 * @file wpnwj.cpp
 * 
 */

#include <string>
#include <iostream>
#include <cstring>
#include <curl/curl.h>
#include <argtable3/argtable3.h>
#include <fstream>

#include "utilinstance.h"
#include "utilstring.h"
#include "utilfile.h"
#include "sslfactory.h"

#include "config-filename.h"
#include "wpnapi.h"

static const char* progname = "wpnw";
#define DEF_FILE_NAME			".wpnw.js"

int main(int argc, char **argv) 
{
	struct arg_str *a_from = arg_str1(NULL, NULL, "<file>", "From");
	struct arg_str *a_to = arg_str1(NULL, NULL, "<file>", "To");
	struct arg_str *a_notification = arg_str1(NULL, NULL, "<file>", "Notification");

	// from contact
	struct arg_str *a_contact = arg_str0("f", "from", "<URL>", "Sender's email e.g. mailto:alice@acme.com or https[s] link");

	struct arg_lit *a_aesgcm = arg_lit0("1", "aesgcm", "Force AESGCM. Default AES128GCM");
	struct arg_lit *a_verbosity = arg_litn("v", "verbose", 0, 4, "0- quiet (default), 1- errors, 2- warnings, 3- debug, 4- debug libs");
	struct arg_lit *a_help = arg_lit0("h", "help", "Show this help");
	struct arg_end *a_end = arg_end(20);

	void* argtable[] = { 
		a_from, a_to, a_notification,
		a_contact,
		a_aesgcm,
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

	int verbosity = a_verbosity->count;
	
	// Load config files and notification JSON
	std::string from = file2string(*a_from->sval);
	std::string to = file2string(*a_to->sval);
	std::string notification = file2string(*a_notification->sval);;
	
	// Check is file empty or not
	if (from.empty()) {
		std::cerr << "From configuration file empty." << std::endl;
		nerrors++;
	}
	if (to.empty()) {
		std::cerr << "To configuration file empty." << std::endl;
		nerrors++;
	}
	if (notification.empty()) {
		std::cerr << "Notification file empty." << std::endl;
		nerrors++;
	}

	enum VAPID_PROVIDER from_provider;
	std::string from_registrationId;
	std::string from_privateKey;
	std::string from_publicKey;
	std::string from_authSecret;
	uint64_t from_androidId;
	uint64_t from_securityToken;
	std::string from_appId;

	// validate JSON
	int r = parseConfig
	(
		from,
		from_provider,
		from_registrationId,
		from_privateKey,
		from_publicKey,
		from_authSecret,
		from_androidId,
		from_securityToken,
		from_appId
	);
	if (r != 0) {
		std::cerr << "From configuration file is invalid." << std::endl;
		nerrors++;
	}

	enum VAPID_PROVIDER to_provider;
	std::string to_registrationId;
	std::string to_privateKey;
	std::string to_publicKey;
	std::string to_authSecret;
	uint64_t to_androidId;
	uint64_t to_securityToken;
	std::string to_appId;

	r = parseConfig
	(
		to,
		to_provider,
		to_registrationId,
		to_privateKey,
		to_publicKey,
		to_authSecret,
		to_androidId,
		to_securityToken,
		to_appId
	);
	if (r != 0) {
		std::cerr << "To configuration file is invalid." << std::endl;
		nerrors++;
	}
	
	// validate notification JSON format
	std::string nto;
	std::string title;
	std::string body;
	std::string icon; 
	std::string click_action;

	r = parseNotificationJson(
		notification,
		nto,
		title,
		body,
		icon, 
		click_action
	);

	if (r != 0) {
		std::cerr << "Notification file is invalid." << std::endl;
		nerrors++;
	}

	std::string contact = *a_contact->sval;
	bool aesgcm = a_aesgcm->count > 0;	
	
	if (to_registrationId.empty()) {
		nerrors++;
		std::cerr << "Recipient registration id missed." << std::endl;
	}
	if (to_publicKey.empty()) {
		nerrors++;
		std::cerr << "Recipient public key missed." << std::endl;
	}
	if (to_authSecret.empty()) {
		nerrors++;
		std::cerr << "Recipient auth missed." << std::endl;
	}
	
	// special case: '--help' takes precedence over error reporting
	if ((a_help->count) || nerrors)
	{
		if (nerrors)
			arg_print_errors(stderr, a_end, progname);
		std::cerr << "Usage: " << progname << std::endl;
		arg_print_syntax(stderr, argtable, "\n");
		std::cerr << "Web push sender" << std::endl;
		arg_print_glossary(stderr, argtable, "  %-27s %s\n");
		arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
		return 1;
	}
	arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));

	char privateKeyC[96];
	char publicKeyC[240];
	std::string appId;

	// In windows, this will init the winsock stuff
	curl_global_init(CURL_GLOBAL_ALL);
	OpenSSL_add_all_algorithms();

	// write
	char retval[4096];
	char endpoint[256];
	endpointC(endpoint, sizeof(endpoint), to_registrationId.c_str(), 1, (int) to_provider);	///< 0- Chrome, 1- Firefox

	strncpy(privateKeyC, from_privateKey.c_str(), sizeof(privateKeyC));
	strncpy(publicKeyC, from_publicKey.c_str(), sizeof(publicKeyC));

	if (verbosity > 0)
	{
		std::cerr
			<< "endpoint: " << endpoint << std::endl
			<< "provider: " << (to_provider == PROVIDER_FIREFOX ? "firefox" : "chrome") << std::endl
			<< "privateKey: " << privateKeyC << std::endl
			<< "publicKey: " << publicKeyC << std::endl;
	}

	std::string msg  = mkNotificationJson(nto, title, body, icon, click_action);
	time_t t = time(NULL) + 86400 - 60;
	r = webpushVapidC(
		retval, sizeof(retval),
		publicKeyC,
		privateKeyC,
		endpoint,
		to_publicKey.c_str(),
		to_authSecret.c_str(),
		msg.c_str(),
		contact.c_str(),
		aesgcm ? AESGCM : AES128GCM,
		t
	);

	if (r < 200 || r > 299) 
	{
		std::cerr << "Send error " << r 
			<< ": " << retval
			<< std::endl;
		return r;
	} 
	return r;
}
