# Web push notification command line interface client

wpn

[Project description](https://docs.google.com/document/d/19pM4g-hvx2MUVV2Ggljw1MMTg9tMCEH7aHYYKpJMaWk/edit?usp=sharing)

## Build

```
cd wpn
./autogen.sh
./configure
make
```

## Subscribe

```
./wpn -s -u https://fcm.googleapis.com -p 518511566414
{
	"token": "c9UC0WcwvyM:APA91bFlAcs7RbWVDPLW42nfL8RN8YYpe0zFnXcT0pMAJihu0WAOqGuoPujHYVJUHC0eRy5DTFepXvlaIyClHEpy6J6itEdT-QzD5SMCLt3HfBH_20LrWIuAXRrGLOWW8g9Y8aF1ikBc",
	"pushSet": "eJpriwkjrcU:APA91bHS4Ohb5In3ssqr3nPWI_EtFbAHEYvxN3SX1Omct5hjy48CeyTCZw5bzxyST1Bhj4m0WynXoq7pmw3IM0JuAQ8poeJe99vFJSeYGKgXtut_2Cmyxwu_V6xrDUqp-k8HDaeN_5fy"
}
```

## Dependencies

- curl https://github.com/curl/curl
- argtable2 https://github.com/jonathanmarvens/argtable2/
- openssl 1.1.0 https://github.com/openssl/openssl
- nghttp2 https://github.com/nghttp2/nghttp2
- ecec https://github.com/web-push-libs/ecec
- JSON for Modern C++ https://nlohmann.github.io/json/	https://github.com/nlohmann/json	License MIT (included in third_party)

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

## Dialogs

Correct enpoint

./wpn -v -s -a 1 -p https://fcm.googleapis.com -i 518511566414

Incorrect enpoint

./wpn -v -s -a 1 -p https://sure-phone.firebaseio.com -i 518511566414

### Request
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
