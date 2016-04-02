/* Poly
   Useful
   Lib */

#ifndef __PUL_SRC_SLIDERCLASS_H__
#define __PUL_SRC_SLIDERCLASS_H__

#include <pul/pul.h>

typedef struct
{
	PUU32	bVertical;
	PUS32	Min;
	PUS32	Max;
	PUS32	Vis;
	PUS32	Val;
	PUU32	bLockKnobPos;
	PUS32	KnobPos, KnobSize, KnobMaxPos, TotalSize;
	PUS32	BackupPos, BackupMousePos;
	PUS32	BackupVal;
} pusSliderClassData;

PUU32 puSliderClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 );

#endif