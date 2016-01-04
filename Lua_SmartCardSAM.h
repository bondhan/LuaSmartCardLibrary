#pragma once

#include <windows.h>
#include <winscard.h>
#include "PCSCWRAP.H"
#include "bytestring.h"
#include "TMP_CONF.h"

#define SC_OK		SCARD_S_SUCCESS
#define SC_BAD		0xBEDA

#define MSG_BUFFER_SIZE 1024

#define SCARD_MYPROTOSET SCARD_PROTOCOL_T0 |   \
                         SCARD_PROTOCOL_T1 |   \
                         SCARD_PROTOCOL_RAW |  \
                         SCARD_PROTOCOL_OPTIMAL


#ifdef __cplusplus
extern "C" {
#endif

	DECLDIR int luaopen_sam(lua_State *L);
	int lua_connect_reader_sam(lua_State *L);
	int lua_listreaders_sam(lua_State *L);
	int lua_disconnect_reader_sam(lua_State *L);
	int lua_get_atr_sam(lua_State *L);
	int lua_send_apdu_sam(lua_State *L);
	int lua_send_apdu_auto_sam(lua_State *L);
	int send_apdu_sam(lua_State *L, const bytestring_t *command, bytestring_t *result);
	int subr_card_stringify_sw_sam(lua_State *L);
	int subr_card_warm_reset_sam(lua_State *L);
	int lua_dopps_sam(lua_State* L);
	int lua_cold_reset_sam(lua_State *L);
	int lua_get_card_info_sam(lua_State* L);
	int bool_ret_function_sam(lua_State* L);
	int lua_do_reset_encoding_station_sam(lua_State* L);
	int one_param_bool_ret_function_sam(lua_State* L);
	 
	char msg[MSG_BUFFER_SIZE];
	sc_context *reader;

#ifdef __cplusplus
}
#endif