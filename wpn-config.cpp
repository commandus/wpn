#include "wpn-config.h"
#include <iostream>
#include <argtable3/argtable3.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>

#include "google/protobuf/stubs/common.h"

#include "config-filename.h"
#ifdef _MSC_VER
#else
#include <dlfcn.h>
#endif

#include "platform.h"
#include "utilstring.h"

#define DEF_FILE_NAME			".wpn.js"
#define DEF_FCM_ENDPOINT_PREFIX	"https://fcm.googleapis.com/fcm/send/"
#define DEF_OUTPUT_SO_FN		"libwpn-stdout.so"
#define DEF_FUNC_NOTIFY			"desktopNotify"
#define DEF_COMMON_NAME			"everyone"

#ifdef _MSC_VER
#define DEF_PLUGIN_FILE_EXT		".dll"
#else
#define DEF_PLUGIN_FILE_EXT		".so"
#endif

#define DEF_FILE_NAME			".wpn.js"

static const char* progname = "wpn";

static const char* SUBSCRIBE_URLS[SUBSCRIBE_URL_COUNT] = {
	SUBSCRIBE_URL_1
};

std::string WpnConfig::getDefaultFCMEndPoint()
{
	std::string r(DEF_FCM_ENDPOINT_PREFIX);
	if (config->androidCredentials)
		r = r + config->androidCredentials->getGCMToken();
	return r;
}

WpnConfig::WpnConfig
(
	int argc,
	char* argv[]
)
{
	errorcode = parseCmd(argc, argv);
}

WpnConfig::~WpnConfig()
{
	if (config)
		delete config;
}

/**
 * Parse command line into WpnConfig class
 * Return 0- success
 *        1- show help and exit, or command syntax error
 *        2- output file does not exists or can not open to write
 **/
