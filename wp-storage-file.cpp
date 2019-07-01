#include <inttypes.h>

#include <fstream>
#include "utilinstance.h"
#include "wp-storage-file.h"

#include "utilstring.h"
#include "utilvapid.h"
#include "endpoint.h"

// --------------- ClientOptions ---------------

#define DEF_NAME ""

void ClientOptions::parse
(
	const std::string &keys,
	const std::string &delimiter
)
{
	std::string k[1];
	size_t p0 = 0, p1;
	int i = 0;
	while ((p1 = keys.find(delimiter, p0))) 
	{
		k[i] = keys.substr(p0, p1 - p0);
		p0 = p1 + delimiter.length();
		i++;
		if (i >= 1)
			break;
	}
	if (k[0].empty())
	{
		name = "";
	}
	else
		name = k[0]; 
}

void ClientOptions::read(
	std::istream &strm,
	const std::string &delimiter
) {
	if (strm.fail()) {
		name = "";
		return;
	}

	std::string keys;
	std::getline(strm, keys);
	parse(keys, delimiter);
}

ClientOptions::ClientOptions()
	: verbosity(0)
{
	this->name = DEF_NAME;
}

ClientOptions::ClientOptions(
	const std::string &name
)
	: verbosity(0)
{
	this->name = name;
}

ClientOptions::ClientOptions(
	std::istream &strm,
	const std::string &delimiter
)
	: verbosity(0)
{
	read(strm, delimiter);
}

ClientOptions::ClientOptions(
	const json &value
)
	: verbosity(0)
{
	json::const_iterator f = value.find("name");
	if (f != value.end())
		name = f.value();
	f = value.find("verbosity");
	if (f != value.end())
		verbosity = f.value();
}

std::ostream::pos_type ClientOptions::write(
	std::ostream &strm,
	const std::string &delimiter,
	const int writeFormat
) const
{
	std::ostream::pos_type r = strm.tellp();
	switch (writeFormat)
	{
		case 1:
			strm << toJson().dump();
			break;
		default:
			strm << name << delimiter << verbosity << std::endl;
	}
	r = strm.tellp() - r;
	return r;
}

std::ostream::pos_type ClientOptions::write(
	const std::string &fileName
) const
{
	std::ofstream strm(fileName);
	std::ostream::pos_type r = write(strm, DEF_DELIMITER);
	strm.close();
	return r;
}
	
json ClientOptions::toJson() const
{
	json r = {
		{ "name", name },
		{ "verbosity", verbosity }
	};
	return r;
}

int ClientOptions::getVerbosity()
{
	return verbosity;
}

void ClientOptions::setVerbosity(
	int value
)
{
	verbosity = value;
}

// --------------- AndroidCredentials ---------------

AndroidCredentials::AndroidCredentials()
	: mAppId(genAppId()), mAndroidId(0), mSecurityToken(0), mGCMToken("")
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
	strm.close();
}

AndroidCredentials::AndroidCredentials(
	const json &value
)
{
	std::string appId;
	uint64_t androidId;
	uint64_t securityToken;
	std::string GCMToken;
	json::const_iterator f = value.find("appId");
	if (f != value.end())
		appId = f.value();
	else
		appId = "";
	f = value.find("androidId");
	if (f != value.end())
		androidId = f.value();
	else
		androidId = 0;
	f = value.find("securityToken");
	if (f != value.end())
		securityToken = f.value();
	else
		securityToken = 0;
	f = value.find("GCMToken");
	if (f != value.end())
		GCMToken = f.value();
	else
		GCMToken = "";
	init(appId, androidId, securityToken, GCMToken);
}

std::string AndroidCredentials::genAppId()
{
	return mkInstanceId();
}

void AndroidCredentials::init
(
	const std::string &appId,
	uint64_t androidId,
	uint64_t securityToken,
	const std::string &gcmToken
)
{
	mAppId = appId;
	mAndroidId = androidId;
	mSecurityToken = securityToken;
	mGCMToken = gcmToken;
}

void AndroidCredentials::parse
(
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
		mGCMToken = "";
	}
	else
		init(k[0], strtoul(k[1].c_str(), NULL, 10), strtoul(k[2].c_str(), NULL, 10), k[3]); 
}

