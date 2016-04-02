/* Poly
   Useful
   Lib */

#ifndef __PUL_SRC_LISTCLASS_H__
#define __PUL_SRC_LISTCLASS_H__

#include <pul/pul.h>

typedef struct
{
	PUU32		NumRecords;
	pusList		List;
	pusNode*	pCurrentRecord;
	pusNode*	pNewRecord;
	PUU32		CurrentField;
} pusListClassData;

PUU32 puListClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 );

#endif