int WpnConfig::parseCmd
(
	int argc,
	char* argv[]
)
{
	subscriptionMode = SUBSCRIBE_DB;
	struct arg_lit *a_list = arg_lit0("P", "list", "Print subscription list");
	struct arg_lit *a_list_qrcode = arg_lit0("q", "qrcode", "QRCode list subscriptions");
	struct arg_lit *a_invert_qrcode = arg_lit0("Q", "qrcode-inverted", "inverted QR code (white console)");
	struct arg_str *a_list_email = arg_str0("M", "mailto", "<common name>", "e-mail list subscriptions to the person. Use with optional --subject --template-file ");
	struct arg_lit *a_link_email = arg_lit0("E", "link", "list subscriptions link");
	struct arg_lit *a_keys = arg_lit0("y", "keys", "Print VAPID keys");
	struct arg_lit *a_credentials = arg_lit0(NULL, "id", "Print device identifiers and security tokens");
	struct arg_lit *a_subscribe_vapid = arg_lit0("s", "subscribe", "Subscribe with VAPID. Mandatory -u -n --private-key --public-key --auth-secret");
	struct arg_lit *a_subscribe_fcm = arg_lit0("S", "subscribe-fcm", "Subscribe with FCM. Mandatory -e -n, optional -r, -k");
	struct arg_lit *a_unsubscribe = arg_lit0("u", "unsubscribe", "Unsubscribe with -e");
	struct arg_lit *a_send = arg_lit0("m", "message", "Send message with -k (FCM), -d, -a (VAPID) or -n; execute -x. Or -t, -b, -i, -a");
	struct arg_str *a_sub = arg_str0(NULL, "sub", "<URL>", "sub link e.g. mailto://alice@acme.com");
	struct arg_str *a_file_name = arg_str0("c", "config", "<file>", "Configuration file. Default ~/" DEF_FILE_NAME);

	struct arg_str *a_name = arg_str0("n", "name", "<name>", "Subscription name");
	struct arg_str *a_subscribe_url = arg_str0("r", "registrar", "<URL>", "Subscription registrar URL, like https://fcm.googleapis.com/fcm/connect/subscribe or 1. Default 1");
	struct arg_str *a_authorized_entity = arg_str0("e", "entity", "<entity-id>", "Push message sender identifier, usually decimal number");

	// send options
	struct arg_str *a_server_key = arg_str0("K", "fcm-key", "<key>", "FCM server key to send");
	struct arg_str *a_recipient_tokens = arg_strn(NULL, NULL, "<account#>", 0, 100, "Recipient token.");
	struct arg_str *a_recipient_token_file = arg_str0("j", "json", "<file name or URL>", "Recipient token JSON file e.g. [[1,\"token\",..");

	// notification options
	struct arg_str *a_subject = arg_str0("t", "subject", "<Text>", "Subject (topic)");
	struct arg_str *a_body = arg_str0("b", "body", "<Text>", "Body");
	struct arg_str *a_icon = arg_str0("i", "icon", "<URL>", "http[s]:// icon address.");
	struct arg_str *a_link = arg_str0("l", "link", "<URL>", "http[s]:// action address.");
	struct arg_str *a_data = arg_str0("D", "data", "<text>", "Extra data");
	struct arg_str *a_command = arg_str0("x", "execute", "<command line>", "e.g. ls");

	// VAPID sender's options
	struct arg_str *a_contact = arg_str0("f", "from", "<email>", "Sender's email e.g. mailto:alice@acme.com");
	// VAPID subscriber's options
	struct arg_str *a_p256dh = arg_str0("d", "p256dh", "<key>", "Recipient's p256dh public key");
	struct arg_str *a_auth = arg_str0("a", "auth", "<secret>", "Recipient's auth secret");

	// other options
	struct arg_str *a_output = arg_str0("o", "format", "<text|json>", "Output format. Default text.");
	struct arg_str *a_template_file = arg_str0(NULL, "template-file", "<file>", "e-mail HTML template file with $name $subject $body");
	// output options
	struct arg_str *a_output_lib_filenames = arg_strn(NULL, "plugin", "<file name>", 0, 100, "Output shared library file name or directory");
	struct arg_str *a_notify_function_name = arg_str0(NULL, "plugin-func", "<name>", "Output function name. Default " DEF_FUNC_NOTIFY);

	// other
	struct arg_lit *a_aesgcm = arg_lit0("1", "aesgcm", "Force AESGCM. Default AES128GCM");
	struct arg_lit *a_verbosity = arg_litn("v", "verbose", 0, 4, "0- quiet (default), 1- errors, 2- warnings, 3- debug, 4- debug libs");

	struct arg_str *a_endpoint = arg_str0(NULL, "fcm-endpoint", "<URL>", "Override FCM push service endpoint URL prefix.");

	// override 'sender' VAPID keys
	struct arg_str *a_vapid_private_key = arg_str0("p", "private-key", "<base64>", "Override VAPID private key.");
	struct arg_str *a_vapid_public_key = arg_str0("k", "public-key", "<base64>", "Override VAPID public key");
	struct arg_str *a_vapid_auth_secret = arg_str0(NULL, "auth-secret", "<base64>", "Override VAPID auth secret");

	// last
	struct arg_str *a_last_persistent_id = arg_str0(NULL, "last", "<id>", "Last persistent id");

	// helper options
	struct arg_lit *a_version = arg_lit0(NULL, "version", "Print version");
	struct arg_lit *a_generatevapidkeys = arg_lit0(NULL, "generate-vapid-keys", "Generate VAPID keys");

	struct arg_lit *a_help = arg_lit0("h", "help", "Show this help");
	struct arg_end *a_end = arg_end(20);

	void* argtable[] = { 
		a_list, a_list_qrcode, a_invert_qrcode, a_list_email, a_link_email, a_credentials, a_keys, 
		a_subscribe_vapid, a_subscribe_fcm, a_unsubscribe, a_send, a_sub,
		a_name, a_subscribe_url, a_authorized_entity, a_file_name,
		a_server_key, a_subject, a_body, a_icon, a_link, a_data, a_command,
		a_contact, a_p256dh, a_auth,
		a_recipient_tokens, a_recipient_token_file, a_output, a_template_file,
		a_output_lib_filenames, a_notify_function_name,
		a_aesgcm, a_verbosity, a_vapid_private_key, a_vapid_public_key, a_vapid_auth_secret,
		a_endpoint, 
		a_version, a_generatevapidkeys,
		a_last_persistent_id,
		a_help, a_end 
	};

	int nerrors;

	// verify the argtable[] entries were allocated successfully
	if (arg_nullcheck(argtable) != 0)
	{
		arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
		return 1;
	}
	// Parse the command line as defined by argtable[]
	nerrors = arg_parse(argc, argv, argtable);

	if (a_file_name->count)
		file_name = *a_file_name->sval;
	else
		file_name = getDefaultConfigFileName(DEF_FILE_NAME);

	// read config
	config = new ConfigFile(file_name);
	if (config->errorCode) {
		std::cerr << "Error " << config->errorCode << ": " << config->errorDescription << std::endl;
		return config->errorCode;
	}

	config->clientOptions->setVerbosity(a_verbosity->count);

	if (a_subscribe_url->count)
		subscribeUrl = *a_subscribe_url->sval;
	else
		subscribeUrl = "";
	if (a_name->count)
		name = *a_name->sval;
	else
		name = "";
	if (a_sub->count)
		sub = *a_sub->sval;
	else
		sub = "";
	int m = strtol(subscribeUrl.c_str(), NULL, 10);
	if ((m > 0) && (m <= SUBSCRIBE_URL_COUNT))
	{
		m--;
		subscribeUrl = SUBSCRIBE_URLS[m];
	}
	if (subscribeUrl.empty())
		subscribeUrl = SUBSCRIBE_URLS[0];

	if (a_authorized_entity->count)
		authorizedEntity = *a_authorized_entity->sval;
	else
		authorizedEntity = "";

	cmd = CMD_LISTEN;
	if (a_list->count)
		cmd = CMD_LIST;
	else
		if ((a_list_qrcode->count) || (a_invert_qrcode->count))
			cmd = CMD_LIST_QRCODE;
		else
			if (a_link_email->count)
				cmd = CMD_LIST_LINK;
			else
				if (a_list_email->count) 
				{
					cmd = CMD_LIST_EMAIL;
					cn = std::string(*a_list_email->sval);
				}
				else
					if (a_keys->count)
						cmd = CMD_KEYS;
					else
						if (a_credentials->count)
							cmd = CMD_CREDENTIALS;
						else
							if (a_subscribe_vapid->count)
								cmd = CMD_SUBSCRIBE_VAPID;
							else
								if (a_subscribe_fcm->count)
									cmd = CMD_SUBSCRIBE_FCM;
								else
									if (a_unsubscribe->count)
										cmd = CMD_UNSUBSCRIBE;
									else
										if (a_send->count)
											cmd = CMD_PUSH;
										else
											if (a_generatevapidkeys->count)
												cmd = CMD_GENERATE_VAPID_KEYS;
											else
												if (a_version->count)
													cmd = CMD_PRINT_VERSION;

	if (a_endpoint->count)
		fcm_endpoint = *a_endpoint->sval;
	else {
		fcm_endpoint = getDefaultFCMEndPoint();
	}

	if (a_vapid_private_key->count) {
		subscriptionMode = SUBSCRIBE_FORCE_VAPID;;
		private_key = *a_vapid_private_key->sval;
	}
	if (a_vapid_public_key->count) {
		subscriptionMode = SUBSCRIBE_FORCE_VAPID;;
		public_key = *a_vapid_public_key->sval;
	}
	if (a_vapid_auth_secret->count) {
		subscriptionMode = SUBSCRIBE_FORCE_VAPID;;
		vapid_recipient_auth = *a_vapid_auth_secret->sval;
	}

	if (a_last_persistent_id->count) {
		lastPersistentId = *a_last_persistent_id->sval;
	}

	if (a_contact->count) {
		vapid_sender_contact = *a_contact->sval;
	}
	if (a_p256dh->count) {
		vapid_recipient_p256dh = *a_p256dh->sval;
	}
	if (a_auth->count) {
		auth_secret = *a_auth->sval;
	}

	if (a_notify_function_name->count)
		notifyFunctionName = *a_notify_function_name->sval;
	else
		notifyFunctionName = DEF_FUNC_NOTIFY;
	
	if (a_template_file->count)
	{
		std::string fn = std::string(*a_template_file->sval);
		email_template = file2string(fn);
		if (email_template.empty())
		{
			// try load from the Internet;
			email_template = url2string(fn);
		}
	}
	if (cn.empty())
	{
		cn = DEF_COMMON_NAME;
	}

	if (a_output_lib_filenames->count)
	{
		for (int i = 0; i < a_output_lib_filenames->count; i++)
		{
			notifyLibFileNames.push_back(a_output_lib_filenames->sval[i]);
		}
	}
	else
	{
		notifyLibFileNames.push_back(DEF_OUTPUT_SO_FN);
	}
	
	if (cmd == CMD_PUSH)
	{
		if (a_server_key->count == 0)
		{
			if (a_name->count == 0)
			{
				if ((a_recipient_tokens->count == 0) && (a_recipient_token_file->count == 0)) {
					std::cerr << "Recipient endpoint missed. " << std::endl;
					nerrors++;
				}
				if (a_p256dh->count == 0) {
					std::cerr << "-d, --p256dh missed. " << std::endl;
					nerrors++;
				}
				if (a_auth->count == 0) {
					std::cerr << "-a, --auth missed. " << std::endl;
					nerrors++;
				}
			}
		}
		if (a_command->count == 0)
		{
			if (a_subject->count == 0)
			{
				std::cerr << "-s missed." << std::endl;
				nerrors++;
			}
			if (a_body->count == 0)
			{
				std::cerr << "-b missed." << std::endl;
				nerrors++;
			}
			/*
			if (a_icon->count == 0)
			{
				std::cerr << "-i missed." << std::endl;
				nerrors++;
			}
			if (a_link->count == 0)
			{
				std::cerr << "-l missed." << std::endl;
				nerrors++;
			}
			*/
		}
	}

	if (cmd == CMD_SUBSCRIBE_FCM)
	{
		if (name.empty()) 
		{
			std::cerr << "No subscription name. Set valid -n option." << std::endl;
			nerrors++;
		}
		if (subscribeUrl.empty()) 
		{
			std::cerr << "Unknown registar. Set valid -r option." << std::endl;
			nerrors++;
		}
		if (authorizedEntity.empty()) 
		{
			std::cerr << "Missing -e <entity-id> option." << std::endl;
			nerrors++;
		}
	}

	if (cmd == CMD_SUBSCRIBE_VAPID)
	{
		if (name.empty()) 
		{
			std::cerr << "No subscription name. Set valid -n option." << std::endl;
			nerrors++;
		}
		if (private_key.empty()) 
		{
			std::cerr << "No VAPID private key. Set valid --private-key option." << std::endl;
			nerrors++;
		}
		if (public_key.empty()) 
		{
			std::cerr << "No VAPID public key. Set valid --public-key option." << std::endl;
			nerrors++;
		}
		if (auth_secret.empty()) 
		{
			std::cerr << "No VAPID auth secret key. Set valid --auth-secret option." << std::endl;
			nerrors++;
		}
	}

	if (a_server_key->count)
	{
		subscriptionMode = SUBSCRIBE_FORCE_FIREBASE;
		serverKey = *a_server_key->sval;
	}
	else
	{
		serverKey = "";
	}

	if (a_subject->count)
	{
		subject = *a_subject->sval;
	}
	else
	{
		subject = "";
	}

	if (a_body->count)
	{
		body = *a_body->sval;
	}
	else
	{
		body = "";
	}

	if (a_icon->count)
	{
		icon = *a_icon->sval;
	}
	else
	{
		icon = "";
	}

	if (a_link->count)
	{
		link = *a_link->sval;
	} 
	else 
	{
		link = "";
	}

	if (a_data->count)
	{
		data = *a_data->sval;
	} 
	else 
	{
		data = "";
	}

	if (a_command->count)
	{
		command = *a_command->sval;
	} 
	else 
	{
		command = "";
	}

	for (int i = 0; i < a_recipient_tokens->count; i++)
	{
		recipientTokens.push_back(a_recipient_tokens->sval[i]);
	}

	if (a_recipient_token_file->count)
	{
		std::string fn(*a_recipient_token_file->sval);
		std::string data = file2string(fn);
		if (data.empty())
		{
			// try load from the Internet;
			data = url2string(fn);
		}
		if (!data.empty()) {
			parseJsonRecipientTokens(recipientTokens, data);
		}
	}

	outputFormat = 0;	// 0- text, 1- json
	if (a_output->count)
	{
		if (strcmp("json", *a_output->sval) == 0)
			outputFormat = 1;
	}

	aesgcm = a_aesgcm->count > 0;
	
	invert_qrcode = a_invert_qrcode->count > 0;
	
	// special case: '--help' takes precedence over error reporting
	if ((a_help->count) || nerrors)
	{
		if (nerrors)
			arg_print_errors(stderr, a_end, progname);
		std::cerr << "Usage: " << progname << std::endl;
		arg_print_syntax(stderr, argtable, "\n");
		std::cerr << "Web push notification command line interface client" << std::endl;
		arg_print_glossary(stderr, argtable, "  %-25s %s\n");
		arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
		return 1;
	}

	arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
	return 0;
}

