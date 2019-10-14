#include "sslfactory.h"

#ifdef _MSC_VER
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <unistd.h>
#include <resolv.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <string.h>
#include "errlist.h"

void initSSL()
{
	OpenSSL_add_all_algorithms();
//	ERR_load_BIO_strings();
//	ERR_load_crypto_strings();
//	SSL_load_error_strings();
}

/**
 * creates the socket & TCP-connect to serve
 **/
#pragma warning( disable : 4996)
int createTCPsocket
(
	SOCKET *sock,
	const char *hostname,
	int port
)
{
	struct hostent *host;
	struct sockaddr_in dest_addr;

	if ((host = gethostbyname(hostname)) == NULL ) 
	{
		return ERR_SSL_RESOLVE;
	}

	// create the basic TCP socket
	*sock = socket(AF_INET, SOCK_STREAM, 0);

	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(port);
	dest_addr.sin_addr.s_addr = *(long*)(host->h_addr);

	// Zeroing the rest of the struct
	memset(&(dest_addr.sin_zero), '\0', 8);

	char *tmp_ptr = inet_ntoa(dest_addr.sin_addr);
	// Try to connect
	if (connect(*sock, (struct sockaddr *) &dest_addr, sizeof(struct sockaddr)) == -1 ) 
	{
		return ERR_SSL_CONNECT;
	}
	return 0;
}

int createContext
(
	SSL_CTX **ctx
) 
{
	// initialize SSL library and register algorithms
	if (SSL_library_init() < 0)
	{
		return ERR_SSL_INIT;
	}

	// Set SSLv2 client hello, also announce SSLv3 and TLSv1      *
	const SSL_METHOD *method = SSLv23_client_method();

	// Try to create a new SSL context
	if ((*ctx = SSL_CTX_new(method)) == NULL)
	{
		return ERR_SSL_CONTEXT;
	}

	// Disabling SSLv2 will leave v3 and TSLv1 for negotiation
	SSL_CTX_set_options(*ctx, SSL_OP_NO_SSLv2);
	return 0;
}

void doneContext
(
	SSL_CTX *ctx
) 
{
	if (ctx)
		SSL_CTX_free(ctx);
}

int createSSLsocket
(
	SSL **retval,
	SSL_CTX *ctx,
	SOCKET &socket
) 
{
	// Create new SSL connection state object
	*retval = SSL_new(ctx);

	// Attach the SSL session to the socket descriptor
	SSL_set_fd(*retval, socket);
	
	// Try to SSL-connect here, returns 1 for success
	if (SSL_connect(*retval) != 1)
	{
		return ERR_SSL_SESSION;
	}
	return 0;
}

SSLFactory::SSLFactory()
{
	int r = createContext(&mContext);
	if (r)
		mContext = NULL;
}

SSLFactory::~SSLFactory()
{
	doneContext(mContext);
}

SSL *SSLFactory::connect
(
	SOCKET *socket,
	const std::string &host, 
	int port
)
{
	int r = createTCPsocket(socket, host.c_str(), port);
	if (r)
		return NULL;
	struct timeval tv;
	memset(&tv, 0, sizeof(struct timeval));
	tv.tv_sec = 2;  // 2s
	setsockopt(*socket, SOL_SOCKET, SO_RCVTIMEO, (const char *) &tv, sizeof(struct timeval));
	SSL *ret;
	r = createSSLsocket(&ret, mContext, *socket);
	if (r)
		return NULL;
	// SSL_CTX_set_timeout(mContext, 1);	// default 300s
	return ret;
}

void SSLFactory::disconnect
(
	SOCKET &socket,
	SSL *ssl
)
{
#ifdef _MSC_VER
	closesocket(socket);
#else
	close(socket);
#endif
	if (ssl != NULL)
	{
		SSL_shutdown(ssl);
		SSL_free(ssl);
	}
}
