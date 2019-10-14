#define ERR_OK		0

// ece.h
#define ECE_ERROR_OUT_OF_MEMORY             -1
#define ECE_ERROR_INVALID_PRIVATE_KEY       -2
#define ECE_ERROR_INVALID_PUBLIC_KEY        -3
#define ECE_ERROR_COMPUTE_SECRET            -4
#define ECE_ERROR_ENCODE_PUBLIC_KEY         -5
#define ECE_ERROR_DECRYPT                   -6
#define ECE_ERROR_DECRYPT_PADDING           -7
#define ECE_ERROR_ZERO_PLAINTEXT            -8
#define ECE_ERROR_SHORT_BLOCK               -9
#define ECE_ERROR_SHORT_HEADER              -10
#define ECE_ERROR_ZERO_CIPHERTEXT           -11
#define ECE_ERROR_HKDF                      -12
#define ECE_ERROR_INVALID_ENCRYPTION_HEADER -13
#define ECE_ERROR_INVALID_CRYPTO_KEY_HEADER -14
#define ECE_ERROR_INVALID_RS                -15
#define ECE_ERROR_INVALID_SALT              -16
#define ECE_ERROR_INVALID_DH                -17
#define ECE_ERROR_ENCRYPT                   -18
#define ECE_ERROR_ENCRYPT_PADDING           -19
#define ECE_ERROR_INVALID_AUTH_SECRET       -20
#define ECE_ERROR_GENERATE_KEYS             -21
#define ECE_ERROR_DECRYPT_TRUNCATED         -22


#define ERR_OFFSET_WPN                      50

// SSL factory
#define ERR_SSL_INIT	                    -51
#define ERR_SSL_INIT	                    -51
#define ERR_SSL_CONTEXT	                    -52
#define ERR_SSL_SESSION	                    -53
#define ERR_SSL_RESOLVE	                    -54
#define ERR_SSL_CONNECT	                    -55
#define ERR_SSL_NO_SSL	                    -56

// subscription
#define ERR_SUBSCRIPTION_NOT_FOUND			-57
#define ERR_SUBSCRIPTION_TOKEN_NOT_FOUND	-58
#define	ERR_REGISTER_SUBSCRIPTION			-59
#define ERR_NO_ANDROID_ID_N_TOKEN			-60
#define ERR_NO_FCM_TOKEN					-61

// storage file
#define ERR_CONFIG_FILE_READ		        -62
#define ERR_CONFIG_FILE_PARSE_JSON	        -63

// cmd output
#define ERR_FAILED_RUN		                -64	// Failed to run command
#define ERR_FAILED_PIPE		                -65	// Failed create pipe. Windows only
#define ERR_PIPE_SET_INFO	                -66	// Failed set up info. Windows only

// wp-subscribe
#define ERR_MODE					        -67
#define ERR_PARAM_ENDPOINT			        -68
#define ERR_PARAM_AUTH_ENTITY		        -69
#define ERR_CONNECTION				        -70
#define	ERR_SUBSCRIBE				        -71

// recv
#define ERR_WRONG_PARAM				        -72
#define ERR_REGISTER_VAL			        -73	// Error registering
#define ERR_REGISTER_FAIL			        -74

// wp-push
#define ERR_PARAM_SERVER_KEY		        -75
#define ERR_PARAM_CLIENT_TOKEN		        -76
#define ERR_PARSE_RESPONSE			        -77

// wp-connection
#define ERR_INSUFFICIENT_SIZE               -78

// mcs client
#define ERR_NO_CONFIG       				-79
#define ERR_NO_KEYS				        	-80
#define ERR_NO_CREDS				        -81
#define ERR_CHECKIN					        -82
#define ERR_NO_CONNECT				        -83
#define ERR_MEM						        -84
#define ERR_DISCONNECTED			        -85

#define ERR_WSA		                        -86     // Windows only
