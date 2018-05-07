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

typedef bool (*desktopNotifyFunc)
(
	const std::string &title,
	const std::string &body,
	const std::string &icon,		///< Specifies an icon filename or stock icon to display.
	const std::string &sound,		///< sound file name
	const std::string &link,		///< click action
	const std::string &linkType,	///< click action
	int urgency, 					///< low- 0, normal, critical
	int timeout, 					///< timeout in milliseconds at which to expire the notification.
	const std::string &category,
	const std::string &extra
);

extern "C"
bool desktopNotify
(
	const std::string &title,
	const std::string &body,
	const std::string &icon = "",		///< Specifies an icon filename or stock icon to display.
	const std::string &sound = "",		///< sound file name
	const std::string &link = "",		///< click action
	const std::string &linkType = "",	///< click action
	int urgency = 0, 					///< low- 0, normal, critical
	int timeout = 0, 					///< timeout in milliseconds at which to expire the notification.
	const std::string &category = "",
	const std::string &extra = ""
);