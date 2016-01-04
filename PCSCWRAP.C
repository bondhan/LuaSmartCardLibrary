/* *************************************************************************

   Smart Card Digging Utilities
   PC/SC Wrappers

   Written by Ilya O. Levin, http://www.literatecode.com

   This source code released for free "as is" under the whatever ABC
   license without warranty or liability of any kind. Use it at your
   own risk any way you like for legitimate purposes only.

   *************************************************************************
*/

#include "pcscwrap.h"
#include "TMP_CONF.h"
#include "log_util.h"


const char *PCSC_ERROR[] = {
	/* 00 */	"No error",
	/* 01 */	"Error 01",
	/* 02 */ 	"The action was cancelled by an SCardCancel request",
	/* 03 */ 	"The supplied handle was invalid",
	/* 04 */	"Error 04",
	/* 05 */	"Error 05",
	/* 06 */	"Error 06",
	/* 07 */	"Error 07",
	/* 08 */	"Error 08",
	/* 09 */	"Error 09",
	/* OA */	"The user-specified timeout value has expired",
	/* 0B */	"The smart card cannot be accessed because of other connections outstanding",
	/* 0C */ 	"The operation requires a smart card, but no smart card is currently in the device",
	/* 0D */	"Error 0D",
	/* 0E */	"Error 0E",
	/* 0F */ 	"The requested protocols are incompatible with the protocol currently in use with the smart card",
	/* 10 */	"Error 10",
	/* 11 */	"Error 11",
	/* 12 */	"Error 12",
	/* 13 */	"Error 13",
	/* 14 */	"Error 14",
	/* 15 */	"Error 15",
	/* 16 */ 	"An attempt was made to end a non-existent transaction",
	/* 17 */	"The specified reader is not currently available for use",
	/* 18 */	"Error 1A",
	/* 19 */	"Error 1B",
	/* 1A */	"Error 1C",
	/* 1B */	"Error 1D",
	/* 1C */	"Error 1C",
	/* 1D */	"The Smart card resource manager is not running",
	/* 1E */	"Error 1E",
	/* 1F */	"Error 1F",
	/* 20 */	"Error 20",
	/* 21 */	"Error 21",
	/* 22 */	"Error 22",
	/* 23 */	"Error 23",
	/* 24 */	"Error 24",
	/* 25 */	"Error 25",
	/* 26 */	"Error 26",
	/* 27 */	"Error 27",
	/* 28 */	"Error 28",
	/* 29 */	"Error 29",
	/* 2A */	"Error 2A",
	/* 2B */	"Error 2B",
	/* 2C */	"Error 2C",
	/* 2D */	"Error 2D",
	/* 2E */ 	"Cannot find a smart card reader",
	/* 2F */	"Error 2F",

	/* 66 */ 	"The smart card is not responding to a reset",
	/* 67 */ 	"Power has been removed from the smart card, so that further communication is not possible",
	/* 68 */ 	"The smart card has been reset, so any shared state information is invalid",
	/* 69 */ 	"The smart card has been removed, so further communication is not possible"
};


/* -------------------------------------------------------------------------  */
char *sc_listreaders(void)
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

	return cc;
} /* sc_listreaders */


/* -----------------------------------------------------------------------------
   this is the exact copy of CRT's strstr() function
*/
char * __cdecl x_strstr(const char * str1, const char * str2)
{
	char *cp = (char *)str1;
	char *s1, *s2;

	if (!*str2) return((char *)str1);
	while (*cp)
	{
		s1 = cp;
		s2 = (char *)str2;
		while (*s1 && *s2 && !(*s1 - *s2)) s1++, s2++;
		if (!*s2) return(cp);
		cp++;
	}

	return(NULL);
} /* x_strstr */


/* -------------------------------------------------------------------------- */
LPTSTR findfirststr(char *buf, char *tpl)
{
	LPTSTR rc = NULL;

	if ((tpl == NULL) || (buf == NULL)) return (buf);

	while (*(buf))
	{
		rc = (LPTSTR)x_strstr((const char *)buf, (const char *)tpl);
		if (rc != NULL) { rc = buf; break; }
		else while (*(buf++));
	}

	return rc;
} /* findfirststr */


