#import <React/RCTBridge+Private.h>
#import <React/RCTUtils.h>
#import <jsi/jsi.h>
#import "NativeGdk.h"
#import "GdkHostObject.hpp"

@implementation NativeGdk
RCT_EXPORT_MODULE()


RCT_EXPORT_BLOCKING_SYNCHRONOUS_METHOD(install)
{
    RCTBridge* bridge = [RCTBridge currentBridge];
    RCTCxxBridge* cxxBridge = (RCTCxxBridge*)bridge;


    if (cxxBridge == nil) {
        return @false;
    }

    using namespace facebook;

    auto jsiRuntime = (jsi::Runtime*) cxxBridge.runtime;
    if (jsiRuntime == nil) {
        return @false;
    }
    auto& runtime = *jsiRuntime;

    auto gdkCreateNewInstance = jsi::Function::createFromHostFunction(runtime,
                                                                       jsi::PropNameID::forAscii(runtime, "gdkCreateNewInstance"),
                                                                       0,
                                                                         [&](jsi::Runtime& runtime,
                                                                          const jsi::Value& thisValue,
                                                                          const jsi::Value* arguments,
                                                                          size_t count) -> jsi::Value {



        auto instance = std::make_shared<GdkHostObject>();
        return jsi::Object::createFromHostObject(runtime, instance);
    });
    runtime.global().setProperty(runtime, "gdkCreateNewInstance", std::move(gdkCreateNewInstance));

    return @true;
}


@end
