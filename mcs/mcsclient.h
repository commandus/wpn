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

#include <inttypes.h>
#include "wpn-config.h"
#include "wp-storage-file.h"

#define ERR_NO_CONFIG				-1
#define ERR_NO_KEYS					-2
#define ERR_NO_CREDS				-3
#define ERR_NO_ANDROID_ID_N_TOKEN	-4
#define ERR_CHECKIN					-5


class MCSClient
{
private:
	const WpnConfig *mConfig;
	const WpnKeys* mKeys;
	AndroidCredentials *mCredentials;

	int curlPost(
		const std::string &url,
		const std::string &contentType,
		const std::string &content,
		std::string *retval
	);
	int checkIn();
	std::string getAppId();
public:
	MCSClient();
	MCSClient(
		const WpnConfig *config, 
		const WpnKeys* mKeys,
		AndroidCredentials *androidCredentials
 	);
	MCSClient(const MCSClient& other);
	void setConfig(const WpnConfig *config);
	void setKeys(const WpnKeys* keys);
	~MCSClient();
	MCSClient& operator=(const MCSClient& other);
	bool operator==(const MCSClient& other) const;
	
		
	uint64_t getAndroidId();
    uint64_t getSecurityToken();
	bool hasIdNToken();
	int registerDevice();
	int read();
	int write();

	int connect();
};

#endif // MCSCLIENT_H
