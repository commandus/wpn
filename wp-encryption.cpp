#include <inttypes.h>

#include <ece.h>
#include <fstream>
#include "wp-encryption.h"

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
	init2(keys, delimiter);
}

WpnKeys::WpnKeys(
	std::istream &strm
)
{
	init3(strm);
}

WpnKeys::WpnKeys(
	const std::string &fileName
)
{
	std::ifstream strm(fileName);
	init3(strm);
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

void WpnKeys::init2(
	const std::string &keys,
	const std::string &delimiter
)
{
	std::string k[3];

	size_t p0 = 0, p1;
	int i = 0;
	while ((p1 = keys.find(delimiter, p0))) 
	{
		k[i] = keys.substr(p0, p1);
		p0 = p1 + delimiter.length();
		i++;
		if (i >= 3)
			break;
	}
}

void WpnKeys::init3(
	std::istream &strm
)
{
	std::string keys;
	std::getline(strm, keys);
	init2(keys, " ");
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

std::string WpnKeys::getPrivateKey()
{
	char r[ECE_WEBPUSH_PRIVATE_KEY_LENGTH * 2];
	return std::string(r, ece_base64url_encode(privateKey, ECE_WEBPUSH_PRIVATE_KEY_LENGTH, ECE_BASE64URL_OMIT_PADDING, r, sizeof(r)));
}

std::string WpnKeys::getPublicKey()
{
	char r[ECE_WEBPUSH_PUBLIC_KEY_LENGTH * 2];
	return std::string(r, ece_base64url_encode(publicKey, ECE_WEBPUSH_PUBLIC_KEY_LENGTH, ECE_BASE64URL_OMIT_PADDING, r, sizeof(r)));
}

std::string WpnKeys::getAuthSecret()
{
	char r[ECE_WEBPUSH_AUTH_SECRET_LENGTH* 2];
	return std::string(r, ece_base64url_encode(authSecret, ECE_WEBPUSH_AUTH_SECRET_LENGTH, ECE_BASE64URL_OMIT_PADDING, r, sizeof(r)));
}

void WpnKeys::save(std::ostream &strm)
{
	strm << getPrivateKey() << " " << getPublicKey() << " " << getAuthSecret() << std::endl;
}

void WpnKeys::save(const std::string &fileName)
{
	std::ofstream strm(fileName);
	save(strm);
	strm.close();
}
