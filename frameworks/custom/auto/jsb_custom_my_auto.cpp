#include "jsb_custom_my_auto.hpp"
#include "scripting/js-bindings/manual/cocos2d_specifics.hpp"
#include "my/CustomClass.h"
//#include "my/CustomClass.h"

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
JSClass  *jsb_cocos2d_CustomClass_class;
JSObject *jsb_cocos2d_CustomClass_prototype;

bool js_custom_my_CustomClass_init(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    cocos2d::CustomClass* cobj = (cocos2d::CustomClass *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_custom_my_CustomClass_init : Invalid Native Object");
    if (argc == 0) {
        bool ret = cobj->init();
        JS::RootedValue jsret(cx);
        jsret = BOOLEAN_TO_JSVAL(ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_custom_my_CustomClass_init : wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}
bool js_custom_my_CustomClass_helloMsg(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    cocos2d::CustomClass* cobj = (cocos2d::CustomClass *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "js_custom_my_CustomClass_helloMsg : Invalid Native Object");
    if (argc == 0) {
        std::string ret = cobj->helloMsg();
        JS::RootedValue jsret(cx);
        jsret = std_string_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "js_custom_my_CustomClass_helloMsg : wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}
bool js_custom_my_CustomClass_create(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    if (argc == 0) {

        auto ret = cocos2d::CustomClass::create();
        js_type_class_t *typeClass = js_get_type_from_native<cocos2d::CustomClass>(ret);
        JS::RootedObject jsret(cx, jsb_ref_autoreleased_create_jsobject(cx, ret, typeClass, "cocos2d::CustomClass"));
        args.rval().set(OBJECT_TO_JSVAL(jsret));
        return true;
    }
    JS_ReportError(cx, "js_custom_my_CustomClass_create : wrong number of arguments");
    return false;
}

bool js_custom_my_CustomClass_constructor(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    cocos2d::CustomClass* cobj = new (std::nothrow) cocos2d::CustomClass();

    js_type_class_t *typeClass = js_get_type_from_native<cocos2d::CustomClass>(cobj);

    // link the native object with the javascript object
    JS::RootedObject jsobj(cx, jsb_ref_create_jsobject(cx, cobj, typeClass, "cocos2d::CustomClass"));
    args.rval().set(OBJECT_TO_JSVAL(jsobj));
    if (JS_HasProperty(cx, jsobj, "_ctor", &ok) && ok)
        ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(jsobj), "_ctor", args);
    return true;
}


void js_register_custom_my_CustomClass(JSContext *cx, JS::HandleObject global) {
    jsb_cocos2d_CustomClass_class = (JSClass *)calloc(1, sizeof(JSClass));
    jsb_cocos2d_CustomClass_class->name = "CustomClass";
    jsb_cocos2d_CustomClass_class->addProperty = JS_PropertyStub;
    jsb_cocos2d_CustomClass_class->delProperty = JS_DeletePropertyStub;
    jsb_cocos2d_CustomClass_class->getProperty = JS_PropertyStub;
    jsb_cocos2d_CustomClass_class->setProperty = JS_StrictPropertyStub;
    jsb_cocos2d_CustomClass_class->enumerate = JS_EnumerateStub;
    jsb_cocos2d_CustomClass_class->resolve = JS_ResolveStub;
    jsb_cocos2d_CustomClass_class->convert = JS_ConvertStub;
    jsb_cocos2d_CustomClass_class->flags = JSCLASS_HAS_RESERVED_SLOTS(2);

    static JSPropertySpec properties[] = {
        JS_PS_END
    };

    static JSFunctionSpec funcs[] = {
        JS_FN("init", js_custom_my_CustomClass_init, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("helloMsg", js_custom_my_CustomClass_helloMsg, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FS_END
    };

    static JSFunctionSpec st_funcs[] = {
        JS_FN("create", js_custom_my_CustomClass_create, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FS_END
    };

    jsb_cocos2d_CustomClass_prototype = JS_InitClass(
        cx, global,
        JS::NullPtr(),
        jsb_cocos2d_CustomClass_class,
        js_custom_my_CustomClass_constructor, 0, // constructor
        properties,
        funcs,
        NULL, // no static properties
        st_funcs);

    JS::RootedObject proto(cx, jsb_cocos2d_CustomClass_prototype);
    JS::RootedValue className(cx, std_string_to_jsval(cx, "CustomClass"));
    JS_SetProperty(cx, proto, "_className", className);
    JS_SetProperty(cx, proto, "__nativeObj", JS::TrueHandleValue);
    JS_SetProperty(cx, proto, "__is_ref", JS::TrueHandleValue);
    // add the proto and JSClass to the type->js info hash table
    jsb_register_class<cocos2d::CustomClass>(cx, jsb_cocos2d_CustomClass_class, proto, JS::NullPtr());
}

void register_all_custom_my(JSContext* cx, JS::HandleObject obj) {
    // Get the ns
    JS::RootedObject ns(cx);
    get_or_create_js_obj(cx, obj, "cc", &ns);

    js_register_custom_my_CustomClass(cx, ns);
}

