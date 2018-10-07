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

static const char* progname = "wpnw";
#define DEF_FILE_NAME			".wpnw.js"

static std::string jsonConfig
(
	enum VAPID_PROVIDER provider,
 	const char* registrationIdC,
	const char* privateKeyC,
	const char* publicKeyC,
	const char* authSecretC,
	uint64_t androidId,
	uint64_t securityToken,
	const std::string &appId
)
{
	json json = {
		{ "provider", provider == PROVIDER_FIREFOX ? "firefox" : "chrome" },
		{ "appId", appId},
		{ "registrationId", registrationIdC},
		{ "privateKey", privateKeyC },
		{ "publicKey", publicKeyC },
		{ "authSecret", authSecretC },
		{ "androidId", androidId },
		{ "securityToken", securityToken }
	};
	return json.dump(2);
}

static int save
(
	enum VAPID_PROVIDER provider,
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
	std::string d = jsonConfig(
		provider,
		registrationIdC,
		privateKeyC,
		publicKeyC,
		authSecretC,
		androidId,
		securityToken,
		appId
	);
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
	struct arg_str *a_provider = arg_str0("p", "provider", "chrome|firefox", "Re-init web push provider. Default chrome.");

	struct arg_str *a_registrationid = arg_str0("r", "registration", "<id>", "Recipient registration id");
	struct arg_str *a_p256dh = arg_str0("d", "p256dh", "<base64>", "VAPID public key");
	struct arg_str *a_auth = arg_str0("a", "auth", "<base64>", "VAPID auth");
	
	struct arg_str *a_subject = arg_str0("t", "subject", "<Text>", "Subject (topic)");
	struct arg_str *a_body = arg_str0("b", "body", "<Text>", "Body");
	struct arg_str *a_icon = arg_str0("i", "icon", "<URL>", "http[s]:// icon address.");
	struct arg_str *a_link = arg_str0("l", "link", "<URL>", "http[s]:// action address.");

	
	struct arg_str *a_contact = arg_str0("f", "from", "<URL>", "Sender's email e.g. mailto:alice@acme.com or https[s] link");

 	struct arg_lit *a_aesgcm = arg_lit0("1", "aesgcm", "Force AESGCM. Default AES128GCM");
	struct arg_lit *a_verbosity = arg_litn("v", "verbose", 0, 4, "0- quiet (default), 1- errors, 2- warnings, 3- debug, 4- debug libs");
	struct arg_lit *a_help = arg_lit0("h", "help", "Show this help");
	struct arg_end *a_end = arg_end(20);

	void* argtable[] = { 
		a_file_name, a_provider,
		a_registrationid, a_p256dh,
		a_auth, 
		a_subject, a_body, a_icon, a_link,
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

	std::string filename;
	if (a_file_name->count)
		filename = *a_file_name->sval;
	else
		filename = getDefaultConfigFileName(DEF_FILE_NAME);
	int verbosity = a_verbosity->count;
	std::string registrationid = *a_registrationid->sval;
	std::string p256dh = *a_p256dh->sval;
	std::string auth = *a_auth->sval;
	std::string subject; 
	std::string icon;
	std::string link;
	std::string body;
	
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
	bool isNew = a_provider->count;
	if (isNew) {
		if ("firefox" == std::string(*a_provider->sval)) {
			provider = PROVIDER_FIREFOX;
		}
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
	std::string appId;

	try {
		std::string s;
		s = j["provider"];;
		if (s == "firefox")
			provider = PROVIDER_FIREFOX;
		else
			provider = PROVIDER_CHROME;
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

	// In windows, this will init the winsock stuff
	curl_global_init(CURL_GLOBAL_ALL);
	OpenSSL_add_all_algorithms();

	int r  = 0;
	if (isNew || appId.empty()) 
	{
		if (verbosity > 1)
			std::cerr << "Generate credentials.." << std::endl;
		// generate a new application name. Is it required?
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
			provider,
			filename,
			registrationIdC,
			privateKeyC,
			publicKeyC,
			authSecretC,
			androidId,
			securityToken,
			appId
		);
		if (verbosity > 1)
			std::cerr << "Credentials saved in " << filename << ", code: " << r << std::endl;
	}

	// write
	
	int retcode;
	char retval[4096];
	char endpoint[256];
	endpointC(endpoint, sizeof(endpoint), registrationid.c_str(), 1, (int) provider);	///< 0- Chrome, 1- Firefox

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

	time_t t = time(NULL) + 86400 - 60;
	if (verbosity > 1)
	{
		r = webpushVapidCmdC(
			retval, sizeof(retval),
			publicKeyC,
			privateKeyC,
			cmdFileName.c_str(),
			endpoint,
			p256dh.c_str(),
			auth.c_str(),
			body.c_str(),
			contact.c_str(),
			aesgcm ? AESGCM : AES128GCM,
			t
		);
		if (r > 0)
			std::cerr << "curl: " << std::endl << retval << std::endl;
	}
	r = webpushVapidC(
		retval, sizeof(retval),
		publicKeyC,
		privateKeyC,
		endpoint,
		p256dh.c_str(),
		auth.c_str(),
		body.c_str(),
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
