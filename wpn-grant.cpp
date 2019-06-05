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
#include "config-filename.h"
#include "utilvapid.h"
#include "wp-storage-file.h"
#include "wp-registry.h"

#define  DEF_CONFIG_FILE_NAME ".wpn.js"

static const char* progname = "wpn-grant";


int main(int argc, char **argv) 
{
	struct arg_int *a_id = arg_int0(NULL, NULL, "<id>", "Client identifier to establish connection");
	struct arg_lit *a_rm = arg_lit0("d", "delete", "Remove connection");
	struct arg_lit *a_register = arg_lit0("R", "register", "re-register");
	struct arg_str *a_name = arg_str0("n", "name", "<alias>", "Public(encrypt) or private key(decrypt)");
	struct arg_str *a_config = arg_str0("c", "config", "<file>", "Config file. Default " DEF_CONFIG_FILE_NAME);
	struct arg_lit *a_verbosity = arg_litn("v", "verbose", 0, 3, "Set verbosity level");
	struct arg_lit *a_help = arg_lit0("h", "help", "Show this help");
	struct arg_end *a_end = arg_end(20);

	void* argtable[] = { 
		a_id, a_rm,
		a_name, a_config,
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
		std::cerr << "No registration identifier supplied. To register enter:" << std::endl << "wpn-grant -R" << std::endl;
		exit(1);
	}
	int r;
	if (id) {
		// identifier is specified
		if (setName) {
			// if set name option is specified
			Subscription *s = wpnConfig.subscriptions->getById(id);
			if (s)
				s->setName(name);
			else
				std::cerr << "Error: no subcription " << id << " found." << std::endl;
		}
	} else {
		// no identifier specified
		if (setName) {
			wpnConfig.clientOptions->name = name;
		}
		if (reRegister) {
			RegistryClient c(&wpnConfig);
			uint64_t id;
			if (!c.add(&id)) {
				std::cerr << "Error " << c.errorCode << ": " << c.errorDescription << std::endl;
				return c.errorCode;
			}
		}
		wpnConfig.save(config);

		// print id
		std::cout << wpnConfig.wpnKeys->id;
		if (verbosity) {
			std::cout << "\t" << wpnConfig.wpnKeys->secret << "\t" << wpnConfig.clientOptions->name << std::endl;
		}
		std::cout << std::endl;
		// print subscription's id and name
		for (std::vector<Subscription>::const_iterator it = wpnConfig.subscriptions->list.begin(); it != wpnConfig.subscriptions->list.end(); ++it) {
			if (verbosity) {

			}
		}
	}
	return 0;
}
