/* Poly
   Useful
   Lib */

#ifndef __PUL_SRC_TEXTCLASS_H__
#define __PUL_SRC_TEXTCLASS_H__

#include <pul/pul.h>

typedef struct
{
	PUU8* pString;
	PUU32 TextX, TextY;
	PUU32 MinWidth;
	PUU32 CenterMode;
	PUU32 bFixedWidth;
} pusTextClassData;

PUU32 puTextClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 );

#endif