/* -------------------------------------------------------------------------- */
LONG sc_init(sc_context *ctx, char *rdr)
{
	SCARD_READERSTATE rSt = { 0 };
	LONG rc = SCARD_S_SUCCESS;
	LPTSTR c = NULL, cc = NULL;
	DWORD dw = SCARD_AUTOALLOCATE;

	rc = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &ctx->hCtx);
	if (rc == SCARD_S_SUCCESS)
	{
		rc = SCardListReaders(ctx->hCtx, NULL, (LPTSTR)&c, &dw);
		if (rc == SCARD_S_SUCCESS)
		{
			cc = findfirststr(c, rdr);
			if (cc != NULL)
			{
				rSt.szReader = cc;
				rSt.dwCurrentState = SCARD_STATE_UNAWARE;
				rc = SCardGetStatusChange(ctx->hCtx, 0, &rSt, 1);
				if ((rSt.dwEventState & SCARD_STATE_EMPTY) > 0) rc = SC_BAD;
				else
				{
					ctx->rdr = NULL;
					ctx->rdrsz = SCARD_AUTOALLOCATE;
					ctx->CLA = 0;
					ctx->proto = SCARD_PCI_T0;
					ctx->rw = 0;
					rc = SCardConnect(ctx->hCtx, cc, SCARD_SHARE_SHARED,
						SCARD_MYPROTOSET, &ctx->hCard, &dw);
					if (dw == SCARD_PROTOCOL_T1) ctx->proto = SCARD_PCI_T1;
					else if (dw == SCARD_PROTOCOL_RAW) ctx->proto = SCARD_PCI_RAW;
					if (rc == SCARD_S_SUCCESS)
					{
						ctx->isConnected = 1;

						SCardGetAttrib(ctx->hCard, SCARD_ATTR_DEVICE_FRIENDLY_NAME,
							(LPBYTE)&ctx->rdr, &ctx->rdrsz);

					} else
						ctx->isConnected = 0;
				}
			}
			else rc = SC_BAD;
			SCardFreeMemory(ctx->hCtx, c);
		}
		if (rc != SCARD_S_SUCCESS) SCardReleaseContext(ctx->hCtx);
	}

	return rc;
} /* sc_init */


LONG sc_reconnect(sc_context *ctx)
{
	SCARD_READERSTATE rSt = { 0 };
	LONG rc = SCARD_S_SUCCESS;
	LPTSTR c = NULL, cc = NULL;
	DWORD dw = SCARD_AUTOALLOCATE;

	//disconnect
	rc = SCardDisconnect(ctx->hCard, SCARD_LEAVE_CARD); /* SCARD_LEAVE_CARD); */

	//reconnect
	rc = SCardConnect(ctx->hCtx, ctx->rdr, SCARD_SHARE_SHARED,
		SCARD_MYPROTOSET, &ctx->hCard, &dw);

	if (dw == SCARD_PROTOCOL_T1) 
		ctx->proto = SCARD_PCI_T1;
	else if (dw == SCARD_PROTOCOL_RAW) 
		ctx->proto = SCARD_PCI_RAW;

	if (rc == SCARD_S_SUCCESS)
	{
		ctx->rdr = NULL;
		ctx->rdrsz = SCARD_AUTOALLOCATE;

		SCardGetAttrib(ctx->hCard, SCARD_ATTR_DEVICE_FRIENDLY_NAME,
			(LPBYTE)&ctx->rdr, &ctx->rdrsz);

		ctx->isConnected = 1;
	} else
		ctx->isConnected = 0;

	return rc;
}

