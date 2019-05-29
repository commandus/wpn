#include <inttypes.h>

#include <fstream>
#include "utilinstance.h"
#include "wp-storage-file.h"
#include "utilstring.h"
#include "utilvapid.h"
#include "endpoint.h"

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
	init(value["appId"], value["androidId"], value["securityToken"], value["GCMToken"]);
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
}

WpnKeys::WpnKeys (
	const WpnKeys &value
) {
	init(value);
}

WpnKeys::WpnKeys
(
	uint64_t id,
	const std::string &private_key,
	const std::string &public_key,
	const std::string &auth_secret
)
{
	init(id, private_key, public_key, auth_secret);
}

WpnKeys::WpnKeys
(
	uint64_t id,
	const std::string &keys,
	const std::string &delimiter
)
{
	this->id = id;
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
	init(value["id"], value["privateKey"], value["publicKey"], value["authSecret"]);
}

void WpnKeys::init(
	uint64_t id,
	const std::string &private_key,
	const std::string &public_key,
	const std::string &auth_secret
)
{
	this->id = id;
	ece_base64url_decode(private_key.c_str(), private_key.size(), ECE_BASE64URL_REJECT_PADDING, privateKey, ECE_WEBPUSH_PRIVATE_KEY_LENGTH);
	ece_base64url_decode(public_key.c_str(), public_key.size(), ECE_BASE64URL_REJECT_PADDING, publicKey, ECE_WEBPUSH_PUBLIC_KEY_LENGTH);
	ece_base64url_decode(auth_secret.c_str(), auth_secret.size(), ECE_BASE64URL_REJECT_PADDING, authSecret, ECE_WEBPUSH_AUTH_SECRET_LENGTH);
}

void WpnKeys::init(
	const WpnKeys &value
)
{
	init(value.id, value.getPrivateKey(), value.getPublicKey(), value.getAuthSecret());
}

void WpnKeys::parse(
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
		if (i >= 3)
			break;
	}
	if (k[2].empty())
		generate();
	else
		init(std::stoi(k[0]), k[1], k[2], k[3]); 
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
			strm << id << delimiter << getPrivateKey() << delimiter << getPublicKey() << delimiter << getAuthSecret() << std::endl;
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
		{ "privateKey", getPrivateKey() },
		{ "publicKey", getPublicKey() },
		{ "authSecret", getAuthSecret() }
	};
	return r;
}

// --------------- Subscription ---------------

Subscription::Subscription()
	: name(""), subscribeUrl(""), subscribeMode(0), endpoint(""), authorizedEntity(""),
	token(""), pushSet(""), persistentId("")
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
	authorizedEntity(a_authorizedEntity), token(a_token), pushSet(a_pushSet),
	persistentId(aPersistentId)
{
	name = escapeURLString(aName);
}

// VAPID
Subscription::Subscription(
	uint64_t id,
	const std::string &a_name,
	const std::string &a_endpoint,
	const std::string &a_privateKey,
	const std::string &a_publicKey,
	const std::string &a_authSecret,
	const std::string &aPersistentId
)
	: subscribeUrl(""), subscribeMode(SUBSCRIBE_FORCE_VAPID), endpoint(a_endpoint), 
	serverKey(""), authorizedEntity(""), token(""), pushSet(""),
	persistentId(aPersistentId)
{
	name = escapeURLString(a_name);
	setWpnKeys(WpnKeys(id, a_privateKey, a_publicKey, a_authSecret));
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
	strm.close();
}

Subscription::Subscription(
	const json &value
)
{
	this->subscribeMode = value["subscribeMode"];
	switch (this->subscribeMode) {
	case SUBSCRIBE_FORCE_FIREBASE:
		this->name = value["name"];
		this->subscribeUrl = value["subscribeUrl"];
		this->endpoint = value["endpoint"];
		this->authorizedEntity = value["authorizedEntity"];
		this->token = value["token"];
		this->pushSet = value["pushSet"];
		this->persistentId = value["persistentId"];
		this->serverKey = value["serverKey"];
		break;
	case SUBSCRIBE_FORCE_VAPID:
	{
		this->wpnKeys.init(value["id"], value["publicKey"], value["privateKey"], value["authSecret"]);
		this->name = value["name"];
		this->endpoint = value["endpoint"];
		this->persistentId = value["persistentId"];
		break;
	}
	default:
		break;
	}
}

std::string Subscription::getName() const
{
	return name;
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
			{ "subscribeMode", getSubscribeMode() },
			{ "name", getName() },
			{ "endpoint", getEndpoint() },
			{ "publicKey", wpnKeys.getPublicKey() },
			{ "privateKey", wpnKeys.getPrivateKey() },
			{ "authSecret", wpnKeys.getAuthSecret() },
			{ "persistentId", getPersistentId() }
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
	const std::string &a_public_key,	// VAPID public key
	const std::string &a_private_key,	// VAPID private key
	const std::string &a_auth_secret	// VAPID auth secret
)
{
	WpnKeys wpnKeys(id, a_public_key, a_private_key, a_auth_secret);
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
				// 0- SubscribeMode 1- Name 2- Endpoint 3- PersistentId 4, 5, 6, 7- WpnKeys
				initVAPID(k[1], k[2], k[3], std::stoi(k[4]), k[5], k[6], k[7]); 
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
		if (!s.valid())
			break;
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
	bool r;
	androidCredentials = NULL;
	wpnKeys = NULL;
	subscriptions = NULL;
	try {
		r = (value.find("credentials") != value.end())
			&& (value.find("keys") != value.end())
			&& (value.find("subscriptions") != value.end());
		if (r) 
		{
			androidCredentials = new AndroidCredentials(value["credentials"]);
			wpnKeys = new WpnKeys(value["keys"]);
			subscriptions = new Subscriptions(value["subscriptions"]);
		}
	} catch(...) {
		r = false;
	}
	if (!androidCredentials) 
		androidCredentials = new AndroidCredentials();
	if (!wpnKeys) 
		wpnKeys = new WpnKeys();
	if (!subscriptions) 
		subscriptions = new Subscriptions();
	return r;
}

void ConfigFile::read(
	std::istream &strm,
	const std::string &delimiter
) {
	androidCredentials = new AndroidCredentials(strm);
	wpnKeys = new WpnKeys(strm);
	subscriptions = new Subscriptions(strm);
	std::istream::pos_type r = strm.tellg();
}

ConfigFile::ConfigFile(
	const std::string &fileName
) {
	std::ifstream configRead(fileName.c_str());
	if (fileName.find(".js") != std::string::npos) {
		json j;
		try {
			configRead >> j;
		}
		catch (...) {
			std::cerr << "Error parse " << fileName << std::endl;
		}
		fromJson(j);
	} else {
		read(configRead);
	}
	configRead.close();
}

ConfigFile::ConfigFile(
	const json &value
) {
	fromJson(value);
}

std::ostream::pos_type ConfigFile::write(
	std::ostream &strm,
	const std::string &delimiter,
	const int writeFormat
)  const
{
	std::ostream::pos_type r = androidCredentials->write(strm, delimiter, writeFormat);
	r += wpnKeys->write(strm, delimiter, writeFormat);
	r += subscriptions->write(strm, delimiter, writeFormat);
	return r;
}

std::ostream::pos_type ConfigFile::save(
	const std::string &fileName
) const
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
	json c = androidCredentials->toJson();
	json k = wpnKeys->toJson();
	json s = subscriptions->toJson();
	json r = {
		{ "credentials", c},
		{ "keys", k },
		{ "subscriptions", s }
	};
	return r;
}
