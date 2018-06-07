#ifndef WP_SEND_H
#define WP_SEND_H

#define ERR_PARAM_SERVER_KEY		-1
#define ERR_PARAM_CLIENT_TOKEN		-2
#define ERR_PARSE_RESPONSE			-3

/**
* Push raw JSON to device
* @return 200-299- success, <0- error
*/
int push2ClientJSON
(
	std::string *retval,
	const std::string &server_key,
	const std::string &client_token,
	const std::string &json
);

/**
* Push notification to device
* @return 200-299- success, <0- error
*/
int push2ClientNotification
(
	std::string *retval,
	const std::string &server_key,
	const std::string &client_token,
	const std::string &title,
	const std::string &body,
	const std::string &icon,
	const std::string &click_action
);

/**
* Push "command output" to device
* @param server_key FCM subscription server key
* @param client_token FCM token
* @param persistent_id reference to request. If empty, it is request, otherwise response
* @param command command line
* @param code execution return code, usually 0
* @param output result from stdout
* @return 200-299- success, <0- error
*/
int push2ClientData
(
	std::string *retval,
	const std::string &server_key,
	const std::string &client_token,
	const std::string &persistent_id,
	const std::string &command,
	int code,
	const std::string &output
);

#endif
