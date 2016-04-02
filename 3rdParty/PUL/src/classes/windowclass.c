/* Poly
   Useful
   Lib */

#include <pul/pul.h>
#include "windowclass.h"
#include "../include/pul.h"

PUU32 puWindowGetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr );
PUU32 puWindowSetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr, PUU32 _Val );

enum
{
	DEFER_NOP,
	DEFER_REDRAW,
	DEFER_RECALC
};

PUU32 puWindowClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 )
{
	pusWindowClassData* pData;
	pData = ( pusWindowClassData* )_pData;

	switch( _MethodID )
	{
		/* Base methods */
		case PUM_NEW:
			pData->Window = 0;
			pData->Parent = 0;
			pData->pTitle = NULL;
			pData->Contents = 0;
			pData->ActiveControl = 0;
			pData->RastPort = 0;
			pData->RenderRastPort = 0;
			pData->MinWidth = 50;
			pData->MinHeight = 25;
			pData->MaxWidth = 32768;
			pData->MaxHeight = 32768;
			pData->XPos = 50;
			pData->YPos = 50;
			pData->Width = 0;
			pData->Height = 0;
			pData->bNoClose = FALSE;
			pData->bForceTop = FALSE;
			pData->bIconifyBox = FALSE;
			pData->bIconified = FALSE;
			pData->bPopup = FALSE;
			pData->bHorCenter = FALSE;
			pData->bVerCenter = FALSE;
			pData->bDeferUpdate = FALSE;
			pData->DeferState = DEFER_NOP;
			pData->WantedHeight = 0;
			pData->bRelayouted = FALSE;
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		case PUM_DELETE:
			if( pData->Contents )
				puDoMethod( pData->Contents, PUM_REMREF, 0, 0 );
				//puDeleteObject( pData->Contents );

			if( pData->Window && pData->RastPort )
				puReleaseRastPort( pData->Window, pData->RastPort );

			if( pData->Window )
				puDestroyWindow( pData->Window );

			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		case PUM_SET:
			puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
			return puWindowSetAttr( _Object, _Class, _pData, _Param1, _Param2 );

		case PUM_GET:
			return puWindowGetAttr( _Object, _Class,_pData, _Param1 );

		/* Window methods */
		case PUM_WINDOW_CALCMINMAX:
			if( pData->Contents )
			{
				puDoMethod( pData->Contents, PUM_CONTROL_CALCMINMAX, _Param1, _Object );

				pData->MinWidth = puGetAttribute( pData->Contents, PUA_CONTROL_MINWIDTH );
				pData->MaxWidth = puGetAttribute( pData->Contents, PUA_CONTROL_MAXWIDTH );
				pData->MinHeight = puGetAttribute( pData->Contents, PUA_CONTROL_MINHEIGHT );
				pData->MaxHeight = puGetAttribute( pData->Contents, PUA_CONTROL_MAXHEIGHT );
			}
			break;

		case PUM_WINDOW_PRELAYOUT:
			if( pData->Contents && pData->Window )
			{
				pusRect Rect;
				PUU32 RastPort;
				puGetWindowInnerRect( pData->Window, &Rect );

				/* Layout only width and X axis positions */
				puSetAttribute( pData->Contents, PUA_CONTROL_WIDTH, Rect.Width );
				RastPort = puGetRastPort( pData->Window );
				puDoMethod( pData->Contents, PUM_CONTROL_PRELAYOUT, RastPort, _Param1 );
				puReleaseRastPort( pData->Window, RastPort );

				/* Retrieve desired height */
				pData->MinHeight = puGetAttribute( pData->Contents, PUA_CONTROL_MINHEIGHT );
				pData->MaxHeight = puGetAttribute( pData->Contents, PUA_CONTROL_MAXHEIGHT );
			}
			break;

		case PUM_WINDOW_LAYOUT:
			/* Get the current size of the window,
			if the window is open */
			if( pData->Contents && pData->Window )
			{
				pusRect Rect;
				PUU32 RastPort;
				puGetWindowInnerRect( pData->Window, &Rect );

				pData->bRelayouted = TRUE;

				/* Set object size and position */
			//	puSetAttribute( pData->Contents, PUA_CONTROL_POSX, 2 );
			//	puSetAttribute( pData->Contents, PUA_CONTROL_POSY, 2 );
				puSetAttribute( pData->Contents, PUA_CONTROL_WIDTH, Rect.Width );
				puSetAttribute( pData->Contents, PUA_CONTROL_HEIGHT, Rect.Height );

				RastPort = puGetRastPort( pData->Window );
				puDoMethod( pData->Contents, PUM_CONTROL_LAYOUT, RastPort, 0 );
				puReleaseRastPort( pData->Window, RastPort );
			}
			break;

		case PUM_WINDOW_RENDER:
			{
			//	pusRect Rect;
				pData->RenderRastPort = _Param1;

			/*	puSetDrawPen( _Param1, 0, 0, 0 );
				puSetFillPen( _Param1, 0, 255, 0 );

				puGetWindowInnerRect( pData->Window, &Rect );
				Rect.X = 0;
				Rect.Y = 0;
				puRect( _Param1, &Rect );*/

				if( pData->Contents )
					puDoMethod( pData->Contents, PUM_CONTROL_RENDER, pData->RenderRastPort, 0 );

				pData->RenderRastPort = 0;
				pData->bRelayouted = FALSE;
			}
			break;

		case PUM_WINDOW_GETRASTPORT:
			if( pData->RenderRastPort )
				return pData->RenderRastPort;

			if( pData->RastPort )
				return pData->RastPort;

			return 0;

		case PUM_WINDOW_REDRAWRECT:
			if( pData->Window )
			{
				if( pData->bDeferUpdate )
				{
					if( pData->DeferState < DEFER_REDRAW )
						pData->DeferState = DEFER_REDRAW;
				}
				else
					puRedrawWindowRect( pData->Window, ( pusRect* )_Param1 );
			}
			break;

		case PUM_WINDOW_MOUSEEVENT:
			if( pData->ActiveControl )
				puDoMethod( pData->ActiveControl, PUM_CONTROL_MOUSEEVENT, _Param1, _Object );
			else if( pData->Contents )
				puDoMethod( pData->Contents, PUM_CONTROL_MOUSEEVENT, _Param1, _Object );
			break;

		case PUM_WINDOW_KEYBOARDEVENT:
			if( pData->ActiveControl )
				puDoMethod( pData->ActiveControl, PUM_CONTROL_KEYBOARDEVENT, _Param1, _Object );
			else if( pData->Contents )
				puDoMethod( pData->Contents, PUM_CONTROL_KEYBOARDEVENT, _Param1, _Object );
			break;

		case PUM_WINDOW_GETRECT:
			if( pData->Window )
				puGetWindowRect( pData->Window, ( pusRect* )_Param1 );
			break;

		case PUM_WINDOW_GETINNERRECT:
			if( pData->Window )
				puGetWindowInnerRect( pData->Window, ( pusRect* )_Param1 );
			break;

		case PUM_WINDOW_RECALC:
			if( pData->Window )
			{
				if( pData->bDeferUpdate )
				{
					if( pData->DeferState < DEFER_RECALC )
						pData->DeferState = DEFER_RECALC;
				}
				else
				{
					puDoMethod( _Object, PUM_WINDOW_CALCMINMAX, pData->RastPort, 0 );
					puDoMethod( _Object, PUM_WINDOW_PRELAYOUT, pData->RastPort, 0 );
					puCheckWindowSize( pData->Window );
					puDoMethod( _Object, PUM_WINDOW_LAYOUT, pData->RastPort, 0 );
					puDoMethod( _Object, PUM_WINDOW_RENDER, pData->RastPort, 0 );
				}
			}
			break;

		default:
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
	}

	return 0;
}


