//
//  TwoFactorCall.hpp
//  react-native-gdk
//
//  Created by Mattia Ferrari on 06/02/24.
//

#ifndef TwoFactorCall_hpp
#define TwoFactorCall_hpp

#include <gdk.h>
#include "json.hpp"


using namespace nlohmann;

class TwoFactorCall {

public:
    TwoFactorCall(GA_auth_handler* handler);
//    ~TwoFactorCall();
    
    void call();
    json getStatus();
 
private:
    GA_auth_handler* ptr;
        
};

#endif /* TwoFactorCall_hpp */
