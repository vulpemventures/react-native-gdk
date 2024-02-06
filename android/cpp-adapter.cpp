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

    jsiRuntime.global().setProperty(jsiRuntime, "sessionDir", jsi::String::createFromUtf8(jsiRuntime, dir.c_str()));

    auto gdkCreateNewInstance = jsi::Function::createFromHostFunction(jsiRuntime,
                                                                       jsi::PropNameID::forAscii(jsiRuntime, "gdkCreateNewInstance"),
                                                                       0,
                                                                       [&](jsi::Runtime& runtime,
                                                                          const jsi::Value& thisValue,
                                                                          const jsi::Value* arguments,
                                                                          size_t count) -> jsi::Value {

      jsi::String rawDir = runtime.global().getProperty(runtime, "sessionDir").getString(runtime);

      auto instance = std::make_shared<GdkHostObject>(rawDir.utf8(runtime));
      return jsi::Object::createFromHostObject(runtime, instance);
    });
    jsiRuntime.global().setProperty(jsiRuntime, "gdkCreateNewInstance", std::move(gdkCreateNewInstance));
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
