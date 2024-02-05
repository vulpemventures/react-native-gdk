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

namespace jsi = facebook::jsi;

namespace utils {
    // wraps a call from GA throwing a jsi Error matching the error code
    void wrapCall(int gdk_function_result, jsi::Runtime &rt);
    void getThreadErrorDetails(jsi::Runtime &rt, int errorCode);

    // conversions
    jsi::Object GAJsonToObject(jsi::Runtime &rt, const GA_json *src);
}

#endif /* utils_hpp */
