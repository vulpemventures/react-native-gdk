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


    jsi::Object resolve(TwoFactorCall call) {
        while (true) {
            jsi::Object obj = call.getStatus();
            jsi::Value rawStatus = obj.getProperty(call.rt, "status");
            
            jsi::String status = rawStatus.getString(call.rt);
            
            if (status.utf8(call.rt) == "call") {
                call.call();
            } else if (status.utf8(call.rt) == "done") {
                return obj;
            } else {
                jsi::Value rawError = obj.getProperty(call.rt, "error");
                jsi::String error = rawError.getString(call.rt);
                throw jsi::JSError(call.rt, error.utf8(call.rt));
            }
        }
    }

}
