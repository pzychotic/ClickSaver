/* Poly
   Useful
   Lib */

#ifndef __PUL_SRC_CONTAINERCLASS_H__
#define __PUL_SRC_CONTAINERCLASS_H__

#include <pul/pul.h>

typedef struct
{
	PULID	Contents;
} pusContainerClassData;

PUU32 puContainerClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 );

#endif