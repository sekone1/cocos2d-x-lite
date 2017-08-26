#include "base/ccConfig.h"
#ifndef __custom_socket_h__
#define __custom_socket_h__

#include "spidermonkey/jsapi.h"
#include "spidermonkey/jsfriendapi.h"

extern JSClass  *jsb_cocos2d_Socket_class;
extern JSObject *jsb_cocos2d_Socket_prototype;

bool js_custom_socket_Socket_constructor(JSContext *cx, uint32_t argc, jsval *vp);
void js_custom_socket_Socket_finalize(JSContext *cx, JSObject *obj);
void js_register_custom_socket_Socket(JSContext *cx, JS::HandleObject global);
void register_all_custom_socket(JSContext* cx, JS::HandleObject obj);
bool js_custom_socket_Socket_send(JSContext *cx, uint32_t argc, jsval *vp);
bool js_custom_socket_Socket_setAddr(JSContext *cx, uint32_t argc, jsval *vp);
bool js_custom_socket_Socket_connect(JSContext *cx, uint32_t argc, jsval *vp);
bool js_custom_socket_Socket_close(JSContext *cx, uint32_t argc, jsval *vp);
bool js_custom_socket_Socket_destroy(JSContext *cx, uint32_t argc, jsval *vp);
bool js_custom_socket_Socket_Socket(JSContext *cx, uint32_t argc, jsval *vp);

#endif // __custom_socket_h__
