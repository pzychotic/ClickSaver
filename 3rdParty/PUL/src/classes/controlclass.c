/* Poly
   Useful
   Lib */

#include <pul/pul.h>
#include "controlclass.h"
#include "../include/pul.h"

PUU32 puControlGetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr );
PUU32 puControlSetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr, PUU32 _Val );
void puControlCalcMinMax( pusControlClassData* __pData );

#define ParamIndex( x ) ( ( PUA_CONTROL_##x ) - PUA_CONTROL_POSX )

PUU32 puControlClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 )
{
//	PUU32 WantedHeight;
	PUU32 Tmp;
	pusControlClassData* pData;
	pData = ( pusControlClassData* )_pData;

	switch( _MethodID )
	{
		case PUM_NEW:
			memset( pData->PosSizeParams, 0, sizeof( PUU32 ) * 11 );
			pData->WindowObj = 0;
			pData->Frame = PUFRAME_NONE;
			pData->State = PUSTATE_NORMAL;
			pData->IsButton = FALSE;
			pData->bPressed = FALSE;
			pData->Weight = 256;
			pData->bHidden = FALSE;
			pData->bKeepRoom = TRUE;
			pData->bWeak = FALSE;
			pData->pLabel = NULL;
			pData->bClicked = FALSE;
			pData->bKBFocus = FALSE;
			pData->DeferRelayoutCount = 0;
			pData->bRelayout = FALSE;
			pData->MinMaxUpdateFlags = 0xffff;

			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		case PUM_SET:
			puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
			return puControlSetAttr( _Object, _Class, _pData, _Param1, _Param2 );

		case PUM_GET:
			return puControlGetAttr( _Object, _Class,_pData, _Param1 );

		/* Control methods */
		case PUM_CONTROL_GETINNERRECT:
			{
				pusRect* pRect;
				PUU32 RastPort;

				pRect = ( pusRect* )_Param1;

				Tmp = ParamIndex( POSX );
				pRect->X = pData->PosSizeParams[ Tmp ];
				Tmp = ParamIndex( POSY );
				pRect->Y = pData->PosSizeParams[ Tmp ];
				Tmp = ParamIndex( WIDTH );
				pRect->Width = pData->PosSizeParams[ Tmp ];
				Tmp = ParamIndex( HEIGHT );
				pRect->Height = pData->PosSizeParams[ Tmp ];

				RastPort = puDoMethod( pData->WindowObj, PUM_WINDOW_GETRASTPORT, 0, 0 );
				puGetFrameInnerRect( RastPort, pData->Frame, pData->State, pRect, pData->pLabel );
			//	puDoMethod( pData->WindowObj, PUM_WINDOW_RELEASERASTPORT, RastPort, 0 );
				return TRUE;
			}
			break;

		case PUM_CONTROL_GETOUTERRECT:
			{
				pusRect* pRect;
//				PUU32 RastPort;

				pRect = ( pusRect* )_Param1;

				Tmp = ParamIndex( POSX );
				pRect->X = pData->PosSizeParams[ Tmp ];
				Tmp = ParamIndex( POSY );
				pRect->Y = pData->PosSizeParams[ Tmp ];
				Tmp = ParamIndex( WIDTH );
				pRect->Width = pData->PosSizeParams[ Tmp ];
				Tmp = ParamIndex( HEIGHT );
				pRect->Height = pData->PosSizeParams[ Tmp ];

				return TRUE;
			}
			break;

		case PUM_CONTROL_CALCMINMAX:
			pData->WindowObj = _Param2;
			puControlCalcMinMax( pData );
			break;

		case PUM_CONTROL_PRELAYOUT:
			puControlCalcMinMax( pData );
/*			if( WantedHeight = pData->PosSizeParams[ PUA_CONTROL_WANTEDHEIGHT - PUA_CONTROL_POSX ] )
			{
				pusRect Rect;
				PUU32 RastPort;

				// Add frame size to wanted height
				Rect.X = Rect.Y = 0;
				Rect.Width = Rect.Height = 0;
				RastPort = puDoMethod( pData->WindowObj, PUM_WINDOW_GETRASTPORT, 0, 0 );
				puGetFrameOuterRect( RastPort, pData->Frame, PUSTATE_NORMAL, &Rect, pData->pLabel );
				pData->PosSizeParams[ PUA_CONTROL_MINHEIGHT - PUA_CONTROL_POSX ] = WantedHeight + Rect.Height;
				pData->PosSizeParams[ PUA_CONTROL_MAXHEIGHT - PUA_CONTROL_POSX ] = WantedHeight + Rect.Height;
			}*/
			break;

		case PUM_CONTROL_RENDER:
			{
				pusRect Rect;
				Rect.X = puGetAttribute( _Object, PUA_CONTROL_POSX );
				Rect.Y = puGetAttribute( _Object, PUA_CONTROL_POSY );
				Rect.Width = puGetAttribute( _Object, PUA_CONTROL_WIDTH );
				Rect.Height = puGetAttribute( _Object, PUA_CONTROL_HEIGHT );

				if( pData->IsButton )
				{
					if( pData->bPressed )
						puDrawFrame( _Param1, pData->Frame, PUSTATE_ACTIVE, &Rect, pData->pLabel );
					else
						puDrawFrame( _Param1, pData->Frame, PUSTATE_NORMAL, &Rect, pData->pLabel );
				}
				else
					puDrawFrame( _Param1, pData->Frame, pData->State, &Rect, pData->pLabel );
			}
			break;

		case PUM_CONTROL_CLEARBG:
			{
				pusRect Rect;
				pusRect* pRect;

				if( _Param2 )
					pRect = ( pusRect* )_Param2;
				else
				{
					Rect.X = puGetAttribute( _Object, PUA_CONTROL_POSX );
					Rect.Y = puGetAttribute( _Object, PUA_CONTROL_POSY );
					Rect.Width = puGetAttribute( _Object, PUA_CONTROL_WIDTH );
					Rect.Height = puGetAttribute( _Object, PUA_CONTROL_HEIGHT );

					puGetFrameInnerRect( _Param1, pData->Frame, pData->State, &Rect, pData->pLabel );

					pRect = &Rect;
				}

				puDrawFrameBG( _Param1, pData->Frame, pData->State, pRect );
			}
			break;

		case PUM_CONTROL_RECALC:
			if( pData->WindowObj )
			{
				PUU32 RastPort;
				PUU32 MinMaxBackup[4];
				RastPort = puDoMethod( pData->WindowObj, PUM_WINDOW_GETRASTPORT, 0, 0 );
				memcpy( MinMaxBackup, &pData->PosSizeParams[4], sizeof( PUU32 ) * 4 );

		//		pData->MinMaxUpdateFlags = 0xffff;
				puDoMethod( _Object, PUM_CONTROL_CALCMINMAX, RastPort, pData->WindowObj );

				// If our min/max size have changed, we ask a recalc to the window
				if( memcmp( MinMaxBackup, &pData->PosSizeParams[4], sizeof( PUU32 ) * 4 ) )
			//	if( pData->MinMaxUpdateFlags )
					puDoMethod( pData->WindowObj, PUM_WINDOW_RECALC, 0, 0 );
				else
					puDoMethod( _Object, PUM_CONTROL_REDRAWINNER, 0, 0 );
			}
			break;

		case PUM_CONTROL_RELAYOUT:
			if( pData->DeferRelayoutCount )
				pData->bRelayout = TRUE;
			else if( pData->WindowObj )
			{
				PUU32 RastPort;
				RastPort = puDoMethod( pData->WindowObj, PUM_WINDOW_GETRASTPORT, 0, 0 );

				puDoMethod( _Object, PUM_CONTROL_PRELAYOUT, RastPort, 0 );
				puDoMethod( _Object, PUM_CONTROL_LAYOUT, RastPort, 0 );
				puDoMethod( _Object, PUM_CONTROL_REDRAW, 0, 0 );

//				puDoMethod( pData->WindowObj, PUM_WINDOW_RELEASERASTPORT, 0, 0 );
			}
			break;

		case PUM_CONTROL_REDRAW:
			if( pData->WindowObj && !pData->bHidden )
			{
				pusRect Rect;
				Tmp = ParamIndex( POSX );
				Rect.X = pData->PosSizeParams[ Tmp ];
				Tmp = ParamIndex( POSY );
				Rect.Y = pData->PosSizeParams[ Tmp ];
				Tmp = ParamIndex( WIDTH );
				Rect.Width = pData->PosSizeParams[ Tmp ];
				Tmp = ParamIndex( HEIGHT );
				Rect.Height = pData->PosSizeParams[ Tmp ];

				puDoMethod( pData->WindowObj, PUM_WINDOW_REDRAWRECT, ( PUU32 )&Rect, _Param1 );
			}
			break;

		case PUM_CONTROL_REDRAWINNER:
			if( pData->WindowObj && !pData->bHidden )
			{
				pusRect Rect;
				PUU32 RastPort;

				Tmp = ParamIndex( POSX );
				Rect.X = pData->PosSizeParams[ Tmp ];
				Tmp = ParamIndex( POSY );
				Rect.Y = pData->PosSizeParams[ Tmp ];
				Tmp = ParamIndex( WIDTH );
				Rect.Width = pData->PosSizeParams[ Tmp ];
				Tmp = ParamIndex( HEIGHT );
				Rect.Height = pData->PosSizeParams[ Tmp ];

				RastPort = puDoMethod( pData->WindowObj, PUM_WINDOW_GETRASTPORT, 0, 0 );
				puGetFrameInnerRect( RastPort, pData->Frame, pData->State, &Rect, pData->pLabel );
//				puDoMethod( pData->WindowObj, PUM_WINDOW_RELEASERASTPORT, RastPort, 0 );

				puDoMethod( pData->WindowObj, PUM_WINDOW_REDRAWRECT, ( PUU32 )&Rect, _Param1 );
			}
			break;

		case PUM_CONTROL_KEYBOARDEVENT:
			if( !pData->IsButton )
				break;

			{
				pusKeyboardEvent* pKeyboardEvent;
				pKeyboardEvent = ( pusKeyboardEvent* )_Param1;

				if( pKeyboardEvent->Type == PUKEYBOARD_ASCII && pKeyboardEvent->Code == 0xd )
						puSetAttribute( _Object, PUA_CONTROL_CLICKED, TRUE );
			}
			break;

		case PUM_CONTROL_MOUSEEVENT:
			if( !pData->IsButton )
				break;

			{
				pusMouseEvent* pMouseEvent;
				pusRect Rect;

				pMouseEvent = ( pusMouseEvent* )_Param1;

				if( pData->State == PUSTATE_ACTIVE )
				{
					PUU32 LastPressed;

					Tmp = ParamIndex( POSX );
					Rect.X = pData->PosSizeParams[ Tmp ];
					Tmp = ParamIndex( POSY );
					Rect.Y = pData->PosSizeParams[ Tmp ];
					Tmp = ParamIndex( WIDTH );
					Rect.Width = pData->PosSizeParams[ Tmp ];
					Tmp = ParamIndex( HEIGHT );
					Rect.Height = pData->PosSizeParams[ Tmp ];

					LastPressed = pData->bPressed;

					if( puCheckPointInRect( &Rect, pMouseEvent->MouseX, pMouseEvent->MouseY ) )
						pData->bPressed = TRUE;
					else
						pData->bPressed = FALSE;

					if( pMouseEvent->ButtonCode == ( PUMOUSE_LMB | PU_KEYUP ) )
					{
						pData->bClicked = pData->bPressed;
						puSetAttribute( _Object, PUA_CONTROL_STATE, PUSTATE_NORMAL );
						pData->bPressed = FALSE;

						// Notification...
						puSetAttribute( _Object, PUA_CONTROL_CLICKED, pData->bClicked );
					}

					if( pData->bPressed != LastPressed )
						puDoMethod( pData->WindowObj, PUM_WINDOW_REDRAWRECT, ( PUU32 )&Rect, 0 );
				}
				else if( pData->State != PUSTATE_DISABLED && ( pMouseEvent->ButtonCode & ~PU_KEYDOUBLECLICK )== PUMOUSE_LMB )
				{
					Tmp = ParamIndex( POSX );
					Rect.X = pData->PosSizeParams[ Tmp ];
					Tmp = ParamIndex( POSY );
					Rect.Y = pData->PosSizeParams[ Tmp ];
					Tmp = ParamIndex( WIDTH );
					Rect.Width = pData->PosSizeParams[ Tmp ];
					Tmp = ParamIndex( HEIGHT );
					Rect.Height = pData->PosSizeParams[ Tmp ];

					if( puCheckPointInRect( &Rect, pMouseEvent->MouseX, pMouseEvent->MouseY ) )
					{
						pData->bPressed = TRUE;
						puSetAttribute( _Object, PUA_CONTROL_STATE, PUSTATE_ACTIVE );
					}
				}
			}
			break;

		default:
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
	}

	return 0;
}

