/*
 * Web push notification command line client
 * Copyright (C) 2018  Andrei Ivanov andrei.i.ivanov@commandus.com
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#ifndef MCSCLIENT_H
#define MCSCLIENT_H

#include <thread>
#include <sstream>
#include <inttypes.h>

#include <google/protobuf/message.h>

#include "wpn-config.h"
#include "wp-storage-file.h"
#include "sslfactory.h"
#include "onullstream.hpp"

#define ERR_NO_CONFIG				-1
#define ERR_NO_KEYS					-2
#define ERR_NO_CREDS				-3
#define ERR_NO_ANDROID_ID_N_TOKEN	-4
#define ERR_NO_FCM_TOKEN			-5
#define ERR_CHECKIN					-6
#define ERR_REGISTER_VAL			-7	// Error registering
#define ERR_REGISTER_FAIL			-8
#define ERR_NO_CONNECT				-9
#define ERR_MEM						-10

enum PROTO_STATE {
	STATE_VERSION = 0,
	STATE_TAG = 1
};

using namespace google::protobuf;

class MCSClient;

class MCSReceiveBuffer
{
private:
	MCSClient *mClient;
	u_int8_t mVersion;	// last known is 38
	enum PROTO_STATE state;
	int parse();
	MessageLite *createMessage(uint8_t tag);
public:
	std::string buffer;
	MCSReceiveBuffer();
	void setClient(MCSClient *client);
	// Return 0 if incomplplete	and is not parcelable
	int process();
	void put(const void *buf, int size);
	uint8_t getVersion();
};

class MCSClient
{
private:
	onullstream onullstrm;
	MCSReceiveBuffer mBuffer;
	SSLFactory mSSLFactory;
	int mSocket;
	WpnConfig *mConfig;	// config.wpnKeys, config.androidCredentials
	
	int curlPost(
		const std::string &url,
		const std::string &contentType,
		const std::string &content,
		std::string *retval
	);
	int checkIn();
	std::string getAppId();
	/// obtain GCM token
 	int registerDevice();
	// std::vector<std::string> mPersistentIds;
	void init();
public:
	bool mStop;
	MCSReceiveBuffer *mStream;
	SSL *mSsl;

	MCSClient();
	MCSClient(
		WpnConfig *config
	);
	MCSClient(const MCSClient& other);
	void setConfig(WpnConfig *config);
	WpnConfig *getConfig();
	~MCSClient();

	uint64_t getAndroidId();
    uint64_t getSecurityToken();
	bool hasIdNToken();

	int write();

	int logIn();

	int connect();
	void stop();
	int sendVersion();
	int sendTag
	(
		uint8_t tag,
		const MessageLite *msg
	);

	void ping();
	
	void writeStream(std::istream &strm);
	std::ostream &log
	(
		int level
	);
	int decode
	(
		std::string &retval,
		const std::string &source,
		const std::string &cryptoKeyHeader,
		const std::string &encryptionHeader
	);

	static void mkNotifyMessage
	(
		NotifyMessage &retval,
		const std::string &authorizedEntity,	///< e.g. 246829423295
		const std::string &title,
		const std::string &body,
		const std::string &icon,
		const std::string &click_action,
		const std::string &data
	);

	static bool mkNotifyMessage
	(
		NotifyMessage &retval,
		const std::string &json
	);

	size_t notifyAll
	(
		const std::string &persistent_id,
		const std::string &from,
		const std::string &appName,
		const std::string &appId,
		int64_t sent,
		const NotifyMessage &notification
	) const;
};

#endif // MCSCLIENT_H
