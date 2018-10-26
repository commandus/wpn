/**
  * wpn options
  * @file wpn-config.h
  **/

#ifndef WPN_CONFIG_H
#define WPN_CONFIG_H	1

#include <string>
#include <vector>
#include "nlohmann/json.hpp"
#include "wp-storage-file.h"
#include "wpn-notify.h"

using json = nlohmann::json;

#define CMD_LISTEN				0
#define CMD_KEYS				1
#define CMD_CREDENTIALS			2
#define CMD_LIST 				3
#define CMD_LIST_QRCODE			4
#define CMD_LIST_LINK			5
#define CMD_LIST_EMAIL			6
#define CMD_SUBSCRIBE_FCM		7
#define CMD_SUBSCRIBE_VAPID		8
#define CMD_UNSUBSCRIBE			9
#define CMD_PUSH				10
#define CMD_GENERATE_VAPID_KEYS	11

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
	bool aesgcm;									///< VAPID: use AESGCM. Default AES128GCM
	std::string file_name;							///< config file, e.g. https://sure-phone.firebaseio.com"
	std::string name;								///< subscription name
	std::string subscribeUrl;						///< default https://fcm.googleapis.com/fcm/connect/subscribe
	std::string fcm_endpoint;						///<subscription entity
	std::string authorizedEntity;					///<subscription entity, sender Id

	int subscriptionMode;
	// send options: key, recipient's FCM tokens
	std::string serverKey;							///< FCM server key
	std::vector<std::string> recipientTokens;		///< recipient's FCM tokens
	std::string recipientTokenFile;					///< file name or http[s] link, e.g. https://ikfia.wpn.commandus.com/app/token?accesskey=2117177

	std::string vapid_sender_contact;
	std::string vapid_recipient_p256dh;
	std::string vapid_recipient_auth;

	// VAPID keys overrides 
	std::string private_key;
	std::string public_key;
	std::string auth_secret;
	std::string sub;
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

	AndroidCredentials *androidCredentials;
	WpnKeys *wpnKeys;
	Subscriptions *subscriptions;
	std::istream::pos_type read(const std::string &fileName);
	std::ostream::pos_type write() const;

	size_t loadNotifyFuncs();
	void unloadNotifyFuncs();
	std::vector <std::string> notifyLibFileNames;
	std::vector <SO_INSTANCE> notifyLibs;
	std::vector <OnNotifyC> onNotifyList;

	bool setPersistentId(const std::string &authorizedEntity, const std::string &persistent_id);
	void getPersistentIds(std::vector<std::string> &retval);

	/**
	* Get subscription
	* @param subscriptionName subscription name
	* @return server key from subscription by the name of subscription
	*/
	const Subscription *getSubscription(const std::string &subscriptionName) const;
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
	json toJson() const;
	bool fromJson(const json &value);
	bool save() const;
	
	std::string getDefaultFCMEndPoint();
};

#endif
