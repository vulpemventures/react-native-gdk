//
//  GdkHostObject.cpp
//  react-native-gdk
//
//  Created by Mattia Ferrari on 04/02/24.
//

#include "GdkHostObject.hpp"

using json = nlohmann::json;



GdkHostObject::GdkHostObject(std::string dirUrl) {
    sessionDirectoryUrl = dirUrl;
}

GdkHostObject::~GdkHostObject() {
    GA_destroy_session(session);
}


std::vector<jsi::PropNameID> GdkHostObject::getPropertyNames(jsi::Runtime& rt) {
    std::vector<jsi::PropNameID> result;
    // add functions here
    result.push_back(jsi::PropNameID::forUtf8(rt, std::string("generateMnemonic12")));
    result.push_back(jsi::PropNameID::forUtf8(rt, std::string("init")));
    result.push_back(jsi::PropNameID::forUtf8(rt, std::string("createSession")));
    result.push_back(jsi::PropNameID::forUtf8(rt, std::string("connect")));
    result.push_back(jsi::PropNameID::forUtf8(rt, std::string("register")));
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
    
    if (propName == "init") {
            return jsi::Function::createFromHostFunction(runtime,
                                                         jsi::PropNameID::forAscii(runtime, funcName),
                                                         1,
                                                         [this](jsi::Runtime& runtime,
                                                                const jsi::Value& thisValue,
                                                                const jsi::Value* arguments,
                                                                size_t count) -> jsi::Value {
                
            GA_json *configs;
            json obj;
                
            jsi::String logLvel = arguments[0].getString(runtime);
                
            std::string url(sessionDirectoryUrl);

            obj["datadir"] = url;
            obj["log_level"] = logLvel.utf8(runtime);
            
            utils::wrapCall(GA_convert_string_to_json(obj.dump().c_str(), &configs), runtime);
            utils::wrapCall(GA_init(configs), runtime);
            
            GA_destroy_json(configs);

            return jsi::Value::undefined();
        });
    }
    
    if (propName == "createSession") {
            return jsi::Function::createFromHostFunction(runtime,
                                                         jsi::PropNameID::forAscii(runtime, funcName),
                                                         0,
                                                         [this](jsi::Runtime& runtime,
                                                                const jsi::Value& thisValue,
                                                                const jsi::Value* arguments,
                                                                size_t count) -> jsi::Value {
            
                
            if (session != NULL) {
                GA_destroy_session(session);
            }
                
            utils::wrapCall(GA_create_session(&session), runtime);
            return jsi::Value::undefined();
        });
    }
    
    if (propName == "connect") {
            return jsi::Function::createFromHostFunction(runtime,
                                                         jsi::PropNameID::forAscii(runtime, funcName),
                                                         2,
                                                         [this](jsi::Runtime& runtime,
                                                                const jsi::Value& thisValue,
                                                                const jsi::Value* arguments,
                                                                size_t count) -> jsi::Value {
            
                
                GA_json *netParams;
                json obj;
                    
                jsi::String network = arguments[0].getString(runtime);
                jsi::String userAgent = arguments[1].getString(runtime);
                    

                obj["name"] = network.utf8(runtime);
                obj["user_agent"] = network.utf8(runtime);
                obj["use_tor"] = false;
                
                utils::wrapCall(GA_convert_string_to_json(obj.dump().c_str(), &netParams), runtime);
                utils::wrapCall(GA_connect(session, netParams), runtime);
                
                GA_destroy_json(netParams);

                return jsi::Value::undefined();
        });
    }
    
    if (propName == "register") {
            return jsi::Function::createFromHostFunction(runtime,
                                                         jsi::PropNameID::forAscii(runtime, funcName),
                                                         2,
                                                         [this](jsi::Runtime& runtime,
                                                                const jsi::Value& thisValue,
                                                                const jsi::Value* arguments,
                                                                size_t count) -> jsi::Value {
            
                GA_auth_handler *call;
                
                
                GA_json *hw_device_json;
                jsi::Value hw_device = arguments[0].getObject(runtime);
                std::string hw_device_string = utils::stringify(runtime, hw_device);
                utils::wrapCall(GA_convert_string_to_json(hw_device_string.c_str(), &hw_device_json), runtime);
                                
                GA_json *details_json;
                jsi::Value details = arguments[1].getObject(runtime);
                std::string details_string = utils::stringify(runtime, details);
                utils::wrapCall(GA_convert_string_to_json(details_string.c_str(), &details_json), runtime);
                                
                utils::wrapCall(GA_register_user(session, hw_device_json, details_json, &call), runtime);
                
                TwoFactorCall twoFactorCall(call, runtime);
                
                jsi::Object res = utils::resolve(twoFactorCall);
                
                return res;
        });
    }
    
    return jsi::Value::undefined();
}