int WpnConfig::error()
{
	return errorcode;
}

SO_INSTANCE loadPlugin(const std::string &fileName)
{
#ifdef _MSC_VER
	return LoadLibrary(fileName.c_str());
#else
	return dlopen(fileName.c_str(), RTLD_LAZY);
#endif
}

void unloadPlugin(SO_INSTANCE so)
{
#ifdef _MSC_VER
	FreeLibrary(so);
#else
	dlclose(so);
#endif
}

static OnNotifyC loadDesktopNotifyC
(
	SO_INSTANCE so,
	const std::string &functionName
)
{
#ifdef _MSC_VER
	return (OnNotifyC) GetProcAddress(so, functionName.c_str());
#else
	return (OnNotifyC) dlsym(so, functionName.c_str());
#endif
}

size_t WpnConfig::loadNotifyFuncs()
{
	notifyLibs.clear();
	onNotifyList.clear();
	size_t r = 0;
	for (std::vector <std::string>::const_iterator it(notifyLibFileNames.begin()); it != notifyLibFileNames.end(); ++it)
	{
		std::string rp;
		str_realpath(rp, (*it));
		
		std::vector<std::string > files;
		if (isDir(rp))
		{
			filesInPath(rp, DEF_PLUGIN_FILE_EXT, 1, &files);
		}
		else
		{
			files.push_back(rp);
		}

		for (std::vector<std::string>::const_iterator it(files.begin()); it != files.end(); ++it)
		{
			SO_INSTANCE so = loadPlugin(*it);
			if (!so)
			{
				if (config && config->clientOptions && config->clientOptions->getVerbosity() > 1)
				{
					std::cerr << "Can not open shared library file: " << *it << std::endl;
				}
				continue;
			}
			notifyLibs.push_back(so);
			OnNotifyC desktopNotify = loadDesktopNotifyC(so, notifyFunctionName);
			if (!desktopNotify)
			{
				if (config && config->clientOptions && config->clientOptions->getVerbosity() > 1)
				{
					std::cerr << "Can not bind " << notifyFunctionName << "() from shared library file: " << *it << std::endl;
				}
				continue;
			}
			if (config && config->clientOptions && config->clientOptions->getVerbosity() > 2)
			{
				std::cerr << "Shared library " << *it << " loaded successfully." << std::endl;
			}
			onNotifyList.push_back(desktopNotify);
			r++;
		}
	}
	if (config && config->clientOptions && config->clientOptions->getVerbosity() > 1)
	{
		std::cerr << "Shared libraries loaded: " << r << std::endl;
	}
	return r;
}

