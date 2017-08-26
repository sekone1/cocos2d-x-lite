#ifndef __custom_socke_delegate_h__
#define __custom_socke_delegate_h__

#include "jsb_custom_socket_manual.hpp"
#include "cocos2d_specifics.hpp"
#include "custom/socket/Socket.h"
#include "cocos2d.h"
using namespace cocos2d;

class JSB_SocketDelegate : public Socket::Delegate
{
public:
	JSB_SocketDelegate(JSContext *cx_)
		: cx(cx_)
	{
		_JSDelegate.construct(cx);
	}

	~JSB_SocketDelegate()
	{
		_JSDelegate.destroyIfConstructed();
	}

	virtual void onError(Socket* s, Socket::ErrorCode errCode)
	{
		js_proxy_t * p = jsb_get_native_proxy(s);
		if (p == nullptr) return;

        if (Director::getInstance() == nullptr || ScriptEngineManager::getInstance() == nullptr)
			return;

		JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET

		JS::RootedObject jsobj(cx, JS_NewObject(cx, NULL, JS::NullPtr(), JS::NullPtr()));
		
		JS::RootedValue vp(cx);
		vp = int32_to_jsval(cx, (int32_t)errCode);
		JS_SetProperty(cx, jsobj, "code", vp);

		JS::RootedValue args(cx, OBJECT_TO_JSVAL(jsobj));

		ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(_JSDelegate.ref()), "onError", 1, args.address());
	

	}
	virtual void onConnected(Socket* s)
	{
		js_proxy_t * p = jsb_get_native_proxy(s);
		if (p == nullptr) return;

		if (Director::getInstance() == nullptr || ScriptEngineManager::getInstance() == nullptr)
			return;

		JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET

		ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(_JSDelegate.ref()), "onConnected", 0, nullptr);

	}
	virtual void onTimeOut(Socket* s, uint16_t protoID, uint32_t serialNum)
	{
		js_proxy_t * p = jsb_get_native_proxy(s);
		if (p == nullptr) return;

		if (Director::getInstance() == nullptr || ScriptEngineManager::getInstance() == nullptr)
			return;

		JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET

		JS::RootedObject jsobj(cx, JS_NewObject(cx, NULL, JS::NullPtr(), JS::NullPtr()));

		JS::RootedValue vp(cx);
		vp = uint32_to_jsval(cx, serialNum);
		JS_SetProperty(cx, jsobj, "serialNum", vp);

		JS::RootedValue pid(cx);
		pid = uint32_to_jsval(cx, protoID);
		JS_SetProperty(cx, jsobj, "protoID", pid);

		JS::RootedValue args(cx, OBJECT_TO_JSVAL(jsobj));

		ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(_JSDelegate.ref()), "onTimeOut", 1, args.address());
	}
	virtual void onMessage(Socket* s, const Socket::Data& data)
	{
		js_proxy_t * p = jsb_get_native_proxy(s);
		if (p == nullptr) return;

		if (Director::getInstance() == nullptr || ScriptEngineManager::getInstance() == nullptr)
			return;

		JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET

		JS::RootedObject jsobj(cx, JS_NewObject(cx, NULL, JS::NullPtr(), JS::NullPtr()));
		/*
		JS::RootedValue vp(cx);
		vp = c_string_to_jsval(cx, "message");
		JS_SetProperty(cx, jsobj, "type", vp);
		*/

		JS::RootedValue args(cx, OBJECT_TO_JSVAL(jsobj));
		JS::RootedObject buffer(cx, JS_NewArrayBuffer(cx, static_cast<uint32_t>(data.len)));
		if (data.len > 0)
		{
			uint8_t* bufdata = JS_GetArrayBufferData(buffer);
			memcpy((void*)bufdata, (void*)data.bytes, data.len);
		}

		JS::RootedValue dataVal(cx);
		dataVal = OBJECT_TO_JSVAL(buffer);
		JS_SetProperty(cx, jsobj, "data", dataVal);

		ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(_JSDelegate.ref()), "onMessage", 1, args.address());
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
