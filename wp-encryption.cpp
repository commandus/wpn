#include <inttypes.h>

#include <fstream>
#include "wp-encryption.h"

// --------------- WpnKeys ---------------

WpnKeys::WpnKeys()
{
	generate();
}

WpnKeys::WpnKeys
(
	const std::string &private_key,
	const std::string &public_key,
	const std::string &auth_secret
)
{
	init(private_key, public_key, auth_secret);
}

WpnKeys::WpnKeys
(
	const std::string &keys,
	const std::string &delimiter
)
{
	parse(keys, delimiter);
}

WpnKeys::WpnKeys(
	std::istream &strm,
	const std::string &delimiter
)
{
	read(strm, delimiter);
}

WpnKeys::WpnKeys(
	const std::string &fileName
)
{
	std::ifstream strm(fileName);
	read(strm, DEF_DELIMITER);
}

void WpnKeys::init(
	const std::string &private_key,
	const std::string &public_key,
	const std::string &auth_secret
)
{
	ece_base64url_decode(private_key.c_str(), private_key.size(), ECE_BASE64URL_REJECT_PADDING, privateKey, ECE_WEBPUSH_PRIVATE_KEY_LENGTH);
	ece_base64url_decode(public_key.c_str(), public_key.size(), ECE_BASE64URL_REJECT_PADDING, publicKey, ECE_WEBPUSH_PUBLIC_KEY_LENGTH);
	ece_base64url_decode(auth_secret.c_str(), auth_secret.size(), ECE_BASE64URL_REJECT_PADDING, authSecret, ECE_WEBPUSH_AUTH_SECRET_LENGTH);
}

void WpnKeys::parse(
	const std::string &keys,
	const std::string &delimiter
)
{
	std::string k[3];

	size_t p0 = 0, p1;
	int i = 0;
	while ((p1 = keys.find(delimiter, p0))) 
	{
		k[i] = keys.substr(p0, p1 - p0);
		p0 = p1 + delimiter.length();
		i++;
		if (i >= 3)
			break;
	}
	if (k[2].empty())
		generate();
	else
		init(k[0], k[1], k[2]); 
}

void WpnKeys::read(
	std::istream &strm,
	const std::string &delimiter
)
{
	if (strm.fail()) {
		generate();
		return;
	}

	std::string keys;
	std::getline(strm, keys);
	parse(keys, delimiter);
}

/**
 * Generate private hey, encryption public key and public auth 
 * @return 0- success
 */
int WpnKeys::generate()
{
	int err = ece_webpush_generate_keys(
		privateKey, ECE_WEBPUSH_PRIVATE_KEY_LENGTH, 
		publicKey, ECE_WEBPUSH_PUBLIC_KEY_LENGTH, 
		authSecret, ECE_WEBPUSH_AUTH_SECRET_LENGTH
	);
	return err;
}

std::string WpnKeys::getPrivateKey() const
{
	char r[ECE_WEBPUSH_PRIVATE_KEY_LENGTH * 3];
	return std::string(r, ece_base64url_encode(privateKey, ECE_WEBPUSH_PRIVATE_KEY_LENGTH, ECE_BASE64URL_OMIT_PADDING, r, sizeof(r)));
}

std::string WpnKeys::getPublicKey() const
{
	char r[ECE_WEBPUSH_PUBLIC_KEY_LENGTH * 3];
	return std::string(r, ece_base64url_encode(publicKey, ECE_WEBPUSH_PUBLIC_KEY_LENGTH, ECE_BASE64URL_OMIT_PADDING, r, sizeof(r)));
}

std::string WpnKeys::getAuthSecret() const
{
	char r[ECE_WEBPUSH_AUTH_SECRET_LENGTH* 3];
	return std::string(r, ece_base64url_encode(authSecret, ECE_WEBPUSH_AUTH_SECRET_LENGTH, ECE_BASE64URL_OMIT_PADDING, r, sizeof(r)));
}

