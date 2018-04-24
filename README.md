# Web push notification command line interface client

wpn

[Project description](https://docs.google.com/document/d/19pM4g-hvx2MUVV2Ggljw1MMTg9tMCEH7aHYYKpJMaWk/edit?usp=sharing)

## Print credentials

Check credentials:
```
./wpn -c -vvv
private_key	public_key	auth_secret
r1...       BP...       bd..
```

## Subscription list

```
./wpn -l -vv
subscribeUrl                authorizedEntity	token
https://fcm.googleapis.com	246829423295	    drq...

```

## Subscribe

```

./wpn -s -e 246829423295
{
	"token": "c9UC0WcwvyM:APA91bFlAcs7RbWVDPLW42nfL8RN8YYpe0zFnXcT0pMAJihu0WAOqGuoPujHYVJUHC0eRy5DTFepXvlaIyClHEpy6J6itEdT-QzD5SMCLt3HfBH_20LrWIuAXRrGLOWW8g9Y8aF1ikBc",
	"pushSet": "eJpriwkjrcU:APA91bHS4Ohb5In3ssqr3nPWI_EtFbAHEYvxN3SX1Omct5hjy48CeyTCZw5bzxyST1Bhj4m0WynXoq7pmw3IM0JuAQ8poeJe99vFJSeYGKgXtut_2Cmyxwu_V6xrDUqp-k8HDaeN_5fy"
}
```

## Unsubscribe

```

./wpn -d -e 246829423295
{
	"token": "c9UC0WcwvyM:APA91bFlAcs7RbWVDPLW42nfL8RN8YYpe0zFnXcT0pMAJihu0WAOqGuoPujHYVJUHC0eRy5DTFepXvlaIyClHEpy6J6itEdT-QzD5SMCLt3HfBH_20LrWIuAXRrGLOWW8g9Y8aF1ikBc",
	"pushSet": "eJpriwkjrcU:APA91bHS4Ohb5In3ssqr3nPWI_EtFbAHEYvxN3SX1Omct5hjy48CeyTCZw5bzxyST1Bhj4m0WynXoq7pmw3IM0JuAQ8poeJe99vFJSeYGKgXtut_2Cmyxwu_V6xrDUqp-k8HDaeN_5fy"
}
```

## Client push message

After push wpn exits immediately.

Using list of recipient tokens (up to 100, but limited by system environment):

```
./wpn -m -k "AIzaSyAyz-oVjsfRNK53XA6o9DVHN9ZNAaHOfxw" -e 246829423295 -t Subject -b Body -i "https://commandus.com/favicon.ico" -a "https://commandus.com"  dl_liGwFeeA:APA91bEn8GjmoPxbi5xgYYffhrsb6WZjiLZA8Sogb7jBXrsJzoCzplV5SISS9mPd8IN-yFMLTIhCYGsRb925CCqGIZ2TPuoA2kj56hOECvsI-Fou1OdE1j1_FunMoWtkDtSyNx-djcQM
```

Sending by the list of recipient tokens in a file or a web resource (-J option):

```
./wpn -m -k "AIzaSyAyz-oVjsfRNK53XA6o9DVHN9ZNAaHOfxw" -e 246829423295 -t Subject -b Body -i "https://commandus.com/favicon.ico" -a "https://commandus.com" -J "https://ikfia.wpn.commandus.com/app/token?accesskey=2117177"
```

If the -J option is specified, the list is padded, that is, you can specify the recipient tokens in both the command line and the file (web resource).

If you specify the -J option wpn 'll open the file. If it can not be opened for reading, or if it is empty, wpn tries to download web resource from the network.

In the -k option, it's better to specify an outdated server key, since it's shorter.

Option -u "https://ikfia-wpn.firebaseio.com" is not required.

## Service push message

push.php script contains keys.

Do not forget accesskey=2117177 option.

```
https://ikfia.wpn.commandus.com/app/push?accesskey=2117177&title=%D0%9C%D1%80%D0%B0%D0%BA&text=%D0%96%D1%83%D1%82%D1%8C111
```

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

- GCM Credentials
- FCM Credentials
- Subscription 1
- ...
- Subscription N

Each line separated by one space.

GCM Credentials consists of

- appId Application-wide unique identifier UUID 16 bytes (128 bits) long e.g. 550e8400-e29b-41d4-a716-446655440000. See https://tools.ietf.org/html/rfc4122
- androidId Android identifier assigned by Google service 64 bits long unsigned integer
- token Security token 64 bits long unsigned integer
- FCMToken FCM token string

FCM Credentials consists of

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
- JSON for Modern C++ 3.1.2  is licensed under MIT

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
curl -i -H Accept:application/json -H Content-Type:application/json -X POST https://fcm.googleapis.com/fcm/connect/subscribe -H Content-Type: application/json -d '{"endpoint": "https://fcm.googleapis.com","encryption_key":"BHNg9UFl_BQXmtbclRJEVBnUC9aAIvlgfIKNxGnptLLEZntm8hqV-RrMjJrd7d5fwNlXKSZizfczpMQwz1tQ8tY","encryption_auth":"EWMSz0gFhdAna1UgfhR-Qg","authorized_entity":"518511566414"}'
```

### Response
```
{
	"token": "c9UC0WcwvyM:APA91bFlAcs7RbWVDPLW42nfL8RN8YYpe0zFnXcT0pMAJihu0WAOqGuoPujHYVJUHC0eRy5DTFepXvlaIyClHEpy6J6itEdT-QzD5SMCLt3HfBH_20LrWIuAXRrGLOWW8g9Y8aF1ikBc",
	"pushSet": "eJpriwkjrcU:APA91bHS4Ohb5In3ssqr3nPWI_EtFbAHEYvxN3SX1Omct5hjy48CeyTCZw5bzxyST1Bhj4m0WynXoq7pmw3IM0JuAQ8poeJe99vFJSeYGKgXtut_2Cmyxwu_V6xrDUqp-k8HDaeN_5fy"
}
```

