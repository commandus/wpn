#! /bin/bash
# sudo apt-get update
# sudo apt-get install build-essential

PROTOBUF_VERSION=2.6.1
if [ ! -d protobuf-${PROTOBUF_VERSION} ]; then
  wget https://github.com/google/protobuf/releases/download/v${PROTOBUF_VERSION}/protobuf-${PROTOBUF_VERSION}.tar.gz
  tar xzf protobuf-${PROTOBUF_VERSION}.tar.gz
fi;
cd protobuf-${PROTOBUF_VERSION}
sudo ./configure
sudo make
sudo make check
sudo make install 
sudo ldconfig
protoc --version
