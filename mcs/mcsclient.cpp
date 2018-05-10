/*
 * Web push notification command line client
 * Copyright (C) 2018  Andrei Ivanov andrei.i.ivanov@commandus.com * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#include <functional>
#include <stdint.h>
#include <fstream>
#include <sstream>
#include <curl/curl.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>

#include <ece.h>

#include "platform.h"
#include "nlohmann/json.hpp"

#include "mcsclient.h"
#include "utilstring.h"
#include "sslfactory.h"
#include "android_checkin.pb.h"
#include "checkin.pb.h"
#include "mcs.pb.h"
#include "commandoutput.h"
#include "wp-push.h"

#ifdef _MSC_VER
#include <unistd.h>
#else
#endif

using json = nlohmann::json;

// MCS Message tags.
enum MCSProtoTag 
{
	kHeartbeatPingTag       = 0,
	kHeartbeatAckTag        = 1,
	kLoginRequestTag        = 2,
	kLoginResponseTag       = 3,
	kCloseTag               = 4,
	kMessageStanzaTag       = 5,
	kPresenceStanzaTag      = 6,
	kIqStanzaTag            = 7,
	kDataMessageStanzaTag   = 8,
	kBatchPresenceStanzaTag = 9,
	kStreamErrorStanzaTag   = 10,
	kHttpRequestTag         = 11,
	kHttpResponseTag        = 12,
	kBindAccountRequestTag  = 13,
	kBindAccountResponseTag = 14,
	kTalkMetadataTag        = 15,
	kNumProtoTypes          = 16
};

using namespace checkin_proto;
using namespace mcs_proto;
using namespace google::protobuf::io;

#define MCS_PORT		443

static const std::string REGISTER_URL("https://android.clients.google.com/c2dm/register3");
static const std::string CHECKIN_URL("https://android.clients.google.com/checkin");
static const std::string MCS_HOST("mtalk.google.com");
static const std::string DEF_CHROME_VER("63.0.3234.0");

static const u_int8_t kMCSVersion = 41;

static const std::string HDR_CONTENT_TYPE("Content-Type: ");
static const std::string HDR_AUTHORIZATION("Authorization : ");


static const char *IQTYPE_NAMES[] = 
{
	"GET", "SET", "RESULT", "IQ_ERROR"
};

//----------------------------- MCSReceiveBuffer -----------------------------

MCSReceiveBuffer::MCSReceiveBuffer()
	: mClient(NULL), mVersion(0), state(STATE_VERSION), buffer("")
{
}

void MCSReceiveBuffer::setClient(MCSClient *client)
{
	mClient = client;
}

int MCSReceiveBuffer::process()
{
	int sz = buffer.size();
	int count = 0;
	while (sz > 0)
	{
		if (state == STATE_VERSION)
		{
			mVersion = (uint8_t) buffer[0];
			mClient->log(3) << "MCS version: " << (int) mVersion << std::endl;
			buffer.erase(0, 1);
			sz--;
			state = STATE_TAG;
		} else {
			sz = parse();
			if (sz <= 0)
				break;
			count++;
		}
	}
	return count;
}

MessageLite *MCSReceiveBuffer::createMessage(uint8_t tag)
{
	MessageLite *r = NULL;
	switch(tag)
	{
	case kHeartbeatPingTag:
		r = new HeartbeatPing();
		break;
	case kHeartbeatAckTag:
		r = new HeartbeatAck();
		break;
	case kLoginRequestTag:
		r = new LoginRequest();
		break;
	case kLoginResponseTag:
		r = new LoginResponse();
		break;
	case kCloseTag:
		r = new Close();
		break;
	case kMessageStanzaTag:
		break;
	case kPresenceStanzaTag:
		break;
	case kIqStanzaTag:
		r = new IqStanza();
		break;
	case kDataMessageStanzaTag:
		r = new DataMessageStanza();
		break;
	case kBatchPresenceStanzaTag:
		break;
	case kStreamErrorStanzaTag:
			break;
	case kHttpRequestTag:
			break;
	case kHttpResponseTag:
			break;
	case kBindAccountRequestTag:
			break;
	case kBindAccountResponseTag:
			break;
	case kTalkMetadataTag:
			break;
	case kNumProtoTypes:
			break;
	default:
		break;
	}
	return r;
}

int MCSReceiveBuffer::parse()
{
	std::stringstream ss(buffer);
	IstreamInputStream rawInput(&ss);
	CodedInputStream codedInput(&rawInput);
	int c = 0;
	int sz = 0;
	while (true)
	{
		// tag number, size, message
		uint8_t tag;
		bool r = codedInput.ReadRaw(&tag, 1);	// 1 byte long
		if (!r)
			break;
		uint32_t msgSize;
		r = codedInput.ReadVarint32(&msgSize);
		if (!r)
			break;

		mClient->log(3) << "<<<  Tag " << (int) tag << " size " << msgSize << "  >>>" << std::endl;

		google::protobuf::io::CodedInputStream::Limit limit = codedInput.PushLimit(msgSize);
		MessageLite *message = createMessage(tag);
		if (message)
		{
			r = message->ParsePartialFromCodedStream(&codedInput);
			if (!r)
				break;
			r = codedInput.ConsumedEntireMessage();
			std::string d;
			message->SerializeToString(&d);
			mClient->log(3) << "Tag: " << (int) tag << " size: " << msgSize << ": " << hexString(d) << std::endl;
			switch (tag)
			{
				case kLoginResponseTag:
				{
					LoginResponse* r = (LoginResponse*) message;
					mClient->log(3) << "Login " << r->id() << " ";
					if (r->has_error())
					{
						mClient->log(3) << "error " << r->error().code();
						if (r->error().has_message())
						{
							mClient->log(3) << ": " << r->error().message() << " ";
						}
					}
					for (int i = 0; i < r->setting_size(); i++)
					{
						mClient->log(3) << "setting " << i << ":" << r->setting(i).name() << r->setting(i).value() << std::endl;
					}
					if (r->has_server_timestamp())
					{
						time_t t = r->server_timestamp()/1000;
						struct tm *tm = localtime(&t);
						mClient->log(3) << "server time " <<  std::asctime(tm) << std::endl;
					}
					
					if (r->has_heartbeat_config())
					{
						mClient->log(3) << " has heart beat config" << std::endl;
					}
				}
					break;
				case kHeartbeatAckTag:
				{
					HeartbeatAck* r = (HeartbeatAck*) message;
					mClient->log(3) << " HeartbeatAck " << std::endl;
					if (r->has_last_stream_id_received())
					{
						mClient->log(3) << " last_stream_id_received: " << r->last_stream_id_received() << " ";
					}
					if (r->has_status())
					{
						mClient->log(3) << " status: " << r->status() << " ";
					}
					if (r->has_stream_id())
					{
						mClient->log(3) << " stream_id: " << r->stream_id() << " ";
					}
				}
					break;
				case kBindAccountResponseTag:
					{
					}
					break;
				case kIqStanzaTag:
				{
					IqStanza* r = (IqStanza*) message;
					mClient->log(3) << "IqStanza " << IQTYPE_NAMES[r->type()] << " " << r->id() << " ";
					if (r->has_rmq_id())
						mClient->log(3) << " rmq_id: " << r->rmq_id();
					if (r->has_from())
						mClient->log(3) << " from: " << r->from();
					if (r->has_to())
						mClient->log(3) << " to: " << r->to();
					if (r->has_error())
					{
						mClient->log(3) << " error: ";
						if (r->error().has_code())
							mClient->log(3) << " code: " << r->error().code();
						if (r->error().has_extension())
						{
							mClient->log(3) << " extension: ";
							if (r->error().extension().has_id())
								mClient->log(3) << " id: " << r->error().extension().id();
							if (r->error().extension().has_data())
								mClient->log(3) << " data: " << r->error().extension().data();
						}
					}
					if (r->has_extension())
					{
						mClient->log(3) << " extension: ";
						if (r->error().extension().has_id())
							mClient->log(3) << " id: " << r->error().extension().id();
						if (r->error().extension().has_data())
							mClient->log(3) << " data " << r->error().extension().data();
					}
					if (r->has_persistent_id())
						mClient->log(3) << " persistent_id: " << r->persistent_id();
					if (r->has_stream_id())
						mClient->log(3) << " stream_id: " << r->stream_id();
					if (r->has_last_stream_id_received())
						mClient->log(3) << " last_stream_id_received: " << r->last_stream_id_received();
					if (r->has_account_id())
						mClient->log(3) << " account_id: " << r->account_id();
  					if (r->has_status())
						mClient->log(3) << " status: " << r->status();
					mClient->log(3) << std::endl;
				}
					break;
				case kDataMessageStanzaTag:
				{
					DataMessageStanza* r = (DataMessageStanza*) message;
					std::string cryptoKeyHeader;
					std::string encryptionHeader;
					std::string persistent_id;
					std::string from;
					std::string subtype;
					int64_t sent;
					
					mClient->log(3) << "DataMessageStanza" << std::endl;
					for (int a = 0; a < r->app_data_size(); a++)
					{
						mClient->log(3) << " app_data key: " << r->app_data(a).key() 
						<< " data: " << r->app_data(a).value() << std::endl;
						if (r->app_data(a).key() == "crypto-key")
							cryptoKeyHeader = r->app_data(a).value();
						if (r->app_data(a).key() == "encryption")
							encryptionHeader = r->app_data(a).value();
						if (r->app_data(a).key() == "subtype")
							subtype = r->app_data(a).value();
					}
					if (r->has_id())
						mClient->log(3) << " id: " << r->id();
					if (r->has_category())
						mClient->log(3) << " category: " << r->category();
					if (r->has_device_user_id())
						mClient->log(3) << " device_user_id: " << r->device_user_id();
					if (r->has_from())
					{
						from = r->from();
						mClient->log(3) << " from: " << r->from();
					}
					if (r->has_persistent_id())
					{
						persistent_id = r->persistent_id();
						mClient->log(3) << " persistent_id: " << r->persistent_id();
					}
					if (r->has_from_trusted_server())
						mClient->log(3) << " from_trusted_server: " << r->from_trusted_server();
					if (r->has_immediate_ack())
						mClient->log(3) << " immediate_ack: " << r->immediate_ack();
					if (r->has_last_stream_id_received())
						mClient->log(3) << " last_stream_id_received: " << r->last_stream_id_received();
					if (r->has_queued())
						mClient->log(3) << " queued: " << r->queued();
					if (r->has_raw_data())
					{
						mClient->log(3) << " raw_data: " << hexString(r->raw_data());
						if (mClient)
						{
							std::string d;
							int dr = mClient->decode(d, r->raw_data(), cryptoKeyHeader, encryptionHeader);
							if (dr == 0)
							{
								mClient->log(3) << " data size " << d.size() << " :" << std::endl << d << std::endl;
								//
								std::string appName;
								std::string appId;
							    size_t start_pos = subtype.find("#");
								if (start_pos == std::string::npos) 
								{
									appName = subtype;
									appId = "";
								}
								else
								{
									appName = subtype.substr(0, start_pos);
									appId = subtype.substr(start_pos + 1);
								}
								NotifyMessage notification;
								if (mClient->parseJSONNotifyMessage(notification, d))
								{
									mClient->getConfig()->setPersistentId(notification.authorizedEntity, persistent_id);
									mClient->notifyAll(persistent_id, from, appName, appId, sent, notification);
								} 
								else
								{
									// TODO
									if (notification.data.empty())
									{
										mClient->log(3) << " no data field in the message";
									}
									else
									{
										std::string command;
										if (mClient->parseJSONCommandOutput(command, notification.data))
										{
											mClient->log(3) << " Command: " << command;
											CommandOutput co;
											std::stringstream ss;
											int r = co.exec(&ss, command);
											if (r)
											{
												mClient->log(3) << " Error " << r << " execute command: " << command;
											}
											else
											{
												std::string r = ss.str();
												if (r.size())
												{
													std::string output;
													int rp = push2ClientData(&output, mClient->getConfig()->serverKey, from, persistent_id, command, 0, r);
													if (rp)
													{
														mClient->log(3) << "Error send reply to command " << command << " rp";
													}
													else
													{
														mClient->log(3) << "Send reply to command " << command << " successfull";
													}
												}
												else
												{
													mClient->log(3) << "Command " << command << " return nothing";
												}
											}
										}
										else
										{
											mClient->log(3) << " can not parse data field in the message " << notification.data;
										}
									}
								}
							}
							else
							{
								mClient->log(3) << " error decode data " << dr;
							}
						}
					}
					if (r->has_reg_id())
						mClient->log(3) << " reg_id: " << r->reg_id();
					if (r->has_sent())
					{
						sent = r->sent();
						mClient->log(3) << " sent: " << r->sent();
					}
					if (r->has_status())
						mClient->log(3) << " status: " << r->status();
					if (r->has_stream_id())
						mClient->log(3) << " stream_id: " << r->stream_id();
					if (r->has_to())
						mClient->log(3) << " to: " << r->to();
					if (r->has_token())
						mClient->log(3) << " token: " << r->token();
					if (r->has_ttl())
						mClient->log(3) << " ttl: " << r->ttl();
					mClient->log(3) << std::endl;
				}
					break;
				default:
					break;
			}
		}
		else
		{
			r = codedInput.Skip(msgSize);
		}
		sz = codedInput.CurrentPosition();
		codedInput.ConsumedEntireMessage();
		codedInput.PopLimit(limit);
	}
	if (sz)
	{
		buffer.erase(0, sz);
	}
	return c;
}

void MCSReceiveBuffer::put(const void *buf, int size)
{
	buffer.append(std::string((char*) buf, size));
}

uint8_t MCSReceiveBuffer::getVersion()
{
	return mVersion;
}

//----------------------------- MCSClient helpers -----------------------------

/**
 * @param androidId 0- before register
 * @param securityToken 0- before register
 */