void puControlCalcMinMax( pusControlClassData* _pData )
{
	pusRect Rect, Rect2;
	PUU32 RastPort;
	PUU32 Tmp;

	Tmp = ParamIndex( MAXHEIGHT );
	if( _pData->PosSizeParams[ Tmp ] == 0 )
		_pData->PosSizeParams[ Tmp ] = 32768;

	Tmp = ParamIndex( MAXWIDTH );
	if( _pData->PosSizeParams[ Tmp ] == 0 )
		_pData->PosSizeParams[ Tmp ] = 32768;


	// Add frame size
	Rect.X = Rect.Y = 0;
	Rect.Width = Rect.Height = 0;
	RastPort = puDoMethod( _pData->WindowObj, PUM_WINDOW_GETRASTPORT, 0, 0 );
	puGetFrameOuterRect( RastPort, _pData->Frame, PUSTATE_NORMAL, &Rect, _pData->pLabel );

	if( _pData->pLabel )
		puTextSize( RastPort, _pData->pLabel, 0, &Rect2 );

//	puDoMethod( _pData->WindowObj, PUM_WINDOW_RELEASERASTPORT, RastPort, 0 );

	Tmp = ParamIndex( MINHEIGHT );
	if( _pData->MinMaxUpdateFlags & ( 1 << Tmp ) )
		_pData->PosSizeParams[ Tmp ] += Rect.Height;

	Tmp = ParamIndex( MINWIDTH );
	if( _pData->MinMaxUpdateFlags & ( 1 << Tmp ) )
		_pData->PosSizeParams[ Tmp ] += Rect.Width;

	if( _pData->pLabel )
	{
		Tmp = ParamIndex( MAXWIDTH );
		if( _pData->MinMaxUpdateFlags & ( 1 << Tmp ) )
			if( _pData->PosSizeParams[ Tmp ] < ( PUU32 )( Rect2.Width + 10 ) )
				_pData->PosSizeParams[ Tmp ] = Rect2.Width + 10;

		Tmp = ParamIndex( MINWIDTH );
		if( _pData->MinMaxUpdateFlags & ( 1 << Tmp ) )
			if( _pData->PosSizeParams[ Tmp ] < ( PUU32 )( Rect2.Width + 10 ) )
				_pData->PosSizeParams[ Tmp ] = Rect2.Width + 10;
	}

	Tmp = ParamIndex( MAXHEIGHT );
	if( _pData->MinMaxUpdateFlags & ( 1 << Tmp ) )
		if( _pData->PosSizeParams[ Tmp ] < 32768 )
			_pData->PosSizeParams[ Tmp ] += Rect.Height;

	Tmp = ParamIndex( MAXWIDTH );
	if( _pData->MinMaxUpdateFlags & ( 1 << Tmp ) )
		if( _pData->PosSizeParams[ Tmp ] < 32768 )
			_pData->PosSizeParams[ Tmp ] += Rect.Width;

	_pData->MinMaxUpdateFlags = 0;
}


