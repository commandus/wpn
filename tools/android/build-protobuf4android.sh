#!/bin/bash
set -u

TOOLS_ROOT=`pwd`
ARCHS=("android" "android-armeabi" "android-x86")
ABIS=("armeabi" "armeabi-v7a" "x86")
# ARCHS=("android")
# ABIS=("armeabi")
ANDROID_API=${ANDROID_API:-21}
NDK=${ANDROID_NDK}

# Setup architectures, library name and other vars + cleanup from previous runs
LIB_NAME="protobuf-3.6.1"
LIB_DEST_DIR=${TOOLS_ROOT}/libs
[ -d ${LIB_DEST_DIR} ] && rm -rf ${LIB_DEST_DIR}
[ -f "${LIB_NAME}.zip" ] || wget -O ${LIB_NAME}.zip https://github.com/protocolbuffers/protobuf/archive/master.zip;

#
# configureEnv arch output clang
# where
#   arch:  android-armeabi, ..
#   output: folder name prefix
#   clang: empty- gcc
#
configureEnv() {
  ARCH=$1
  OUT=$2
  CLANG=${3:-""}
  TOOLCHAIN_ROOT=${TOOLS_ROOT}/${OUT}-android-toolchain

  if [ "$ARCH" == "android" ]; then
    export ARCH_FLAGS="-mthumb"
    export ARCH_LINK=""
    export TOOL="arm-linux-androideabi"
    NDK_FLAGS="--arch=arm"
  elif [ "$ARCH" == "android-armeabi" ]; then
    export ARCH_FLAGS="-march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16 -mthumb -mfpu=neon"
    export ARCH_LINK="-march=armv7-a -Wl,--fix-cortex-a8"
    export TOOL="arm-linux-androideabi"
    NDK_FLAGS="--arch=arm"
  elif [ "$ARCH" == "android64-aarch64" ]; then
    export ARCH_FLAGS=""
    export ARCH_LINK=""
    export TOOL="aarch64-linux-android"
    NDK_FLAGS="--arch=arm64"
  elif [ "$ARCH" == "android-x86" ]; then
    export ARCH_FLAGS="-march=i686 -mtune=intel -msse3 -mfpmath=sse -m32"
    export ARCH_LINK=""
    export TOOL="i686-linux-android"
    NDK_FLAGS="--arch=x86"
  elif [ "$ARCH" == "android64" ]; then
    export ARCH_FLAGS="-march=x86-64 -msse4.2 -mpopcnt -m64 -mtune=intel"
    export ARCH_LINK=""
    export TOOL="x86_64-linux-android"
    NDK_FLAGS="--arch=x86_64"
  elif [ "$ARCH" == "android-mips" ]; then
    export ARCH_FLAGS=""
    export ARCH_LINK=""
    export TOOL="mipsel-linux-android"
    NDK_FLAGS="--arch=mips"
  elif [ "$ARCH" == "android-mips64" ]; then
    export ARCH="linux64-mips64"
    export ARCH_FLAGS=""
    export ARCH_LINK=""
    export TOOL="mips64el-linux-android"
    NDK_FLAGS="--arch=mips64"
  fi;

  [ -d ${TOOLCHAIN_ROOT} ] || python $NDK/build/tools/make_standalone_toolchain.py \
                                     --api ${ANDROID_API} \
                                     --stl libc++ \
                                     --install-dir=${TOOLCHAIN_ROOT} \
                                     $NDK_FLAGS

  export TOOLCHAIN_PATH=${TOOLCHAIN_ROOT}/bin
  export NDK_TOOLCHAIN_BASENAME=${TOOLCHAIN_PATH}/${TOOL}
  export SYSROOT=${TOOLCHAIN_ROOT}/sysroot
  export CROSS_SYSROOT=$SYSROOT
  if [ -z "${CLANG}" ]; then
    export CC=${NDK_TOOLCHAIN_BASENAME}-gcc
    export CXX=${NDK_TOOLCHAIN_BASENAME}-g++
  else
    export CC=${NDK_TOOLCHAIN_BASENAME}-clang
    export CXX=${NDK_TOOLCHAIN_BASENAME}-clang++
  fi;
  export LINK=${CXX}
  export LD=${NDK_TOOLCHAIN_BASENAME}-ld
  export AR=${NDK_TOOLCHAIN_BASENAME}-ar
  export RANLIB=${NDK_TOOLCHAIN_BASENAME}-ranlib
  export STRIP=${NDK_TOOLCHAIN_BASENAME}-strip
  export CPPFLAGS=${CPPFLAGS:-""}
  export LIBS="${LIBS:-""} -llog"
  export CFLAGS="${ARCH_FLAGS} -fpic -ffunction-sections -funwind-tables -fstack-protector -fno-strict-aliasing -finline-limit=64"
  export LDFLAGS="${ARCH_LINK}"

#  CFLAGS="-march=${ABI} -mfloat-abi=softfp -DGOOGLE_PROTOBUF_NO_RTTI" \
#  LDFLAGS="-static-libstdc++ -llog" \
}

