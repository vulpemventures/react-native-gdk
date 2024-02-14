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
    // Error utilities, wraps a call from GA throwing an error matching the error code
    void wrapCall(int gdk_function_result);
    void getThreadErrorDetails(int errorCode);
    class Exception : public std::runtime_error {
    public:
        explicit Exception(const std::string& message) : std::runtime_error(message) {}
    };

    // debug utility
    int consoleLog(jsi::Runtime &rt, std::string msg);

    // resolve a TwoFactorCall
    json resolve(TwoFactorCall call);


    // handle notifications
    void notificationsHandler(void* ctx, GA_json* details);

    // conversions
    jsi::Object GAJsonToObject(jsi::Runtime &rt, const GA_json *src);
    void jsiValueJsonToGAJson(jsi::Runtime &rt, jsi::Value src, GA_json **dest);
    // definition of js JSON.stringify and JSON.parse
    std::string stringify(jsi::Runtime &runtime, const jsi::Value &value);
    jsi::Value parse(jsi::Runtime &runtime, std::string src);
    

    // promise definition
    struct Promise {
        Promise(jsi::Runtime &rt, jsi::Function resolve, jsi::Function reject);

        void resolve(const jsi::Value &result);
        void reject(const std::string &error);

        jsi::Runtime &runtime_;
        jsi::Function resolve_;
        jsi::Function reject_;
    };
    using Promised = std::function<void(jsi::Runtime &rt, std::shared_ptr<Promise>)>;
    
    // creates and returns a promise given the funnction to be wrapped in promise
    jsi::Value makePromise(jsi::Runtime &rt, const Promised func);


}

#endif /* utils_hpp */
