/* Poly
   Useful
   Lib */

#include "../include/pul.h"
#include <pul/pul.h>
#include <stdlib.h>
#include "registerclass.h"

void puRegisterCalcMinMax( PULID _Object, pusRegisterClassData* _pData, PUU32 _Param1, PUU32 _Param2 );
void puRegisterPreLayout( PULID _Object, pusRegisterClassData* _pData, PUU32 _Param1, PUU32 _Param2 );
void puRegisterLayout( PULID _Object, pusRegisterClassData* _pData, PUU32 _Param1, PUU32 _Param2 );
void puRegisterRender( PULID _Object, pusRegisterClassData* _pData, PUU32 _Param1, PUU32 _Param2 );
void puRegisterDrawTabFrame( pusRegisterTab* _pTab, pusRegisterClassData* _pData, PUU32 _RastPort );
void puRegisterMouseEvent( PULID _Object, pusRegisterClassData* _pData, PUU32 _Param1, PUU32 _Param2 );
void puRegisterKeyboardEvent( PULID _Object, pusRegisterClassData* _pData, PUU32 _Param1, PUU32 _Param2 );

PUU32 puRegisterClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 )
{
	pusRegisterClassData* pData;
	pData = ( pusRegisterClassData* )_pData;

	switch( _MethodID )
	{
		case PUM_NEW:
			puInitList( &pData->TabList );
			pData->pCurrentTab = NULL;
			pData->pSelectedTab = NULL;
			pData->bFixedWidth = FALSE;
			pData->bFixedHeight = FALSE;
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		case PUM_DELETE:
			{
				pusNode* pCurrent;
				pusNode* pNext;
				pusRegisterTab* pTab;
				
				pCurrent = pData->TabList.pFirst;

				while( pNext = pCurrent->pNext )
				{
					pTab = ( pusRegisterTab* )pCurrent;
					if( pTab->Object )
						puDoMethod( pTab->Object, PUM_REMREF, 0, 0 );
					free( pCurrent );
					pCurrent = pNext;
				}
			}
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		case PUM_SET:
			switch( _Param1 )
			{
				case PUA_REGISTER_FIXEDWIDTH:
					pData->bFixedWidth = _Param2;
					break;

				case PUA_REGISTER_FIXEDHEIGHT:
					pData->bFixedHeight = _Param2;
					break;

				case PUA_REGISTER_CURRENTTAB:
					{
						pusNode* pCurrent;
						pusNode* pNext;

						pCurrent = pData->TabList.pFirst;

						while( pNext = pCurrent->pNext )
						{
							if( !( _Param2-- ) )
								break;

							pCurrent = pNext;
						}

						pData->pSelectedTab = ( pusRegisterTab* )pCurrent;
						puDoMethod( _Object, PUM_CONTROL_REDRAWINNER, 0, 0 );
					}
					break;
			}
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		/* Register methods */
		case PUM_REGISTER_ADDTAB:	// Param1, Label
			{
				pusRegisterTab* pTab;
				if( !( pTab = ( pusRegisterTab* )malloc( sizeof( pusRegisterTab ) ) ) )
					return FALSE;

				pTab->pLabel = ( PUU8* )_Param1;
				pTab->Object = 0;
				puAddTail( &pData->TabList, &pTab->Node );

				if( !pData->pCurrentTab )
					pData->pCurrentTab = pTab;

				if( !pData->pSelectedTab )
					pData->pSelectedTab = pTab;
			}
			return TRUE;

		case PUM_REGISTER_SETTABOBJECT:
			if( pData->pCurrentTab )
			{
				pData->pCurrentTab->Object = _Param1;
				puDoMethod( pData->pCurrentTab->Object, PUM_ADDREF, 0, 0 );
				pData->pCurrentTab = ( pusRegisterTab* )pData->pCurrentTab->Node.pNext;

				if( !pData->pCurrentTab->Node.pNext )
					pData->pCurrentTab = NULL;
			}
			break;

		/* Control methods */
		case PUM_CONTROL_CALCMINMAX:
			puRegisterCalcMinMax( _Object, pData, _Param1, _Param2 );
			puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 ); 
			break;

		case PUM_CONTROL_PRELAYOUT:
			puRegisterLayout( _Object, pData, _Param1, _Param2 );
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		case PUM_CONTROL_LAYOUT:
			puRegisterLayout( _Object, pData, _Param1, _Param2 );
			break;

		case PUM_CONTROL_RENDER:
			puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 ); 
			puRegisterRender( _Object, pData, _Param1, _Param2 );
			break;

		case PUM_CONTROL_MOUSEEVENT:
			puRegisterMouseEvent( _Object, pData, _Param1, _Param2 );
			break;

		case PUM_CONTROL_KEYBOARDEVENT:
			puRegisterKeyboardEvent( _Object, pData, _Param1, _Param2 );
			break;

		default:
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
	}

	return 0;
}

