#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

#include "lua_log.h"
#include"log_util.h"

static int subr_log_print(lua_State* L)
{
	int level = lua_tointeger(L, 1);
	const char *message = luaL_checkstring(L, 2);
	log_printf(level, "%s", message);
	return 0;
}

static const struct luaL_Reg loglib[] = {
	{ "open_logfile", lua_open_log_file },
	{ "close_logfile", lua_close_log_file },
	{ "print", subr_log_print },
	{ NULL, NULL }  /* sentinel */
};


int luaopen_log(lua_State* L)
{
	luaL_newlib(L, loglib);
	lua_pushstring(L, "DEBUG");   lua_pushinteger(L, LOG_DEBUG); lua_settable(L, -3);
	lua_pushstring(L, "INFO");    lua_pushinteger(L, LOG_INFO); lua_settable(L, -3);
	lua_pushstring(L, "WARNING"); lua_pushinteger(L, LOG_WARNING); lua_settable(L, -3);
	lua_pushstring(L, "ERROR");   lua_pushinteger(L, LOG_ERROR); lua_settable(L, -3);
	lua_setglobal(L, "log");

	return 1;
}

int lua_open_log_file(lua_State* L)
{
	char *msg[512];
	int debug_level = 5;

	const char *log_filename = luaL_checkstring(L, 1);

	if (lua_gettop(L) == 2) {		
		debug_level = luaL_checkinteger(L, 2);
	}

	log_open_file(log_filename, msg, debug_level);

	lua_pushstring(L, msg);

	return 1;
}

int lua_close_log_file(lua_State* L)
{
	log_close_file();

	return 1;
}