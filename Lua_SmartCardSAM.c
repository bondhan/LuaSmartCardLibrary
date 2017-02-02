#pragma once

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

#include "Lua_SmartCardSAM.h"
#include "pcscwrap.h"
#include "bytestring.h"
#include "utils.h"
#include "iso7816.h"
#include "log_util.h"
#include "TMP_CONF.h"
/*
The rule for smartcard sending and receiving is that all 
	-	send command must be converted from string hex to bytestring
	-	receiving data must be converted frong bytes to bytestring

	Therefore all output of cards command are string hex
*/


static const struct luaL_Reg pcsc_samlib[] =
{
	{ "list_readers", lua_listreaders_sam },
	{ "init_reader", bool_ret_function_sam },
	{ "reset_and_sleep", bool_ret_function_sam },
	{ "connect_reader", lua_connect_reader_sam },
	{ "disconnect_reader", lua_disconnect_reader_sam },
	{ "get_atr", lua_get_atr_sam },
	{ "send", lua_send_apdu_sam },
	{ "send_auto", lua_send_apdu_auto_sam },
	{ "do_pps", lua_dopps_sam },
	{ "stringify_sw", subr_card_stringify_sw_sam },
	{ "warm_reset", subr_card_warm_reset_sam },
	{ "cold_reset", lua_cold_reset_sam },
	{ "set_voltage", bool_ret_function_sam },
	{ "set_timeout", one_param_bool_ret_function_sam },
	{ "get_card_info", lua_get_card_info_sam },
	{ NULL, NULL }  /* sentinel */
};

int bool_ret_function_sam(lua_State* L)
{
	//for compatibility
	lua_pushboolean(L, 1); // return true

	return 1;
}

int one_param_bool_ret_function_sam(lua_State* L)
{
	//for compatibility
	int timeout_ms = luaL_checkinteger(L,1);

	lua_pushboolean(L, 1); // return true

	return 1;
}


int lua_get_card_info_sam(lua_State* L)
{
	char tmpBuf[100] = { '\0' };
	char msg[] = "0000000000000000";
	bytestring_t *data_bytestring = NULL;	
	
	lua_pushinteger(L, 0);
	lua_pushinteger(L, 0);
	lua_pushinteger(L, 0);
	lua_pushinteger(L, 0);
	lua_pushinteger(L, 0);
	lua_pushinteger(L, 0);

	sprintf(tmpBuf, "8:%s", msg);
	data_bytestring = bytestring_new_from_string(tmpBuf);

	lua_push_bytestring(L, data_bytestring);

	return 7;
}

int lua_dopps_sam(lua_State* L)
{
	log_printf(LOG_INFO, "PPS is exchanged automatically by OS");

	return 0 ;
}
int luaopen_sam(lua_State *L)
{
	luaL_newlib(L, pcsc_samlib);
	lua_setglobal(L, "pcsc_sam");	

	return 1;
}

int lua_listreaders_sam(lua_State *L)
{
	SCARDCONTEXT hCtx = { 0 };
	LONG rc = SCARD_S_SUCCESS;
	DWORD dw = SCARD_AUTOALLOCATE;
	LPTSTR c = NULL;
	char *cc = NULL;

	rc = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &hCtx);
	if (rc == SCARD_S_SUCCESS)
	{
		rc = SCardListReaders(hCtx, NULL, (LPTSTR)&c, &dw);
		if (rc == SCARD_S_SUCCESS)
		{
			cc = (char *)malloc(dw);
			if (cc != NULL)
			{
				cc[--dw] = 0; cc[--dw] = 0;
				while (dw--)  cc[dw] = (char)((c[dw]) ? c[dw] : 0x0A);
			}
			SCardFreeMemory(hCtx, c);
		}
		SCardReleaseContext(hCtx);
	}

	lua_pushstring(L, cc);

	return 1;
} /* sc_listreaders */

int lua_connect_reader_sam(lua_State *L)
{
	LONG rc = SCARD_S_SUCCESS;
	
	char *selectedReader = (char*)lua_tostring(L, 1);	//get the reader name

	if (reader == NULL) {
		reader = (sc_context *)malloc(sizeof(sc_context));
		memset(reader, 0, sizeof(sc_context));
	}

	rc = sc_init(reader, selectedReader);

	memset(msg, '\0', MSG_BUFFER_SIZE);
	if (rc == SC_OK)
	{		
		log_printf(LOG_INFO, "Connected to reader: %s", reader->rdr);		
		
		lua_pushboolean(L, 1); 
	}
	else
	{

		log_printf(LOG_INFO, "Cannot connect to reader %s", selectedReader);

		lua_pushboolean(L, 0); 
	}
	
	return 1;
}

int lua_get_atr_sam(lua_State *L)
{
	char atr[100] = { '\0' };
	memset(msg, '\0', MSG_BUFFER_SIZE);
	LONG rc = CheckCardStatus(reader, msg);

	if (rc == SCARD_S_SUCCESS)
	{
		bin2Hex(atr, reader->atr, reader->atrLen);
		sprintf(msg, "%s", atr);
	}

	lua_pushstring(L, msg);

	return 1;
}

