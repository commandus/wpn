#! /bin/bash
# sudo apt-get update
# apt-get install build-essential wget sudo
# yum install build-essential wget sudo

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
