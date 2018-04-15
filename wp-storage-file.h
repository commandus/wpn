/**
 * Message Encryption for Web Push draft-ietf-webpush-encryption-08
 * https://tools.ietf.org/html/draft-ietf-webpush-encryption-08
 * 
 * https://github.com/web-push-libs/ecec
 * 
 * Helper utilities
 */
#ifndef WP_STORAGE_FILE_H
#define WP_STORAGE_FILE_H

#include <string>
#include <iostream>
#include <vector>
#include <ece.h>

#define DEF_DELIMITER " "

class AndroidCredentials
{
private:
	uint64_t mAndroidId;
    uint64_t mSecurityToken;

	void init(
		uint64_t androidId,
		uint64_t securityToken
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
	AndroidCredentials();
	AndroidCredentials(
		uint64_t androidId,
		uint64_t securityToken
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

	uint64_t getAndroidId() const;
	uint64_t getSecurityToken() const;
	void setAndroidId(uint64_t value);
	void setSecurityToken(uint64_t value);

	void write(
		std::ostream &strm,
		const std::string &delimiter = DEF_DELIMITER
	) const;
	void write(
		const std::string &fileName
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

	int generate();
	void init(
		const std::string &private_key,
		const std::string &public_key,
		const std::string &auth_secret
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
	WpnKeys();
	WpnKeys(
		const std::string &private_key,
		const std::string &public_key,
		const std::string &auth_secret
   	);
	WpnKeys(
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

	std::string getPrivateKey() const;
	std::string getPublicKey() const;
	std::string getAuthSecret() const;
	
	void write(
		std::ostream &strm,
		const std::string &delimiter = DEF_DELIMITER
	) const;
	void write(
		const std::string &fileName
	) const;
};

class Subscription
{
private:
	std::string subscribeUrl;
	int subscribeMode;
	std::string endpoint;
	std::string authorizedEntity;
	std::string token;
	std::string pushSet;
	void init(
		std::string subscribeUrl,
		int subscribeMode,
		const std::string &endpoint,
		const std::string &authorizedEntity,
		const std::string &token,
		const std::string &pushSet
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
	Subscription();
	Subscription(
		std::string subscribeUrl,
		int subscribeMode,
		const std::string &endpoint,
		const std::string &authorizedEntity,
		const std::string &token,
		const std::string &pushSet
	);
	Subscription(
		std::istream &strm,
		const std::string &delimiter = DEF_DELIMITER
	);
	Subscription(
		const std::string &fileName
	);

	std::string getSubscribeUrl() const;
	int getSubscribeMode() const;
	std::string getEndpoint() const;
	std::string getAuthorizedEntity() const;
	std::string getToken() const;
	std::string getPushSet() const;

	void setSubscribeUrl(const std::string &value);
	void setSubscribeMode(int value);
	void setEndpoint(const std::string &value);
	void setAuthorizedEntity(const std::string &value);
	void setToken(const std::string &value);
	void setPushSet(const std::string &value);
	
	void write(
		std::ostream &strm,
		const std::string &delimiter
	) const;
	void write(
		const std::string &fileName
	) const;
	bool valid() const;
};

class Subscriptions
{
private:
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
	
	std::vector<Subscription> list;
	
	void write(
		std::ostream &strm,
		const std::string &delimiter = DEF_DELIMITER
	)  const;
	void write(
		const std::string &fileName
	) const;
};

#endif
