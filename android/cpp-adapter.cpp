#include <jni.h>
#include <jsi/jsi.h>
#include "GdkHostObject.hpp"

using namespace facebook;

std::string jstringToString(JNIEnv *env, jstring jstr) {
    if (jstr == nullptr) {
        return "";
    }

    const char *utfChars = env->GetStringUTFChars(jstr, nullptr);
    if (utfChars == nullptr) {
        return ""; // Error occurred, handle accordingly
    }

    std::string str(utfChars);

    // Release the resources
    env->ReleaseStringUTFChars(jstr, utfChars);

    return str;
}

void install(jsi::Runtime& jsiRuntime, std::string dir) {
  auto instance = std::make_shared<GdkHostObject>(dir);
  jsi::Object GDK = jsi::Object::createFromHostObject(jsiRuntime, instance);
  jsiRuntime.global().setProperty(jsiRuntime, "GDK", std::move(GDK));

}

extern "C"
JNIEXPORT void JNICALL
Java_com_gdk_GdkModule_nativeInstall(JNIEnv *env, jobject clazz, jlong jsiPtr, jstring dir) {
    std::string dirString = jstringToString(env, dir);
    auto runtime = reinterpret_cast<jsi::Runtime*>(jsiPtr);
    if (runtime) {
        install(*runtime, dirString);
    }
}
