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
 * @file wpn-lnotify.cpp
 * 
 */

#include <string>

static const std::string APP_NAME("wpn");

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

// return true if has reply
typedef bool (*desktopNotifyFunc)
(
	const std::string &persistent_id,
	const std::string &from,				///< e.g. BDOU99-h67HcA6JeFXHbSNMu7e2yNNu3RzoMj8TM4W88jITfq7ZmPvIM1Iv-4_l2LxQcYwhqby2xGpWwzjfAnG4
	const std::string &appName,
	const std::string &appId,
	int64_t sent,
 
	const NotifyMessage *request,
	NotifyMessage *reply
);

extern "C"
bool desktopNotify
(
	const std::string &persistent_id,
	const std::string &from,				///< e.g. BDOU99-h67HcA6JeFXHbSNMu7e2yNNu3RzoMj8TM4W88jITfq7ZmPvIM1Iv-4_l2LxQcYwhqby2xGpWwzjfAnG4
	const std::string &appName,
	const std::string &appId,
	int64_t sent,
 
	const NotifyMessage *notification,
	NotifyMessage *reply
);
