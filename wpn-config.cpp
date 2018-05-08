#include "wpn-config.h"
#include <iostream>
#include <argtable2.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <fstream>
#include <dlfcn.h>

#include "platform.h"
#include "utilstring.h"
#include "nlohmann/json.hpp"

#define DEF_FILE_NAME			".wpn"
#define DEF_FCM_ENDPOINT_PREFIX	"https://fcm.googleapis.com/fcm/send/"
#define DEF_OUTPUT_SO_FN		"libwpn-stdout.so"
#define DEF_FUNC_NOTIFY			"desktopNotify"

#ifdef _MSC_VER
#define DEF_PLUGIN_FILE_EXT		".dll"
#else
#define DEF_PLUGIN_FILE_EXT		".so"
#endif

using json = nlohmann::json;

static const char* progname = "wpn";

static const char* SUBSCRIBE_URLS[SUBSCRIBE_URL_COUNT] = {
	SUBSCRIBE_URL_1
};

/**
 * https://stackoverflow.com/questions/2910377/get-home-directory-in-linux-c
 */
static std::string getDefaultConfigFileName()
{
	struct passwd *pw = getpwuid(getuid());
	const char *homedir = pw->pw_dir;
	std::string r(homedir);
	return r + "/" + DEF_FILE_NAME;
}

static int parseJsonRecipientTokens
(
	std::vector<std::string> &retval,
	const std::string &value
)
{
	int c = 0;
	try
	{
		json list = json::parse(value);
		// iterate the array
		for (json::iterator it(list.begin()); it != list.end(); ++it) 
		{
			json a = *it;
			retval.push_back(a[1]);
			c++;
		}
	}
	catch(...)
	{
	}
	return c;
}

std::string WpnConfig::getDefaultEndPoint()
{
	std::string r(DEF_FCM_ENDPOINT_PREFIX);
	if (androidCredentials)
		r = r + androidCredentials->getGCMToken();
	return r;
}

WpnConfig::WpnConfig()
	: cmd(CMD_LISTEN), verbosity(0), file_name(getDefaultConfigFileName()), endpoint(""), authorizedEntity(""), notifyFunctionName(DEF_FUNC_NOTIFY)
{
}

WpnConfig::WpnConfig
(
	int argc,
	char* argv[]
)
{
	errorcode = parseCmd(argc, argv);
}

WpnConfig::~WpnConfig()
{
	if (androidCredentials)
		delete androidCredentials;
	if (wpnKeys)
		delete wpnKeys;
	if (subscriptions)
		delete subscriptions;
}

/**
 * Parse command line into WpnConfig class
 * Return 0- success
 *        1- show help and exit, or command syntax error
 *        2- output file does not exists or can not open to write
 **/
