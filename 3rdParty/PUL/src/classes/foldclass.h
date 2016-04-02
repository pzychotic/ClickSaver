/* Poly
   Useful
   Lib */

#ifndef __PUL_SRC_FOLDCLASS_H__
#define __PUL_SRC_FOLDCLASS_H__

#include <pul/pul.h>

typedef struct
{
	PULID	Contents;
	PUU8*	pLabel;
	PUU32	bFolded;
	PUU32	bHilighted;
	PUU32	BarHeight;
} pusFoldClassData;

PUU32 puFoldClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 );

#endif