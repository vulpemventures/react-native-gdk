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
  auto instance = std::make_shared<GdkHostObject>(dir, jsiRuntime);
  jsi::Object GDK = jsi::Object::createFromHostObject(jsiRuntime, instance);
  jsiRuntime.global().setProperty(jsiRuntime, "GDK", std::move(GDK));
}

void setNotificationHandler(jsi::Runtime& rt) {

  jsi::Object GDK = rt.global().getProperty(rt, "GDK").getObject(rt);
  auto i = GDK.getHostObject<GdkHostObject>(rt);


  GA_notification_handler handler = [](void *ctx, GA_json* details) -> void {
    GdkHostObject* instance = static_cast<GdkHostObject*>(ctx);

    jsi::Object val = utils::GAJsonToObject(instance->rt, details);
    jsi::String evt = val.getProperty(instance->rt, "event").asString(instance->rt);

    auto connIterator = instance->handler.find(evt.utf8(instance->rt));

    if (connIterator != instance->handler.end()) {
        std::shared_ptr<jsi::Function> handle = connIterator->second;
        handle->call(instance->rt, val);
    }
  };

  GA_set_notification_handler(i->session, handler, i.get());
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

extern "C"
JNIEXPORT void JNICALL
Java_com_gdk_GdkModule_nativeSetNotificationHandler(JNIEnv *env, jobject clazz, jlong jsiPtr) {
    auto runtime = reinterpret_cast<jsi::Runtime*>(jsiPtr);
    if (runtime) {
        setNotificationHandler(*runtime);
    }
}
