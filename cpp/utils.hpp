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
#include "GdkHostObject.hpp"
#include "json.hpp"

using json = nlohmann::json;
namespace jsi = facebook::jsi;

namespace utils {
    // wraps a call from GA throwing a jsi Error matching the error code
    void wrapCall(int gdk_function_result);
    void getThreadErrorDetails(int errorCode);

    // debug utility
    int consoleLog(jsi::Runtime &rt, std::string msg);

    // resolve a TwoFactorCall
    json resolve(TwoFactorCall call);


    // handle notifications
    void notificationsHandler(void* ctx, GA_json* details);

    // conversions
    jsi::Object GAJsonToObject(jsi::Runtime &rt, const GA_json *src);
    std::string stringify(jsi::Runtime &runtime, const jsi::Value &value);
    jsi::Value parse(jsi::Runtime &runtime, std::string src);
    void jsiValueJsonToGAJson(jsi::Runtime &rt, jsi::Value src, GA_json **dest);
    
    struct Promise {
        Promise(jsi::Runtime &rt, jsi::Function resolve, jsi::Function reject);

        void resolve(const jsi::Value &result);
        void reject(const std::string &error);

        jsi::Runtime &runtime_;
        jsi::Function resolve_;
        jsi::Function reject_;
    };

    using Promised = std::function<void(jsi::Runtime &rt, std::shared_ptr<Promise>)>;
    jsi::Value makePromise(jsi::Runtime &rt, const Promised func);

    class Exception : public std::runtime_error {
    public:
        explicit Exception(const std::string& message) : std::runtime_error(message) {}
    };

}

#endif /* utils_hpp */
