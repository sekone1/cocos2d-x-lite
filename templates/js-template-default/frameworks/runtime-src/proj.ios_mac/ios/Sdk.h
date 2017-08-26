//
//  Sdk.hpp
//  rabbit
//
//
//

//#ifndef Sdk_hpp
//#define Sdk_hpp

#import <UIKit/UIKit.h>

//#import <stdio.h>
#import "cocos2d.h"
#import "custom/sdk/Sdk.h"
using namespace cocos2d;
class SdkHandle : Sdk::InterDelegate
{
    SdkHandle();
    ~SdkHandle();
    static SdkHandle* s_SdkHandle;
public:
     UIViewController* view;
    void showAd();
    void buy(const char* productID, const char* orderID);
    void finishTrans(const char* transID);
    void checkUnFinishTrans();
    static SdkHandle* getInstance();
};

//#endif /* Sdk_hpp */