void AndroidCredentials::read
(
	std::istream &strm,
	const std::string &delimiter
)
{
	if (strm.fail()) {
		mAppId = genAppId();
		mAndroidId = 0;
		mSecurityToken = 0;
		mGCMToken = "";
		return;
	}

	std::string keys;
	std::getline(strm, keys);
	parse(keys, delimiter);
}

// #define APP_ID_PREFIX "wp:com.commandus.wpn#"
#define APP_ID_PREFIX "" 

const std::string AndroidCredentials::getAppId() const
{
	return APP_ID_PREFIX + mAppId;
}

uint64_t AndroidCredentials::getAndroidId() const
{
	return mAndroidId;
}

uint64_t AndroidCredentials::getSecurityToken() const
{
	return mSecurityToken;
}

const std::string &AndroidCredentials::getGCMToken() const
{
	return mGCMToken;
}

void AndroidCredentials::setAndroidId(uint64_t value)
{
	mAndroidId = value;
}

void AndroidCredentials::setSecurityToken(uint64_t value)
{
	mSecurityToken = value;
}

void AndroidCredentials::setGCMToken(const std::string &value)
{
	mGCMToken = value;
}

std::ostream::pos_type AndroidCredentials::write
(
	std::ostream &strm,
	const std::string &delimiter,
	const int writeFormat
) const
{
	std::ostream::pos_type r = strm.tellp();
	switch (writeFormat)
	{
		case 1:
			strm << toJson().dump();
			break;
		default:
			strm << mAppId << delimiter << mAndroidId << delimiter << mSecurityToken 
			<< delimiter << mGCMToken << std::endl;
	}
	r = strm.tellp() - r;
	return r;
}

std::ostream::pos_type AndroidCredentials::write
(
	const std::string &fileName
) const
{
	std::ofstream strm(fileName);
	std::ostream::pos_type r = write(strm, DEF_DELIMITER);
	strm.close();
	return r;
}

json AndroidCredentials::toJson(
) const
{
	json r = {
		{ "appId", mAppId },
		{ "androidId", mAndroidId },
		{ "securityToken", mSecurityToken },
		{ "GCMToken", mGCMToken }
	};
	return r;
}

// --------------- WpnKeys ---------------

WpnKeys::WpnKeys()
{
	generate();
}

WpnKeys::WpnKeys (
	const WpnKeys &value
)
{
	init(value);
}

WpnKeys::WpnKeys
(
	uint64_t id,
	uint64_t secret,
	const std::string &private_key,
	const std::string &public_key,
	const std::string &auth_secret
)
{
	init(id, secret, private_key, public_key, auth_secret);
}

WpnKeys::WpnKeys
(
	uint64_t id,
	uint64_t secret,
	const std::string &keys,
	const std::string &delimiter
)
{
	this->id = id;
	this->secret = secret,
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
	strm.close();
}

WpnKeys::WpnKeys(
	const json &value
)
{
	uint64_t id;
	uint64_t secret;
	std::string privateKey;
	std::string publicKey;
	std::string authSecret;

	json::const_iterator f = value.find("id");
	if (f != value.end())
		id = f.value();
	f = value.find("secret");
	if (f != value.end())
		secret = f.value();
	f = value.find("privateKey");
	if (f != value.end())
		privateKey = f.value();
	f = value.find("publicKey");
	if (f != value.end())
		publicKey = f.value();
	f = value.find("authSecret");
	if (f != value.end())
		authSecret = f.value();
	init(id, secret, privateKey, publicKey, authSecret);
}

void WpnKeys::init(
	uint64_t id,
	uint64_t secret,
	const std::string &private_key,
	const std::string &public_key,
	const std::string &auth_secret
)
{
	this->id = id;
	this->secret = secret;
	ece_base64url_decode(private_key.c_str(), private_key.size(), ECE_BASE64URL_REJECT_PADDING, privateKey, ECE_WEBPUSH_PRIVATE_KEY_LENGTH);
	ece_base64url_decode(public_key.c_str(), public_key.size(), ECE_BASE64URL_REJECT_PADDING, publicKey, ECE_WEBPUSH_PUBLIC_KEY_LENGTH);
	ece_base64url_decode(auth_secret.c_str(), auth_secret.size(), ECE_BASE64URL_REJECT_PADDING, authSecret, ECE_WEBPUSH_AUTH_SECRET_LENGTH);
}

