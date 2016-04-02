/* Poly
   Useful
   Lib */

#ifndef __PUL_SRC_H__
#define __PUL_SRC_H__

#include <pul/pul.h>
#include <string.h>

PUU32 puCheckPointInRect( pusRect* _pRect, PUS32 _X, PUS32 _Y );

/* Low level window functions */
PUU32	puSysInit();
PUU32	puCreateWindow( PULID _WindowObj, PUU32 _Parent, char* pTitle, pusRect* _pRect, PUU32 _bNoClose, PUU32 _bIconify, PUU32 _bForceTop, PUU32 _bPopup );
void	puDestroyWindow( PUU32 _Window );
void	puGetWindowRect( PUU32 _Window, pusRect* _pWindowRect );
void	puGetWindowInnerRect( PUU32 _Window, pusRect* _pWindowRect );
void	puGetScreenRect( pusRect* _pScreenRect );
void	puRedrawWindowRect( PUU32 _Window, pusRect* _pRect );
void	puSetCapture( PUU32 _Window );
void	puCheckWindowSize( PUU32 _Window );
void	puSetWindowPos( PUU32 _Window, PUU32 _XPos, PUU32 _YPos );
void	puShowWindow( PUU32 _Window );
void	puIconifyWindow( PUU32 _Window );
void	puWaitMessages();

/* Clipboard functions */
PUU8*	puGetClipboardText();
void	puPutClipboardText( PUU8* _pText, PUU32 _TextSize );

/* Low level drawing functions */
PUU32	puGetRastPort( PUU32 _Window );
void	puReleaseRastPort( PUU32 _Window, PUU32 _RastPort );

void	puGetPenPos( PUU32 _RastPort, PUU32* _pXPos, PUU32* _pYPos );
void	puSetDrawPen( PUU32 _RastPort, PUU8 _Red, PUU8 _Green, PUU8 _Blue );
void	puSetFillPen( PUU32 _RastPort, PUU8 _Red, PUU8 _Green, PUU8 _Blue );
void	puMove( PUU32 _RastPort, PUU32 _XPos, PUU32 _YPos );
void	puLine( PUU32 _RastPort, PUU32 _XPos, PUU32 _YPos );
void	puRect( PUU32 _RastPort, pusRect* _pRect );
void	puInvRect( PUU32 _RastPort, pusRect* _pRect );
void	puTriangle( PUU32 _RastPort, PUU32 _X1, PUU32 _Y1, PUU32 _X2, PUU32 _Y2, PUU32 _X3, PUU32 _Y3 );

void	puSetFont( PUU32 _RastPort, PUU8 _Type );
void	puTextSize( PUU32 _RastPort, PUU8* _pText, PUU32 _Length, pusRect* _pRect );
PUU32	puTextFit( PUU32 _RastPort, PUU8* _pText, PUU32 _Length, PUU32 Size );
void	puDrawText( PUU32 _RastPort, PUU8* _pText, PUU32 _Length, PUU32 _XPos, PUU32 _YPos );

void	puSetClipping( PUU32 _RastPort, pusRect* pRect );

PUU32	puAllocBitmap( PUU32 _RastPort, PUU32 _Width, PUU32 _Height, PUU32 _PixFormat, void* _pData );
void	puFreeBitmap( PUU32 _Bitmap );
void	puDrawBitmap( PUU32 _RastPort, PUU32 _Bitmap, pusRect* _pRect );

/* A bit higher level drawing functions */
void	puHorGradientRect( PUU32 _RastPort, pusRect* _pRect, PUU8 _R1, PUU8 _G1, PUU8 _B1, PUU8 _R2, PUU8 _G2, PUU8 _B2 );
void	puVerGradientRect( PUU32 _RastPort, pusRect* _pRect, PUU8 _R1, PUU8 _G1, PUU8 _B1, PUU8 _R2, PUU8 _G2, PUU8 _B2 );

/* Frame functions */
void	puDrawFrame( PUU32 _RastPort, PUU32 _Type, PUU32 _State, pusRect* _pRect, PUU8* _pLabel );
void	puDrawFrameBG( PUU32 _RastPort, PUU32 _Type, PUU32 _State, pusRect* _pRect );
void	puGetFrameInnerRect( PUU32 _RastPort, PUU32 _Type, PUU32 _State, pusRect* _pRect, PUU8* _pLabel );
void	puGetFrameOuterRect( PUU32 _RastPort, PUU32 _Type, PUU32 _State, pusRect* _pRect, PUU8* _pLabel );

/* Icon function */
void	puDrawIcon( PUU32 _RastPort, PUU32 _Type, PUU32 _State, PUU32 _Left, PUU32 _Top );
void	puGetIconSize( PUU32 _Type, PUU32 _State, pusRect* _pRect );

/* Icon types */
enum
{
	PUICON_CHECKBOX
};

/* App messages functions */
void	puSendAppMessage( PUU32 _Message, PUU32 _Param );

#endif