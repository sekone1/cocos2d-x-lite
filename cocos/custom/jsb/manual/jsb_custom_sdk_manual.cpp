#include "jsb_custom_sdk_manual.hpp"
#include "scripting/js-bindings/manual/cocos2d_specifics.hpp"
#include "custom/sdk/Sdk.h"
#include "jsb_custom_sdk_delegate.h"

template<class T>
static bool dummy_constructor(JSContext *cx, uint32_t argc, jsval *vp)
{
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
JSClass  *jsb_cocos2d_Sdk_class;
JSObject *jsb_cocos2d_Sdk_prototype;

bool js_custom_sdk_Sdk_destroy(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    cocos2d::Sdk* cobj = (cocos2d::Sdk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_custom_sdk_Sdk_destroy : Invalid Native Object");
    if (argc == 0) {
        cobj->destroy();
        args.rval().setUndefined();
        return true;
    }

    JS_ReportError(cx, "js_custom_sdk_Sdk_destroy : wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}
bool js_custom_sdk_Sdk_showAd(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    cocos2d::Sdk* cobj = (cocos2d::Sdk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_custom_sdk_Sdk_showAd : Invalid Native Object");
    if (argc == 0) {
        cobj->showAd();
        args.rval().setUndefined();
        return true;
    }

    JS_ReportError(cx, "js_custom_sdk_Sdk_showAd : wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}
bool js_custom_sdk_Sdk_getInstance(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    if (argc == 0) {

        auto ret = cocos2d::Sdk::getInstance();
        js_type_class_t *typeClass = js_get_type_from_native<cocos2d::Sdk>(ret);
        JS::RootedObject jsret(cx, jsb_ref_get_or_create_jsobject(cx, ret, typeClass, "cocos2d::Sdk"));
        args.rval().set(OBJECT_TO_JSVAL(jsret));
        
        
        if(ret->getDelegate() == nullptr)
        {
            JSB_SdkDelegate* delegate = new (std::nothrow) JSB_SdkDelegate(cx);
            delegate->setJSDelegate(jsret);
            ret->setDelegate(delegate);
            delegate->release();
            
        }
        
        return true;
    }
    JS_ReportError(cx, "js_custom_sdk_Sdk_getInstance : wrong number of arguments");
    return false;
}


bool js_custom_sdk_Sdk_checkUnFinishTrans(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    cocos2d::Sdk* cobj = (cocos2d::Sdk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_custom_sdk_Sdk_checkUnFinishTrans : Invalid Native Object");
    if (argc == 0) {
        cobj->checkUnFinishTrans();
        args.rval().setUndefined();
        return true;
    }
    
    JS_ReportError(cx, "js_custom_sdk_Sdk_checkUnFinishTrans : wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}
bool js_custom_sdk_Sdk_buy(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    cocos2d::Sdk* cobj = (cocos2d::Sdk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_custom_sdk_Sdk_buy : Invalid Native Object");
    if (argc == 2) {
        const char* arg0 = nullptr;
        const char* arg1 = nullptr;
        std::string arg0_tmp; ok &= jsval_to_std_string(cx, args.get(0), &arg0_tmp); arg0 = arg0_tmp.c_str();
        std::string arg1_tmp; ok &= jsval_to_std_string(cx, args.get(1), &arg1_tmp); arg1 = arg1_tmp.c_str();
        JSB_PRECONDITION2(ok, cx, false, "js_custom_sdk_Sdk_buy : Error processing arguments");
        cobj->buy(arg0, arg1);
        args.rval().setUndefined();
        return true;
    }
    
    JS_ReportError(cx, "js_custom_sdk_Sdk_buy : wrong number of arguments: %d, was expecting %d", argc, 2);
    return false;
}

bool js_custom_sdk_Sdk_finishTrans(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    cocos2d::Sdk* cobj = (cocos2d::Sdk *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_custom_sdk_Sdk_finishTrans : Invalid Native Object");
    if (argc == 1) {
        const char* arg0 = nullptr;
        std::string arg0_tmp; ok &= jsval_to_std_string(cx, args.get(0), &arg0_tmp); arg0 = arg0_tmp.c_str();
        JSB_PRECONDITION2(ok, cx, false, "js_custom_sdk_Sdk_finishTrans : Error processing arguments");
        cobj->finishTrans(arg0);
        args.rval().setUndefined();
        return true;
    }
    
    JS_ReportError(cx, "js_custom_sdk_Sdk_finishTrans : wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}


void js_register_custom_sdk_Sdk(JSContext *cx, JS::HandleObject global) {
    jsb_cocos2d_Sdk_class = (JSClass *)calloc(1, sizeof(JSClass));
    jsb_cocos2d_Sdk_class->name = "Sdk";
    jsb_cocos2d_Sdk_class->addProperty = JS_PropertyStub;
    jsb_cocos2d_Sdk_class->delProperty = JS_DeletePropertyStub;
    jsb_cocos2d_Sdk_class->getProperty = JS_PropertyStub;
    jsb_cocos2d_Sdk_class->setProperty = JS_StrictPropertyStub;
    jsb_cocos2d_Sdk_class->enumerate = JS_EnumerateStub;
    jsb_cocos2d_Sdk_class->resolve = JS_ResolveStub;
    jsb_cocos2d_Sdk_class->convert = JS_ConvertStub;
    jsb_cocos2d_Sdk_class->flags = JSCLASS_HAS_RESERVED_SLOTS(2);
    
    static JSPropertySpec properties[] = {
        JS_PS_END
    };
    
    static JSFunctionSpec funcs[] = {
        JS_FN("checkUnFinishTrans", js_custom_sdk_Sdk_checkUnFinishTrans, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("buy", js_custom_sdk_Sdk_buy, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("destroy", js_custom_sdk_Sdk_destroy, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("showAd", js_custom_sdk_Sdk_showAd, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("finishTrans", js_custom_sdk_Sdk_finishTrans, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FS_END
    };
    
    static JSFunctionSpec st_funcs[] = {
        JS_FN("getInstance", js_custom_sdk_Sdk_getInstance, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FS_END
    };
    
    jsb_cocos2d_Sdk_prototype = JS_InitClass(
                                             cx, global,
                                             JS::NullPtr(),
                                             jsb_cocos2d_Sdk_class,
                                             empty_constructor, 0,
                                             properties,
                                             funcs,
                                             NULL, // no static properties
                                             st_funcs);
    
    JS::RootedObject proto(cx, jsb_cocos2d_Sdk_prototype);
    JS::RootedValue className(cx, std_string_to_jsval(cx, "Sdk"));
    JS_SetProperty(cx, proto, "_className", className);
    JS_SetProperty(cx, proto, "__nativeObj", JS::TrueHandleValue);
    JS_SetProperty(cx, proto, "__is_ref", JS::TrueHandleValue);
    // add the proto and JSClass to the type->js info hash table
    jsb_register_class<cocos2d::Sdk>(cx, jsb_cocos2d_Sdk_class, proto, JS::NullPtr());
}

void register_all_custom_sdk(JSContext* cx, JS::HandleObject obj) {
    // Get the ns
    JS::RootedObject ns(cx);
    get_or_create_js_obj(cx, obj, "cc", &ns);
    
    js_register_custom_sdk_Sdk(cx, ns);
}


