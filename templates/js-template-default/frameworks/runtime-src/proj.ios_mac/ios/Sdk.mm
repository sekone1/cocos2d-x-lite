//
//  Sdk.cpp
//  rabbit
//
//

#import "Sdk.h"
#import "UnityAds/UnityAds.h"
#import "IAPHelper.h"


SdkHandle* SdkHandle::s_SdkHandle = nullptr;

SdkHandle* SdkHandle::getInstance()
{
    if (!s_SdkHandle)
    {
        s_SdkHandle = new (std::nothrow) SdkHandle();
        CCASSERT(s_SdkHandle, "FATAL: Not enough memory");
    }
    
    return s_SdkHandle;
}

SdkHandle::SdkHandle(){
    Sdk::getInstance()->setInterDelegate(this);
}

SdkHandle::~SdkHandle(){
    CC_ASSERT(this == s_SdkHandle);
    s_SdkHandle = nullptr;
}


void SdkHandle::buy(const char* productID, const char* orderID){
    [[IAPHelper shared] purchaseProduct:[NSString stringWithUTF8String:orderID] productId: [NSString stringWithUTF8String:productID] quantity:1];
}

void SdkHandle::finishTrans(const char* transID){
    [[IAPHelper shared] finishTransactionById:[NSString stringWithUTF8String:transID]];
}

void SdkHandle::checkUnFinishTrans(){
    [[IAPHelper shared] checkUnFinishTrans];
}
void SdkHandle::showAd(){
    [UnityAds show:view placementId:@"rewardedVideo"];
    //[UnityAds show:view placementId:@"rewardedVideo"];
}
