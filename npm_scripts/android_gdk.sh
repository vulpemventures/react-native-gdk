#!/usr/bin/env bash

# Downloads and installs the pre-built gdk libraries for use by green_ios
set -e

# # Find out where we are being run from to get paths right
if [ ! -d "$(pwd)/ios" ]; then
    echo "Run fetch script from root folder"
    exit 1
fi


# Pre-requisites
function check_command() {
    command -v "$1" >/dev/null 2>&1 || { echo >&2 "$1 not found, exiting."; exit 1; }
}
check_command curl
check_command gzip
check_command shasum

VERSION="release_0.72.2"
FOLDERNAME="gdk-${VERSION}"
TARBALL="${FOLDERNAME}.tar.gz"
SHA256=a091a5e37f3ce69e1281d28bd2ea5a4898a5d48b6fb8059f412058b5e842b783
DESTINATION="libgreen_gdk-android"

URL="https://github.com/Blockstream/gdk/releases/download/${VERSION}/${TARBALL}"

# Remove previous prebuilts
rm -rf ${DESTINATION} ${FOLDERNAME} ${TARBALL}

# =================================================================================================
echo "Fetching gdk prebuilt for android supported archs..."
curl -sL -o ${TARBALL} "${URL}"
echo "${SHA256}  ${TARBALL}" | shasum -a 256 --check
tar xf ${TARBALL}
# =================================================================================================

# Copy the prebuilts for supported android archs
# https://developer.android.com/ndk/guides/abis
mkdir ${DESTINATION}
cp -rf ${FOLDERNAME}/lib/x86_64 ${DESTINATION}/
cp -rf ${FOLDERNAME}/lib/x86 ${DESTINATION}/
cp -rf ${FOLDERNAME}/lib/armeabi-v7a ${DESTINATION}/
cp -rf ${FOLDERNAME}/lib/arm64-v8a ${DESTINATION}/

# Cleanup
rm -rf ${FOLDERNAME} ${TARBALL}
