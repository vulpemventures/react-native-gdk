//
//  TwoFactorCall.cpp
//  react-native-gdk
//
//  Created by Mattia Ferrari on 06/02/24.
//

#include "TwoFactorCall.hpp"
#include "utils.hpp"

TwoFactorCall::TwoFactorCall(GA_auth_handler* handler, jsi::Runtime &runtime): rt(runtime) {
    ptr = handler;
}

//TwoFactorCall::~TwoFactorCall() {
//    GA_destroy_auth_handler(ptr);
//}

void TwoFactorCall::call() {
    utils::wrapCall(GA_auth_handler_call(ptr), rt);
}

jsi::Object TwoFactorCall::getStatus() {
    GA_json *status;
    utils::wrapCall(GA_auth_handler_get_status(ptr, &status), rt);

    return utils::GAJsonToObject(rt, status);
}
