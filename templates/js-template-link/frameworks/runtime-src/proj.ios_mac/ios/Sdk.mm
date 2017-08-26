//
//  Sdk.cpp
//  rabbit
//
//

#import "Sdk.h"
#import "UnityAds/UnityAds.h"


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


void SdkHandle::showAd(){
    [UnityAds show:view placementId:@"rewardedVideo"];
    //[UnityAds show:view placementId:@"rewardedVideo"];
}
