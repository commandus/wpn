#ifndef WP_SEND_H
#define WP_SEND_H

#define ERR_PARAM_SERVER_KEY		-1
#define ERR_PARAM_CLIENT_TOKEN		-2
#define ERR_CONNECTION				-3
#define ERR_SERVER					-4

/**
* @brief Push notification to device
*/
int push2instance
(
	std::string *retval,
	const std::string &server_key,
	const std::string &client_token,
	const std::string &title,
	const std::string &body,
	const std::string &icon,
	const std::string &click_action
);

#endif