static std::string mkCheckinRequest(
	uint64_t androidId, 
	uint64_t securityToken
)
{
	AndroidCheckinRequest req;
	req.set_user_serial_number(0);
	req.set_version(3);
	if (androidId)
		req.set_id(androidId);
	if (securityToken)
		req.set_security_token(securityToken);
	AndroidCheckinProto* checkIn = req.mutable_checkin();
	ChromeBuildProto *chrome = checkIn->mutable_chrome_build();
	checkIn->set_type(DEVICE_CHROME_BROWSER);
	chrome->set_platform(ChromeBuildProto_Platform_PLATFORM_LINUX);
	*chrome->mutable_chrome_version() = DEF_CHROME_VER;
	chrome->set_channel(ChromeBuildProto_Channel_CHANNEL_DEV);
	
	std::string r;
	req.SerializeToString(&r);
	return r;
}

static MessageLite * mkPing()
{
	HeartbeatPing *req = new HeartbeatPing();
	// req->set_status(0);
	return req;
}

/**
 * @param androidId 0- before register
 * @param securityToken 0- before register
 */
static MessageLite *mkLoginRequest
(
	uint64_t androidId,
	uint64_t securityToken,
	const std::vector<std::string> &persistentIds
)
{
	LoginRequest *req = new LoginRequest();
	req->set_adaptive_heartbeat(false);
	req->set_auth_service(LoginRequest_AuthService_ANDROID_ID);
	std::stringstream st;
	st << securityToken;
	req->set_auth_token(st.str());

	req->set_id("chrome-" + DEF_CHROME_VER);
	req->set_domain("mcs.android.com");

	std::stringstream sh;
	sh << std::hex << androidId;
	std::string haid = sh.str();
	std::stringstream ss;
	ss << androidId;
	std::string said = ss.str();

	req->set_device_id("android-" + haid);
	req->set_network_type(1);
	req->set_resource(said);
	req->set_user(said);
	req->set_use_rmq2(true);
	auto sc = req->mutable_setting();
	Setting *s = sc->Add();
	*s->mutable_name() = "new_vc";
	*s->mutable_value() = "1";
	for (std::vector<std::string>::const_iterator it(persistentIds.begin()); it != persistentIds.end(); ++it)
	{
		std::cerr << "===========pessis " << *it << std::endl;
		*req->add_received_persistent_id() = *it;
	}
	return req;
}

