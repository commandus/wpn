/**
 * Copyright (c) 2018 Andrei Ivanov <andrei.i.ivanov@commandus.com>
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 * 
 * @file wpn.cpp
 * 
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
					config.subscribeUrl, config.getDefaultEndPoint(), config.authorizedEntity,
					&d, &headers, config.verbosity);
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
				int r = push2ClientNotification(&retval,
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
				config.loadDesktopNotifyFuncs();
				if (config.verbosity > 0)
				{
				}
				MCSClient client(&config);
				client.connect();
				std::cerr << "Listen" << std::endl
				<< "Enter q to quit" << std::endl
				<< "p: ping" << std::endl;
				client.writeStream(std::cin);
				client.stop();
				config.unloadDesktopNotifyFuncs();
			}
	}

	config.write();
	
	return 0;
}
