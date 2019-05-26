# Tests

## ec encrypt/decrypt

### Usage

#### Encryption

```
ec public_key < file_to_encrypt
```

####  Decryption

```
ec private_key < file_to_decrypt
```

### Build

```
gcc -c -I ../third_party ../third_party/argtable3/argtable3.c
gcc -c -I ../third_party ../third_party/ecec/base64url.c
gcc -c -I ../third_party ../third_party/ecec/decrypt.c
gcc -c -I ../third_party ../third_party/ecec/params.c
gcc -c -I ../third_party ../third_party/ecec/keys.c
gcc -c -I ../third_party ../third_party/ecec/trailer.c
gcc -c -I ../third_party ../third_party/ecec/encrypt.c
gcc -c -I ../third_party ../third_party/ecec/decrypt.c
g++ -std=c++11 -o ec -I .. -I ../third_party ec.cpp argtable3.o base64url.o params.o keys.o trailer.o encrypt.o decrypt.o -L/usr/local/lib -lssl -lcrypto
```
