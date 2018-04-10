/**
 * Message Encryption for Web Push draft-ietf-webpush-encryption-08
 * https://tools.ietf.org/html/draft-ietf-webpush-encryption-08
 * 
 * https://github.com/web-push-libs/ecec
 * 
 * Helper utilities
 */
#ifndef WP_ENCRYPTION_H
#define WP_ENCRYPTION_H

#include <string>
#include <iostream>

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
	void init2(
		const std::string &keys,
		const std::string &delimiter
	);
	void init3(
		std::istream &strm
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
		std::istream &strm
	);
	WpnKeys(
		const std::string &fileName
	);

	std::string getPrivateKey();
	std::string getPublicKey();
	std::string getAuthSecret();
	
	void save(std::ostream &strm);
};

#endif