void WpnConfig::unloadNotifyFuncs()
{
	for (std::vector <SO_INSTANCE>::const_iterator it(notifyLibs.begin()); it != notifyLibs.end(); ++it)
	{
		unloadPlugin(*it);
	}
	notifyLibs.clear();
}

void WpnConfig::getPersistentIds(std::vector<std::string> &retval)
{
	retval.clear();
	retval.push_back(config->subscriptions->getReceivedPersistentId());
	for (std::vector<Subscription>::iterator it(config->subscriptions->list.begin()); it != config->subscriptions->list.end(); ++it)
	{
		std::string v = it->getPersistentId();
		if (!v.empty())
		{
			retval.push_back(v);
		}
	}
}

/**
* Get subscription by name
* @param subscriptionName subscription name
* @return server key from subscription by the name of subscription
*/
const Subscription *WpnConfig::getSubscription(const std::string &subscriptionName) const
{
	for (std::vector<Subscription>::const_iterator it(config->subscriptions->list.begin()); it != config->subscriptions->list.end(); ++it)
	{
		std::string n = it->getName();
		if (n == subscriptionName)
		{
			return &*it;
		}
	}
	return NULL;
}

/**
 * Get server key
 * @param subscriptionName subscription name
 * @return server key from subscription by the name of subscription
 */
