/**
  * wpn options
  * @file wpn-config.h
  **/

#ifndef WPN_CONFIG_H
#define WPN_CONFIG_H

#include <string>
#include <vector>

#define CMD_LISTEN				0
#define CMD_CREDENTIALS			1
#define CMD_LIST 				2
#define CMD_SUBSCRIBE 			3
#define CMD_UNSUBSCRIBE			4
#define CMD_PUSH				5

#define SUBSCRIBE_URL_COUNT		1
#define SUBSCRIBE_URL_1	 		"https://fcm.googleapis.com/fcm/connect/subscribe"

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
	int cmd;										///< CMD_*
	int verbosity;									///< 0-quiet, 3- debug
	std::string file_name;							///< config file, e.g. https://sure-phone.firebaseio.com"
	std::string subscribeUrl;						///< e.g. https://fcm.googleapis.com/fcm/connect/subscribe or 1 
	std::string endpoint;							///<subscription entity
	std::string authorizedEntity;					///<subscription entity, sender Id
	
	// send options
	std::string serverKey;							///< FCM server key
	std::vector<std::string> recipientTokens;		///< recipient's FCM tokens
	std::string recipientTokenFile;					///< file name or http[s] link, e.g. https://ikfia.wpn.commandus.com/app/token?accesskey=2117177
	std::string subject;							///< subject
	std::string body;								///< message body
	std::string icon;
	std::string link;
	
	WpnConfig();
	WpnConfig
	(
		int argc,
		char* argv[]
	);
	int error();
};

#endif
