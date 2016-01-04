#pragma once

#include "TMP_CONF.h"

#ifdef __cplusplus
extern "C" {
#endif

DECLDIR int luaopen_log(lua_State* L);

static int subr_log_print(lua_State* L);
static int lua_open_log_file(lua_State* L);
static int lua_close_log_file(lua_State* L);

#ifdef __cplusplus
}
#endif