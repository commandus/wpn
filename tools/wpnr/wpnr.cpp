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
 * Signals:
 * 1 SIGHUP re-read config
 * 2 SIGINT interrupt
 */

#include <string>
#include <iostream>
#include <curl/curl.h>
#include <argtable3/argtable3.h>
#include <fstream>
#include <cstring>
#include <ctime>

#include <signal.h>

#include "platform.h"
#include "sslfactory.h"

#include "config-filename.h"
#include "wp-storage-file.h"
#include "wp-registry.h"
#include "utilinstance.h"
#include "utilrecv.h"
#include "endpoint.h"
#include "mcs/mcsclient.h"
#include "notify2string.h"

static const char* progname = "wpnr";

#define DEF_CONFIG_FILE_NAME			".wpn.js"
#define	EMPTY_LAST_PERSISTENT_ID		""
#define MSG_INTERRUPTED 				"Interrupted "
#define MSG_RELOAD_CONFIG_REQUEST		"Re-read config "
#define PROMPT_STARTED					"Enter q or press Ctrl+C to quit"

static int quitFlag = 0;

void signalHandler(int signal)
{
	switch(signal)
	{
	case SIGHUP:
		std::cerr << MSG_RELOAD_CONFIG_REQUEST << std::endl;
		quitFlag = 2;
		break;
	case SIGINT:
		std::cerr << MSG_INTERRUPTED << std::endl;
		quitFlag = 1;
		break;
	default:
		std::cerr << "Handle " << signal << std::endl;
		break;
	}
}

#ifdef _MSC_VER
// TODO
void setSignalHandler()
{
}
#else
void setSignalHandler()
{
	struct sigaction action;
	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = &signalHandler;
	sigset_t set; 
	sigemptyset(&set);                                                             
	sigaddset(&set, SIGHUP); 
	sigaddset(&set, SIGINT);
	action.sa_mask = set;
	sigaction(SIGHUP, &action, NULL);
	sigaction(SIGINT, &action, NULL);
}
#endif

void readCommand(
	int *quitFlag,
	std::istream &strm,
	MCSClient *client
)
{
	std::string line;
	do {
		if (strm.eof()) {
			// after any signal received stdin is closed and marked as eof()
			break;
		}
		strm >> line;
		if (line.find_first_of("qQ") == 0)
			*quitFlag = 1;
		else {
			if (client && !line.empty()) {
				// Can not write to the push service
				// client->write("BHLrGrU0N6e-c8YrIjDXXnsyv2tw2T9eyfc_8TNrdlEtHJmbXcjvWDgduZ2M3hpxTcdjcmtq1_Gi1b2KlXpRHcc", line);
			}
		}
	} while (*quitFlag == 0);
}

#pragma warning(disable : 4996)
void onNotify
(
	void *env,
	const char *persistent_id,
	const char *from,				///< e.g. BDOU99-h67HcA6JeFXHbSNMu7e2yNNu3RzoMj8TM4W88jITfq7ZmPvIM1Iv-4_l2LxQcYwhqby2xGpWwzjfAnG4
	const char *appName,
	const char *appId,
	int64_t sent,
	const NotifyMessageC *msg
)
{
	ConfigFile *config = (ConfigFile *) env;
	Subscription *s = config->subscriptions->findByPublicKey(from);

	time_t t = sent / 1000;
	struct tm *tm = localtime(&t);
	std::string st = std::asctime(tm);
	if (config->clientOptions->getVerbosity() > 0) {
		std::cerr<< "Notify persistent_id: " << persistent_id << std::endl
			<< "from: " << from << std::endl
			<< "sent: " << st << std::endl
			<< std::endl << std::endl;
	}
	if (msg) {
		uint64_t id = s->getWpnKeys().id;
		std::string name;
		if (s) {
			name = s->getName();
			if (name.empty())
				name = "noname";
			std::cout << id << "(" << name << ")\t";
		} else {
			name = "unknown";
			std::cout << from << "(" << name << ")\t";
		}
		std::stringstream ss;
		switch (config->outputFormatCode) {
		case 1:
			ss << notify2Json(id, name.c_str(), persistent_id, from, appName, appId, sent, msg);
			break;
		case 2:
			ss << notify2Delimiter("\t", id, name.c_str(), persistent_id, from, appName, appId, sent, msg);
			break;
		case 3:
			ss << notify2Csv(id, name.c_str(), persistent_id, from, appName, appId, sent, msg);
			break;
		default:
			ss << msg->body;
			break;
		}
		std::cout << ss.str() << std::endl;
	}
}

void onLog
(
	void *env,
	int severity,
	const char *message
)
{
	ConfigFile *config = (ConfigFile *) env;
	if (config->clientOptions->getVerbosity() > 0) {
		std::cerr << message;
	}
}

int main(int argc, char **argv) 
{
	struct arg_str *a_file_name = arg_str0("c", "config", "<file>", "Configuration file. Default ~/" DEF_CONFIG_FILE_NAME);
	// TODO add Firefox read
	struct arg_lit *a_verbosity = arg_litn("v", "verbose", 0, 4, "0- quiet (default), 1- errors, 2- warnings, 3- debug, 4- debug libs");
	struct arg_lit *a_output_format = arg_litn("o", "output", 0, 3, "0- body only, 1- JSON, 2- tab delimited, 3- CSV");
	struct arg_lit *a_help = arg_lit0("h", "help", "Show this help");
	struct arg_end *a_end = arg_end(20);

	void* argtable[] = { 
		a_file_name,
		a_verbosity, a_output_format,
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
	int outputFormatCode = a_output_format->count;

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

	// In windows, this will init the winsock stuff
	curl_global_init(CURL_GLOBAL_ALL);
	OpenSSL_add_all_algorithms();

	// Signal handler
	setSignalHandler();
	do {
		quitFlag = 0;
		// load config file
		ConfigFile wpnConfig(filename);
		if (wpnConfig.errorCode) {
			std::cerr << "Error " << wpnConfig.errorCode << ": " << wpnConfig.errorDescription << std::endl;
			exit(wpnConfig.errorCode);
		}

		wpnConfig.clientOptions->setVerbosity(verbosity);
		wpnConfig.outputFormatCode = outputFormatCode;

		RegistryClient rclient(&wpnConfig);
		if (!rclient.validate()) {
			std::cerr << "Error register client" << std::endl;
		}

		// read
		int retcode;
		MCSClient *client = (MCSClient*) startClient(
			&retcode,
			wpnConfig.subscriptions,
			wpnConfig.wpnKeys->getPrivateKey(),
			wpnConfig.wpnKeys->getAuthSecret(),
			wpnConfig.androidCredentials->getAndroidId(),
			wpnConfig.androidCredentials->getSecurityToken(),
			onNotify, &wpnConfig,
			onLog, &wpnConfig,
			verbosity
		);
		std::cerr << PROMPT_STARTED << std::endl;
		// loop
		readCommand(&quitFlag, std::cin, client);
		stopClient(client);
		// Save last persistent ids
		wpnConfig.save();
	} while (quitFlag == 2);

	return 0;
}
