#import <React/RCTBridgeModule.h>

@interface NativeGdk : NSObject <RCTBridgeModule>
    @property (nonatomic, assign) BOOL setBridgeOnMainQueue;
@end
