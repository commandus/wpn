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

#define ERROR_COUNT 6
#define ERR_OK		0
#define ERR_INIT	-1
#define ERR_CONTEXT	-2
#define ERR_SESSION	-3
#define ERR_RESOLVE	-4
#define ERR_CONNECT	-5

void initSSL()
{
	OpenSSL_add_all_algorithms();
//	ERR_load_BIO_strings();
//	ERR_load_crypto_strings();
//	SSL_load_error_strings();
}

static const char *ERR_SSL[ERROR_COUNT] = {
	"OK"
	"Can not initialize OpenSSL",
	"Can not create a new SSL context",
	"Can not build a SSL session",
	"Can not resolve host name",
	"Can not connect to host"
};

static const char *getErrorDescription(int error)
{
	int idx = -error;
	if (idx >= 0 && idx < ERROR_COUNT)
	{
		return ERR_SSL[idx];
	}
	else
		return NULL;
}

/**
 * creates the socket & TCP-connect to serve
 **/
int createTCPsocket
(
	int *sock,
	const char *hostname,
	int port
)
{
	struct hostent *host;
	struct sockaddr_in dest_addr;

	if ((host = gethostbyname(hostname)) == NULL ) 
	{
		return ERR_RESOLVE;
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
		return ERR_CONNECT;
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
		return ERR_INIT;
	}

	// Set SSLv2 client hello, also announce SSLv3 and TLSv1      *
	const SSL_METHOD *method = SSLv23_client_method();

	// Try to create a new SSL context
	if ((*ctx = SSL_CTX_new(method)) == NULL)
	{
		return ERR_CONTEXT;
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
	int socket
) 
{
	// Create new SSL connection state object
	*retval = SSL_new(ctx);

	// Attach the SSL session to the socket descriptor
	SSL_set_fd(*retval, socket);
	
	// Try to SSL-connect here, returns 1 for success
	if (SSL_connect(*retval) != 1)
	{
		return ERR_SESSION;
	}
	return 0;
}

void doneSSLsocket
(
	SSL *ssl,
	int socket
) 
{
#ifdef _MSC_VER
	closesocket(socket);
#else
	close(socket);
#endif
	if (ssl)
		SSL_free(ssl);
	ssl = NULL;
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

SSL *SSLFactory::open
(
	int *socket,
	const std::string &host, 
	int port
)
{
	int r = createTCPsocket(socket, host.c_str(), port);
	if (r)
		return NULL;
	SSL *ret;
	r = createSSLsocket(&ret, mContext, *socket);
	if (r)
		return NULL;
	return ret;
}

void SSLFactory::close
(
	int socket,
	SSL *ssl
)
{
	doneSSLsocket(ssl, socket);
}
