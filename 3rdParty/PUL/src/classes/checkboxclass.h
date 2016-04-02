/* Poly
   Useful
   Lib */

#ifndef __PUL_SRC_CHECKBOXCLASS_H__
#define __PUL_SRC_CHECKBOXCLASS_H__

#include <pul/pul.h>

typedef struct
{
	PUU8*	pLabel;
	PUU32	bChecked;
} pusCheckBoxClassData;

PUU32 puCheckBoxClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 );

#endif