# Parameters: ARCH ABI
configure_make() {
  configureEnv $*
  ARCH=$1; ABI=$2;
  export CXXSTL=${NDK}/sources/cxx-stl/llvm-libc++
  export CXXFLAGS="${CFLAGS} -std=c++11 -frtti -fexceptions -I${CXXSTL}/include -I${CXXSTL}/libs/${ABI}/include"

  # Copy libraries
  cp ${TOOLS_ROOT}/../output/android/openssl-${ABI}/lib/libssl.a ${SYSROOT}/usr/lib
  cp ${TOOLS_ROOT}/../output/android/openssl-${ABI}/lib/libcrypto.a ${SYSROOT}/usr/lib
  cp -r ${TOOLS_ROOT}/../output/android/openssl-${ABI}/include/openssl ${SYSROOT}/usr/include

  cp ${TOOLS_ROOT}/../output/android/curl-${ABI}/lib/libcurl.a ${SYSROOT}/usr/lib
  cp -r ${TOOLS_ROOT}/../output/android/curl-${ABI}/include/curl ${SYSROOT}/usr/include

  # Unarchive library, then configure and make for specified architectures
  if [ ! -d ${LIB_NAME} ]; then
    unzip -qq "${LIB_NAME}.zip"
    mv protobuf-master ${LIB_NAME}
  fi;
  
  pushd "${LIB_NAME}"
#  git submodule update --init --recursive
#  ./autogen.sh

  mkdir -p ${LIB_DEST_DIR}/${ABI}
#  ./autogen.sh
echo "prefix=${LIB_DEST_DIR}/${ABI} host=${TOOL} with-sysroot=${SYSROOT} march ${ABI}"
./configure --prefix=${LIB_DEST_DIR}/${ABI} \
  --host=${TOOL} \
  --with-sysroot=${SYSROOT} \
  --disable-shared \
  --enable-cross-compile \
  --with-protoc=protoc

#   --with-protoc=protoc \
#  CFLAGS="-march=${ABI} -mfloat-abi=softfp -DGOOGLE_PROTOBUF_NO_RTTI" \
#  CPPFLAGS="-march=${ABI}" \
#  LDFLAGS="-static-libstdc++" \

  PATH=$TOOLCHAIN_PATH:$PATH
  make clean
  if make -j4; then
    make install
    OUTPUT_ROOT=${TOOLS_ROOT}/../output/android/protobuf-${ABI}
    [ -d ${OUTPUT_ROOT}/include ] || mkdir -p ${OUTPUT_ROOT}/include
    cp -r ${LIB_DEST_DIR}/${ABI}/include/google/protobuf ${OUTPUT_ROOT}/include

    [ -d ${OUTPUT_ROOT}/lib ] || mkdir -p ${OUTPUT_ROOT}/lib
    cp ${LIB_DEST_DIR}/${ABI}/lib/libprotobuf.a ${OUTPUT_ROOT}/lib
  fi;
  popd
}

for ((i=0; i < ${#ARCHS[@]}; i++))
do
  if [[ $# -eq 0 ]] || [[ "$1" == "${ARCHS[i]}" ]]; then
    # Do not build 64 bit arch if ANDROID_API is less than 21 which is
    # the minimum supported API level for 64 bit.
    [[ ${ANDROID_API} < 21 ]] && ( echo "${ABIS[i]}" | grep 64 > /dev/null ) && continue;
    configure_make "${ARCHS[i]}" "${ABIS[i]}"
  fi
done
