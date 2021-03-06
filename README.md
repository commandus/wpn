# Web push notification command line interface client

## Overview

This software is intended for sending Web push notification from the command line and passing 
notifications from stdout using pipes ('|') to other programs.

- wpnw write notifications from stdin or command line option
- wpnr read notifications and output received messages to stdout

wpnw and wpnr uses configuration files.

Each configuration file corresponds to one client.

Client has unique public and private keys.

Client can be registered in the "yellow pages" service with a number.

This number can be used instead of keys for simplicity.

- wpn-grant creates configuration file and register client's public key in the registry.

Your client need get access to write messages to specified client.

To do this, wpn-grant give ability to subscribe client to receive messages from other client in 3 steps.

In each step client subscribe to receive messages and creates and provide information how to send message from other client.

For instance, clients 1 and 2 want to subscribe so client 1 and client 2 can read and write messages.

- Step 1. Client 1 subscribes to 2.

- Step 2. Client 2 subscribes to 1.

- Step 3. Client 1 subscribes to 2 again.

Initiator of subscription must repeat subscription one time because client 2 on step 1 does not provide information how to send message yet.

Information how to send message can be used only by other client, nobody other can not use this information to send mesages.
Service delete this information after 1 day, so you need finish subscription in one day.

To avoid passing public key to the registry and use service for exchange keys you can use configuration files itself.
In this case you must somehow distribute configuration files. You don't need register configuration files with service.

To do this there are other tools:

- wpnlinkj subscribe client using configuration files
- wpnlink subscribe client without configuration files (provide keys in command line options)
- wpn read messages using output plugins (other way to send recieved messages to programs)