PUU32 puControlGetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr )
{
	PUU32 Tmp;
	pusControlClassData* pData;
	PUU32 bClicked;
	pData = ( pusControlClassData* )_pData;

	if( _Attr >= PUA_CONTROL_POSX && _Attr <= PUA_CONTROL_MAXHEIGHT )
	{
		Tmp = _Attr - PUA_CONTROL_POSX;
		return pData->PosSizeParams[ Tmp ];
	}
	
	switch( _Attr )
	{
		case PUA_CONTROL_STATE:
			return pData->State;

		case PUA_CONTROL_WEIGHT:
			return pData->Weight;

		case PUA_CONTROL_HIDDEN:
			return pData->bHidden;

		case PUA_CONTROL_KEEPROOM:
			return pData->bKeepRoom;

		case PUA_CONTROL_ACTIVEWEAK:
			return pData->bWeak;

		case PUA_CONTROL_WINDOW:
			return pData->WindowObj;

		case PUA_CONTROL_CLICKED:
			bClicked = pData->bClicked;
			pData->bClicked = FALSE;
			return bClicked;

		case PUA_CONTROL_HASKEYBOARDFOCUS:
			return pData->bKBFocus;

		default:
			return puDoSuperMethod( _Object, _Class, PUM_GET, _Attr, 0 );
	}
}


