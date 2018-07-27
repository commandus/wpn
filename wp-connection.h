/**
 * Web push connection
 * Helper utilities
 */
#ifndef WP_CONNECTION_H
#define WP_CONNECTION_H

#define ERR_MODE					-1
#define ERR_PARAM_ENDPOINT			-2
#define ERR_PARAM_AUTH_ENTITY		-3
#define ERR_CONNECTION				-4

#include <string>
#include "wp-storage-file.h"

/**
 * Connect to the push service and process push messages
 * @param wpnKeys keys
 * @param subscription connection properties
 * @param errorDescription return error explanation if not NULL
 * @param stop stopping flag
 * @param verbosity default 0- none
 * @return 200-299 - OK (HTTP code), less than 0- fatal error (see ERR_*)
 */
int connectionRun
(
	const WpnKeys &wpnKeys, 
	const Subscription &subscription, 
	std::string *errorDescription,
	bool *stop,
	int verbosity = 0
);

#endif