std::string WpnConfig::getSubscriptionServerKey
(
	const std::string &subscriptionName
) const
{
	const Subscription *subscription = getSubscription(subscriptionName);
	if (subscription)
		return subscription->getServerKey();
	return "";
}

/**
 * Get FCM token
 * @param subscriptionName subscription name
 * @return FCM token from subscription by the name of subscription
 */
std::string WpnConfig::getSubscriptionToken
(
	const std::string &subscriptionName
) const
{
	const Subscription *subscription = getSubscription(subscriptionName);
	if (subscription)
		return subscription->getToken();
	return "";
}

bool WpnConfig::setPersistentId
(
	const std::string &authorizedEntity, 
	const std::string &persistentId
)
{
	if (!config->subscriptions)
		return false;
	for (std::vector<Subscription>::iterator it(config->subscriptions->list.begin()); it != config->subscriptions->list.end(); ++it)
	{
		if (it->getAuthorizedEntity() == authorizedEntity)
		{
			it->setPersistentId(persistentId);
			return true;
		}
	}
	config->subscriptions->setReceivedPersistentId(persistentId);
	return true;
}

std::string WpnConfig::versionString()
{
	std::stringstream r;
	r << "libprotobuf: " << google::protobuf::internal::VersionString(GOOGLE_PROTOBUF_VERSION);
	return r.str();
}
