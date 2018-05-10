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
