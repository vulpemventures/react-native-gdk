//
//  GdkHostObject.cpp
//  react-native-gdk
//
//  Created by Mattia Ferrari on 04/02/24.
//

#include "GdkHostObject.hpp"

using json = nlohmann::json;


GdkHostObject::GdkHostObject(std::string dirUrl, jsi::Runtime &runtime): rt(runtime) {
    sessionDirectoryUrl = dirUrl;
}

GdkHostObject::~GdkHostObject() {
    GA_set_notification_handler(session, nullptr, nullptr);
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
    result.push_back(jsi::PropNameID::forUtf8(rt, std::string("getSubaccounts")));
    result.push_back(jsi::PropNameID::forUtf8(rt, std::string("addListener")));
    result.push_back(jsi::PropNameID::forUtf8(rt, std::string("removeListener")));
    result.push_back(jsi::PropNameID::forUtf8(rt, std::string("validateMnemonic")));
    result.push_back(jsi::PropNameID::forUtf8(rt, std::string("getTransactions")));
    result.push_back(jsi::PropNameID::forUtf8(rt, std::string("getUnspentOutputs")));
    result.push_back(jsi::PropNameID::forUtf8(rt, std::string("refresh")));
    result.push_back(jsi::PropNameID::forUtf8(rt, std::string("getFeeEstimates")));
    result.push_back(jsi::PropNameID::forUtf8(rt, std::string("getPreviousAddresses")));
    result.push_back(jsi::PropNameID::forUtf8(rt, std::string("getMnemonic")));
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

            utils::wrapCall(GA_create_session(&session), runtime);
                
            utils::wrapCall(GA_set_notification_handler(session, utils::notificationsHandler, this), runtime);
                    
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
                utils::jsiValueJsonToGAJson(runtime, arguments[0].getObject(runtime), &hw_device_json);


                GA_json *details_json;
                utils::jsiValueJsonToGAJson(runtime, arguments[1].getObject(runtime), &details_json);

                utils::wrapCall(GA_register_user(session, hw_device_json, details_json, &call), runtime);

                TwoFactorCall twoFactorCall(call);
                json res = utils::resolve(twoFactorCall);
                return utils::parse(runtime, res.dump());
        });
    }

    if (propName == "login") {
            return jsi::Function::createFromHostFunction(runtime,
                                                         jsi::PropNameID::forAscii(runtime, funcName),
                                                         2,
                                                         [this](jsi::Runtime& runtime,
                                                                const jsi::Value& thisValue,
                                                                const jsi::Value* arguments,
                                                                size_t count) -> jsi::Value {

                GA_auth_handler *call;

                GA_json *hw_device_json;
                utils::jsiValueJsonToGAJson(runtime, arguments[0].getObject(runtime), &hw_device_json);


                GA_json *details_json;
                utils::jsiValueJsonToGAJson(runtime, arguments[1].getObject(runtime), &details_json);


                utils::wrapCall(GA_login_user(session, hw_device_json, details_json, &call), runtime);

                TwoFactorCall twoFactorCall(call);
                json res = utils::resolve(twoFactorCall);
                return utils::parse(runtime, res.dump());
        });
    }

    if (propName == "getSubaccounts") {
            return jsi::Function::createFromHostFunction(runtime,
                                                         jsi::PropNameID::forAscii(runtime, funcName),
                                                         1,
                                                         [this](jsi::Runtime& runtime,
                                                                const jsi::Value& thisValue,
                                                                const jsi::Value* arguments,
                                                                size_t count) -> jsi::Value {

                GA_auth_handler *call;

                GA_json *details;
                utils::jsiValueJsonToGAJson(runtime, arguments[0].getObject(runtime), &details);

                utils::wrapCall(GA_get_subaccounts(session, details, &call), runtime);

                TwoFactorCall twoFactorCall(call);
                json res = utils::resolve(twoFactorCall);
                return utils::parse(runtime, res.dump());
        });
    }
    
    if (propName == "createSubaccount") {
            return jsi::Function::createFromHostFunction(runtime,
                                                         jsi::PropNameID::forAscii(runtime, funcName),
                                                         1,
                                                         [this](jsi::Runtime& runtime,
                                                                const jsi::Value& thisValue,
                                                                const jsi::Value* arguments,
                                                                size_t count) -> jsi::Value {

                GA_auth_handler *call;
                GA_json *details;
                utils::jsiValueJsonToGAJson(runtime, arguments[0].getObject(runtime), &details);
                
                utils::wrapCall(GA_create_subaccount(session, details, &call), runtime);

                TwoFactorCall twoFactorCall(call);
                json res = utils::resolve(twoFactorCall);
                return utils::parse(runtime, res.dump());
        });
    }
    
    if (propName == "getReceiveAddress") {
            return jsi::Function::createFromHostFunction(runtime,
                                                         jsi::PropNameID::forAscii(runtime, funcName),
                                                         1,
                                                         [this](jsi::Runtime& runtime,
                                                                const jsi::Value& thisValue,
                                                                const jsi::Value* arguments,
                                                                size_t count) -> jsi::Value {

                GA_auth_handler *call;
                GA_json *details;
                utils::jsiValueJsonToGAJson(runtime, arguments[0].getObject(runtime), &details);
                
                utils::wrapCall(GA_get_receive_address(session, details, &call), runtime);

                TwoFactorCall twoFactorCall(call);
                json res = utils::resolve(twoFactorCall);
                return utils::parse(runtime, res.dump());
        });
    }
    
    if (propName == "addListener") {
            return jsi::Function::createFromHostFunction(runtime,
                                                         jsi::PropNameID::forAscii(runtime, funcName),
                                                         2,
                                                         [this](jsi::Runtime& runtime,
                                                                const jsi::Value& thisValue,
                                                                const jsi::Value* arguments,
                                                                size_t count) -> jsi::Value {


                jsi::String event = arguments[0].getString(runtime);
                
                std::shared_ptr<jsi::Function> fn = std::make_shared<jsi::Function>(arguments[1].getObject(runtime).asFunction(runtime));
                
                
                handler[event.utf8(runtime)] = fn;
                
                return jsi::Value::undefined();
        });
    }
    
    if (propName == "removeListener") {
            return jsi::Function::createFromHostFunction(runtime,
                                                         jsi::PropNameID::forAscii(runtime, funcName),
                                                         1,
                                                         [this](jsi::Runtime& runtime,
                                                                const jsi::Value& thisValue,
                                                                const jsi::Value* arguments,
                                                                size_t count) -> jsi::Value {


                std::string eventName = arguments[0].getString(runtime).utf8(runtime);
                
                auto it = handler.find(eventName);
                if (it != handler.end()) {
                    handler.erase(it);
                }
                                
                return jsi::Value::undefined();
        });
    }
    
    if (propName == "validateMnemonic") {
            return jsi::Function::createFromHostFunction(runtime,
                                                         jsi::PropNameID::forAscii(runtime, funcName),
                                                         1,
                                                         [this](jsi::Runtime& runtime,
                                                                const jsi::Value& thisValue,
                                                                const jsi::Value* arguments,
                                                                size_t count) -> jsi::Value {


                uint32_t valid = 0;
                
                std::string mnemonic = arguments[0].getString(runtime).utf8(runtime);
                
                utils::wrapCall(GA_validate_mnemonic(mnemonic.c_str(), &valid), runtime);
                
                return jsi::Value(valid == GA_TRUE);
        });
    }
    
    if (propName == "getTransactions") {
            return jsi::Function::createFromHostFunction(runtime,
                                                         jsi::PropNameID::forAscii(runtime, funcName),
                                                         1,
                                                         [this](jsi::Runtime& runtime,
                                                                const jsi::Value& thisValue,
                                                                const jsi::Value* arguments,
                                                                size_t count) -> jsi::Value {


                GA_auth_handler *call;
                GA_json *details;
                utils::jsiValueJsonToGAJson(runtime, arguments[0].getObject(runtime), &details);
                
                utils::wrapCall(GA_get_transactions(session, details, &call), runtime);

                TwoFactorCall twoFactorCall(call);
                json res = utils::resolve(twoFactorCall);
                return utils::parse(runtime, res.dump());
        });
    }
    
    if (propName == "getUnspentOutputs") {
            return jsi::Function::createFromHostFunction(runtime,
                                                         jsi::PropNameID::forAscii(runtime, funcName),
                                                         1,
                                                         [this](jsi::Runtime& runtime,
                                                                const jsi::Value& thisValue,
                                                                const jsi::Value* arguments,
                                                                size_t count) -> jsi::Value {


                GA_auth_handler *call;
                GA_json *details;
                utils::jsiValueJsonToGAJson(runtime, arguments[0].getObject(runtime), &details);
                
                utils::wrapCall(GA_get_unspent_outputs(session, details, &call), runtime);

                TwoFactorCall twoFactorCall(call);
                json res = utils::resolve(twoFactorCall);
                return utils::parse(runtime, res.dump());
        });
    }
    
    if (propName == "refresh") {
            return jsi::Function::createFromHostFunction(runtime,
                                                         jsi::PropNameID::forAscii(runtime, funcName),
                                                         0,
                                                         [this](jsi::Runtime& runtime,
                                                                const jsi::Value& thisValue,
                                                                const jsi::Value* arguments,
                                                                size_t count) -> jsi::Value {

            GA_json *configs;
            json obj;


            obj["icons"] = true;
            obj["assets"] = true;
            obj["refresh"] = true;

            utils::wrapCall(GA_convert_string_to_json(obj.dump().c_str(), &configs), runtime);
            utils::wrapCall(GA_refresh_assets(session, configs), runtime);

            GA_destroy_json(configs);

            return jsi::Value::undefined();
        });
    }
    
    if (propName == "getFeeEstimates") {
            return jsi::Function::createFromHostFunction(runtime,
                                                         jsi::PropNameID::forAscii(runtime, funcName),
                                                         0,
                                                         [this](jsi::Runtime& runtime,
                                                                const jsi::Value& thisValue,
                                                                const jsi::Value* arguments,
                                                                size_t count) -> jsi::Value {

            GA_json *estimates;
            
            utils::wrapCall(GA_get_fee_estimates(session, &estimates), runtime);
            jsi::Object res = utils::GAJsonToObject(runtime, estimates);
            GA_destroy_json(estimates);

            return res;
        });
    }
    
    if (propName == "getPreviousAddresses") {
            return jsi::Function::createFromHostFunction(runtime,
                                                         jsi::PropNameID::forAscii(runtime, funcName),
                                                         1,
                                                         [this](jsi::Runtime& runtime,
                                                                const jsi::Value& thisValue,
                                                                const jsi::Value* arguments,
                                                                size_t count) -> jsi::Value {

            GA_auth_handler *call;
            GA_json *details;
            utils::jsiValueJsonToGAJson(runtime, arguments[0].getObject(runtime), &details);
            
            utils::wrapCall(GA_get_previous_addresses(session, details, &call), runtime);

            TwoFactorCall twoFactorCall(call);
            json res = utils::resolve(twoFactorCall);
            return utils::parse(runtime, res.dump());
        });
    }
    
    if (propName == "getMnemonic") {
            return jsi::Function::createFromHostFunction(runtime,
                                                         jsi::PropNameID::forAscii(runtime, funcName),
                                                         1,
                                                         [this](jsi::Runtime& runtime,
                                                                const jsi::Value& thisValue,
                                                                const jsi::Value* arguments,
                                                                size_t count) -> jsi::Value {

            GA_auth_handler *call;
            GA_json *details;
            utils::jsiValueJsonToGAJson(runtime, arguments[0].getObject(runtime), &details);
            
            utils::wrapCall(GA_get_credentials(session, details, &call), runtime);

            TwoFactorCall twoFactorCall(call);
            json res = utils::resolve(twoFactorCall);
            return utils::parse(runtime, res.dump());
        });
    }

    return jsi::Value::undefined();
}