void puRegisterCalcMinMax( PULID _Object, pusRegisterClassData* _pData, PUU32 _Param1, PUU32 _Param2 )
{
	PUU32 MinWidth = 0, MaxWidth = 0;
	PUU32 MinHeight = 0, MaxHeight = 0;
	PUU32 ObjWidth, ObjHeight;
	pusRect Rect;
	pusNode* pCurrent;
	pusNode* pNext;
	pusRegisterTab* pTab;

	_pData->TabsHeight = 0;
	_pData->TabsWidth = 0;

	pCurrent = _pData->TabList.pFirst;
	while( pNext = pCurrent->pNext )
	{
		pTab = ( pusRegisterTab* )pCurrent;

		// Compute current tab size
		puTextSize( _Param1, pTab->pLabel, 0, &Rect );
		_pData->TabsWidth += Rect.Width + 12;
		_pData->TabsHeight = Rect.Height + 8;

		if( pTab->Object )
		{
			puDoMethod( pTab->Object, PUM_CONTROL_CALCMINMAX, _Param1, _Param2 );

			ObjWidth = puGetAttribute( pTab->Object, PUA_CONTROL_MINWIDTH );
			ObjHeight = puGetAttribute( pTab->Object, PUA_CONTROL_MINHEIGHT );
			if( ObjWidth > MinWidth )
				MinWidth = ObjWidth;

			if( ObjHeight > MinHeight )
				MinHeight = ObjHeight;

			ObjWidth = puGetAttribute( pTab->Object, PUA_CONTROL_MAXWIDTH );
			ObjHeight = puGetAttribute( pTab->Object, PUA_CONTROL_MAXHEIGHT );
			if( ObjWidth > MaxWidth )
				MaxWidth = ObjWidth;

			if( ObjHeight > MaxHeight )
				MaxHeight = ObjHeight;
		}

		pCurrent = pNext;
	}

	if( _pData->TabsWidth > MinWidth )
		MinWidth = _pData->TabsWidth;

	if( _pData->TabsWidth > MaxWidth )
		MaxWidth = _pData->TabsWidth;

	MinHeight += _pData->TabsHeight + 8;
	MaxHeight += _pData->TabsHeight + 8;

	MinWidth += 8;
	MaxWidth += 8;

	if( _pData->bFixedWidth )
		MaxWidth = MinWidth;

	if( _pData->bFixedHeight )
		MaxHeight = MinHeight;

	puSetAttribute( _Object, PUA_CONTROL_MINWIDTH, MinWidth );
	puSetAttribute( _Object, PUA_CONTROL_MAXWIDTH, MaxWidth );
	puSetAttribute( _Object, PUA_CONTROL_MINHEIGHT, MinHeight );
	puSetAttribute( _Object, PUA_CONTROL_MAXHEIGHT, MaxHeight );
}

