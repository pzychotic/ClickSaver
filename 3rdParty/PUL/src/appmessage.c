/* Poly
   Useful
   Lib */

#include <pul/pul.h>
#include "include/pul.h"
#include "include/Globals.h"

PUU32 puPostAppMessage( PUU32 _Message, PUU32 _Param )
{
	pusAppMessage* pAppMessage;

	if( g_Globals.pAppMessCallBack )
		if( g_Globals.pAppMessCallBack( _Message, _Param ) )
			return TRUE;

	if( g_Globals.FirstAppMessage == g_Globals.LastAppMessage && g_Globals.NumAppMessages )
		return FALSE;

	pAppMessage = &g_Globals.AppMessageFIFO[ g_Globals.LastAppMessage ];
	pAppMessage->Message = _Message;
	pAppMessage->Param = _Param;

	g_Globals.LastAppMessage++;
	if( g_Globals.LastAppMessage >= PU_MAX_APPMESSAGES )
		g_Globals.LastAppMessage = 0;

	g_Globals.NumAppMessages++;

	return TRUE;
}

pusAppMessage* puGetAppMessage()
{
	pusAppMessage* pAppMessage;

	if( !g_Globals.NumAppMessages )
		return NULL;

	pAppMessage = &g_Globals.AppMessageFIFO[ g_Globals.FirstAppMessage ];

	g_Globals.FirstAppMessage++;
	if( g_Globals.FirstAppMessage >= PU_MAX_APPMESSAGES )
		g_Globals.FirstAppMessage = 0;

	g_Globals.NumAppMessages--;

	return pAppMessage;
}

pusAppMessage* puWaitAppMessages()
{
	while( !g_Globals.NumAppMessages )
		puWaitMessages();

	return puGetAppMessage();
}

void puSetAppMessCallBack( PUAPPMESSCALLBACK _pAppMessCallBack )
{
	g_Globals.pAppMessCallBack = _pAppMessCallBack;
}