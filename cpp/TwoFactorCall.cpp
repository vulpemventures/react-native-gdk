//
//  TwoFactorCall.cpp
//  react-native-gdk
//
//  Created by Mattia Ferrari on 06/02/24.
//

#include "TwoFactorCall.hpp"
#include "utils.hpp"

TwoFactorCall::TwoFactorCall(GA_auth_handler* handler) {
    ptr = handler;
}

//TwoFactorCall::~TwoFactorCall() {
//    GA_destroy_auth_handler(ptr);
//}

void TwoFactorCall::call() {
    GA_auth_handler_call(ptr);
}

json TwoFactorCall::getStatus() {
    GA_json *resJson;
    GA_auth_handler_get_status(ptr, &resJson);
    char *stringJson;
    GA_convert_json_to_string(resJson, &stringJson);
    std::string s(stringJson);
    json res = json::parse(s);
    return res;
}
