//
//  TwoFactorCall.hpp
//  react-native-gdk
//
//  Created by Mattia Ferrari on 06/02/24.
//

#ifndef TwoFactorCall_hpp
#define TwoFactorCall_hpp

#include <gdk.h>
#include <jsi/jsi.h>

using namespace facebook;


class TwoFactorCall {

public:
    jsi::Runtime &rt;
    TwoFactorCall(GA_auth_handler* handler, jsi::Runtime &runtime);
//    ~TwoFactorCall();
    
    void call();
    jsi::Object getStatus();
 
private:
    GA_auth_handler* ptr;
        
};

#endif /* TwoFactorCall_hpp */
