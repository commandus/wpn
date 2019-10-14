#include <inttypes.h>
#include "wp-registry.h"

/**
 * Check does subscription identifiers is in configuration file, load it if not
 * @return count of succuessfully loaded new subscriptions from the registry
 **/
size_t loadMissedSubscriptionsFromRegistry
(
	Subscriptions *subscriptions,
	RegistryClient *rclient,
	const std::vector<std::string> &subscriptionids
);
