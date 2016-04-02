/* Poly
   Useful
   Lib */

#ifndef __PUL_SRC_TABLECLASS_H__
#define __PUL_SRC_TABLECLASS_H__

#include <pul/pul.h>

typedef struct
{
	PUU32			NumFields;
	pusTableField*	pFields;
} pusTableClassData;

PUU32 puTableClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 );

#endif