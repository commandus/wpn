#include "subscription-load.h"
#include "errlist.h"

/**
 * Load subscription if not fouind in config file
 * @return 0- success (no changes made) 1- success (changed) <0- error
 */
static int loadMissedSubscriptionFromRegistry
(
	Subscriptions *subscriptions,
	RegistryClient *rclient,
	const uint64_t id
)
{
	Subscription *s = subscriptions->getById(id);

	// get from service
	if (!s) {
		std::string v;
		if (rclient->get(id, &v)) {
			s = subscriptions->getById(id);
		}
	}

	int r = ERR_CONNECTION;
	// check token
	if (s) {
		r = 0;
		if (!s->hasToken()) {
			// Make subscription
			r = 1;
			if (s->getSentToken().empty()) {
				if (!rclient->subscribeById(id)) {
					std::cerr << "Error " << rclient->errorCode << ": "
					<< rclient->errorDescription << ". Can not subscribe to " << id << "." << std::endl;
					return ERR_SUBSCRIPTION_TOKEN_NOT_FOUND;
				}
			}
			if (s->getSentToken().empty()) {
				return ERR_SUBSCRIPTION_TOKEN_NOT_FOUND;
			}
			// Send subscription (sentToken) to the service
			if (!rclient->addSubscription(id)) {
				std::cerr << "Error: can not register subscription to " << id << "." << std::endl;
				return ERR_REGISTER_SUBSCRIPTION;
			}
			// try get subscription from the service
			int r = rclient->getSubscription(id);
			if (r) {
				std::cerr << "Error " << r << ": can not get subscription " << id << "." << std::endl;
				return ERR_REGISTER_SUBSCRIPTION;
			}
		}
	}
	return r;
}

/**
 * Check does subscription identifiers is in configuration file, load it if not
 * @return count of succuessfully loaded new subscriptions from the registry
 **/
size_t loadMissedSubscriptionsFromRegistry
(
	Subscriptions *subscriptions,
	RegistryClient *rclient,
	const std::vector<std::string> &subscriptionids
)
{
	size_t c = 0;
	for (size_t i = 0; i < subscriptionids.size(); i++) {
		uint64_t id = strtoull(subscriptionids[i].c_str(), NULL, 10);
		if (loadMissedSubscriptionFromRegistry(subscriptions, rclient, id) == 1) {
			c++;
		}
	}
	return c;
}
