/* Poly
   Useful
   Lib */

#ifndef __PUL_SRC_TEXTENTRYCLASS_H__
#define __PUL_SRC_TEXTENTRYCLASS_H__

#include <pul/pul.h>

typedef struct
{
	PUU8*	pBuffer;
	PUU8*	pFirstChar;
	PUU32	BufferSize;
	PUU32	bEditMode;
	PUU32	CursorCharPos;
	PUS32	LastCursorPos;
	PUS32	CursorPixPos, SelectionEndPixPos;
	PUU32	SelectionEndCharPos;
	PUS32	LastEndSelPos;
	PUU32	bSelectionMode;
	PUU32	bReadOnly;
	PUU32	bHilight;
	PUU32	bNumeric;
	PUU32	Value, Min, Max, Format;
} pusTextEntryClassData;

PUU32 puTextEntryClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 );

#endif