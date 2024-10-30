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
check_command xcodebuild
check_command lipo

# remove previous installs
rm -f gdk-iphone.tar.gz gdk-iphone-sim.tar.gz gdk-iphone-sim-x86_64.tar.gz libgreenaddress_full.a
rm -rf gdk-iphone gdk-iphonesim-arm64 gdk-iphonesim-x86_64 libgreenaddress_full.xcframework

VERSION="release_0.71.3"

echo "Fetching gdk prebuilt for iphone physical devices..."

# The version of gdk to fetch and its sha256 checksum for integrity checking
DEVICE_SHA256="8c5be13bd8cbfaba6bb0e8bfb10fda89541010e15bfee062e8ed6ce468fc48b5"
# Setup gdk version and url
DEVICE_TARBALL="gdk-iphone.tar.gz"
DEVICE_URL="https://github.com/Blockstream/gdk/releases/download/${VERSION}/${DEVICE_TARBALL}"

curl -sL -o ${DEVICE_TARBALL} "${DEVICE_URL}"
echo "${DEVICE_SHA256}  ${DEVICE_TARBALL}" | shasum -a 256 --check
tar xf ${DEVICE_TARBALL}

# move gdk headers (only for the first time) into the cpp main folder
yes | cp -rf gdk-iphone/include/gdk/* cpp/include


# xcodebuild -create-xcframework \
# -library gdk-iphone/lib/iphoneos/libgreenaddress_full.a \
# -output iphone/libgreenaddress_full.xcframework


echo "Fetching gdk prebuilt for arm64 simulator devices..."


# The version of gdk to fetch and its sha256 checksum for integrity checking
SIM_SHA256="ca6f71031aba6af807ac12172fc0cd15e7ff4ee3873d77ae46efe1b748f8702c"
# Setup gdk version and url
SIM_TARBALL="gdk-iphone-sim.tar.gz"
SIM_URL="https://github.com/Blockstream/gdk/releases/download/${VERSION}/${SIM_TARBALL}"

curl -sL -o ${SIM_TARBALL} "${SIM_URL}"
echo "${SIM_SHA256}  ${SIM_TARBALL}" | shasum -a 256 --check
tar xf ${SIM_TARBALL}



echo "Fetching gdk prebuilt for intel simulator devices..."


# The version of gdk to fetch and its sha256 checksum for integrity checking
INTEL_SHA256="42f5f78d2da0667f291ec43e5c1505cd100967eba812958f406ea9547e1f7242"
# Setup gdk version and url
INTEL_TARBALL="gdk-iphone-sim-x86_64.tar.gz"
INTEL_URL="https://github.com/Blockstream/gdk/releases/download/${VERSION}/${INTEL_TARBALL}"

curl -sL -o ${INTEL_TARBALL} "${INTEL_URL}"
echo "${INTEL_SHA256}  ${INTEL_TARBALL}" | shasum -a 256 --check
tar xf ${INTEL_TARBALL}


# pack all simulators binaries into a universal binary using lipo
lipo -create \
  gdk-iphonesim-x86_64/lib/iphonesimulator/libgreenaddress_full.a \
  gdk-iphonesim-arm64/lib/iphonesimulator/libgreenaddress_full.a \
  -output libgreenaddress_full.a

# pack simulators binaries and physical device libraries into the framework
xcodebuild -create-xcframework \
  -library gdk-iphone/lib/iphoneos/libgreenaddress_full.a \
  -library libgreenaddress_full.a \
  -output libgreenaddress_full.xcframework

# # Clean up any previous install
rm -f gdk-iphone.tar.gz gdk-iphone-sim.tar.gz gdk-iphone-sim-x86_64.tar.gz libgreenaddress_full.a
rm -rf gdk-iphone gdk-iphonesim-arm64 gdk-iphonesim-x86_64