int WpnConfig::parseCmd
(
	int argc,
	char* argv[]
)
{
	struct arg_lit *a_list = arg_lit0("l", "list", "List subscriptions");
	struct arg_lit *a_keys = arg_lit0("y", "keys", "Print keys");
	struct arg_lit *a_credentials = arg_lit0("c", "credentials", "Print credentials");
	struct arg_lit *a_subscribe = arg_lit0("s", "subscribe", "Subscribe with -e)");
	struct arg_lit *a_unsubscribe = arg_lit0("d", "unsubscribe", "Unsubscribe with -e");
	struct arg_lit *a_send = arg_lit0("m", "message", "Send message with -k, -e, -t, -b, -i, -a");
	struct arg_str *a_file_name = arg_str0("f", "file", "<file>", "Configuration file. Default ~/" DEF_FILE_NAME);
	
	struct arg_str *a_subscribe_url = arg_str0("r", "registrar", "<URL>", "Subscription registrar URL, like https://fcm.googleapis.com/fcm/connect/subscribe or 1. Default 1");
	struct arg_str *a_endpoint = arg_str0("u", "endpoint", "<URL>", "Push service endpoint URL prefix.");
	struct arg_str *a_authorized_entity = arg_str0("e", "entity", "<entity-id>", "Push message sender identifier, usually decimal number");

	// send options
	struct arg_str *a_server_key = arg_str0("k", "key", "<server key>", "Server key to send");
	struct arg_str *a_subject = arg_str0("t", "subject", "<Text>", "Subject (topic)");
	struct arg_str *a_body = arg_str0("b", "body", "<Text>", "Body");
	struct arg_str *a_icon = arg_str0("i", "icon", "<URI>", "http[s]:// icon address.");
	struct arg_str *a_link = arg_str0("a", "link", "<URI>", "https:// action address.");
	struct arg_str *a_recipient_tokens = arg_strn(NULL, NULL, "<account#>", 0, 100, "Recipient token.");
	struct arg_str *a_recipient_token_file = arg_str0("J", "json", "<file name or URL>", "JSON file e.g. [[1,\"token\",..");
	struct arg_str *a_output = arg_str0("o", "format", "<text|json>", "Output format. Default text.");

	// output options
	struct arg_str *a_output_lib_filenames = arg_strn("O", "output", "<file name>", 0, 100, "Output shared library file name or directory");
	struct arg_str *a_notify_function_name = arg_str0("F", "output-func", "<name>", "Output function name. Default " DEF_FUNC_NOTIFY);
	
	struct arg_lit *a_verbosity = arg_litn("v", "verbose", 0, 4, "0- quiet (default), 1- errors, 2- warnings, 3- debug, 4- debug libs");
	struct arg_lit *a_help = arg_lit0("h", "help", "Show this help");
	struct arg_end *a_end = arg_end(20);

	void* argtable[] = { 
		a_list, a_credentials, a_keys, a_subscribe, a_unsubscribe, a_send,
		a_subscribe_url, a_endpoint, a_authorized_entity,
		a_file_name,
		a_server_key, a_subject, a_body, a_icon, a_link, a_recipient_tokens, a_recipient_token_file,
		a_output,
		a_output_lib_filenames, a_notify_function_name,
		a_verbosity, a_help, a_end 
	};

	int nerrors;

	// verify the argtable[] entries were allocated successfully
	if (arg_nullcheck(argtable) != 0)
	{
		arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
		return 1;
	}
	// Parse the command line as defined by argtable[]
	nerrors = arg_parse(argc, argv, argtable);

	if (a_file_name->count)
		file_name = *a_file_name->sval;
	else
		file_name = getDefaultConfigFileName();

	// read
	std::ifstream configRead(file_name.c_str());
	androidCredentials = new AndroidCredentials(configRead);
	wpnKeys = new WpnKeys(configRead);
	subscriptions = new Subscriptions (configRead);
	configRead.close();

	if (a_subscribe_url->count)
		subscribeUrl = *a_subscribe_url->sval;
	else
		subscribeUrl = "";
	int m = strtol(subscribeUrl.c_str(), NULL, 10);
	if ((m > 0) && (m <= SUBSCRIBE_URL_COUNT))
	{
		m--;
		subscribeUrl = SUBSCRIBE_URLS[m];
	}
	if (subscribeUrl.empty())
		subscribeUrl = SUBSCRIBE_URLS[0];

	if (a_endpoint->count)
		endpoint = *a_endpoint->sval;
	else
		endpoint = getDefaultEndPoint();

	if (a_authorized_entity->count)
		authorizedEntity = *a_authorized_entity->sval;
	else
		authorizedEntity = "";

	cmd = CMD_LISTEN;
	if (a_list->count)
		cmd = CMD_LIST;
	else
		if (a_keys->count)
			cmd = CMD_KEYS;
		else
			if (a_credentials->count)
				cmd = CMD_CREDENTIALS;
			else
				if (a_subscribe->count)
					cmd = CMD_SUBSCRIBE;
				else
					if (a_unsubscribe->count)
						cmd = CMD_UNSUBSCRIBE;
					else
						if (a_send->count)
							cmd = CMD_PUSH;

	if (a_notify_function_name->count)
		notifyFunctionName = *a_notify_function_name->sval;
	else
		notifyFunctionName = DEF_FUNC_NOTIFY;
	if (a_output_lib_filenames->count)
	{
		for (int i = 0; i < a_output_lib_filenames->count; i++)
		{
			notifyLibFileNames.push_back(a_output_lib_filenames->sval[i]);
		}
	}
	else
	{
		notifyLibFileNames.push_back(DEF_OUTPUT_SO_FN);
	}
	
	if (cmd == CMD_PUSH)
	{
		if (a_server_key->count == 0)
		{
			std::cerr << "-k missed." << std::endl;
			nerrors++;
		}
		if (a_subject->count == 0)
		{
			std::cerr << "-s missed." << std::endl;
			nerrors++;
		}
		if (a_body->count == 0)
		{
			std::cerr << "-b missed." << std::endl;
			nerrors++;
		}
		if (a_icon->count == 0)
		{
			std::cerr << "-i missed." << std::endl;
			nerrors++;
		}
		if (a_link->count == 0)
		{
			std::cerr << "-l missed." << std::endl;
			nerrors++;
		}
	}

	if ((cmd == CMD_SUBSCRIBE) || (cmd == CMD_UNSUBSCRIBE))
	{
		if (cmd == CMD_SUBSCRIBE)
		{
			if (subscribeUrl.empty()) 
			{
				std::cerr << "Missing -a option." << std::endl;
				nerrors++;
			}
		}
		if (authorizedEntity.empty()) 
		{
			std::cerr << "Missing -e <entity-id> option." << std::endl;
			nerrors++;
		}
	}

	if (a_server_key->count)
	{
		serverKey = *a_server_key->sval;
	}
	if (a_subject->count)
	{
		subject = *a_subject->sval;
	}
	if (a_body->count)
	{
		body = *a_body->sval;
	}
	if (a_icon->count)
	{
		icon = *a_icon->sval;
	}
	if (a_link->count)
	{
		link = *a_link->sval;
	}

	for (int i = 0; i < a_recipient_tokens->count; i++)
	{
		recipientTokens.push_back(a_recipient_tokens->sval[i]);
	}

	if (a_recipient_token_file->count)
	{
		std::string fn(*a_recipient_token_file->sval);
		std::string data = file2string(fn);
		if (data.empty())
		{
			// try load from the Internet;
			data = url2string(fn);
		}
		if (!data.empty()) {
			parseJsonRecipientTokens(recipientTokens, data);
		}
	}

	outputFormat = 0;	// 0- text, 1- json
	if (a_output->count)
	{
		if (strcmp("json", *a_output->sval) == 0)
			outputFormat = 1;
	}

	verbosity = a_verbosity->count;
	
	// special case: '--help' takes precedence over error reporting
	if ((a_help->count) || nerrors)
	{
		if (nerrors)
			arg_print_errors(stderr, a_end, progname);
		std::cerr << "Usage: " << progname << std::endl;
		arg_print_syntax(stderr, argtable, "\n");
		std::cerr << "Web push notification command line interface client" << std::endl;
		arg_print_glossary(stderr, argtable, "  %-25s %s\n");
		arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
		return 1;
	}

	arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
	return 0;
}