/**
  * @brief CURL write callback
  */
static size_t write_string(void *contents, size_t size, size_t nmemb, void *userp)
{
	if (userp)
		((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

static size_t write_header(char* buffer, size_t size, size_t nitems, void *userp) {
	size_t sz = size * nitems;
	((std::string*)userp)->append((char*)buffer, sz);
	return sz;
}

static const char *CURL_TYP_NAMES[7] = {
	"INFO",
	"HEADER IN",
	"HEADER OUT",
	"DATA IN",
	"DATA OUT",
	"SSL DATA IN",
	"SSL DATA OUT"
};

static int curl_trace
(
	CURL *handle, 
	curl_infotype typ,
	char *data, 
	size_t size,
	void *userp
)
{
	if (typ >= 0 && typ < 7)
		std::cerr << CURL_TYP_NAMES[typ] << ": " << std::endl;
	std::string s(data, size);
	if (typ > 2)
		s = hexString(s);
	std::cerr << s << std::endl;
	return 0;
}

//----------------------------- MCSClient -----------------------------

/**
  * POST data, return received data in retval
  * @return 200-299 success, otherwise error code. retval contains error description
  */
int MCSClient::curlPost
(
	const std::string &url,
	const std::string &contentType,
	const std::string &content,
	std::string *retval
)
{
	CURL *curl = curl_easy_init();
	if (!curl)
		return CURLE_FAILED_INIT; 
	CURLcode res;
	
	
	struct curl_slist *chunk = NULL;
	chunk = curl_slist_append(chunk, (HDR_CONTENT_TYPE + contentType).c_str());
	std::stringstream ss;
	ss << HDR_AUTHORIZATION << "AidLogin " 
		<< mConfig->androidCredentials->getAndroidId() << ":" 
		<< mConfig->androidCredentials->getSecurityToken();
	if (hasIdNToken())
		curl_slist_append(chunk, ss.str().c_str());
	
	curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, content.size());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, content.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_string);
	if (retval)
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, retval);

	std::string debugHeaders;
	
	if (mConfig->verbosity > 3)
	{
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, curl_trace);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, &debugHeaders);
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &write_header);
	}

    res = curl_easy_perform(curl);

	if (mConfig->verbosity > 2) {
		log(3) << debugHeaders << std::endl;
	}

	int http_code;

    if (res != CURLE_OK)
	{
		if (retval)
			*retval = curl_easy_strerror(res);
		http_code = - res;
	}
	else
	{
		curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
	}
	curl_easy_cleanup(curl);
	return http_code;
}

