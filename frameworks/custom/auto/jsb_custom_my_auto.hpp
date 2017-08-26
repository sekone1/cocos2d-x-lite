#include "base/ccConfig.h"
#ifndef __custom_my_h__
#define __custom_my_h__

#include "jsapi.h"
#include "jsfriendapi.h"

extern JSClass  *jsb_cocos2d_CustomClass_class;
extern JSObject *jsb_cocos2d_CustomClass_prototype;

bool js_custom_my_CustomClass_constructor(JSContext *cx, uint32_t argc, jsval *vp);
void js_custom_my_CustomClass_finalize(JSContext *cx, JSObject *obj);
void js_register_custom_my_CustomClass(JSContext *cx, JS::HandleObject global);
void register_all_custom_my(JSContext* cx, JS::HandleObject obj);
bool js_custom_my_CustomClass_init(JSContext *cx, uint32_t argc, jsval *vp);
bool js_custom_my_CustomClass_helloMsg(JSContext *cx, uint32_t argc, jsval *vp);
bool js_custom_my_CustomClass_create(JSContext *cx, uint32_t argc, jsval *vp);
bool js_custom_my_CustomClass_CustomClass(JSContext *cx, uint32_t argc, jsval *vp);

#endif // __custom_my_h__
