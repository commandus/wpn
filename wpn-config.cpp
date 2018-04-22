#include "wpn-config.h"
#include <iostream>
#include <argtable2.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#define DEF_FILE_NAME			".wpn"

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

WpnConfig::WpnConfig()
	: cmd(CMD_LISTEN), verbosity(0), file_name(getDefaultConfigFileName()), endpoint(""), authorizedEntity("")
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
	struct arg_lit *a_credentials = arg_lit0("c", "credentials", "Print credentials");
	struct arg_lit *a_subscribe = arg_lit0("s", "subscribe", "Subscribe with -a -p -i");
	struct arg_lit *a_unsubscribe = arg_lit0("u", "unsubscribe", "Unsubscribe with -p -i");
	struct arg_lit *a_send = arg_lit0("m", "message", "Send message with -p -i");
	
	struct arg_str *a_file_name = arg_str0("f", "file", "<file>", "Configuration file. Default ~/" DEF_FILE_NAME);
	
	struct arg_str *a_subscribe_url = arg_str0("a", "registrar", "<URL>", "Subscription registrar URL, like https://fcm.googleapis.com/fcm/connect/subscribe or 1");
	struct arg_str *a_endpoint = arg_str0("p", "pushsvc", "<URL>", "Push service URL, like https://*.firebaseio.com");
	struct arg_str *a_authorized_entity = arg_str0("i", "entity", "<identifier>", "Push message sender identifier, usually decimal number");

	// send options
	struct arg_str *a_server_key = arg_str0("k", "key", "<server key>", "Server key to send");
	struct arg_str *a_subject = arg_str0("s", "subject", "<Text>", "Subject");
	struct arg_str *a_body = arg_str0("b", "body", "<Text>", "Body");
	struct arg_str *a_icon = arg_str0("p", "icon", "<URI>", "http[s]:// icon address.");
	struct arg_str *a_link = arg_str0("l", "link", "<URI>", "https:// action address.");
	struct arg_str *a_recipient_tokens = arg_strn(NULL, NULL, "<account#>", 0, 100, "Recipient token.");

	struct arg_lit *a_verbosity = arg_litn("v", "verbose", 0, 3, "0- quiet (default), 1- errors, 2- warnings, 3- debug");
	struct arg_lit *a_help = arg_lit0("h", "help", "Show this help");
	struct arg_end *a_end = arg_end(20);

	void* argtable[] = { 
		a_list, a_credentials, a_subscribe, a_unsubscribe, a_send,
		a_subscribe_url, a_endpoint, a_authorized_entity,
		a_file_name,
		a_subject, a_body, a_icon, a_link, a_recipient_tokens,
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

	if (a_endpoint->count)
		endpoint = *a_endpoint->sval;
	else
		endpoint = "";

	if (a_authorized_entity->count)
		authorizedEntity = *a_authorized_entity->sval;
	else
		authorizedEntity = "";

	cmd = CMD_LISTEN;
	if (a_list->count)
		cmd = CMD_LIST;
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
		if (a_recipient_tokens->count == 0)
		{
			std::cerr << "No recipient(s)." << std::endl;
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
		if (endpoint.empty() || authorizedEntity.empty()) 
		{
			std::cerr << "Missing -p, -i options." << std::endl;
			nerrors++;
		}
	}
	
	if (a_file_name->count)
		file_name = *a_file_name->sval;
	else
		file_name = getDefaultConfigFileName();

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
