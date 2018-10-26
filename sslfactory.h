#ifndef SSLFACTORY_H
#define SSLFACTORY_H	1

#include <string>
#include <openssl/ssl.h>

void initSSL();

class SSLFactory
{
private:
	SSL_CTX *mContext;
public:
	SSLFactory();
	~SSLFactory();
	SSL *connect(
		int *socket,
		const std::string &host, 
		int port
	);
	void disconnect(int socket, SSL *ssl);
};

#endif // SSLFACTORY_H
