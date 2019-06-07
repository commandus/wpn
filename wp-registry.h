#ifndef WP_REGISTRY_H
#define WP_REGISTRY_H	1

#include <string>

#include "wp-storage-file.h"

/**
 * Web push subscription registration service client
 */

class RegistryClient
{
private:
/**
* Remote call
* @param retval return value
* @param method "POST", "GET", "DELETE"
* @param path "key", "subscription"
* @param value JSON serialized message.  See push2ClientNotificationFCM()
* @return true- success
*/
bool rpc
(
	std::string *retval,
	const std::string &method,
	const std::string &path,
	uint64_t id,
	const std::string &value,
	bool debug
);
public:
	ConfigFile *config;
	int errorCode;
	std::string errorDescription;

	RegistryClient(
		ConfigFile *config
	);
	~RegistryClient();

	bool add(
		uint64_t *retval
	);

	bool get(
		uint64_t id,
		std::string *retVal
	);

	bool rm();

	bool addSubscription(
		uint64_t id2
	);

	int getSubscription(
		std::string &retval,
		uint64_t id2
	);

	int rmSubscription(
		std::string &retval,
		uint64_t id2
	);

	bool subscribeById(
		uint64_t id
	);

};

#endif
