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
	struct arg_str *a_from = arg_str1(NULL, NULL, "<from-file>", "From");
	struct arg_str *a_to = arg_str1(NULL, NULL, "<to-file>", "To");
	struct arg_str *a_notification = arg_str1(NULL, NULL, "<notification-file>", "Notification");
	
	// optional sender contact
	struct arg_str *a_contact = arg_str0("f", "from", "<URL>", "Sender's email e.g. mailto:alice@acme.com or https[s] link");

	struct arg_lit *a_aesgcm = arg_lit0("1", "aesgcm", "Force AESGCM. Default AES128GCM");
	struct arg_lit *a_verbosity = arg_litn("v", "verbose", 0, 4, "0- quiet (default), 1- errors, 2- warnings, 3- debug, 4- debug libs");
	struct arg_lit *a_help = arg_lit0("h", "help", "Show this help");
	struct arg_end *a_end = arg_end(20);

	void* argtable[] = { 
		// mandatory
		a_from, a_to, a_notification,
		// optional
		a_contact, a_aesgcm,
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

	int verbosity;
	ClientConfig from;
	ClientConfig to;
	NotificationData notificationData;
	
	std::string contact;
	bool aesgcm;

	if (nerrors == 0) {
		verbosity = a_verbosity->count;
		
		// Load config files and notification JSON
		std::string fromString = file2string(*a_from->sval);
		std::string toString = file2string(*a_to->sval);
		std::string notificationString = file2string(*a_notification->sval);
		contact = *a_contact->sval;
		aesgcm = a_aesgcm->count > 0;	

		// validate JSON
		int r = 0;
		// Check is file empty or not
		if (fromString.empty()) {
			std::cerr << "From configuration file empty." << std::endl;
			nerrors++;
		} else {
			r = parseConfig(
				fromString,
				from.provider,
				from.registrationId,
				from.privateKey,
				from.publicKey,
				from.authSecret,
				from.androidId,
				from.securityToken,
				from.appId
			);
			if (r != 0) {
				std::cerr << "From configuration file is invalid." << std::endl;
				nerrors++;
			} else {
				if (from.registrationId.empty()) {
					nerrors++;
					std::cerr << "Recipient registration id missed." << std::endl;
				}
			}
		}

		if (toString.empty()) {
			std::cerr << "To configuration file empty." << std::endl;
			nerrors++;
		} else {
			r = parseConfig(
				toString,
				to.provider,
				to.registrationId,
				to.privateKey,
				to.publicKey,
				to.authSecret,
				to.androidId,
				to.securityToken,
				to.appId
			);
			if (r != 0) {
				std::cerr << "To configuration file is invalid." << std::endl;
				nerrors++;
			} else {
				if (to.publicKey.empty()) {
					nerrors++;
					std::cerr << "Recipient public key missed." << std::endl;
				}
				if (to.authSecret.empty()) {
					nerrors++;
					std::cerr << "Recipient auth missed." << std::endl;
				}
			}
		}

		// validate notification JSON format
		if (notificationString.empty()) {
			std::cerr << "Notification file empty." << std::endl;
			nerrors++;
		} else {
			r = parseNotificationJson(
				notificationString,
				notificationData.to,
				notificationData.title,
				notificationData.body,
				notificationData.icon, 
				notificationData.click_action
			);

			if (r != 0) {
				std::cerr << "Notification file is invalid." << std::endl;
				nerrors++;
			} else {
				if (notificationData.body.empty() && notificationData.title.empty()) {
					nerrors++;
					std::cerr << "Message body and/or title missed." << std::endl;
				}
			}
		}

		contact = *a_contact->sval;
		aesgcm = a_aesgcm->count > 0;
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
	endpointC(endpoint, sizeof(endpoint), to.registrationId.c_str(), 1, (int) to.provider);	///< 0- Chrome, 1- Firefox

	strncpy(privateKeyC, from.privateKey.c_str(), sizeof(privateKeyC));
	strncpy(publicKeyC, from.publicKey.c_str(), sizeof(publicKeyC));

	if (verbosity > 0)
	{
		std::cerr
			<< "endpoint: " << endpoint << std::endl
			<< "provider: " << (to.provider == PROVIDER_FIREFOX ? "firefox" : "chrome") << std::endl
			<< "privateKey: " << privateKeyC << std::endl
			<< "publicKey: " << publicKeyC << std::endl;
	}

	std::string msg  = mkNotificationJson(notificationData.to, notificationData.title, notificationData.body, notificationData.icon, notificationData.click_action);
	time_t t = time(NULL) + 86400 - 60;
	int r = webpushVapidC(
		retval, sizeof(retval),
		publicKeyC,
		privateKeyC,
		endpoint,
		notificationData.to.c_str(),
		to.authSecret.c_str(),
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
