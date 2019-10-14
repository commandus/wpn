#include "errlist.h"
#include <cstddef>

#define ERROR_ECE_COUNT                 23
#define ERROR_SSL_COUNT                 7
#define ERROR_WPN_COUNT                 37

static const char *ERR_ECEC[ERROR_ECE_COUNT] = {
	"OK",
    "Out of memory",
    "Invalid private key",
    "Invalid public key",
    "Error compute secret",
    "Error encode public key",
    "Error decrypt",
    "Error decrypt padding",
    "Error zero plain text",
    "Too short block",
    "Too short header",
    "Zero cipher text",
    "Error HKDF",
    "Invalid Encryption header",
    "Invalid Crypto-key header",
    "Invalid rs",
    "Invalid salt",
    "Invalid dh",
    "Error encrypt",
    "Error encrypt padding",
    "Invalid auth secret",
    "Error generate keys",
    "Error decrypt truncated"
};

static const char *ERR_WPN[ERROR_WPN_COUNT] = {
    // SSL factory
	"OK",                               // 50
	"Can not initialize OpenSSL",
	"Can not create a new SSL context",
	"Can not build a SSL session",
	"Can not resolve host name",
	"Can not connect to host",          // 55
	"SSL not initialized",

    // subscription
    "Subscription not found",
    "Subscription token not found",
    "Register subscription error",
    "No android id and/or token",       // 60
    "No FCM token",

    // storage file
    "Can not read config file",
    "Error parse config file",

    // cmd output
    "failed to run command",
    "failed create pipe.",               // 65
    "failed set up info.",

    // wp-subscribe
    "Wrong mode",
    "Wrong endpoint",
    "Wrong auth entity",
    "Connection error",                 // 70
    "Subscribe error",

    // recv
    "Wrong parameter",
    "Wrong register value",
    "Register failed",

    // wp-push
    "Wrong server key",                 // 75
    "Wrong client token",
    "Error parse response",

    // wp-connection
    "Insufficient buffer size",          // 78

    // mcs client
    "No configuration is provided",
    "No keys are provided",              // 80
    "No credentials is provided",
    "Checkin error",
    "No connection",
    "Memory fault",
    "Disconnected",                       // 85
    "WSA error"
};

/**
 * @param module- 0- wpn, 1- ecec, 2- ssl
 **/
static const char *getErrorDescription(
    int error,
    const int module = 0
)
{
    int idx = error < 0 ? -error : error;
	if (idx < 0) {
        return NULL;
    }
    switch (module)
    {
    case 1:
        if (idx < ERROR_ECE_COUNT) {
            return NULL;
        }
        return ERR_ECEC[idx];
    case 2:
        if (idx < ERROR_SSL_COUNT) {
            return NULL;
        }
        // TODO
        return "";
    default:
        idx -= ERR_OFFSET_WPN;      // 50
        if (idx < ERROR_WPN_COUNT) {
            return NULL;
        }
        return ERR_WPN[idx];
    }
}
