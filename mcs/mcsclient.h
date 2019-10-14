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
#include <algorithm>
#include <iostream>
#include <inttypes.h>

#include <google/protobuf/message.h>
#include <ece.h>

#include "wpn-notify.h"
#include "wp-storage-file.h"
#include "sslfactory.h"
#include "onullstream.hpp"

#include "heartbeat.h"

enum PROTO_STATE {
	STATE_VERSION = 0,
	STATE_TAG = 1
};

typedef struct
{
	std::string authorizedEntity;	///< e.g. 246829423295
	std::string title;
	std::string body;
	std::string icon;				///< Specifies an icon filename or stock icon to display.
	std::string sound;				///< sound file name
	std::string link;				///< click action
	std::string linkType;			///< click action content type
	int urgency; 					///< low- 0, normal, critical
	int timeout; 					///< timeout in milliseconds at which to expire the notification.
	std::string category;
	std::string extra;
	std::string data;				///< extra data in JSON format
} NotifyMessage;

using namespace google::protobuf;

struct severity {
	int value = 0;
	severity(int v)
		: value(v)
	{
	}
};

class CallbackLogger : public std::streambuf
{
private:
	int verbosity;
	std::stringstream buffer;
	OnLogC onLog;
	void *onLogEnv;
	CallbackLogger &flush();
public:
	CallbackLogger();
	void setCallback(
		OnLogC aonLog,
		void *aonLogEnv
	);
	virtual int overflow(int c);
	friend CallbackLogger& operator<<(CallbackLogger &out, severity const& v) {
		out.verbosity = v.value;
		return out;
	}
	friend CallbackLogger& operator<<(CallbackLogger &out, const std::string &v) {
		out.onLog(out.onLogEnv, out.verbosity, v.c_str());
		return out;
	}
	friend CallbackLogger& operator<<(CallbackLogger &out, int v) {
		out.onLog(out.onLogEnv, out.verbosity, std::to_string(v).c_str());
		return out;
	}
	friend CallbackLogger& operator<<(CallbackLogger &out, int64_t v) {
		out.onLog(out.onLogEnv, out.verbosity, std::to_string(v).c_str());
		return out;
	}
	friend CallbackLogger& operator<<(CallbackLogger &out, uint32_t v) {
		out.onLog(out.onLogEnv, out.verbosity, std::to_string(v).c_str());
		return out;
	}
	friend CallbackLogger& operator<<(CallbackLogger &out, uint64_t v) {
		out.onLog(out.onLogEnv, out.verbosity, std::to_string(v).c_str());
		return out;
	}
};

class MCSClient
{
private:
	enum PROTO_STATE state;
	onullstream onullstrm;
	SSLFactory mSSLFactory;
	int mSocket;
	std::string mStream;
	Subscriptions *mSubscriptions;
	bool mStop;
	SSL *mSsl;
	bool hasIdNToken();
	bool ready();
	// Return 0 if incomplete and is not parcelable
	void put(const void *buf, int size);
	std::thread *listenerThread;
	uint32_t lastStreamIdRecieved;
	uint32_t lastStreamIdSent;
	int process();
	int sendVersion();
	// called by HeartbeatManager
	void sendHeartBeat();
	void reconnect();
public:
	HeartbeatManager *heartbeatManager;
	uint8_t privateKey[ECE_WEBPUSH_PRIVATE_KEY_LENGTH];
	uint8_t authSecret[ECE_WEBPUSH_AUTH_SECRET_LENGTH];
	uint64_t androidId;
	uint64_t securityToken;
	OnNotifyC onNotify;
	void *onNotifyEnv;
	OnLogC onLog;
	void *onLogEnv;

	int verbosity;
	void setLastPersistentId(const std::string &subscriptionKey, const std::string &persistentId);

	MCSClient(
		Subscriptions *subscriptions,
		const std::string &privateKey,
		const std::string &authSecret,
		uint64_t androidId,
		uint64_t securityToken,
		OnNotifyC onNotify, 
		void *onNotifyEnv,
		OnLogC onLog,
		void *onLogEnv,
		int verbosity
	);
	MCSClient(const MCSClient& other);
	~MCSClient();

	int logIn();
	int connect();
	void disconnect();
	int send
	(
		uint8_t tag,
		const MessageLite *msg
	);

	int ping();
	int read();
	int write(
		const std::string &receiverId,
		const std::string &value
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

	void notify
	(
		const std::string &persistent_id,
		const std::string &from,
		const std::string &appName,
		const std::string &appId,
		int64_t sent,
		const NotifyMessage &notification
	) const;

	CallbackLogger log;

	int sendStreamAck(const std::string &persistent_id);
	uint32_t getLastStreamIdRecieved();
	void setLastStreamIdRecieved(uint32_t value);
};

/**
 * Start client
 * @param retcode can be NULL
 */
void *startClient
(
	int *retcode,
	Subscriptions *subscriptions,
	const std::string &privateKey,
	const std::string &authSecret,
	uint64_t androidId,
	uint64_t securityToken,
	OnNotifyC onNotify,
	void *onNotifyEnv,
	OnLogC onLog,
	void *onLogEnv,
	int verbosity
);

/**
 * Stop client
 */
void stopClient
(
	void *client
);

#endif // MCSCLIENT_H