void MCSClient::init()
{
	mStream = &mBuffer; 
	mStream->setClient(this);
}

MCSClient::MCSClient()
	: mConfig(NULL)
{
	init();
}

MCSClient::MCSClient(
	WpnConfig *config
)
	: mConfig(config)
{
	init();
}

WpnConfig * MCSClient::getConfig()
{
	return mConfig;
}

void MCSClient::setConfig
(
	WpnConfig *config
)
{
	mConfig = config;
}

MCSClient::MCSClient
(
	const MCSClient& other
)
{
	init();
	mConfig = other.mConfig;
}

MCSClient::~MCSClient()
{
	stop();
}

int readLoop(MCSClient *client)
{
	unsigned char buffer[4096];
	client->log(3) << "Listen loop started" << std::endl;
	int r = client->logIn();
	while (!client->mStop)
	{
		int r = SSL_read(client->mSsl, buffer, sizeof(buffer));
		if (r > 0) 
		{
			// mClient->log(3) << "Received " << r << " bytes: " << std::endl << hexString(std::string((char *) buffer, r)) << std::endl;
			client->mStream->put(buffer, r);
			if (size_t c = client->mStream->process())
			{
				client->log(3) << "Processed " << c << " messages" << std::endl;
			}
		}
		sleep(0);
	}
	client->log(3) << "Listen loop stopped" << std::endl;
	return 0;
}

