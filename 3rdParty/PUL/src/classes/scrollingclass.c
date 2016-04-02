/* Poly
   Useful
   Lib */

#include "../include/pul.h"
#include <pul/pul.h>
#include "scrollingclass.h"

PUU32 puScrollingSetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr, PUU32 _Val );
void puScrollingAutoHideHSlider( PULID _Object, pusScrollingClassData* _pData );
void puScrollingAutoHideVSlider( PULID _Object, pusScrollingClassData* _pData );
void puScrollingCalcMinMax(	PULID _Object, pusScrollingClassData* _pData );

enum
{
	ROOTOBJ = 1,
	SPACER,
	HSCROLLER,
	VSCROLLER
};

static PUU32 ColDef[]=
{
	PU_ACTION_OBJDEF, ROOTOBJ, ( PUU32 )"HorGroup", PUA_HORGROUP_SPACING, 0, 0,
		PUM_ADDCHILD, PU_ACTION_OBJDEF, 0, ( PUU32 )"VerGroup", PUA_VERGROUP_SPACING, 0, 0,
			PUM_ADDCHILD, /*PU_ACTION_OBJDEF, CONTAINER, ( PUU32 )"Container", PUA_CONTAINER_CONTENTS,*/
				PU_ACTION_OBJDEF, SPACER, ( PUU32 )"Spacer", PUA_CONTROL_FRAME, PUFRAME_BOX, 0, 0,
				//0, 0,
			PUM_ADDCHILD, PU_ACTION_OBJDEF, HSCROLLER, ( PUU32 )"Slider",
	//			PUA_CONTROL_FRAME, PUFRAME_NONE,
				PUA_CONTROL_HIDDEN, TRUE,
				PUA_SLIDER_MAX, 0,
				PUA_SLIDER_VISIBLE, 1, 0, 0,
		PU_ENDGROUP,
		
		PUM_ADDCHILD, PU_ACTION_OBJDEF, VSCROLLER, ( PUU32 )"Slider",
	//		PUA_CONTROL_FRAME, PUFRAME_NONE,
			PUA_CONTROL_HIDDEN, TRUE,
			PUA_SLIDER_ISVERTICAL, TRUE, 
			PUA_SLIDER_MAX, 0,
			PUA_SLIDER_VISIBLE, 1, 0, 0,
	PU_ENDGROUP,

	PU_ACTION_END
};

/*
	Notification definition:
		<condition>
		<action>

	<condition>:
	Attribute change:
		PUNOTIFY_CONDITION_ATTRCHANGE, <attr id>

	Attribute == value:
		PUNOTIFY_CONDITION_ATTREQUALS, <attr id>, <value>

	<obj>
		<objid> if PUM_ADDNOTIFY is invoked with an objcollection as param2, a PULID to the object otherwise

	<action>:
	Callback:
		PUNOTIFY_ACTION_CALLBACK, <callback addr>

	Set attribute to constant:
		PUNOTIFY_ACTION_SET, <obj>, <attr id>, value

	Set attribute to notified attribute value:
		PUNOTIFY_ACTION_SETVAL, <obj>, <attr id>

	Toggle attribute between 0 and 1:
		PUNOTIFY_ACTION_TOGGLE, <obj>, <attr id>

	Call a method without params:
		PUNOTIFY_ACTION_DOMETHOD, <obj>, <method id>
*/

