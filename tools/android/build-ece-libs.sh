#!/bin/bash
set -u
TOOLS_ROOT=`pwd`
ANDROID_API=${ANDROID_API:-21}
ARCHS=("android" "android-armeabi" "android-x86")
ABIS=("armeabi" "armeabi-v7a" "x86")
NDK=${ANDROID_NDK}

# Setup architectures, library name and other vars + cleanup from previous runs
LIB_NAME="wpnc-1.0.0"
LIB_DEST_DIR=${TOOLS_ROOT}/libs

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
  export LIBS=${LIBS:-""}
  export CFLAGS="${ARCH_FLAGS} -fpic -ffunction-sections -funwind-tables -fstack-protector -fno-strict-aliasing -finline-limit=64"
  export CXXFLAGS="${CFLAGS} -std=c++11 -frtti -fexceptions"
  export LDFLAGS="${ARCH_LINK}"
}

configureMake() {
  ARCH=$1
  ABI=$2
  configureEnv $ARCH $ABI $LIB_NAME
  # fix me
  cp ${TOOLS_ROOT}/../output/android/openssl-${ABI}/lib/libssl.a ${SYSROOT}/usr/lib
  cp ${TOOLS_ROOT}/../output/android/openssl-${ABI}/lib/libcrypto.a ${SYSROOT}/usr/lib
  cp -r ${TOOLS_ROOT}/../output/android/openssl-${ABI}/include/openssl ${SYSROOT}/usr/include

  mkdir -p ${LIB_DEST_DIR}/${ABI}
  ./configure --prefix=${LIB_DEST_DIR}/${ABI} \
              --with-sysroot=${SYSROOT} \
              --host=${TOOL} \
              --with-ssl=/usr
  PATH=$TOOLCHAIN_PATH:$PATH
  make clean
  if make -j4
  then
    make install

    OUTPUT_ROOT=${TOOLS_ROOT}/../output/android/curl-${ABI}
    [ -d ${OUTPUT_ROOT}/include ] || mkdir -p ${OUTPUT_ROOT}/include
    cp -r ${LIB_DEST_DIR}/${ABI}/include/curl ${OUTPUT_ROOT}/include

    [ -d ${OUTPUT_ROOT}/lib ] || mkdir -p ${OUTPUT_ROOT}/lib
    cp ${LIB_DEST_DIR}/${ABI}/lib/libcurl.a ${OUTPUT_ROOT}/lib
  fi;
  popd;
}

for ((i=0; i < ${#ARCHS[@]}; i++))
do
  if [[ $# -eq 0 ]] || [[ "$1" == "${ARCHS[i]}" ]]; then
    [[ ${ANDROID_API} < 21 ]] && ( echo "${ABIS[i]}" | grep 64 > /dev/null ) && continue;
    configureMake "${ARCHS[i]}" "${ABIS[i]}"
  fi
done
