//
//  utils.hpp
//  react-native-gdk
//
//  Created by Mattia Ferrari on 04/02/24.
//

#ifndef utils_hpp
#define utils_hpp

#include <gdk.h>
#include <jsi/jsi.h>
#include "TwoFactorCall.hpp"

namespace jsi = facebook::jsi;

namespace utils {
    // wraps a call from GA throwing a jsi Error matching the error code
    void wrapCall(int gdk_function_result, jsi::Runtime &rt);
    void getThreadErrorDetails(jsi::Runtime &rt, int errorCode);

    // debug utility
    int consoleLog(jsi::Runtime &rt, std::string msg);

    // resolve a TwoFactorCall
    jsi::Object resolve(TwoFactorCall call);

    // conversions
    jsi::Object GAJsonToObject(jsi::Runtime &rt, const GA_json *src);
    std::string stringify(jsi::Runtime &runtime, const jsi::Value &value);
    void jsiValueJsonToGAJson(jsi::Runtime &rt, jsi::Value src, GA_json **dest);
}

#endif /* utils_hpp */
