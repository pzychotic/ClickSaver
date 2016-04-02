/* Poly
   Useful
   Lib */

#ifndef __PUL_SRC_VERGROUPCLASS_H__
#define __PUL_SRC_VERGROUPCLASS_H__

#include <pul/pul.h>

typedef struct
{
	PUU32 NumChild;
	PUU32 Spacing;
	PUU32 CenterMode;
	PUU32 bFixedWidth, bFixedHeight;
} pusVerGroupClassData;

PUU32 puVerGroupClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 );

typedef struct
{
	PULID Object;
	PUU32 State;
	PUU32 Weight;
	PUU32 MinWidth, MaxWidth;
	PUU32 MinHeight, MaxHeight;
} pusVGroupLayoutInfo;

/* Layouted object states */
#define PULAYOUT_FREE		0
#define PULAYOUT_BLOCKEDMAX	1
#define PULAYOUT_BLOCKEDMIN	2

/* General layout state */
#define PULAYOUT_MORESPACE	1
#define PULAYOUT_LESSSPACE	2


#endif