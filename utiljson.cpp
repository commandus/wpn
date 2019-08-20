#include "utiljson.h"

const char* JSON_ID = "id";
const char* JSON_NAME = "name";
const char* JSON_PERSISTENT_ID = "persistent_id";
const char* JSON_FROM = "from";
const char* JSON_APPNAME = "appName";
const char* JSON_APPID = "appId";
const char* JSON_SENT = "sent";

const char* JSON_NOTIFICATION = "notification";
static const std::string JSON_TO = "to";
static const char* JSON_NOTIFICATION_TITLE = "title";
static const char* JSON_NOTIFICATION_BODY = "body";
static const char* JSON_NOTIFICATION_ICON = "icon";
static const char* JSON_NOTIFICATION_SOUND = "sound";
static const char* JSON_NOTIFICATION_LINK = "click_action";
static const char* JSON_NOTIFICATION_CATEGORY = "category";
static const char* JSON_NOTIFICATION_DATA = "data";

static const std::string KEY_SUBSCRIPTION_PUBLIC = "public";
static const std::string KEY_SUBSCRIPTION_PRIVATE = "private";
static const std::string KEY_SUBSCRIPTION_ENDPOINT = "endpoint";
static const std::string KEY_SUBSCRIPTION_P256DH = "p256dh";
static const std::string KEY_SUBSCRIPTION_AUTH = "auth";
static const std::string KEY_SUBSCRIPTION_CONTACT = "contact";


std::string notify2json(
    uint64_t id,
	const char *name,
   	const char *persistent_id,
	const char *from,				///< e.g. BDOU99-h67HcA6JeFXHbSNMu7e2yNNu3RzoMj8TM4W88jITfq7ZmPvIM1Iv-4_l2LxQcYwhqby2xGpWwzjfAnG4
	const char *appName,
	const char *appId,
	int64_t sent,
    const NotifyMessageC *notify
) {
    json r = {
        { JSON_ID, id },
        { JSON_NAME, name },
        { JSON_PERSISTENT_ID, persistent_id },
        { JSON_FROM, from },
        { JSON_APPNAME, appName },
        { JSON_APPID, appId },
        { JSON_SENT, sent },
        { JSON_NOTIFICATION, 
            {
                { JSON_NOTIFICATION_TITLE, notify->title},
                { JSON_NOTIFICATION_BODY, notify->body},
                { JSON_NOTIFICATION_ICON, notify->icon},
                { JSON_NOTIFICATION_SOUND, notify->sound},
                { JSON_NOTIFICATION_LINK, notify->link},
                { JSON_NOTIFICATION_CATEGORY, notify->category},
                { JSON_NOTIFICATION_DATA, notify->data}
            }
        }
    };
    return r.dump();
}

std::string jsAddSubscription(
    const std::string &publicKey,
    const std::string &authSecret,
    const std::string &token)
{
	json js = {
		{ "publicKey", publicKey },
		{ "authSecret", authSecret },
		{ "token", token }
	};
    return js.dump();
}

bool jsGetSubscription(
    const std::string &js,
    std::string &retPublicKey,
    std::string &retToken,
    std::string &retAuthSecret
)
{	
	json j;
	try {
		std::stringstream(js) >> j;
	}
	catch (json::exception e) {
		return false;
	}
	catch (...) {
		return false;
	}
	try {
		json::const_iterator f = j.find("publicKey");
		if (f != j.end())
			retPublicKey = f.value();
		f = j.find("token");
		if (f != j.end()) 
			retToken = f.value();
		f = j.find("authSecret");
		if (f != j.end())
			retAuthSecret = f.value();
	} catch(...) {
		return false;
	}
}

bool jsValid (
    const std::string &js
) 
{
    bool r = false;
    try 
    {
        json response = json::parse(js);
        r = true;
    }
    catch(...) 
    {
    }
    return r;
}

std::string jsClientNotification
(
    const std::string &to,
    const std::string &subject, 
    const std::string &body,
    const std::string &icon,
    const std::string &link
)
{
	json requestBody = {
		{ JSON_NOTIFICATION, 
			{
				{ JSON_NOTIFICATION_BODY, body },
			}
		}
	};
	if (!to.empty())
		requestBody[JSON_TO] = to;
	json n = requestBody[JSON_NOTIFICATION];
	if (subject.empty())
		n[JSON_NOTIFICATION_TITLE] = subject;
	if (subject.empty())
		n[JSON_NOTIFICATION_ICON] = icon;
	if (link.empty())
		n[JSON_NOTIFICATION_LINK] = link;
	return requestBody.dump();
}

