#include <string>
#include <iostream>
#include <argtable3/argtable3.h>
#include "nlohmann/json.hpp"

#include "utilvapid.h"

using json = nlohmann::json;

static const char* progname = "wpn";

int main(int argc, char **argv) 
{
	std::string title;
	std::string body;
	std::string icon;
	std::string click_action;
	std::string click_action_title;
	std::string sub;

	std::string publicKey;
	std::string privateKey;

	std::string endpoint;
	std::string p256dh;
	std::string auth;
	
	std::string contact;
	std::string curl_file;
	
	int contentEncoding; // AESGCM or AES128GCM

	struct arg_str *a_title = arg_str1("t", "title", "<title>", "Title of push notification");
	struct arg_str *a_body = arg_str1("b", "body", "<text>", "Push notification body");
	struct arg_str *a_icon = arg_str1("i", "icon", "<URL>", "Push notification icon URL");
	struct arg_str *a_action_link = arg_str1("l", "action-link", "<URL>", "Push notification action link URL");
	struct arg_str *a_action_title = arg_str1("c", "action-caption", "<URL>", "Push notification action caption on click URL");
	struct arg_str *a_sub = arg_str1("f", "from", "<URL>", "mailto: address of the contact");

	struct arg_str *a_public_key = arg_str1("k", "public", "<key>", "VAPID public key");
	struct arg_str *a_private_key = arg_str1("p", "private", "<key>", "VAPID private key");
	struct arg_str *a_endpoint = arg_str1("e", "endpoint", "<URL>", "Recipient's endpoint URL");
	struct arg_str *a_p256dh = arg_str1("d", "p256dh", "<key>", "Recipient's endpoint p256dh");
	struct arg_str *a_auth = arg_str1("a", "auth", "<key>", "Recipient's endpoint auth");

	struct arg_str *a_contact = arg_str0("f", "from", "<email>", "Sender's email e.g. mailto:alice@acme.com");
	struct arg_str *a_curl_file = arg_str0("o", "curl", "<file>", "Print curl command. File keeps ciphered data.");
	
	struct arg_lit *a_aes128gcm = arg_lit0("1", "aesgcm", "content encoding aes128gcm. Default aes128gcm");
	struct arg_lit *a_help = arg_lit0("h", "help", "Show this help");
	struct arg_end *a_end = arg_end(20);

	void* argtable[] = { 
		a_title, a_body, a_icon, a_action_link, a_action_title, a_sub, 
		a_public_key, a_private_key, a_endpoint, a_p256dh, a_auth,
		a_aes128gcm,
		a_contact,
		a_curl_file,
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

	title = *a_title->sval;
	body = *a_body->sval;
	icon = *a_icon->sval;
	click_action = *a_action_link->sval;
	click_action_title = *a_action_title->sval;
	sub = *a_sub->sval;
	publicKey = *a_public_key->sval;
	privateKey = *a_private_key->sval;
	endpoint = *a_endpoint->sval;
	p256dh = *a_p256dh->sval;
	auth = *a_auth->sval;
	if (a_aes128gcm->count)
		contentEncoding = AES128GCM;
	else
		contentEncoding = AESGCM;
	if (a_contact->count)
		contact = *a_contact->sval;
	else
		contact = "";
	if (a_curl_file->count)
		curl_file = *a_curl_file->sval;
	else
		curl_file = "";

	// special case: '--help' takes precedence over error reporting
	if ((a_help->count) || nerrors)
	{
		if (nerrors)
			arg_print_errors(stderr, a_end, progname);
		std::cerr << "Usage: " << progname << std::endl;
		arg_print_syntax(stderr, argtable, "\n");
		std::cerr << "Web push message CURL printer" << std::endl;
		arg_print_glossary(stderr, argtable, "  %-25s %s\n");
		arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
		return 1;
	}

	arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));

	std::string filename = "aesgcm.bin";

	json requestBody = {
		{"notification", 
			{
				{"title", title},
				{"body", body},
				{"icon", icon},
				{"click_action", click_action}
			}
		}
	};

	std::string r;
	if (curl_file.empty()) {
		int httpcode = webpushVapid(r, publicKey, privateKey, endpoint, p256dh, auth, requestBody.dump(), contact, contentEncoding); 
		std::cout << r << std::endl;
		if (httpcode < 0) {
			std::cerr << httpcode << std::endl;
		}
	} else {
		r = webpushVapidCmd(publicKey, privateKey, filename, endpoint, p256dh, auth,
			requestBody.dump(), contact, contentEncoding);
		std::cout << r << std::endl;
	}
	return 0;
}
