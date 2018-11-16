#include "endpoint.h"

/**
 * VAPID Endpoint
 * @see https://github.com/web-push-libs/webpush-java/wiki/Endpoints
 */
std::string endpoint(
	const std::string &registrationId,			///< GCMToken
	const bool send,
	const int browser							///< 0- Chrome, 1- Firefox
)
{
	switch (browser) {
	case 1:
		// Firefox VAPID
		return "https://updates.push.services.mozilla.com/wpush/v2/" + registrationId;
	default:
		// Chrome VAPID
		if (send)
			return "https://fcm.googleapis.com/wp/" + registrationId;
		else
			return "https://fcm.googleapis.com/fcm/send/" + registrationId;

	}
}
