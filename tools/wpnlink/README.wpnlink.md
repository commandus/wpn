# wpnlink

Subscribe to subsciption.

## Shared library

.lib/libwpnapi.so

## Build separately

Build using automake or manually:

```
gcc -c -I ../../third_party ../../third_party/argtable3/argtable3.c
g++ -std=c++11 -o wpnlink -I ../.. -I ../../third_party wpnlink.cpp ../../config-filename.cpp argtable3.o -lcurl -L/usr/local/lib -L../../.libs -lwpnapi -lprotobuf -lssl -lcrypto
```
