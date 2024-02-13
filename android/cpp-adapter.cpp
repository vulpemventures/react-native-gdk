#include <ReactCommon/CallInvokerHolder.h>
#include <fbjni/fbjni.h>
#include <jni.h>
#include <jsi/jsi.h>
#include "GdkHostObject.hpp"

using namespace facebook;

struct GdkModule : jni::JavaClass<GdkModule> {
  static constexpr auto kJavaDescriptor =
      "Lcom/gdk/GdkModule;";

  static void registerNatives() {
    javaClassStatic()->registerNatives(
        {
         makeNativeMethod("installNativeJsi",
                          GdkModule::installNativeJsi)
        });
  }

private:
  static void installNativeJsi(
      jni::alias_ref<jni::JObject> thiz, jlong jsiRuntimePtr,
      jni::alias_ref<react::CallInvokerHolder::javaobject> jsCallInvokerHolder,
      jni::alias_ref<jni::JString> datadir) {
    auto jsiRuntime = reinterpret_cast<jsi::Runtime *>(jsiRuntimePtr);
    auto jsCallInvoker = jsCallInvokerHolder->cthis()->getCallInvoker();
    std::string dir = datadir->toStdString();

    install(*jsiRuntime, jsCallInvoker, dir);

  }

  static void install(jsi::Runtime& runtime, std::shared_ptr<facebook::react::CallInvoker> jsCallInvoker, std::string datadir) {
    auto instance = std::make_shared<GdkHostObject>(datadir, runtime, jsCallInvoker);
    jsi::Object GDK = jsi::Object::createFromHostObject(runtime, instance);
    runtime.global().setProperty(runtime, "GDK", std::move(GDK));
  }

};

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *) {
  return jni::initialize(vm, [] { GdkModule::registerNatives(); });
}
