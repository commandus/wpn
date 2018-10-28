# Test shared library

## Shared library

.lib/libwpnapi.so

## Build separately

Build using automake or:
```
gcc -c -I ../../third_party ../../third_party/argtable3/argtable3.c
g++ -std=c++11 -o wpnlinkj -I ../.. -I ../../third_party wpnlinkj.cpp ../../config-filename.cpp argtable3.o -lcurl -L/usr/local/lib -L../../.libs -lwpnapi -lprotobuf -lssl -lcrypto
```