/* -------------------------------------------------------------------------- */
LONG sc_init_u(sc_context *ctx, char *rdr) /* *rdr must be a full name */
{
	SCARD_READERSTATE rSt = { 0 };
	LONG rc = SCARD_S_SUCCESS;
	DWORD dw = SCARD_AUTOALLOCATE;

	rc = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &ctx->hCtx);
	if (rc == SCARD_S_SUCCESS)
	{
		rSt.szReader = rdr;
		rSt.dwCurrentState = SCARD_STATE_UNAWARE;
		rc = SCardGetStatusChange(ctx->hCtx, 0, &rSt, 1);
		if ((rSt.dwEventState & SCARD_STATE_EMPTY) > 0) rc = SC_BAD;
		else
		{
			ctx->rdr = NULL;
			ctx->rdrsz = SCARD_AUTOALLOCATE;
			ctx->CLA = 0;
			ctx->proto = SCARD_PCI_T0;
			ctx->rw = 0;
			rc = SCardConnect(ctx->hCtx, rdr, SCARD_SHARE_SHARED,
				SCARD_MYPROTOSET, &ctx->hCard, &dw);
			if (rc == SCARD_S_SUCCESS)
			{
				ctx->isConnected = 1;

				SCardGetAttrib(ctx->hCard, SCARD_ATTR_DEVICE_FRIENDLY_NAME,
					(LPBYTE)&ctx->rdr, &ctx->rdrsz);
				if (dw == SCARD_PROTOCOL_T1) ctx->proto = SCARD_PCI_T1;
				else if (dw == SCARD_PROTOCOL_RAW) ctx->proto = SCARD_PCI_RAW;
			} else
				ctx->isConnected = 0;
		}
		if (rc != SCARD_S_SUCCESS) SCardReleaseContext(ctx->hCtx);
	}

	return rc;
} /* sc_init */


/* -------------------------------------------------------------------------- */
LONG sc_done(sc_context *ctx, DWORD lvSt)
{
	LONG rc = SCARD_S_SUCCESS;
	register BYTE i = 0xFF;

	rc = SCardDisconnect(ctx->hCard, lvSt); /* SCARD_LEAVE_CARD); */
	if (rc == SCARD_S_SUCCESS)
	{
		if (ctx->rdr != NULL) SCardFreeMemory(ctx->hCtx, ctx->rdr);
		ctx->rdrsz = 0; ctx->rdr = NULL;
		rc = SCardReleaseContext(ctx->hCtx);
	}
	while (i-- > 0) ctx->sw[i] = 0;
	ctx->rw = 0;

	return rc;
} /* sc_finish */


/* -------------------------------------------------------------------------- */
LONG sc_selectfile(sc_context *ctx, DWORD fID)
{
	LONG rc = SCARD_S_SUCCESS;
	DWORD dw = sizeof(ctx->sw);
	BYTE *buf = ctx->sw;

	buf[0] = ctx->CLA; ctx->lCLA = buf[0];
	buf[1] = 0xa4; buf[2] = buf[3] = buf[6] = 0; buf[4] = 2; buf[5] = 0x3f;
	if (fID > 0)
	{
		buf[6] = (BYTE)(fID & 255); buf[5] = (BYTE)((fID >> 8) & 255);
	}

	rc = SCardTransmit(ctx->hCard, ctx->proto, buf, 7, NULL, ctx->sw, &dw);

	ctx->rw = ctx->sw[0] * 256 + ctx->sw[1];

	return rc;
} /* sc_selectfile */


/* -------------------------------------------------------------------------- */
LONG sc_getresponse(sc_context *ctx)
{
	LONG rc = SCARD_S_SUCCESS;
	DWORD dw = sizeof(ctx->sw);
	BYTE *buf = ctx->sw;

	buf[0] = ctx->lCLA;  /* set a proper class */
	buf[4] = ctx->sw[1]; /* set data length */
	buf[1] = 0xC0;  buf[2] = buf[3] = 0;

	rc = SCardTransmit(ctx->hCard, ctx->proto, buf, 5, NULL, ctx->sw, &dw);

	ctx->rw = ctx->sw[0] * 256 + ctx->sw[1];

	return rc;
} /* sc_getresponse */


/* -------------------------------------------------------------------------- */
LONG sc_readdata(sc_context *ctx, BYTE datsz, DWORD offs)
{
	LONG rc = SCARD_S_SUCCESS;
	DWORD dw = sizeof(ctx->sw);
	BYTE *buf = ctx->sw;

	buf[0] = ctx->CLA;
	ctx->lCLA = buf[0];
	buf[1] = 0xB0;
	buf[3] = (BYTE)offs & 255;
	buf[2] = (BYTE)(offs >> 8) & 255;
	buf[4] = datsz;

	rc = SCardTransmit(ctx->hCard, ctx->proto, buf, 5, NULL, ctx->sw, &dw);
	ctx->rw = ctx->sw[0] * 256 + ctx->sw[1];

	return rc;
} /* sc_readdata */