void puRegisterPreLayout( PULID _Object, pusRegisterClassData* _pData, PUU32 _Param1, PUU32 _Param2 )
{
	pusRect Rect, Rect3;
	pusNode* pCurrent;
	pusNode* pNext;
	pusRegisterTab* pTab;
	PUU32 XPos, Min, Max, Size;

	puDoMethod( _Object, PUM_CONTROL_GETINNERRECT, ( PUU32 )&Rect, 0 );
	XPos = Rect.X;

	// Determine contents rectangle
	Rect3.X = Rect.X + 4;
	Rect3.Width = Rect.Width - 8;

	pCurrent = _pData->TabList.pFirst;
	while( pNext = pCurrent->pNext )
	{
		pTab = ( pusRegisterTab* )pCurrent;

		// Determine size and position of the contained object for the current tab
		if( pTab->Object )
		{
			Size = Rect3.Width;

			Min = puGetAttribute( pTab->Object, PUA_CONTROL_MINWIDTH );
			Max = puGetAttribute( pTab->Object, PUA_CONTROL_MAXWIDTH );

			if( Size < Min )
				Size = Min;

			if( Size > Max )
				Size = Max;

			puSetAttribute( pTab->Object, PUA_CONTROL_WIDTH, Size );
			puSetAttribute( pTab->Object, PUA_CONTROL_POSX, Rect3.X + ( ( Rect3.Width - Size ) >> 1 ) );


			puDoMethod( pTab->Object, PUM_CONTROL_PRELAYOUT, _Param1, _Param2 );
		}

		pCurrent = pNext;
	}

	puRegisterCalcMinMax( _Object, _pData, _Param1, _Param2 );
}

void puRegisterLayout( PULID _Object, pusRegisterClassData* _pData, PUU32 _Param1, PUU32 _Param2 )
{
	pusRect Rect, Rect2, Rect3;
	pusNode* pCurrent;
	pusNode* pNext;
	pusRegisterTab* pTab;
	PUU32 XPos, Min, Max, Size;

	puDoMethod( _Object, PUM_CONTROL_GETINNERRECT, ( PUU32 )&Rect, 0 );
	XPos = Rect.X;

	// Determine contents rectangle
	Rect3.X = Rect.X + 4;
	Rect3.Y = Rect.Y + _pData->TabsHeight + 4;
	Rect3.Width = Rect.Width - 8;
	Rect3.Height = Rect.Height - _pData->TabsHeight - 8;

	pCurrent = _pData->TabList.pFirst;
	while( pNext = pCurrent->pNext )
	{
		pTab = ( pusRegisterTab* )pCurrent;

		puTextSize( _Param1, pTab->pLabel, 0, &Rect2 );
		pTab->Rect.X = XPos;
		pTab->Rect.Y = Rect.Y;
		pTab->Rect.Width = Rect2.Width + 12;
		pTab->Rect.Height = _pData->TabsHeight;

		pTab->LabelX = pTab->Rect.X + 5;
		pTab->LabelY = pTab->Rect.Y + 4 + 1;
		XPos += Rect2.Width + 12; // + 1;

		// Determine size and position of the contained object for the current tab
		if( pTab->Object )
		{
			Size = Rect3.Width;

			Min = puGetAttribute( pTab->Object, PUA_CONTROL_MINWIDTH );
			Max = puGetAttribute( pTab->Object, PUA_CONTROL_MAXWIDTH );

			if( Size < Min )
				Size = Min;

			if( Size > Max )
				Size = Max;

			puSetAttribute( pTab->Object, PUA_CONTROL_WIDTH, Size );
			puSetAttribute( pTab->Object, PUA_CONTROL_POSX, Rect3.X + ( ( Rect3.Width - Size ) >> 1 ) );

			Size = Rect3.Height;

			Min = puGetAttribute( pTab->Object, PUA_CONTROL_MINHEIGHT );
			Max = puGetAttribute( pTab->Object, PUA_CONTROL_MAXHEIGHT );

			if( Size < Min )
				Size = Min;

			if( Size > Max )
				Size = Max;

			puSetAttribute( pTab->Object, PUA_CONTROL_HEIGHT, Size );
			puSetAttribute( pTab->Object, PUA_CONTROL_POSY, Rect3.Y + ( ( Rect3.Height - Size ) >> 1 ) );

			puDoMethod( pTab->Object, PUM_CONTROL_LAYOUT, _Param1, _Param2 );
		}

		pCurrent = pNext;
	}
}


