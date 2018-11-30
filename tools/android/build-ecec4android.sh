#!/bin/bash
set -u
source ./_shared.sh
# Setup architectures, library name and other vars + cleanup from previous runs
LIB_NAME="ecec-0.9.1"
LIB_DEST_DIR=${TOOLS_ROOT}/libs
[ -d ${LIB_DEST_DIR} ] && rm -rf ${LIB_DEST_DIR}
[ -f "${LIB_NAME}.tar.gz" ] || wget -O ecec-0.9.1.tar.gz https://github.com/web-push-libs/ecec/archive/v0.9.1.tar.gz;

configure_make() {
  ARCH=$1; ABI=$2;
  # Unarchive library, then configure and make for specified architectures
#  rm -rf "${LIB_NAME}"
#  tar xfz "${LIB_NAME}.tar.gz"
  pushd "${LIB_NAME}"

  mkdir -p build
  cd build
   cmake -DOPENSSL_ROOT_DIR=../../../output/android/openssl-$ABI ..
  cd ..
  
  OUTPUT_ROOT=${TOOLS_ROOT}/../output/android/openssl-${ABI}
  [ -d ${OUTPUT_ROOT}/include ] || mkdir -p ${OUTPUT_ROOT}/include
  cp -r ${LIB_DEST_DIR}/${ABI}/include/openssl ${OUTPUT_ROOT}/include

  [ -d ${OUTPUT_ROOT}/lib ] || mkdir -p ${OUTPUT_ROOT}/lib
  cp ${LIB_DEST_DIR}/${ABI}/lib/libcrypto.a ${OUTPUT_ROOT}/lib
  cp ${LIB_DEST_DIR}/${ABI}/lib/libssl.a ${OUTPUT_ROOT}/lib

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
