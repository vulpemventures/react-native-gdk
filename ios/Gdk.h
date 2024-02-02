#ifdef __cplusplus
#import "react-native-gdk.h"
#endif

#ifdef RCT_NEW_ARCH_ENABLED
#import "RNGdkSpec.h"

@interface Gdk : NSObject <NativeGdkSpec>
#else
#import <React/RCTBridgeModule.h>

@interface Gdk : NSObject <RCTBridgeModule>
#endif

@end
