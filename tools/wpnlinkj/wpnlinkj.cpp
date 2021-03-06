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

#include "utilinstance.h"
#include "sslfactory.h"

#include "config-filename.h"
#include "utilfile.h"
#include "utilstring.h"
#include "endpoint.h"
#include "wp-subscribe.h"
#include "wp-registry.h"

static const char* progname = "wpnlinkj";
#undef SUPPORT_FIREFOX

int main(int argc, char **argv) 
{
	struct arg_file *a_files = arg_filen(NULL, NULL, "<file>", 2, 100, "Config file(s), 2..100");
	struct arg_lit *a_verbosity = arg_litn("v", "verbose", 0, 4, "0- quiet (default), 1- errors, 2- warnings, 3- debug, 4- debug libs");
	struct arg_lit *a_force = arg_lit0("f", "force", "Create config files if not exists");
	struct arg_lit *a_help = arg_lit0("h", "help", "Show this help");
	struct arg_end *a_end = arg_end(20);

	void* argtable[] = { 
		a_files,
		a_force, a_verbosity,
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
	int verbosity = a_verbosity->count;
	int force_create_config_file = a_force->count;
	std::vector<ConfigFile*> configs;

	if ((nerrors == 0) && (a_files->count > 0)) {
		configs.reserve(a_files->count);
		for (int i = 0; i < a_files->count; i++) {
			ClientConfig c;
			std::string subscriberString = file2string(std::string(a_files->filename[i]));
			// Check is file empty or not
			ConfigFile cf(a_files->filename[i]);
			if (cf.errorCode) {
				if (force_create_config_file) {
					RegistryClient rclient(&cf);
					// check client consistency
					if (!rclient.validate()) {
						std::cerr << "Error check-in and/or register client, file " << a_files->filename[i] << std::endl;
					} else {
						cf.save();
					}
				}
				std::cerr << "Subscriber configuration file " << a_files->filename[i] << " does not exists, empty or invalid." << std::endl;
				nerrors++;
			} else {
				configs.push_back(new ConfigFile(a_files->filename[i]));
			}
		}
	}

	// special case: '--help' takes precedence over error reporting
	if ((a_help->count) || nerrors) {
		if (nerrors)
			arg_print_errors(stderr, a_end, progname);
		std::cerr << "Usage: " << progname << std::endl;
		arg_print_syntax(stderr, argtable, "\n");
		std::cerr << "Subscribe everyone at all." << std::endl;
		arg_print_glossary(stderr, argtable, "  %-27s %s\n");
		arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
		exit(-30);
	}
	arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));

	// In windows, this will init the winsock stuff
	curl_global_init(CURL_GLOBAL_ALL);
	OpenSSL_add_all_algorithms();

	// User can subscribe one or more subscribers to the publisher

	for (std::vector<ConfigFile*>::const_iterator it_publisher(configs.begin()); it_publisher != configs.end(); ++it_publisher) {
		ConfigFile *publisher = *it_publisher;
		for (std::vector<ConfigFile*>::iterator it_subscriber(configs.begin()); it_subscriber != configs.end(); ++it_subscriber) {
			if (it_publisher == it_subscriber)
				continue;
			ConfigFile *subscriber = *it_subscriber;
			RegistryClient rclient(subscriber);
			// check client consistency
			if (!rclient.validate()) {
				std::cerr << "Error check-in and/ or register client" << std::endl;
			}

			if (verbosity > 1) {
				std::cerr << "Subscribe Android id: " 
				<< subscriber->androidCredentials->getAndroidId() << ", security token: " 
				<< subscriber->androidCredentials->getSecurityToken() << ", application id: " << subscriber->androidCredentials->getAppId()
				<< std::endl;
			}

			// check does subscription exists
			Subscription *s = subscriber->subscriptions->findByPublicKey(publisher->wpnKeys->getPublicKey());
			if (s) 
				continue;	// already subscribed
			// Make subscription
			Subscription ns;
			// Set VAPID public key
			ns.getWpnKeysPtr()->setPublicKey(publisher->wpnKeys->getPublicKey());
			ns.getWpnKeysPtr()->setAuthSecret(publisher->wpnKeys->getAuthSecret());
			ns.setName(publisher->clientOptions->name);
			// Make sure it is not got from the service
			ns.getWpnKeysPtr()->id = 0;
			ns.setSubscribeMode(SUBSCRIBE_FORCE_VAPID);
			
			// Subscribe
			std::string retval;
			std::string headers;
			std::string token;			///< returns subscription token
			std::string pushset;		///< returns pushset. Not implemented. Returns empty string
			int r = subscribe(&retval, &headers, 
				token, pushset, 
				std::to_string(subscriber->androidCredentials->getAndroidId()),
				std::to_string(subscriber->androidCredentials->getSecurityToken()),
				subscriber->androidCredentials->getAppId(),
				publisher->wpnKeys->getPublicKey(),
				verbosity
			);
			if (!((r >= 200) && (r < 300))) {
				std::cerr << "Error " << r << ": " << retval << std::endl;
				break;
			}
			ns.setToken(token);
			subscriber->subscriptions->list.push_back(ns);
			subscriber->save();
		}
	}
	// free config files
	for (std::vector<ConfigFile*>::iterator it(configs.begin()); it != configs.end(); ++it) {
		delete *it;
	}
}
