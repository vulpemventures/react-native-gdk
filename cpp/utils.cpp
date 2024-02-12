//
//  utils.cpp
//  react-native-gdk
//
//  Created by Mattia Ferrari on 04/02/24.
//

#include "utils.hpp"

namespace utils {
    void getThreadErrorDetails(jsi::Runtime &rt, int errorCode) {
        GA_json *details;
        char *stringJson;
        GA_get_thread_error_details(&details);
        int res = GA_convert_json_value_to_string(details, "details", &stringJson);
        GA_destroy_json(details);
        if (res != GA_OK) {
            throw jsi::JSError(rt, "Cannot get error details, trying to call GA_convert_json_value_to_string, but returned: " + std::to_string(res));
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
        throw jsi::JSError(rt, errorMessage);
    }
    

    void wrapCall(int gdk_function_result, jsi::Runtime &rt) {
        if (gdk_function_result != GA_OK) {
            getThreadErrorDetails(rt, gdk_function_result);
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


    json resolve(TwoFactorCall call) {
        while (true) {
            json obj = call.getStatus();
            std::string status = obj["status"];

            if (status == "call") {
                call.call();
            } else if (status == "done") {
                json result = obj["result"];
                return result;
            } else {
                json error = obj["error"];
                throw std::runtime_error(error.dump());
            }
        }
    }

    // transform a jsi value containing a json to string and then to a GA_json
    void jsiValueJsonToGAJson(jsi::Runtime &rt, jsi::Value src, GA_json **dest) {
        std::string srcString = utils::stringify(rt, src);
        utils::wrapCall(GA_convert_string_to_json(srcString.c_str(), dest), rt);
    }


    void notificationsHandler(void* ctx, GA_json* details) {
        GdkHostObject* instance = static_cast<GdkHostObject*>(ctx);
        
        char *stringJson;
        GA_convert_json_to_string(details, &stringJson);
        std::string s(stringJson);
        json res = json::parse(s);

        auto connIterator = instance->handler.find(res["event"]);
        if (connIterator != instance->handler.end()) {
            std::shared_ptr<jsi::Function> handle = connIterator->second;
            std::string nativeString(res.dump());
//            jsi::Value s = jsi::String::createFromUtf8(instance->rt, nativeString);
            handle->call(instance->rt, nativeString);
        }
    }

}
