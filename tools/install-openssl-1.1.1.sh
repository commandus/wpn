#! /bin/bash
# sudo apt-get update
# sudo apt-get install build-essential

VERSION=1.1.1a
if [ ! -d protobuf-${VERSION} ]; then
  wget -c --no-check-certificate https://www.openssl.org/source/openssl-${VERSION}.tar.gz
  tar xzf openssl-${VERSION}.tar.gz
fi;
cd openssl-${VERSION}
sudo ./config
sudo make
sudo make install 
sudo ln -s /usr/local/bin/openssl /usr/bin/openssl