int MCSClient::connect()
{
	if (!mConfig)
		return ERR_NO_CONFIG;
	if (!mConfig->wpnKeys)
		return ERR_NO_KEYS;
	if (!mConfig->androidCredentials)
		return ERR_NO_CREDS;
	int r;
	if (mConfig->androidCredentials->getAndroidId() == 0)
	{
		r = checkIn();
		if (r < 200 || r >= 300)
		{
			return ERR_NO_ANDROID_ID_N_TOKEN;
		}
	}
	if (mConfig->androidCredentials->getGCMToken().empty())
	{
		for (int i = 0; i < 5; i++) 
		{
			r = registerDevice();
			if (r >= 200 && r < 300)
			{
				break;
			}
			sleep(1);
		}
		if (r < 200 || r >= 300)
		{
			return ERR_NO_FCM_TOKEN;
		}
	}
	
	mSsl = mSSLFactory.open(&mSocket, MCS_HOST, MCS_PORT);
	if (!mSsl)
		return ERR_NO_CONNECT;
	mStop = false;
	
	if (mConfig->verbosity > 1)
		log(3) << "Android id: " << mConfig->androidCredentials->getAndroidId() 
			<< " security token: " << mConfig->androidCredentials->getSecurityToken() << std::endl;
	std::thread listenerThread(readLoop, this);
	listenerThread.detach();
	return 0;
}

void MCSClient::stop()
{
	mStop = true;
}

bool MCSClient::hasIdNToken()
{
	if (!mConfig->androidCredentials)
		return false;
	return (mConfig->androidCredentials->getAndroidId() && mConfig->androidCredentials->getSecurityToken());
}

