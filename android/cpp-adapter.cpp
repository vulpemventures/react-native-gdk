#include <jni.h>
#include <jsi/jsi.h>
#include "GdkHostObject.hpp"

using namespace facebook;

// Function to convert jstring to char*
char* jstringToChar(JNIEnv* env, jstring jStr) {
    const char* str = env->GetStringUTFChars(jStr, nullptr);
    // Make a copy of the string to ensure memory safety
    char* result = strdup(str);
    // Release the original string
    env->ReleaseStringUTFChars(jStr, str);
    return result;
}

void install(jsi::Runtime& jsiRuntime, char* dir) {
    auto gdkCreateNewInstance = jsi::Function::createFromHostFunction(jsiRuntime,
                                                                       jsi::PropNameID::forAscii(jsiRuntime, "gdkCreateNewInstance"),
                                                                       1,
                                                                       [&](jsi::Runtime& runtime,
                                                                          const jsi::Value& thisValue,
                                                                          const jsi::Value* arguments,
                                                                          size_t count) -> jsi::Value {
      auto instance = std::make_shared<GdkHostObject>(dir);
      return jsi::Object::createFromHostObject(runtime, instance);
    });
    jsiRuntime.global().setProperty(jsiRuntime, "gdkCreateNewInstance", std::move(gdkCreateNewInstance));
}

extern "C"
JNIEXPORT void JNICALL
Java_com_gdk_GdkModule_nativeInstall(JNIEnv *env, jobject clazz, jlong jsiPtr, jstring dir) {
    char* cString = jstringToChar(env, dir);
    auto runtime = reinterpret_cast<jsi::Runtime*>(jsiPtr);
    if (runtime) {
        install(*runtime, cString);
    }
}
