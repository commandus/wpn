/**
 * Web push connection
 * Helper utilities
 */
#ifndef WP_CONNECTION_H
#define WP_CONNECTION_H	1

#include <string>
#include "wp-storage-file.h"

/**
 * I dont remember what is it
 * Connect to the push service and process push messages
 * @param wpnKeys keys
 * @param subscription connection properties
 * @param errorDescription return error explanation if not NULL
 * @param stop stopping flag
 * @param verbosity default 0- none
 * @return 200-299 - OK (HTTP code), less than 0- fatal error
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
