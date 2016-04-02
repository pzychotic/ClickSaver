/* Poly
   Useful
   Lib */

#ifndef __PUL_SRC_GROUPCLASS_H__
#define __PUL_SRC_GROUPCLASS_H__

#include <pul/pul.h>

typedef struct
{
	PUU32 Dummy;	
} pusGroupClassData;

PUU32 puGroupClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 );

#endif