int MCSClient::write()
{
	if (!mConfig)
		return 0;
	return mConfig->write();
}

int MCSClient::checkIn()
{
	if (!mConfig->androidCredentials)
		return ERR_NO_CREDS;
	uint64_t androidId = mConfig->androidCredentials->getAndroidId();
	uint64_t securityToken = mConfig->androidCredentials->getSecurityToken();
	std::string protobuf = mkCheckinRequest(androidId, securityToken);
	if (mConfig->verbosity > 2)
	{
		log(3) << "Send to " << CHECKIN_URL 
			<< " checkin: " << std::endl
			<< hexString(protobuf) << std::endl;
	}

	std::string retval;
	int r = curlPost(CHECKIN_URL, "application/x-protobuffer", protobuf, &retval);
	if (mConfig->verbosity > 1)
	{
		log(3) << "Check in " << CHECKIN_URL 
			<< " android id: " << mConfig->androidCredentials->getAndroidId()
			<< " app id: " << mConfig->androidCredentials->getAppId()
			<< " security token: " << mConfig->androidCredentials->getSecurityToken()
			<< " security token: " << mConfig->androidCredentials->getSecurityToken() 
			<< " return code: " << r 
			<< " value: " << std::endl << std::endl << retval << std::endl;
	}
	if (r < 200 || r >= 300)
		return r;
	
	AndroidCheckinResponse resp;
	bool cr = resp.ParseFromString(retval);
	// Whether statistics were recorded properly.
	if (!cr)
		return -r;
	
	if (mConfig->androidCredentials)
	{
		uint64_t androidId = resp.android_id();
		uint64_t securityToken = resp.security_token();
		mConfig->androidCredentials->setAndroidId(androidId);
		mConfig->androidCredentials->setSecurityToken(securityToken);
		if (androidId && securityToken)
		{
			if (mConfig->verbosity > 2)
			{
				log(3) << "Credentials assigned, android id:  " << androidId 
					<< ", security token: " << securityToken << std::endl;
			}
		}
	}
	return r;
}

std::string MCSClient::getAppId()
{
	std::stringstream r;
	r << "wp:com.commandus.wpn#" << mConfig->androidCredentials->getAppId();
	return r.str();
}

const uint8_t REGISTER_SERVER_KEY[] = 
{
	0x04,
	0x33,
	0x94,
	0xf7,
	0xdf,
	0xa1,
	0xeb,
	0xb1,
	0xdc,
	0x03,
	0xa2,
	0x5e,
	0x15,
	0x71,
	0xdb,
	0x48,
	0xd3,
	0x2e,
	0xed,
	0xed,
	0xb2,
	0x34,
	0xdb,
	0xb7,
	0x47,
	0x3a,
	0x0c,
	0x8f,
	0xc4,
	0xcc,
	0xe1,
	0x6f,
	0x3c,
	0x8c,
	0x84,
	0xdf,
	0xab,
	0xb6,
	0x66,
	0x3e,
	0xf2,
	0x0c,
	0xd4,
	0x8b,
	0xfe,
	0xe3,
	0xf9,
	0x76,
	0x2f,
	0x14,
	0x1c,
	0x63,
	0x08,
	0x6a,
	0x6f,
	0x2d,
	0xb1,
	0x1a,
	0x95,
	0xb0,
	0xce,
	0x37,
	0xc0,
	0x9c,
	0x6e
};

static std::string base64encode
(
	const void *source,
	size_t size
)
{
	size_t requiredSize =  ece_base64url_encode(source, size, ECE_BASE64URL_OMIT_PADDING, NULL, 0);
	std::string r(requiredSize, '\0');
	ece_base64url_encode(source, size, ECE_BASE64URL_OMIT_PADDING, (char *) r.c_str(), r.size());
	return r;
}

/**
 * obtain GCM token
 */
