#include "utiljson.h"

#define SUBSCRIBE_FORCE_FIREBASE	1
#define SUBSCRIBE_FORCE_VAPID		2

static const char* JSON_ID = "id";
static const char* JSON_NAME = "name";
static const char* JSON_PERSISTENT_ID = "persistent_id";
static const char* JSON_FROM = "from";
static const char* JSON_APPNAME = "appName";
static const char* JSON_APPID = "appId";
static const char* JSON_SENT = "sent";

static const char* JSON_NOTIFICATION = "notification";
static const char* JSON_TO = "to";
static const char* JSON_NOTIFICATION_TITLE = "title";
static const char* JSON_NOTIFICATION_BODY = "body";
static const char* JSON_NOTIFICATION_ICON = "icon";
static const char* JSON_NOTIFICATION_SOUND = "sound";
static const char* JSON_NOTIFICATION_LINK = "click_action";
static const char* JSON_NOTIFICATION_CATEGORY = "category";
static const char* JSON_NOTIFICATION_DATA = "data";

static const char* KEY_SUBSCRIPTION_PUBLIC = "public";
static const char* KEY_SUBSCRIPTION_PRIVATE = "private";
static const char* KEY_SUBSCRIPTION_ENDPOINT = "endpoint";
static const char* KEY_SUBSCRIPTION_P256DH = "p256dh";
static const char* KEY_SUBSCRIPTION_AUTH = "auth";
static const char* KEY_SUBSCRIPTION_CONTACT = "contact";

#ifdef USE_JSON_RAPID
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;

