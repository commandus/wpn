#ifndef SSLFACTORY_H
#define SSLFACTORY_H	1

#include <string>
#include <openssl/ssl.h>
#include "platform.h"

#define ERR_OK		0
#define ERR_INIT	-1
#define ERR_CONTEXT	-2
#define ERR_SESSION	-3
#define ERR_RESOLVE	-4
#define ERR_CONNECT	-5
#define ERR_NO_SSL	-6

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