void WpnKeys::write(
	std::ostream &strm,
	const std::string &delimiter
) const
{
	strm << getPrivateKey() << delimiter << getPublicKey() << delimiter << getAuthSecret() << std::endl;
}

void WpnKeys::write(
	const std::string &fileName
) const
{
	std::ofstream strm(fileName);
	write(strm, DEF_DELIMITER);
	strm.close();
}

// --------------- Subscription ---------------

Subscription::Subscription()
	:	endpoint(""), authorizedEntity(""), token(""), pushSet()
{
}

Subscription::Subscription(
	const std::string &a_endpoint,
	const std::string &a_authorizedEntity,
	const std::string &a_token,
	const std::string &a_pushSet
)
	:	endpoint(a_endpoint), authorizedEntity(a_authorizedEntity), token(a_token), pushSet(a_pushSet)
{
}

Subscription::Subscription(
	std::istream &strm,
	const std::string &delimiter
)
{
	read(strm, delimiter);
}

Subscription::Subscription(
	const std::string &fileName
)
{
	std::ifstream strm(fileName);
	read(strm, DEF_DELIMITER);
}

std::string Subscription::getEndpoint() const
{
	return endpoint;
}

std::string Subscription::getAuthorizedEntity() const
{
	return authorizedEntity;
}

std::string Subscription::getToken() const
{
	return token;
}

std::string Subscription::getPushSet() const
{
	return pushSet;
}

void Subscription::write
(
	std::ostream &strm,
	const std::string &delimiter
) const
{
	strm << getEndpoint() << delimiter << getAuthorizedEntity() << delimiter << getToken() << delimiter << getPushSet() << std::endl;
}

void Subscription::write
(
	const std::string &fileName
) const
{
	std::ofstream strm(fileName);
	write(strm, DEF_DELIMITER);
	strm.close();
}

void Subscription::init(
	const std::string &a_endpoint,
	const std::string &a_authorizedEntity,
	const std::string &a_token,
	const std::string &a_pushSet
)
{
	endpoint = a_endpoint;
	authorizedEntity = a_authorizedEntity;
	token = a_token;
	pushSet = a_pushSet;
}

void Subscription::parse(
	const std::string &keys,
	const std::string &delimiter
)
{
	std::string k[4];

	size_t p0 = 0, p1;
	int i = 0;
	while ((p1 = keys.find(delimiter, p0))) 
	{
		k[i] = keys.substr(p0, p1 - p0);
		p0 = p1 + delimiter.length();
		i++;
		if (i >= 4)
			break;
	}
	if (!k[2].empty())
		init(k[0], k[1], k[2], k[3]); 
}

void Subscription::read(
	std::istream &strm,
	const std::string &delimiter
)
{
	if (strm.fail()) {
		return;
	}

	std::string keys;
	std::getline(strm, keys);
	parse(keys, delimiter);
}

bool Subscription::valid() const
{
	return !endpoint.empty();
}

// --------------- Subscriptions ---------------

Subscriptions::Subscriptions()
{
}

Subscriptions::Subscriptions(
	std::istream &strm,
	const std::string &delimiter
)
{
	read(strm, delimiter);
}

Subscriptions::Subscriptions(
	const std::string &fileName
)
{
	std::ifstream strm(fileName);
	read(strm, DEF_DELIMITER);
}
	
void Subscriptions::write(
	std::ostream &strm,
	const std::string &delimiter
) const
{
	for (std::vector<Subscription>::const_iterator it(list.begin()); it != list.end(); ++it)
	{
		it->write(strm, delimiter);
	}
}

void Subscriptions::write(
	const std::string &fileName
) const
{
	std::ofstream strm(fileName);
	write(strm, DEF_DELIMITER);
	strm.close();
}

void Subscriptions::read(
	std::istream &strm,
	const std::string &delimiter
)
{
	while (!strm.fail()) 
	{
		Subscription s(strm, delimiter);
		if (!s.valid())
			break;
	}
}