/* ------------------------------------------------------------------------- */
LONG sc_updatedata(sc_context *ctx, BYTE *dat, BYTE datsz, DWORD offs)
{
	LONG rc = SCARD_S_SUCCESS;
	DWORD dw = sizeof(ctx->sw);
	BYTE *buf = ctx->sw;
	register BYTE i;

	if ((dat != NULL) && (datsz > 0))
	{
		buf[0] = ctx->CLA;
		buf[1] = 0xD6;
		buf[3] = (BYTE)offs & 255;
		buf[2] = (BYTE)(offs >> 8) & 255;
		buf[4] = datsz;
		ctx->lCLA = buf[0];
		for (i = 0; i < datsz; i++) buf[5 + i] = dat[i];
		datsz += 5;

		rc = SCardTransmit(ctx->hCard, ctx->proto, buf, datsz, NULL, ctx->sw, &dw);
		ctx->rw = ctx->sw[0] * 256 + ctx->sw[1];
	}

	return rc;
} /* sc_updatedata */

/* -------------------------------------------------------------------------- */
LONG sc_rawsend(sc_context *ctx, void *buf, BYTE bufsz)
{
	DWORD dw = sizeof(ctx->sw);
	LONG rc;

	if ((buf == NULL) || (bufsz == 0)) return SCARD_E_CARD_UNSUPPORTED;
	if (ctx != buf) ZeroMemory(ctx->sw, sizeof(ctx->sw));
	rc = SCardTransmit(ctx->hCard, ctx->proto, (BYTE *)buf, bufsz, NULL, ctx->sw, &dw);
	ctx->rw = ctx->sw[0] * 256 + ctx->sw[1];

	return rc;
} /* sc__rawsend */