void puRegisterRender( PULID _Object, pusRegisterClassData* _pData, PUU32 _Param1, PUU32 _Param2 )
{
	pusNode* pCurrent;
	pusNode* pNext;
	pusRegisterTab* pTab;
	pusRect Rect, Rect2, TmpRect;
	PUU8 bClear = TRUE;

	puDoMethod( _Object, PUM_CONTROL_GETINNERRECT, ( PUU32 )&Rect, 0 );

	// Draw frame
	Rect2.X = Rect.X;
	Rect2.Width = Rect.Width;
	Rect2.Y = Rect.Y + _pData->TabsHeight;
	Rect2.Height = Rect.Height - _pData->TabsHeight;
	puDrawFrame( _Param1, PUFRAME_BOX, PUSTATE_NORMAL, &Rect2, NULL );


	TmpRect.X = Rect.X;
	TmpRect.Width = Rect.Width;
	TmpRect.Y = Rect.Y;
	TmpRect.Height = _pData->TabsHeight;
	puDrawFrameBG( _Param1, PUFRAME_BOX, PUSTATE_NORMAL, &TmpRect );

	// Draw tabs
	pCurrent = _pData->TabList.pFirst;

	while( pNext = pCurrent->pNext )
	{
		pTab = ( pusRegisterTab* )pCurrent;

		puRegisterDrawTabFrame( pTab, _pData, _Param1 );

		if( pTab == _pData->pSelectedTab )
			puDrawText( _Param1, pTab->pLabel, 0, pTab->LabelX, pTab->LabelY );
		else
			puDrawText( _Param1, pTab->pLabel, 0, pTab->LabelX, pTab->LabelY + 2 );
		pCurrent = pNext;
	}

	puGetFrameInnerRect( _Param1, PUFRAME_BOX, PUSTATE_NORMAL, &Rect2, NULL );

	// Draw contents of selected tab
	if( _pData->pSelectedTab )
	{
		if( _pData->pSelectedTab->Object )
		{
			puDoMethod( _pData->pSelectedTab->Object, PUM_CONTROL_RENDER, _Param1, _Param2 );

			puDoMethod( _pData->pSelectedTab->Object, PUM_CONTROL_GETOUTERRECT, ( PUU32 )&Rect, 0 );

			TmpRect.X = Rect2.X;
			TmpRect.Y = Rect2.Y;
			TmpRect.Width = Rect2.Width;
			TmpRect.Height = Rect.Y - Rect2.Y;
			if( TmpRect.Height )
				puDrawFrameBG( _Param1, PUFRAME_BOX, PUSTATE_NORMAL, &TmpRect );

			TmpRect.Y = Rect.Y + Rect.Height;
			TmpRect.Height = Rect2.Y + Rect2.Height - TmpRect.Y;
			if( TmpRect.Height )
				puDrawFrameBG( _Param1, PUFRAME_BOX, PUSTATE_NORMAL, &TmpRect );

			TmpRect.Y = Rect.Y;
			TmpRect.Height = Rect.Height;
			TmpRect.Width = Rect.X - Rect2.X;
			if( TmpRect.Width )
				puDrawFrameBG( _Param1, PUFRAME_BOX, PUSTATE_NORMAL, &TmpRect );

			TmpRect.X = Rect.X + Rect.Width;
			TmpRect.Width = Rect2.X + Rect2.Width - TmpRect.X;
			if( TmpRect.Width )
				puDrawFrameBG( _Param1, PUFRAME_BOX, PUSTATE_NORMAL, &TmpRect );

			bClear = FALSE;
		}
	}

	if( bClear )
		puDrawFrameBG( _Param1, PUFRAME_BOX, PUSTATE_NORMAL, &Rect2 );
}

