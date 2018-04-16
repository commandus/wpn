#include <inttypes.h>

#include <fstream>
#include "third_party/sole/sole.hpp"
#include "wp-storage-file.h"

// --------------- AndroidCredentials ---------------

AndroidCredentials::AndroidCredentials()
	: mAppId(genAppId()), mAndroidId(0), mSecurityToken(0), mFCMToken("")
{
}

AndroidCredentials::AndroidCredentials
(
	const std::string &appId,
	uint64_t androidId,
	uint64_t securityToken,
	const std::string &fcmToken
)
{
	init(appId, androidId, securityToken, fcmToken);
}

AndroidCredentials::AndroidCredentials
(
	const std::string &keys,
	const std::string &delimiter
)
{
	parse(keys, delimiter);
}

AndroidCredentials::AndroidCredentials(
	std::istream &strm,
	const std::string &delimiter
)
{
	read(strm, delimiter);
}

AndroidCredentials::AndroidCredentials(
	const std::string &fileName
)
{
	std::ifstream strm(fileName);
	read(strm, DEF_DELIMITER);
}

std::string AndroidCredentials::genAppId()
{
	return sole::uuid4().str();
}

void AndroidCredentials::init(
	const std::string &appId,
	uint64_t androidId,
	uint64_t securityToken,
	const std::string &fcmToken
)
{
	mAppId = appId;
	mAndroidId = androidId;
	mSecurityToken = securityToken;
	mFCMToken = fcmToken;
}

void AndroidCredentials::parse(
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
	if (k[3].empty())
	{
		mAppId = genAppId();
		mAndroidId = 0;
		mSecurityToken = 0;
		mFCMToken = "";
	}
	else
		init(k[0], strtoul(k[1].c_str(), NULL, 10), strtoul(k[2].c_str(), NULL, 10), k[3]); 
}

void AndroidCredentials::read(
	std::istream &strm,
	const std::string &delimiter
)
{
	if (strm.fail()) {
		mAppId = genAppId();
		mAndroidId = 0;
		mSecurityToken = 0;
		mFCMToken = "";
		return;
	}

	std::string keys;
	std::getline(strm, keys);
	parse(keys, delimiter);
}

const std::string &AndroidCredentials::getAppId() const
{
	return mAppId;
}

uint64_t AndroidCredentials::getAndroidId() const
{
	return mAndroidId;
}

uint64_t AndroidCredentials::getSecurityToken() const
{
	return mSecurityToken;
}

const std::string &AndroidCredentials::getFCMToken() const
{
	return mFCMToken;
}

void AndroidCredentials::setAndroidId(uint64_t value)
{
	mAndroidId = value;
}

void AndroidCredentials::setSecurityToken(uint64_t value)
{
	mSecurityToken = value;
}

void AndroidCredentials::setFCMToken(const std::string &value)
{
	mFCMToken = value;
}

void AndroidCredentials::write
(
	std::ostream &strm,
	const std::string &delimiter
) const
{
	strm << mAppId << delimiter << mAndroidId << delimiter << mSecurityToken << delimiter << mFCMToken << std::endl;
}

void AndroidCredentials::write(
	const std::string &fileName
) const
{
	std::ofstream strm(fileName);
	write(strm, DEF_DELIMITER);
	strm.close();
}

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
	:	subscribeUrl(""), subscribeMode(0), endpoint(""), authorizedEntity(""), token(""), pushSet("")
{
}

Subscription::Subscription(
	std::string aSubscribeUrl,
	int aSubscribeMode,
	const std::string &a_endpoint,
	const std::string &a_authorizedEntity,
	const std::string &a_token,
	const std::string &a_pushSet
)
	:	subscribeUrl(aSubscribeUrl), subscribeMode(aSubscribeMode), endpoint(a_endpoint), authorizedEntity(a_authorizedEntity), token(a_token), pushSet(a_pushSet)
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

std::string Subscription::getSubscribeUrl() const
{
	return subscribeUrl;
}

int Subscription::getSubscribeMode() const
{
	return subscribeMode;
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

void Subscription::setSubscribeUrl(const std::string &value)\
{
	subscribeUrl = value;
}

void Subscription::setSubscribeMode(int value)
{
	subscribeMode = value;
}

void Subscription::setEndpoint(const std::string &value)
{
	endpoint = value;
}

void Subscription::setAuthorizedEntity(const std::string &value)
{
	authorizedEntity = value;
}

void Subscription::setToken(const std::string &value)
{
	token = value;
}

void Subscription::setPushSet(const std::string &value)
{
	pushSet = value;
}

void Subscription::write
(
	std::ostream &strm,
	const std::string &delimiter
) const
{
	strm << getSubscribeUrl() << delimiter << getSubscribeMode() << delimiter 
		<< getEndpoint() << delimiter << getAuthorizedEntity() << delimiter 
		<< getToken() << delimiter << getPushSet() << std::endl;
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
	std::string a_subscribeUrl,
	int a_subscribeMode,
	const std::string &a_endpoint,
	const std::string &a_authorizedEntity,
	const std::string &a_token,
	const std::string &a_pushSet
)
{
	subscribeUrl = a_subscribeUrl;
	subscribeMode = a_subscribeMode;
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
	std::string k[6];

	size_t p0 = 0, p1;
	int i = 0;
	while ((p1 = keys.find(delimiter, p0))) 
	{
		k[i] = keys.substr(p0, p1 - p0);
		p0 = p1 + delimiter.length();
		i++;
		if (i >= 6)
			break;
	}
	if (!k[2].empty())
		init(k[0], strtol(k[1].c_str(), NULL, 10), k[2], k[3], k[4], k[5]); 
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
		list.push_back(s);
	}
}
