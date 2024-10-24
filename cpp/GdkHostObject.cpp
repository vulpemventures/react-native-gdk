//
//  GdkHostObject.cpp
//  react-native-gdk
//
//  Created by Mattia Ferrari on 04/02/24.
//

#include "GdkHostObject.hpp"

using json = nlohmann::json;


GdkHostObject::GdkHostObject(std::string dirUrl, jsi::Runtime &runtime, std::shared_ptr<react::CallInvoker> jsCallInvoker): invoker(jsCallInvoker), rt(runtime) {
    sessionDirectoryUrl = dirUrl;
    pool = std::make_shared<ThreadPool>();

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
    result.push_back(jsi::PropNameID::forUtf8(rt, std::string("getBalance")));
    result.push_back(jsi::PropNameID::forUtf8(rt, std::string("refresh")));
    result.push_back(jsi::PropNameID::forUtf8(rt, std::string("getFeeEstimates")));
    result.push_back(jsi::PropNameID::forUtf8(rt, std::string("getPreviousAddresses")));
    result.push_back(jsi::PropNameID::forUtf8(rt, std::string("getMnemonic")));
    result.push_back(jsi::PropNameID::forUtf8(rt, std::string("setPin")));
    result.push_back(jsi::PropNameID::forUtf8(rt, std::string("getTransactionDetails")));
    result.push_back(jsi::PropNameID::forUtf8(rt, std::string("createTransaction")));
    result.push_back(jsi::PropNameID::forUtf8(rt, std::string("blindTransaction")));
    result.push_back(jsi::PropNameID::forUtf8(rt, std::string("signTransaction")));
    result.push_back(jsi::PropNameID::forUtf8(rt, std::string("sendTransaction")));
    result.push_back(jsi::PropNameID::forUtf8(rt, std::string("broadcastTransaction")));
    result.push_back(jsi::PropNameID::forUtf8(rt, std::string("getNetworks")));
    result.push_back(jsi::PropNameID::forUtf8(rt, std::string("signPsbt")));
    result.push_back(jsi::PropNameID::forUtf8(rt, std::string("registerNetwork")));
    result.push_back(jsi::PropNameID::forUtf8(rt, std::string("destroySession")));
    return result;
}


