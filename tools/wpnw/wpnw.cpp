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
#include <iterator>
#include <cstring>
#include <curl/curl.h>
#include <argtable3/argtable3.h>
#include <fstream>

#include "wp-storage-file.h"
#include "utilinstance.h"
#include "utilfile.h"
#include "utilrecv.h"
#include "utilvapid.h"
#include "endpoint.h"
#include "sslfactory.h"
#include "wp-registry.h"

#include "config-filename.h"
#include "wpnapi.h"
#include "errlist.h"
#include "subscription-load.h"

static const char* progname = "wpnw";
#define DEF_CONFIG_FILE_NAME			".wpn.js"

int sendMessage(
	void *curl,
	std::string &retval,
	const std::string &cmdFileName,
	
	const std::string &registrationid,
	const std::string &endPoint,
	int provider,

	const std::string &publicKey,
	const std::string &privateKey,
	const std::string &endpoint,
	const std::string &p256dh,
	const std::string &auth,
	const std::string &msg,
	const std::string &contact,
	bool aesgcm,
	int verbosity
) {
	if (verbosity > 0) {
		std::cerr
			<< "registrationid: " <<  registrationid << std::endl
			<< "endpoint: " << endPoint << std::endl
			<< "provider: " << (provider == PROVIDER_FIREFOX ? "firefox" : "chrome") << std::endl
			<< "privateKey: " << privateKey << std::endl
			<< "publicKey: " << publicKey << std::endl
			<< "p256dh: " << p256dh << std::endl 
			<< "auth: " << auth << std::endl 
			<< "msg: " << msg << std::endl 
			<< "contact: " << contact << std::endl 
			<< (aesgcm ? "AESGCM" : "AES128GCM") << std::endl;
	}

	time_t t = time(NULL) + 86400 - 60;

	if (verbosity > 1) {
		// print out curl
		std::string r = webpushVapidCmd(
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
		std::cerr << r << std::endl;
	}

	int r = webpushVapid(
		curl,			// re-use CURL connection
		retval, 
		publicKey,		// from
		privateKey,		// from
		endPoint,
		p256dh,			// to
		auth,			// to
		msg,
		contact,
		aesgcm ? AESGCM : AES128GCM,
		t
	);

	if (r < 200 || r > 299) 
	{
		std::cerr << "Send error " << r << ": " << retval << std::endl;
	}
	return r;
}

int sendMessageSubscription(
	void *curl,
	std::string &retval,
	const std::string &cmdFileName,
	const Subscription *subscription,
	const std::string &publicKey,
	const std::string &privateKey,
	const std::string &contact,
	bool aesgcm,
	int verbosity,

	const std::string &subject,
	const std::string &body,
	const std::string &icon,
	const std::string &link,
	const std::string &data
) {
	std::string registrationid = subscription->getToken();
	std::string p256dh = subscription->getWpnKeys().getPublicKey();
	std::string auth = subscription->getWpnKeys().getAuthSecret();
	int provider = subscription->getSubscribeMode();
	std::string endPoint = endpoint(registrationid, true, provider);	///< 0- Chrome, 1- Firefox
	std::string msg = jsClientNotification(registrationid, subject, body, icon, link, data);

	return sendMessage(
		curl,
		retval,
		cmdFileName,
		registrationid,
		endPoint,
		provider,

		publicKey,
		privateKey,
		endPoint,

		p256dh,
		auth,
		msg,
		contact,
		aesgcm,
		verbosity
	);
}

int main(int argc, char **argv) 
{
	struct arg_str *a_subscriptionids = arg_strn(NULL, NULL, "<number>", 0, 32768, "Subscription number/name. If no provide  -r, -d, -a");
	// message itself
	struct arg_str *a_subject = arg_str0("t", "subject", "<Text>", "Subject (topic)");
	struct arg_str *a_body = arg_str0("b", "body", "<Text>", "Default read from stdin");
	struct arg_str *a_icon = arg_str0("i", "icon", "<URL>", "http[s]:// icon address.");
	struct arg_str *a_link = arg_str0("l", "link", "<URL>", "http[s]:// action address.");
	struct arg_str *a_data = arg_str0("D", "data", "<text>", "optional payload");

	// from
	struct arg_str *a_contact = arg_str0("f", "from", "<URL>", "Sender's email e.g. mailto:alice@acme.com or https[s] link");

	// to
	struct arg_str *a_provider = arg_str0("p", "provider", "chrome|firefox", "Default chrome.");
	struct arg_str *a_force_publickey = arg_str0("K", "public-key", "<base64>", "Override VAPID public key");
	struct arg_str *a_force_privatekey = arg_str0("k", "private-key", "<base64>", "Override VAPID private key");

	// use different config files
	struct arg_str *a_config = arg_str0("c", "config", "<file>", "Config file. Default " DEF_CONFIG_FILE_NAME);

	// if no subscription id or name
	struct arg_str *a_registrationid = arg_str0("r", "registration", "<id>", "no id/name: Subscription VAPID key)");
	struct arg_str *a_p256dh = arg_str0("d", "p256dh", "<base64>", "no id/name: VAPID public key");
	struct arg_str *a_auth = arg_str0("a", "auth", "<base64>", "no id/name: VAPID auth");

	struct arg_lit *a_aesgcm = arg_lit0("1", "aesgcm", "Force AESGCM. Default AES128GCM");
	struct arg_lit *a_verbosity = arg_litn("v", "verbose", 0, 4, "0- quiet (default), 1- errors, 2- warnings, 3- debug, 4- debug libs");
	struct arg_lit *a_help = arg_lit0("h", "help", "Show this help");
	struct arg_end *a_end = arg_end(20);

	void* argtable[] = { 
		a_subscriptionids,
		a_registrationid, a_p256dh, a_auth, 
		a_subject, a_body, a_icon, a_link,
		// from
		a_contact, 
		// to
		a_provider, a_force_publickey, a_force_privatekey,
		// options
		a_config, a_data, a_aesgcm, a_verbosity, a_help, a_end 
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

	std::vector<std::string> subscriptionids;
	for (size_t i = 0; i < a_subscriptionids->count; i++) {
		subscriptionids.push_back(a_subscriptionids->sval[i]);
	}
	
	// read config
	std::string config;
	if (a_config->count)
		config = *a_config->sval;
	else
		config = getDefaultConfigFileName(DEF_CONFIG_FILE_NAME);
	ConfigFile wpnConfig(config);
	wpnConfig.clientOptions->setVerbosity(verbosity);
	if (wpnConfig.errorCode) {
		std::cerr << "Error " << wpnConfig.errorCode << ": " << wpnConfig.errorDescription << std::endl;
		exit(wpnConfig.errorCode);
	}

	RegistryClient rclient(&wpnConfig);
	if (!rclient.validate()) {
		std::cerr << "Error register client" << std::endl;
	}

	std::string privateKey = wpnConfig.wpnKeys->getPrivateKey();
	std::string publicKey = wpnConfig.wpnKeys->getPublicKey();

	std::string registrationid;
	std::string p256dh;
	std::string auth;

	bool isNoIdOrName = a_subscriptionids->count == 0;
	if (isNoIdOrName) {
		if (a_registrationid->count)
			registrationid = *a_registrationid->sval;
		if (a_p256dh->count)
			p256dh = *a_p256dh->sval;
		if (a_auth->count)
			auth = *a_auth->sval;

		if (registrationid.empty()) {
			nerrors++;
			std::cerr << "No subscription id/name, recipient registration id missed." << std::endl;
		}
		if (p256dh.empty()) {
			nerrors++;
			std::cerr << "No subscription id/name, recipient public key missed." << std::endl;
		}
		if (auth.empty()) {
			nerrors++;
			std::cerr << "No subscription id/name, recipient auth missed." << std::endl;
		}
	}

	std::string subject; 
	std::string icon;
	std::string link;
	std::string data;
	
	std::string force_privatekey = *a_force_privatekey->sval;
	std::string force_publickey = *a_force_publickey->sval; 
		
	if (a_subject->count)
		subject = *a_subject->sval;
	else
		subject = "";

	if (a_icon->count)
		icon = *a_icon->sval;
	else
		icon = "";

	if (a_link->count)
		link = *a_link->sval;
	else 
		link = "";
	if (a_data->count)
		data = *a_data->sval;
	else 
		data = "";
	
	std::string contact = *a_contact->sval;
	std::string cmdFileName = "curl.out";
	bool aesgcm = a_aesgcm->count > 0;

	// special case: '--help' takes precedence over error reporting
	if ((a_help->count) || nerrors)
	{
		if (nerrors)
			arg_print_errors(stderr, a_end, progname);
		std::cerr << "Usage: " << progname << std::endl;
		arg_print_syntax(stderr, argtable, "\n");
		std::cerr << "Send web push message from stdin" << std::endl;
		arg_print_glossary(stderr, argtable, "  %-27s %s\n");
		arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
		return 1;
	}

	enum VAPID_PROVIDER provider = PROVIDER_CHROME;
	if ("firefox" == std::string(*a_provider->sval)) {
		provider = PROVIDER_FIREFOX;
	}

	std::string body;
	if (a_body->count)
		body = *a_body->sval;
	else
		body = "";

	arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));

	// In windows, this will init the winsock stuff
	curl_global_init(CURL_GLOBAL_ALL);
	OpenSSL_add_all_algorithms();

	int r = 0;

	// read body
	if (body.empty()) {
		// don't skip the whitespace while reading
		std::cin >> std::noskipws;
		// use stream iterators to copy the stream to a string
		std::istream_iterator<char> it(std::cin);
		std::istream_iterator<char> end;
		body = std::string(it, end);
	}

	// override keys
	if (!force_privatekey.empty())
		privateKey = force_privatekey;
	if (!force_publickey.empty())
		publicKey = force_publickey;

	if (isNoIdOrName) {
		// No id or name is provided, get from -r -d -a options
		std::string endPoint = endpoint(registrationid, true, (int) provider);	///< 0- Chrome, 1- Firefox
		std::string msg = jsClientNotification(registrationid, subject, body, icon, link, data);

		std::string retval;
		int r = sendMessage(
			NULL,			// re-use CURL connection
			retval,
			cmdFileName,
			registrationid,
			endPoint,
			provider,

			publicKey,		// from
			privateKey,		// from
			endPoint,
			p256dh,			// to
			auth,			// to
			msg,
			contact,
			aesgcm,
			verbosity
		);
		if (r < 200 || r > 299) {
			exit(r);
		}
		exit(0);
	}

	// check does subscription identifiers is in configuration file, load it if not
	r = loadMissedSubscriptionsFromRegistry(wpnConfig.subscriptions, &rclient, subscriptionids);
	if (r < 0) {
		std::cerr << "Error " << r << ": Can not load subscription(s) from the registry." << std::endl;
		exit(r);
	}
	if (r > 0) {
		// Thera are new subscriptions ready to save in the config file.
		wpnConfig.save();
	}
	CURL *curl = curl_easy_init();

	size_t c = 0;	// count subscriptions
	for (size_t i = 0; i < subscriptionids.size(); i++) {
		std::string subscriptionid = subscriptionids[i];
		// each entry can be number (as decimal string), name of subscription or wildcard( "*" character")
		std::vector<Subscription>::const_iterator s = wpnConfig.subscriptions->findId(subscriptionid);
		if (s != wpnConfig.subscriptions->list.end())
		{
			if (!s->hasToken()) {
				std::cerr << "Error " << ERR_SUBSCRIPTION_TOKEN_NOT_FOUND << ": subscription "  << subscriptionid << " token not found." << std::endl;
				continue;
			}
			c++;

			std::string retval;
			int r = sendMessageSubscription(
				curl,			// re-use CURL connection
				retval,
				cmdFileName,
				&*s,
				publicKey,		// from
				privateKey,		// from
				contact,
				aesgcm,
				verbosity,
				subject,
				body,
				icon,
				link,
				data
			);

			if (r < 200 || r > 299) {
				// 410: push subscription has unsubscribed or expired.
				if (r >= 400 && r < 500) {
					r = rclient.reSubscribe(subscriptionid);
					if (r < 0) {
						std::cerr << "Error " << r << ": Can not re-subscribe to " << subscriptionid << std::endl;
					} else {
						// Thera are new subscriptions ready to save in the config file.
						wpnConfig.save();
						// try again
						std::vector<Subscription>::const_iterator s2 = wpnConfig.subscriptions->findId(subscriptionid);
						if (s2 != wpnConfig.subscriptions->list.end()) {
							sendMessageSubscription(
								curl,			// re-use CURL connection
								retval,
								cmdFileName,
								&*s2,
								publicKey,		// from
								privateKey,		// from
								contact,
								aesgcm,
								verbosity,
								subject,
								body,
								icon,
								link,
								data
							);
						}
					}
				} else
					break;
			}
		}
	}
	curl_easy_cleanup(curl);
	if (c == 0) {
		std::cerr << "Error " << ERR_SUBSCRIPTION_NOT_FOUND << std::endl;
		exit(ERR_SUBSCRIPTION_NOT_FOUND);
	}
	return r;
}
