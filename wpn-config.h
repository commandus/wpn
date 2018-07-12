/**
  * wpn options
  * @file wpn-config.h
  **/

#ifndef WPN_CONFIG_H
#define WPN_CONFIG_H

#include <string>
#include <vector>
#include "wp-storage-file.h"
#include "wpn-notify.h"

#define CMD_LISTEN				0
#define CMD_KEYS				1
#define CMD_CREDENTIALS			2
#define CMD_LIST 				3
#define CMD_LIST_QRCODE			4
#define CMD_LIST_LINK			5
#define CMD_LIST_EMAIL			6
#define CMD_SUBSCRIBE 			7
#define CMD_UNSUBSCRIBE			8
#define CMD_PUSH				9

#define SUBSCRIBE_URL_COUNT		1
#define SUBSCRIBE_URL_1	 		"https://fcm.googleapis.com/fcm/connect/subscribe"

#ifdef _MSC_VER
#include "Windows.h"
typedef HMODULE  SO_INSTANCE;
#else
typedef void * SO_INSTANCE;
#endif


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
	int outputFormat;								///< 0- text, 1- json
	int verbosity;									///< 0-quiet, 3- debug
	std::string file_name;							///< config file, e.g. https://sure-phone.firebaseio.com"
	std::string name;								///< subscription name
	std::string subscribeUrl;						///< default https://fcm.googleapis.com/fcm/connect/subscribe
	std::string endpoint;							///<subscription entity
	std::string authorizedEntity;					///<subscription entity, sender Id

	// send options: key, recipient's FCM tokens
	std::string serverKey;							///< FCM server key
	std::vector<std::string> recipientTokens;		///< recipient's FCM tokens
	std::string recipientTokenFile;					///< file name or http[s] link, e.g. https://ikfia.wpn.commandus.com/app/token?accesskey=2117177
	// send options
	std::string subject;							///< subject
	std::string body;								///< message body
	std::string icon;
	std::string link;
	std::string command;							///< command to be executed, van be empty(nothing to do)
	// output external function name
	std::string notifyFunctionName;
	bool invert_qrcode;
	std::string email_template;						/// email template. $subject $name, $body
	std::string cn;									/// email template $name value
	
	WpnConfig();
	WpnConfig
	(
		int argc,
		char* argv[]
	);
	~WpnConfig();
	int error();
	std::string getDefaultEndPoint();
	
	AndroidCredentials *androidCredentials;
	WpnKeys *wpnKeys;
	Subscriptions *subscriptions;
	int read(const std::string &fileName);
	int write() const;

	size_t loadDesktopNotifyFuncs();
	void unloadDesktopNotifyFuncs();
	std::vector <std::string> notifyLibFileNames;
	std::vector <SO_INSTANCE> notifyLibs;
	std::vector <desktopNotifyFunc> desktopNotifyFuncs;
	bool setPersistentId(const std::string &authorizedEntity, const std::string &persistent_id);
	void getPersistentIds(std::vector<std::string> &retval);
	/**
	* Get server key
	* @param subscriptionName subscription name
	* @return server key from subscription by the name of subscription
	*/
	std::string getSubscriptionServerKey(const std::string &subscriptionName) const;
	/**
	* Get FCM token
	* @param subscriptionName subscription name
	* @return FCM token from subscription by the name of subscription
	*/
	std::string getSubscriptionToken
	(
		const std::string &subscriptionName
	) const;
};

#endif
