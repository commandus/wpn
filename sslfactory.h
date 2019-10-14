#ifndef SSLFACTORY_H
#define SSLFACTORY_H	1

#include <string>
#include <openssl/ssl.h>
#include "platform.h"

void initSSL();

class SSLFactory
{
private:
	SSL_CTX *mContext;
public:
	SSLFactory();
	~SSLFactory();
	SSL *connect(
		SOCKET *socket,
		const std::string &host, 
		int port
	);
	void disconnect(SOCKET &socket, SSL *ssl);
};

#endif // SSLFACTORY_H
