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
        throw new jsi::JSError(rt, errorMessage);
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


}
