#ifndef OAUTH_H
#define OAUTH_H

#define ERR_NO_PROVIDER_IMPL	-1
#define ERR_PARSE_IDENTITY_RESPONSE		-2

enum AUTH_PROVIDER 
{
	AUTH_ANONYMOUS = 0, 
	AUTH_EMAIL = 1,
	AUTH_PHONE = 2,
	AUTH_GOOGLE = 3,
	AUTH_PLAY = 4,
	AUTH_FACEBOOK = 5,
	AUTH_TWITTER = 6,
	AUTH_GITHUB = 7
};

#endif
