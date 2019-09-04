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
#include <argtable3/argtable3.h>
#include <curl/curl.h>

#include "platform.h"
#include "wpn.h"
#include "wp-storage-file.h"
#include "wp-subscribe.h"
#include "wp-push.h"
#include "sslfactory.h"
#include "mcs/mcsclient.h"
#include "utilqr.h"
#include "utilstring.h"
#include "utilvapid.h"
#include "utilrecv.h"
#include "sslfactory.h"

#define ERR_WSA		-1

#define PROMPT_STARTED		"Enter q or press Ctrl+C to quit, p to ping"

#define LINK_SUREPHONE_D	"https://mail.surephone.commandus.com/?d="
#define DEF_EMAIL_TEMPLATE	"<html><body>$subject<br/>Hi, $name<br/>Click the link below on the phone, tablet or other device on which surephone is installed.\
If the program is not already installed, \
<a href=\"https://play.google.com/store/apps/details?id=com.commandus.surephone\">install it</a>.\
<br/>$body</body></html>"

static int quitFlag = 0;

void signalHandler(int signal)
{
	switch(signal)
	{
	case SIGHUP:
		std::cerr << MSG_RELOAD_CONFIG_REQUEST << std::endl;
		quitFlag = 2;
		fclose(0);
		break;
	case SIGINT:
		std::cerr << MSG_INTERRUPTED << std::endl;
		quitFlag = 1;
		break;
	default:
		break;
	}
}

