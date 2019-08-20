#include <string>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

#include "notify2string.h"

std::string notify2json(
    uint64_t id,
	const char *name,
   	const char *persistent_id,
	const char *from,				///< e.g. BDOU99-h67HcA6JeFXHbSNMu7e2yNNu3RzoMj8TM4W88jITfq7ZmPvIM1Iv-4_l2LxQcYwhqby2xGpWwzjfAnG4
	const char *appName,
	const char *appId,
	int64_t sent,
    const NotifyMessageC *notify
);

std::string jsAddSubscription(
    const std::string &publicKey,
    const std::string &authSecret,
    const std::string &token
);

bool jsGetSubscription(
    const std::string &json,
    std::string &retPublicKey,
    std::string &retToken,
    std::string &retAuthSecret
);

bool jsValid (
    const std::string &js
);

std::string jsClientNotification
(
    const std::string &client_token,
    const std::string &title, 
    const std::string &body,
    const std::string &icon,
    const std::string &click_action
);

std::string jsClientCommand
(
    const std::string &client_token,
    const std::string &command,
    const std::string &persistent_id, 
    int code,
    const std::string &output,
    const std::string &server_key,
    const std::string &token
);

/**
 * Parse notification file
 * @return 0- success, -1: Invalid JSON, -2: Important information missed
 */
int parseNotificationJson
(
	const std::string &value,
	std::string &to,
	std::string &title,
	std::string &body,
	std::string &icon, 
	std::string &click_action
);

/**
 * Parse subscription file
 * @return 0- success, -1: Invalid JSON, -2: Important information missed
 */
int parseSubscriptionJson
(
	const std::string &value,
	std::string &publicKey,
	std::string &privateKey,
	std::string &endpoint,
	std::string &p256dh,
	std::string &auth,
	std::string &contact
);

std::string jsSubscribeFCM(
    const std::string &authorizedEntity,
	const std::string &endPoint,
	const std::string &receiverAndroidId,
	const std::string &receiverSecurityToken
);

bool jsSubscribeFCMParseResponse(
    const std::string &value,
    std::string &retToken,
    std::string &retPushSet
);

std::string jsSubscribeFCMParseErrorResponse(
    const std::string &value
);
