/**
 * Message Encryption for Web Push draft-ietf-webpush-encryption-08
 * https://tools.ietf.org/html/draft-ietf-webpush-encryption-08
 * 
 * https://github.com/web-push-libs/ecec
 * 
 * AndroidCredentials, WpnKeys, Subscriptions
 * Helper utilities
 */
#ifndef WP_STORAGE_FILE_H_
#define WP_STORAGE_FILE_H_	1

#include <string>
#include <iostream>
#include <vector>
#include <ece.h>
#include "nlohmann/json.hpp"

#define DEF_DELIMITER " "	// space character

#define SUBSCRIBE_DB				0
#define SUBSCRIBE_FORCE_FIREBASE	1
#define SUBSCRIBE_FORCE_VAPID		2

#define FORMAT_TEXT					0
#define FORMAT_JSON					1

using json = nlohmann::json;

class ClientOptions
{
private:
	void read(
		std::istream &strm,
		const std::string &delimiter
	);
	void parse(
		const std::string &keys,
		const std::string &delimiter
	);
public:
	ClientOptions();
	ClientOptions(
		const std::string &name
   	);
	ClientOptions(
		std::istream &strm,
		const std::string &delimiter = DEF_DELIMITER		 
	);
	ClientOptions(
		const json &value
	);
	std::string name;

	std::ostream::pos_type write(
		std::ostream &strm,
		const std::string &delimiter = DEF_DELIMITER,
		const int writeFormat = FORMAT_TEXT
	) const;
	std::ostream::pos_type write(
		const std::string &fileName
	) const;
	json toJson(
	) const;
};

class AndroidCredentials
{
private:
	std::string mAppId;
	uint64_t mAndroidId;
	uint64_t mSecurityToken;
	std::string mGCMToken;

	void init(
		const std::string &appId,
		uint64_t androidId,
		uint64_t securityToken,
		const std::string &gcmToken
	);
	void parse(
		const std::string &keys,
		const std::string &delimiter
	);
	void read(
		std::istream &strm,
		const std::string &delimiter
	);
	std::string genAppId();
public:
	AndroidCredentials();
	AndroidCredentials(
		const std::string &appId,
		uint64_t androidId,
		uint64_t securityToken,
		const std::string &fcmToken
   	);
	AndroidCredentials(
		const std::string &keys,
		const std::string &delimiter
	);
	AndroidCredentials(
		std::istream &strm,
		const std::string &delimiter = DEF_DELIMITER		 
	);
	AndroidCredentials(
		const std::string &fileName
	);
	AndroidCredentials(
		const json &value
	);
	const std::string getAppId() const;
	uint64_t getAndroidId() const;
	uint64_t getSecurityToken() const;
	const std::string &getGCMToken() const;
	void setAndroidId(uint64_t value);
	void setSecurityToken(uint64_t value);
	void setGCMToken(const std::string &value);

	std::ostream::pos_type write(
		std::ostream &strm,
		const std::string &delimiter = DEF_DELIMITER,
		const int writeFormat = FORMAT_TEXT
	) const;
	std::ostream::pos_type write(
		const std::string &fileName
	) const;
	json toJson(
	) const;
};

class WpnKeys
{
private:
	// The subscription private key. This key should never be sent to the app
	// server. It should be persisted with the endpoint and auth secret, and used
	// to decrypt all messages sent to the subscription.
	uint8_t privateKey[ECE_WEBPUSH_PRIVATE_KEY_LENGTH];

	// The subscription public key. This key should be sent to the app server,
	// and used to encrypt messages. The Push DOM API exposes the public key via
	// `pushSubscription.getKey("p256dh")`.
	uint8_t publicKey[ECE_WEBPUSH_PUBLIC_KEY_LENGTH];

	// The shared auth secret. This secret should be persisted with the
	// subscription information, and sent to the app server. The DOM API exposes
	// the auth secret via `pushSubscription.getKey("auth")`.
	uint8_t authSecret[ECE_WEBPUSH_AUTH_SECRET_LENGTH];

	void parse(
		const std::string &keys,
		const std::string &delimiter
	);
	void read(
		std::istream &strm,
		const std::string &delimiter
	);
public:
	uint64_t id;
	uint64_t secret;
	void init(
		uint64_t id,
		uint64_t secret,
		const std::string &private_key,
		const std::string &public_key,
		const std::string &auth_secret
	);
	void init(
		const WpnKeys &wpnKeys
	);
	WpnKeys();
	WpnKeys(
		const WpnKeys &wpnKeys
	);
	WpnKeys(
		uint64_t id,
		uint64_t secret,
		const std::string &private_key,
		const std::string &public_key,
		const std::string &auth_secret
   	);
	WpnKeys(
		uint64_t id,
		uint64_t secret,
		const std::string &keys,
		const std::string &delimiter
	);
	WpnKeys(
		std::istream &strm,
		const std::string &delimiter = DEF_DELIMITER		 
	);
	WpnKeys(
		const std::string &fileName
	);
	WpnKeys(
		const json &value
	);
	std::string getPrivateKey() const;
	const uint8_t *getPrivateKeyArray() const;
	std::string getPublicKey() const;
	void setPublicKey(const std::string &value);
	std::string getAuthSecret() const;
	const uint8_t *getAuthSecretArray() const;
	
	std::ostream::pos_type write(
		std::ostream &strm,
		const std::string &delimiter = DEF_DELIMITER,
		const int writeFormat = FORMAT_TEXT
	) const;
	std::ostream::pos_type write(
		const std::string &fileName
	) const;

	json toJson(
	) const;

	int generate();
};

