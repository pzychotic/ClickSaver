/* Poly
   Useful
   Lib */

#ifndef __PUL_SRC_IMAGECLASS_H__
#define __PUL_SRC_IMAGECLASS_H__

#include <pul/pul.h>

typedef struct
{
	PUU32	Width, Height;
	PUU32	Image;
	PUU32	PixFormat;
	PUU8	BGRed, BGGreen, BGBlue;
	void*	pData;
} pusImageClassData;

PUU32 puImageClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 );

#endif