#define RAPIDJSON_ADD_PCHAR(D, V, A, NAME, STR) \
	{ \
		size_t len = strlen(STR); \
		if (len) { \
			V.SetString(STR, len, A); \
			D.AddMember(#NAME, V, A); \
		} \
	}

#define RAPIDJSON_ADD_STRING(D, V, A, NAME, STR) \
	if (!STR.empty()) { \
		V.SetString(STR.c_str(), STR.size(), A); \
		D.AddMember(#NAME, V, A); \
	}

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
	Document d;
	Document::AllocatorType& a = d.GetAllocator();
	d.SetObject();
	Value v;
	v.SetUint64(id);
	d.AddMember("id", v, a);
	RAPIDJSON_ADD_PCHAR(d, v, a, name, name)
	RAPIDJSON_ADD_PCHAR(d, v, a, persistent_id, persistent_id)
	RAPIDJSON_ADD_PCHAR(d, v, a, from, from)
	RAPIDJSON_ADD_PCHAR(d, v, a, appName, appName)
	RAPIDJSON_ADD_PCHAR(d, v, a, appId, appId)
	RAPIDJSON_ADD_PCHAR(d, v, a, appName, appName)
	v.SetInt64(sent);
	d.AddMember("sent", v, a);
	Value n;
	n.SetObject();
	d.AddMember("notification", n, a);
	RAPIDJSON_ADD_PCHAR(n, v, a, title, notify->title)
	RAPIDJSON_ADD_PCHAR(n, v, a, body, notify->body)
	RAPIDJSON_ADD_PCHAR(n, v, a, icon, notify->icon)
	RAPIDJSON_ADD_PCHAR(n, v, a, sound, notify->sound)
	RAPIDJSON_ADD_PCHAR(n, v, a, click_action, notify->link)
	RAPIDJSON_ADD_PCHAR(n, v, a, category, notify->category)
	RAPIDJSON_ADD_PCHAR(n, v, a, data, notify->data)
 	
	StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    d.Accept(writer);
	return buffer.GetString();
}

std::string jsAddSubscription(
    const std::string &publicKey,
    const std::string &authSecret,
    const std::string &token)
{
	Document d;
	Document::AllocatorType& a = d.GetAllocator();
	d.SetObject();
	Value v;
	RAPIDJSON_ADD_STRING(d, v, a, publicKey, publicKey)
	RAPIDJSON_ADD_STRING(d, v, a, authSecret, authSecret)
	RAPIDJSON_ADD_STRING(d, v, a, token, token)

	StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    d.Accept(writer);
	return buffer.GetString();
}

bool jsGetSubscription(
    const std::string &js,
    std::string &retPublicKey,
    std::string &retToken,
    std::string &retAuthSecret
)
{	
	Document d;
	d.Parse(js.c_str());
	if (d.HasParseError()) {
		return false;
	}
	if (d.HasMember("publicKey")) {
		retPublicKey = d["publicKey"].GetString();
	}
	if (d.HasMember("token")) {
		retToken = d["token"].GetString();
	}
	if (d.HasMember("authSecret")) {
		retAuthSecret = d["authSecret"].GetString();
	}
	return true;
}

bool jsValid (
    const std::string &js
) 
{
	Document d;
	d.Parse(js.c_str());
	return !d.HasParseError();
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
	Document d;
	Document::AllocatorType& a = d.GetAllocator();
	d.SetObject();
	Value n;
	n.SetObject();
	d.AddMember("notification", n, a);
	Value v;
	RAPIDJSON_ADD_STRING(d, v, a, to, to)
	
	RAPIDJSON_ADD_STRING(n, v, a, body, body)
	RAPIDJSON_ADD_STRING(n, v, a, title, subject)
	RAPIDJSON_ADD_STRING(n, v, a, icon, icon)
	RAPIDJSON_ADD_STRING(n, v, a, click_action, link)

	StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    d.Accept(writer);
	return buffer.GetString();
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
	Document d;
	Document::AllocatorType& a = d.GetAllocator();
	d.SetObject();
	Value n;
	n.SetObject();
	d.AddMember("data", n, a);
	Value v;
	RAPIDJSON_ADD_STRING(d, v, a, to, client_token)
	
	RAPIDJSON_ADD_STRING(n, v, a, command, command)
	RAPIDJSON_ADD_STRING(n, v, a, persistent_id, persistent_id)
	v.SetUint64(code);
	n.AddMember("code", v, a);
	RAPIDJSON_ADD_STRING(n, v, a, output, output)
	RAPIDJSON_ADD_STRING(n, v, a, server_key, server_key)
	RAPIDJSON_ADD_STRING(n, v, a, token, token)
	StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    d.Accept(writer);
	return buffer.GetString();
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
	Document d;
	d.Parse(value.c_str());
	if (d.HasParseError()) {
		return -1;
	}
	to = "";
	title = "";
	body = "";
	icon = "";
	click_action = "";

	if (d.HasMember("to")) {
		to = d["to"].GetString();
	}
	if (!d.HasMember("notification")) {
		return 0;
	}
	Value n = d["notification"].GetObject();
	if (d.HasMember("title")) {
		title = d["title"].GetString();
	}
	if (d.HasMember("body")) {
		body = d["body"].GetString();
	}
	if (d.HasMember("icon")) {
		icon = d["icon"].GetString();
	}
	if (d.HasMember("click_action")) {
		click_action = d["click_action"].GetString();
	}
	return 0;
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
	Document d;
	d.Parse(value.c_str());
	if (d.HasParseError()) {
		return -1;
	}
	publicKey = "";
	privateKey = "";
	endpoint = "";
	p256dh = "";
	auth = "";
	contact = "";

	if (d.HasMember("public")) {
		publicKey = d["public"].GetString();
	}
	if (d.HasMember("private")) {
		privateKey = d["private"].GetString();
	}
	if (d.HasMember("endpoint")) {
		endpoint = d["endpoint"].GetString();
	}
	if (d.HasMember("p256dh")) {
		p256dh = d["p256dh"].GetString();
	}
	if (d.HasMember("auth")) {
		auth = d["auth"].GetString();
	}
	if (d.HasMember("contact")) {
		contact = d["contact"].GetString();
	}
	return 0;
}

std::string jsSubscribeFCM(
    const std::string &authorizedEntity,
	const std::string &endPoint,
	const std::string &receiverAndroidId,
	const std::string &receiverSecurityToken
)
{
	Document d;
	Document::AllocatorType& a = d.GetAllocator();
	d.SetObject();
	Value v;
	RAPIDJSON_ADD_STRING(d, v, a, authorized_entity, authorizedEntity)
	RAPIDJSON_ADD_STRING(d, v, a, endpoint, endPoint)
	RAPIDJSON_ADD_STRING(d, v, a, encryption_key, receiverAndroidId)
	RAPIDJSON_ADD_STRING(d, v, a, encryption_auth, receiverSecurityToken)

	StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    d.Accept(writer);
	return buffer.GetString();
}

bool jsSubscribeFCMParseResponse(
    const std::string &value,
    std::string &retToken,
    std::string &retPushSet
)
{
	Document d;
	d.Parse(value.c_str());
	if (d.HasParseError()) {
		return -1;
	}
    retToken = "";
    retPushSet = "";
	StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    d.Accept(writer);
	return buffer.GetString();

	if (d.HasMember("token")) {
		retToken = d["token"].GetString();
	}
	if (d.HasMember("pushSet")) {
		retPushSet = d["pushSet"].GetString();
	}
	return 0;
}

std::string jsSubscribeFCMParseErrorResponse(
    const std::string &value
)
{
	Document d;
	d.Parse(value.c_str());
	if (d.HasParseError()) {
		return "";
	}
    std::string r = "";

	if (d.HasMember("error")) {
		Value e = d["error"].GetObject();
		if (d.HasMember("message")) {
			r = e["message"].GetString();
		}
	}
	return r;
}

int parseJsonRecipientTokens
(
	std::vector<std::string> &retval,
	const std::string &value
)
{
	int c = 0;
	try
	{
		JsonDocument list;
		list.Parse(value.c_str());
		if (list.HasParseError()) {
			return -1;
		}
		if (!list.IsArray()) {
			return -2;
		}
		// iterate the array
		for (size_t it = 0; it < list.Size(); it++) 
		{
			if (!list[it].IsArray()) {
				continue;
			}
			if (!list[it].Size() < 2) {
				continue;
			}
			retval.push_back(list[it][1].GetString());
			c++;
		}
	}
	catch(...)
	{
	}
	return c;
}

std::string jsDump(
    const JsonValue &value
) {
	Document d;
	Document::AllocatorType& a = d.GetAllocator();
	d.CopyFrom(value, a);
	StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    d.Accept(writer);
	return std::string(buffer.GetString());
}

std::string jsDumpDocument(
    JsonDocument &value
) {
	Document::AllocatorType& a = value.GetAllocator();
	StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    value.Accept(writer);
	return std::string(buffer.GetString());
}

JsonValue jsClientOptions(
	const std::string &name,
	int verbosity
) {
	Document d;
	Document::AllocatorType& a = d.GetAllocator();
	d.SetObject();
	Value v;
	RAPIDJSON_ADD_STRING(d, v, a, name, name)
	v.SetInt(verbosity);
	d.AddMember("verbosity", v, a);
	return d.GetObject();
}

bool jsGetString(
	const JsonValue &value,
	const std::string &name,
	std::string &retVal
)
{
	if (value.HasMember(name.c_str())) {
		retVal = value[name.c_str()].GetString();
	}
}

bool jsGetInt(
	const JsonValue &value,
	const std::string &name,
	int &retVal
)
{
	if (value.HasMember(name.c_str())) {
		retVal = value[name.c_str()].GetInt();
	}
}

bool jsGetUint64(
	const JsonValue &value,
	const std::string &name,
	uint64_t &retVal
)
{
	if (value.HasMember(name.c_str())) {
		retVal = value[name.c_str()].GetUint64();
	}
}

size_t jsArrayCount(
	const JsonValue &value
) {
	if (!value.IsArray()) {
		return 0;
	}
	return value.Size();
}

const JsonValue &jsArrayGet(
	const JsonValue &value,
	size_t index
) {
	return value[index];
}

JsonValue jsAndroidCredentials(
	const std:: string &appId,
	uint64_t androidId,
	uint64_t securityToken, 
	const std:: string &GCMToken
) {
	Document d;
	Document::AllocatorType &a = d.GetAllocator();
	d.SetObject();
	Value v;
	RAPIDJSON_ADD_STRING(d, v, a, appId, appId)
	v.SetUint64(androidId);
	d.AddMember("androidId", v, a);
	v.SetUint64(androidId);
	d.AddMember("securityToken", v, a);
	RAPIDJSON_ADD_STRING(d, v, a, GCMToken, GCMToken)
	return d.GetObject();
}

JsonValue jsWpnKeys(
	uint64_t id,
	uint64_t secret,	
	const std::string &privateKey,
	const std::string &publicKey,
	const std::string &authSecret
) {
	Document d;
	Document::AllocatorType& a = d.GetAllocator();
	d.SetObject();
	Value v;
	v.SetUint64(id);
	d.AddMember("id", v, a);
	v.SetUint64(secret);
	d.AddMember("secret", v, a);
	RAPIDJSON_ADD_STRING(d, v, a, privateKey, privateKey)
	RAPIDJSON_ADD_STRING(d, v, a, publicKey, publicKey)
	RAPIDJSON_ADD_STRING(d, v, a, authSecret, authSecret)
	return d.GetObject();
}

JsonValue jsSubscription(
	int subscribeMode, 
	const std::string &name,
	const std::string &token,
	const std::string &persistentId,
	const std::string &subscribeUrl,
	const std::string &endpoint,
	const std::string &authorizedEntity,
	const std::string &pushSet,
	const std::string &serverKey,
	const std::string &sentToken,
	uint64_t id,
	const std::string &publicKey,
	const std::string &authSecret
)
{ 
	Document d;
	Document::AllocatorType& a = d.GetAllocator();
	d.SetObject();
	Value v;
	v.SetInt(subscribeMode);
	d.AddMember("subscribeMode", v, a);
	RAPIDJSON_ADD_STRING(d, v, a, name, name)
	RAPIDJSON_ADD_STRING(d, v, a, token, token)
	RAPIDJSON_ADD_STRING(d, v, a, persistentId, persistentId)

	switch (subscribeMode) {
	case SUBSCRIBE_FORCE_FIREBASE:
		RAPIDJSON_ADD_STRING(d, v, a, subscribeUrl, subscribeUrl)
		RAPIDJSON_ADD_STRING(d, v, a, endpoint, endpoint)
		RAPIDJSON_ADD_STRING(d, v, a, authorizedEntity, authorizedEntity)
		RAPIDJSON_ADD_STRING(d, v, a, pushSet, pushSet)
		RAPIDJSON_ADD_STRING(d, v, a, serverKey, serverKey)
		break;
	case SUBSCRIBE_FORCE_VAPID:
		RAPIDJSON_ADD_STRING(d, v, a, sentToken, sentToken)
		v.SetUint64(id);
		d.AddMember("id", v, a);
		RAPIDJSON_ADD_STRING(d, v, a, publicKey, publicKey)
		RAPIDJSON_ADD_STRING(d, v, a, authSecret, authSecret)
		break;
	default:
		break;
	}
	return d.GetObject();
}

#endif

#ifdef USE_JSON_NLOHMANN

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
    JsonValue r = {
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
	JsonValue js = {
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
	JsonValue j;
	try {
		std::stringstream(js) >> j;
	}
	catch (JsonValue::exception e) {
		return false;
	}
	catch (...) {
		return false;
	}
	try {
		JsonValue::const_iterator f = j.find("publicKey");
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
	return true;
}

bool jsValid (
    const std::string &js
) 
{
    bool r = false;
    try 
    {
        JsonValue response = JsonValue::parse(js);
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
	JsonValue requestBody = {
		{ JSON_NOTIFICATION, 
			{
				{ JSON_NOTIFICATION_BODY, body },
			}
		}
	};
	if (!to.empty())
		requestBody[JSON_TO] = to;
	JsonValue n = requestBody[JSON_NOTIFICATION];
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
    JsonValue requestBody = {
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
	JsonValue j;
	int r = 0;
	try {
		j = JsonValue::parse(value);
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
			JsonValue n = j.at(JSON_NOTIFICATION);
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
	JsonValue j;
	int r = 0;
	try {
		j = JsonValue::parse(value);
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
   	JsonValue j = {
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
    JsonValue js;
	try {
		js = JsonValue::parse(value);
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
    JsonValue js;
	try {
		js = JsonValue::parse(value);
	}
	catch (...) {
        return r;
	}
    JsonValue e = js["error"];
    r = e["message"];
    return r;
}

int parseJsonRecipientTokens
(
	std::vector<std::string> &retval,
	const std::string &value
)
{
	int c = 0;
	try
	{
		JsonValue list = JsonValue::parse(value);
		// iterate the array
		for (JsonValue::iterator it(list.begin()); it != list.end(); ++it) 
		{
			JsonValue a = *it;
			if (a.size() > 1) {
				retval.push_back(a[1]);
			}
			c++;
		}
	}
	catch(...)
	{
	}
	return c;
}

std::string jsDump(
    const JsonValue &value
) {
	return value.dump(4);
}

JsonValue jsClientOptions(
	const std::string &name,
	int verbosity
) {
	JsonValue r = {
		{ "name", name },
		{ "verbosity", verbosity }
	};
	return r;
}

bool jsGetString(
	const JsonValue &value,
	const std::string &name,
	std::string &retVal
) {
	JsonValue::const_iterator f = value.find(name);
	if (f != value.end()) {
		retVal = f.value();
		return true;
	}
	return false;
}

bool jsGetInt(
	const JsonValue &value,
	const std::string &name,
	int &retVal
)
{
	JsonValue::const_iterator f = value.find(name);
	if (f != value.end()) {
		retVal = f.value();
		return true;
	}
	return false;
}

bool jsGetUint64(
	const JsonValue &value,
	const std::string &name,
	uint64_t &retVal
) {
	JsonValue::const_iterator f = value.find(name);
	if (f != value.end()) {
		retVal = f.value();
		return true;
	}
	return false;
}

size_t jsArrayCount(
	const JsonValue &value
) {
	if (!value.is_array()) {
		return 0;
	}
	return value.size();
}

const JsonValue &jsArrayGet(
	const JsonValue &value,
	size_t index
) {
	return value[index];
}

JsonValue jsAndroidCredentials(
	const std:: string &appId,
	uint64_t androidId,
	uint64_t securityToken, 
	const std:: string &GCMToken
) {
	JsonValue r = {
		{ "appId", appId },
		{ "androidId", androidId },
		{ "securityToken", securityToken },
		{ "GCMToken", GCMToken }
	};
	return r;
}

JsonValue jsWpnKeys(
	uint64_t id,
	uint64_t secret,	
	const std::string &privateKey,
	const std::string &publicKey,
	const std::string &authSecret
) {
	JsonValue r = {
		{ "id", id },
		{ "secret", secret },
		{ "privateKey", privateKey },
		{ "publicKey", publicKey },
		{ "authSecret", authSecret }
	};
	return r;
}

JsonValue jsSubscription(
	int subscribeMode, 
	const std::string &name,
	const std::string &token,
	const std::string &persistentId,
	const std::string &subscribeUrl,
	const std::string &endpoint,
	const std::string &authorizedEntity,
	const std::string &pushSet,
	const std::string &serverKey,
	const std::string &sentToken,
	uint64_t id,
	const std::string &publicKey,
	const std::string &authSecret
)
{ 
	switch (subscribeMode) {
		case SUBSCRIBE_FORCE_FIREBASE:
		{
			JsonValue r = {
				{ "subscribeMode", subscribeMode },
				{ "name", name },
				{ "token", token },
				{ "persistentId", persistentId },
				{ "subscribeUrl", subscribeUrl },
				{ "endpoint", endpoint },
				{ "authorizedEntity", authorizedEntity },
				{ "pushSet", pushSet },
				{ "serverKey", serverKey }
			};
			return r;
		}
		case SUBSCRIBE_FORCE_VAPID:
		{
			JsonValue r = {
				{ "subscribeMode", subscribeMode },
				{ "name", name },
				{ "token", token },
				{ "persistentId", persistentId },
				{ "sentToken", sentToken },
				{ "id", id },
				{ "publicKey", publicKey },
				{ "authSecret", authSecret }
			};
			return r;
		}
	}
	JsonValue r = {};
	return r;
}

#endif
