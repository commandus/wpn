#ifndef OAUTHREQUEST_H
#define OAUTHREQUEST_H

#include <string>
#include <vector>

#include "oauth.h"

class OAuthRequest
{
private:
	enum AUTH_PROVIDER provider;
	std::string identifier;
	std::string providerId;
	std::string continueUri;
	std::string scope;
	std::vector<std::pair <std::string, std::string> > customParameters;
public:
	OAuthRequest(
		enum AUTH_PROVIDER provider,
		const std::string &identifier,
		const std::string &providerId,
		const std::string &continueUri,
		const std::string &scope
	);
	OAuthRequest(const OAuthRequest& other);
	~OAuthRequest();
	OAuthRequest& operator=(const OAuthRequest& other);
	bool operator==(const OAuthRequest& other) const;
	void addParameter(const std::string &name, const std::string &value);
	std::string toString();
};

#endif // OAUTHREQUEST_H
