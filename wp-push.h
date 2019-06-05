#ifndef WP_PUSH_H
#define WP_PUSH_H	1

#include <string>

#define ERR_PARAM_SERVER_KEY		-1
#define ERR_PARAM_CLIENT_TOKEN		-2
#define ERR_PARSE_RESPONSE			-3

/**
 * FCM web push utilities
 */

/**
* Push serialized JSON string to device FCM
* @param retval return error description if not null
* @param value JSON serialized message.  See push2ClientNotificationFCM()
* @return 200-299- success, negative- error code
* @see push2ClientNotificationFCM()
*/
int push2ClientJSON
(
	std::string *retval,
	const std::string &server_key,
	const std::string &client_token,
	const std::string &json,
	int verbosity
);

/**
* Push notification to device FCM
* @return 200-299- success, negative- error code
*/
int push2ClientNotificationFCM
(
	std::string *retval,
	const std::string &server_key,
	const std::string &client_token,
	const std::string &title,
	const std::string &body,
	const std::string &icon,
	const std::string &click_action,
	int verbosity
);

/**
* Push "command output" to device
* @param server_key FCM subscription server key
* @param token FCM recipient token
* @param client_token FCM token
* @param persistent_id reference to request
* @param command command line
* @param code execution return code, usually 0
* @param output result from stdout
* @return 200-299- success, nefative- error code
* @see push2ClientJSON()
*/
int push2ClientDataFCM
(
	std::string *retval,
	const std::string &server_key,
	const std::string &token,
	const std::string &client_token,
	const std::string &persistent_id,
	const std::string &command,
	int code,
	const std::string &output,
	int verbosity
);

#endif
