/*
 * Web push notification text formatting routines.
 */

#include <sstream>
#include "utiljson.h"
#include "notify2string.h"

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
    return notify2json(id, name, persistent_id, from, appName, appId, sent, notify);
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
