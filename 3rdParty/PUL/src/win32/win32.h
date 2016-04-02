/* Poly
   Useful
   Lib */

#ifndef __PUL_SRC_WIN32_H__
#define __PUL_SRC_WIN32_H__

#define PUL_MAX_WINDOWS 32

typedef struct
{
	HDC		hdc;
	HPEN	hpen;
	HPEN	invispen;
	HBRUSH	hbrush;

	PUU32	PenX;
	PUU32	PenY;
} pusWin32RastPort;

void puWin32InitRastPort( pusWin32RastPort* _pRastPort );
void puWin32DeleteRastPort( pusWin32RastPort* _pRastPort );

void puCheckWindowSize( PUU32 _Window );

#endif