std::string jsClientCommand
(
    const std::string &client_token,
    const std::string &command,
    const std::string &persistent_id, 
    int code,
    const std::string &output,
    const std::string &server_key,
    const std::string &token
)
{
    json requestBody = {
		{"to", client_token},
		{"data", 
			{
				{"command", command},
				{"persistent_id", persistent_id},
				{"code", code},
				{"output", output},
				{"serverKey", server_key},
				{"token", token}
			}
		}
	};
    return requestBody.dump();
}

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
)
{
	json j;
	int r = 0;
	try {
		j = json::parse(value);
	}
	catch (...) {
		r = -1;
	}

	if (r == 0)
	{
		try {
			if (j.count(JSON_TO))
				to = j.at(JSON_TO);
			else
				to = "";
			json n = j.at(JSON_NOTIFICATION);
			if (n.count(JSON_NOTIFICATION_TITLE))
				title = n.at(JSON_NOTIFICATION_TITLE);
			else
				title = "";
			if (n.count(JSON_NOTIFICATION_BODY))
				body = n.at(JSON_NOTIFICATION_BODY);
			else
				body = "";
			if (n.count(JSON_NOTIFICATION_ICON))
				icon = n.at(JSON_NOTIFICATION_ICON);
			else
				icon = "";
			if (n.count(JSON_NOTIFICATION_LINK))
				click_action = n.at(JSON_NOTIFICATION_LINK);
			else
				click_action = "";
		} catch(...) {
			r = -2;
		}
	}
	return r;
}

/**
 * Parse subscription file
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
)
{
	json j;
	int r = 0;
	try {
		j = json::parse(value);
	}
	catch (...) {
		r = -1;
	}

	if (r == 0)
	{
		try {
			if (j.count(KEY_SUBSCRIPTION_PUBLIC))
				publicKey = j.at(KEY_SUBSCRIPTION_PUBLIC);
			else
				publicKey = "";
			if (j.count(KEY_SUBSCRIPTION_PRIVATE))
				privateKey = j.at(KEY_SUBSCRIPTION_PRIVATE);
			else
				privateKey = "";
			if (j.count(KEY_SUBSCRIPTION_ENDPOINT))
				endpoint = j.at(KEY_SUBSCRIPTION_ENDPOINT);
			else
				endpoint = "";
			if (j.count(KEY_SUBSCRIPTION_P256DH))
				p256dh = j.at(KEY_SUBSCRIPTION_P256DH);
			else
				p256dh = "";
			if (j.count(KEY_SUBSCRIPTION_AUTH))
				auth = j.at(KEY_SUBSCRIPTION_AUTH);
			else
				auth = "";
			if (j.count(KEY_SUBSCRIPTION_CONTACT))
				contact = j.at(KEY_SUBSCRIPTION_CONTACT);
			else
				contact = "";
		} catch(...) {
			r = -2;
		}
	}
	return r;
}

std::string jsSubscribeFCM(
    const std::string &authorizedEntity,
	const std::string &endPoint,
	const std::string &receiverAndroidId,
	const std::string &receiverSecurityToken
)
{
   	json j = {
		{ "authorized_entity", authorizedEntity },
		{ "endpoint", endPoint },
		{ "encryption_key", receiverAndroidId },
		{ "encryption_auth", receiverSecurityToken }
	};
	return j.dump();
}

bool jsSubscribeFCMParseResponse(
    const std::string &value,
    std::string &retToken,
    std::string &retPushSet
)
{
    json js;
	try {
		js = json::parse(value);
	}
	catch (...) {
        return false;
	}
    retToken = js["token"];
    retPushSet = js["pushSet"];
    return true;
}

std::string jsSubscribeFCMParseErrorResponse(
    const std::string &value
)
{
    std::string r = "";
    json js;
	try {
		js = json::parse(value);
	}
	catch (...) {
        return r;
	}
    json e = js["error"];
    r = e["message"];
    return r;
}