PUU32 puControlSetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr, PUU32 _Val )
{
	PUU32 Tmp;
	pusControlClassData* pData;
	PUU32 OldState;
	pusRect Rect;

	pData = ( pusControlClassData* )_pData;

	if( _Attr >= PUA_CONTROL_POSX && _Attr <= PUA_CONTROL_MAXHEIGHT )
	{
		Tmp = _Attr - PUA_CONTROL_POSX;
		pData->PosSizeParams[ Tmp ] = _Val;

		if( _Attr >= PUA_CONTROL_MINWIDTH )
			pData->MinMaxUpdateFlags |= 1 << Tmp;
		return TRUE;
	}

	switch( _Attr )
	{
		case PUA_CONTROL_FRAME:
			pData->Frame = _Val;
			break;

		case PUA_CONTROL_LABEL:
			pData->pLabel = ( PUU8* )_Val;
			break;

		case PUA_CONTROL_STATE:
			OldState = pData->State;
			pData->State = _Val;

			if( pData->State != OldState )
			{
				switch( OldState )
				{
					case PUSTATE_ACTIVE:
						puSetAttribute( pData->WindowObj, PUA_WINDOW_ACTIVECONTROL, 0 );
						break;
				}

				switch( pData->State )
				{
					case PUSTATE_ACTIVE:
						puSetAttribute( pData->WindowObj, PUA_WINDOW_ACTIVECONTROL, _Object );
						break;
				}

				if( pData->IsButton )
				{
					Tmp = ParamIndex( POSX );
					Rect.X = pData->PosSizeParams[ Tmp ];
					Tmp = ParamIndex( POSY );
					Rect.Y = pData->PosSizeParams[ Tmp ];
					Tmp = ParamIndex( WIDTH );
					Rect.Width = pData->PosSizeParams[ Tmp ];
					Tmp = ParamIndex( HEIGHT );
					Rect.Height = pData->PosSizeParams[ Tmp ];

					puDoMethod( pData->WindowObj, PUM_WINDOW_REDRAWRECT, ( PUU32 )&Rect, 0 );
				}
			}
			break;

		case PUA_CONTROL_ISBUTTON:
			pData->IsButton = _Val;
			break;

		case PUA_CONTROL_WEIGHT:
			pData->Weight = _Val;
			break;

		case PUA_CONTROL_HIDDEN:
			pData->bHidden = _Val;
			if( !pData->bKeepRoom )
				puDoMethod( _Object, PUM_CONTROL_RECALC, 0, 0 );
			break;

		case PUA_CONTROL_KEEPROOM:
			pData->bKeepRoom = _Val;
			break;

		case PUA_CONTROL_ACTIVEWEAK:
			pData->bWeak = _Val;

			if( puGetAttribute( _Object, PUA_CONTROL_STATE ) == PUSTATE_ACTIVE )
				puSetAttribute( pData->WindowObj, PUA_WINDOW_ACTIVECONTROL, _Object );
			break;

		case PUA_CONTROL_HASKEYBOARDFOCUS:
			pData->bKBFocus = _Val;
			break;

		case PUA_CONTROL_DEFERRELAYOUT:
			if( _Val )
				pData->DeferRelayoutCount++;
			else
				pData->DeferRelayoutCount--;

			if( pData->DeferRelayoutCount < 0 )
				pData->DeferRelayoutCount = 0;

			if( !pData->DeferRelayoutCount && pData->bRelayout )
			{
				pData->bRelayout = FALSE;
				puDoMethod( _Object, PUM_CONTROL_RELAYOUT, 0, 0 );
			}
			break;

		case PUA_CONTROL_WINDOW:
			pData->WindowObj = _Val;
			break;
	}

	return TRUE;
}