#include <inttypes.h>
#include <fstream>
#include <sstream>
#include <nghttp2/nghttp2.h>
#include "wp-connection.h"

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
	int verbosity
)
{
	int r = 0;
	return r;
}
