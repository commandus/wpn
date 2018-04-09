/**
  * wpn options
  * @file wpn-config.h
  **/

#ifndef WPN_CONFIG_H
#define WPN_CONFIG_H

#include <string>
#include <vector>

#define CMD_RW				0
#define CMD_PRINT_TOX_ID	1

/**
 * Command line interface (CLI) tool configuration structure
 */
class WpnConfig
{
private:
	/**
	* Parse command line into WpnConfig class
	* Return 0- success
	*        1- show help and exit, or command syntax error
	*        2- output file does not exists or can not open to write
	**/
	int parseCmd
	(
		int argc,
		char* argv[]
	);
	int errorcode;
public:
	int cmd;
	std::string file_name;							///< config file
	WpnConfig();
	WpnConfig
	(
		int argc,
		char* argv[]
	);
	int error();
};

#endif
