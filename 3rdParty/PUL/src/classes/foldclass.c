/* Poly
   Useful
   Lib */

#include "../include/pul.h"
#include <pul/pul.h>
#include <stdlib.h>
#include "foldclass.h"

void puFoldCalcMinMax( PULID _Object, pusFoldClassData* _pData, PUU32 _Param1, PUU32 _Param2 );
void puFoldLayout( PULID _Object, pusFoldClassData* _pData, PUU32 _Param1, PUU32 _Param2 );
void puFoldRender( PULID _Object, pusFoldClassData* _pData, PUU32 _Param1, PUU32 _Param2 );
void puFoldMouseEvent( PULID _Object, pusFoldClassData* _pData, PUU32 _Param1, PUU32 _Param2 );
void puFoldKeyboardEvent( PULID _Object, pusFoldClassData* _pData, PUU32 _Param1, PUU32 _Param2 );

PUU32 puFoldClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 )
{
	pusFoldClassData* pData;
	pData = ( pusFoldClassData* )_pData;

	switch( _MethodID )
	{
		case PUM_NEW:
			pData->Contents = 0;
			pData->pLabel = NULL;
			pData->bFolded = FALSE;
			pData->bHilighted = FALSE;
			pData->BarHeight = 0;
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		case PUM_DELETE:
			if( pData->Contents )
				puDoMethod( pData->Contents, PUM_REMREF, 0, 0 );
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		case PUM_GET:
			switch( _Param1 )
			{
				case PUA_FOLD_FOLDED:
					return pData->bFolded;
			}
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		case PUM_SET:
			switch( _Param1 )
			{
				case PUA_FOLD_CONTENTS:
					if( pData->Contents )
						puDoMethod( pData->Contents, PUM_REMREF, 0, 0 );

					pData->Contents = _Param2;

					if( pData->Contents )
						puDoMethod( pData->Contents, PUM_ADDREF, 0, 0 );
					break;

				case PUA_FOLD_LABEL:
					pData->pLabel = ( PUU8* )_Param2;
					puDoMethod( _Object, PUM_CONTROL_REDRAWINNER, 0, 0 );
					break;

				case PUA_FOLD_HILIGHT:
					pData->bHilighted = _Param2;
					puDoMethod( _Object, PUM_CONTROL_REDRAWINNER, 0, 0 );
					break;

				case PUA_FOLD_FOLDED:
					pData->bFolded = _Param2;
					puDoMethod( _Object, PUM_CONTROL_RECALC, 0, 0 );
					break;
			}
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		/* Control methods */
		case PUM_CONTROL_CALCMINMAX:
			puFoldCalcMinMax( _Object, pData, _Param1, _Param2 );
			puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 ); 
			break;

		case PUM_CONTROL_LAYOUT:
			puFoldLayout( _Object, pData, _Param1, _Param2 );
			break;

		case PUM_CONTROL_RENDER:
			puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 ); 
			puFoldRender( _Object, pData, _Param1, _Param2 );
			break;

		case PUM_CONTROL_MOUSEEVENT:
			puFoldMouseEvent( _Object, pData, _Param1, _Param2 );
			break;

		case PUM_CONTROL_KEYBOARDEVENT:
			puFoldKeyboardEvent( _Object, pData, _Param1, _Param2 );
			break;

		default:
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
	}

	return 0;
}