PUU32 puScrollingClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 )
{
	pusScrollingClassData* pData;
//	PULID HScroller, VScroller;
	pData = ( pusScrollingClassData* )_pData;

	switch( _MethodID )
	{
		case PUM_NEW:
//			pData->Contents = 0;
//			pData->Root = 0;
			pData->HScroller = 0;
			pData->VScroller = 0;
			pData->bAutoHideScrollers = TRUE;
			pData->bShowHScroller = FALSE;
			pData->bShowVScroller = FALSE;
			pData->HVis = 0;
			pData->VVis = 0;
			pData->Width = 0;
			pData->Height = 0;
			pData->bNoframes = FALSE;
			pData->MinMaxUpdateFlags = 0;

			if( !puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 ) )
				return FALSE;

			if( !( pData->HScroller = puNewObjectFromName( "Slider", NULL ) ) )
				return FALSE;

			if( !( pData->VScroller = puNewObjectFromName( "Slider", NULL ) ) )
			{
				puDeleteObject( pData->HScroller );
				return FALSE;
			}

			puSetAttribute( pData->VScroller, PUA_SLIDER_ISVERTICAL, TRUE );


			{
				PUU32 Notify[8];

				Notify[0] = PUNOTIFY_CONDITION_ATTRCHANGE;
				Notify[1] = PUA_SCROLLING_WIDTH;
				Notify[2] = PUNOTIFY_ACTION_SETVAL;
				Notify[3] = pData->HScroller;
				Notify[4] = PUA_SLIDER_MAX;
				puDoMethod( _Object, PUM_ADDNOTIFICATION, ( PUU32 )Notify, 0 );

				Notify[1] = PUA_SCROLLING_HEIGHT;
				Notify[3] = pData->VScroller;
				puDoMethod( _Object, PUM_ADDNOTIFICATION, ( PUU32 )Notify, 0 );

				Notify[4] = PUA_SLIDER_VISIBLE;

				Notify[1] = PUA_SCROLLING_HORVIS;
				Notify[3] = pData->HScroller;
				puDoMethod( _Object, PUM_ADDNOTIFICATION, ( PUU32 )Notify, 0 );

				Notify[1] = PUA_SCROLLING_VERVIS;
				Notify[3] = pData->VScroller;
				puDoMethod( _Object, PUM_ADDNOTIFICATION, ( PUU32 )Notify, 0 );

				Notify[4] = PUA_SLIDER_VALUE;
				Notify[1] = PUA_SCROLLING_POSY;
				puDoMethod( _Object, PUM_ADDNOTIFICATION, ( PUU32 )Notify, 0 );

				Notify[1] = PUA_SCROLLING_POSX;
				Notify[3] = pData->HScroller;
				puDoMethod( _Object, PUM_ADDNOTIFICATION, ( PUU32 )Notify, 0 );

				Notify[1] = PUA_SLIDER_VALUE;
				Notify[3] = _Object;
				Notify[4] = PUA_SCROLLING_POSX;
				puDoMethod( pData->HScroller, PUM_ADDNOTIFICATION, ( PUU32 )Notify, 0 );

				Notify[4] = PUA_SCROLLING_POSY;
				puDoMethod( pData->VScroller, PUM_ADDNOTIFICATION, ( PUU32 )Notify, 0 );
			}

			// Create the objects
	/*		if( pData->pCol = puCreateObjectCollection( ColDef ) )
			{
				PUU32 Notify[8];

//				pData->Container = puGetObjectFromCollection( pData->pCol, CONTAINER );
				pData->Contents = puGetObjectFromCollection( pData->pCol, SPACER );
				pData->Root = puGetObjectFromCollection( pData->pCol, ROOTOBJ );
				HScroller = puGetObjectFromCollection( pData->pCol, HSCROLLER );
				VScroller = puGetObjectFromCollection( pData->pCol, VSCROLLER );


				Notify[0] = PUNOTIFY_CONDITION_ATTRCHANGE;
				Notify[1] = PUA_SCROLLING_WIDTH;
				Notify[2] = PUNOTIFY_ACTION_SETVAL;
				Notify[3] = HSCROLLER;
				Notify[4] = PUA_SLIDER_MAX;
				puDoMethod( _Object, PUM_ADDNOTIFICATION, ( PUU32 )Notify, ( PUU32 )pData->pCol );

				Notify[1] = PUA_SCROLLING_HEIGHT;
				Notify[3] = VSCROLLER;
				puDoMethod( _Object, PUM_ADDNOTIFICATION, ( PUU32 )Notify, ( PUU32 )pData->pCol );

				Notify[4] = PUA_SLIDER_VISIBLE;

				Notify[1] = PUA_SCROLLING_HORVIS;
				Notify[3] = HSCROLLER;
				puDoMethod( _Object, PUM_ADDNOTIFICATION, ( PUU32 )Notify, ( PUU32 )pData->pCol );

				Notify[1] = PUA_SCROLLING_VERVIS;
				Notify[3] = VSCROLLER;
				puDoMethod( _Object, PUM_ADDNOTIFICATION, ( PUU32 )Notify, ( PUU32 )pData->pCol );

				Notify[4] = PUA_SLIDER_VALUE;
				Notify[1] = PUA_SCROLLING_POSY;
				puDoMethod( _Object, PUM_ADDNOTIFICATION, ( PUU32 )Notify, ( PUU32 )pData->pCol );

				Notify[1] = PUA_SCROLLING_POSX;
				Notify[3] = HSCROLLER;
				puDoMethod( _Object, PUM_ADDNOTIFICATION, ( PUU32 )Notify, ( PUU32 )pData->pCol );

				Notify[1] = PUA_SLIDER_VALUE;
				Notify[3] = _Object;
				Notify[4] = PUA_SCROLLING_POSX;
				puDoMethod( HScroller, PUM_ADDNOTIFICATION, ( PUU32 )Notify, 0 );

				Notify[4] = PUA_SCROLLING_POSY;
				puDoMethod( VScroller, PUM_ADDNOTIFICATION, ( PUU32 )Notify, 0 );
			}*/

			/*		#define PUA_SCROLLING_WIDTH				PU_SCROLLINGCLASS_ATT + 5
#define PUA_SCROLLING_HEIGHT			PU_SCROLLINGCLASS_ATT + 6 */
			return TRUE;

		case PUM_DELETE:
			if( pData->HScroller )
				puDeleteObject( pData->HScroller );

			if( pData->VScroller )
				puDeleteObject( pData->VScroller );

		//	if( pData->pCol )
			//	puDeleteObjectCollection( pData->pCol );
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		case PUM_SET:
			puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
			return puScrollingSetAttr( _Object, _Class, pData, _Param1, _Param2 );

		/* Control methods */
		case PUM_CONTROL_CALCMINMAX:
			if( pData->HScroller )
				puDoMethod( pData->HScroller, PUM_CONTROL_CALCMINMAX, _Param1, _Param2 );

			if( pData->VScroller )
				puDoMethod( pData->VScroller, PUM_CONTROL_CALCMINMAX, _Param1, _Param2 );

			puScrollingCalcMinMax( _Object, pData );

			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		case PUM_CONTROL_PRELAYOUT:
			puScrollingCalcMinMax( _Object, pData );
		/*	if( !pData->bNoframes )
			{
				pusRect Rect;
				PUU32 Val;

				Rect.X = Rect.Y = 0;
				Rect.Width = Rect.Height = 0;
				puGetFrameOuterRect( 0, PUFRAME_BOX, PUSTATE_NORMAL, &Rect, NULL );

				Val = puGetAttribute( _Object, PUA_CONTROL_MINHEIGHT );
//			if( Val )
					puSetAttribute( _Object, PUA_CONTROL_MINHEIGHT, Val + Rect.Height );
					puSetAttribute( _Object, PUA_CONTROL_MAXHEIGHT, Val + Rect.Height );
			}*/

			puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
		/*	{
				pusRect Rect;
				puDoMethod( _Object, PUM_CONTROL_GETINNERRECT, ( PUU32 )&Rect, 0 );
				puSetAttribute( pData->Root, PUA_CONTROL_POSX, Rect.X );
				puSetAttribute( pData->Root, PUA_CONTROL_WIDTH, Rect.Width );
			//uDoMethod( pData->Root, PUM_CONTROL_PRELAYOUT, _Param1, _Param2 );
				puSetAttribute( pData->Contents, PUA_CONTROL_WANTEDHEIGHT, puGetAttribute( _Object, PUA_CONTROL_WANTEDHEIGHT ) );
				puDoMethod( pData->Root, PUM_CONTROL_PRELAYOUT, _Param1, _Param2 );
				puSetAttribute( _Object, PUA_CONTROL_WANTEDHEIGHT, puGetAttribute( pData->Root, PUA_CONTROL_WANTEDHEIGHT ) );
			}*/
			break;

		case PUM_CONTROL_LAYOUT:
			{
				pusRect Rect;
				PUU32 SizeHC, SizeVC = 0;
				puDoMethod( _Object, PUM_CONTROL_GETINNERRECT, ( PUU32 )&Rect, 0 );
	
				if( pData->bShowVScroller )
				{
					SizeVC = puGetAttribute( pData->VScroller, PUA_CONTROL_MINWIDTH );
					puSetAttribute( pData->VScroller, PUA_CONTROL_POSX, Rect.X + Rect.Width - SizeVC );
					puSetAttribute( pData->VScroller, PUA_CONTROL_POSY, Rect.Y );
					puSetAttribute( pData->VScroller, PUA_CONTROL_WIDTH, SizeVC );
					puSetAttribute( pData->VScroller, PUA_CONTROL_HEIGHT, Rect.Height );

					puDoMethod( pData->VScroller, PUM_CONTROL_LAYOUT, _Param1, _Param2 );
				}

				if( pData->bShowHScroller )
				{
					SizeHC = puGetAttribute( pData->HScroller, PUA_CONTROL_MINHEIGHT );
					puSetAttribute( pData->HScroller, PUA_CONTROL_POSX, Rect.X );
					puSetAttribute( pData->HScroller, PUA_CONTROL_POSY, Rect.Y + Rect.Height - SizeHC );
					puSetAttribute( pData->HScroller, PUA_CONTROL_WIDTH, Rect.Width - SizeVC );
					puSetAttribute( pData->HScroller, PUA_CONTROL_HEIGHT, SizeHC );

					puDoMethod( pData->HScroller, PUM_CONTROL_LAYOUT, _Param1, _Param2 );
				}
			}
			break;

		case PUM_CONTROL_RENDER:
			{
				pusRect Rect;

				puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

				if( !pData->bNoframes )
				{
					PUU32 Size;
					puDoMethod( _Object, PUM_CONTROL_GETINNERRECT, ( PUU32 )&Rect, 0 );
					if( pData->bShowVScroller )
					{
						Size = puGetAttribute( pData->VScroller, PUA_CONTROL_MINWIDTH );
						Rect.Width -= Size;
					}

					if( pData->bShowHScroller )
					{
						Size = puGetAttribute( pData->HScroller, PUA_CONTROL_MINHEIGHT );
						Rect.Height -= Size;
					}

					puDrawFrame( _Param1, PUFRAME_BOX, PUSTATE_NORMAL, &Rect, NULL );
				}

				if( pData->bShowHScroller )
				{
					puDoMethod( pData->HScroller, PUM_CONTROL_RENDER, _Param1, _Param2 );

					if( pData->bNoframes )
					{
						puDoMethod( pData->HScroller, PUM_CONTROL_GETINNERRECT, ( PUU32 )&Rect, 0 );
						Rect.Height = 1;
						Rect.Y--;
						puSetFillPen( _Param1, 0, 0, 0 );
						puRect( _Param1, &Rect );

						Rect.X--;
						puSetFillPen( _Param1, 255, 255, 255 );
						puRect( _Param1, &Rect );
					}
				}

				if( pData->bShowVScroller )
				{
					puDoMethod( pData->VScroller, PUM_CONTROL_RENDER, _Param1, _Param2 );

					if( pData->bNoframes )
					{
						puDoMethod( pData->VScroller, PUM_CONTROL_GETINNERRECT, ( PUU32 )&Rect, 0 );
						Rect.Width = 1;
						Rect.X--;
						puSetFillPen( _Param1, 0, 0, 0 );
						puRect( _Param1, &Rect );

						Rect.X--;
						puSetFillPen( _Param1, 255, 255, 255 );
						puRect( _Param1, &Rect );

						Rect.X -= 2;
						Rect.Width = 2;
				//		puSetFillPen( _Param1, 255, 0, 255 );
				//		puRect( _Param1, &Rect );
						puDoMethod( _Object, PUM_CONTROL_CLEARBG, _Param1, ( PUU32 )&Rect );
					}
				}

			//	puDoMethod( pData->Root, PUM_CONTROL_RENDER, _Param1, _Param2 );
			}
			break;

		case PUM_CONTROL_MOUSEEVENT:
			{
				pusRect Rect;
				pusMouseEvent* pMouseEvent;
				pMouseEvent = ( pusMouseEvent* )_Param1;

				if( pData->bShowHScroller )
				{
					puDoMethod( pData->HScroller, PUM_CONTROL_GETOUTERRECT, ( PUU32 )&Rect, 0 );
					if( puCheckPointInRect( &Rect, pMouseEvent->MouseX, pMouseEvent->MouseY ) )
					{
						puDoMethod( pData->HScroller, PUM_CONTROL_MOUSEEVENT, _Param1, _Param2 );
						return 0;
					}
				}

				if( pData->bShowVScroller )
				{
					puDoMethod( pData->VScroller, PUM_CONTROL_GETOUTERRECT, ( PUU32 )&Rect, 0 );
					if( puCheckPointInRect( &Rect, pMouseEvent->MouseX, pMouseEvent->MouseY ) )
					{
						puDoMethod( pData->VScroller, PUM_CONTROL_MOUSEEVENT, _Param1, _Param2 );
						return 0;
					}
				}

				puDoMethod( _Object, PUM_SCROLLING_MOUSEEVENT, _Param1, _Param2 );
			// If it's in the contents area, forward a PUM_SCROLLING_MOUSEEVENT to self
				// otherwise forward to superclass
			/*	pusRect Rect;
				pusMouseEvent* pMouseEvent;
				pMouseEvent = ( pusMouseEvent* )_Param1;

				puDoMethod( pData->Contents, PUM_CONTROL_GETINNERRECT, ( PUU32 )&Rect, 0 );
				if( puCheckPointInRect( &Rect, pMouseEvent->MouseX, pMouseEvent->MouseY ) )
					puDoMethod( _Object, PUM_SCROLLING_MOUSEEVENT, _Param1, _Param2 );
				else
					puDoMethod( pData->Root, PUM_CONTROL_MOUSEEVENT, _Param1, _Param2 );*/
			}
			break;

		/* Scrolling methods */
		case PUM_SCROLLING_GETVIEWRECT:
			{
				pusRect* pRect;
				PUU32 Size;
				pRect = ( pusRect* )_Param1;
				puDoMethod( _Object, PUM_CONTROL_GETINNERRECT, ( PUU32 )pRect, 0 );
	
				if( pData->bShowVScroller )
				{
					Size = puGetAttribute( pData->VScroller, PUA_CONTROL_MINWIDTH );
					pRect->Width -= Size;

					if( pData->bNoframes )
						pRect->Width -= 4;
				}

				if( pData->bShowHScroller )
				{
					Size = puGetAttribute( pData->HScroller, PUA_CONTROL_MINHEIGHT );
					pRect->Height -= Size;
					if( pData->bNoframes )
						pRect->Height -= 2;
				}

				if( !pData->bNoframes )
					puGetFrameInnerRect( 0, PUFRAME_BOX, PUSTATE_NORMAL, pRect, NULL );
			}
			break;

		//	return puDoMethod( pData->Contents, PUM_CONTROL_GETINNERRECT, _Param1, _Param2 );

		case PUM_SCROLLING_REDRAWINNER:
			return puDoMethod( _Object, PUM_CONTROL_REDRAWINNER, _Param1, _Param2 );
//			return puDoMethod( pData->Contents, PUM_CONTROL_REDRAWINNER, _Param1, _Param2 );

		default:
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
	}

	return 0;
}