/* -------------------------------------------------------------------------- */
char * rc_symb(LONG rc)
{
	char *c;
	switch (rc)
	{
	case SC_BAD: c = "SCARD_GENERIC_ERROR"; break;
	case SCARD_E_BAD_SEEK: c = "SCARD_E_BAD_SEEK"; break;
	case SCARD_E_CANCELLED: c = "SCARD_E_CANCELLED"; break;
	case SCARD_E_CANT_DISPOSE: c = "SCARD_E_CANT_DISPOSE"; break;
	case SCARD_E_CARD_UNSUPPORTED: c = "SCARD_E_CARD_UNSUPPORTED"; break;
	case SCARD_E_CERTIFICATE_UNAVAILABLE: c = "SCARD_E_CERTIFICATE_UNAVAILABLE"; break;
	case SCARD_E_COMM_DATA_LOST: c = "SCARD_E_COMM_DATA_LOST"; break;
	case SCARD_E_DIR_NOT_FOUND: c = "SCARD_E_DIR_NOT_FOUND"; break;
	case SCARD_E_DUPLICATE_READER: c = "SCARD_E_DUPLICATE_READER"; break;
	case SCARD_E_FILE_NOT_FOUND: c = "SCARD_E_FILE_NOT_FOUND"; break;
	case SCARD_E_ICC_CREATEORDER: c = "SCARD_E_ICC_CREATEORDER"; break;
	case SCARD_E_ICC_INSTALLATION: c = "SCARD_E_ICC_INSTALLATION"; break;
	case SCARD_E_INSUFFICIENT_BUFFER: c = "SCARD_E_INSUFFICIENT_BUFFER"; break;
	case SCARD_E_INVALID_ATR: c = "SCARD_E_INVALID_ATR"; break;
	case SCARD_E_INVALID_CHV: c = "SCARD_E_INVALID_CHV"; break;
	case SCARD_E_INVALID_HANDLE: c = "SCARD_E_INVALID_HANDLE"; break;
	case SCARD_E_INVALID_PARAMETER: c = "SCARD_E_INVALID_PARAMETER"; break;
	case SCARD_E_INVALID_TARGET: c = "SCARD_E_INVALID_TARGET"; break;
	case SCARD_E_INVALID_VALUE: c = "SCARD_E_INVALID_VALUE"; break;
	case SCARD_E_NO_ACCESS: c = "SCARD_E_NO_ACCESS"; break;
	case SCARD_E_NO_DIR: c = "SCARD_E_NO_DIR"; break;
	case SCARD_E_NO_FILE: c = "SCARD_E_NO_FILE"; break;
	case SCARD_E_NO_MEMORY: c = "SCARD_E_NO_MEMORY"; break;
	case SCARD_E_NO_READERS_AVAILABLE: c = "SCARD_E_NO_READERS_AVAILABLE"; break;
	case SCARD_E_NO_SERVICE: c = "SCARD_E_NO_SERVICE"; break;
	case SCARD_E_NO_SMARTCARD: c = "SCARD_E_NO_SMARTCARD"; break;
	case SCARD_E_NO_SUCH_CERTIFICATE: c = "SCARD_E_NO_SUCH_CERTIFICATE"; break;
	case SCARD_E_NOT_READY: c = "SCARD_E_NOT_READY"; break;
	case SCARD_E_NOT_TRANSACTED: c = "SCARD_E_NOT_TRANSACTED"; break;
	case SCARD_E_PCI_TOO_SMALL: c = "SCARD_E_PCI_TOO_SMALL"; break;
	case SCARD_E_PROTO_MISMATCH: c = "SCARD_E_PROTO_MISMATCH"; break;
	case SCARD_E_READER_UNAVAILABLE: c = "SCARD_E_READER_UNAVAILABLE"; break;
	case SCARD_E_READER_UNSUPPORTED: c = "SCARD_E_READER_UNSUPPORTED"; break;
	case SCARD_E_SERVICE_STOPPED: c = "SCARD_E_SERVICE_STOPPED"; break;
	case SCARD_E_SHARING_VIOLATION: c = "SCARD_E_SHARING_VIOLATION"; break;
	case SCARD_E_SYSTEM_CANCELLED: c = "SCARD_E_SYSTEM_CANCELLED"; break;
	case SCARD_E_TIMEOUT: c = "SCARD_E_TIMEOUT"; break;
	case SCARD_E_UNEXPECTED: c = "SCARD_E_UNEXPECTED"; break;
	case SCARD_E_UNKNOWN_CARD: c = "SCARD_E_UNKNOWN_CARD"; break;
	case SCARD_E_UNKNOWN_READER: c = "SCARD_E_UNKNOWN_READER"; break;
	case SCARD_E_UNKNOWN_RES_MNG: c = "SCARD_E_UNKNOWN_RES_MNG"; break;
	case SCARD_E_UNSUPPORTED_FEATURE: c = "SCARD_E_UNSUPPORTED_FEATURE"; break;
	case SCARD_E_WRITE_TOO_MANY: c = "SCARD_E_WRITE_TOO_MANY"; break;
	case SCARD_F_COMM_ERROR: c = "SCARD_F_COMM_ERROR"; break;
	case SCARD_F_INTERNAL_ERROR: c = "SCARD_F_INTERNAL_ERROR"; break;
	case SCARD_F_UNKNOWN_ERROR: c = "SCARD_F_UNKNOWN_ERROR"; break;
	case SCARD_F_WAITED_TOO_LONG: c = "SCARD_F_WAITED_TOO_LONG"; break;
	case SCARD_P_SHUTDOWN: c = "SCARD_P_SHUTDOWN"; break;
	case SCARD_S_SUCCESS: c = "SCARD_S_SUCCESS"; break;
	case SCARD_W_CANCELLED_BY_USER: c = "SCARD_W_CANCELLED_BY_USER"; break;
	case SCARD_W_CHV_BLOCKED: c = "SCARD_W_CHV_BLOCKED"; break;
	case SCARD_W_EOF: c = "SCARD_W_EOF"; break;
	case SCARD_W_REMOVED_CARD: c = "SCARD_W_REMOVED_CARD"; break;
	case SCARD_W_RESET_CARD: c = "SCARD_W_RESET_CARD"; break;
	case SCARD_W_SECURITY_VIOLATION: c = "SCARD_W_SECURITY_VIOLATION"; break;
	case SCARD_W_UNPOWERED_CARD: c = "SCARD_W_UNPOWERED_CARD"; break;
	case SCARD_W_UNRESPONSIVE_CARD: c = "SCARD_W_UNRESPONSIVE_CARD"; break;
	case SCARD_W_UNSUPPORTED_CARD: c = "SCARD_W_UNSUPPORTED_CARD"; break;
	case SCARD_W_WRONG_CHV: c = "SCARD_W_WRONG_CHV"; break;
	default:  c = "UNKNOWN CODE"; break;
	}
	return c;
} /* rc_symb */


