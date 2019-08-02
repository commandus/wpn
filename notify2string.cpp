/*
 * Web push notification text formatting routines.
 */

#include <sstream>
#include "nlohmann/json.hpp"
#include "notify2string.h"

using json = nlohmann::json;

const char* JSON_ID = "id";
const char* JSON_NAME = "name";
const char* JSON_PERSISTENT_ID = "persistent_id";
const char* JSON_FROM = "from";
const char* JSON_APPNAME = "appName";
const char* JSON_APPID = "appId";
const char* JSON_SENT = "sent";

const char* JSON_NOTIFICATION = "notification";
static const char* JSON_NOTIFICATION_TITLE = "title";
static const char* JSON_NOTIFICATION_BODY = "body";
static const char* JSON_NOTIFICATION_ICON = "icon";
static const char* JSON_NOTIFICATION_SOUND = "sound";
static const char* JSON_NOTIFICATION_LINK = "click_action";
static const char* JSON_NOTIFICATION_CATEGORY = "category";
static const char* JSON_NOTIFICATION_DATA = "data";

static json notify2json(
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
    return r;
}

std::string notify2Json(
    uint64_t id,
	const char *name,
   	const char *persistent_id,
	const char *from,				///< e.g. BDOU99-h67HcA6JeFXHbSNMu7e2yNNu3RzoMj8TM4W88jITfq7ZmPvIM1Iv-4_l2LxQcYwhqby2xGpWwzjfAnG4
	const char *appName,
	const char *appId,
	int64_t sent,
    const NotifyMessageC *notify
)
{
    json r = notify2json(id, name, persistent_id, from, appName, appId, sent, notify);
    return r.dump();
}

std::string notify2Delimiter(
	const char *delimiter,
    uint64_t id,
	const char *name,
   	const char *persistent_id,
	const char *from,				///< e.g. BDOU99-h67HcA6JeFXHbSNMu7e2yNNu3RzoMj8TM4W88jITfq7ZmPvIM1Iv-4_l2LxQcYwhqby2xGpWwzjfAnG4
	const char *appName,
	const char *appId,
	int64_t sent,
    const NotifyMessageC *notify
) {
    std::stringstream r;
    r << id << delimiter
        << name << delimiter
        << persistent_id << delimiter
        << from << delimiter
        << appName << delimiter
        << appId << delimiter
        << sent << delimiter
        << notify->title << delimiter
        << notify->body << delimiter
        << notify->icon << delimiter
        << notify->sound << delimiter
        << notify->link << delimiter
        << notify->category << delimiter
        << notify->data;
    return r.str();
}

static std::string csvEscape
(
    const std::string &separator,
    const std::string &str
)
{
    size_t pQ = str.find("\"", 0);
    size_t pS = str.find(separator, 0);
    bool needQuotes = (pQ != std::string::npos) || (pS != std::string::npos);
    if (!needQuotes) {
        return str;
    }
    std::stringstream r;
    r << "\"";
    for (std::string::const_iterator it(str.begin()); it != str.end(); ++it) {
        if (*it == '\"')
            r << "\\";
        r << *it;
    }
    r << "\"";
    return r.str();
}

std::string notify2Csv(
    uint64_t id,
	const char *name,
   	const char *persistent_id,
	const char *from,				///< e.g. BDOU99-h67HcA6JeFXHbSNMu7e2yNNu3RzoMj8TM4W88jITfq7ZmPvIM1Iv-4_l2LxQcYwhqby2xGpWwzjfAnG4
	const char *appName,
	const char *appId,
	int64_t sent,
    const NotifyMessageC *notify
) {
    std::string delimiter = ",";
    std::stringstream r;
    r << id << delimiter
        << csvEscape(delimiter, name) << delimiter
        << csvEscape(delimiter, persistent_id) << delimiter
        << csvEscape(delimiter, from) << delimiter
        << csvEscape(delimiter, appName) << delimiter
        << csvEscape(delimiter, appId) << delimiter
        << sent << delimiter
        << csvEscape(delimiter, notify->title) << delimiter
        << csvEscape(delimiter, notify->body) << delimiter
        << csvEscape(delimiter, notify->icon) << delimiter
        << csvEscape(delimiter, notify->sound) << delimiter
        << csvEscape(delimiter, notify->link) << delimiter
        << csvEscape(delimiter, notify->category) << delimiter
        << csvEscape(delimiter, notify->data);
    return r.str();
}