[Project description, ru](https://docs.google.com/document/d/19pM4g-hvx2MUVV2Ggljw1MMTg9tMCEH7aHYYKpJMaWk/edit?usp=sharing)

## Related projects

Registry service repositories:

[PHP](https://gitlab.com/commandus/wpn-registry-php.git)

[HTML](https://gitlab.com/commandus/wpn-registry-html.git)

## Print device identifiers and security tokens

Check:
```
./wpn --id -vvv
app id      android id  security token	GCM token
2ea0892f-.. 57468817..  2325..   d5DROBGRLmk:A..
```

## Read messages

User wpnr or wpn to read messages.
You need provide plugin with wpn, how to do this please see section "Output plugins".

### Send message

Need:

- from public key, sender.wpnKeys->getPublicKey()
- from private key, sender.wpnKeys->getPrivateKey()
- to endpoint, subscription->getToken() => endpoint()
- to public key, subscription->getWpnKeys().getPublicKey();
- to auth, subscription->getWpnKeys().getAuthSecret();

### GCM token as QR code

Use -q or -Q option to see QR code for subscription.

List of subscription's VAPID tokens with -v option.

First is client's VAPID key, then subscription's VAPID keys
on black screen (white characters, black background:

```
 ./wpn -q
 2	No 2
                                                          
                                                          
                                                          
                                                          
        ██████████████    ██        ██████████████        
        ██          ██      ████    ██          ██        
        ██  ██████  ██  ██████████  ██  ██████  ██        
        ██  ██████  ██  ██████  ██  ██  ██████  ██        
        ██  ██████  ██    ██████    ██  ██████  ██        
        ██          ██    ██  ████  ██          ██        
        ██████████████  ██  ██  ██  ██████████████        
                                                          
              ████  ████        ██        ████            
        ████      ██  ████    ████    ██████  ████        
        ████      ████████      ████████  ██    ██        
          ██  ████      ████████      ████    ██          
        ██  ████    ██  ██          ██████████████        
                        ████      ██        ██████        
        ██████████████  ██████    ████    ████  ██        
        ██          ██    ██████  ████      ██            
        ██  ██████  ██  ████  ██    ██  ██  ████          
        ██  ██████  ██  ████    ██    ████                
        ██  ██████  ██    ██  ████    ██████  ████        
        ██          ██            ██  ██  ██  ████        
        ██████████████      ██████  ██████  ████          
                                                          
                                                          
                                                          
                                                          


``` 

on white screen (black characters, white background:

```
./wpn -Q

reverse image
``` 

## e-mail

In case of mobile device havent builtin camera, or it is impossible to scan QR code in some other reason, you can send email from
the desktop where wpn is installed to the mobile device.

Open this e-mail in the device and click on link to connect mobile phone to the wpn.

You also need install Surephone mobile app in the mobile device.

E-mail contains link to the Google Play where you can install Surephone mobile app.

wpn just produce text for e-mail to be send and do not send e-mail directly therefore you need use pre-installed mail software.

I used mail from mailutils package for instance.

If you havent pre-installed mail software, you can copy produced text and paste in other mail program or web mail in the browser.

Just get link:

```
./wpn -E
```

and somehow send produced link to the mobile device.

### Using SMTP in Ubuntu

It is hard way, use it if you have a lot of mobile clients.

First install mail program. Please refer to Appendix A how to install mailutils and set postfix.

Option -M (--mailto) outputs e-mail body in HTML with link to connect mobile phone to the wpn:
notificationData
```
./wpn -M "name"
```

This link contains information how to establish connection to wpn.

If Surephone mobile application, this link open this application and connect it to wpn.

Then pipe output
```
./wpn -M "Alice" | mail -s "$(echo -e "Click link in your phone\nContent-Type: text/html;charset=utf-8")" bob@acme.com
```

Use custom emale template with --template-file option:
```
./wpn -M "Alice" --template-file email-template.html | mail -s "$(echo -e "Connect device to wpn\nContent-Type: text/html;charset=utf-8")" bob@acme.com
```

In the email-template.html file use placeholders:

- $body Mark action link placement
- $name placeholder for -e parameter (name)
- $subject placeholder for --subject parameter, usully header or other custom information

Please check HTML template file character set is compatible with mail program expected. In example above charset is utf-8.

## Print keys

Check keys:
```
./wpn -y -vvv
private_key	public_key	auth_secret
r1...       BP...       bd..
```

## Subscription list

without -v options returns

- name
- authorizedEntity		
- serverKey	server key to send message
- token		FCM token

with -v, -vv, -vvv, -vvvv options returns

- name
- authorizedEntity
- endpoint
- persistentId
- pushSet
- serverKey
- subscribeMode
- subscribeUrl
- token

```
./wpn -P -vv
subscribeUrl                authorizedEntity	token
https://fcm.googleapis.com	246829423295	    drq...

```

## Subscribe

Set -e (authorized-entity) and -K (FCM server key):

```
./wpn -s -e 246829423295 -K server_key
{
	"token": "c9UC0WcwvyM:APA91bFlAcs7RbWVDPLW42nfL8RN8YYpe0zFnXcT0pMAJihu0WAOqGuoPujHYVJUHC0eRy5DTFepXvlaIyClHEpy6J6itEdT-QzD5SMCLt3HfBH_20LrWIuAXRrGLOWW8g9Y8aF1ikBc",
	"pushSet": "eJpriwkjrcU:APA91bHS4Ohb5In3ssqr3nPWI_EtFbAHEYvxN3SX1Omct5hjy48CeyTCZw5bzxyST1Bhj4m0WynXoq7pmw3IM0JuAQ8poeJe99vFJSeYGKgXtut_2Cmyxwu_V6xrDUqp-k8HDaeN_5fy"
}
```

Optional server key (API key for web application) (-K) is used to send reply. Server key looks like "AI....1I" (~40 characters).

Value of authorized-entity is decimal number identifies subscription if FCM.


### VAPID

["authorized_entity":"103953800507"](https://firebase.google.com/docs/cloud-messaging/js/client)

#### wplink

Subscribe client to recieve messages from another client without config files:

```
./wpnlink -s <subscriptiion VAPID> -i <Android id> -t <sucuritry token> -a <application id>
```

#### wpnr

Read notification using default of provided configuration file with client properties.

Print out received notification message body to stdout.

Using default configuration file ~/.wpn.js:

```
./wpnr
```

Using configuration file 22.js

```
./wpnr -c 22.js
```

To stop, enter q or press Ctrl+C.

##### Options

Use output format -o, -oo, -ooo to print out notification message meta information (in order as is, except JSON format):

- id: client id (if not registered, 0)
- name: client name (if not set, noname)
- persistent_id: identifier of notification message
- from: public key of the client
- appName: application name
- appId: appliocation identifier
- sent: time when message was originated, can be 0 (not provided) seconds from the Unix epoch time
- title: noitification title
- body: notification message itself
- icon: URL of the image
- sound: URL of the notifiction sound
- link: action URL
- category: category
- data: optional text

If not provided, output notification message body only.

-o JSON

-oo tab delimited

-ooo CSV


#### wpnlinkj

Subscribe client to recieve messages from another client using config files:

wpnlinkj <sender JSON file> <receiver JSON file>

In the example below:

```
./wpnlinkj client11.js client12.js 
```

client12 'll receive messages sent by client11

wpnlinkj produces VAPID public key (subscription VAPID key). 

Sender must must provide this key in the "to" field.

You can not send message using reciever's VAPID public key of the receiver, you 'll need subscription's key.

Example how to create two new clients and subscribe one of then:
```
./wpnlinkj client11.js client12.js 
cbcGO31NFjY:APA91bEqATE-_V...
```

#### curl

```
curl -i -H "Accept:application/json" -H "Content-Type:application/json" -X POST https://fcm.googleapis.com/fcm/connect/subscribe -d '{"endpoint": "https://fcm.googleapis.com/fcm/send/eBj1u43iA9Q:APA91bHz7hxB0ImaMZM_4G1K9Lv8MPgIu_Ta0r5Gt9LtI_Gm34B9nkTEOZVZxiZu2YjAIpWo5aZ1SaoBy3n8t_F_lqTsGhwzjdVVhf9kGT6E8KH7CGZBggrKxDxBgj21gb6MlyzTxrbr","encryption_key":"BM97-HP_Pw_RIrkp1mwVaYTEgR21Pl4PD1QYzDGYi5o7mp-YB6Cr9Pbz7_D7l3r5Zb4Ji-pLKubCza_lE4SsZIA","encryption_auth":"0L9jlM_NNYaurD3SSp_ZDg","authorized_entity":"103953800507"}
```

Result:

{
  "token": "daPb4z1IVT8:APA91bFJrYXJUt0aHW7YhKijlPHUkvZJdN4RbzaWqK1zIxAe381joPskAXOq3D1hAxhHRq-UH12g6RbPuVwbdMeJtdNyKa0gZDRVZ7yiU1U8snEkRQb0S0sbj7wKEKiPuNnPJnp6CdLX",
  "pushSet": "e5Z6rtVPcz8:APA91bGIgO2rwT9E21pv-f7PMRqZDunIIkaMEazkYIM8oN7g-gu79jwEW1p0Q1jrmD6wYwXF6qLHDfUZLI4-f1XDPjfKTrokBEChXaclihE3HYApmpH4SKw5VnXKrkO3r9v0_n8IS_k0"
}


## Unsubscribe

Specify subscription endpoint (-e):

```
./wpn -u -e 246829423295
{
	"token": "c9UC0WcwvyM:APA91bFlAcs7RbWVDPLW42nfL8RN8YYpe0zFnXcT0pMAJihu0WAOqGuoPujHYVJUHC0eRy5DTFepXvlaIyClHEpy6J6itEdT-QzD5SMCLt3HfBH_20LrWIuAXRrGLOWW8g9Y8aF1ikBc",
	"pushSet": "eJpriwkjrcU:APA91bHS4Ohb5In3ssqr3nPWI_EtFbAHEYvxN3SX1Omct5hjy48CeyTCZw5bzxyST1Bhj4m0WynXoq7pmw3IM0JuAQ8poeJe99vFJSeYGKgXtut_2Cmyxwu_V6xrDUqp-k8HDaeN_5fy"
}
```

## Unsubscribe all

```
./wpn -u
```

## Client push message

wpn supports two types of web push:

- GCM
- VAPID

GCM way requires registered Firebase project, you need have one private key from the project to push messages to one or more subscribers.

VAPID is not linked to Firebase project, you need have one auto-generated VAPID private key and public key of the recipient. Therefore you can
push one message to one subscriber per time.

Old browsers supports GCM web push, modern browsers supports both of them.

VAPID use two different encodings, old version use AES GCM and new version AES 128 GCM. By default a modern AES 128 GCM encoding is used.

Utilities to subscribe:

- wpn-grant

Subscribe to the user 2 with debug output:

```
./wpn-grant -c 1.js 2 -vvv
```

Utilities to read/write messages:

- wpn read/write
- wpnr read only
- wpnw write only

### Client push message using GCM

Send notification with -m option.

You need provide one or more recipient FCM token(s) up to 100 (limited by system actually) in command line or from JSON file (with -j option).

Also you need provide:

- FCM server key (-K) to authorize send operation

Otherwise, you can provide -n option with name of subscripton (if you also subscribed). In this case subscripton entity and server key are obtained from this subscription:

- subscription name (-n)

Message composed of:

- Subject (-t option) mandatory
- Body (-b option) mandatory
- Icon (-i option) optional
- Action URL and subject (-l, -t options) optional

After push wpn exits immediately.

Sending to one recipient:

```
./wpn -m -K "AIzaSyBfUt1N5aabh8pubYiBPKOq9OcIoHv_41I" -t Subject -b Body -i "https://commandus.com/favicon.ico" -a "https://commandus.com" f22BfuvjjCc:APA91bHc4xzOyN5318sBkspPG9n2zBkP-jHl2EdJVKRHHv0njkplgPVe8s9MVkApTaWHkK9s9137gsPiWnmb_S9IF9h5LX3k8eg9jitNqs0xb7NK9BbPeC-nDw1SuCptZKTuEcKOvKpC
```

In the -K option, it's better to specify an outdated server key, since it's shorter.

Sending by the list of recipient tokens in a file or a web resource (-j option):

```
./wpn -m -K "AIzaSyAyz-oVjsfRNK53XA6o9DVHN9ZNAaHOfxw" -e 246829423295 -t Subject -b Body -i "https://commandus.com/favicon.ico" -a "https://commandus.com" -j "https://ikfia.wpn.commandus.com/app/token?accesskey=2117177"
```
If the -j option is specified, the list is padded, that is, you can specify the recipient's FCM tokens in both the command line and the file (web resource).

If you specify the -j option wpn 'll open the file. If it can not be opened for reading, or if it is empty, wpn tries to download web resource from the network.

Sending using subcription name (-n option) instead of FCM server key (option -K):

```
./wpn -m -n "ubuntu16" -t "Subject kkkk--111" -b "Body 2" -i "https://commandus.com/favicon.ico" -a "https://commandus.com" "d4rd_JZJ940:APA91bEGqtzfqZaohjke2dCqY8z5xJOMmkKud1SU646l2QlwBiZpzb9hTgTefvCrdhQ6-oR82SgjyYpF3kkcgGdMqmhZArMAb4G-D8N_ZYg0BDmgnx92AXKUUqFzCJwqwzTvOmqkjfFy" -vvv
```

Option -u "https://ikfia-wpn.firebaseio.com" is not required.

### Client push message using VAPID

Send notification with -m option requires options to set

- message subject and body, optional icon and action link
- WPN keys to authorize push operation
- Recipient's endpoint URL, [ECDH public key](https://en.wikipedia.org/wiki/Elliptic-curve_Diffie%E2%80%93Hellman) and auth 

Message composed of:

- Subject (-t option), mandatory
- Body (-b option), mandatory
- Icon (-i option), optional
- Action URL and subject(-l, -t options), both optional

Except message itself options can be stored in subscription. 

If you created subscription earlier, you can provide -n option with name of subscripton to set WPN keys and recipient endpoint.
In this case endpoint and WPN keys are obtained from this subscription:

- subscription name (-n)

If not you need set all required options as described below.

wpn auto generate "default" WPN keys and keeps them in configuration file.

By default wpn uses theese auto-generated WPN keys, of course you can override them by following three options:

-  -p, --private-key VAPID private key
-  -k, --public-key VAPID public key

VAPID auth secret does not used for push.


- -d, --p256dh Recipient's endpoint p256dh
- -a, --auth Recipient's endpoint auth


Print default WPN keys with use -y, --keys option.

You need provide one recipient endpoint in command line or from JSON file (with -j option).

Do not pass more than one endpoint.

Also you need provide:

- --private-key
- --public-key

Message composed of:

- Subject (-t option)
- Body (-b option)
- Icon (-i option)
- Action URL (-a option)

After push wpn exits immediately.

Send:

Chrome:
```
 ./wpn -m -f "andrei.i.ivanov@gmail.com" -t "hi there" -b "body message" -i https://commandus.com/favicon.ico -l https://commandus.com/ -t "Visit site" -k BM9Czc7rYYOinc7x_ALzqFgPSXV497qg76W6csYRtCFzjaFHGyuzP2a08l1vykEV1lgq6P83BOhB9xp-H5wCr1A -p _93Jy3cT0SRuUA1B9-D8X_zfszukGUMjIcO5y44rqCk https://fcm.googleapis.com/fcm/send/fsvJsFvUpvE:APA91bFLUUr0Owxupb1AqRZ_DE5AfVta35Hm2SAbczaGEQF6PgtEbhI0_ZWArirhcbioKakGPPR5lq4plQBm6QJazCKTiuQvRE1ptidKLq6S2y7h_89spPRi_E9ncJS59A5knRnKSxRh1T6TzJKizW739bWAQm7KKg -d BK0nI6BHSAM7yhv-5TUybvgzUSePr95RUkvNwDw3D7EsG_p0XpbbXnnY7PcwVWv9-v-17dDvD1mK7n4LWXcgSnM -a yMBwZZsKcENvwyeAux8FVg -vvvv -1
 ```

 Opera:
```
 ./wpn -m -f "andrei.i.ivanov@gmail.com" -t "hi there" -b "body message" -i https://commandus.com/favicon.ico -l https://commandus.com/ -c "Visit site" -k BM9Czc7rYYOinc7x_ALzqFgPSXV497qg76W6csYRtCFzjaFHGyuzP2a08l1vykEV1lgq6P83BOhB9xp-H5wCr1A -p _93Jy3cT0SRuUA1B9-D8X_zfszukGUMjIcO5y44rqCk https://fcm.googleapis.com/fcm/send/eVlrA0npluI:APA91bH5XkiU654FC9WWRKoBHhxUeUJBfedhCxyiK78VbOgnVgd_Jb4USMMYNrTwgykuK_uZlG4n8GnEUJLi-SNTBdwHvXesNBpVX2mp5Vae2iTRbxWTkgntuzcrVvlAghnZLf9EWrGQlHt20jO5y2kuP60Rrp3hSQ -d BHVRcPq9Mf8Ci2T3YwknrvSB2r-0qypk_lJ7nU75S2GraxJnPQlFfaQPzy6Tt-BY2DkghwhZbmPEEErWxVzTHPE -a Hx5DZb93lE98ub6KYRdveg 
 ```

## libraries

- libwpn.a	static library
- libwpn.so	C++ shared library
- libwpnapi.so	C shared library

C++

- std::string webpushVapidCmd();	// Helper function for testing. Print out "curl ..."  command line string
- int webpushVapid();				// Push message
- int webpushVapidData()			// Push message data

```
#include "utilvapid.h"
```

C

- size_t webpushVapidCmdC();	// Helper function for testing. Print out "curl ..."  command line string
- int webpushVapidC();			// Push message
- int webpushVapidDataC()		// Push message data

```
#include "wpnapi.h"
```

EXPORT int webpushVapidC(
	char* retval,
	size_t retvalsize,
	const char*publicKey,
	const char*privateKey,
	const char*endpoint,
	const char*p256dh,
	const char*auth,
	const char*body,
	const char*contact,
	int contentEncoding,
	time_t expiration = 0
);

## Service push message

push.php script contains keys.

Do not forget accesskey=2117177 option.

```
https://ikfia.wpn.commandus.com/app/push?accesskey=2117177&title=%D0%9C%D1%80%D0%B0%D0%BA&text=%D0%96%D1%83%D1%82%D1%8C111
```

## Output plugins

Output plugins are shared libraries See declaration in wpn-notify.h

```
./wpn -vv --plugin ../wpn-lnotify/.libs/libwpn-lnotify.so --plugin ../wpn-lnotify/.libs/libwpn-stdout.so
```

## Listen messages

```
./wpn --plugin ../wpn-lnotify/.libs
```

## Other options

Some helpful options for developers:

### re-generate VAPID keys

[VAPID and FCM](https://developers.google.com/web/ilt/pwa/introduction-to-push-notifications#vapid)


```
./wpn --generate-vapid-keys
```

produce JSON output:
```
{
  "privateKey":"rL9s2AQJnUoyGDjd-Vm251i_eFUgEK3kiQNJi9p1u3Q",
  "publicKey":"BMF5kzOI4ZRKIaU2XF8koyhJNm7vGY2S9ZKe0OBUbK7rgFmR7D2uUppnIYhs_vVc-8z9CxGwzMLldZ8cYVm9mIc",
  "authSecret":"xib7Leaop34KFdBpUVUJaQ"
}
```

Check is a new generated VAPID keys are stored:

```
./wpn -y -o json
```

### Different configuration file name

Set up different configuration file name with -c option:

```
./wpn -c config.cfg
```

Default configuration file name is ~/.wpn

## Files

- ~/.wpn

### List of recipient FCM token format

JSON array of array e.g.:
```
[
	[1,"<FCM Token>",0,...],
	...
]
```
FCM token must be second item in array.

### Settings format

~/.wpn text file keeps settings in lines:

- Identifiers and tokens
- Keys
- Subscription 1
- ...
- Subscription N

Each line separated by one space.

Identifiers and tokens section consists of identifiers and security tokens:

- appId Application- Android instance identifier (8 bytes minus 4 high bits, base64 encoded and some chars replaced (url conformant))
- androidId identifier assigned by Google service 64 bits long unsigned integer (a.k.a Android id)
- token Security token 64 bits long unsigned integer
- GCMToken GCM token string

Keys consists of

- privateKey base64 encoded 32 bytes
- publicKey base64 encoded 65 bytes
- authSecret base64 encoded 16 bytes

Subscription consists of

- subscribeUrl
- subscribeMode always 1
- endpoint
- authorizedEntity
- token
- pushSet
- server key (can be empty)

## Utilities

### Encryption/decryption tool using AES128GCM

ec tool encrypt/decrypt small pieces of data for testing puproses only.

Generate public/private keys

```
ec -g
```

Prints base64-encoded tab-delimited public and private key.

#### Encryption

Public key must be base64 encoded.

```
ec public_key < file_to_encrypt
```

#### Decryption

Private key must be base64 encoded.

```
ec private_key < file_to_decrypt
```

## Build

C++11 compliant compiler required.

Tested with gcc 4.9, clang 3.8

- gcc, g++ 4.8+
- clang, clang++ 3.4+

### Using clang

```
./configure CC=clang CXX=clang++
```

Check list:

- Tools
- Libraries

See below how to install tools and dependencies.

After check completed, build:
```
cd wpn
./autogen.sh
./configure
make
```

You can not build outside the source tree.

If you want, install:

```
sudo make install
```

### docker build

Start docker shell with mounted source directory:

```
docker run -itv $HOME/src:$HOME/src centos:nova bash
```

Then rebuild

```
cd ~/wpn
make clean; make
```

Then exit docker shell and cleanup (replace name to yours):

```
docker ps -a
docker commit stoic_ramanujan
docker images
docker tag c30cb68a6443 centos:nova
# remove closed containers
docker rm $(docker ps -qa --no-trunc --filter "status=exited")
```

### Problems

Error: undefined reference to `google::protobuf::internal::empty_string_

Different versions of gcc 4/5+ used to compile sources and protobuf library.

Rebuild protobuf lib:

```
cd tools
./install-protobuf-2.6.1.sh
```

### Tools

- Autoconf 2.63
- CMake 3.1 or higher
- Protobuf compiler (protoc)

#### Ubuntu

```
sudo apt update
sudo apt install autoconf libtool g++ cmake
```

### Dependencies

- curl https://github.com/curl/curl
- zlib https://zlib.net/ (curl dependency)
- openssl 1.1.0 https://github.com/openssl/openssl
- Protobuf 2.6.1 (use tools/install-protobuf-2.6.1.sh to install proper version)
- ecec https://github.com/web-push-libs/ecec (included in third_party- a few changes added(see ecec section))
- argtable3 https://github.com/argtable/argtable3 https://www.argtable.org/ BSD (included in third_party/ )
- JSON for Modern C++ 3.1.2 https://github.com/nlohmann/json (included in third_party/ )
- QR-Code-generator https://github.com/nayuki/QR-Code-generator (included in third_party/ )

#### Install most of libs

Before configure check libs, you can install it in Ubuntu:

```
sudo apt install libcurl4-openssl-dev libnghttp2-dev libunwind-dev libgoogle-glog-dev libprotobuf-dev protobuf-compiler
```

ecec, openssl must be installed from git repositories

### curl

Ubuntu:
```
sudo apt install libcurl4-openssl-dev
```

### openssl

ecec library requires OpenSSL version > 1.1.0, Ubuntu's repository has too old version.

Install library from the tarball.

```
tools/install-openssl-1.1.1.sh
```

or

```
cd lib
wget -c https://github.com/openssl/openssl/archive/OpenSSL_1_1_0h.tar.gz
tar xvfz OpenSSL_1_1_0h.tar.gz
mv openssl-OpenSSL_1_1_0h openssl-1.1.0h
cd openssl-1.1.0h
./config
make
sudo make install
sudo ldconfig
openssl version
```
OpenSSL by default installed in Ubuntu:

- includes /usr/local/include/openssl
- manuals /usr/local/share/man
- html /usr/local/share/doc/openssl
- libs /usr/local/lib

MacOS:

```
brew install openssl@1.1
```

### nghttp2

Ubuntu:
```
sudo apt install libnghttp2-dev
```

```
git clone git@github.com:nghttp2/nghttp2.git
cd nghttp2
autoreconf -i
./configure
make
sudo make install
```

MacOS:

```
brew install nghttp2
```

### ecec

```
git clone https://github.com/web-push-libs/ecec.git
cd ecec
vi CMakerLists.txt
	+ set(CMAKE_POSITION_INDEPENDENT_CODE ON)
mkdir build
cd build
cmake -DOPENSSL_ROOT_DIR=/usr/local ..
# MacOS: cmake -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl@1.1ß ..
make
sudo cp libece.a /usr/local/lib
cd ../include
sudo cp -r * /usr/local/include
```

### libunwind

```
sudo apt install libunwind-dev
```

MacOS:

```

```

### libglog

```
sudo apt install libgoogle-glog-dev
```

### libprotobuf

For wpn-gtk required libprotobuf version 2.6.1.

Use tools/install-protobuf-2.6.1.sh script to install 2.6.1 version.

```
cd wpn
cd tools
./install-protobuf-2.6.1.sh
cd ..
make wpn
./wpn --version
make libwpn.la
```

./wpn --version must return:
```
libprotobuf: 2.6.1
```

Otherwise install other version by

```
sudo apt install libprotobuf-dev protobuf-compiler
```

MacOS:

```
brew install protobuf
```

## Errors

Subscription errors

Error=PHONE_REGISTRATION_ERROR

Android device is not registered

## License

This software is licensed under the MIT License:

Copyright © 2018 Andrei Ivanov

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This software depends on libraries which has differen licenses:

- curl is licensed under MIT
- zlib is licensed under zlib License
- openssl is licensed under original SSL license, original SSLeay License
- ecec is licensed under MIT
- JSON for Modern C++ is licensed under MIT
- QR-Code-generator is licensed under MIT

## Dialogs

```
./wpn -v -s -a 1 -i 518511566414
```
similar to request:

```
POST https://fcm.googleapis.com/fcm/connect/subscribe
Accept: application/json
Content-Type: application/json

{
"endpoint": "https://fcm.googleapis.com",
"authorized_entity":"518511566414",
"encryption_key":"BHNg9UFl_BQXmtbclRJEVBnUC9aAIvlgfIKNxGnptLLEZntm8hqV-RrMjJrd7d5fwNlXKSZizfczpMQwz1tQ8tY",
"encryption_auth":"EWMSz0gFhdAna1UgfhR-Qg"
}
```

Curl:

```
curl -i -H "Accept:application/json" -H "Content-Type:application/json" -X POST https://fcm.googleapis.com/fcm/connect/subscribe -d '{"endpoint": "https://fcm.googleapis.com","encryption_key":"BHNg9UFl_BQXmtbclRJEVBnUC9aAIvlgfIKNxGnptLLEZntm8hqV-RrMjJrd7d5fwNlXKSZizfczpMQwz1tQ8tY","encryption_auth":"EWMSz0gFhdAna1UgfhR-Qg","authorized_entity":"518511566414"}'
```

```
curl -i -H "Accept:application/json" -H "Content-Type:application/json" -X POST https://fcm.googleapis.com/fcm/connect/subscribe -d '{"endpoint": "https://fcm.googleapis.com","encryption_key":"BHNg9UFl_BQXmtbclRJEVBnUC9aAIvlgfIKNxGnptLLEZntm8hqV-RrMjJrd7d5fwNlXKSZizfczpMQwz1tQ8tY","encryption_auth":"EWMSz0gFhdAna1UgfhR-Qg","authorized_entity":"518511566414"}'
```

### Response
```
{
	"token": "c9UC0WcwvyM:APA91bFlAcs7RbWVDPLW42nfL8RN8YYpe0zFnXcT0pMAJihu0WAOqGuoPujHYVJUHC0eRy5DTFepXvlaIyClHEpy6J6itEdT-QzD5SMCLt3HfBH_20LrWIuAXRrGLOWW8g9Y8aF1ikBc",
	"pushSet": "eJpriwkjrcU:APA91bHS4Ohb5In3ssqr3nPWI_EtFbAHEYvxN3SX1Omct5hjy48CeyTCZw5bzxyST1Bhj4m0WynXoq7pmw3IM0JuAQ8poeJe99vFJSeYGKgXtut_2Cmyxwu_V6xrDUqp-k8HDaeN_5fy"
}
```

## Issues

### OpenSSL

Script to build NDK toolchain and compile OpenSSL

https://github.com/leenjewel/openssl_for_ios_and_android

If error

```
... -mandroid
```

occured means OpenSSL Perl build script is uncompatible with newer NDK versions (since version 14),

- In OpenSSL's configdata.pm delete -mandroid
- In ~/git/openssl_for_ios_and_android/tools/build-openssl4android.sh comment line: rm -rf "${LIB_NAME}"
- In ~/git/openssl_for_ios_and_android/tools/build-openssl4android.sh comment line: tar xfz "${LIB_NAME}.tar.gz"

because script untar downloaded OpenSSL tarball each time and overrides changes in the source directory

### Library choice

- https://github.com/Thalhammer/jwt-cpp fails with ES256

### ECEC library

Bugs:

- Chrome padding bug
- -fPic issue

https://github.com/web-push-libs/ecec/issues/37

MCS using '=' padding, e.g.

```
encryption: salt=3jFrNEVgtPynKcHPmHXawA==
```

ece_webpush_aesgcm_headers_extract_params() return -13 error in this case.

Solution:

Line 182  added: || c == '=';

Add line

```
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
```
into CMakeFile.txt


## Appendix 1. Configuring Postfix to relay e-mail to gmail smarthost using SMTP relay over SSL(not TLS)

TLS port can be blocked by firewall, in this case use instructions above.
No more than 500 messages per day you can send using SMTP.

Please note if your Google account is two-factor authoriziation is enabled application password for mail service required.

```
sudo apt install mailutils
    Select Internet
sudo vi /etc/postfix/master.cf
Uncomment line
    smtps     inet  n       -       y       -       -       smtpd
sudo vi /etc/postfix/main.cf
    Add lines
    relayhost = [smtp.gmail.com]:465
    smtpd_use_tls=no
    smtp_use_tls = no
    smtp_sasl_auth_enable = yes
    smtp_sasl_password_maps = hash:/etc/postfix/sasl_passwd
    smtp_sasl_security_options = noanonymous
    smtp_tls_CAfile = /etc/postfix/cacert.pem
    smtp_tls_wrappermode = yes
	smtp_tls_security_level = encrypt

sudo vi /etc/postfix/sasl_passwd

  [smtp.gmail.com]:465    andrei.i.ivanov@gmail.com:PASSWORD_OR_APP_PASSWORD_IF_TWO_FACTOR_AUTH_ENABLED

sudo postmap /etc/postfix/sasl_passwd
sudo /etc/init.d/postfix reload

sudo service rsyslog restart
echo "Test mail from postfix" | mail -s "Test Postfix" andrei.i.ivanov@gmail.com
cat /var/log/mail.log
# remove all not sent messages
mailq
sudo postsuper -d ALL

```

### curl (static)

Directory D:\l\deps\zlib_x86_static contains precompiled OpenSSL libs and headers
```
cd curl
cd winbuild64
nmake /f Makefile.vc mode=static nmake /f Makefile.vc mode=static VC=15 RTLIBCFG=static WITH_SSL=static WITH_DEVEL=D:\l\deps\zlib_x64_static

cd ..
cd winbuild
nmake /f Makefile.vc mode=static nmake /f Makefile.vc mode=static VC=15 RTLIBCFG=static WITH_SSL=static WITH_DEVEL=D:\l\deps\zlib_x64_static
```

## Bug list

-1 --aesgcm does not work.

Service respond 201, but nothing is received by remote peer.

