#include <iostream>
#include <sstream>
#include "wp-push.h"

#include <curl/curl.h>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

#define FCM_SEND "https://fcm.googleapis.com/fcm/send"

/**
  * @brief CURL write callback
  */
static size_t write_string(void *contents, size_t size, size_t nmemb, void *userp)
{
	if (userp)
		((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

/**
* Push raw JSON to device
* @return 200-299- success, <0- error
*/
int push2ClientJSON
(
	std::string *retval,
	const std::string &server_key,
	const std::string &client_token,
	const std::string &value
)
{
	if (server_key.empty())
		return ERR_PARAM_SERVER_KEY;
	if (client_token.empty())
		return ERR_PARAM_CLIENT_TOKEN;
	CURL *curl = curl_easy_init();
	if (!curl)
		return CURLE_FAILED_INIT; 
	CURLcode res;
	
	struct curl_slist *chunk = NULL;
	chunk = curl_slist_append(chunk, ("Content-Type: application/json"));
	chunk = curl_slist_append(chunk, ("Authorization: key=" + server_key).c_str());

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

	curl_easy_setopt(curl, CURLOPT_URL, FCM_SEND);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, value.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, value.size());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_string);
	std::string r;
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &r);
// std::cerr << "Send to " << FCM_SEND << ": " << value<< std::endl;		
	res = curl_easy_perform(curl);
	int http_code;

    if (res != CURLE_OK)
	{
		if (retval)
			*retval = curl_easy_strerror(res);
		http_code = - res;
	}
	else
	{
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
		if ((http_code >= 200) && (http_code < 300))
		{
			try 
			{
				json response = json::parse(r);
				if (retval)
					*retval = r;
			}
			catch(...) 
			{
// std::cerr << "Parse error" << std::endl;
				if (client_token.empty())
					return ERR_PARSE_RESPONSE;	
			}
		}
		else
		{
			// Error
// std::cerr << "Error " << http_code << ": " << r << std::endl;				
		}
	}
	curl_easy_cleanup(curl);
	return http_code;
}

/**
* Push notification to device
* @return 200-299- success, <0- error
*/
int push2ClientNotification
(
	std::string *retval,
	const std::string &server_key,
	const std::string &client_token,
	const std::string &title,
	const std::string &body,
	const std::string &icon,
	const std::string &click_action
)
{
	json requestBody = {
		{"to", client_token},
		{"notification", 
			{
				{"title", title},
				{"body", body},
				{"icon", icon},
				{"click_action", click_action}
			}
		}
	};
	return push2ClientJSON(retval, server_key, client_token, requestBody.dump());
}

/**
* Push "command output" to device
* @return 200-299- success, <0- error
*/
int push2ClientData
(
	std::string *retval,
	const std::string &server_key,
	const std::string &client_token,
	const std::string &persistent_id,
	const std::string &command,
	int code,
	const std::string &output
)
{
	json requestBody = {
		{"to", client_token},
		{"data", 
			{
				{"command", command},
				{"persistent_id", persistent_id},
				{"code", code},
				{"output", output}
			}
		}
	};
	std::string data = requestBody.dump();
	return push2ClientJSON(retval, server_key, client_token, requestBody);
}
