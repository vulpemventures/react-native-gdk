//
//  GdkHostObject.cpp
//  react-native-gdk
//
//  Created by Mattia Ferrari on 04/02/24.
//

#include "GdkHostObject.hpp"


GdkHostObject::GdkHostObject() {}

std::vector<jsi::PropNameID> GdkHostObject::getPropertyNames(jsi::Runtime& rt) {
    std::vector<jsi::PropNameID> result;
    // add functions here
    result.push_back(jsi::PropNameID::forUtf8(rt, std::string("generateMnemonic12")));
    return result;
}


jsi::Value GdkHostObject::get(jsi::Runtime& runtime, const jsi::PropNameID& propNameId) {
    auto propName = propNameId.utf8(runtime);
    auto funcName = "gdk." + propName;

    if (propName == "generateMnemonic12") {
        return jsi::Function::createFromHostFunction(runtime,
                                                         jsi::PropNameID::forAscii(runtime, funcName),
                                                         0,
                                                         [this](jsi::Runtime& runtime,
                                                                const jsi::Value& thisValue,
                                                                const jsi::Value* arguments,
                                                                size_t count) -> jsi::Value {
            char *buffer;
            utils::wrapCall(GA_generate_mnemonic_12(&buffer), runtime);
            auto mnemonic = jsi::String::createFromUtf8(runtime, buffer);
            GA_destroy_string(buffer);
            return mnemonic;
        });

    }

    return jsi::Value::undefined();
}