void puFoldCalcMinMax( PULID _Object, pusFoldClassData* _pData, PUU32 _Param1, PUU32 _Param2 )
{
	PUU32 MinWidth = 0, MaxWidth = 0;
	PUU32 MinHeight = 0, MaxHeight = 0;
	PUU32 ObjWidth, ObjHeight;
	pusRect Rect;

	if( _pData->pLabel )
	{
		puTextSize( _Param1, _pData->pLabel, 0, &Rect );
		MinWidth = Rect.Width + 60;
		MaxWidth = Rect.Width + 60;
		MinHeight = Rect.Height;
		MaxHeight = Rect.Height;
	}

	if( MinHeight < 12 )
		MinHeight = MaxHeight = 12;

	if( MinWidth < 50 )
		MinWidth = MaxWidth = 50;

	if( _pData->Contents ) //&& !_pData->bFolded )
	{
		puDoMethod( _pData->Contents, PUM_CONTROL_CALCMINMAX, _Param1, _Param2 );

		ObjWidth = puGetAttribute( _pData->Contents, PUA_CONTROL_MINWIDTH );
		ObjHeight = puGetAttribute( _pData->Contents, PUA_CONTROL_MINHEIGHT );
		if( ( ObjWidth + 20 ) > MinWidth )
			MinWidth = ObjWidth + 20;

		if( !_pData->bFolded )
			MinHeight += ObjHeight + 4;

		ObjWidth = puGetAttribute( _pData->Contents, PUA_CONTROL_MAXWIDTH );
		ObjHeight = puGetAttribute( _pData->Contents, PUA_CONTROL_MAXHEIGHT );
		if( ObjWidth > MaxWidth )
			MaxWidth = ObjWidth;

		if( !_pData->bFolded )
			MaxHeight += ObjHeight + 4;

		if( MaxHeight > 32768 )
			MaxHeight = 32768;
	}

	puSetAttribute( _Object, PUA_CONTROL_MINWIDTH, MinWidth );
	puSetAttribute( _Object, PUA_CONTROL_MAXWIDTH, MaxWidth );
	puSetAttribute( _Object, PUA_CONTROL_MINHEIGHT, MinHeight );
	puSetAttribute( _Object, PUA_CONTROL_MAXHEIGHT, MaxHeight );
}

void puFoldLayout( PULID _Object, pusFoldClassData* _pData, PUU32 _Param1, PUU32 _Param2 )
{
	pusRect Rect, Rect2;

	Rect2.Height = 0;

	if( _pData->Contents ) // && !_pData->bFolded )
	{
		if( _pData->pLabel )
			puTextSize( _Param1, _pData->pLabel, 0, &Rect2 );

		if( Rect2.Height < 12 )
			Rect2.Height = 12;

		_pData->BarHeight = Rect2.Height;

		puDoMethod( _Object, PUM_CONTROL_GETINNERRECT, ( PUU32 )&Rect, 0 );

		puSetAttribute( _pData->Contents, PUA_CONTROL_WIDTH, Rect.Width - 20 );
		puSetAttribute( _pData->Contents, PUA_CONTROL_POSX, Rect.X + 10 );

		puSetAttribute( _pData->Contents, PUA_CONTROL_HEIGHT, Rect.Height - Rect2.Height - 2 );
		puSetAttribute( _pData->Contents, PUA_CONTROL_POSY, Rect.Y + Rect2.Height + 2 );

		puDoMethod( _pData->Contents, PUM_CONTROL_LAYOUT, _Param1, _Param2 );
	}
}
	
