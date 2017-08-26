#ifndef __custom_socke_delegate_h__
#define __custom_socke_delegate_h__

#include "cocos2d_specifics.hpp"
#include "custom/sdk/Sdk.h"
#include "cocos2d.h"
using namespace cocos2d;

class JSB_SdkDelegate : public Sdk::Delegate
{
public:
	JSB_SdkDelegate(JSContext *cx_)
		: cx(cx_)
	{
		_JSDelegate.construct(cx);
	}

	~JSB_SdkDelegate()
	{
		_JSDelegate.destroyIfConstructed();
	}
    
    virtual void onAdsDidFinish(Sdk* s, Sdk::AdState state)
    {
        js_proxy_t * p = jsb_get_native_proxy(s);
        if (p == nullptr) return;
        
        if (Director::getInstance() == nullptr || ScriptEngineManager::getInstance() == nullptr)
            return;
        
        JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
        
        JS::RootedObject jsobj(cx, JS_NewObject(cx, NULL, JS::NullPtr(), JS::NullPtr()));
        
        JS::RootedValue vp(cx);
        vp = int32_to_jsval(cx, (int32_t)state);
        JS_SetProperty(cx, jsobj, "state", vp);
        
        JS::RootedValue args(cx, OBJECT_TO_JSVAL(jsobj));
        
        ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(_JSDelegate.ref()), "onAdsDidFinish", 1, args.address());
    }
    virtual void onAdsReady(Sdk* s)
    {
        js_proxy_t * p = jsb_get_native_proxy(s);
        if (p == nullptr) return;
        
        if (Director::getInstance() == nullptr || ScriptEngineManager::getInstance() == nullptr)
            return;
        
        JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
        
        ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(_JSDelegate.ref()), "onAdsReady", 0, nullptr);
        
    }


	void setJSDelegate(JS::HandleObject pJSDelegate)
	{
		_JSDelegate.ref() = pJSDelegate;
	}
private:
	JSContext *cx;
	mozilla::Maybe<JS::PersistentRootedObject> _JSDelegate;
};
#endif
