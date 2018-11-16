#ifndef ENDPOINT_H
#define ENDPOINT_H 1

#include <string>

/**
 * VAPID Endpoint
 * @see https://github.com/web-push-libs/webpush-java/wiki/Endpoints
 */
std::string endpoint(
	const std::string &registrationId,			///< GCMToken
	const bool send = false,
	const int browser = 0						///< 0- Chrome, 1- Firefox
);

#endif
