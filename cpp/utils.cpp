//
//  utils.cpp
//  react-native-gdk
//
//  Created by Mattia Ferrari on 04/02/24.
//

#include "utils.hpp"

namespace utils {


    void getThreadErrorDetails(int errorCode) {
        GA_json *details;
        char *stringJson;
        GA_get_thread_error_details(&details);
        int res = GA_convert_json_value_to_string(details, "details", &stringJson);
        GA_destroy_json(details);
        if (res != GA_OK) {
            throw Exception("Cannot get error details, trying to call GA_convert_json_value_to_string, but returned: " + std::to_string(res));
        }
        
        std::string errorMessage = "";
        switch (errorCode) {
            case GA_RECONNECT:
                errorMessage.append("RECONNECT_ERROR: ");
                break;
            case GA_TIMEOUT:
                errorMessage.append("TIMEOUT_ERROR: ");
                break;
            case GA_NOT_AUTHORIZED:
                errorMessage.append("NOT_AUTHORIZED_ERROR: ");
                break;
            case GA_SESSION_LOST:
                errorMessage.append("SESSION_LOST_ERROR: ");
                break;
            default:
                errorMessage.append("GENERIC_ERROR (" + std::to_string(errorCode) + "): ");
                break;
        }
        
        errorMessage.append(stringJson);
        throw Exception(errorMessage);
    }
    

    void wrapCall(int gdk_function_result) {
        if (gdk_function_result != GA_OK) {
            getThreadErrorDetails(gdk_function_result);
        }
    }

    jsi::Object GAJsonToObject(jsi::Runtime &rt, const GA_json *src) {
        char *stringJson;
        GA_convert_json_to_string(src, &stringJson);
        size_t len = strlen(stringJson);

        jsi::Value val = jsi::Value::createFromJsonUtf8(rt, reinterpret_cast<const uint8_t*>(stringJson), len);

        return val.getObject(rt);
    }

    int consoleLog(jsi::Runtime& runtime, std::string msg) {
            std::string msgCopy(msg);
            jsi::Object globalObject = runtime.global();

            // Access the console object from the global object
            jsi::Object consoleObject = globalObject.getProperty(runtime, "console").getObject(runtime);


            // Get the console.log function
            jsi::Function consoleLog = consoleObject.getProperty(runtime, "log").getObject(runtime).asFunction(runtime);


            // Call console.log with arguments
            consoleLog.call(runtime, msgCopy);
            return 0;
        }

    std::string stringify(jsi::Runtime &runtime, const jsi::Value &value) {
        jsi::Object globalObject = runtime.global();
        
        jsi::Object jsonObject = globalObject.getProperty(runtime, "JSON").getObject(runtime);
        
        jsi::Function stringifyFunction = jsonObject.getProperty(runtime, "stringify").getObject(runtime).asFunction(runtime);

        jsi::Value jsonString = stringifyFunction.call(runtime, value);
        return jsonString.getString(runtime).utf8(runtime);
    }

    jsi::Value parse(jsi::Runtime &runtime, std::string src) {
        jsi::Object globalObject = runtime.global();
        
        jsi::Object jsonObject = globalObject.getProperty(runtime, "JSON").getObject(runtime);
        
        jsi::Function stringifyFunction = jsonObject.getProperty(runtime, "parse").getObject(runtime).asFunction(runtime);
        jsi::Value s = jsi::String::createFromUtf8(runtime, src);
        jsi::Value res = stringifyFunction.call(runtime, s);
        return res;
    }

    json resolve(GA_auth_handler* call) {
        while (true) {
            GA_json *resJson;
            GA_auth_handler_get_status(call, &resJson);
            char *stringJson;
            GA_convert_json_to_string(resJson, &stringJson);
            GA_destroy_json(resJson);
            std::string s(stringJson);
            json obj = json::parse(s);
            std::string status = obj["status"];

            if (status == "call") {
                GA_auth_handler_call(call);
            } else if (status == "done") {
                GA_destroy_auth_handler(call);
                return obj;
            } else {
                GA_destroy_auth_handler(call);
                return obj;
            }
        }
    }

    // transform a jsi value containing a json to string and then to a GA_json
    void jsiValueJsonToGAJson(jsi::Runtime &rt, jsi::Value src, GA_json **dest) {
        std::string srcString = utils::stringify(rt, src);
        utils::wrapCall(GA_convert_string_to_json(srcString.c_str(), dest));
    }


    // this function will be called by gdk internal websocket to emit notifications
    void notificationsHandler(void* ctx, GA_json* details) {
        // we pass the host object as ctx through the void* pointer because we need both js runtime and js callinvoker
        GdkHostObject* instance = static_cast<GdkHostObject*>(ctx);
        
        char *stringJson;
        GA_convert_json_to_string(details, &stringJson);
        GA_destroy_json(details);
        std::string s(stringJson);
        json res = json::parse(s);

        auto connIterator = instance->handler.find(res["event"]);
        if (connIterator != instance->handler.end()) {
            std::shared_ptr<jsi::Function> handle = connIterator->second;
            std::string nativeString(res.dump());

            // to access safely the js runtime from another thread we need to call the runtime within the call invoker invoke async
            std::shared_ptr<react::CallInvoker> i = instance->invoker.lock();
            i->invokeAsync([=] {
                jsi::Value parsed = utils::parse(instance->rt, nativeString);
                handle->call(instance->rt, parsed);
            });
        }
    }

    Promise::Promise(jsi::Runtime &rt, jsi::Function resolve, jsi::Function reject)
    : runtime_(rt), resolve_(std::move(resolve)), reject_(std::move(reject)) {}

    void Promise::resolve(const jsi::Value &result) {
        resolve_.call(runtime_, result);
    }

    void Promise::reject(const std::string &message) {
        jsi::Object error(runtime_);
        error.setProperty(
                          runtime_, "message", jsi::String::createFromUtf8(runtime_, message));
        reject_.call(runtime_, error);
    }

    jsi::Value makePromise(jsi::Runtime &rt,const Promised func) {
        // Grab js global promise object
        jsi::Function JSPromise = rt.global().getPropertyAsFunction(rt, "Promise");
        // function to pass to the promise constructor: new Promise((resolve, reject) => {})
        jsi::Function fn = jsi::Function::createFromHostFunction(
                                                                 rt,
                                                                 jsi::PropNameID::forAscii(rt, "fn"),
                                                                 2, // resolve, reject
                                                                 [func](
                                                                        jsi::Runtime &rt2,
                                                                        const jsi::Value &thisVal,
                                                                        const jsi::Value *args,
                                                                        size_t count) {
            jsi::Function resolve = args[0].getObject(rt2).getFunction(rt2);
            jsi::Function reject = args[1].getObject(rt2).getFunction(rt2);
            auto wrapper = std::make_shared<Promise>(rt2, std::move(resolve), std::move(reject));
            func(rt2, wrapper);
            return jsi::Value::undefined();
        });

        return JSPromise.callAsConstructor(rt, fn);
    }

}
