#ifndef __custom_module_register_h__
#define __custom_module_register_h__
#include "scripting/js-bindings/manual/ScriptingCore.h"

#include "jsb_custom_socket_manual.hpp"
#include "jsb_custom_sdk_manual.hpp"

void js_custom_module_register(ScriptingCore* sc)
{
    sc->addRegisterCallback(register_all_custom_socket);
    sc->addRegisterCallback(register_all_custom_sdk);
}
#endif