LONG CheckCardStatus(sc_context *ctx, char *msg)
{
#define SCARD_NEGOTIABLEMODE    -0x0020
#define SCARD_SPECIFICMODE      -0x0040

	LONG lReturn;

	DWORD     pdwState;
	DWORD     pdwProtocol;
	DWORD     pbATRLen = 32;

	lReturn = SCardStatus(ctx->hCard,
		(LPTSTR)ctx->rdr,
		&ctx->rdrsz,
		&pdwState,
		&pdwProtocol,
		(LPBYTE)&ctx->atr,
		&pbATRLen);

	ctx->pdwState = pdwState;
	ctx->atrLen = pbATRLen;
	ctx->proto = pdwProtocol;

	if (lReturn != SCARD_S_SUCCESS)
		if (pdwState == SCARD_ABSENT)
			sprintf(msg, "%s\n", "There is no card in the reader \n");
		else if (pdwState == SCARD_PRESENT)
			sprintf(msg, "%s\n", "There is a card in the reader, but it has not been moved into position for use \n");
		else if (pdwState == SCARD_SWALLOWED)
			sprintf(msg, "%s\n", "There is a card in the reader in position for use. The card is not powered \n");
		else if (pdwState == SCARD_NEGOTIABLEMODE)
			sprintf(msg, "%s\n", "Power is being provided to the card, but the reader driver is unaware of the mode of the card \n");
		else if (pdwState == SCARD_PRESENT)
			sprintf(msg, "%s\n", "The card has been reset and is awaiting PTS negotiation \n");
		else if (pdwState == SCARD_SPECIFICMODE)
		{
			sprintf(msg, "%s\n", "The card has been reset and specific communication protocols have been established \n");

			if (pdwProtocol == SCARD_PROTOCOL_RAW)
				sprintf(msg, "%s\n", "The Raw Transfer protocol is in use \n");
			else if (pdwProtocol == SCARD_PROTOCOL_RAW)
				sprintf(msg, "%s\n", "The ISO 7816/3 T=0 protocol is in use \n");
			else if (pdwProtocol == SCARD_PROTOCOL_RAW)
				sprintf(msg, "%s\n", "The ISO 7816/3 T=1 protocol is in use \n");
		}

		return lReturn;
}


unsigned short pcsc_transmit(sc_context *ctx, bytestring_t *command, bytestring_t *result)
{
	BYTE REC_DAT[MAX_PCSC_READ_LENGTH];
	DWORD REC_LEN = MAX_PCSC_READ_LENGTH;

	DWORD dw = sizeof(ctx->sw);
	LONG rc;
	unsigned short SW;

	if ((command == NULL) || (result == NULL)) return SCARD_E_CARD_UNSUPPORTED;

	if (ctx != command)
		ZeroMemory(ctx->sw, sizeof(ctx->sw));

	if (ctx->proto == SCARD_PROTOCOL_T0)
	{		
		rc = SCardTransmit(ctx->hCard, SCARD_PCI_T0,
			bytestring_get_data(command),
			bytestring_get_size(command),
			SCARD_PCI_NULL,
			REC_DAT, &REC_LEN);
	}
	else if (ctx->proto == SCARD_PROTOCOL_T1)
	{
		rc= SCardTransmit(ctx->hCard, SCARD_PCI_T1,
			bytestring_get_data(command),
			bytestring_get_size(command),
			SCARD_PCI_NULL,
			REC_DAT, &REC_LEN);

	}
	else
	{
		log_printf(LOG_ERROR, "Unknown smartcard protocol: %i", ctx->proto);
		
		return RUNTIME_ERROR_SW;
	}

	if (rc != SCARD_S_SUCCESS)
	{
		log_printf(LOG_ERROR, "Failed to transmit command to card: %s (error 0x%08x).",
			pcsc_stringify_error(rc),
			rc);
		return RUNTIME_ERROR_SW;
	}
	if (REC_LEN >= 2)
	{
		bytestring_assign_data(result, REC_LEN - 2, REC_DAT);
		SW = (REC_DAT[REC_LEN - 2] << 8) | REC_DAT[REC_LEN - 1];
	}
	else if (REC_LEN == 1)
	{
		bytestring_clear(result);
		SW = REC_DAT[0];
	}
	else
	{
		log_printf(LOG_ERROR, "Transmited %i bytes to the card (%s), but recieved a response of length %i, without any status word included.",
			bytestring_get_size(command),
			pcsc_stringify_protocol(ctx->proto),
			REC_LEN);

		return RUNTIME_ERROR_SW;
	}

	return SW;
} /* sc__rawsend */



