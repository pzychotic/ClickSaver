/* Poly
   Useful
   Lib */

#ifndef __PUL_SRC_SPACERCLASS_H__
#define __PUL_SRC_SPACERCLASS_H__

#include <pul/pul.h>

typedef struct
{
	PUU32 MinWidth, MaxWidth;
	PUU32 MinHeight, MaxHeight;
} pusSpacerClassData;

PUU32 puSpacerClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 );

#endif