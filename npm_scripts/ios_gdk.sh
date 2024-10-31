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
check_command xcodebuild
check_command lipo

VERSION="release_0.72.2"

DEVICE_SHA256="f77e292a0a308cd33771e2b17cac21db76f83cc85bc1b2a681f78a8178f57107"
DEVICE_FOLDER="gdk-iphone"
DEVICE_TARBALL="${DEVICE_FOLDER}.tar.gz"
DEVICE_URL="https://github.com/Blockstream/gdk/releases/download/${VERSION}/${DEVICE_TARBALL}"
DEVICE_NESTED_FOLDER="arm64-apple-ios13.00"

SIM_SHA256="f3237c142b8d03308e376b54d1de92d865c968b36be959255a1244b97495c644"
SIM_FOLDER="gdk-iphonesim-arm64"
SIM_TARBALL="gdk-iphone-sim.tar.gz"
SIM_URL="https://github.com/Blockstream/gdk/releases/download/${VERSION}/${SIM_TARBALL}"
SIM_NESTED_FOLDER="arm64-apple-ios13.00"

INTEL_SHA256="5a11fff9dc5c42e5d3287b75859ada27b59f85ed3164c58ebd71db5d5f753686"
INTEL_FOLDER="gdk-iphonesim-x86_64"
INTEL_TARBALL="gdk-iphone-sim-x86_64.tar.gz"
INTEL_URL="https://github.com/Blockstream/gdk/releases/download/${VERSION}/${INTEL_TARBALL}"
INTEL_NESTED_FOLDER="x86_64-apple-ios13.00"

LIBGREEN="libgreen_gdk_full"

# Remove previous installs
rm -f ${DEVICE_TARBALL} ${SIM_TARBALL} ${INTEL_TARBALL} ${LIBGREEN}.a
rm -rf ${DEVICE_FOLDER} ${SIM_FOLDER} ${INTEL_FOLDER} ${LIBGREEN}.xcframework

# =================================================================================================
# PHYSICAL DEVICE
echo "Fetching gdk prebuilt for iphone physical devices..."
curl -sL -o ${DEVICE_TARBALL} "${DEVICE_URL}"
echo "${DEVICE_SHA256}  ${DEVICE_TARBALL}" | shasum -a 256 --check
tar xf ${DEVICE_TARBALL}

# ARM SIMULATOR
echo "Fetching gdk prebuilt for arm64 simulator devices..."
curl -sL -o ${SIM_TARBALL} "${SIM_URL}"
echo "${SIM_SHA256}  ${SIM_TARBALL}" | shasum -a 256 --check
tar xf ${SIM_TARBALL}

# INTEL SIMULATOR
echo "Fetching gdk prebuilt for intel simulator devices..."
curl -sL -o ${INTEL_TARBALL} "${INTEL_URL}"
echo "${INTEL_SHA256}  ${INTEL_TARBALL}" | shasum -a 256 --check
tar xf ${INTEL_TARBALL}
# =================================================================================================

# move gdk headers (only for the first time) into the cpp main folder
cp -rf gdk-iphone/include/gdk/* cpp/include

# pack all simulators binaries into a universal binary using lipo
lipo -create \
  ${INTEL_FOLDER}/lib/${INTEL_NESTED_FOLDER}/${LIBGREEN}.a \
  ${SIM_FOLDER}/lib/${SIM_NESTED_FOLDER}/${LIBGREEN}.a \
  -output ${LIBGREEN}.a

# pack simulators binaries and physical device libraries into the framework
xcodebuild -create-xcframework \
  -library ${DEVICE_FOLDER}/lib/${DEVICE_NESTED_FOLDER}/${LIBGREEN}.a \
  -library ${LIBGREEN}.a \
  -output ${LIBGREEN}.xcframework

# Cleanup
rm -f ${DEVICE_TARBALL} ${SIM_TARBALL} ${INTEL_TARBALL} ${LIBGREEN}.a
rm -rf ${DEVICE_FOLDER} ${SIM_FOLDER} ${INTEL_FOLDER}
