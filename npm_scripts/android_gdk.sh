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
    command -v $1 >/dev/null 2>&1 || { echo >&2 "$1 not found, exiting."; exit 1; }
}
check_command curl
check_command gzip
check_command shasum

VERSION="release_0.71.3"
TARBALL="gdk-android-jni.tar.gz"
SHA256=6c379748f218493757cd7b1a49e5a032bd61c254e76928292c03601546b6baae


URL="https://github.com/Blockstream/gdk/releases/download/${VERSION}/${TARBALL}"

rm -rf libgreenaddress-android gdk-android-jni gdk-android-jni.tar.gz

echo "Fetching gdk prebuilt for android supported archs..."

curl -sL -o ${TARBALL} "${URL}"
echo "${SHA256}  ${TARBALL}" | shasum -a 256 --check
tar xf ${TARBALL}

mkdir libgreenaddress-android;
yes | cp -rf gdk-android-jni/lib/* libgreenaddress-android/

rm -rf gdk-android-jni gdk-android-jni.tar.gz
