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
int push2ClientNotificationFCM
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
* Push notification to device
* @param endpoint endpoint
* @param privateKey PK
* @param publicKey public key
* @param authSecret secret
* @return 200-299- success, <0- error
*/
int push2ClientNotificationVAPID
(
	std::string *retval,
	const std::string &endpoint,
	const std::string &privateKey,
	const std::string &publicKey,
	const std::string &aud,
	const std::string &sub,
	const std::string &title,
	const std::string &body,
	const std::string &icon,
	const std::string &click_action
);

/**
* Push "command output" to device
* @param server_key FCM subscription server key
* @param token FCM recipient token
* @param client_token FCM token
* @param persistent_id reference to request. If empty, it is request, otherwise response
* @param command command line
* @param code execution return code, usually 0
* @param output result from stdout
* @return 200-299- success, <0- error
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
	const std::string &output
);

/**
* Push "command output" to device
* @param endpoint endpoint
* @param privateKey PK
* @param publicKey public key
* @param authSecret secret
* @param persistent_id reference to request. If empty, it is request, otherwise response
* @param command command line
* @param code execution return code, usually 0
* @param output result from stdout
* @return 200-299- success, <0- error
*/
int push2ClientDataVAPID
(
	std::string *retval,
	const std::string &endpoint,
	const std::string &privateKey,
	const std::string &publicKey,
	const std::string &authSecret,
	const std::string &persistent_id,
	const std::string &command,
	int code,
	const std::string &output
);

#endif
