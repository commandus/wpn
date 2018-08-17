# Web push notification command line interface client

wpn

[Project description](https://docs.google.com/document/d/19pM4g-hvx2MUVV2Ggljw1MMTg9tMCEH7aHYYKpJMaWk/edit?usp=sharing)

## Print device identifiers and security tokens

Check:
```
./wpn --id -vvv
app id      android id  security token	GCM token
2ea0892f-.. 57468817..  2325..   d5DROBGRLmk:A..
```

### GCM token as QR code

on black screen (white characters, black background:

```
 ./wpn -q
 
██████████████████████████████████████████████████████████████████████████████████████████████████████████
██████████████████████████████████████████████████████████████████████████████████████████████████████████
██████████████████████████████████████████████████████████████████████████████████████████████████████████
██████████████████████████████████████████████████████████████████████████████████████████████████████████
████████              ██████  ██    ██████████  ██████      ██              ████  ██              ████████
████████  ██████████  ██          ████████    ████  ██      ██    ████    ████  ████  ██████████  ████████
████████  ██      ██  ████      ██  ██  ████  ████████████  ████  ██    ██  ██  ████  ██      ██  ████████
████████  ██      ██  ██  ██████    ██████          ████      ████      ██████    ██  ██      ██  ████████
████████  ██      ██  ██████  ██    ████████  ██            ██████      ████      ██  ██      ██  ████████
████████  ██████████  ██  ██████████  ██  ██  ██  ██████  ████  ██████  ████████████  ██████████  ████████
████████              ██  ██  ██  ██  ██  ██  ██  ██  ██  ██  ██  ██  ██  ██  ██  ██              ████████
██████████████████████████      ██████  ██  ████  ██████    ██  ██  ██  ██████  ██████████████████████████
████████          ██        ██      ██    ██  ██          ██████████    ████████    ██  ██  ██  ██████████
████████  ██████████████          ████████  ██  ████████    ██  ██    ████████    ██████      ██  ████████
██████████████  ██        ████  ████        ██    ████            ██████    ██████      ████    ██████████
████████            ██    ██      ██    ██  ██████████  ██████  ██  ██  ████████  ██          ████████████
████████      ██████  ████  ██      ██            ████  ██  ██████  ██  ████  ████    ████  ████  ████████
████████  ████  ██████  ██    ██    ████████  ████    ██    ██            ████      ████    ██    ████████
████████    ████████    ████  ██  ████████    ████            ██  ██████  ██  ████  ██████      ██████████
████████    ██████████████      ██  ██████  ██  ██████  ████    ██            ██████  ██      ██  ████████
████████████  ██  ██  ██  ██        ██████      ████  ██  ████████    ████  ████████  ████        ████████
████████████        ██  ████  ██████  ████    ██      ██    ██  ██          ██      ████  ██      ████████
████████    ████████  ██████  ██████  ████  ██            ██  ██████    ████      ████  ██      ██████████
████████████  ████  ████            ██████  ██████████  ██████  ████████  ██████    ██████        ████████
██████████████            ████      ██  ██    ██          ██████████    ██████            ████  ██████████
████████████  ██  ██████          ██  ████    ██  ██████    ██            ██████  ██████  ██  ████████████
██████████████    ██  ██  ██  ██████  ██    ██    ██  ██  ████████  ████████      ██  ██        ██████████
████████  ██████  ██████    ████    ████    ████  ██████    ████            ████  ██████      ██  ████████
████████  ██                        ████      ██          ████  ██    ████  ████          ██  ████████████
██████████████    ████  ██    ██    ██████            ████  ████      ██  ████        ████        ████████
████████  ████  ████  ████        ██      ██  ██    ██  ████████  ████  ██    ████  ██████  ██████████████
██████████████      ████  ██  ██  ██            ██  ████    ██████    ████████        ██          ████████
██████████  ████          ████  ██████    ██    ████████      ████████  ██████████████      ██████████████
████████    ████    ██  ██            ████  ████      ██    ████          ██████    ████████    ██████████
████████  ████  ██        ████    ██  ██      ██    ████  ██  ██  ████  ████    ██  ██  ██  ██  ██████████
████████  ████  ██████  ████  ██  ██  ██████    ██                        ██        ██            ████████
████████████████████      ██████    ██        ████  ██    ██████████████████████  ████    ████    ████████
████████    ██    ██████████    ████  ██████  ██  ██  ████  ██        ██████████████  ██████████  ████████
████████████████  ██  ████  ████████  ██  ██  ██        ██  ████████  ████  ██  ██  ████████    ██████████
██████████        ████      ██  ██  ██          ██████      ██████  ██  ██          ██        ████████████
████████  ████    ██  ████  ████    ██  ██    ██          ██  ████████  ████████          ██  ██  ████████
████████████████████████    ██        ████  ████  ██████    ████████  ██  ████    ██████      ██  ████████
████████              ██    ██████  ██        ██  ██  ██  ████████  ████████  ██  ██  ██  ████  ██████████
████████  ██████████  ████          ████    ██    ██████    ██    ████        ██  ██████  ██  ██  ████████
████████  ██      ██  ██  ██  ██    ██  ██    ██            ████████  ██████  ██            ████  ████████
████████  ██      ██  ██  ██  ██    ██████  ██    ████      ████          ████████████      ██  ██████████
████████  ██      ██  ██  ██      ██    ████    ██  ██  ████  ██████  ████████    ██████████  ██  ████████
████████  ██████████  ██  ██████████        ████    ██  ██    ██  ██  ██  ██████    ██████    ████████████
████████              ██    ██████  ██          ████████      ████████  ██████    ████████████  ██████████
██████████████████████████████████████████████████████████████████████████████████████████████████████████
██████████████████████████████████████████████████████████████████████████████████████████████████████████
██████████████████████████████████████████████████████████████████████████████████████████████████████████
██████████████████████████████████████████████████████████████████████████████████████████████████████████ 
``` 

on white screen (black characters, white background:

```
./wpn -Q
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

Option -e (--mailto) outputs e-mail body in HTML with link to connect mobile phone to the wpn:

```
./wpn -e "name"
```

This link contains information how to establish connection to wpn.

If Surephone mobile application, this link open this application and connect it to wpn.

Then pipe output
```
./wpn -e "Alice" | mail -s "$(echo -e "Click link in your phone\nContent-Type: text/html;charset=utf-8")" bob@acme.com
```

Use custom emale template with --template-file option:
```
./wpn -e "Alice" --template-file email-template.html | mail -s "$(echo -e "Connect device to wpn\nContent-Type: text/html;charset=utf-8")" bob@acme.com
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
- Action URL and caption(-l, -c options) optional

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
- Action URL and caption(-l, -c options), both optional

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
 ./wpn -m -f "andrei.i.ivanov@gmail.com" -t "hi there" -b "body message" -i https://commandus.com/favicon.ico -l https://commandus.com/ -c "Visit site" -k BM9Czc7rYYOinc7x_ALzqFgPSXV497qg76W6csYRtCFzjaFHGyuzP2a08l1vykEV1lgq6P83BOhB9xp-H5wCr1A -p _93Jy3cT0SRuUA1B9-D8X_zfszukGUMjIcO5y44rqCk https://fcm.googleapis.com/fcm/send/fsvJsFvUpvE:APA91bFLUUr0Owxupb1AqRZ_DE5AfVta35Hm2SAbczaGEQF6PgtEbhI0_ZWArirhcbioKakGPPR5lq4plQBm6QJazCKTiuQvRE1ptidKLq6S2y7h_89spPRi_E9ncJS59A5knRnKSxRh1T6TzJKizW739bWAQm7KKg -d BK0nI6BHSAM7yhv-5TUybvgzUSePr95RUkvNwDw3D7EsG_p0XpbbXnnY7PcwVWv9-v-17dDvD1mK7n4LWXcgSnM -a yMBwZZsKcENvwyeAux8FVg -vvvv -1
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

- appId Application-wide unique identifier UUID 16 bytes (128 bits) long e.g. 550e8400-e29b-41d4-a716-446655440000. See https://tools.ietf.org/html/rfc4122
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

### webpush-curl

To send message to recipient, message, sender and recipient options are mandatory.

Message:

- -t, --title Message subject
- -b, --body Message body
- -i, --icon Message icon URL
- -l, --action-link Message action link URL
- -c, --action-caption Message action link caption

Sender:

- -k, --public Sender's VAPID public key 
- -p, --private Sender's VAPID private key

Recipient:

- -d, --p256dh Recipient's endpoint p256dh
- -a, --auth Recipient's endpoint auth
- Recipient's endpoint URL

Optional:

- -f, --from Sender's e-mail, e.g. mailto:alice@acme.com, or http(s) URL. 
- -1, --aesgcm Turn aesgcm encoding. Default is aes128gcm
- -o, --curl Do not send, print curl command. File keeps ciphered data.

Option --сurl can be used for debugging with curl external program. File keeps encoded data for sending.

Print out CURL command line to send web push notification

Firefox:

Send using AES GCM:
```
./webpush-curl -t Title -b body -i https://commandus.com/favicon.ico -f mailto:andrei.i.i.ivanov@commandus.com -l https://commandus.com/ -c "Visit site" -k BM9Czc7rYYOinc7x_ALzqFgPSXV497qg76W6csYRtCFzjaFHGyuzP2a08l1vykEV1lgq6P83BOhB9xp-H5wCr1A -p _93Jy3cT0SRuUA1B9-D8X_zfszukGUMjIcO5y44rqCk -a 4SgZbJVmKUP56tJ39wcWPw https://updates.push.services.mozilla.com/wpush/v2/gAAAAABbZ7cIJuyrIqApNuZd0AVjSSrYk5Cef5cI29-g8iRpHvFZzvqO6bI0ymUcf1tJpvg0lCIF7GxAbU7yg7EMXUh6c4MKaFPsSEsLzC7Mlb1JyIAMz5Wf0orVg15A2OD9dBCCUwbol78DdinNpwz-ExA67dH7InfiUDeYZS6QmVNXaPhzpGo -d BBpYsgvCmjRZTlwQ__nWoeaLwuqxVc9Eg-GSloPxQdvVxapVybJKJMns8IMkYQUDiLBrnXp-qFugkPBq3fOncvY
```

Chrome:

Send using AES GCM:
```
./webpush-curl -t "hi there" -b "body message" -i https://commandus.com/favicon.ico -f "" -l https://commandus.com/ -c "Visit site" -k BM9Czc7rYYOinc7x_ALzqFgPSXV497qg76W6csYRtCFzjaFHGyuzP2a08l1vykEV1lgq6P83BOhB9xp-H5wCr1A -p _93Jy3cT0SRuUA1B9-D8X_zfszukGUMjIcO5y44rqCk -e https://fcm.googleapis.com/fcm/send/fsvJsFvUpvE:APA91bFLUUr0Owxupb1AqRZ_DE5AfVta35Hm2SAbczaGEQF6PgtEbhI0_ZWArirhcbioKakGPPR5lq4plQBm6QJazCKTiuQvRE1ptidKLq6S2y7h_89spPRi_E9ncJS59A5knRnKSxRh1T6TzJKizW739bWAQm7KKg -d BK0nI6BHSAM7yhv-5TUybvgzUSePr95RUkvNwDw3D7EsG_p0XpbbXnnY7PcwVWv9-v-17dDvD1mK7n4LWXcgSnM -a yMBwZZsKcENvwyeAux8FVg
```

Print out curl command line:
```
./webpush-curl -t "hi there" -b "body message" -i https://commandus.com/favicon.ico -f "" -l https://commandus.com/ -c "Visit site" -k BM9Czc7rYYOinc7x_ALzqFgPSXV497qg76W6csYRtCFzjaFHGyuzP2a08l1vykEV1lgq6P83BOhB9xp-H5wCr1A -p _93Jy3cT0SRuUA1B9-D8X_zfszukGUMjIcO5y44rqCk -e https://fcm.googleapis.com/fcm/send/fsvJsFvUpvE:APA91bFLUUr0Owxupb1AqRZ_DE5AfVta35Hm2SAbczaGEQF6PgtEbhI0_ZWArirhcbioKakGPPR5lq4plQBm6QJazCKTiuQvRE1ptidKLq6S2y7h_89spPRi_E9ncJS59A5knRnKSxRh1T6TzJKizW739bWAQm7KKg -d BK0nI6BHSAM7yhv-5TUybvgzUSePr95RUkvNwDw3D7EsG_p0XpbbXnnY7PcwVWv9-v-17dDvD1mK7n4LWXcgSnM -a yMBwZZsKcENvwyeAux8FVg -o aesgcm.bin

curl -v -X POST -H "Content-Type: application/octet-stream" -H "Content-Encoding: aesgcm"  ...
```

## Build

```
./autogen.sh
./configure
make
```

Dependencies

- curl https://github.com/curl/curl
- argtable3 https://github.com/argtable/argtable3 https://www.argtable.org/ BSD
- openssl 1.1.0 https://github.com/openssl/openssl
- nghttp2 https://github.com/nghttp2/nghttp2
- ecec https://github.com/web-push-libs/ecec
- JSON for Modern C++ https://nlohmann.github.io/json/	https://github.com/nlohmann/json
- Sole- lightweight C++11 library to generate universally unique identificators https://github.com/r-lyeh/sole
- glog https://github.com/google/glog
- unwind https://github.com/libunwind/libunwind
- JSON for Modern C++ 3.1.2 https://github.com/nlohmann/json
- QR-Code-generator https://github.com/nayuki/QR-Code-generator

### openssl

```
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

### nghttp2

```
git clone git@github.com:nghttp2/nghttp2.git
cd nghttp2
autoreconf -i
./configure
make
sudo make install
```

### ecec

```
git clone git@github.com:web-push-libs/ecec.git
https://github.com/web-push-libs/ecec.git
cd ecec
mkdir build
cd build
cmake -DOPENSSL_ROOT_DIR=/usr/local ..
make
sudo cp libece.a /usr/local/lib
cd ../include
sudo cp -r * /usr/local/include
```

## Tools

- Autoconf 2.63
- CMake 3.1 or higher

## Building

C++11 compliant compiler required.

```
cd wpn
./autogen.sh
./configure
make
```

If you want, install:

```
sudo make install
```

## Error codes

- -1 Out of memory
- -2 Invalid private key
- -3 Invalid public key
- -4 Error compute secret
- -5 Error encode public key
- -6 Error decrypt
- -7 Error decrypt padding
- -8 Error zero plain text
- -9 Too short block
- -10 Too short header
- -11 Zero cipker text
- -12 Error HKDF
- -13 Invalid encryption header
- -14 Invalid Crypto-key header
- -15 Invalid rs
- -16 Invalid salt
- -17 Invalid dh
- -18 Error encrypt
- -19 Error encrypt padding
- -20 Invalid auth secret
- -21 Error generate keys
- -22 Error decrypt truncated

## License

This software is licensed under the MIT License:

Copyright © 2018 Andrei Ivanov

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This software depends on libraries which has differen licenses:

- curl  is licensed under MIT
- openssl 1.1.0  is licensed under original SSL license, original SSLeay License
- nghttp2  is licensed under MIT
- ecec  is licensed under MIT
- JSON for Modern C++  is licensed under MIT
- Sole, lightweight C++11 library to generate universally unique identificators is licensed under  zlib/libpng licensed.
- glog Copyright (c) 2006, Google Inc.
- unwind is licensed under MIT license
- JSON for Modern C++ 3.1.2 is licensed under MIT
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

### Library choice

- https://github.com/Thalhammer/jwt-cpp fails with ES256

### ECEC library

https://github.com/web-push-libs/ecec/issues/37

MCS using '=' padding, e.g.

```
encryption: salt=3jFrNEVgtPynKcHPmHXawA==
```

ece_webpush_aesgcm_headers_extract_params() return -13 error in this case.

Solution:

Line 182  added: || c == '=';

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


sub mailto:andrei.i.ivanov@gmail.com
public BM9Czc7rYYOinc7x_ALzqFgPSXV497qg76W6csYRtCFzjaFHGyuzP2a08l1vykEV1lgq6P83BOhB9xp-H5wCr1A
private _93Jy3cT0SRuUA1B9-D8X_zfszukGUMjIcO5y44rqCk



'Content-Length': 151,
'Content-Type': 'application/octet-stream',
'Content-Encoding': 'aesgcm',
Encryption: 'salt=EHsLe9nq7wohprzTL5IJag',
'Crypto-Key': 'dh=BN9uvaWWj7TO6I6lXwOex7t-N86-IWC7xYnB5DxzyafcDBlLTOoFT5Z7oy-6mBuJaRZI4OuLUbRyEUqYUozODpM;p256ecdsa=BM9Czc7rYYOinc7x_ALzqFgPSXV497qg76W6csYRtCFzjaFHGyuzP2a08l1vykEV1lgq6P83BOhB9xp-H5wCr1A',
Authorization: 'WebPush eyJ0eXAiOiJKV1QiLCJhbGciOiJFUzI1NiJ9.eyJhdWQiOiJodHRwczovL3VwZGF0ZXMucHVzaC5zZXJ2aWNlcy5tb3ppbGxhLmNvbSIsImV4cCI6MTUzMzY0OTk3NCwic3ViIjoibWFpbHRvOmFuZHJlaS5pLml2YW5vdkBnbWFpbC5jb20ifQ.lod0oYFtf6Nf_Tt3ItI-r1axSW7w3Sq7iNaakbyZ420qYm_PBwH-pU9Wt9yG91nuiLCjGM2UbuAitpqQSo_VvA'