void WpnKeys::init(
	const WpnKeys &value
)
{
	init(value.id, value.secret, value.getPrivateKey(), value.getPublicKey(), value.getAuthSecret());
}

void WpnKeys::parse(
	const std::string &keys,
	const std::string &delimiter
)
{
	std::string k[5];

	size_t p0 = 0, p1;
	int i = 0;
	while ((p1 = keys.find(delimiter, p0))) 
	{
		k[i] = keys.substr(p0, p1 - p0);
		p0 = p1 + delimiter.length();
		i++;
		if (i >= 5)
			break;
	}
	if (k[3].empty())
		generate();
	else
		init(std::stoi(k[0]), std::stoi(k[1]), k[2], k[3], k[4]); 
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

const uint8_t *WpnKeys::getPrivateKeyArray() const
{
	return (const uint8_t *) &privateKey;
}

std::string WpnKeys::getPrivateKey() const
{
	return base64UrlEncode(privateKey, ECE_WEBPUSH_PRIVATE_KEY_LENGTH);
}

std::string WpnKeys::getPublicKey() const
{
	return base64UrlEncode(publicKey, ECE_WEBPUSH_PUBLIC_KEY_LENGTH);
}

const uint8_t *WpnKeys::getAuthSecretArray() const
{
	return (const uint8_t *) &authSecret;
}

std::string WpnKeys::getAuthSecret() const
{
	return base64UrlEncode(authSecret, ECE_WEBPUSH_AUTH_SECRET_LENGTH);
}

void WpnKeys::setPrivateKey(
	const std::string &value
)
{
	ece_base64url_decode(value.c_str(), value.size(), ECE_BASE64URL_REJECT_PADDING, privateKey, ECE_WEBPUSH_PRIVATE_KEY_LENGTH);
}

void WpnKeys::setPublicKey(
	const std::string &value
)
{
	ece_base64url_decode(value.c_str(), value.size(), ECE_BASE64URL_REJECT_PADDING, publicKey, ECE_WEBPUSH_PUBLIC_KEY_LENGTH);
}

void WpnKeys::setAuthSecret(
	const std::string &value
)
{
	ece_base64url_decode(value.c_str(), value.size(), ECE_BASE64URL_REJECT_PADDING, authSecret, ECE_WEBPUSH_AUTH_SECRET_LENGTH);
}

std::ostream::pos_type WpnKeys::write
(
	std::ostream &strm,
	const std::string &delimiter,
	const int writeFormat
) const
{
	std::ostream::pos_type r = strm.tellp();
	switch (writeFormat)
	{
		case FORMAT_JSON:
			strm << toJson().dump();
			break;
		default:
			strm << id << delimiter << secret << delimiter << getPrivateKey() << delimiter << getPublicKey() << delimiter << getAuthSecret() << std::endl;
	}
	return strm.tellp() - r;
}

std::ostream::pos_type WpnKeys::write(
	const std::string &fileName
) const
{
	std::ofstream strm(fileName);
	std::ostream::pos_type r = write(strm, DEF_DELIMITER);
	strm.close();
	return r;
}

json WpnKeys::toJson(
) const
{
	json r = {
		{ "id", id },
		{ "secret", secret },
		{ "privateKey", getPrivateKey() },
		{ "publicKey", getPublicKey() },
		{ "authSecret", getAuthSecret() }
	};
	return r;
}

// --------------- Subscription ---------------

Subscription::Subscription()
	: name(""), subscribeUrl(""), subscribeMode(0), endpoint(""), authorizedEntity(""),
	token(""), sentToken(""), pushSet(""), persistentId("")
{
}

Subscription::Subscription(
	const std::string &aName,
	const std::string &aSubscribeUrl,
	const std::string &a_endpoint,
	const std::string &a_serverKey,
	const std::string &a_authorizedEntity,
	const std::string &a_token,
	const std::string &a_pushSet,
	const std::string &aPersistentId
)
	: subscribeUrl(aSubscribeUrl), subscribeMode(SUBSCRIBE_FORCE_FIREBASE), endpoint(a_endpoint), serverKey(a_serverKey),
	authorizedEntity(a_authorizedEntity), token(a_token), sentToken(""), pushSet(a_pushSet),
	persistentId(aPersistentId)
{
	name = escapeURLString(aName);
}

// VAPID
Subscription::Subscription(
	uint64_t id,
	uint64_t secret,
	const std::string &a_name,
	const std::string &a_endpoint,
	const std::string &a_privateKey,
	const std::string &a_publicKey,
	const std::string &a_authSecret,
	const std::string &aPersistentId
)
	: subscribeUrl(""), subscribeMode(SUBSCRIBE_FORCE_VAPID), endpoint(a_endpoint), 
	serverKey(""), authorizedEntity(""), token(""), sentToken(""), pushSet(""),
	persistentId(aPersistentId)
{
	name = escapeURLString(a_name);
	setWpnKeys(WpnKeys(id, secret, a_privateKey, a_publicKey, a_authSecret));
}

	Subscription::Subscription(
		uint64_t id,
		const std::string &a_name,
		const std::string &publicKey
	)
	: subscribeUrl(""), subscribeMode(SUBSCRIBE_FORCE_VAPID), endpoint(""), 
	serverKey(""), authorizedEntity(""), token(""), sentToken(""), pushSet(""), persistentId("")
{
	name = escapeURLString(a_name);
	setWpnKeys(WpnKeys(id, 0, "", publicKey, ""));
}

Subscription::Subscription(
	std::istream &strm,
	const std::string &delimiter
)
{
	read(strm, delimiter);
}

void Subscription::fromStream(
	uint64_t id,
	std::istream &strm
)
{
	json j;
	try {
		strm >> j;
		fromJson(j);
	}
	catch (...) {
	}
}

Subscription::Subscription(
	uint64_t id,
	std::istream &strm
)
{
	fromStream(id, strm);
}

Subscription::Subscription(
	const std::string &fileName
)
{
	std::ifstream strm(fileName);
	read(strm, DEF_DELIMITER);
	strm.close();
}

void Subscription::fromJson(const json &value)
{
	json::const_iterator f = value.find("subscribeMode");
	if (f != value.end())
		subscribeMode = f.value();
	switch (this->subscribeMode) {
	case SUBSCRIBE_FORCE_FIREBASE:
		f = value.find("name");
		if (f != value.end())
			name = f.value();
		f = value.find("subscribeUrl");
		if (f != value.end())
			subscribeUrl = f.value();
		f = value.find("endpoint");
		if (f != value.end())
			endpoint = f.value();
		f = value.find("authorizedEntity");
		if (f != value.end())
			authorizedEntity = f.value();
		f = value.find("token");
		if (f != value.end())
			token = f.value();
		f = value.find("pushSet");
		if (f != value.end())
			pushSet = f.value();
		f = value.find("persistentId");
		if (f != value.end())
			persistentId = f.value();
		f = value.find("serverKey");
		if (f != value.end())
			serverKey = f.value();
		break;
	case SUBSCRIBE_FORCE_VAPID:
	{
		/*
		f = value.find("secret");
		if (f != value.end()) {
			secret = f.value();
			getWpnKeysPtr()->secret = secret;
		}
		f = value.find("privateKey");
		if (f != value.end())
			privateKey = f.value();
		f = value.find("persistentId");
		if (f != value.end())
			persistentId = f.value();
		f = value.find("pushSet");
		if (f != value.end())
			pushSet = f.value();
		*/
		uint64_t id;
		std::string publicKey;
		std::string privateKey;
		std::string authSecret;
		f = value.find("id");
		if (f != value.end())
			id = f.value();
		else
			id = 0;
		f = value.find("publicKey");
		if (f != value.end())
			publicKey = f.value();
		f = value.find("authSecret");
		if (f != value.end())
			authSecret = f.value();
		this->wpnKeys.init(id, 0, privateKey, publicKey, authSecret);
		f = value.find("name");
		if (f != value.end())
			name = f.value();
		f = value.find("endpoint");
		if (f != value.end())
			endpoint = f.value();
		f = value.find("token");
		if (f != value.end())
			token = f.value();
		f = value.find("sentToken");
		if (f != value.end())
			sentToken = f.value();
		break;
	}
	default:
		break;
	}
}

Subscription::Subscription(
	const json &value
)
{
	fromJson(value);
}

std::string Subscription::getName() const
{
	return name;
}

std::string Subscription::getSentToken() const
{
	return sentToken;
}

void Subscription::setSentToken(
	const std::string &value
)
{
	sentToken = value;
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
	/*
	if (endpoint.empty()) {
		endPoint = endpoint(token, true, 0);	///< 0- Chrome, 1- Firefox
	}
	*/
	return endpoint;
}

std::string Subscription::getServerKey() const
{
	return serverKey;
}

std::string Subscription::getAuthorizedEntity() const
{
	return authorizedEntity;
}

std::string Subscription::getToken() const
{
	return token;
}

bool Subscription::hasToken() const
{
	// 152 characters
	return token.length() > 100;
}

std::string Subscription::getPushSet() const
{
	return pushSet;
}

const std::string &Subscription::getPersistentId() const
{
	return persistentId;
}

const WpnKeys& Subscription::getWpnKeys() const
{
	return wpnKeys;
}

WpnKeys* Subscription::getWpnKeysPtr()
{
	return &wpnKeys;
}

void Subscription::setName(const std::string &value)
{
	name = escapeURLString(value);
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

void Subscription::setServerKey(const std::string &value)
{
	serverKey = value;
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

void Subscription::setWpnKeys(const WpnKeys &value)
{
	wpnKeys = value;
}

void setPersistentId
(
	const std::string &value
);

void Subscription::setPersistentId
(
	const std::string &value
)
{
	if (!value.empty())
		persistentId = value;
}

std::ostream::pos_type Subscription::write
(	std::ostream &strm,
	const std::string &delimiter,
	const int writeFormat
) const
{
	std::ostream::pos_type r = strm.tellp();
	switch (writeFormat)
	{
		case FORMAT_JSON:
			{
				strm << toJson().dump();
			}
			break;
		default:
		{
			int subscriptionMode = getSubscribeMode();
			switch (subscriptionMode) {
			case SUBSCRIBE_FORCE_FIREBASE:
				strm << getSubscribeMode() << delimiter << getName() << delimiter << getSubscribeUrl() << delimiter
					<< getEndpoint() << delimiter << getServerKey() << delimiter
					<< getAuthorizedEntity() << delimiter << getToken() << getPushSet() << delimiter << getPersistentId() << std::endl;
				break;
			case SUBSCRIBE_FORCE_VAPID:
				strm << getSubscribeMode() << delimiter << getName() << delimiter << getEndpoint() << delimiter << getPersistentId() << delimiter;
				getWpnKeys().write(strm, delimiter, writeFormat);
				break;
			default:
				break;
			}
		}
		break;
	}
	return strm.tellp() - r;
}

std::ostream::pos_type Subscription::write
(
	const std::string &fileName
) const
{
	std::ofstream strm(fileName);
	std::ostream::pos_type r = write(strm, DEF_DELIMITER, FORMAT_TEXT);
	strm.close();
	return r;
}

json Subscription::toJson(
) const
{
	json r;
	switch (getSubscribeMode()) {
	case SUBSCRIBE_FORCE_FIREBASE:
		r = {
			{ "subscribeMode", getSubscribeMode() },
			{ "name", getName() },
			{ "subscribeUrl", getSubscribeUrl() },
			{ "endpoint", getEndpoint() },
			{ "authorizedEntity", getAuthorizedEntity() },
			{ "token", getToken() },
			{ "pushSet", getPushSet() },
			{ "persistentId", getPersistentId() },
			{ "serverKey", getServerKey() }
		};
		break;
	case SUBSCRIBE_FORCE_VAPID:
	{
		const WpnKeys& wpnKeys = getWpnKeys();
		r = {
			{ "id", wpnKeys.id },

			{ "subscribeMode", getSubscribeMode() },
			{ "name", getName() },
			{ "token", getToken() },
			{ "sentToken", getSentToken() },

			/*
			{ "endpoint", getEndpoint() },
			{ "pushSet", getPushSet() },
			{ "secret", wpnKeys.secret },
			{ "privateKey", wpnKeys.getPrivateKey() },
			{ "persistentId", getPersistentId() }
			*/

			{ "publicKey", wpnKeys.getPublicKey() },
			{ "authSecret", wpnKeys.getAuthSecret() }
		};
	}
	break;
	default:
		break;
	}
	return r;
}

/// Initialize VAPID
void Subscription::initVAPID1(
	const std::string &a_name,
	const std::string &a_endpoint,
	const std::string &a_persistentId,
	const WpnKeys *a_wpn_keys
)
{
	subscribeMode = SUBSCRIBE_FORCE_VAPID;
	name = a_name;
	endpoint = a_endpoint;
	persistentId = a_persistentId;
	wpnKeys = *a_wpn_keys;
}

/// Initialize VAPID
void Subscription::initVAPID(
	const std::string &a_name,
	const std::string &a_endpoint,
	const std::string &a_persistentId,
	uint64_t id,
	uint64_t secret,
	const std::string &a_public_key,	// VAPID public key
	const std::string &a_private_key,	// VAPID private key
	const std::string &a_auth_secret	// VAPID auth secret
)
{
	WpnKeys wpnKeys(id, secret, a_public_key, a_private_key, a_auth_secret);
	initVAPID1(a_name, a_endpoint, a_persistentId, &wpnKeys);
}

/// Initialize FCM
void Subscription::initFCM(
	const std::string &a_name,
	const std::string &a_subscribeUrl,
	const std::string &a_endpoint,
	const std::string &a_serverKey,
	const std::string &a_authorizedEntity,
	const std::string &a_token,
	const std::string &a_pushSet,
	const std::string &a_persistentId
)
{
	subscribeMode = SUBSCRIBE_FORCE_FIREBASE;
	name = a_name;
	subscribeUrl = a_subscribeUrl;
	endpoint = a_endpoint;
	serverKey = a_serverKey;
	authorizedEntity = a_authorizedEntity;
	token = a_token;
	pushSet = a_pushSet;
	persistentId = a_persistentId;
}

void Subscription::parse
(
	const std::string &keys,
	const std::string &delimiter
)
{
	std::string k[9];

	size_t p0 = 0, p1;
	int i = 0;
	while ((p1 = keys.find(delimiter, p0))) 
	{
		k[i] = keys.substr(p0, p1 - p0);
		p0 = p1 + delimiter.length();
		i++;
		if (i >= 9)
			break;
	}
	if ((!k[2].empty()) && (!k[0].empty())) {
		int subscriptionMode = strtol(k[0].c_str(), NULL, 10);
		switch (subscriptionMode) {
			case SUBSCRIBE_FORCE_FIREBASE:
				initFCM(k[1], k[2], k[3], k[4], k[5], k[6], k[7], k[8]); 
				break;
			case SUBSCRIBE_FORCE_VAPID:
				// 0- SubscribeMode 1- Name 2- Endpoint 3- PersistentId 4, 5, 6, 7, 8- WpnKeys
				initVAPID(k[1], k[2], k[3], std::stoi(k[4]), std::stoi(k[5]), k[6], k[7], k[8]); 
				break;
			default:
				break;
		}
	}
}

void Subscription::read
(
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

bool Subscription::operator==(const Subscription &val) const
{
	return (this->endpoint == val.endpoint) &&
		(this->authorizedEntity == val.authorizedEntity);
}

// --------------- Subscriptions ---------------

Subscriptions::Subscriptions()
	: receivedPersistentId("")
{
}

Subscriptions::Subscriptions(
	std::istream &strm,
	const std::string &delimiter
)
{
	read(strm, delimiter);
}

Subscriptions::Subscriptions
(
	const std::string &fileName
)
{
	std::ifstream strm(fileName);
	read(strm, DEF_DELIMITER);
	strm.close();
}

Subscriptions::Subscriptions(
	const json &value
)
{
	for (json::const_iterator it = value.begin(); it != value.end(); ++it) {
		Subscription s(*it);
		list.push_back(s);
	}
}

/**
 * Constructor for find() only
 */
Subscription::Subscription
(
	const std::string &end_point,
	const std::string &authorized_entity
)
:	endpoint(end_point), authorizedEntity(authorized_entity)
{
}

void Subscriptions::setReceivedPersistentId(const std::string &value)
{
	receivedPersistentId = value;
}

const std::string & Subscriptions::getReceivedPersistentId() const
{
	return receivedPersistentId;
}

std::ostream::pos_type Subscriptions::write
(
	std::ostream &strm,
	const std::string &delimiter,
	const int writeFormat
) const
{
	std::ostream::pos_type r = 0;
	strm << receivedPersistentId << std::endl;
	for (std::vector<Subscription>::const_iterator it(list.begin()); it != list.end(); ++it)
	{
		r += it->write(strm, delimiter, writeFormat);
	}
	return r;
}

std::ostream::pos_type Subscriptions::write
(
	const std::string &fileName
) const
{
	std::ofstream strm(fileName);
	std::ostream::pos_type r = write(strm, DEF_DELIMITER);
	strm.close();
	return r;
}

void Subscriptions::read(
	std::istream &strm,
	const std::string &delimiter
)
{
	if (!strm.fail()) {
		std::getline(strm, receivedPersistentId);
	}

	while (!strm.fail()) 
	{
		Subscription s(strm, delimiter);
		if (!s.valid())
			break;
		list.push_back(s);
	}
}

json Subscriptions::toJson(
) const
{
	json subscriptions = json::array();
	for (std::vector<Subscription>::const_iterator it(list.begin()); it != list.end(); ++it)
	{
		subscriptions.push_back(it->toJson());
	}
	return subscriptions;
}

Subscription *Subscriptions::getById(
	uint64_t id
) const
{
	std::vector<Subscription>::const_iterator it = std::find_if(list.begin(), list.end(),
        [id](const Subscription &m) -> bool { return m.getWpnKeys().id == id; });
	if (it == list.end())
		return NULL;
	else
		return (Subscription *) &(*it);
}

Subscription *Subscriptions::findByNameOrId(
	const std::string &name
) const
{
	if (name.empty())
		return NULL;
	if (std::all_of(name.begin(), name.end(), ::isdigit)) {
		// decimal number
		uint64_t id = strtoull(name.c_str(), NULL, 10);
		return getById(id);
	} else {
		// name
		std::vector<Subscription>::const_iterator it = std::find_if(list.begin(), list.end(),
			[name](const Subscription &m) -> bool { return m.getName() == name; });
		if (it == list.end())
			return NULL;
		else
			return (Subscription *) &(*it);
	}
}

Subscription *Subscriptions::findByPublicKey(
	const std::string &value
) const
{
	if (value.empty())
		return NULL;
	std::vector<Subscription>::const_iterator it = std::find_if(list.begin(), list.end(),
		[value](const Subscription &m) -> bool { return m.getWpnKeys().getPublicKey() == value; });
	if (it == list.end())
		return NULL;
	else
		return (Subscription *) &(*it);
}

bool Subscriptions::rmById(
	uint64_t id
)
{
	std::vector<Subscription>::const_iterator it = std::find_if(list.begin(), list.end(),
        [id](const Subscription &m) -> bool { return m.getWpnKeys().id == id; });
	if (it == list.end())
		return false;
	list.erase(it);
	return true;
}

void Subscriptions::putSubsciptionVapidPubKey(
	uint64_t id,
	const std::string &vapidPublicKey
)
{
	Subscription* v = getById(id);
	if (v) {
		v->getWpnKeysPtr()->setPublicKey(vapidPublicKey);
	} else {
		Subscription s(id, "", vapidPublicKey);
		list.push_back(s);
	}
}

void generateVAPIDKeys
(
	std::string &privateKey,
	std::string &publicKey,
	std::string &authSecret
)
{
	WpnKeys k;
	k.generate();

	privateKey = k.getPrivateKey();
	publicKey = k.getPublicKey();
	authSecret = k.getAuthSecret();
}

// ConfigFile

bool ConfigFile::fromJson(const json &value)
{
	bool r = (value.find("options") != value.end())
		&& (value.find("credentials") != value.end())
		&& (value.find("keys") != value.end())
		&& (value.find("subscriptions") != value.end());
	try {
		json::const_iterator f = value.find("options");
		if (f != value.end())
			clientOptions = new ClientOptions(f.value());
		f = value.find("credentials");
		if (f != value.end()) 
			androidCredentials = new AndroidCredentials(f.value());
		f = value.find("keys");
		if (f != value.end()) 
			wpnKeys = new WpnKeys(f.value());
		f = value.find("subscriptions");
		if (f != value.end()) {
			subscriptions = new Subscriptions(f.value());
		}
	} catch(...) {
		r = false;
	}
	return r;
}

void ConfigFile::invalidate() {
	if (!clientOptions) 
		clientOptions = new ClientOptions();
	if (!androidCredentials) 
		androidCredentials = new AndroidCredentials();
	if (!wpnKeys) 
		wpnKeys = new WpnKeys();
	if (!subscriptions) 
		subscriptions = new Subscriptions();
}

void ConfigFile::read(
	std::istream &strm,
	const std::string &delimiter
) {
	clientOptions = new ClientOptions(strm);
	androidCredentials = new AndroidCredentials(strm);
	wpnKeys = new WpnKeys(strm);
	subscriptions = new Subscriptions(strm);
	std::istream::pos_type r = strm.tellg();
}

ConfigFile::ConfigFile(
	const std::string &filename
)
	: errorCode(0), errorDescription(""), fileName(filename), clientOptions(NULL), androidCredentials(NULL), wpnKeys(NULL), subscriptions(NULL)
{
	std::ifstream configRead(filename.c_str());
	if (configRead.fail()) {
		errorCode = ERR_CONFIG_FILE_READ;
		errorDescription = "Error read " + filename;
	} else {
		if (filename.find(".js") != std::string::npos) {
			json j;
			try {
				configRead >> j;
			}
			catch (json::exception e) {
				errorCode = ERR_CONFIG_FILE_PARSE_JSON;
				errorDescription = filename + " error " + e.what();
			}
			catch (...) {
				errorCode = ERR_CONFIG_FILE_PARSE_JSON;
				errorDescription = "Error parse " + filename;

			}
			fromJson(j);
		} else {
			read(configRead);
		}
	}
	configRead.close();
	invalidate();
}

ConfigFile::~ConfigFile()
{
	if (clientOptions)
		delete clientOptions;
	if (androidCredentials)
		delete androidCredentials;
	if (wpnKeys)
		delete wpnKeys;
	if (subscriptions)
		delete subscriptions;
}

std::ostream::pos_type ConfigFile::write(
	std::ostream &strm,
	const std::string &delimiter,
	const int writeFormat
)  const
{
	std::ostream::pos_type r = clientOptions->write(strm, delimiter, writeFormat);
	r += androidCredentials->write(strm, delimiter, writeFormat);
	r += wpnKeys->write(strm, delimiter, writeFormat);
	r += subscriptions->write(strm, delimiter, writeFormat);
	return r;
}

std::ostream::pos_type ConfigFile::save
() const
{
	std::ofstream configWrite(fileName);
	std::ostream::pos_type r;
	if (fileName.find(".js") != std::string::npos) {
		configWrite << toJson().dump(4) << std::endl;
		r = configWrite.tellp();
	} else
		r = write(configWrite);
	configWrite.close();
	return r;
}

json ConfigFile::toJson(
) const
{
	json o = clientOptions->toJson();
	json c = androidCredentials->toJson();
	json k = wpnKeys->toJson();
	json s = subscriptions->toJson();
	json r = {
		{ "options", o},
		{ "credentials", c},
		{ "keys", k },
		{ "subscriptions", s }
	};
	return r;
}
