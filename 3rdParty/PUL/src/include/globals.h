/* Poly
   Useful
   Lib */

#ifndef __PUL_SRC_GLOBALS_H__
#define __PUL_SRC_GLOBALS_H__

#include <pul/pul.h>

#define PU_MAX_APPMESSAGES 128

typedef struct
{
	pusList				ClassList;
	pusAppMessage		AppMessageFIFO[ PU_MAX_APPMESSAGES ];
	PUU32				FirstAppMessage, LastAppMessage, NumAppMessages;
	PUU8*				pLogBuffer;
	PUAPPMESSCALLBACK	pAppMessCallBack;
} pusGlobals;

extern pusGlobals g_Globals;

#endif