void puScrollingCalcMinMax(	PULID _Object, pusScrollingClassData* _pData )
{
	if( !_pData->bNoframes )
	{
		pusRect Rect;
		PUU32 Val;

		Rect.X = Rect.Y = 0;
		Rect.Width = Rect.Height = 0;
		puGetFrameOuterRect( 0, PUFRAME_BOX, PUSTATE_NORMAL, &Rect, NULL );

		if( _pData->MinMaxUpdateFlags &= 1 << ( PUA_CONTROL_MAXWIDTH - PUA_CONTROL_MINWIDTH ) )
		{
			Val = puGetAttribute( _Object, PUA_CONTROL_MAXWIDTH );
			if( Val )
				puSetAttribute( _Object, PUA_CONTROL_MAXWIDTH, Val + Rect.Width );
		}
		
		if( _pData->MinMaxUpdateFlags &= 1 << ( PUA_CONTROL_MAXHEIGHT - PUA_CONTROL_MINWIDTH ) )
		{
			Val = puGetAttribute( _Object, PUA_CONTROL_MAXHEIGHT );
			if( Val )
				puSetAttribute( _Object, PUA_CONTROL_MAXHEIGHT, Val + Rect.Height );
		}

		if( _pData->MinMaxUpdateFlags &= 1 << ( PUA_CONTROL_MINWIDTH - PUA_CONTROL_MINWIDTH ) )
			puSetAttribute( _Object, PUA_CONTROL_MINWIDTH, puGetAttribute( _Object, PUA_CONTROL_MINWIDTH ) + Rect.Width );

		if( _pData->MinMaxUpdateFlags &= 1 << ( PUA_CONTROL_MINHEIGHT - PUA_CONTROL_MINWIDTH ) )
			puSetAttribute( _Object, PUA_CONTROL_MINHEIGHT, puGetAttribute( _Object, PUA_CONTROL_MINHEIGHT ) + Rect.Height );
	}

	_pData->MinMaxUpdateFlags = 0;
}

