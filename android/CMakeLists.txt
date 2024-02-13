project(NativeGdk)
cmake_minimum_required(VERSION 3.9.0)

set (PACKAGE_NAME "react-native-gdk")
set (BUILD_DIR ${CMAKE_SOURCE_DIR}/build)
set (CMAKE_VERBOSE_MAKEFILE ON)
set (CMAKE_CXX_STANDARD 17)

include_directories(
        "../cpp"
        "../cpp/include"
        "${NODE_MODULES_DIR}/react-native/ReactAndroid/src/main/jni/react/turbomodule"
        "${NODE_MODULES_DIR}/react-native/ReactCommon"
        "${NODE_MODULES_DIR}/react-native/ReactCommon/callinvoker"
        "${NODE_MODULES_DIR}/react-native/ReactCommon/jsi"
        "${NODE_MODULES_DIR}/react-native/ReactCommon/turbomodule/core"
        "${NODE_MODULES_DIR}/react-native/ReactCommon/react/nativemodule/core"
)

find_package(fbjni REQUIRED CONFIG)
find_package(ReactAndroid REQUIRED CONFIG)

add_library(
        ${PACKAGE_NAME}
        SHARED
        ./cpp-adapter.cpp
        ../cpp/GdkHostObject.cpp
        ../cpp/json.cpp
        ../cpp/utils.cpp
        ../cpp/TwoFactorCall.cpp
        ../cpp/ThreadPool.cpp
)

find_library(
        LOG_LIB
        log
)



set_target_properties(
        ${PACKAGE_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_EXTENSIONS OFF
        POSITION_INDEPENDENT_CODE ON
)


add_library(ga SHARED IMPORTED)
set_target_properties(
    ga
    PROPERTIES
    IMPORTED_LOCATION
    ${CMAKE_SOURCE_DIR}/../libgreenaddress-android/${CMAKE_ANDROID_ARCH_ABI}/libgreenaddress.so
)


target_link_libraries(
        ${PACKAGE_NAME} # <-- our cpp code
        ${LOG_LIB} # <-- logcat
        ga
        ReactAndroid::jsi # <-- JSI
        android # <-- Android JNI core
        ReactAndroid::turbomodulejsijni
        fbjni::fbjni
        ReactAndroid::reactnativejni
        ReactAndroid::react_nativemodule_core
)
