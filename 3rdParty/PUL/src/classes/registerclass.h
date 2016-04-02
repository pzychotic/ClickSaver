/* Poly
   Useful
   Lib */

#ifndef __PUL_SRC_REGISTERCLASS_H__
#define __PUL_SRC_REGISTERCLASS_H__

#include <pul/pul.h>

/* One tab */
typedef struct
{
	pusNode	Node;
	PUU8*	pLabel;
	PULID	Object;
	pusRect	Rect;
	PUU32	LabelX, LabelY;
} pusRegisterTab;

typedef struct
{
	pusList			TabList;
	pusRegisterTab*	pSelectedTab;
	pusRegisterTab*	pCurrentTab;
	PUU32	TabsWidth, TabsHeight;
	PUU32	bFixedWidth, bFixedHeight;
} pusRegisterClassData;

PUU32 puRegisterClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 );

#endif