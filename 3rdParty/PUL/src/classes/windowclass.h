/* Poly
   Useful
   Lib */

#ifndef __PUL_SRC_WINDOWCLASS_H__
#define __PUL_SRC_WINDOWCLASS_H__

#include <pul/pul.h>

typedef struct
{
	PUU32	Window;
	PUU32	Parent;
	PUU32	RenderRastPort;
	PUU32	RastPort;

	PUU32	XPos, YPos;
	PUU32	Width, Height;
	char*	pTitle;
	PUU32	bNoClose, bForceTop, bIconifyBox, bIconified, bPopup;
	PUU32	bHorCenter, bVerCenter;

	PULID	Contents;
	PULID	ActiveControl;

	PUU32	MinWidth, MaxWidth;
	PUU32	MinHeight, MaxHeight;
	PUU32	WantedHeight;

	PUU32	bDeferUpdate;
	PUU32	DeferState;
	PUU32	bRelayouted;
} pusWindowClassData;

PUU32 puWindowClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 );

#endif