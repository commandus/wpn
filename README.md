# Web push notification command line interface client

wpn

[Project description](https://docs.google.com/document/d/19pM4g-hvx2MUVV2Ggljw1MMTg9tMCEH7aHYYKpJMaWk/edit?usp=sharing)

## Print credentials

Check credentials:
```
./wpn -p -vvv
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

### Ubuntu

First install mail program. Please refer to Appendix A how to install Postfix.

```
./wpn -e "Alice Bobson" | mail -s "$(echo -e "Click link in your phone\nContent-Type: text/html;charset=utf-8")" andrei.i.ivanov@gmail.com
```

```
./wpn -e "Alice Bobson" --template-file email-template.html | mail -s "$(echo -e "Connect device to wpn\nContent-Type: text/html;charset=utf-8")" andrei.i.ivanov@gmail.com
```

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
./wpn -l -vv
subscribeUrl                authorizedEntity	token
https://fcm.googleapis.com	246829423295	    drq...

```

## Subscribe

Set -e (authorized-entity) and -k (server key):

```
./wpn -s -e 246829423295 -k server_key
{
	"token": "c9UC0WcwvyM:APA91bFlAcs7RbWVDPLW42nfL8RN8YYpe0zFnXcT0pMAJihu0WAOqGuoPujHYVJUHC0eRy5DTFepXvlaIyClHEpy6J6itEdT-QzD5SMCLt3HfBH_20LrWIuAXRrGLOWW8g9Y8aF1ikBc",
	"pushSet": "eJpriwkjrcU:APA91bHS4Ohb5In3ssqr3nPWI_EtFbAHEYvxN3SX1Omct5hjy48CeyTCZw5bzxyST1Bhj4m0WynXoq7pmw3IM0JuAQ8poeJe99vFJSeYGKgXtut_2Cmyxwu_V6xrDUqp-k8HDaeN_5fy"
}
```

Optional server key (API key for web application) (-k) is used to send reply. Server key looks like "AI....1I" (~40 characters).

Value of authorized-entity is decimal number identifies subscription if FCM.

## Unsubscribe

Specify subscription endpoint (-e):

```
./wpn -d -e 246829423295
{
	"token": "c9UC0WcwvyM:APA91bFlAcs7RbWVDPLW42nfL8RN8YYpe0zFnXcT0pMAJihu0WAOqGuoPujHYVJUHC0eRy5DTFepXvlaIyClHEpy6J6itEdT-QzD5SMCLt3HfBH_20LrWIuAXRrGLOWW8g9Y8aF1ikBc",
	"pushSet": "eJpriwkjrcU:APA91bHS4Ohb5In3ssqr3nPWI_EtFbAHEYvxN3SX1Omct5hjy48CeyTCZw5bzxyST1Bhj4m0WynXoq7pmw3IM0JuAQ8poeJe99vFJSeYGKgXtut_2Cmyxwu_V6xrDUqp-k8HDaeN_5fy"
}
```

## Unsubscribe all

```
./wpn -d
```

## Client push message

Send notification with -m option.

You need provide one or more recipient FCM token(s) up to 100 (limited by system actually) in command line or from JSON file (with -j option).

Also you need provide:

- server key (-k) to authorize send operation

Otherwise, you can provide -n option with name of subscripton (if you also subscribed). In this case subscripton entity and server key are obtained from this subscription:

- subscription name (-n)

Message composed of:

- Subject (-t option)
- Body (-b option)
- Icon (-i option)
- Action URL (-a option)

After push wpn exits immediately.

Sending to one recipient:

```
./wpn -m -k "AIzaSyBfUt1N5aabh8pubYiBPKOq9OcIoHv_41I" -t Subject -b Body -i "https://commandus.com/favicon.ico" -a "https://commandus.com" f22BfuvjjCc:APA91bHc4xzOyN5318sBkspPG9n2zBkP-jHl2EdJVKRHHv0njkplgPVe8s9MVkApTaWHkK9s9137gsPiWnmb_S9IF9h5LX3k8eg9jitNqs0xb7NK9BbPeC-nDw1SuCptZKTuEcKOvKpC
```

In the -k option, it's better to specify an outdated server key, since it's shorter.

Sending by the list of recipient tokens in a file or a web resource (-j option):

```
./wpn -m -k "AIzaSyAyz-oVjsfRNK53XA6o9DVHN9ZNAaHOfxw" -e 246829423295 -t Subject -b Body -i "https://commandus.com/favicon.ico" -a "https://commandus.com" -j "https://ikfia.wpn.commandus.com/app/token?accesskey=2117177"
```
If the -j option is specified, the list is padded, that is, you can specify the recipient's FCM tokens in both the command line and the file (web resource).

If you specify the -j option wpn 'll open the file. If it can not be opened for reading, or if it is empty, wpn tries to download web resource from the network.

Sending using subcription name (-n option) instead of server key (option -k):

```
./wpn -m -n "ubuntu16" -t "Subject kkkk--111" -b "Body 2" -i "https://commandus.com/favicon.ico" -a "https://commandus.com" "d4rd_JZJ940:APA91bEGqtzfqZaohjke2dCqY8z5xJOMmkKud1SU646l2QlwBiZpzb9hTgTefvCrdhQ6-oR82SgjyYpF3kkcgGdMqmhZArMAb4G-D8N_ZYg0BDmgnx92AXKUUqFzCJwqwzTvOmqkjfFy" -vvv
```

Option -u "https://ikfia-wpn.firebaseio.com" is not required.

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

- Credentials
- Keys
- Subscription 1
- ...
- Subscription N

Each line separated by one space.

Credentials consists of

- appId Application-wide unique identifier UUID 16 bytes (128 bits) long e.g. 550e8400-e29b-41d4-a716-446655440000. See https://tools.ietf.org/html/rfc4122
- androidId Android identifier assigned by Google service 64 bits long unsigned integer
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

## Dependencies

- curl https://github.com/curl/curl
- argtable2 https://github.com/jonathanmarvens/argtable2/
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
./wpn -v -s -a 1 -p https://fcm.googleapis.com -i 518511566414
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
