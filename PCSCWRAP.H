/* ************************************************************************

   Smart Card Digging Utilities
   PC/SC Wrappers

   Written by Ilya O. Levin, http://www.literatecode.com    

   This source code released for free "as is" under the whatever ABC 
   license without warranty or liability of any kind. Use it at your 
   own risk any way you like for legitimate purposes only. 

   ************************************************************************
*/
#pragma once
#pragma comment( lib, "winscard" )
#pragma warning (disable: 4514)     // "unref inline fn has been removed"
#pragma warning (push, 1)
#define WINDOWS_MEAN_AND_LEAN
#include <windows.h>
#include <winscard.h>
#include "bytestring.h"
#pragma warning (pop)

#define SC_OK		SCARD_S_SUCCESS
#define SC_BAD		0xBEDA

#define SCARD_MYPROTOSET SCARD_PROTOCOL_T0 |   \
                         SCARD_PROTOCOL_T1 |   \
                         SCARD_PROTOCOL_RAW |  \
                         SCARD_PROTOCOL_OPTIMAL

#define SCARD_PCI_NULL NULL

#define MAX_PCSC_READ_LENGTH 1024


#ifdef __cplusplus
extern "C" { 
#endif

typedef struct
{
  SCARDCONTEXT hCtx;    
  SCARDHANDLE  hCard;
  LPCSCARD_IO_REQUEST proto;
  BYTE sw[256];              // output of the last operation
  DWORD  rdrsz;              // length of reader's name
  LPBYTE rdr;                // name of the connected reader
  //BYTE readerName[100];
  BYTE CLA;                  // CLA byte
  BYTE lCLA;                 // last operation's class (CLA byte) 
  unsigned short rw;         // status word of last operation
  BYTE atr[33];
  DWORD atrLen;
  DWORD pdwState;		
  int isConnected;
} sc_context;

//char *sc_listreaders (void);
//LONG sc_init (sc_context *, char *);
//LONG sc_init_u (sc_context *, char *);
//LONG sc_done (sc_context *, DWORD);
//#define sc_finish(x) sc_done(x, SCARD_LEAVE_CARD)
//LONG sc_selectfile (sc_context *, DWORD);
//LONG sc_getresponse (sc_context *);
//LONG sc_readdata (sc_context *, BYTE, DWORD);
//LONG sc_updatedata (sc_context *, BYTE *, BYTE, DWORD);
//LONG sc_rawsend (sc_context *, void *, BYTE);
//char *rc_symb (LONG); 
//LONG CheckCardStatus(sc_context *ctx, char *msg);
//unsigned short pcsc_transmit(sc_context *ctx, bytestring_t *command, bytestring_t *result);
static const char *pcsc_stringify_protocol(DWORD proto);
const char *pcsc_stringify_error(long err);
//int pcsc_warm_reset(sc_context *cr);

LONG sc_reconnect(sc_context *ctx);

#ifdef __cplusplus
} 
#endif
