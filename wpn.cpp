/**
 * @file wpn.cpp
 */

#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <signal.h>
#include <argtable2.h>
#include <curl/curl.h>

#include "wpn.h"
#include "wp-storage-file.h"
#include "wp-subscribe.h"

#ifdef _WIN32

void setSignalHandler(int signal)
{
}

#else
void signalHandler(int signal)
{
	switch(signal)
	{
	case SIGINT:
		std::cerr << MSG_INTERRUPTED << std::endl;
		break;
	default:
		break;
	}
}

void setSignalHandler(int signal)
{
	struct sigaction action;
	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = &signalHandler;
	sigaction(signal, &action, NULL);
}
#endif

int main(int argc, char** argv)
{
	// Signal handler
	setSignalHandler(SIGINT);

	// In windows, this will init the winsock stuff
	curl_global_init(CURL_GLOBAL_ALL);

	WpnConfig config(argc, argv);
	if (config.error())
		exit(config.error());

	std::ifstream configRead(config.file_name);
	WpnKeys wpnKeys(configRead);
	Subscriptions subscriptions(configRead);
	configRead.close();

	switch (config.cmd)
	{
		case CMD_LIST:
			{
				if (config.verbosity > 0)
					std::cout << "subscribeUrl\tsubscribeMode\tendpoint\tauthorizedEntity\ttoken\tpushSet" << std::endl;
				subscriptions.write(std::cout, "\t");
			}
			break;
		case CMD_CREDENTIALS:
			{
				if (config.verbosity > 0)
					std::cout << "private_key\tpublic_key\tauth_secret" << std::endl;
				wpnKeys.write(std::cout, "\t");
				std::cout << std::endl;
			}
			break;
		case CMD_SUBSCRIBE:
			{
				Subscription subscription;
				std::string d;
				int r = subscribe(subscription, SUBSCRIBE_FIREBASE, wpnKeys, 
					config.subscribeUrl, config.endpoint, config.authorized_entity,  &d, 
					config.verbosity);
				if ((r < 200) || (r >= 300))
				{
					std::cerr << "Error " << r << ": " << d << std::endl;
				}
				else 
				{
					subscriptions.list.push_back(subscription);
				}
			}
			break;
		case CMD_UNSUBSCRIBE:
			break;
		case CMD_SEND:
			break;
		default:
			{
				if (config.verbosity > 0)
				{
					std::cerr << "Not implemented yet." << std::endl;
				}
			}
	}

	std::ofstream configWrite(config.file_name);
	wpnKeys.write(configWrite);
	subscriptions.write(configWrite);
	configWrite.close();
	
	return 0;
}
