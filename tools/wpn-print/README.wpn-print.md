# Print out credentials

Test shared library

## Shared library

.lib/libwpnapi.so

## Build separately

Build using automake or:
```
g++ -std=c++11 -o wpn-print -I ../.. -I ../../third_party wpn-print.cpp ../../config-filename.cpp ../../utilfile.cpp argtable3.o -L/usr/local/lib -L../../ -lwpnpp -lprotobuf-lite -lece -lcrypto -lcurl
```
