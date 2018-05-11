/**
 * FireBase subscription
 * Helper utilities
 */
#ifndef WP_SUBSCRIBE_H
#define WP_SUBSCRIBE_H

#define SUBSCRIBE_FIREBASE			1

#define ERR_MODE					-1
#define ERR_PARAM_ENDPOINT			-2
#define ERR_PARAM_AUTH_ENTITY		-3
#define ERR_CONNECTION				-4

#include <string>
#include "wp-storage-file.h"

/**
 * Make subscription
 * @param subscription return value
 * @param subscribeMode always 1
 * @param wpnKeys reserved
 * @param subscribeUrl URL e.g. https://fcm.googleapis.com/fcm/connect/subscribe
 * @param endPoint https URL e.g. https://sure-phone.firebaseio.com
 * @param serverKey optional key used to send messages
 * @param authorizedEntity usual decimal number string
 * @param retVal can be NULL
 * @param retHeaders can be NULL
 * @param verbosity default 0- none
 * @return 200-299 - OK (HTTP code), less than 0- fatal error (see ERR_*)
 */
int subscribe
(
	Subscription &subscription, 
	int subscribeMode, 
	const WpnKeys &wpnKeys, 
	const std::string &subscribeUrl,
	const std::string &endPoint,
	const std::string &authorizedEntity,
	const std::string &serverKey,
	std::string *retVal,
	std::string *retHeaders,
	int verbosity
);

#endif
