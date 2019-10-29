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
static const char* JSON_NOTIFICATION_ACTIONS = "actions";
static const char* JSON_NOTIFICATION_ACTION = "action";

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
	if (d.IsNull()) {
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

	Value v;
	RAPIDJSON_ADD_STRING(d, v, a, to, to)

	RAPIDJSON_ADD_STRING(n, v, a, body, body)
	RAPIDJSON_ADD_STRING(n, v, a, title, subject)
	RAPIDJSON_ADD_STRING(n, v, a, icon, icon)
	RAPIDJSON_ADD_STRING(n, v, a, click_action, link)

	/*
	Value actions(kArrayType);
	Value action;
	action.SetObject();
	RAPIDJSON_ADD_STRING(action, v, a, action, link)
	actions.PushBack(action, a);
	*/

	d.AddMember("notification", n, a);

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

bool jsParseClientCommand
(
	const std::string &value,
    std::string &command,
    std::string &persistent_id, 
    int *code,
    std::string &output,
    std::string &serverKey,
    std::string &token
)
{
	Document d;
	d.Parse(value.c_str());
	if (d.HasParseError()) {
		return false;
	}
    command = "";
    persistent_id = ""; 
    if (code)
		*code = 0;
    output = "";
    serverKey = "";
    token = "";

	if (d.HasMember("command")) {
		command = d["command"].GetString();
	}
	if (d.HasMember("serverKey")) {
		serverKey = d["serverKey"].GetString();
	}
	if (d.HasMember("token")) {
		token = d["token"].GetString();
	}
	if (d.HasMember("persistent_id")) {
		persistent_id = d["persistent_id"].GetString();
	}
	if (d.HasMember("output")) {
		output = d["output"].GetString();
	}
	if (code) {
		if (d.HasMember("code")) {
			*code = d["code"].GetInt();
		}
	}
	if (d.HasMember("command")) {
		command = d["command"].GetString();
	}
	return true;
}

/**
 * Parse notification file
 * @return 0- success, -1: Invalid JSON, -2: Important information missed
 */
int parseNotificationJson
(
	const std::string &value,
	std::string &from,
	std::string &to,
	std::string &title,
	std::string &body,
	std::string &icon, 
	std::string &click_action,
	std::string &data
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

	if (d.HasMember("from")) {
		from = d["from"].GetString();
	}
	if (d.HasMember("to")) {
		to = d["to"].GetString();
	}
	if (!d.HasMember("notification")) {
		return 0;
	}
	Value &n = d["notification"];
	if (n.IsObject()) {
		if (n.HasMember("title")) {
			title = n["title"].GetString();
		}
		if (n.HasMember("body")) {
			body = n["body"].GetString();
		}
		if (n.HasMember("icon")) {
			icon = n["icon"].GetString();
		}
		if (n.HasMember("click_action")) {
			click_action = n["click_action"].GetString();
		}
		if (n.HasMember("data")) {
			data = n["data"].GetString();
		}
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
		return false;
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
	return true;
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
		const Value &e = d["error"].GetObject();
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
			if (list[it].Size() >= 2) {
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
	Document::AllocatorType & a = value.GetAllocator();
	StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    value.Accept(writer);
	return std::string(buffer.GetString());
}

void jsClientOptions(
	JsonDocument &document,
	const std::string &name,
	int verbosity
) {
	Document::AllocatorType& a = document.GetAllocator();
	document.SetObject();
	Value v;
	RAPIDJSON_ADD_STRING(document, v, a, name, name)
	v.SetInt(verbosity);
	document.AddMember("verbosity", v, a);
}

bool jsGetString(
	const JsonValue &value,
	const std::string &name,
	std::string &retVal
)
{
	const char *n = name.c_str();
	if (value.IsObject()) {
		if (value.HasMember(n)) {
			const Value &v = value[n];
			if (v.IsString()) {
				retVal = v.GetString();
				return true;
			}
		}
	}
	return false;
}

bool jsGetInt(
	const JsonValue &value,
	const std::string &name,
	int &retVal
)
{
	const char *n = name.c_str();
	if (value.IsObject()) {
		if (value.HasMember(n)) {
			const Value &v = value[n];
			if (v.IsInt()) {
				retVal = v.GetInt();
				return true;
			}
		}
	}
	return false;
}

bool jsGetUint64(
	const JsonValue &value,
	const std::string &name,
	uint64_t &retVal
)
{
	const char *n = name.c_str();
	if (value.IsObject()) {
		if (value.HasMember(n)) {
			const Value &v = value[n];
			if (v.IsUint64()) {
				retVal = v.GetUint64();
				return true;
			}
		}
	}
	return false;
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

void jsAndroidCredentials(
	JsonDocument &document,
	const std:: string &appId,
	uint64_t androidId,
	uint64_t securityToken, 
	const std:: string &GCMToken
) {
	Document::AllocatorType &a = document.GetAllocator();
	document.SetObject();
	Value v;
	RAPIDJSON_ADD_STRING(document, v, a, appId, appId)
	v.SetUint64(androidId);
	document.AddMember("androidId", v, a);
	v.SetUint64(securityToken);
	document.AddMember("securityToken", v, a);
	RAPIDJSON_ADD_STRING(document, v, a, GCMToken, GCMToken)
}

void jsWpnKeys(
	JsonDocument &document,
	uint64_t id,
	uint64_t secret,	
	const std::string &privateKey,
	const std::string &publicKey,
	const std::string &authSecret
) {
	Document::AllocatorType& a = document.GetAllocator();
	document.SetObject();
	Value v;
	v.SetUint64(id);
	document.AddMember("id", v, a);
	v.SetUint64(secret);
	document.AddMember("secret", v, a);
	RAPIDJSON_ADD_STRING(document, v, a, privateKey, privateKey)
	RAPIDJSON_ADD_STRING(document, v, a, publicKey, publicKey)
	RAPIDJSON_ADD_STRING(document, v, a, authSecret, authSecret)
}

void jsSubscription(
	JsonDocument &document,
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
	Document::AllocatorType& a = document.GetAllocator();
	document.SetObject();
	Value v;
	v.SetInt(subscribeMode);
	document.AddMember("subscribeMode", v, a);
	RAPIDJSON_ADD_STRING(document, v, a, name, name)
	RAPIDJSON_ADD_STRING(document, v, a, token, token)
	RAPIDJSON_ADD_STRING(document, v, a, persistentId, persistentId)

	switch (subscribeMode) {
	case SUBSCRIBE_FORCE_FIREBASE:
		RAPIDJSON_ADD_STRING(document, v, a, subscribeUrl, subscribeUrl)
		RAPIDJSON_ADD_STRING(document, v, a, endpoint, endpoint)
		RAPIDJSON_ADD_STRING(document, v, a, authorizedEntity, authorizedEntity)
		RAPIDJSON_ADD_STRING(document, v, a, pushSet, pushSet)
		RAPIDJSON_ADD_STRING(document, v, a, serverKey, serverKey)
		break;
	case SUBSCRIBE_FORCE_VAPID:
		RAPIDJSON_ADD_STRING(document, v, a, sentToken, sentToken)
		v.SetUint64(id);
		document.AddMember("id", v, a);
		RAPIDJSON_ADD_STRING(document, v, a, publicKey, publicKey)
		RAPIDJSON_ADD_STRING(document, v, a, authSecret, authSecret)
		break;
	default:
		break;
	}
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
	if (!subject.empty())
		n[JSON_NOTIFICATION_TITLE] = subject;
	if (!subject.empty())
		n[JSON_NOTIFICATION_ICON] = icon;
	if (!link.empty()) {
		n[JSON_NOTIFICATION_LINK] = link;
	}
  ]
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

bool jsParseClientCommand
(
	const std::string &value,
    std::string &command,
    std::string &persistent_id, 
    int *code,
    std::string &output,
    std::string &serverKey,
    std::string &token
)
{
	bool r = false;
	try
	{
		JsonValue m = JsonValue::parse(value);
		try
		{
			command = m.at("command");
		}
		catch(...)
		{
			r = false;
		}
		try
		{
			serverKey = m.at("serverKey");
		}
		catch(...)
		{
			serverKey = "";
		}
		try
		{
			token = m.at("token");
		}
		catch(...)
		{
			token = "";
		}
		try
		{
			persistent_id = m.at("persistent_id");
		}
		catch(...)
		{
			persistent_id = "";
		}
		try
		{
			output = m.at("output");
		}
		catch(...)
		{
			output = "";
		}
		if (code)
		{
			try
			{
				*code = m.at("code");
			}
			catch(...)
			{
				*code = 0;
			}
		}
	}
	catch(...)
	{
		r = false;
	}
	return r;
}

/**
 * Parse notification file
 * @return 0- success, -1: Invalid JSON, -2: Important information missed
 */
int parseNotificationJson
(
	const std::string &value,
	std::string &from,
	std::string &to,
	std::string &title,
	std::string &body,
	std::string &icon, 
	std::string &click_action,
	std::string &data
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
		try
		{
			if (j.count(JSON_FROM))
				from = j.at(JSON_FROM);
		}
		catch(...)
		{
		}

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
			if (n.count(JSON_NOTIFICATION_DATA))
				data = j.at(JSON_NOTIFICATION_DATA);
			else
				data = "";
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

void jsClientOptions(
	JsonDocument &document,
	const std::string &name,
	int verbosity
) {
	document = {
		{ "name", name },
		{ "verbosity", verbosity }
	};
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

void jsAndroidCredentials(
	JsonDocument &document,
	const std:: string &appId,
	uint64_t androidId,
	uint64_t securityToken, 
	const std:: string &GCMToken
) {
	document = {
		{ "appId", appId },
		{ "androidId", androidId },
		{ "securityToken", securityToken },
		{ "GCMToken", GCMToken }
	};
}

void jsWpnKeys(
	JsonDocument &document,
	uint64_t id,
	uint64_t secret,	
	const std::string &privateKey,
	const std::string &publicKey,
	const std::string &authSecret
) {
	document = {
		{ "id", id },
		{ "secret", secret },
		{ "privateKey", privateKey },
		{ "publicKey", publicKey },
		{ "authSecret", authSecret }
	};
}

void jsSubscription(
	JsonDocument &document,
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
			document = {
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
			break;
		case SUBSCRIBE_FORCE_VAPID:
			document = {
				{ "subscribeMode", subscribeMode },
				{ "name", name },
				{ "token", token },
				{ "persistentId", persistentId },
				{ "sentToken", sentToken },
				{ "id", id },
				{ "publicKey", publicKey },
				{ "authSecret", authSecret }
			};
			break;
		default:
			document = {};
	}
}
#endif