PUU32 puWindowGetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr )
{
	pusWindowClassData* pData;
	pData = ( pusWindowClassData* )_pData;

	switch( _Attr )
	{
		case PUA_WINDOW_OPENED:
			return pData->Window ? TRUE : FALSE;

		case PUA_WINDOW_WIDTH:
			return pData->Width;

		case PUA_WINDOW_HEIGHT:
			return pData->Height;

		case PUA_WINDOW_CONTENTS:
			return pData->Contents;

		case PUA_WINDOW_ACTIVECONTROL:
			return pData->ActiveControl;

		case PUA_WINDOW_HANDLE:
			return pData->Window;

		case PUA_WINDOW_PARENTHANDLE:
			return pData->Parent;

		case PUA_CONTROL_MINWIDTH:
			return pData->MinWidth;

		case PUA_CONTROL_MAXWIDTH:
			return pData->MaxWidth;

		case PUA_CONTROL_MINHEIGHT:
			return pData->MinHeight;

		case PUA_CONTROL_MAXHEIGHT:
			return pData->MaxHeight;

		default:
			return puDoSuperMethod( _Object, _Class, PUM_GET, _Attr, 0 );
	}
}


PUU32 puWindowSetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr, PUU32 _Val )
{
	pusWindowClassData* pData;
	pData = ( pusWindowClassData* )_pData;

	switch( _Attr )
	{
		case PUA_WINDOW_OPENED:
			if( _Val )
			{
				pusRect Rect, Rect2;

				if( pData->Window )
					return TRUE;

				Rect.X = pData->XPos;
				Rect.Y = pData->YPos;
				Rect.Width = pData->Width;
				Rect.Height = pData->Height;

				if( !( pData->Window = puCreateWindow( _Object, pData->Parent, pData->pTitle, &Rect, pData->bNoClose, pData->bIconifyBox, pData->bForceTop, pData->bPopup ) ) )
					return FALSE;

				pData->RastPort = puGetRastPort( pData->Window );
				puDoMethod( _Object, PUM_WINDOW_CALCMINMAX, pData->RastPort, 0 );
				puDoMethod( _Object, PUM_WINDOW_PRELAYOUT, pData->RastPort, 0 );
				puDoMethod( _Object, PUM_WINDOW_LAYOUT, pData->RastPort, 0 );

				puCheckWindowSize( pData->Window );
				puGetWindowRect( pData->Window, &Rect );
				puGetScreenRect( &Rect2 );

				if( pData->bHorCenter )
					pData->XPos = ( Rect2.Width - Rect.Width ) >> 1;

				if( pData->bVerCenter )
					pData->YPos = ( Rect2.Height - Rect.Height ) >> 1;

				puSetWindowPos( pData->Window, pData->XPos, pData->YPos );

				if( pData->bIconified )
					puIconifyWindow( pData->Window );
				else
					puShowWindow( pData->Window );
			}
			else
			{
				pusRect Rect;
				PUU32 Window;

				if( !pData->Window )
					return TRUE;

				if( pData->RastPort )
					puReleaseRastPort( pData->Window, pData->RastPort );

				/* Save window position and size */
				puGetWindowRect( pData->Window, &Rect );
				pData->XPos = Rect.X;
				pData->YPos = Rect.Y;
				pData->Width = Rect.Width;
				pData->Height = Rect.Height;

				Window = pData->Window;
				pData->Window = 0;
				puDestroyWindow( Window );
			}
			break;

		case PUA_WINDOW_XPOS:
			pData->XPos = _Val;
			break;

		case PUA_WINDOW_YPOS:
			pData->YPos = _Val;
			break;

		case PUA_WINDOW_WIDTH:
			pData->Width = _Val;
			break;

		case PUA_WINDOW_HEIGHT:
			pData->Height = _Val;
			break;

		case PUA_WINDOW_NOCLOSEBOX:
			pData->bNoClose = _Val;
			break;

		case PUA_WINDOW_FORCETOP:
			pData->bForceTop = _Val;
			break;

		case PUA_WINDOW_ICONIFYBOX:
			pData->bIconifyBox = _Val;
			break;

		case PUA_WINDOW_ICONIFIED:
			pData->bIconified = _Val;

			if( pData->Window )
			{
				if( pData->bIconified )
					puIconifyWindow( pData->Window );
				else
					puShowWindow( pData->Window );
			}
			break;

		case PUA_WINDOW_TITLE:
			pData->pTitle = ( PUU8* )_Val;
			break;

		case PUA_WINDOW_CONTENTS:
			pData->Contents = _Val;

			if( pData->Contents )
				puDoMethod( pData->Contents, PUM_ADDREF, 0, 0 );
			break;

		case PUA_WINDOW_ACTIVECONTROL:
			pData->ActiveControl = _Val;
			if( pData->ActiveControl )
			{
				if( !puGetAttribute( pData->ActiveControl, PUA_CONTROL_ACTIVEWEAK ) )
					puSetCapture( pData->Window );
				else
					puSetCapture( NULL );
			}
			else
				puSetCapture( NULL );
			break;

		case PUA_WINDOW_HANDLE:
			pData->Window = _Val;
			break;

		case PUA_WINDOW_PARENTHANDLE:
			pData->Parent = _Val;
			break;

		case PUA_WINDOW_HORCENTER:
			pData->bHorCenter = _Val;
			break;

		case PUA_WINDOW_VERCENTER:
			pData->bVerCenter = _Val;
			break;

		case PUA_WINDOW_DEFERUPDATE:
			pData->bDeferUpdate = _Val;

			if( !pData->bDeferUpdate )
			{
				switch( pData->DeferState )
				{
					case DEFER_REDRAW:
						puDoMethod( _Object, PUM_WINDOW_REDRAWRECT, NULL, NULL );
						break;

					case DEFER_RECALC:
						puDoMethod( _Object, PUM_WINDOW_RECALC, NULL, NULL );
						break;
				}

				pData->DeferState = DEFER_NOP;
			}
			break;


		case PUA_WINDOW_POPUP:
			pData->bPopup = _Val;
			break;
	}

	return TRUE;
}