#include "jsb_custom_socket_manual.hpp"
#include "cocos2d_specifics.hpp"
#include "custom/socket/Socket.h"
#include "jsb_custom_socket_delegate.h"

template<class T>
static bool dummy_constructor(JSContext *cx, uint32_t argc, jsval *vp) {
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedValue initializing(cx);
    bool isNewValid = true;
    JS::RootedObject global(cx, ScriptingCore::getInstance()->getGlobalObject());
    isNewValid = JS_GetProperty(cx, global, "initializing", &initializing) && initializing.toBoolean();
    if (isNewValid)
    {
        TypeTest<T> t;
        js_type_class_t *typeClass = nullptr;
        std::string typeName = t.s_name();
        auto typeMapIter = _js_global_type_map.find(typeName);
        CCASSERT(typeMapIter != _js_global_type_map.end(), "Can't find the class type!");
        typeClass = typeMapIter->second;
        CCASSERT(typeClass, "The value is null.");

        JS::RootedObject proto(cx, typeClass->proto.ref());
        JS::RootedObject parent(cx, typeClass->parentProto.ref());
        JS::RootedObject _tmp(cx, JS_NewObject(cx, typeClass->jsclass, proto, parent));
        
        args.rval().set(OBJECT_TO_JSVAL(_tmp));
        return true;
    }

    JS_ReportError(cx, "Constructor for the requested class is not available, please refer to the API reference.");
    return false;
}

static bool empty_constructor(JSContext *cx, uint32_t argc, jsval *vp) {
    return false;
}

static bool js_is_native_obj(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    args.rval().setBoolean(true);
    return true;
}
JSClass  *jsb_cocos2d_Socket_class;
JSObject *jsb_cocos2d_Socket_prototype;


bool js_custom_socket_Socket_setAddr(JSContext *cx, uint32_t argc, jsval *vp)
{
	JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
	js_proxy_t *proxy = jsb_get_js_proxy(obj);
	Socket* cobj = (Socket *)(proxy ? proxy->ptr : NULL);
	JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

	if (argc == 2){
		string host;
		bool ok = jsval_to_std_string(cx, args.get(0), &host);
		JSB_PRECONDITION2(ok, cx, false, "js_custom_socket_Socket_setAddr : Error processing arguments(0)");
		uint32_t port = 0;
		ok = jsval_to_uint32(cx, args.get(1), &port);
		JSB_PRECONDITION2(ok, cx, false, "js_custom_socket_Socket_setAddr : Error processing arguments(1)");
		cobj->setAddr(host.c_str(), port);
		args.rval().setUndefined();
		return true;
	}
	JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
	return false;
}

bool js_custom_socket_Socket_connect(JSContext *cx, uint32_t argc, jsval *vp)
{
	JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
	js_proxy_t *proxy = jsb_get_js_proxy(obj);
	Socket* cobj = (Socket *)(proxy ? proxy->ptr : NULL);
	JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

	if (argc == 0){
		cobj->connect();
		args.rval().setUndefined();
		return true;
	}
	JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
	return false;
}
bool js_custom_socket_Socket_close(JSContext *cx, uint32_t argc, jsval *vp)
{
	JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
	js_proxy_t *proxy = jsb_get_js_proxy(obj);
	Socket* cobj = (Socket *)(proxy ? proxy->ptr : NULL);
	JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

	if (argc == 0){
		cobj->close();
		args.rval().setUndefined();
		return true;
	}
	JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
	return false;
}