PUU32 puScrollingSetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr, PUU32 _Val )
{
	pusScrollingClassData* pData;

	pData = ( pusScrollingClassData* )_pData;

	switch( _Attr )
	{
		case PUA_SCROLLING_VIEWMINWIDTH:
//			if( pData->Contents )
				puSetAttribute( _Object, PUA_SPACER_MINWIDTH, _Val );
			break;

		case PUA_SCROLLING_VIEWMAXWIDTH:
		//	if( pData->Contents )
				puSetAttribute( _Object, PUA_SPACER_MAXWIDTH, _Val );
			break;

		case PUA_SCROLLING_VIEWMINHEIGHT:
		//	if( pData->Contents )
				puSetAttribute( _Object, PUA_SPACER_MINHEIGHT, _Val );
			break;

		case PUA_SCROLLING_VIEWMAXHEIGHT:
		//	if( pData->Contents )
				puSetAttribute( _Object, PUA_SPACER_MAXHEIGHT, _Val );
			break;

		case PUA_SCROLLING_WIDTH:
			pData->Width = _Val;
			if( pData->bAutoHideScrollers )
				puScrollingAutoHideHSlider( _Object, _pData );
			break;

		case PUA_SCROLLING_HORVIS:
			pData->HVis = _Val;
			if( pData->bAutoHideScrollers )
				puScrollingAutoHideHSlider( _Object, _pData );
			break;

		case PUA_SCROLLING_HEIGHT:
			pData->Height = _Val;
			if( pData->bAutoHideScrollers )
				puScrollingAutoHideVSlider( _Object, _pData );
			break;

		case PUA_SCROLLING_VERVIS:
			pData->VVis = _Val;
			if( pData->bAutoHideScrollers )
				puScrollingAutoHideVSlider( _Object, _pData );
			break;

		case PUA_SCROLLING_AUTOHIDESCROLLERS:
			pData->bAutoHideScrollers = ( PUU8 )_Val;
			break;

		case PUA_SCROLLING_SHOWHSCROLLER:
			pData->bShowHScroller = ( PUU8 )_Val;
			break;

		case PUA_SCROLLING_SHOWVSCROLLER:
			pData->bShowVScroller = ( PUU8 )_Val;
			break;

		case PUA_SCROLLING_NOFRAMES:
			pData->bNoframes = ( PUU8 )_Val;

			if( pData->bNoframes )
			{
				puSetAttribute( pData->HScroller, PUA_CONTROL_FRAME, PUFRAME_NONE );
				puSetAttribute( pData->VScroller, PUA_CONTROL_FRAME, PUFRAME_NONE );
			}
			else
			{
				puSetAttribute( pData->HScroller, PUA_CONTROL_FRAME, PUFRAME_BOX );
				puSetAttribute( pData->VScroller, PUA_CONTROL_FRAME, PUFRAME_BOX );
			}
			break;

		case PUA_CONTROL_MINWIDTH:
		case PUA_CONTROL_MAXWIDTH:
		case PUA_CONTROL_MINHEIGHT:
		case PUA_CONTROL_MAXHEIGHT:
			pData->MinMaxUpdateFlags |= 1 << ( _Attr - PUA_CONTROL_MINWIDTH );
			break;

	/*	case PUA_CONTROL_POSX:
		case PUA_CONTROL_POSY:
		case PUA_CONTROL_WIDTH:
		case PUA_CONTROL_HEIGHT:
			pData->Height = 0;
			puSetAttribute( pData->Contents, _Attr, _Val );
			break;*/

//			pData->Contents

/*		#define PUA_SCROLLING_WIDTH				PU_SCROLLINGCLASS_ATT + 5
#define PUA_SCROLLING_HEIGHT			PU_SCROLLINGCLASS_ATT + 6
#define PUA_SCROLLING_POSX				PU_SCROLLINGCLASS_ATT + 7
#define PUA_SCROLLING_POSY				PU_SCROLLINGCLASS_ATT + 8*/

	}

	return TRUE;
}