int WpnConfig::error()
{
	return errorcode;
}

int WpnConfig::write()
{
	std::ofstream configWrite(file_name);
	androidCredentials->write(configWrite);
	wpnKeys->write(configWrite);
	subscriptions->write(configWrite);
	configWrite.close();
}

size_t WpnConfig::loadDesktopNotifyFuncs()
{
	notifyLibs.clear();
	desktopNotifyFuncs.clear();
	size_t r = 0;
	for (std::vector <std::string>::const_iterator it(notifyLibFileNames.begin()); it != notifyLibFileNames.end(); ++it)
	{
		std::string rp;
		str_realpath(rp, (*it));
		
		std::vector<std::string > files;
		if (isDir(rp))
		{
			filesInPath(rp, DEF_PLUGIN_FILE_EXT, 1, &files);
		}
		else
		{
			files.push_back(rp);
		}

		for (std::vector<std::string>::const_iterator it(files.begin()); it != files.end(); ++it)
		{
			void *so = dlopen(it->c_str(), RTLD_LAZY);
			if (!so)
			{
				if (verbosity > 1)
				{
					std::cerr << "Can not open shared library file: " << *it << std::endl;
				}
				continue;
			}
			notifyLibs.push_back(so);
			desktopNotifyFunc desktopNotify = (desktopNotifyFunc) dlsym(so, notifyFunctionName.c_str());
			if (!desktopNotify)
			{
				if (verbosity > 1)
				{
					std::cerr << "Can not bind " << notifyFunctionName << "() from shared library file: " << *it << std::endl;
				}
				continue;
			}
			if (verbosity > 2)
			{
				std::cerr << "Shared library " << *it << " loaded successfully." << std::endl;
			}
			desktopNotifyFuncs.push_back(desktopNotify);
			r++;
		}
	}
	if (verbosity > 1)
	{
		std::cerr << "Shared libraries loaded: " << r << std::endl;
	}
	return r;
}

void WpnConfig::unloadDesktopNotifyFuncs()
{
	for (std::vector <void *>::const_iterator it(notifyLibs.begin()); it != notifyLibs.end(); ++it)
	{
		dlclose(*it);
	}
	notifyLibs.clear();
}