bool js_custom_socket_Socket_destroy(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    Socket* cobj = (Socket *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");
    
    if (argc == 0){
        cobj->destroy();
        args.rval().setUndefined();
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}


bool js_custom_socket_Socket_send(JSContext *cx, uint32_t argc, jsval *vp)
{

	JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
	js_proxy_t *proxy = jsb_get_js_proxy(obj);
	Socket* cobj = (Socket *)(proxy ? proxy->ptr : NULL);
	JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

	if (argc == 1){
		do
		{
			if (args.get(0).isObject())
			{
				uint8_t *bufdata = NULL;
				uint32_t len = 0;

				JS::RootedObject jsobj(cx, args.get(0).toObjectOrNull());
				/*
				if (JS_IsArrayBufferObject(jsobj))
				{
					bufdata = JS_GetArrayBufferData(jsobj);
					len = JS_GetArrayBufferByteLength(jsobj);
				}
				else if (JS_IsArrayBufferViewObject(jsobj))
				{
					bufdata = (uint8_t*)JS_GetArrayBufferViewData(jsobj);
					len = JS_GetArrayBufferViewByteLength(jsobj);
				}
				if (bufdata && len > 0)
				{
					bool ret = cobj->send(bufdata, len);
					jsval jsret = JSVAL_NULL;
					jsret = BOOLEAN_TO_JSVAL(ret);
					args.rval().set(jsret);
					return true;
				}
				*/

				if (JS_IsArrayObject(cx,jsobj)){
					uint32_t length;
					JS_GetArrayLength(cx, jsobj, &length);
					int totalLen = 0;
					vector<Socket::Data> datas;
					for (uint32_t i = 0; i<length; i++) {
						JS::RootedValue valarg(cx);
						JS_GetElement(cx, jsobj, i, &valarg);
						JSObject* tempObj = &valarg.toObject();
						if (JS_IsArrayBufferObject(tempObj))
						{
							bufdata = JS_GetArrayBufferData(tempObj);
							len = JS_GetArrayBufferByteLength(tempObj);
						}
						else if (JS_IsArrayBufferViewObject(tempObj))
						{
							bufdata = (uint8_t*)JS_GetArrayBufferViewData(tempObj);
							len = JS_GetArrayBufferViewByteLength(tempObj);
						}
						totalLen += len;
						Socket::Data d;
						d.len = len;
						d.bytes = (char*)const_cast<unsigned char*>(bufdata);
						datas.push_back(d);
					}

					bool ret = cobj->send(datas, totalLen);
					jsval jsret = JSVAL_NULL;
					jsret = BOOLEAN_TO_JSVAL(ret);
					args.rval().set(jsret);
					return true;
				}
			}

			JS_ReportError(cx, "data type to be sent is unsupported.");
			return false;
		} while (0);

		args.rval().setUndefined();

		return true;
	}
	JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
	return false;
	/*
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    cocos2d::Socket* cobj = (cocos2d::Socket *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_custom_socket_Socket_send : Invalid Native Object");
    if (argc == 2) {
        const unsigned char* arg0 = nullptr;
        unsigned int arg1 = 0;
        #pragma warning NO CONVERSION TO NATIVE FOR unsigned char*
		ok = false;
        ok &= jsval_to_uint32(cx, args.get(1), &arg1);
        JSB_PRECONDITION2(ok, cx, false, "js_custom_socket_Socket_send : Error processing arguments");
        bool ret = cobj->send(arg0, arg1);
        jsval jsret = JSVAL_NULL;
        jsret = BOOLEAN_TO_JSVAL(ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_custom_socket_Socket_send : wrong number of arguments: %d, was expecting %d", argc, 2);
    return false;
	*/
}
bool js_custom_socket_Socket_constructor(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    cocos2d::Socket* cobj = new (std::nothrow) cocos2d::Socket();
	/*
    cocos2d::Ref *_ccobj = dynamic_cast<cocos2d::Ref *>(cobj);
    if (_ccobj) {
        _ccobj->autorelease();
    }
	*/

    TypeTest<cocos2d::Socket> t;
    js_type_class_t *typeClass = nullptr;
    std::string typeName = t.s_name();
    auto typeMapIter = _js_global_type_map.find(typeName);
    CCASSERT(typeMapIter != _js_global_type_map.end(), "Can't find the class type!");
    typeClass = typeMapIter->second;
    CCASSERT(typeClass, "The value is null.");
    JS::RootedObject proto(cx, typeClass->proto.ref());
    JS::RootedObject parent(cx, typeClass->parentProto.ref());
    JS::RootedObject obj(cx, JS_NewObject(cx, typeClass->jsclass, proto, parent));
    args.rval().set(OBJECT_TO_JSVAL(obj));

	JSB_SocketDelegate* delegate = new (std::nothrow) JSB_SocketDelegate(cx);
	delegate->setJSDelegate(obj);
	cobj->setDelegate(delegate);
    delegate->release();

    // link the native object with the javascript object
    js_proxy_t* p = jsb_new_proxy(cobj, obj);
    AddNamedObjectRoot(cx, &p->obj, "cocos2d::Socket");
    if (JS_HasProperty(cx, obj, "_ctor", &ok) && ok)
        ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(obj), "_ctor", args);
    return true;
}

void js_cocos2d_Socket_finalize(JSFreeOp *fop, JSObject *obj) {
    CCLOGINFO("jsbindings: finalizing JS object %p (Socket)", obj);
}
void js_register_custom_socket_Socket(JSContext *cx, JS::HandleObject global) {
    jsb_cocos2d_Socket_class = (JSClass *)calloc(1, sizeof(JSClass));
    jsb_cocos2d_Socket_class->name = "Socket";
    jsb_cocos2d_Socket_class->addProperty = JS_PropertyStub;
    jsb_cocos2d_Socket_class->delProperty = JS_DeletePropertyStub;
    jsb_cocos2d_Socket_class->getProperty = JS_PropertyStub;
    jsb_cocos2d_Socket_class->setProperty = JS_StrictPropertyStub;
    jsb_cocos2d_Socket_class->enumerate = JS_EnumerateStub;
    jsb_cocos2d_Socket_class->resolve = JS_ResolveStub;
    jsb_cocos2d_Socket_class->convert = JS_ConvertStub;
    jsb_cocos2d_Socket_class->finalize = js_cocos2d_Socket_finalize;
    jsb_cocos2d_Socket_class->flags = JSCLASS_HAS_RESERVED_SLOTS(2);

    static JSPropertySpec properties[] = {
        JS_PSG("__nativeObj", js_is_native_obj, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_PS_END
    };

    static JSFunctionSpec funcs[] = {
		JS_FN("send", js_custom_socket_Socket_send, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		JS_FN("setAddr", js_custom_socket_Socket_setAddr, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		JS_FN("connect", js_custom_socket_Socket_connect, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("close", js_custom_socket_Socket_close, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("destroy", js_custom_socket_Socket_destroy, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FS_END
    };

    JSFunctionSpec *st_funcs = NULL;

    jsb_cocos2d_Socket_prototype = JS_InitClass(
        cx, global,
        JS::NullPtr(),
        jsb_cocos2d_Socket_class,
        js_custom_socket_Socket_constructor, 0, // constructor
        properties,
        funcs,
        NULL, // no static properties
        st_funcs);

    // add the proto and JSClass to the type->js info hash table
    JS::RootedObject proto(cx, jsb_cocos2d_Socket_prototype);
    jsb_register_class<cocos2d::Socket>(cx, jsb_cocos2d_Socket_class, proto, JS::NullPtr());
}
void register_all_custom_socket(JSContext* cx, JS::HandleObject obj) {
    // Get the ns
    JS::RootedObject ns(cx);
    get_or_create_js_obj(cx, obj, "cc", &ns);

    js_register_custom_socket_Socket(cx, ns);
}

