/**
 * FCM subscription helper functions
 */
#ifndef WP_SUBSCRIBE_H_
#define WP_SUBSCRIBE_H_	1

#define ERR_MODE					-1
#define ERR_PARAM_ENDPOINT			-2
#define ERR_PARAM_AUTH_ENTITY		-3
#define ERR_CONNECTION				-4

#include <string>
#include "wp-storage-file.h"

/**
 * Make subscription VAPID
 * @param retVal can be NULL
 * @param retHeaders can be NULL
 * @param retToken return subscription token
 * @param retPushSet return subscription push set. Not implemented. Returns empty string
 * @param receiverPublicKey receiver public key
 * @param receiverAuth receiver auth secret
 * @param receiverAppId application identifier 
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
	const std::string &receiverAppId,
	const std::string &authorizedEntity,
	int verbosity
);

/**
 * Unsubscribe VAPID
 * @param retVal can be NULL
 * @param retHeaders can be NULL
 * @param retToken return subscription token
 * @param retPushSet return subscription push set. Not implemented. Returns empty string
 * @param receiverPublicKey receiver public key
 * @param receiverAuth receiver auth secret
 * @param receiverAppId application identifier 
 * @param verbosity default 0- none
 * @return 200-299 - OK (HTTP code), less than 0- fatal error (see ERR_*)
 **/
int unsubscribe
(
	std::string *retVal,
	std::string *retHeaders,
	std::string &retToken,
	std::string &retPushSet,
	const std::string &receiverPublicKey,
	const std::string &receiverAuth,
	const std::string &receiverAppId,
	const std::string &authorizedEntity,
	int verbosity
);

/**
 * Make subscription FCM
 * authorizedEntity MUST BE "103953800507"
 * @see https://firebase.google.com/docs/cloud-messaging/js/client
 * @param retVal can be NULL
 * @param retHeaders can be NULL
 * @param retToken return subscription token
 * @param retPushSet return subscription push set
 * @param receiverAndroidId receiver Android id
 * @param receiverSecurityToken receiver security number
 * @param wpnKeys reserved
 * @param subscribeUrl URL e.g. https://fcm.googleapis.com/fcm/connect/subscribe
 * @param endPoint https URL e.g. https://sure-phone.firebaseio.com
 * @param authorizedEntity usual decimal number string "103953800507"
 * @param verbosity default 0- none
 * @return 200-299 - OK (HTTP code), less than 0- fatal error (see ERR_*)
 */
int subscribeFCM
(
	std::string *retVal,
	std::string *retHeaders,
	std::string &retToken,
	std::string &retPushSet,
	const std::string &receiverAndroidId,
	const std::string &receiverSecurityToken,
	const std::string &subscribeUrl,
	const std::string &endPoint,
	const std::string &authorizedEntity,
	int verbosity
);

#endif
