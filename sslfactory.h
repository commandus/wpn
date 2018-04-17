#ifndef SSLFACTORY_H
#define SSLFACTORY_H

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
	SSL *open(
		int *socket,
		const std::string &host, 
		int port
	);
	void close(int socket, SSL *ssl);
};

#endif // SSLFACTORY_H