int lua_disconnect_reader_sam(lua_State *L)
{
	LONG rc = SCARD_S_SUCCESS;
	
	rc = sc_done(reader, SCARD_LEAVE_CARD);

	if (rc != SCARD_S_SUCCESS)
		log_printf(LOG_INFO, "Reader failed to disconnect!");
	else
		log_printf(LOG_INFO, "Reader disconnected!");

	return 0;
}

int lua_send_apdu_sam(lua_State *L)
{
	bytestring_t *apdu_bytestring = NULL;
	bytestring_t *result = bytestring_new(8);
	char tmpBuf[1024] = { '\0' };

	char *apdu_command_string = (char*)lua_tostring(L, 1);	//get the apdu command

	if (apdu_command_string == NULL) // if it is bytestring
	{
		apdu_bytestring = luaL_check_bytestring(L, 1);
	}
	else
	{
		sprintf(tmpBuf, "8:%s", apdu_command_string);
		apdu_bytestring = bytestring_new_from_string(tmpBuf);
	}
	
	unsigned short SW = send_apdu_sam(L, apdu_bytestring, result, 1);

	lua_pushinteger(L, SW);
	lua_push_bytestring(L, result);
	
	return 2;
}

int lua_send_apdu_auto_sam(lua_State *L)
{
	bytestring_t *apdu_bytestring = NULL;
	bytestring_t *result = bytestring_new(8);
	char tmpBuf[1024] = { '\0' };
	unsigned short SW;

	char *apdu_command_string = (char*)lua_tostring(L, 1);	//get the apdu command

	if (apdu_command_string == NULL) // if it is bytestring
	{
		apdu_bytestring = luaL_check_bytestring(L, 1);
	}
	else
	{
		sprintf(tmpBuf, "8:%s", apdu_command_string);
		apdu_bytestring = bytestring_new_from_string(tmpBuf);
	}
	
	SW = send_apdu_sam(L, apdu_bytestring, result, 1);

	lua_pushinteger(L, SW);
	lua_push_bytestring(L, result);
	
	return 2;
}

int send_apdu_sam(lua_State *L, const bytestring_t *command, bytestring_t *result, int getResponse)
{	
	apdu_descriptor_t ad;

	bytestring_t* command_dup;
	bytestring_t* get_response;
	bytestring_t* tmp_response;
	char *tmp = NULL;
	unsigned  short SW;
	unsigned char SW1, SW2;

	if (iso7816_describe_apdu(&ad, command) != ISO7816_OK)
	{
		tmp = bytestring_to_format("%D", command);
		log_printf(LOG_ERROR, "Could not parse APDU format: %s", tmp);
		
		free(tmp);
	
		return RUNTIME_ERROR_SW;
	}
		
	tmp = bytestring_to_format("%D", command);
	log_printf(LOG_INFO, "send: %s [%s]", tmp, iso7816_stringify_apdu_class(ad.apdu_class));		
	free(tmp);

	SW = pcsc_transmit(reader, command, result);
	SW1 = (SW >> 8) & 0xFF;
	SW2 = SW & 0xFF;
	
	tmp = bytestring_to_format("%D", result);
	log_printf(LOG_INFO, "Recv: %04X %s ", SW, tmp);
	free(tmp);

	if (getResponse == 1)
	{
		if (SW1 == 0x6C) /* Re-issue with right length */
		{
			command_dup = bytestring_duplicate(command);
			if (ad.le_len == 3) /* in case of extended le */
				bytestring_resize(command_dup, bytestring_get_size(command_dup) - 2);
			bytestring_set_element(command_dup, -1, SW2);
			SW = send_apdu_sam(L, command_dup, result, getResponse);
			bytestring_free(command_dup);
		}
		else
		{
			while (SW1 == 0x61) /* use Get Response */
			{
				get_response = bytestring_new_from_string("8:00C0000000");
				tmp_response = bytestring_new_from_string("8:0000");

				bytestring_set_element(get_response, 4, SW2);

				SW = send_apdu_sam(L, get_response, tmp_response, getResponse);

				bytestring_append_data(result,
					bytestring_get_size(tmp_response),
					bytestring_get_data(tmp_response));

				bytestring_free(get_response);
				bytestring_free(tmp_response);
				SW1 = (SW >> 8) & 0xFF;
				SW2 = SW & 0xFF;
			}
		}

	}
	return SW;
}


int subr_card_stringify_sw_sam(lua_State *L)
{
	unsigned short sw = (unsigned short)luaL_checkinteger(L, 1);
	const char *strval = iso7816_stringify_sw(sw);

	lua_pushstring(L, strval);

	return 1;
}

int subr_card_warm_reset_sam(lua_State *L)
{
	if (pcsc_warm_reset(reader))
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);

	return 1;
}

int lua_cold_reset_sam(lua_State *L)
{
	LONG rc = SCARD_S_SUCCESS;

	rc = sc_reconnect(reader);

	if (rc == SC_OK)
	{
		log_printf(LOG_INFO, "Re-connected to reader: %s", reader->rdr);

		lua_pushboolean(L, 1);
	}
	else
	{
		log_printf(LOG_INFO, "Cannot connect to reader %s", reader->rdr);

		lua_pushboolean(L, 0);
	}

	return 0;
}
