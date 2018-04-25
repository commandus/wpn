/**
 * @file wpn.cpp
 */

#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <signal.h>
#include <argtable2.h>
#include <curl/curl.h>

#include "wpn.h"
#include "wp-storage-file.h"
#include "wp-subscribe.h"
#include "wp-push.h"
#include "sslfactory.h"
#include "mcs/mcsclient.h"

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
	initSSL();
	
	WpnConfig config(argc, argv);
	if (config.error())
		exit(config.error());

	switch (config.cmd)
	{
		case CMD_LIST:
			{
				if ((config.outputFormat == 0) && (config.verbosity > 0))
					std::cout << "subscribeUrl\tsubscribe mode\tendpoint\tauthorized entity\tFCM token\tpushSet" << std::endl;
				config.subscriptions->write(std::cout, "\t", config.outputFormat);
			}
			break;
		case CMD_CREDENTIALS:
			{
				if ((config.outputFormat == 0) && (config.verbosity > 0))
					std::cout << "application identifer\tandroid identifer\tsecurity token\tGCM token" << std::endl;
				config.androidCredentials->write(std::cout, "\t", config.outputFormat);
				std::cout << std::endl;
			}
			break;
		case CMD_KEYS:
			{
				if ((config.outputFormat == 0) && (config.verbosity > 0))
					std::cout << "private key\tpublic key\tauthentication secret" << std::endl;
				config.wpnKeys->write(std::cout, "\t", config.outputFormat);
				std::cout << std::endl;
			}
			break;
		case CMD_SUBSCRIBE:
			{
				Subscription subscription;
				std::string d;
				std::string headers;
				int r = subscribe(subscription, SUBSCRIBE_FIREBASE, *config.wpnKeys, 
					config.subscribeUrl, config.endpoint, config.authorizedEntity,  &d, &headers,
					config.androidCredentials->getAndroidId(), config.androidCredentials->getSecurityToken(),
					config.verbosity);
				if ((r < 200) || (r >= 300))
				{
					std::cerr << "Error " << r << ": " << d << std::endl;
				}
				else 
				{
					config.subscriptions->list.push_back(subscription);
				}
				if (config.verbosity > 0)
				{
					subscription.write(std::cout, "\t", config.outputFormat);
				}
			}
			break;
		case CMD_UNSUBSCRIBE:
			{
				if (true)
				{
					Subscription f(config.endpoint, config.authorizedEntity);
					std::vector<Subscription>::iterator it = std::find(config.subscriptions->list.begin(),
						config.subscriptions->list.end(), f);
					if (it != config.subscriptions->list.end())
						config.subscriptions->list.erase(it);
				}
			}
			break;
		case CMD_PUSH:
		{
			std::string retval;
			for (std::vector<std::string>::const_iterator it(config.recipientTokens.begin()); it != config.recipientTokens.end(); ++it)
			{
				int r = push2ClientFCMToken(&retval,
					config.serverKey, *it,
					config.subject,
					config.body, config.icon, config.link
				);
				if (r >= 200 && r < 300)
					std::cout << retval << std::endl;
			}
		}
			break;
		default:
			{
				if (config.verbosity > 0)
				{
				}
				MCSClient client(&config, config.wpnKeys, config.androidCredentials);
				client.connect();
				std::cerr << "Listen" << std::endl
				<< "Enter q to quit" << std::endl
				<< "p: ping" << std::endl;
				client.writeStream(std::cin);
				client.stop();
			}
	}

	config.write();
	
	return 0;
}