void puRegisterDrawTabFrame( pusRegisterTab* _pTab, pusRegisterClassData* _pData, PUU32 _RastPort )
{
	pusRect Rect, Rect2, TabRect;

	TabRect.X = _pTab->Rect.X;
	TabRect.Y = _pTab->Rect.Y;
	TabRect.Width = _pTab->Rect.Width;
	TabRect.Height = _pTab->Rect.Height;

	if( _pData->pSelectedTab != _pTab )
	{
		TabRect.Y += 3;
		TabRect.Height -= 3;

		TabRect.X += 1;
		TabRect.Width -= 2;

		puSetFillPen( _RastPort, 220, 220, 220 );
	}
	else
		puSetFillPen( _RastPort, 255, 255, 255 );

	/* white part */
	Rect2.X = TabRect.X;
	Rect2.Y = TabRect.Y;
	Rect2.Width = TabRect.Width;
	Rect2.Height = 2;
	puRect( _RastPort, &Rect2 );

	Rect2.Height = TabRect.Height;
	Rect2.Width = 2;
	puRect( _RastPort, &Rect2 );

	/* black part */
	if( _pData->pSelectedTab != _pTab )
		puSetFillPen( _RastPort, 60, 60, 60 );
	else
		puSetFillPen( _RastPort, 0, 0, 0 );

	Rect2.X += TabRect.Width - 2;
	Rect2.Y = TabRect.Y + 1;
	Rect2.Height = TabRect.Height - 1;
	Rect2.Width = 2;
	puRect( _RastPort, &Rect2 );

	Rect.X = TabRect.X + 2;
	Rect.Y = TabRect.Y + 2;
	Rect.Width = TabRect.Width - 4;
	Rect.Height = TabRect.Height - 2;

	if( _pData->pSelectedTab == _pTab )
	{
		puSetFillPen( _RastPort, 170, 170, 170 );
		Rect.Height += 2;
	}
	else
		puSetFillPen( _RastPort, 140, 140, 140 );

	puRect( _RastPort, &Rect );
}

void puRegisterMouseEvent( PULID _Object, pusRegisterClassData* _pData, PUU32 _Param1, PUU32 _Param2 )
{
	PUU32 State;
	pusMouseEvent* pMouseEvent;
	pusRect Rect;
	pusNode* pCurrent;
	pusNode* pNext;
	pusRegisterTab* pTab;

	pMouseEvent = ( pusMouseEvent* )_Param1;
	State = puGetAttribute( _Object, PUA_CONTROL_STATE );

	if( State == PUSTATE_ACTIVE )
	{
		if( pMouseEvent->ButtonCode == ( PUMOUSE_LMB | PU_KEYUP ) )
			puSetAttribute( _Object, PUA_CONTROL_STATE, PUSTATE_NORMAL );
	}
	else
	{
		puDoMethod( _Object, PUM_CONTROL_GETINNERRECT, ( PUU32 )&Rect, 0 );

		// Check for events in tabs area
		if( pMouseEvent->MouseY < ( PUS32 )( Rect.Y + _pData->TabsHeight ) )
		{
			if( pMouseEvent->ButtonCode == PUMOUSE_LMB )
			{
				puSetAttribute( _Object, PUA_CONTROL_STATE, PUSTATE_ACTIVE );
				pCurrent = _pData->TabList.pFirst;

				while( pNext = pCurrent->pNext )
				{
					pTab = ( pusRegisterTab* )pCurrent;

					if( puCheckPointInRect( &pTab->Rect, pMouseEvent->MouseX, pMouseEvent->MouseY ) )
					{
						if( _pData->pSelectedTab != pTab )
						{
							_pData->pSelectedTab = pTab;
							puDoMethod( _Object, PUM_CONTROL_REDRAWINNER, 0, 0 );
							break;
						}
					}

					pCurrent = pNext;
				}
			}
		}
		else
		{
			// Forward events to the object contained in the selected tab
			if( _pData->pSelectedTab )
				if( _pData->pSelectedTab->Object )
					puDoMethod( _pData->pSelectedTab->Object, PUM_CONTROL_MOUSEEVENT, _Param1, _Param2 );
		}
	}
}

void puRegisterKeyboardEvent( PULID _Object, pusRegisterClassData* _pData, PUU32 _Param1, PUU32 _Param2 )
{
	// Forward events to the object contained in the selected tab
	if( _pData->pSelectedTab )
		if( _pData->pSelectedTab->Object )
			puDoMethod( _pData->pSelectedTab->Object, PUM_CONTROL_KEYBOARDEVENT, _Param1, _Param2 );
}