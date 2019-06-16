# Tests

## ec encrypt/decrypt

### Usage

Auth is optional, if auth is not provided or empty, auth is zeroes.

#### Generate keys

```
ec -g
public_key private_key auth
```

#### Encryption

With zero auth:

```
ec public_key < file_to_encrypt
```

#### Decryption

With zero auth:

```
ec private_key < file_to_decrypt
```

## aes128gcm alghorithm

[ECEC C library example](https://github.com/web-push-libs/ecec)

[RFC 8188](https://tools.ietf.org/html/rfc8188)

### Build separately

```
gcc -c -I ../third_party ../third_party/argtable3/argtable3.c
gcc -c -I ../third_party ../third_party/ecec/base64url.c
gcc -c -I ../third_party ../third_party/ecec/decrypt.c
gcc -c -I ../third_party ../third_party/ecec/params.c
gcc -c -I ../third_party ../third_party/ecec/keys.c
gcc -c -I ../third_party ../third_party/ecec/trailer.c
gcc -c -I ../third_party ../third_party/ecec/encrypt.c
gcc -c -I ../third_party ../third_party/ecec/decrypt.c
g++ -std=c++11 -o ec -I .. -I ../third_party ec.cpp ../utilvapid.cpp argtable3.o base64url.o params.o keys.o trailer.o encrypt.o decrypt.o -L/usr/local/lib -lssl -lcrypto
strip ec
```

```
gcc -c -I ../third_party ../third_party/argtable3/argtable3.c
g++ -O2 -std=c++11 -o ec -I .. -I ../third_party ec.cpp ../utilvapid.cpp argtable3.o -L/usr/local/lib -lssl -lcrypto -lece
strip ec
```