#ifdef _MSC_VER
// TODO
void setSignalHandler()
{
}
#else
void setSignalHandler()
{
	struct sigaction action;
	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = &signalHandler;
	sigaction(SIGINT, &action, NULL);
	sigaction(SIGHUP, &action, NULL);
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

void onLog
(
	void *env,
	int severity,
	const char *message
)
{
	WpnConfig *c = (WpnConfig *) env;
	if (c && c->config && c->config->clientOptions->getVerbosity() >= severity)
	{
		std::cerr << message;
	}
}

/**
  * Call dynamically loaded functions to notify user
  */
void onNotify(
	void *env,
	const char *persistent_id,
	const char *from,
	const char *appName,
	const char *appId,
	int64_t sent,
	const NotifyMessageC *notification
)
{
	WpnConfig *c = (WpnConfig *) env;
	if (c && c->config && c->config->clientOptions->getVerbosity() > 2)
	{
		if (notification)
		{
			std::cerr << "Body:" << notification->body << std::endl;
		}
	}
	for (std::vector <OnNotifyC>::const_iterator it(((WpnConfig*) env)->onNotifyList.begin()); it != ((WpnConfig*)env)->onNotifyList.end(); ++it)
	{
		(*it) (env, persistent_id, from, appName, appId, sent, notification);
	}
}

void readCommand(
	int *quitFlag,
	std::istream &strm,
	MCSClient *client
)
{
	std::string s;
	while ((*quitFlag == 0) && (!strm.eof()))
	{
		strm >> s;
		if (s.find_first_of("qQ") == 0)
		{
			*quitFlag = 1;
			break;
		}
		if (s.find_first_of("pP") == 0)
		{
			client->ping();
		}
	}
}

int main(int argc, char** argv)
{
	// Signal handler
	setSignalHandler();
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
				if ((config.outputFormat == 0) && (config.config->clientOptions->getVerbosity() > 0))
					std::cout << "subscribeUrl\tsubscribe mode\tendpoint\tauthorized entity\tFCM token\tpushSet" << std::endl;
				config.config->subscriptions->write(std::cout, "\t", config.outputFormat);
			}
			break;
		case CMD_LIST_QRCODE:
			{
				if ((config.outputFormat == 0) && (config.config->clientOptions->getVerbosity() > 0))
					std::cout << "FCM QRCodes:" << std::endl;

				std::stringstream ss;
				std::string foreground = u8"\u2588\u2588";
				std::string background = "  ";
				std::string v = config.config->wpnKeys->getPublicKey();
				if (config.invert_qrcode)
						v = qr2string(v, foreground, background);
					else
						v = qr2string(v, background, foreground);
				std::cout 
					<< config.config->wpnKeys->id << "\t"
					<< config.config->clientOptions->name << std::endl
					<< v << std::endl;
				long r = 0;
				for (std::vector<Subscription>::const_iterator it(config.config->subscriptions->list.begin()); it != config.config->subscriptions->list.end(); ++it)
				{
					std::string v = it->getWpnKeys().getPublicKey();
					if (config.invert_qrcode)
						v = qr2string(v, foreground, background);
					else
						v = qr2string(v, background, foreground);
					std::cout 
						<< it->getWpnKeys().id << "\t"
						<< it->getName() << std::endl
						<< v << std::endl;
				}
			}
			break;
		case CMD_LIST_LINK:
			{
				std::stringstream ssBody;
				for (std::vector<Subscription>::const_iterator it(config.config->subscriptions->list.begin()); it != config.config->subscriptions->list.end(); ++it)
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
				for (std::vector<Subscription>::const_iterator it(config.config->subscriptions->list.begin()); it != config.config->subscriptions->list.end(); ++it)
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
				if ((config.outputFormat == 0) && (config.config->clientOptions->getVerbosity() > 0))
					std::cout << "application identifer\tandroid identifer\tsecurity token\tGCM token" << std::endl;
				config.config->androidCredentials->write(std::cout, "\t", config.outputFormat);
				std::cout << std::endl;
			}
			break;
		case CMD_KEYS:
			{
				if ((config.outputFormat == 0) && (config.config->clientOptions->getVerbosity() > 0))
					std::cout << "private key\tpublic key\tauthentication secret" << std::endl;
				config.config->wpnKeys->write(std::cout, "\t", config.outputFormat);
				std::cout << std::endl;
			}
			break;
		case CMD_SUBSCRIBE_FCM:
			{
				std::string d;
				std::string headers;
				std::string token;
				std::string pushset;
				int r = subscribeFCM(&d, &headers, token, pushset, 
					config.config->wpnKeys->getPublicKey(),
					config.config->wpnKeys->getAuthSecret(),  
					config.subscribeUrl, config.getDefaultFCMEndPoint(), config.authorizedEntity,
					config.config->clientOptions->getVerbosity());
				if ((r < 200) || (r >= 300))
				{
					std::cerr << "Error " << r << ": " << d << std::endl;
				}
				else 
				{
					Subscription subscription;
					subscription.setToken(token);
					subscription.setPushSet(pushset);
					subscription.setServerKey(config.serverKey);
					subscription.setSubscribeUrl(config.subscribeUrl);
					subscription.setSubscribeMode(SUBSCRIBE_FORCE_FIREBASE);
					subscription.setEndpoint(config.getDefaultFCMEndPoint());
					subscription.setAuthorizedEntity(config.authorizedEntity);
					config.config->subscriptions->list.push_back(subscription);
					if (config.config->clientOptions->getVerbosity() > 0)
					{
						subscription.write(std::cout, "\t", config.outputFormat);
					}
				}
			}
			break;
		case CMD_SUBSCRIBE_VAPID:
			{
				// TODO
				Subscription subscription;
				config.config->subscriptions->list.push_back(subscription);
				if (config.config->clientOptions->getVerbosity() > 0)
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
					Subscription f(config.fcm_endpoint, config.authorizedEntity);
					for (std::vector<Subscription>::iterator it(config.config->subscriptions->list.begin()); it != config.config->subscriptions->list.end(); ++it)
					{
						// TODO
					}
					config.config->subscriptions->list.clear();
				}
				else
				{
					Subscription f(config.fcm_endpoint, config.authorizedEntity);
					std::vector<Subscription>::iterator it = std::find(config.config->subscriptions->list.begin(),
						config.config->subscriptions->list.end(), f);
					if (it != config.config->subscriptions->list.end())
						config.config->subscriptions->list.erase(it);
				}
			}
			break;
		case CMD_PUSH:
		{
			std::string retval;
			std::string token = "";
			std::string serverKey;
			WpnKeys wpnKeys;
			if (!config.private_key.empty()) {
				// override Wpn keys
				wpnKeys.init(0, 0, config.private_key, config.public_key, config.auth_secret); 
			}
			switch (config.subscriptionMode) {
				case SUBSCRIBE_FORCE_FIREBASE:
					serverKey = config.serverKey;
					break;
				case SUBSCRIBE_FORCE_VAPID:
					break;
				default:
					// Load server key from the subscription, by the name
					const Subscription *subscription = config.getSubscription(config.name);
					if (subscription) {
						switch (subscription->getSubscribeMode()) {
							case SUBSCRIBE_FORCE_FIREBASE:
								config.subscriptionMode = SUBSCRIBE_FORCE_FIREBASE;
								serverKey = subscription->getServerKey();
								token = subscription->getToken();
								break;
							case SUBSCRIBE_FORCE_VAPID:
								{
									config.subscriptionMode = SUBSCRIBE_FORCE_VAPID;
									// subscription MUST keep wpn keys
									WpnKeys wpnkeys = subscription->getWpnKeys();
									wpnKeys.init(subscription->getWpnKeys());
								}
								break;
							default:
								break;
						}
					}
					break;
			}

			std::string body = jsClientNotification("", config.subject, config.body, config.icon, config.link);

			// for VAPID only one endpoint not many
			for (std::vector<std::string>::const_iterator it(config.recipientTokens.begin()); it != config.recipientTokens.end(); ++it)
			{
				int r;
				if (config.command.empty())
				{
					if (config.config->clientOptions->getVerbosity() > 1)
						std::cout << "Sending notification to " << *it << std::endl;
					switch (config.subscriptionMode) {
						case SUBSCRIBE_FORCE_FIREBASE:
							r = push2ClientNotificationFCM(&retval, serverKey, *it,
								config.subject, config.body, config.icon, config.link, config.config->clientOptions->getVerbosity());
							break;
						case SUBSCRIBE_FORCE_VAPID:
							if (config.config->clientOptions->getVerbosity() > 3) {
								std::cerr << "sender public key: " << wpnKeys.getPublicKey() << std::endl 
									<< "sender private key: " << wpnKeys.getPrivateKey() << std::endl
									<< "endpoint: " << *it << std::endl
									<< "public key: " << config.vapid_recipient_p256dh << std::endl 
									<< "auth secret: " << config.auth_secret << std::endl
									<< "body: " << body << std::endl
									<< "sub: " << config.sub << std::endl;
							}
							r = webpushVapid(NULL, retval, wpnKeys.getPublicKey(), wpnKeys.getPrivateKey(), *it, config.vapid_recipient_p256dh, config.auth_secret, 
								body, config.sub, config.aesgcm ? AESGCM : AES128GCM);
							if (config.config->clientOptions->getVerbosity() > 3) {
								std::string filename = "aesgcm.bin";
								retval = webpushVapidCmd(wpnKeys.getPublicKey(), wpnKeys.getPrivateKey(), filename, *it, config.vapid_recipient_p256dh, config.auth_secret, 
								body, config.sub, config.aesgcm ? AESGCM : AES128GCM);
							}
							break;
					}
				}
				else
				{
					if (config.config->clientOptions->getVerbosity() > 1)
						std::cout << "Execute command " << config.command << " on " << *it << std::endl;
					switch (config.subscriptionMode) {
						case SUBSCRIBE_FORCE_FIREBASE:
							r = push2ClientDataFCM(&retval, serverKey, token, *it, "", config.command, 0, "", config.config->clientOptions->getVerbosity());
							break;
						case SUBSCRIBE_FORCE_VAPID:
							r = webpushVapid(NULL, retval, wpnKeys.getPublicKey(), wpnKeys.getPrivateKey(), *it, config.vapid_recipient_p256dh, config.vapid_recipient_auth, 
								body, config.sub, config.aesgcm ? AESGCM : AES128GCM);
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
		case CMD_PRINT_VERSION:
		{
			std::cout << config.versionString() << std::endl;
			break;
		}
		case CMD_GENERATE_VAPID_KEYS:
		{
			Subscription subscription;
			std::string d;
			std::string headers;

			config.config->wpnKeys->generate();
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
			std::cout << config.config->wpnKeys->toJsonString() << std::endl;
		}
			break;
		default:
			{
				do {
					quitFlag = 0;
					config.loadNotifyFuncs();
					MCSClient client(
						config.config->subscriptions,
						config.config->wpnKeys->getPrivateKey(),
						config.config->wpnKeys->getAuthSecret(),
						config.config->androidCredentials->getAndroidId(),
						config.config->androidCredentials->getSecurityToken(),
						onNotify, &config, onLog, &config,
						config.config->clientOptions->getVerbosity()
					);

					// check in
					if (config.config->androidCredentials->getAndroidId() == 0)
					{
						uint64_t androidId = config.config->androidCredentials->getAndroidId();
						uint64_t securityToken = config.config->androidCredentials->getSecurityToken();
						int r = checkIn(&androidId, &securityToken, config.config->clientOptions->getVerbosity());
						if (r < 200 || r >= 300)
						{
							return ERR_NO_ANDROID_ID_N_TOKEN;
						}
						config.config->androidCredentials->setAndroidId(androidId);
						config.config->androidCredentials->setSecurityToken(securityToken);
					}
					// register
					if (config.config->androidCredentials->getGCMToken().empty())
					{
						int r;
						for (int i = 0; i < 5; i++)
						{
							std::string gcmToken;
							r = registerDevice(&gcmToken,
								config.config->androidCredentials->getAndroidId(),
								config.config->androidCredentials->getSecurityToken(),
								config.config->androidCredentials->getAppId(),
								config.config->clientOptions->getVerbosity()
							);
							if (r >= 200 && r < 300)
							{
								config.config->androidCredentials->setGCMToken(gcmToken);
								break;
							}
							sleep(1);
						}
						if (r < 200 || r >= 300)
						{
							return ERR_NO_FCM_TOKEN;
						}
					}

					client.connect();
					std::cerr << PROMPT_STARTED << std::endl;
					readCommand(&quitFlag, std::cin, &client);
					client.disconnect();
					config.unloadNotifyFuncs();
				} while (quitFlag == 2);
			}
	}
	config.config->save();
	return 0;
}
