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
 * @file wpnadd.cpp
 * 
 */

#include <string>
#include <cstring>
#include <algorithm>
#include <iostream>

#include <curl/curl.h>
#include <argtable3/argtable3.h>

#include "sslfactory.h"

#include "config-filename.h"
#include "endpoint.h"
#include "utilvapid.h"
#include "utilfile.h"
#include "utilrecv.h"
#include "wp-storage-file.h"
#include "wp-connection.h"
#include "wp-registry.h"
#include "utilinstance.h"

static const char* progname = "wpnadd";
#define DEF_CONFIG_FILE_NAME			".wpn.js"

enum FORMAT_TYPE {
	FORMAT_TYPE_JSON = 0,
	FORMAT_TYPE_TAB = 1,
	FORMAT_TYPE_ENDPOINT = 2
};

static enum FORMAT_TYPE formatTypeStr2Enum(const char *value)
{
	enum FORMAT_TYPE r;
	std::string v(value);
	std::transform(v.begin(), v.end(), v.begin(), ::tolower);
	if (v == "tab")
		r = FORMAT_TYPE_TAB;
	else
		if (v == "endpoint")
			r = FORMAT_TYPE_ENDPOINT;
		else
			r = FORMAT_TYPE_JSON;
	return r;
}

int main(int argc, char **argv) 
{
	struct arg_str *a_format = arg_str0("f", "format", "json|tab|endpoint", "Default JSON. endpoint- print out endpoint only");
	struct arg_str *a_file_name = arg_str0("c", "config", "<file>", "Configuration file. Default ~/" DEF_CONFIG_FILE_NAME);
	struct arg_lit *a_verbosity = arg_litn("v", "verbose", 0, 4, "0- quiet (default), 1- errors, 2- warnings, 3- debug, 4- debug libs");
	struct arg_lit *a_help = arg_lit0("h", "help", "Show this help");
	struct arg_end *a_end = arg_end(20);

	void* argtable[] = { 
		a_format, a_file_name,
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
		filename = getDefaultConfigFileName(DEF_CONFIG_FILE_NAME);

	int verbosity = a_verbosity->count;

	enum VAPID_PROVIDER provider = PROVIDER_CHROME;
	
	enum FORMAT_TYPE ft = FORMAT_TYPE_JSON;
	
	if (a_format->count) {
		ft = formatTypeStr2Enum(*a_format->sval);
	}

	// special case: '--help' takes precedence over error reporting
	if ((a_help->count) || nerrors)
	{
		if (nerrors)
			arg_print_errors(stderr, a_end, progname);
		std::cerr << "Usage: " << progname << std::endl;
		arg_print_syntax(stderr, argtable, "\n");
		std::cerr << "Web push receiver" << std::endl;
		arg_print_glossary(stderr, argtable, "  %-27s %s\n");
		std::cerr << "Tab delimited config:" << std::endl;
		std::cerr << "provider appId registrationId privateKey publicKey authSecret androidId securityToken endpoint" << std::endl;

		arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
		return 1;
	}
	arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));

	// load config file
	ConfigFile wpnConfig(filename);
	RegistryClient rclient(&wpnConfig);
	if (!rclient.validate(verbosity)) {
		std::cerr << "Error register client" << std::endl;
	}

	int r = 0;

	std::string s;
	switch(ft) {
	case FORMAT_TYPE_TAB:
		{
		s = tabConfig(
			provider,
			wpnConfig.wpnKeys->getPublicKey().c_str(),
			wpnConfig.wpnKeys->getPrivateKey().c_str(),
			wpnConfig.wpnKeys->getPublicKey().c_str(),
			wpnConfig.wpnKeys->getAuthSecret().c_str(),
			wpnConfig.androidCredentials->getAndroidId(),
			wpnConfig.androidCredentials->getSecurityToken(),
			wpnConfig.androidCredentials->getAppId(),
			""
		);
		}
		break;
	case FORMAT_TYPE_ENDPOINT:
		{
		s = endpoint(wpnConfig.wpnKeys->getPublicKey().c_str(), false, (int) provider);	///< 0- Chrome, 1- Firefox
		}
		break;
	default:
		{
		s = jsonConfig(
			provider,
			wpnConfig.wpnKeys->getPublicKey().c_str(),
			wpnConfig.wpnKeys->getPrivateKey().c_str(),
			wpnConfig.wpnKeys->getPublicKey().c_str(),
			wpnConfig.wpnKeys->getAuthSecret().c_str(),
			wpnConfig.androidCredentials->getAndroidId(),
			wpnConfig.androidCredentials->getSecurityToken(),
			wpnConfig.androidCredentials->getAppId(),
			""
		);
		}
		break;
	}
	std::cout << s << std::endl;
	return r;
}
