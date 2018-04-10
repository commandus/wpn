#include "wpn-config.h"
#include <iostream>
#include <argtable2.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#define DEF_FILE_NAME			".wpn"

static const char* progname = "wpn";

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
	: cmd(CMD_SUBSCRIBE), file_name(getDefaultConfigFileName()), endpoint(""), authorized_entity("")
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
	// GTFS https://developers.google.com/transit/gtfs/reference/?csw=1
	struct arg_lit *a_subscribe = arg_lit0("s", "subscribe", "Subscribe with -u URL -p SENDER_ID");
	struct arg_str *a_file_name = arg_str0("f", "file", "<file>", "Tox configuration file. Default ~/" DEF_FILE_NAME);
	struct arg_str *a_endpoint = arg_str0("u", "endpoint", "<URL>", "Push message originator URL, like https://*.firebaseio.com");
	struct arg_str *a_authorized_entity = arg_str0("p", "entity", "<identifier>", "Push message sender identifier, usually decimal number");

	struct arg_lit *a_help = arg_lit0("h", "help", "Show this help");
	struct arg_end *a_end = arg_end(20);

	void* argtable[] = { 
		a_subscribe, 
		a_endpoint,
		a_authorized_entity,
		a_file_name,
		a_help, a_end 
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

	cmd = CMD_LISTEN;
	if (a_subscribe->count)
		cmd = CMD_SUBSCRIBE;

	if (a_file_name->count)
		file_name = *a_file_name->sval;
	else
		file_name = getDefaultConfigFileName();

	// special case: '--help' takes precedence over error reporting
	if ((a_help->count) || nerrors)
	{
		if (nerrors)
			arg_print_errors(stderr, a_end, progname);
		std::cerr << "Usage: " << progname << std::endl;
		arg_print_syntax(stderr, argtable, "\n");
		std::cerr << "Simplest Tox CLI client" << std::endl;
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
