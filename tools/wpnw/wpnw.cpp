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
 * @file wpnw.cpp
 * 
 */

#include <string>
#include <iostream>
#include <cstring>
#include <curl/curl.h>
#include <argtable3/argtable3.h>
#include <fstream>

#include "utilinstance.h"
#include "utilfile.h"
#include "utilrecv.h"
#include "utilvapid.h"
#include "endpoint.h"
#include "sslfactory.h"

#include "config-filename.h"
#include "wpnapi.h"

static const char* progname = "wpnw";
#define DEF_FILE_NAME			".wpnw.js"

int main(int argc, char **argv) 
{
	struct arg_str *a_registrationid = arg_str0("r", "registration", "<id>", "Recipient registration id (subscription VAPID key)");
	struct arg_str *a_p256dh = arg_str0("d", "p256dh", "<base64>", "VAPID public key");
	struct arg_str *a_auth = arg_str0("a", "auth", "<base64>", "VAPID auth");
	
	// message itself
	struct arg_str *a_subject = arg_str0("t", "subject", "<Text>", "Subject (topic)");
	struct arg_str *a_body = arg_str0("b", "body", "<Text>", "Body");
	struct arg_str *a_icon = arg_str0("i", "icon", "<URL>", "http[s]:// icon address.");
	struct arg_str *a_link = arg_str0("l", "link", "<URL>", "http[s]:// action address.");

	// from
	struct arg_str *a_contact = arg_str0("f", "from", "<URL>", "Sender's email e.g. mailto:alice@acme.com or https[s] link");
	// to
	struct arg_str *a_provider = arg_str0("p", "provider", "chrome|firefox", "Default chrome.");
	struct arg_str *a_force_publickey = arg_str0("K", "public-key", "<base64>", "Override VAPID public key");
	struct arg_str *a_force_privatekey = arg_str0("k", "private-key", "<base64>", "Override VAPID private key");

	struct arg_lit *a_aesgcm = arg_lit0("1", "aesgcm", "Force AESGCM. Default AES128GCM");
	struct arg_lit *a_verbosity = arg_litn("v", "verbose", 0, 4, "0- quiet (default), 1- errors, 2- warnings, 3- debug, 4- debug libs");
	struct arg_lit *a_help = arg_lit0("h", "help", "Show this help");
	struct arg_end *a_end = arg_end(20);

	void* argtable[] = { 
		a_registrationid, a_p256dh,
		a_auth, 
		a_subject, a_body, a_icon, a_link,
		// from
		a_contact, 
		// to
		a_provider, a_force_publickey, a_force_privatekey,
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
	std::string registrationid = *a_registrationid->sval;
	std::string p256dh = *a_p256dh->sval;
	std::string auth = *a_auth->sval;
	std::string subject; 
	std::string icon;
	std::string link;
	std::string body;
	
	std::string force_privatekey = *a_force_privatekey->sval;
	std::string force_publickey = *a_force_publickey->sval; 
		
	if (a_subject->count)
	{
		subject = *a_subject->sval;
	}
	else
	{
		subject = "";
	}

	if (a_body->count)
	{
		body = *a_body->sval;
	}
	else
	{
		body = "";
	}

	if (a_icon->count)
	{
		icon = *a_icon->sval;
	}
	else
	{
		icon = "";
	}

	if (a_link->count)
	{
		link = *a_link->sval;
	} 
	else 
	{
		link = "";
	}
	
	std::string contact = *a_contact->sval;
	std::string cmdFileName = "curl.out";
	bool aesgcm = a_aesgcm->count > 0;

	if (registrationid.empty()) {
		nerrors++;
		std::cerr << "Recipient registration id missed." << std::endl;
	}
	if (p256dh.empty()) {
		nerrors++;
		std::cerr << "Recipient public key missed." << std::endl;
	}
	if (auth.empty()) {
		nerrors++;
		std::cerr << "Recipient auth missed." << std::endl;
	}
	if (body.empty()) {
		nerrors++;
		std::cerr << "Message text missed." << std::endl;
	}

	enum VAPID_PROVIDER provider = PROVIDER_CHROME;
	if ("firefox" == std::string(*a_provider->sval)) {
		provider = PROVIDER_FIREFOX;
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

	std::string privateKey;
	std::string publicKey;
	std::string appId;

	// In windows, this will init the winsock stuff
	curl_global_init(CURL_GLOBAL_ALL);
	OpenSSL_add_all_algorithms();

	int r  = 0;

	// write
	std::string retval;
	std::string endPoint = endpoint(registrationid, 1, (int) provider);	///< 0- Chrome, 1- Firefox

	privateKey = force_privatekey;
	publicKey =  force_publickey;

	if (verbosity > 0)
	{
		std::cerr
			<< "endpoint: " << endPoint << std::endl
			<< "provider: " << (provider == PROVIDER_FIREFOX ? "firefox" : "chrome") << std::endl
			<< "privateKey: " << privateKey << std::endl
			<< "publicKey: " << publicKey << std::endl;
	}
	
	std::string msg  = mkNotificationJson(registrationid, subject, body, icon, link);
	time_t t = time(NULL) + 86400 - 60;
	if (verbosity > 1)
	{
		retval = webpushVapidCmd(
			publicKey,
			privateKey,
			cmdFileName,
			endPoint,
			p256dh,
			auth,
			msg,
			contact,
			aesgcm ? AESGCM : AES128GCM,
			t
		);
		std::cerr << "curl: " << std::endl << retval << std::endl;
	}
	
	r = webpushVapid(
		retval, 
		publicKey,
		privateKey,
		endPoint,
		p256dh.c_str(),
		auth.c_str(),
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
