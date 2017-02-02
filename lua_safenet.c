#pragma once

#include <lualib.h>
#include <lauxlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <cryptoki.h>
#include <ctvdef.h>
#include <ctutil.h>

#include "bytestring.h"
#include "utils.h"
#include "log_util.h"
#include "TMP_CONF.h"

#include "lua_safenet.h"

static const struct luaL_Reg safenet_hsm[] =
{
	{ "login", lua_login_hsm },
	{ "logout", lua_logout_hsm },
	{ NULL, NULL }  /* sentinel */
};

int luaopen_safenet(lua_State *L)
{
	luaL_newlib(L, safenet_hsm);
	lua_setglobal(L, "safenet_hsm");

	return 1;
}


//open hsm
int lua_login_hsm(lua_State *L)
{
	//slot no, pin
	CK_RV rv = CKR_OK;
	CK_SESSION_HANDLE hSession = CK_INVALID_HANDLE;
	
	int encoding_station=1;
	
	char *slotId = (char*)lua_tostring(L, 1);	//get the slot ID
	char *pin = (char*)lua_tostring(L, 2);	//get the pin

	lua_getglobal(L, ENCODING_STATION_LITERAL_NAME);
	encoding_station=lua_tointeger(L,-1);

	if (login_hsm(slotId, pin, &hSession) != CKR_OK)
	{
		log_printf(LOG_ERROR, encoding_station,  "Login failed! Check slot no and PIN");		

		lua_pushboolean(L, 0); 
	} 
	else 
	{
		log_printf(LOG_INFO, encoding_station,  "Login successful");		

		lua_pushboolean(L, 1); 
	}		

	return 1;
}

//close hsm
int lua_logout_hsm(lua_State *L)
{
	//slot no, pin
	CK_RV rv = CKR_OK;

	int encoding_station=1;
	
	CK_SESSION_HANDLE hSession = luaL_checkinteger(L, 1);	//get the slot ID

	lua_getglobal(L, ENCODING_STATION_LITERAL_NAME);
	encoding_station=lua_tointeger(L,-1);

	if (logout_hsm(hSession) != CKR_OK)
	{
		log_printf(LOG_ERROR, encoding_station,  "Logout fail!");		

		lua_pushboolean(L, 0); 
	} 
	else 
	{
		log_printf(LOG_INFO, encoding_station,  "Logout successful");		

		lua_pushboolean(L, 1); 
	}		

	return 1;
}