static const char *pcsc_stringify_protocol(DWORD proto)
{
	static char proto_string[32];

	switch (proto)
	{
	case SCARD_PROTOCOL_T0:
		return "T=0";
	case SCARD_PROTOCOL_T1:
		return "T=1";
	case SCARD_PROTOCOL_RAW:
		return "Raw";
	}
	sprintf(proto_string, "UNKNOWN(0x%x)", (unsigned)proto);
	return proto_string;
}

static const char *pcsc_stringify_state(DWORD state)
{
	static char state_string[500];
	int state_string_len;

	*state_string = 0;

	if (state & SCARD_STATE_CHANGED)
		strcat(state_string, " Changed state,");

	if (state & SCARD_STATE_IGNORE)
		strcat(state_string, " Ignore reader,");

	if (state & SCARD_STATE_UNKNOWN)
		strcat(state_string, " Unknown reader,");

	if (state & SCARD_STATE_UNAVAILABLE)
		strcat(state_string, " Status unavailable,");

	if (state & SCARD_STATE_EMPTY)
		strcat(state_string, " Card removed,");

	if (state & SCARD_STATE_PRESENT)
		strcat(state_string, " Card present,");

	if (state & SCARD_STATE_EXCLUSIVE)
		strcat(state_string, " Exclusive access,");

	if (state & SCARD_STATE_INUSE)
		strcat(state_string, " Shared access,");

	if (state & SCARD_STATE_MUTE)
		strcat(state_string, " Silent card,");

	state_string_len = strlen(state_string);
	if (state_string[state_string_len - 1] == ',')
		state_string[state_string_len - 1] = 0;
	else
		strcat(state_string, "UNDEFINED");

	return state_string;
}

const char *pcsc_stringify_error(long err)
{
	static char default_buf[64];
	unsigned index;
	if ((err & 0xFFFFFF00) == 0x80100000)
	{
		index = err & 0xFF;
		if (index<0x30)
			return PCSC_ERROR[index];
		if (index >= 0x66 || index <= 0x69)
			return PCSC_ERROR[index - 0x66 + 0x30];
	}
	else if (err<1700)
	{
		return "SYSTEM ERROR";
	}
	sprintf(default_buf, "Unknown error (0x%08X)", (unsigned)err);
	return default_buf;
}

int pcsc_warm_reset(sc_context *cr)
{
	LONG lReturn;
	
	lReturn = SCardReconnect(cr->hCard,
		SCARD_SHARE_SHARED,
		/* SCARD_SHARE_EXCLUSIVE, */
		cr->proto,
		SCARD_RESET_CARD,
		&(cr->proto));

	if (lReturn == SCARD_S_SUCCESS)
	{
		log_printf(LOG_INFO, "Reconnected reader");
		cr->isConnected = 1;
		return 1;
	}

	log_printf(LOG_ERROR, "Failed to reconnect reader: %s (error 0x%08x).",
		pcsc_stringify_error(lReturn),
		lReturn);
	cr->isConnected = 0;
	return 0;
}
