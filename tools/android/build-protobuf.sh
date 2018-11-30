#!/bin/sh
PREFIX=`pwd`/protobuf/android
#rm -rf ${PREFIX}
mkdir -p ${PREFIX}

export NDK=${ANDROID_NDK}
 
# 1. Use the tools from the Standalone Toolchain
export PATH=YOUR_NDK_STAND_ALONE_TOOL_PATH/bin:$PATH
export SYSROOT=YOUR_NDK_STAND_ALONE_TOOL_PATH/sysroot
export CC="arm-linux-androideabi-gcc --sysroot $SYSROOT"
export CXX="arm-linux-androideabi-g++ --sysroot $SYSROOT"
export CXXSTL=$NDK/sources/cxx-stl/gnu-libstdc++/4.6
 
##########################################
# Fetch Protobuf 2.5.0 from source.
##########################################

(
    cd /tmp
    wget -O /tmp/protobuf-3.6.1.zip https://github.com/protocolbuffers/protobuf/archive/master.zip

    if [ -d /tmp/protobuf-3.6.1 ]
    then
        rm -rf /tmp/protobuf-3.6.1
    fi
    unzip /tmp/protobuf-3.6.1.zip
)

cd /tmp/protobuf-3.6.1

mkdir build

# 3. Run the configure to target a static library for the ARMv7 ABI
# for using protoc, you need to install protoc to your OS first, or use another protoc by path
./configure --prefix=$(pwd)/build \
--host=arm-linux-androideabi \
--with-sysroot=$SYSROOT \
--disable-shared \
--enable-cross-compile \
--with-protoc=protoc \
CFLAGS="-march=armv7-a" \
CXXFLAGS="-march=armv7-a -I$CXXSTL/include -I$CXXSTL/libs/armeabi-v7a/include"
 
# 4. Build
make && make install
 
# 5. Inspect the library architecture specific information
arm-linux-androideabi-readelf -A build/lib/libprotobuf-lite.a

cp build/lib/libprotobuf.a $PREFIX/libprotobuf.a
cp build/lib/libprotobuf-lite.a $PREFIX/libprotobuf-lite.a