void puFoldRender( PULID _Object, pusFoldClassData* _pData, PUU32 _Param1, PUU32 _Param2 )
{
//	pusNode* pCurrent;
//	pusNode* pNext;
	pusRect Rect, Rect2;
	PUU32 YPos;

	puDoMethod( _Object, PUM_CONTROL_GETINNERRECT, ( PUU32 )&Rect, 0 );


	if( _pData->Contents && !_pData->bFolded )
	{
		puDoMethod( _pData->Contents, PUM_CONTROL_GETOUTERRECT, ( PUU32 )&Rect2, 0 );
		Rect2.Width = Rect.Width;
		Rect2.X = Rect.X;
		Rect2.Height = Rect2.Y - Rect.Y;
		Rect2.Y = Rect.Y;
		puDoMethod( _Object, PUM_CONTROL_CLEARBG, _Param1, ( PUU32 )&Rect2 );

		puDoMethod( _pData->Contents, PUM_CONTROL_GETOUTERRECT, ( PUU32 )&Rect2, 0 );
		Rect2.Width = 10;
		Rect2.X = Rect.X;
		puDoMethod( _Object, PUM_CONTROL_CLEARBG, _Param1, ( PUU32 )&Rect2 );

		puDoMethod( _pData->Contents, PUM_CONTROL_GETOUTERRECT, ( PUU32 )&Rect2, 0 );
		Rect2.Width = 10;
		Rect2.X = Rect.X + Rect.Width - 10;
		puDoMethod( _Object, PUM_CONTROL_CLEARBG, _Param1, ( PUU32 )&Rect2 );

	}
	else
		puDoMethod( _Object, PUM_CONTROL_CLEARBG, _Param1, 0 );


	YPos = Rect.Y;

	if( _pData->pLabel )
	{
		puTextSize( _Param1, _pData->pLabel, 0, &Rect2 );
		YPos = Rect.Y + Rect2.Height / 2;
	}

	Rect2.Height = _pData->BarHeight;

	puSetFillPen( _Param1, 255, 255, 255 );
	puSetDrawPen( _Param1, 0, 0, 0 );

	// Draw triangle
	if( _pData->bFolded )
		puTriangle( _Param1, Rect.X + 5, YPos - 6, Rect.X + 11, YPos, Rect.X + 5, YPos + 6 );
	else
		puTriangle( _Param1, Rect.X + 2, YPos - 3, Rect.X + 8, YPos + 3, Rect.X + 14, YPos - 3 );

	// Draw line
	puMove( _Param1, Rect.X + 16, YPos );
	puLine( _Param1, Rect.X + Rect.Width - 8, YPos );

//	puSetDrawPen( _Param1, 255, 255, 255 );
//	puMove( _Param1, Rect.X + 16, YPos + 1 );
//	puLine( _Param1, Rect.X + Rect.Width - 8, YPos + 1 );

	// Draw label
	if( _pData->pLabel )
	{
		puSetFillPen( _Param1, 170, 170, 170 );

		if( _pData->bHilighted )
			puSetDrawPen( _Param1, 255, 0, 0 );
		else
			puSetDrawPen( _Param1, 0, 0, 0 );

		Rect2.X = Rect.X - 3 + ( Rect.Width - Rect2.Width ) / 2;
		Rect2.Width += 6;
		Rect2.Y = Rect.Y;
		puRect( _Param1, &Rect2 );
		puDrawText( _Param1, _pData->pLabel, 0, Rect2.X + 3, Rect2.Y );
	}

	// Draw contents
	if( _pData->Contents && !_pData->bFolded )
		puDoMethod( _pData->Contents, PUM_CONTROL_RENDER, _Param1, _Param2 );
}

void puFoldMouseEvent( PULID _Object, pusFoldClassData* _pData, PUU32 _Param1, PUU32 _Param2 )
{
//	PUU32 State;
	pusMouseEvent* pMouseEvent;
	pusRect Rect;
//	pusNode* pCurrent;
//	pusNode* pNext;

	pMouseEvent = ( pusMouseEvent* )_Param1;

	puDoMethod( _Object, PUM_CONTROL_GETINNERRECT, ( PUU32 )&Rect, 0 );

	Rect.Height = _pData->BarHeight;

	if( puCheckPointInRect( &Rect, pMouseEvent->MouseX, pMouseEvent->MouseY ) )
	{
		if( ( pMouseEvent->ButtonCode & ~PU_KEYDOUBLECLICK ) == PUMOUSE_LMB )
			puSetAttribute( _Object, PUA_FOLD_FOLDED, !_pData->bFolded );
	}
	else
	{
		// Forward events to the object contained in the selected tab
		if( _pData->Contents && !_pData->bFolded )
			puDoMethod( _pData->Contents, PUM_CONTROL_MOUSEEVENT, _Param1, _Param2 );
	}
}

void puFoldKeyboardEvent( PULID _Object, pusFoldClassData* _pData, PUU32 _Param1, PUU32 _Param2 )
{
	// Forward events to the object contained in the selected tab
	if( _pData->Contents && !_pData->bFolded )
		puDoMethod( _pData->Contents, PUM_CONTROL_KEYBOARDEVENT, _Param1, _Param2 );
}