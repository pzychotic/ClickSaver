/* Poly
   Useful
   Lib */

#ifndef __PUL_SRC_SCROLLINGCLASS_H__
#define __PUL_SRC_SCROLLINGCLASS_H__

#include <pul/pul.h>

typedef struct
{
//	PULID					Contents;
//	PULID					Root;

	PULID					HScroller;
	PULID					VScroller;
	PUU8					bShowHScroller;
	PUU8					bShowVScroller;

	PUU8					bAutoHideScrollers;

	PUU8					bNoframes;

//	pusObjectCollection*	pCol;
	PULID					/*Container,*/ Empty;
	PUU32					Width, Height;
	PUU32					HVis, VVis;
	PUU32					PosX, PosY;

	PUU32					MinMaxUpdateFlags;

} pusScrollingClassData;

PUU32 puScrollingClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 );

#endif