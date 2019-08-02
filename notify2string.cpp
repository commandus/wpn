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
        { 
            JSON_ID, id,
            JSON_NAME, name,
            JSON_PERSISTENT_ID, persistent_id,
            JSON_FROM, from,
            JSON_APPNAME, appName,
            JSON_APPID, appId,
            JSON_SENT, sent,
            JSON_NOTIFICATION, {
                { JSON_NOTIFICATION_TITLE, notify->title},
                { JSON_NOTIFICATION_BODY, notify->body},
                { JSON_NOTIFICATION_ICON, notify->icon},
                { JSON_NOTIFICATION_SOUND, notify->sound},
                { JSON_NOTIFICATION_LINK, notify->link},
                { JSON_NOTIFICATION_CATEGORY, notify->category},
                { JSON_NOTIFICATION_DATA, notify->data},
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
