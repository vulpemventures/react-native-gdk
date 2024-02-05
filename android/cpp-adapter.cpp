#include <jni.h>
#include <jsi/jsi.h>
#include "GdkHostObject.hpp"

using namespace facebook;

void install(jsi::Runtime& jsiRuntime) {
    auto gdkCreateNewInstance = jsi::Function::createFromHostFunction(jsiRuntime,
                                                                       jsi::PropNameID::forAscii(jsiRuntime, "gdkCreateNewInstance"),
                                                                       1,
                                                                       [](jsi::Runtime& runtime,
                                                                          const jsi::Value& thisValue,
                                                                          const jsi::Value* arguments,
                                                                          size_t count) -> jsi::Value {
                                                                         auto instance = std::make_shared<GdkHostObject>();
                                                                         return jsi::Object::createFromHostObject(runtime, instance);
                                                                       });
    jsiRuntime.global().setProperty(jsiRuntime, "gdkCreateNewInstance", std::move(gdkCreateNewInstance));
}

extern "C"
JNIEXPORT void JNICALL
Java_com_gdk_GdkModule_nativeInstall(JNIEnv *env, jobject clazz, jlong jsiPtr) {
    auto runtime = reinterpret_cast<jsi::Runtime*>(jsiPtr);
    if (runtime) {
        install(*runtime);
    }
}
