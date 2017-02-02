#pragma once

#include <lua.h>

int luaopen_safenet(lua_State *L);
int lua_login_hsm(lua_State *L);
int lua_logout_hsm(lua_State *L);