jsi::Value GdkHostObject::get(jsi::Runtime& runtime, const jsi::PropNameID& propNameId) {
    auto propName = propNameId.utf8(runtime);
    auto funcName = "gdk." + propName;

  if (propName == "destroySession") {
    return jsi::Function::createFromHostFunction(runtime,
                                                 jsi::PropNameID::forAscii(runtime, funcName),
                                                 0,
                                                 [this](jsi::Runtime& runtime,
                                                        const jsi::Value& thisValue,
                                                        const jsi::Value* arguments,
                                                        size_t count) -> jsi::Value {

      GA_set_notification_handler(session, nullptr, nullptr);
      utils::wrapCall(GA_destroy_session(session));


      return jsi::Value::undefined();
    });
  }
  
    if (propName == "generateMnemonic12") {
        return jsi::Function::createFromHostFunction(runtime,
                                                         jsi::PropNameID::forAscii(runtime, funcName),
                                                         0,
                                                         [this](jsi::Runtime& runtime,
                                                                const jsi::Value& thisValue,
                                                                const jsi::Value* arguments,
                                                                size_t count) -> jsi::Value {
            char *buffer;
            utils::wrapCall(GA_generate_mnemonic_12(&buffer));
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

            utils::wrapCall(GA_convert_string_to_json(obj.dump().c_str(), &configs));
            utils::wrapCall(GA_init(configs));

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

            utils::wrapCall(GA_create_session(&session));

            utils::wrapCall(GA_set_notification_handler(session, utils::notificationsHandler, this));

            return jsi::Value::undefined();
        });
    }

    if (propName == "getNetworks") {
            return jsi::Function::createFromHostFunction(runtime,
                                                         jsi::PropNameID::forAscii(runtime, funcName),
                                                         0,
                                                         [this](jsi::Runtime& runtime,
                                                                const jsi::Value& thisValue,
                                                                const jsi::Value* arguments,
                                                                size_t count) -> jsi::Value {

            utils::Promised func = [=](jsi::Runtime& rt, std::shared_ptr<utils::Promise> p){

                auto task = [=, &rt](){
                    try {
                        GA_json *networks;
                        utils::wrapCall(GA_get_networks(&networks));

                        std::shared_ptr<react::CallInvoker> c = invoker.lock();
                        c->invokeAsync([=, &rt] {
                            jsi::Value res = utils::GAJsonToObject(rt, networks);

                            GA_destroy_json(networks);
                            p->resolve(res);
                        });
                    } catch (utils::Exception e) {
                        std::shared_ptr<react::CallInvoker> c = invoker.lock();
                        c->invokeAsync([=] { p->reject(e.what()); });
                    }

                };

                pool->queueWork(task);

            };

            return utils::makePromise(runtime, func);
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

                utils::wrapCall(GA_convert_string_to_json(obj.dump().c_str(), &netParams));
                utils::wrapCall(GA_connect(session, netParams));

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

                GA_json *hw_device_json;
                utils::jsiValueJsonToGAJson(runtime, arguments[0].getObject(runtime), &hw_device_json);


                GA_json *details_json;
                utils::jsiValueJsonToGAJson(runtime, arguments[1].getObject(runtime), &details_json);

                utils::Promised func = [=](jsi::Runtime& rt, std::shared_ptr<utils::Promise> p){

                    auto task = [=, &rt](){
                        try {
                            GA_auth_handler *call;
                            utils::wrapCall(GA_register_user(session, hw_device_json, details_json, &call));

                            json res = utils::resolve(call);

                            GA_destroy_json(hw_device_json);
                            GA_destroy_json(details_json);

                            std::shared_ptr<react::CallInvoker> c = invoker.lock();
                            c->invokeAsync([=, &rt] {
                                if (res.contains("result")) {
                                    p->resolve(utils::parse(rt, res["result"].dump()));
                                } else {
                                    p->reject(res["error"].dump());
                                }
                            });
                        } catch (utils::Exception e) {
                            GA_destroy_json(hw_device_json);
                            GA_destroy_json(details_json);
                            std::shared_ptr<react::CallInvoker> c = invoker.lock();
                            c->invokeAsync([=] { p->reject(e.what()); });
                        }

                    };

                    pool->queueWork(task);

                };


                return utils::makePromise(runtime, func);
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

                GA_json *hw_device_json;
                utils::jsiValueJsonToGAJson(runtime, arguments[0].getObject(runtime), &hw_device_json);


                GA_json *details_json;
                utils::jsiValueJsonToGAJson(runtime, arguments[1].getObject(runtime), &details_json);

                utils::Promised func = [=](jsi::Runtime& rt, std::shared_ptr<utils::Promise> p){

                    auto task = [=, &rt](){
                        try {
                            GA_auth_handler *call;
                            utils::wrapCall(GA_login_user(session, hw_device_json, details_json, &call));

                            json res = utils::resolve(call);

                            GA_destroy_json(hw_device_json);
                            GA_destroy_json(details_json);

                            std::shared_ptr<react::CallInvoker> c = invoker.lock();
                            c->invokeAsync([=, &rt] {
                                if (res.contains("result")) {
                                    p->resolve(utils::parse(rt, res["result"].dump()));
                                } else {
                                    p->reject(res["error"].dump());
                                }
                            });
                        } catch (utils::Exception e) {
                            GA_destroy_json(hw_device_json);
                            GA_destroy_json(details_json);
                            std::shared_ptr<react::CallInvoker> c = invoker.lock();
                            c->invokeAsync([=] { p->reject(e.what()); });
                        }

                    };

                    pool->queueWork(task);

                };


                return utils::makePromise(runtime, func);
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


                GA_json *details;
                utils::jsiValueJsonToGAJson(runtime, arguments[0].getObject(runtime), &details);

                utils::Promised func = [=](jsi::Runtime& rt, std::shared_ptr<utils::Promise> p){

                    auto task = [=, &rt](){
                        try {
                            GA_auth_handler *call;
                            utils::wrapCall(GA_get_subaccounts(session, details, &call));

                            json res = utils::resolve(call);

                            GA_destroy_json(details);

                            std::shared_ptr<react::CallInvoker> c = invoker.lock();
                            c->invokeAsync([=, &rt] {
                                if (res.contains("result")) {
                                    p->resolve(utils::parse(rt, res["result"].dump()));
                                } else {
                                    p->reject(res["error"].dump());
                                }
                            });
                        } catch (utils::Exception e) {
                            GA_destroy_json(details);
                            std::shared_ptr<react::CallInvoker> c = invoker.lock();
                            c->invokeAsync([=] { p->reject(e.what()); });
                        }

                    };

                    pool->queueWork(task);

                };

                return utils::makePromise(runtime, func);
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
                GA_json *details;
                utils::jsiValueJsonToGAJson(rt, arguments[0].getObject(rt), &details);
                utils::Promised func = [=](jsi::Runtime& rt, std::shared_ptr<utils::Promise> p){

                    auto task = [=, &rt](){
                        try {
                            GA_auth_handler *call;
                            utils::wrapCall(GA_create_subaccount(session, details, &call));
                            json res = utils::resolve(call);

                            GA_destroy_json(details);

                            std::shared_ptr<react::CallInvoker> c = invoker.lock();
                            c->invokeAsync([=, &rt] {
                                if (res.contains("result")) {
                                    p->resolve(utils::parse(rt, res["result"].dump()));
                                } else {
                                    p->reject(res["error"].dump());
                                }
                            });
                        } catch (utils::Exception e) {
                            GA_destroy_json(details);
                            std::shared_ptr<react::CallInvoker> c = invoker.lock();
                            c->invokeAsync([=] { p->reject(e.what()); });
                        }

                    };

                    pool->queueWork(task);

                };

                return utils::makePromise(runtime, func);
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

                GA_json *details;
                utils::jsiValueJsonToGAJson(runtime, arguments[0].getObject(runtime), &details);

                utils::Promised func = [=](jsi::Runtime& rt, std::shared_ptr<utils::Promise> p){

                    auto task = [=, &rt](){
                        try {
                            GA_auth_handler *call;
                            utils::wrapCall(GA_get_receive_address(session, details, &call));
                            json res = utils::resolve(call);

                            GA_destroy_json(details);

                            std::shared_ptr<react::CallInvoker> c = invoker.lock();
                            c->invokeAsync([=, &rt] {
                                if (res.contains("result")) {
                                    p->resolve(utils::parse(rt, res["result"].dump()));
                                } else {
                                    p->reject(res["error"].dump());
                                }
                            });
                        } catch (utils::Exception e) {
                            GA_destroy_json(details);
                            std::shared_ptr<react::CallInvoker> c = invoker.lock();
                            c->invokeAsync([=] { p->reject(e.what()); });
                        }

                    };

                    pool->queueWork(task);

                };


                return utils::makePromise(runtime, func);
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

                utils::wrapCall(GA_validate_mnemonic(mnemonic.c_str(), &valid));

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


                GA_json *details;
                utils::jsiValueJsonToGAJson(runtime, arguments[0].getObject(runtime), &details);

                utils::Promised func = [=](jsi::Runtime& rt, std::shared_ptr<utils::Promise> p){

                    auto task = [=, &rt](){
                        try {
                            GA_auth_handler *call;
                            utils::wrapCall(GA_get_transactions(session, details, &call));
                            json res = utils::resolve(call);

                            GA_destroy_json(details);

                            std::shared_ptr<react::CallInvoker> c = invoker.lock();
                            c->invokeAsync([=, &rt] {
                                if (res.contains("result")) {
                                    p->resolve(utils::parse(rt, res["result"].dump()));
                                } else {
                                    p->reject(res["error"].dump());
                                }
                            });
                        } catch (utils::Exception e) {
                            GA_destroy_json(details);
                            std::shared_ptr<react::CallInvoker> c = invoker.lock();
                            c->invokeAsync([=] { p->reject(e.what()); });
                        }

                    };

                    pool->queueWork(task);

                };

                return utils::makePromise(runtime, func);
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


                GA_json *details;
                utils::jsiValueJsonToGAJson(runtime, arguments[0].getObject(runtime), &details);

                utils::Promised func = [=](jsi::Runtime& rt, std::shared_ptr<utils::Promise> p){

                    auto task = [=, &rt](){
                        try {
                            GA_auth_handler *call;
                            utils::wrapCall(GA_get_unspent_outputs(session, details, &call));
                            json res = utils::resolve(call);
                            GA_destroy_json(details);

                            std::shared_ptr<react::CallInvoker> c = invoker.lock();
                            c->invokeAsync([=, &rt] {
                                if (res.contains("result")) {
                                    p->resolve(utils::parse(rt, res["result"].dump()));
                                } else {
                                    p->reject(res["error"].dump());
                                }
                            });

                        } catch (utils::Exception e) {
                            GA_destroy_json(details);
                            std::shared_ptr<react::CallInvoker> c = invoker.lock();
                            c->invokeAsync([=] { p->reject(e.what()); });
                        }
                    };

                    pool->queueWork(task);

                };

                return utils::makePromise(runtime, func);
        });
    }

    if (propName == "getBalance") {
            return jsi::Function::createFromHostFunction(runtime,
                                                         jsi::PropNameID::forAscii(runtime, funcName),
                                                         1,
                                                         [this](jsi::Runtime& runtime,
                                                                const jsi::Value& thisValue,
                                                                const jsi::Value* arguments,
                                                                size_t count) -> jsi::Value {


                GA_json *details;
                utils::jsiValueJsonToGAJson(runtime, arguments[0].getObject(runtime), &details);

                utils::Promised func = [=](jsi::Runtime& rt, std::shared_ptr<utils::Promise> p){

                    auto task = [=, &rt](){
                        try {
                            GA_auth_handler *call;
                            utils::wrapCall(GA_get_balance(session, details, &call));
                            json res = utils::resolve(call);
                            GA_destroy_json(details);

                            std::shared_ptr<react::CallInvoker> c = invoker.lock();
                            c->invokeAsync([=, &rt] {
                                if (res.contains("result")) {
                                    p->resolve(utils::parse(rt, res["result"].dump()));
                                } else {
                                    p->reject(res["error"].dump());
                                }
                            });

                        } catch (utils::Exception e) {
                            GA_destroy_json(details);
                            std::shared_ptr<react::CallInvoker> c = invoker.lock();
                            c->invokeAsync([=] { p->reject(e.what()); });
                        }
                    };

                    pool->queueWork(task);

                };

                return utils::makePromise(runtime, func);
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

            utils::wrapCall(GA_convert_string_to_json(obj.dump().c_str(), &configs));
            utils::wrapCall(GA_refresh_assets(session, configs));

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

                utils::Promised func = [=](jsi::Runtime& rt, std::shared_ptr<utils::Promise> p){

                    auto task = [=, &rt](){
                        try {
                            GA_json *estimates;
                            utils::wrapCall(GA_get_fee_estimates(session, &estimates));

                            std::shared_ptr<react::CallInvoker> c = invoker.lock();
                            c->invokeAsync([=, &rt] {
                                jsi::Value res = utils::GAJsonToObject(rt, estimates);

                                GA_destroy_json(estimates);
                                p->resolve(res);
                            });
                        } catch (utils::Exception e) {
                            std::shared_ptr<react::CallInvoker> c = invoker.lock();
                            c->invokeAsync([=] { p->reject(e.what()); });
                        }

                    };

                    pool->queueWork(task);

                };

                return utils::makePromise(runtime, func);
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

            GA_json *details;
            utils::jsiValueJsonToGAJson(runtime, arguments[0].getObject(runtime), &details);

            utils::Promised func = [=](jsi::Runtime& rt, std::shared_ptr<utils::Promise> p){

                auto task = [=, &rt](){
                    try {
                        GA_auth_handler *call;
                        utils::wrapCall(GA_get_previous_addresses(session, details, &call));
                        json res = utils::resolve(call);
                        GA_destroy_json(details);

                        std::shared_ptr<react::CallInvoker> c = invoker.lock();
                        c->invokeAsync([=, &rt] {
                            if (res.contains("result")) {
                                p->resolve(utils::parse(rt, res["result"].dump()));
                            } else {
                                p->reject(res["error"].dump());
                            }
                        });

                    } catch (utils::Exception e) {
                        GA_destroy_json(details);
                        std::shared_ptr<react::CallInvoker> c = invoker.lock();
                        c->invokeAsync([=] { p->reject(e.what()); });
                    }
                };

                pool->queueWork(task);

            };


            return utils::makePromise(runtime, func);
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

            GA_json *details;
            utils::jsiValueJsonToGAJson(runtime, arguments[0].getObject(runtime), &details);


            utils::Promised func = [=](jsi::Runtime& rt, std::shared_ptr<utils::Promise> p){

                auto task = [=, &rt](){
                    try {
                        GA_auth_handler *call;
                        utils::wrapCall(GA_get_credentials(session, details, &call));
                        json res = utils::resolve(call);
                        GA_destroy_json(details);

                        std::shared_ptr<react::CallInvoker> c = invoker.lock();
                        c->invokeAsync([=, &rt] {
                            if (res.contains("result")) {
                                p->resolve(utils::parse(rt, res["result"].dump()));
                            } else {
                                p->reject(res["error"].dump());
                            }
                        });

                    } catch (utils::Exception e) {
                        GA_destroy_json(details);
                        std::shared_ptr<react::CallInvoker> c = invoker.lock();
                        c->invokeAsync([=] { p->reject(e.what()); });
                    }
                };

                pool->queueWork(task);

            };

            return utils::makePromise(runtime, func);
        });
    }

    if (propName == "setPin") {
            return jsi::Function::createFromHostFunction(runtime,
                                                         jsi::PropNameID::forAscii(runtime, funcName),
                                                         1,
                                                         [this](jsi::Runtime& runtime,
                                                                const jsi::Value& thisValue,
                                                                const jsi::Value* arguments,
                                                                size_t count) -> jsi::Value {

            GA_json *details;
            utils::jsiValueJsonToGAJson(runtime, arguments[0].getObject(runtime), &details);


            utils::Promised func = [=](jsi::Runtime& rt, std::shared_ptr<utils::Promise> p){

                auto task = [=, &rt](){
                    try {
                        GA_auth_handler *call;
                        utils::wrapCall(GA_encrypt_with_pin(session, details, &call));
                        json res = utils::resolve(call);
                        GA_destroy_json(details);

                        std::shared_ptr<react::CallInvoker> c = invoker.lock();
                        c->invokeAsync([=, &rt] {
                            if (res.contains("result")) {
                                p->resolve(utils::parse(rt, res["result"].dump()));
                            } else {
                                p->reject(res["error"].dump());
                            }
                        });

                    } catch (utils::Exception e) {
                        GA_destroy_json(details);
                        std::shared_ptr<react::CallInvoker> c = invoker.lock();
                        c->invokeAsync([=] { p->reject(e.what()); });
                    }
                };

                pool->queueWork(task);

            };

            return utils::makePromise(runtime, func);
        });
    }

    if (propName == "getTransactionDetails") {
            return jsi::Function::createFromHostFunction(runtime,
                                                         jsi::PropNameID::forAscii(runtime, funcName),
                                                         1,
                                                         [this](jsi::Runtime& runtime,
                                                                const jsi::Value& thisValue,
                                                                const jsi::Value* arguments,
                                                                size_t count) -> jsi::Value {
            std::string hash = arguments[0].getString(runtime).utf8(runtime);

            utils::Promised func = [=](jsi::Runtime& rt, std::shared_ptr<utils::Promise> p){

                auto task = [=, &rt](){
                    try {
                        GA_json *details;
                        utils::wrapCall(GA_get_transaction_details(session, hash.c_str(), &details));

                        std::shared_ptr<react::CallInvoker> c = invoker.lock();
                        c->invokeAsync([=, &rt] {
                            jsi::Value res = utils::GAJsonToObject(rt, details);

                            GA_destroy_json(details);
                            p->resolve(res);
                        });

                    } catch (utils::Exception e) {
                        std::shared_ptr<react::CallInvoker> c = invoker.lock();
                        c->invokeAsync([=] { p->reject(e.what()); });
                    }
                };

                pool->queueWork(task);

            };

            return utils::makePromise(runtime, func);
        });
    }

    if (propName == "createTransaction") {
            return jsi::Function::createFromHostFunction(runtime,
                                                         jsi::PropNameID::forAscii(runtime, funcName),
                                                         1,
                                                         [this](jsi::Runtime& runtime,
                                                                const jsi::Value& thisValue,
                                                                const jsi::Value* arguments,
                                                                size_t count) -> jsi::Value {

            GA_json *details;
            utils::jsiValueJsonToGAJson(runtime, arguments[0].getObject(runtime), &details);


            utils::Promised func = [=](jsi::Runtime& rt, std::shared_ptr<utils::Promise> p){

                auto task = [=, &rt](){
                    try {
                        GA_auth_handler *call;
                        utils::wrapCall(GA_create_transaction(session, details, &call));
                        json res = utils::resolve(call);

                        GA_destroy_json(details);

                        std::shared_ptr<react::CallInvoker> c = invoker.lock();
                        c->invokeAsync([=, &rt] {
                            if (res.contains("result")) {
                                json result = res["result"];
                                // in transaction there is an error field that will be an empty string if all ok, else error.
                                if (result["error"] != "") {
                                    p->reject(result["error"].dump());
                                } else {
                                    p->resolve(utils::parse(rt, result.dump()));
                                }
                            } else {
                                p->reject(res["error"].dump());
                            }
                        });

                    } catch (utils::Exception e) {
                        GA_destroy_json(details);
                        std::shared_ptr<react::CallInvoker> c = invoker.lock();
                        c->invokeAsync([=] { p->reject(e.what()); });
                    }
                };

                pool->queueWork(task);

            };

            return utils::makePromise(runtime, func);
        });
    }

    if (propName == "blindTransaction") {
            return jsi::Function::createFromHostFunction(runtime,
                                                         jsi::PropNameID::forAscii(runtime, funcName),
                                                         1,
                                                         [this](jsi::Runtime& runtime,
                                                                const jsi::Value& thisValue,
                                                                const jsi::Value* arguments,
                                                                size_t count) -> jsi::Value {

            GA_json *details;
            utils::jsiValueJsonToGAJson(runtime, arguments[0].getObject(runtime), &details);


            utils::Promised func = [=](jsi::Runtime& rt, std::shared_ptr<utils::Promise> p){

                auto task = [=, &rt](){
                    try {
                        GA_auth_handler *call;
                        utils::wrapCall(GA_blind_transaction(session, details, &call));
                        json res = utils::resolve(call);
                        GA_destroy_json(details);

                        std::shared_ptr<react::CallInvoker> c = invoker.lock();
                        c->invokeAsync([=, &rt] {
                            if (res.contains("result")) {
                                p->resolve(utils::parse(rt, res["result"].dump()));
                            } else {
                                p->reject(res["error"].dump());
                            }
                        });

                    } catch (utils::Exception e) {
                        GA_destroy_json(details);
                        std::shared_ptr<react::CallInvoker> c = invoker.lock();
                        c->invokeAsync([=] { p->reject(e.what()); });
                    }
                };

                pool->queueWork(task);

            };

            return utils::makePromise(runtime, func);
        });
    }

    if (propName == "signTransaction") {
            return jsi::Function::createFromHostFunction(runtime,
                                                         jsi::PropNameID::forAscii(runtime, funcName),
                                                         1,
                                                         [this](jsi::Runtime& runtime,
                                                                const jsi::Value& thisValue,
                                                                const jsi::Value* arguments,
                                                                size_t count) -> jsi::Value {

            GA_json *details;
            utils::jsiValueJsonToGAJson(runtime, arguments[0].getObject(runtime), &details);


            utils::Promised func = [=](jsi::Runtime& rt, std::shared_ptr<utils::Promise> p){

                auto task = [=, &rt](){
                    try {
                        GA_auth_handler *call;
                        utils::wrapCall(GA_sign_transaction(session, details, &call));
                        json res = utils::resolve(call);
                        GA_destroy_json(details);

                        std::shared_ptr<react::CallInvoker> c = invoker.lock();
                        c->invokeAsync([=, &rt] {
                            if (res.contains("result")) {
                                p->resolve(utils::parse(rt, res["result"].dump()));
                            } else {
                                p->reject(res["error"].dump());
                            }
                        });

                    } catch (utils::Exception e) {
                        GA_destroy_json(details);
                        std::shared_ptr<react::CallInvoker> c = invoker.lock();
                        c->invokeAsync([=] { p->reject(e.what()); });
                    }
                };

                pool->queueWork(task);

            };

            return utils::makePromise(runtime, func);
        });
    }

    if (propName == "sendTransaction") {
            return jsi::Function::createFromHostFunction(runtime,
                                                         jsi::PropNameID::forAscii(runtime, funcName),
                                                         1,
                                                         [this](jsi::Runtime& runtime,
                                                                const jsi::Value& thisValue,
                                                                const jsi::Value* arguments,
                                                                size_t count) -> jsi::Value {

            GA_json *details;
            utils::jsiValueJsonToGAJson(runtime, arguments[0].getObject(runtime), &details);


            utils::Promised func = [=](jsi::Runtime& rt, std::shared_ptr<utils::Promise> p){

                auto task = [=, &rt](){
                    try {
                        GA_auth_handler *call;
                        utils::wrapCall(GA_send_transaction(session, details, &call));
                        json res = utils::resolve(call);
                        GA_destroy_json(details);

                        std::shared_ptr<react::CallInvoker> c = invoker.lock();
                        c->invokeAsync([=, &rt] {
                            if (res.contains("result")) {
                                p->resolve(utils::parse(rt, res["result"].dump()));
                            } else {
                                p->reject(res["error"].dump());
                            }
                        });

                    } catch (utils::Exception e) {
                        GA_destroy_json(details);
                        std::shared_ptr<react::CallInvoker> c = invoker.lock();
                        c->invokeAsync([=] { p->reject(e.what()); });
                    }
                };

                pool->queueWork(task);

            };

            return utils::makePromise(runtime, func);
        });
    }

    if (propName == "signPsbt") {
            return jsi::Function::createFromHostFunction(runtime,
                                                         jsi::PropNameID::forAscii(runtime, funcName),
                                                         1,
                                                         [this](jsi::Runtime& runtime,
                                                                const jsi::Value& thisValue,
                                                                const jsi::Value* arguments,
                                                                size_t count) -> jsi::Value {

            GA_json *details;
            utils::jsiValueJsonToGAJson(runtime, arguments[0].getObject(runtime), &details);


            utils::Promised func = [=](jsi::Runtime& rt, std::shared_ptr<utils::Promise> p){

                auto task = [=, &rt](){
                    try {
                        GA_auth_handler *call;
                        utils::wrapCall(GA_psbt_sign(session, details, &call));
                        json res = utils::resolve(call);
                        GA_destroy_json(details);

                        std::shared_ptr<react::CallInvoker> c = invoker.lock();
                        c->invokeAsync([=, &rt] {
                            if (res.contains("result")) {
                                p->resolve(utils::parse(rt, res["result"].dump()));
                            } else {
                                p->reject(res["error"].dump());
                            }
                        });

                    } catch (utils::Exception e) {
                        GA_destroy_json(details);
                        std::shared_ptr<react::CallInvoker> c = invoker.lock();
                        c->invokeAsync([=] { p->reject(e.what()); });
                    }
                };

                pool->queueWork(task);

            };

            return utils::makePromise(runtime, func);
        });
    }

    if (propName == "broadcastTransaction") {
            return jsi::Function::createFromHostFunction(runtime,
                                                         jsi::PropNameID::forAscii(runtime, funcName),
                                                         1,
                                                         [this](jsi::Runtime& runtime,
                                                                const jsi::Value& thisValue,
                                                                const jsi::Value* arguments,
                                                                size_t count) -> jsi::Value {

            std::string txhex = arguments[0].getString(runtime).utf8(runtime);

            utils::Promised func = [=](jsi::Runtime& rt, std::shared_ptr<utils::Promise> p){

                auto task = [=, &rt](){
                    try {
                        char* txHash;
                        utils::wrapCall(GA_broadcast_transaction(session, txhex.c_str(), &txHash));

                        std::shared_ptr<react::CallInvoker> c = invoker.lock();
                        c->invokeAsync([=, &rt] {
                            std::string hash(txHash);
                            jsi::Value res = jsi::String::createFromUtf8(rt, hash);
                            p->resolve(res);
                        });

                    } catch (utils::Exception e) {
                        std::shared_ptr<react::CallInvoker> c = invoker.lock();
                        c->invokeAsync([=] { p->reject(e.what()); });
                    }
                };

                pool->queueWork(task);

            };

            return utils::makePromise(runtime, func);
        });
    }

    if (propName == "registerNetwork") {
            return jsi::Function::createFromHostFunction(runtime,
                                                         jsi::PropNameID::forAscii(runtime, funcName),
                                                         1,
                                                         [this](jsi::Runtime& runtime,
                                                                const jsi::Value& thisValue,
                                                                const jsi::Value* arguments,
                                                                size_t count) -> jsi::Value {

            GA_json *network_details;
            utils::jsiValueJsonToGAJson(runtime, arguments[1].getObject(runtime), &network_details);
            std::string name = arguments[0].getString(runtime).utf8(runtime);




            utils::wrapCall(GA_register_network(name.c_str(), network_details));

//            GA_destroy_json(network_details);

            return jsi::Value::undefined();
        });
    }

    return jsi::Value::undefined();
}
