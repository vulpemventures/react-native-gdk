#include <jni.h>
#include "react-native-gdk.h"

extern "C"
JNIEXPORT jdouble JNICALL
Java_com_gdk_GdkModule_nativeMultiply(JNIEnv *env, jclass type, jdouble a, jdouble b) {
    return gdk::multiply(a, b);
}
