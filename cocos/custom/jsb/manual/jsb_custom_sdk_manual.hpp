#include "base/ccConfig.h"
#ifndef __custom_sdk_h__
#define __custom_sdk_h__

#include "jsapi.h"
#include "jsfriendapi.h"

extern JSClass  *jsb_cocos2d_Sdk_class;
extern JSObject *jsb_cocos2d_Sdk_prototype;

bool js_custom_sdk_Sdk_constructor(JSContext *cx, uint32_t argc, jsval *vp);
void js_custom_sdk_Sdk_finalize(JSContext *cx, JSObject *obj);
void js_register_custom_sdk_Sdk(JSContext *cx, JS::HandleObject global);
void register_all_custom_sdk(JSContext* cx, JS::HandleObject obj);
bool js_custom_sdk_Sdk_destroy(JSContext *cx, uint32_t argc, jsval *vp);
bool js_custom_sdk_Sdk_showAd(JSContext *cx, uint32_t argc, jsval *vp);
bool js_custom_sdk_Sdk_finishTrans(JSContext *cx, uint32_t argc, jsval *vp);
bool js_custom_sdk_Sdk_getInstance(JSContext *cx, uint32_t argc, jsval *vp);
bool js_custom_sdk_Sdk_getInstance(JSContext *cx, uint32_t argc, jsval *vp);

#endif // __custom_sdk_h__