int MCSClient::registerDevice()
{
	std::string retval;
	std::stringstream formData;
	std::string rkb64 = base64encode(REGISTER_SERVER_KEY, sizeof(REGISTER_SERVER_KEY));
	formData << "app=org.chromium.linux" 
		<< "&X-subtype=" << escapeURLString(getAppId()) 
		<< "&device=" << mConfig->androidCredentials->getAndroidId()
		<< "&sender=" << rkb64;

	int r = curlPost(REGISTER_URL, "application/x-www-form-urlencoded", formData.str(), &retval);

	// retval: token=xxx

	if (mConfig->verbosity > 2)
	{
		log(3) << "Register " << REGISTER_URL 
			<< " return code: " << r 
			<< " value: " << retval << std::endl;
	}
	if (r < 200 || r >= 300)
		return r;

	std::size_t p = retval.find("=", 0);
	if (p == std::string::npos)
		return ERR_REGISTER_VAL;
	std::string k = retval.substr(0, p);
	std::string v = retval.substr(p + 1);
	if (k == "token") 
	{
		mConfig->androidCredentials->setGCMToken(v);
		if (mConfig->verbosity > 1)
			log(3) << "GCM token: " << v << std::endl;
	}
	else
	{
		if (mConfig->verbosity > 0)
		{
			log(3) << k << ": " << v << std::endl;
		}
		r = ERR_REGISTER_FAIL;
	}
	return r;
}

std::ostream &MCSClient::log
(
	int level
)
{
	if (mConfig && (mConfig->verbosity >= level))
	{
		return std::cerr;
	}
	else
	{
		return onullstrm;
	}
}

int MCSClient::logIn()
{
	if (!mConfig->androidCredentials)
		return ERR_NO_CREDS;
	if (!mSsl)
		return ERR_NO_CONNECT;
	uint64_t androidId = mConfig->androidCredentials->getAndroidId();
	uint64_t securityToken = mConfig->androidCredentials->getSecurityToken();
	if (mConfig->verbosity > 2)
	{
		log(3) << "Login to " << MCS_HOST << std::endl;
	}

	std::vector<std::string> persistentIds;
	mConfig->getPersistentIds(persistentIds);
	MessageLite *l =  mkLoginRequest(androidId, securityToken, persistentIds);
	if (!l)
		return ERR_MEM;

	sendVersion();

	sendTag(kLoginRequestTag, l);

	delete l;
	int r = 0;
	return r;
}

int MCSClient::sendVersion()
{
	std::stringstream ss;
	ss << kMCSVersion;
	std::string r = ss.str();
	log(3) << "Send version: " << hexString(r) << std::endl;
	return SSL_write(mSsl, r.c_str(), r.size());
}

int MCSClient::sendTag
(
	uint8_t tag,
	const MessageLite *msg
)
{
	std::stringstream ss;
	OstreamOutputStream *rawOutput = new OstreamOutputStream(&ss);
	CodedOutputStream *codedOutput = new CodedOutputStream(rawOutput);
	codedOutput->WriteRaw(&tag, 1);
	int sz = msg->ByteSize();
	codedOutput->WriteVarint32(sz);
	msg->SerializeToCodedStream(codedOutput);
	delete codedOutput;
	delete rawOutput;
	std::string s(ss.str());
	log(3) << "Send tag: " << (int) tag << " (" << sz << " bytes) : " << hexString(s) << std::endl;
	return SSL_write(mSsl, s.c_str(), s.size());
}

void MCSClient::ping()
{
	log(3) << "ping.." << std::endl;
	MessageLite *l =  mkPing();
	if (!l)
		return;
	sendTag(kHeartbeatPingTag, l);
	delete l;
}

void MCSClient::writeStream
(
	std::istream &strm
)
{
	std::string s;
	while (!strm.eof())
	{
		strm >> s;
		if (s == "q")
			break;
		if (s == "p")
		{
			ping();
		}
	}
}

int ece_webpush_aesgcm_headers_extract_params1
(
	const char* cryptoKeyHeader,
	const char* encryptionHeader,
	uint8_t* salt, size_t saltLen,
	uint8_t* rawSenderPubKey,
	size_t rawSenderPubKeyLen,
	uint32_t* rs
);

/**
 * Decode string
 * @see https://tools.ietf.org/html/draft-ietf-webpush-encryption-03
 */
