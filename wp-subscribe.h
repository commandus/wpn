/**
 * FCM subscription helper functions
 */
#ifndef WP_SUBSCRIBE_H
#define WP_SUBSCRIBE_H

#define ERR_MODE					-1
#define ERR_PARAM_ENDPOINT			-2
#define ERR_PARAM_AUTH_ENTITY		-3
#define ERR_CONNECTION				-4

#include <string>
#include "wp-storage-file.h"

/**
 * Make subscription
 * @param retVal can be NULL
 * @param retHeaders can be NULL
 * @param retToken return subscription token
 * @param retPushSet return subscription push set
 * @param receiverPublicKey receiver public key
 * @param receiverAuth receiver auth secret
 * @param subscribeUrl URL e.g. https://fcm.googleapis.com/fcm/send/[GCM_TOKEN]
 * @param endPoint https URL e.g. https://sure-phone.firebaseio.com
 * @param verbosity default 0- none
 * @return 200-299 - OK (HTTP code), less than 0- fatal error (see ERR_*)
 */
int subscribe
(
	std::string *retVal,
	std::string *retHeaders,
	std::string &retToken,
	std::string &retPushSet,
	const std::string &receiverPublicKey,
	const std::string &receiverAuth,
	const std::string &subscribeUrl,
	const std::string &endPoint,
	const std::string &authorizedEntity,
	int verbosity
);

#endif