class Subscription
{
private:
	std::string name;
	std::string subscribeUrl;
	int subscribeMode;						///<  1: FCM       - 2: VAPID
	std::string endpoint;
	std::string serverKey;				///< FCM server key - VAPID private key
	std::string authorizedEntity;	///< FCM entity     - VAPID public key
	std::string token;						///< FCM token      - VAPID auth secret
	std::string pushSet;
	std::string persistentId;		///< last received message id for unknown reason
	WpnKeys wpnKeys;
	/// Initialize FCM
	void initFCM(
		const std::string &name,
		const std::string &subscribeUrl,
		const std::string &endpoint,
		const std::string &serverKey,
		const std::string &authorizedEntity,
		const std::string &token,
		const std::string &pushSet,
		const std::string &persistentId
	);
	/// Initialize VAPID
	void initVAPID(
		const std::string &name,
		const std::string &endpoint,
		const std::string &persistentId,
		uint64_t id,
		uint64_t secret,
		const std::string &publicKey,
		const std::string &privateKey,
		const std::string &authSecret
	);
	/// Initialize VAPID 1
	void initVAPID1(
		const std::string &name,
		const std::string &endpoint,
		const std::string &persistentId,
		const WpnKeys *wpnKeys
	);
	void parse(
		const std::string &keys,
		const std::string &delimiter
	);
	void read(
		std::istream &strm,
		const std::string &delimiter
	);

public:
	void fromStream(
		uint64_t id,
		std::istream &strm
	);
	void fromJson(const json &value);
	Subscription();
	// FCM
	Subscription(
		const std::string &name,
		const std::string &subscribeUrl,
		const std::string &endpoint,
		const std::string &serverKey,
		const std::string &authorizedEntity,
		const std::string &token,
		const std::string &pushSet,
		const std::string &persistentId
	);

	// VAPID
	Subscription(
		uint64_t id,
		uint64_t secret,
		const std::string &name,
		const std::string &endpoint,
		const std::string &privateKey,
		const std::string &publicKey,
		const std::string &authSecret,
		const std::string &persistentId
	);

	Subscription(
		uint64_t id,
		const std::string &name,
		const std::string &publicKey
	);

	Subscription(
		std::istream &strm,
		const std::string &delimiter = DEF_DELIMITER
	);
	Subscription(
		uint64_t id,
		std::istream &strm
	);
	Subscription(
		const std::string &fileName
	);
	Subscription(
		const json &value
	);

	/// constructor for find only
	Subscription(
		const std::string &endpoint,
		const std::string &authorizedEntity
	);

	std::string getName() const;
	std::string getSubscribeUrl() const;
	int getSubscribeMode() const;
	std::string getEndpoint() const;
	std::string getServerKey() const;
	std::string getAuthorizedEntity() const;
	std::string getToken() const;
	bool hasToken() const;
	std::string getPushSet() const;
	const WpnKeys &getWpnKeys() const;
	WpnKeys* getWpnKeysPtr();

	void setName(const std::string &value);
	void setSubscribeUrl(const std::string &value);
	void setSubscribeMode(int value);
	void setEndpoint(const std::string &value);
	void setServerKey(const std::string &value);
	void setAuthorizedEntity(const std::string &value);
	void setToken(const std::string &value);
	void setPushSet(const std::string &value);
	void setWpnKeys(const WpnKeys &value);

	const std::string &getPersistentId() const;
	void setPersistentId(const std::string &value);

	std::ostream::pos_type write(
		std::ostream &strm,
		const std::string &delimiter,
		const int writeFormat = FORMAT_TEXT
	) const;
	std::ostream::pos_type write(
		const std::string &fileName
	) const;

	json toJson(
	) const;

	bool valid() const;
	bool operator==(const Subscription &val) const;
};

class Subscriptions
{
private:
	std::string receivedPersistentId;
	void read(
		std::istream &strm,
		const std::string &delimiter
	);
public:
	Subscriptions();
	Subscriptions(
		std::istream &strm,
		const std::string &delimiter = DEF_DELIMITER
	);
	Subscriptions(
		const std::string &fileName
	);
	Subscriptions(
		const json &value
	);
	const std::string & getReceivedPersistentId() const;
	void setReceivedPersistentId(const std::string &value);
	std::vector<Subscription> list;

	std::ostream::pos_type write(
		std::ostream &strm,
		const std::string &delimiter = DEF_DELIMITER,
		const int writeFormat = FORMAT_TEXT
	)  const;
	std::ostream::pos_type write(
		const std::string &fileName
	) const;
	json toJson(
	) const;
	Subscription *getById(uint64_t id) const;
	void putSubsciptionVapidPubKey(
		uint64_t id,
		const std::string &vapidPublicKey
	);
	bool rmById(
		uint64_t id
	);

};

class ConfigFile
{
private:
	void read(
		std::istream &strm,
		const std::string &delimiter = DEF_DELIMITER
	);
	bool fromJson(const json &value);
public:
	ClientOptions *clientOptions;
	AndroidCredentials *androidCredentials;
	WpnKeys *wpnKeys;
	Subscriptions *subscriptions;

	ConfigFile(
		const std::string &fileName
	);
	ConfigFile(
		const json &value
	);

	~ConfigFile();
	
	std::ostream::pos_type write(
		std::ostream &strm,
		const std::string &delimiter = DEF_DELIMITER,
		const int writeFormat = FORMAT_TEXT
	)  const;

	std::ostream::pos_type save(
		const std::string &fileName
	) const;

	json toJson() const;
};

void generateVAPIDKeys
(
	std::string &privateKey,
	std::string &publicKey,
	std::string &authSecret
);

#endif
