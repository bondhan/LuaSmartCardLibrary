/********************************************************************** 
*
* This file is part of Cardpeek, the smart card reader utility.
*
* Copyright 2009-2013 by Alain Pannetrat <L1L1@gmx.com>
*
* Cardpeek is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Cardpeek is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Cardpeek.  If not, see <http://www.gnu.org/licenses/>.
*
*/
#ifndef LUA_BYTES_H

#include <lua.h>
#include "bytestring.h"
#include "TMP_CONF.h"

#ifdef __cplusplus
extern "C" {
#endif

	DECLDIR int luaopen_bytes(lua_State* L);
	bytestring_t* luaL_check_bytestring(lua_State *L, int p);
	int luaL_is_bytestring(lua_State *L, int p);
	void lua_push_bytestring(lua_State *L, bytestring_t* bs);

//static int subr_bytes_get(lua_State *L);
//static int subr_bytes_new(lua_State *L);
//static int subr_bytes_set(lua_State *L);
//static int subr_bytes_pad_left(lua_State *L);
//static int subr_bytes_pad_right(lua_State *L);
//static int subr_bytes_sub(lua_State *L);
//static int subr_bytes_convert(lua_State *L);

#ifdef __cplusplus
}
#endif

#endif