void puScrollingAutoHideHSlider( PULID _Object, pusScrollingClassData* _pData )
{
//	PULID Scroller;
	PUU32 LastHidden; //, Hidden;

//	if( _pData->pCol )
//	{
	//	Scroller = puGetObjectFromCollection( _pData->pCol, HSCROLLER );
//		LastHidden = puGetAttribute( Scroller, PUA_CONTROL_HIDDEN );
		LastHidden = _pData->bShowHScroller;

		_pData->bShowHScroller = _pData->HVis < _pData->Width;

		if( _pData->bShowHScroller != LastHidden )
		{
		//	puSetAttribute( Scroller, PUA_CONTROL_HIDDEN, Hidden );
			puDoMethod( _Object, PUM_CONTROL_RELAYOUT, 0, 0 );

			if( !_pData->bShowHScroller )
				puSetAttribute( _pData->HScroller, PUA_SLIDER_VALUE, 0 );
		}
//	}
}

void puScrollingAutoHideVSlider( PULID _Object, pusScrollingClassData* _pData )
{
//	PULID Scroller;
	PUU32 LastHidden; //, Hidden;

//	if( _pData->pCol )
//	{
	//	Scroller = puGetObjectFromCollection( _pData->pCol, VSCROLLER );
	//	LastHidden = puGetAttribute( Scroller, PUA_CONTROL_HIDDEN );
		LastHidden = _pData->bShowVScroller;

		_pData->bShowVScroller = _pData->VVis < _pData->Height;

		if( _pData->bShowVScroller != LastHidden )
		{
		//	puSetAttribute( Scroller, PUA_CONTROL_HIDDEN, Hidden );
			puDoMethod( _Object, PUM_CONTROL_RELAYOUT, 0, 0 );

			if( !_pData->bShowVScroller )
				puSetAttribute( _pData->VScroller, PUA_SLIDER_VALUE, 0 );
		}
//	}
}