int MCSClient::decode
(
	std::string &retval,
	const std::string &source,
	const std::string &cryptoKeyHeader,
	const std::string &encryptionHeader
)
{
	if (!mConfig->wpnKeys)
		return ERR_NO_KEYS;
	uint32_t rs = 0;
	uint8_t salt[ECE_SALT_LENGTH];
	uint8_t rawSenderPubKey[ECE_WEBPUSH_PUBLIC_KEY_LENGTH];
	int err = ece_webpush_aesgcm_headers_extract_params1(cryptoKeyHeader.c_str(), encryptionHeader.c_str(),
		salt, ECE_SALT_LENGTH, rawSenderPubKey, ECE_WEBPUSH_PUBLIC_KEY_LENGTH, &rs);
	if (err != ECE_OK)
	{
		std::string m;
		switch (err) {
			case ECE_ERROR_INVALID_ENCRYPTION_HEADER:
				m = "Invalid encryption: " + encryptionHeader;
				break;
			case ECE_ERROR_INVALID_CRYPTO_KEY_HEADER:
				m = "Invalid crypto-key: " + cryptoKeyHeader;
				break;
			default:
				m = "";
				break;
		}
		log(3) << "Decode error " << err << ", " << m << std::endl;
		return err;
	}
	size_t outSize = ece_aes128gcm_plaintext_max_length((const uint8_t*) source.c_str(), source.size());
	if (outSize == 0)
	{
		log(3) << "Decode error: zero size" << std::endl;
		return ERR_MEM;
	}
	if (outSize > 0)
	{
		if (outSize < 4096)
			outSize = 4096;
		retval = std::string(outSize, '\0');
		ece_webpush_aesgcm_decrypt(
			mConfig->wpnKeys->getPrivateKeyArray(), ECE_WEBPUSH_PRIVATE_KEY_LENGTH,
			mConfig->wpnKeys->getAuthSecretArray(), ECE_WEBPUSH_AUTH_SECRET_LENGTH,
			(const uint8_t *) &salt, ECE_SALT_LENGTH,
			(const uint8_t *) &rawSenderPubKey, ECE_WEBPUSH_PUBLIC_KEY_LENGTH,
			rs,
			(const uint8_t *) source.c_str(), source.size(), 
			(uint8_t *) retval.c_str(), &outSize);
		retval.resize(outSize);
	}
	return 0;
}

void MCSClient::mkNotifyMessage
(
	NotifyMessage &retval,
	const std::string &authorizedEntity,	///< e.g. 246829423295
	const std::string &title,
	const std::string &body,
	const std::string &icon,
	const std::string &click_action,
	const std::string &data
)
{
	retval.authorizedEntity = authorizedEntity;
	retval.title = title;
	retval.body = body;
	retval.icon = icon;
	retval.link = click_action;
	retval.data = data;
}


/**
 * Parse command
 * @param retval return value. If it is data, return data JSON string in retval.data
 * @param value JSON data to be parsed
 * @return true
 */
bool MCSClient::parseJSONCommandOutput
(
	std::string &retval,
	const std::string &value
)
{
	bool r = true;
	try
	{
		json m = json::parse(value);
		try
		{
			retval = m.at("command");
		}
		catch(...)
		{
			r = false;
		}
	}
	catch(...)
	{
		r = false;
	}
	return r;
}

/**
 * Parse FCM JSON message into notification structure or copy data 
 * @param retval return value. If it is data, return data JSON string in retval.data
 * @param value JSON data to be parsed
 * @return true- notification, false- data
 */
bool MCSClient::parseJSONNotifyMessage
(
	NotifyMessage &retval,
	const std::string &value
)
{
	bool r = true;
	try
	{
		json m = json::parse(value);
		try
		{
			retval.authorizedEntity = m.at("from");
		}
		catch(...)
		{
		}
		try
		{
			json notification  = m.at("notification");
			try
			{
				retval.title = notification.at("title");
			}
			catch(...)
			{
				// it is not notification at all
				r = false;
			}
			try
			{
				retval.body = notification.at("body");
			}
			catch(...)
			{
			}
			try
			{
				retval.icon = notification.at("icon");
			}
			catch(...)
			{
			}
			try
			{
				retval.link = notification.at("click_action");
			}
			catch(...)
			{
			}
		}
		catch(...)
		{
		}

		try
		{
			json d = m.at("data");
			retval.data = d.dump();
		}
		catch(...)
		{
		}
	}
	catch(...)
	{
		return false;
	}
	return r;
}

size_t MCSClient::notifyAll
(
	const std::string &persistent_id,
	const std::string &from,
	const std::string &appName,
	const std::string &appId,
	int64_t sent,
	const NotifyMessage &notification
) const
{
	size_t c = 0;
	for (std::vector <desktopNotifyFunc>::const_iterator it(mConfig->desktopNotifyFuncs.begin()); it != mConfig->desktopNotifyFuncs.end(); ++it)
	{
		NotifyMessage response;
		bool r = (*it)(persistent_id, from, appName, appId, sent, &notification, &response);
		if (r)
			c++;
	}
	return c;
}
