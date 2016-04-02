/* Poly
   Useful
   Lib */

#ifndef __PUL_SRC_CONTROLCLASS_H__
#define __PUL_SRC_CONTROLCLASS_H__

#include <pul/pul.h>

typedef struct
{
	PUU32 PosSizeParams[11];

	PUU32 Frame;
	PUU32 State;
	PULID WindowObj;
	PUU32 IsButton;
	PUU32 bPressed;
	PUU32 Weight;
	PUU32 bHidden;
	PUU32 bKeepRoom;
	PUU32 bWeak;
	PUU8* pLabel;
	PUU32 bClicked;
	PUU32 bKBFocus;
	PUS16 DeferRelayoutCount;
	PUU8 bRelayout;
	PUU32 MinMaxUpdateFlags;
} pusControlClassData;
/*
#define PUA_CONTROL_POSX		PU_CONTROLCLASS_ATT + 1
#define PUA_CONTROL_POSY		PU_CONTROLCLASS_ATT + 2
#define PUA_CONTROL_WIDTH		PU_CONTROLCLASS_ATT + 3
#define PUA_CONTROL_HEIGHT		PU_CONTROLCLASS_ATT + 4
#define PUA_CONTROL_MINWIDTH	PU_CONTROLCLASS_ATT + 5
#define PUA_CONTROL_MAXWIDTH	PU_CONTROLCLASS_ATT + 6
#define PUA_CONTROL_MINHEIGHT	PU_CONTROLCLASS_ATT + 7
#define PUA_CONTROL_MAXHEIGHT	PU_CONTROLCLASS_ATT + 8*/

PUU32 puControlClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 );

#endif