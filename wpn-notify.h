#ifndef WPN_NOTIFY_H
#define WPN_NOTIFY_H	1
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
	const char *authorizedEntity;	///< e.g. 246829423295
	const char *title;
	const char *body;
	const char *icon;				///< Specifies an icon filename or stock icon to display.
	const char *sound;				///< sound file name
	const char *link;				///< click action
	const char *linkType;			///< click action content type
	int urgency; 					///< low- 0, normal, critical
	int timeout; 					///< timeout in milliseconds at which to expire the notification.
	const char *category;
	const char *extra;
	const char *data;				///< extra data in JSON format
} NotifyMessageC;

typedef void (*OnNotifyC)
(
	void *env,
	const char *persistent_id,
	const char *from,				///< e.g. BDOU99-h67HcA6JeFXHbSNMu7e2yNNu3RzoMj8TM4W88jITfq7ZmPvIM1Iv-4_l2LxQcYwhqby2xGpWwzjfAnG4
	const char *appName,
	const char *appId,
	int64_t sent,
	const NotifyMessageC *request
);

typedef void(*OnLogC)
(
	void *env,
	int severity,
	const char *message
);

extern "C"
bool desktopNotify
(
	void *env,
	const char *persistent_id,
	const char *from,
	const char *appName,
	const char *appId,
	int64_t sent,
	const NotifyMessageC *notification
);

#endif
