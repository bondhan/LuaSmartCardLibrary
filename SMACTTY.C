/* *************************************************************************

   Smart Card Digging Utilities. 
   Dummy APDU terminal

   Written by Ilya O. Levin, http://www.literatecode.com    

   This source code released for free "as is" under the whatever ABC 
   license without warranty or liability of any kind. Use it at your 
   own risk any way you like for legitimate purposes only. 

   ************************************************************************* 
*/

#pragma warning (push, 1)
#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <io.h>
#pragma warning (pop)
#include "pcscwrap.h"


void dump_resp (sc_context *);
int wrap2bin (char *);

int main(int argc, char* argv[])
{
  sc_context ctx = {0}; 
  char cmd[255];
  long rc;
  char *c, *a; 


  printf("SMACTTY. Smart Card Digging Utilities Version 0.50725 by "
         "Ilya O. Levin\nSimple APDU console for smart cards\n"
         "\n");

  if (argc>1) c = argv[1]; else
  {
    printf("Usage: smactty reader\n\n"
           "Examples:\n\t smactty Gemplus\n"
           "\t smactty \"lto e-gate\"\n"
           "\n");
    c=sc_listreaders();
    if (c)
    {
      printf("Available readers:\n");
      a = strtok(c, "\n"); 
      while (a != NULL) { printf("* %s\n", a); a = strtok(NULL, "\n");}
    }
    else printf("Unable to list smart card readers\n");
    free (c);
    return 0;
  }

  printf("* connecting to \"%s\" - ", c);
  rc = sc_init(&ctx, c); 
  if (rc == SC_OK)
  {
    printf("ok\n* Active reader: \"%s\"\n", ctx.rdr);
   
    while (1)
    {
      printf("\n* Enter command [empty for exit]:\n");
      fgets( cmd, sizeof(cmd)-2, stdin);
      rc = wrap2bin(cmd);
      if (rc == 0) break;
      rc = sc_rawsend(&ctx, cmd, (BYTE) rc);
      printf("\n%s\n", rc_symb(rc));
      dump_resp(&ctx);
    }
    sc_finish(&ctx);
    printf("* done\n");
  } 
  else printf("failed (%s)\n", rc_symb(rc));

  return 0;
} /* main */


/* -------------------------------------------------------------------------- */
void dump_resp (sc_context *ctx)
{
  register unsigned int i, j;

  for (i = 0; i<sizeof(ctx->sw); i++)
  {
    printf("%02x ", ctx->sw[i]);
    if ( (i % 16) == 15) 
    {
       for (j=i-15;j<i;j++) printf("%c", (isprint(ctx->sw[j]))?ctx->sw[j]:'.');
       printf("\n");
    }
  }
} /* dump_resp */


/* -------------------------------------------------------------------------- */
int wrap2bin (char *cmd)
{
  char *hex = "0123456789abcdef";
  register unsigned int i, j, x, t;

  i = j = x = 0;
  while (cmd[i]>0)
  {
    if ( (cmd[i] == 0x0a) || (cmd[i] == 0x0d) ) break;
    if ( cmd[i] == ' ') {i++; continue;}
    if ( cmd[i] == '-') { cmd[j++] = cmd[++i]; x = 0;}
    else 
    {
      t=0; while (hex[t]) if (tolower(cmd[i])==hex[t]) break; else t++;
      if (hex[t]==0) break;
      if ( x == 1) { cmd[j] ^= (unsigned char) t; j++; x = 0;} 
      else { cmd[j] = (unsigned char) t; cmd[j]<<=4; x = 1;}
    }
    i++;
  }

  return j;
} /* wrap2bin */
