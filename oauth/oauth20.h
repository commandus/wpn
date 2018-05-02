/**
 * OAuth 2.0 authentication for requests via Bearer tokens in the Authorization header according to RFC 6749, RFC 6750.
 * Create an OAuth20Credentials object with the bearer token
 */
#ifndef OAUTH20_H
#define OAUTH20_H

#include <string>
#include "oauth.h"
#include "tokenpair.h"

class OAuth20Credentials
{
private:
	enum AUTH_PROVIDER provider;
	TokenPair tokenPair;
	std::string key;	// project key
	std::string identifier;
	std::string continueUri;
protected:
	std::string readRequest();
	std::string endpoint();
public:
	OAuth20Credentials
	(
		enum AUTH_PROVIDER provider, 
		const std::string &projectKey,
  		const std::string &identifier,
  		const std::string &continueUri
	);
	~OAuth20Credentials();
	const std::string &getBearerToken() const;
	int authenticate();
	std::string toString();
};

#endif
