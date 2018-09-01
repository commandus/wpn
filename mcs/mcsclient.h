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

#define ERR_NO_CONFIG				-21
#define ERR_NO_KEYS					-22
#define ERR_NO_CREDS				-23
#define ERR_NO_ANDROID_ID_N_TOKEN	-24
#define ERR_NO_FCM_TOKEN			-25
#define ERR_CHECKIN					-26
#define ERR_NO_CONNECT				-27
#define ERR_MEM						-28

enum PROTO_STATE {
	STATE_VERSION = 0,
	STATE_TAG = 1
};

using namespace google::protobuf;

class MCSClient;

class MCSReceiveBuffer
{
private:
public:
	std::string buffer;
	MCSReceiveBuffer();
	// Return 0 if incomplete and is not parcelable
	void put(const void *buf, int size);
};

class MCSClient
{
private:
	enum PROTO_STATE state;
	onullstream onullstrm;
	SSLFactory mSSLFactory;
	int mSocket;
	WpnConfig *mConfig;	// config.wpnKeys, config.androidCredentials
	
	// std::vector<std::string> mPersistentIds;
	MCSReceiveBuffer mStream;
public:
	bool mStop;
	SSL *mSsl;

	MCSClient();
	MCSClient(
		WpnConfig *config
	);
	MCSClient(const MCSClient& other);
	void setConfig(WpnConfig *config);
	WpnConfig *getConfig();
	~MCSClient();

	bool hasIdNToken();

	std::ostream::pos_type write();

	// Return 0 if incomplete and is not parcelable
	void put(const void *buf, int size);

	int logIn();
	int connect();
	void stop();
	int sendVersion();
	int sendTag
	(
		uint8_t tag,
		const MessageLite *msg
	);

	int ping();

	void writeStream(std::istream &strm);
	std::ostream &log
	(
		int level
	);

	int process();

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

	/**
	* Parse command
	* @param retval return value. If it is data, return data JSON string in retval.data
	* @param value JSON data to be parsed
	* @return true
	*/
	bool parseJSONCommandOutput
	(
		std::string &serverKey,
		std::string &token,
		std::string &persistent_id,
		std::string &command,
		int *code,
		std::string &output,
		const std::string &value
	);

	/**
	* Parse FCM JSON message into notification structure or copy data 
	* @param retval return value. If it is data, return data JSON string in retval.data
	* @param json JSON data to be parsed
	* @return 0- notification, 1- data, less 0 error
	*/
	static int parseJSONNotifyMessage
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
