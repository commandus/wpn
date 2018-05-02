#include "tokenpair.h"

TokenPair::TokenPair()
	: kind(""), idToken(""), refreshToken(""), expiresIn(0), localId("")
{

}

TokenPair::TokenPair(const TokenPair& other)
	: kind(other.kind), idToken(other.idToken), refreshToken(other.refreshToken), expiresIn(other.expiresIn), localId(other.localId)
{

}

TokenPair::~TokenPair()
{

}

TokenPair& TokenPair::operator=(const TokenPair& other)
{
	kind = other.kind;
	idToken = other.idToken;
	refreshToken = other.refreshToken;
	expiresIn = other.expiresIn;
	localId = other.localId;
}

bool TokenPair::operator==(const TokenPair& other) const
{
	return 
		(other.kind == kind)
		&& (other.idToken == idToken)
		&& (other.refreshToken == refreshToken)
		&& (other.expiresIn == expiresIn)
		&& (other.localId == localId);
}

TokenPair::TokenPair(
	const std::string &akind,
	const std::string &aidToken,
	const std::string &arefreshToken,
	const time_t aexpiresIn,
	const std::string &alocalId
)
	: kind(akind), idToken(aidToken), refreshToken(arefreshToken), expiresIn(aexpiresIn), localId(alocalId)
{
}

bool TokenPair::parse(enum AUTH_PROVIDER provider, const std::string &value)
{
	bool r = true;
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
				try
				{
					json js = json::parse(value);
					idToken = js["idToken"];
					refreshToken = js["refreshToken"];
					std::string s = js["expiresIn"];
					expiresIn = strtol(s.c_str(), NULL, 10);
					localId = js["localId"];
					kind = js["kind"];
				}
				catch(...)
				{
					r = false;
				}
			}
	}
	return r;
}

json TokenPair::toJson()
{
	json r = {
		{ "kind", kind},
		{ "idToken", idToken },
		{ "refreshToken", refreshToken},
		{ "expiresIn", expiresIn},
		{ "localId", localId}
	};
	return r;
}

std::string TokenPair::toString()
{
	return toJson().dump();
}
