#include <ece.h>
#include "utilvapid.h"
// #include "sole/sole.hpp"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

std::string generateVAPIDKeysJSON()
{
	// The subscription private key. This key should never be sent to the app
	// server. It should be persisted with the endpoint and auth secret, and used
	// to decrypt all messages sent to the subscription.
	uint8_t rawRecvPrivKey[ECE_WEBPUSH_PRIVATE_KEY_LENGTH];

	// The subscription public key. This key should be sent to the app server,
	// and used to encrypt messages. The Push DOM API exposes the public key via
	// `pushSubscription.getKey("p256dh")`.
	uint8_t rawRecvPubKey[ECE_WEBPUSH_PUBLIC_KEY_LENGTH];

	// The shared auth secret. This secret should be persisted with the
	// subscription information, and sent to the app server. The DOM API exposes
	// the auth secret via `pushSubscription.getKey("auth")`.
	uint8_t rawAuthSecret[ECE_WEBPUSH_AUTH_SECRET_LENGTH];

	int err = ece_webpush_generate_keys(
		rawRecvPrivKey, ECE_WEBPUSH_PRIVATE_KEY_LENGTH, rawRecvPubKey,
		ECE_WEBPUSH_PUBLIC_KEY_LENGTH, rawAuthSecret, ECE_WEBPUSH_AUTH_SECRET_LENGTH
	);
	if (err) {
		return "";
	}
	
	char base64RecvPrivKey[ECE_WEBPUSH_PRIVATE_KEY_LENGTH * 3];
	std::string privateKey(base64RecvPrivKey, 
		ece_base64url_encode(rawRecvPrivKey, ECE_WEBPUSH_PRIVATE_KEY_LENGTH, 
		ECE_BASE64URL_OMIT_PADDING, base64RecvPrivKey, sizeof(base64RecvPrivKey)));
	char base64RecvPubKey[ECE_WEBPUSH_PUBLIC_KEY_LENGTH * 3];
	std::string publicKey(base64RecvPubKey, 
		ece_base64url_encode(rawRecvPubKey, ECE_WEBPUSH_PUBLIC_KEY_LENGTH, ECE_BASE64URL_OMIT_PADDING, 
		base64RecvPubKey, sizeof(base64RecvPubKey)));
	char base64AuthSecret[ECE_WEBPUSH_AUTH_SECRET_LENGTH * 3];
	std::string authSecret(base64AuthSecret, 
		ece_base64url_encode(rawAuthSecret, ECE_WEBPUSH_AUTH_SECRET_LENGTH, ECE_BASE64URL_OMIT_PADDING, 
		base64AuthSecret, sizeof(base64AuthSecret)));
	json j = {
		{"privateKey", privateKey},
		{"publicKey", publicKey},
		{"authSecret", authSecret}
	};
	return j.dump();
}
