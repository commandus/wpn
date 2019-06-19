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
 * @file wpn-grant.cpp
 * 
 */

#include <string>
#include <iostream>
#include <istream>
#include <ostream>
#include <iterator>
#include <vector>
#include <cstring>

#include <openssl/sha.h>
#include <openssl/ssl.h>

#include <ece.h>
#include <ece/keys.h>

#include <argtable3/argtable3.h>

#include "platform.h"
#include "config-filename.h"
#include "utilvapid.h"
#include "utilrecv.h"
#include "wp-storage-file.h"
#include "wp-registry.h"

#define  DEF_CONFIG_FILE_NAME ".wpn.js"

static const char* progname = "wpn-grant";

#define ERR_NOT_FOUND							-1
#define ERR_SUBSCRIBE							-2
#define	ERR_REGISTER_SUBSCRIPTION				-3
#define ERR_NO_ANDROID_ID_N_TOKEN				-4
#define ERR_NO_FCM_TOKEN						-5

int main(int argc, char **argv) 
{
	struct arg_int *a_id = arg_int0(NULL, NULL, "<id>", "Client identifier to establish connection");
	struct arg_lit *a_credentials = arg_lit0("i", "credentials", "Print my id");
	struct arg_lit *a_rm = arg_lit0("d", "delete", "Remove connection");
	struct arg_lit *a_register = arg_lit0("R", "register", "re-register");
	struct arg_str *a_name = arg_str0("n", "name", "<alias>", "Public(encrypt) or private key(decrypt)");
	struct arg_str *a_config = arg_str0("c", "config", "<file>", "Config file. Default " DEF_CONFIG_FILE_NAME);
	struct arg_lit *a_verbosity = arg_litn("v", "verbose", 0, 3, "Set verbosity level");
	struct arg_lit *a_help = arg_lit0("h", "help", "Show this help");
	struct arg_end *a_end = arg_end(20);

	void* argtable[] = { 
		a_id, 
		a_credentials, a_rm,
		a_name, a_config, a_verbosity,
		a_register,
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

	uint64_t id = 0;
	if (a_id->count)
		id = *a_id->ival;
	std::string name;
	bool setName = a_name->count > 0;
	if (setName)
		name = *a_name->sval;
	bool credentials = a_credentials->count > 0;
	bool remove = a_rm->count > 0;
	std::string config;
	if (a_config->count)
		config = *a_config->sval;
	else
		config = getDefaultConfigFileName(DEF_CONFIG_FILE_NAME);

	// special case: '--help' takes precedence over error reporting
	if ((a_help->count) || nerrors)
	{
		if (nerrors)
			arg_print_errors(stderr, a_end, progname);
		std::cerr << "Usage: " << progname << std::endl;
		arg_print_syntax(stderr, argtable, "\n");
		std::cerr << "Manage wpn subscriptions" << std::endl;
		arg_print_glossary(stderr, argtable, "  %-27s %s\n");
		arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
		return 1;
	}
	int verbosity = a_verbosity->count;
	bool reRegister = a_register->count > 0;
	arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));

	OpenSSL_add_all_algorithms();

	ConfigFile wpnConfig(config);
	if (!wpnConfig.wpnKeys->id && !reRegister) {
		std::cerr << "No registration identifier supplied, register with -R." << std::endl;
		exit(1);
	}

	RegistryClient rclient(&wpnConfig);
	// check client consistency
	if (!rclient.validate(verbosity)) {
		std::cerr << "Error check-in and/or register client" << std::endl;
	}

	if (credentials) {
		// print id
		std::cout << wpnConfig.wpnKeys->id;
		if (verbosity) {
			std::cout << "\t" << wpnConfig.wpnKeys->secret << "\t" << wpnConfig.clientOptions->name;
		}
		std::cout << std::endl;
		return 0;
	}

	if (reRegister) {
		uint64_t id;
		if (!rclient.add(&id)) {
			std::cerr << "Error " << rclient.errorCode << ": " << rclient.errorDescription << std::endl;
			return rclient.errorCode;
		}
		wpnConfig.save();
		return 0;
	}

	if (setName) {
		if (id) {
			Subscription *s = wpnConfig.subscriptions->getById(id);
			if (!s) {
				// get from service
				std::string v;
				if (!rclient.get(id, &v)) {
					std::cerr << "Error: no subscription " << id << " found." << std::endl;
					return ERR_NOT_FOUND;
				}
				s = wpnConfig.subscriptions->getById(id);
			}
			if (s) {
				s->setName(name);
				wpnConfig.save();
			}
		} else {
			// no identifier specified
			wpnConfig.clientOptions->name = name;
			wpnConfig.save();
		}
		return 0;
	}

	if (remove) {
		// Remove subscription
		if (id) {
			bool r = wpnConfig.subscriptions->rmById(id);
			if (!r) {
				std::cerr << "Error: can not delete subscription " << id << "." << std::endl;
				return ERR_NOT_FOUND;
			}
			wpnConfig.save();
		} else {
				std::cerr << "Error: no subscription identifier provided." << std::endl;
				return ERR_NOT_FOUND;
		}
		return 0;
	}

	if (id) {
		// identifier is specified
		Subscription *s = wpnConfig.subscriptions->getById(id);
		if (!s) {
			// get from service
			std::string v;
			if (!rclient.get(id, &v)) {
				std::cerr << "Error: no subscription " << id << " found." << std::endl;
				return ERR_NOT_FOUND;
			}
			// Save VAPID public key
			wpnConfig.save();
			s = wpnConfig.subscriptions->getById(id);
		}

		if (!s)
			return ERR_NOT_FOUND;
		if (!s->hasToken()) {
			// Make subscription
			if (s->getSentToken().empty()) {
				// subscribing at the FCM service, set sentToken
				if (verbosity > 2)
					std::cerr << "Subscribing to " << id << std::endl;
				if (!rclient.subscribeById(id)) {
					std::cerr << "Error " << rclient.errorCode << ": "
					<< rclient.errorDescription << ". Can not subscribe to " << id << "." << std::endl;
					return ERR_SUBSCRIBE;
				}
				wpnConfig.save();
			}
			if (s->getSentToken().empty()) {
				return ERR_SUBSCRIBE;
			}
			if (verbosity > 2)
				std::cerr << "Sending subscription to the service " << id << std::endl;
			// Send subscription (sentToken) to the service
			if (!rclient.addSubscription(id)) {
				std::cerr << "Error: can not register subscription to " << id << "." << std::endl;
				return ERR_REGISTER_SUBSCRIPTION;
			}
			// try get subscription from the service
			if (verbosity > 2)
				std::cerr << "Getting subscription from the service " << id << std::endl;
			if (!rclient.getSubscription(id)) {
				std::cerr << "Error: can not get subscription " << id << "." << std::endl;
				return ERR_REGISTER_SUBSCRIPTION;
			}
			wpnConfig.save();
		}

		std::cout << s->getWpnKeys().id;
		if (verbosity) {
			std::cout << "\t" << s->getName();
		}
		if (verbosity > 2) {
			std::cout << "\t" << s->getToken() << "\t" << s->getPushSet();
		}
		std::cout << std::endl;
		return 0;
	}
	
	// print subscription's id and name
	for (std::vector<Subscription>::const_iterator it = wpnConfig.subscriptions->list.begin(); it != wpnConfig.subscriptions->list.end(); ++it) {
		std::cout << it->getWpnKeys().id;
		if (verbosity) {
			std::cout << "\t" << it->getName() << std::endl;
		}
		std::cout << std::endl;
	}

	return 0;
}
