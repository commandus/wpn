/*
 * Copyright (c) 2019 Andrei Ivanov andrei.i.ivanov@gmail.com
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
 */

#ifndef UTILVAPID_H
#define UTILVAPID_H 1

#include <string>

#define AESGCM 		0
#define AES128GCM	1

/**
 * Register application catewgory name
 * app=org.chromium.linux
 * https://android.clients.google.com/c2dm/register3
 */
#define APP_CATEGORY	"org.chromium.linux"

std::string base64UrlEncode(
	const void *data,
	size_t size
);

/**
 * Helper function for testing
 * Print out "curl ..."  command line string
 * @param From: publicKey e.g. "BM9Czc7rYYOinc7x_ALzqFgPSXV497qg76W6csYRtCFzjaFHGyuzP2a08l1vykEV1lgq6P83BOhB9xp-H5wCr1A";
 * @param From: privateKey e.g. "_93..";
 * @param filename	temporary file keeping AES GCM ciphered data
 * @param To: endpoint recipient endpoint
 * @param To: p256dh recipient key 
 * @param To: auth recipient key auth 
 * @param body JSON string message
 * @param Optional contact mailto:
 * @param contentEncoding AESGCM or AES128GCM
 */
std::string webpushVapidCmd(
	const std::string &publicKey,
	const std::string &privateKey,
	const std::string &filename,
	const std::string &endpoint,
	const std::string &p256dh,
	const std::string &auth,
	const std::string &body,
	const std::string &contact,
	int contentEncoding,
	time_t expiration = 0
);

/**
 * Send VAPID web push using CURL library
 * @param retval return error description string
 * @param From: publicKey e.g. "BM9Czc7rYYOinc7x_ALzqFgPSXV497qg76W6csYRtCFzjaFHGyuzP2a08l1vykEV1lgq6P83BOhB9xp-H5wCr1A";
 * @param From: privateKey e.g. "_93..";
 * @param To: endpoint recipient endpoint
 * @param To: p256dh recipient key 
 * @param To: auth recipient key auth 
 * @param body JSON string message
 * @param optional originator contact mailto: or http[s]: URI
 * @param contentEncoding AESGCM or AES128GCM
 * @return 0 or positive- HTTP code(200..299- success), negative- error code
 */
int webpushVapid(
	std::string &retval,
	const std::string &publicKey,
	const std::string &privateKey,
	const std::string &endpoint,
	const std::string &p256dh,
	const std::string &auth,
	const std::string &body,
	const std::string &contact,
	int contentEncoding,
	time_t expiration = 0
);

/**
 * Send VAPID web push using CURL library
 * @param retval return error description string
 * @param subscriptionJSON: {public, private, endpoint, p256dh, auth, contact} 
 * @param subject 
 * @param body message
 * @param icon
 * @param link 
 * @param optional originator contact mailto: or http[s]: URI
 * @param contentEncoding AESGCM or AES128GCM
 * @return 0 or positive- HTTP code(200..299- success), negative- error code
 */
int webpushVapidJSON(
	std::string &retval,
	const std::string &subscriptionJSON,
	const std::string &subject,
	const std::string &body,
	const std::string &icon, 
	const std::string &link,
	int contentEncoding,
	time_t expiration = 0
);

/**
 * Push "command output" to device
 * @param retval return string
 * @param From: publicKey e.g. "BM9Czc7rYYOinc7x_ALzqFgPSXV497qg76W6csYRtCFzjaFHGyuzP2a08l1vykEV1lgq6P83BOhB9xp-H5wCr1A";
 * @param From: privateKey e.g. "_93..";
 * @param To: endpoint recipient endpoint
 * @param To: p256dh recipient key 
 * @param To: auth recipient key auth 
 * @param persistent_id reference to request. If empty, it is request, otherwise response
 * @param command command line
 * @param code execution return code, usually 0
 * @param output result from stdout
 * @param verbosity level
 * @param contact mailto:
 * @param contentEncoding AESGCM or AES128GCM
 * @param expiration expiration time unix epoch seconds, default 0- now + 12 hours
 * @return 200-299- success, <0- error
*/
int webpushVapidData
(
	std::string &retval,
	const std::string &publicKey,
	const std::string &privateKey,
	const std::string &endpoint,
	const std::string &p256dh,
	const std::string &auth,

	const std::string &persistent_id,
	const std::string &command,
	int code,
	const std::string &output,
	int verbosity,
 
 	const std::string &contact,
	int contentEncoding,
	time_t expiration = 0
);

/**
 * Make notification JSON file
 * @return JSON string
 */
std::string mkNotificationJson
(
	const std::string &to,
	const std::string &subject,
	const std::string &body,
	const std::string &icon, 
	const std::string &link 
);

/**
 * Parse notification file
 * @return 0- success, -1: Invalid JSON, -2: Important information missed
 */
int parseNotificationJson
(
	const std::string &value,
	std::string &to,
	std::string &title,
	std::string &body,
	std::string &icon, 
	std::string &click_action
);

/**
 * Make subscrtption JSON file
 * @return JSON string
 */
std::string mkSubscriptionJson
(
	const std::string &publicKey,
	const std::string &privateKey,
	const std::string &endpoint,
	const std::string &p256dh,
	const std::string &auth,
	const std::string &contact
);

/**
 * Parse subscruption file
 * @return 0- success, -1: Invalid JSON, -2: Important information missed
 */
int parseSubscriptionJson
(
	const std::string &value,
	std::string &publicKey,
	std::string &privateKey,
	std::string &endpoint,
	std::string &p256dh,
	std::string &auth,
	std::string &contact
);

#endif
