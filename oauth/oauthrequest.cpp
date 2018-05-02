#include "oauthrequest.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

OAuthRequest::OAuthRequest
(
	enum AUTH_PROVIDER aprovider,
	const std::string &aidentifier,
	const std::string &aproviderId,
	const std::string &acontinueUri,
	const std::string &ascope
)
	: provider(aprovider), identifier(aidentifier), providerId(aproviderId), continueUri(acontinueUri), scope(ascope)
{
}

OAuthRequest::OAuthRequest(const OAuthRequest &other)
{
	provider = other.provider;
	identifier = other.identifier;
	providerId = other.providerId;
	continueUri = other.continueUri;
	scope = other.scope;
}

OAuthRequest::~OAuthRequest()
{

}

OAuthRequest& OAuthRequest::operator=(const OAuthRequest &other)
{
	provider = other.provider;
	identifier = other.identifier;
	providerId = other.providerId;
	continueUri = other.continueUri;
	scope = other.scope;
}

bool OAuthRequest::operator==(const OAuthRequest &other) const
{
	return 
		(provider == other.provider) &&
		(identifier == other.identifier) &&
		(providerId == other.providerId) &&
		(continueUri == other.continueUri) &&
		(scope == other.scope);
}

void OAuthRequest::addParameter(const std::string &name, const std::string &value)
{
	std::pair<std::string, std::string> p;
	p.first = name;
	p.second = value;
	customParameters.push_back(p);
}

std::string OAuthRequest::toString()
{
	switch (provider) {
		case AUTH_EMAIL:
			break;
		case AUTH_PHONE:
			break;
		case AUTH_GOOGLE:
			break;
		case AUTH_PLAY:
			break;
		case AUTH_FACEBOOK:
			break;
		case AUTH_TWITTER:
			break;
		case AUTH_GITHUB:
			break;
		default:
			// AUTH_ANONYMOUS
			{
				json js = {
					{"identifier", identifier},
					{"providerId", providerId},		// "google.com"
					{"continueUri", continueUri},
					{"oauthScope", scope}
				};
				return js.dump();
			}
	}
	return "";
}
