#ifndef TOKENPAIR_H
#define TOKENPAIR_H

#include <string>
#include "oauth.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

class TokenPair
{
public:
	std::string kind;
	std::string idToken; 
	std::string refreshToken;
	time_t expiresIn;
	std::string localId;

	TokenPair();
	TokenPair(
		const std::string &kind,
		const std::string &idToken,
		const std::string &refreshToken,
		const time_t expiresIn,
		const std::string &localId
	);
	TokenPair(const TokenPair& other);
	~TokenPair();
	TokenPair& operator=(const TokenPair& other);
	bool operator==(const TokenPair& other) const;
	
	bool parse(enum AUTH_PROVIDER provider, const std::string &value);
	json toJson();
	std::string toString();
};

#endif // TOKENPAIR_H
