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
#include "utilqr.h"
#include "utilstring.h"
#include "utilvapid.h"

#define ERR_WSA		-1

#define LINK_SUREPHONE_D	"https://mail.surephone.commandus.com/?d="
#define DEF_EMAIL_TEMPLATE	"<html><body>$subject<br/>Hi, $name<br/>Click the link below on the phone, tablet or other device on which surephone is installed.\
If the program is not already installed, \
<a href=\"https://play.google.com/store/apps/details?id=com.commandus.surephone\">install it</a>.\
<br/>$body</body></html>"

#ifdef _MSC_VER

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

#ifdef _MSC_VER
void initWindows()
{
	// Initialize Winsock
	WSADATA wsaData;
	int r = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (r)
	{
		std::cerr << "WSAStartup error %d" << r << std::endl;
		exit(ERR_WSA);
	}
}
#endif

int main(int argc, char** argv)
{
	// Signal handler
	setSignalHandler(SIGINT);
#ifdef _MSC_VER
	initWindows();
#endif
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
				config.subscriptions->write(std::cout, "\t", config.outputFormat, config.verbosity == 0);
			}
			break;
		case CMD_LIST_QRCODE:
			{
				if ((config.outputFormat == 0) && (config.verbosity > 0))
					std::cout << "FCM QRCodes:" << std::endl;
				
				long r = 0;
				for (std::vector<Subscription>::const_iterator it(config.subscriptions->list.begin()); it != config.subscriptions->list.end(); ++it)
				{
					std::stringstream ss;
					ss 
						<< it->getName() << ","
						<< it->getAuthorizedEntity() << ","
						<< it->getServerKey() << ","
						<< it->getToken();
					std::cout << qr2string(ss.str(), config.invert_qrcode) << std::endl;
				}
			}
			break;
		case CMD_LIST_LINK:
			{
				std::stringstream ssBody;
				for (std::vector<Subscription>::const_iterator it(config.subscriptions->list.begin()); it != config.subscriptions->list.end(); ++it)
				{
					std::stringstream ss;
					ss
						<< it->getName() << ","
						<< it->getAuthorizedEntity() << ","
						<< it->getServerKey() << ","
						<< it->getToken();
					ssBody << LINK_SUREPHONE_D << escapeURLString(ss.str()) << std::endl;
				}
				std::cout << ssBody.str();
			}
			break;
		case CMD_LIST_EMAIL:
			{
				if (config.subject.empty()) {
					config.subject = "Connect device to wpn";
				}

				if (config.email_template.empty()) {
					config.email_template = DEF_EMAIL_TEMPLATE;
				}
				std::string m = config.email_template;
				size_t p = m.find("$name");
				if (p != std::string::npos)
				{
					m.replace(p, 5, config.cn);
				}
				p = m.find("$subject");
				if (p != std::string::npos)
				{
					m.replace(p, 8, config.subject);
				}

				std::stringstream ssBody;
				for (std::vector<Subscription>::const_iterator it(config.subscriptions->list.begin()); it != config.subscriptions->list.end(); ++it)
				{
					std::stringstream ss;
					ss 
						<< it->getName() << ","
						<< it->getAuthorizedEntity() << ","
						<< it->getServerKey() << ","
						<< it->getToken();

					std::string u = escapeURLString(ss.str());
					ssBody 
						<< "<p>" 
						<< "<a href=\"" << LINK_SUREPHONE_D << u << "\">Connect to "
						<< it->getName()
						<< "</a>" 
						<< "</p>"
						<< std::endl;
				}
				p = m.find("$body");
				if (p != std::string::npos)
				{
					m.replace(p, 5, ssBody.str());
				}
				std::cout << m << std::endl;
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
		case CMD_SUBSCRIBE_FCM:
			{
				Subscription subscription;
				std::string d;
				std::string headers;

				int r = subscribe(subscription, SUBSCRIBE_FIREBASE, *config.wpnKeys, 
					config.subscribeUrl, config.getDefaultEndPoint(), config.authorizedEntity,
					config.serverKey, &d, &headers, config.verbosity);
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
		case CMD_SUBSCRIBE_VAPID:
			{
				Subscription subscription;
				config.subscriptions->list.push_back(subscription);
				if (config.verbosity > 0)
				{
					subscription.write(std::cout, "\t", config.outputFormat);
				}
			}
			break;
		case CMD_UNSUBSCRIBE:
			{
				if (config.authorizedEntity.empty())
				{
					// delete all
					Subscription f(config.endpoint, config.authorizedEntity);
					for (std::vector<Subscription>::iterator it(config.subscriptions->list.begin()); it != config.subscriptions->list.end(); ++it)
					{
						// TODO
					}
					config.subscriptions->list.clear();
				}
				else
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
			std::string token = "";
			std::string serverKey;
			WpnKeys wpnKeys;
			switch (config.subscriptionMode) {
				case SUBSCRIBE_FIREBASE:
					serverKey = config.serverKey;
					break;
				case SUBSCRIBE_VAPID:
					wpnKeys.init(config.private_key, config.public_key, config.auth_secret); 
					break;
				default:
					// Load server key from the subscription, by the name
					const Subscription *subscription = config.getSubscription(config.name);
					if (subscription) {
						switch (subscription->getSubscribeMode()) {
							case SUBSCRIBE_FIREBASE:
								config.subscriptionMode = SUBSCRIBE_FIREBASE;
								serverKey = subscription->getServerKey();
								token = subscription->getToken();
								break;
							case SUBSCRIBE_VAPID:
								config.subscriptionMode = SUBSCRIBE_VAPID;
								wpnKeys.init(subscription->getWpnKeys()); 
								break;
							default:
								break;
						}
					}
					break;
			}
			for (std::vector<std::string>::const_iterator it(config.recipientTokens.begin()); it != config.recipientTokens.end(); ++it)
			{
				int r;
				if (config.command.empty())
				{
					if (config.verbosity > 1)
						std::cout << "Sending notification to " << *it << std::endl;
					switch (config.subscriptionMode) {
						case SUBSCRIBE_FIREBASE:
							r = push2ClientNotificationFCM(&retval, serverKey, *it,
								config.subject, config.body, config.icon, config.link, config.verbosity);
							break;
						case SUBSCRIBE_VAPID:
							if (config.aud.empty()) {	// https://fcm.googleapis.com
								config.aud = *it; // extractURLProtoAddress(*it);
							}
							r = push2ClientNotificationVAPID(&retval, *it,
								wpnKeys.getPrivateKey(), wpnKeys.getPublicKey(),
									config.aud, config.sub,
									config.subject, config.body, config.icon, config.link, config.verbosity);
							break;
					}
				}
				else
				{
					if (config.verbosity > 1)
						std::cout << "Execute command " << config.command << " on " << *it << std::endl;
					switch (config.subscriptionMode) {
						case SUBSCRIBE_FIREBASE:
							r = push2ClientDataFCM(&retval, serverKey, token, *it, "", config.command, 0, "", config.verbosity);
							break;
						case SUBSCRIBE_VAPID:
							r = push2ClientDataVAPID(&retval, *it,
								wpnKeys.getPrivateKey(), wpnKeys.getPublicKey(), wpnKeys.getAuthSecret(), 
								"", config.command, 0, "", config.verbosity);
							break;
					}					
					
				}
				if (r >= 200 && r < 300)
					std::cout << retval << std::endl;
				else
					std::cerr << "Error " << r << ": " << retval << std::endl;
			}
		}
			break;
		case CMD_GENERATE_VAPID_KEYS:
		{
			Subscription subscription;
			std::string d;
			std::string headers;

			config.wpnKeys->generate();
			/*
			int r = subscribe(subscription, SUBSCRIBE_VAPID, *config.wpnKeys, 
				config.subscribeUrl, config.getDefaultEndPoint(), config.authorizedEntity,
				config.serverKey, &d, &headers, config.verbosity);
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
			*/
			std::cout << config.wpnKeys->asJSON() << std::endl;
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
