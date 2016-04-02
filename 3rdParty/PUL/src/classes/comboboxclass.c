/* Poly
   Useful
   Lib */

#include <pul/pul.h>
#include "comboboxclass.h"
#include "../include/pul.h"

PUU32 puComboBoxGetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr );
PUU32 puComboBoxSetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr, PUU32 _Val );

enum
{
	COMBOBOX_WINDOW,
	COMBOBOX_LISTVIEW
};

static PUU32 puComboboxColDef[]=
{
	PU_ACTION_OBJDEF, COMBOBOX_WINDOW, ( PUU32 )"Window",
	PUA_WINDOW_FORCETOP, TRUE,
	PUA_WINDOW_POPUP, TRUE,
	PUA_WINDOW_CONTENTS,
		PU_ACTION_OBJDEF, COMBOBOX_LISTVIEW, ( PUU32 )"ListView",
			PUA_SCROLLING_NOFRAMES, TRUE,
			PUA_CONTROL_FRAME, PUFRAME_TEXTENTRY,
			PUA_LISTVIEW_HOVER, TRUE,
			PUA_LISTVIEW_EDITABLE, FALSE,
			PUA_LISTVIEW_SHOWSELECTED, TRUE,
			PUA_LISTVIEW_MAXNUMVISIBLE, 6,
			0,
				PUM_ADDNOTIFICATION,
					PUNOTIFY_CONDITION_ATTREQUALS, PUA_LISTVIEW_CLICKED, TRUE,
					PUNOTIFY_ACTION_SET, COMBOBOX_WINDOW, PUA_WINDOW_OPENED, FALSE,

				PUM_ADDNOTIFICATION,
					PUNOTIFY_CONDITION_ATTREQUALS, PUA_LISTVIEW_ENTERPRESSED, TRUE,
					PUNOTIFY_ACTION_SET, COMBOBOX_WINDOW, PUA_WINDOW_OPENED, FALSE,
			0,
	0,
		PUM_ADDNOTIFICATION,
			PUNOTIFY_CONDITION_ATTREQUALS, PUA_WINDOW_CLOSE, TRUE,
			PUNOTIFY_ACTION_SET, COMBOBOX_WINDOW, PUA_WINDOW_OPENED, FALSE,
	0
};

PUU32 puComboBoxClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 )
{
	PUU32 Res;
	pusComboBoxClassData* pData;
	pData = ( pusComboBoxClassData* )_pData;

	switch( _MethodID )
	{
		case PUM_NEW:
			pData->Table = 0;
			pData->PopupWindow = 0;
			pData->Listview = 0;
			pData->Selected = -1;

			pData->pCol = puCreateObjectCollection( puComboboxColDef );
			if( pData->pCol )
			{
				pData->PopupWindow = puGetObjectFromCollection( pData->pCol, COMBOBOX_WINDOW );
				pData->Listview = puGetObjectFromCollection( pData->pCol, COMBOBOX_LISTVIEW );
			}

			{
				PUU32 Notify[5];

				Notify[0] = PUNOTIFY_CONDITION_ATTRCHANGE;
				Notify[1] = PUA_LISTVIEW_CLICKED;
				Notify[2] = PUNOTIFY_ACTION_DOMETHOD;
				Notify[3] = _Object;
				Notify[4] = PUM_COMBOBOX_NOTIFYSELECT;
				puDoMethod( pData->Listview, PUM_ADDNOTIFICATION, ( PUU32 )Notify, 0 );

				Notify[1] = PUA_LISTVIEW_ENTERPRESSED;
				puDoMethod( pData->Listview, PUM_ADDNOTIFICATION, ( PUU32 )Notify, 0 );
			}

			Res = puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

			puSetAttribute( _Object, PUA_CONTROL_FRAME, PUFRAME_BUTTON );

			return Res;

		case PUM_DELETE:
			if( pData->Table )
				puDoMethod( pData->Table, PUM_REMREF, 0, 0 );

			if( pData->pCol )
				puDeleteObjectCollection( pData->pCol );
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
		

		case PUM_SET:
			puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
			return puComboBoxSetAttr( _Object, _Class, _pData, _Param1, _Param2 );

		case PUM_GET:
			return puComboBoxGetAttr( _Object, _Class,_pData, _Param1 );

		/* Control methods */
		case PUM_CONTROL_CALCMINMAX:
			{
				pusRect Rect, Rect2;
				PUU32 Width, Height;
				
				Rect.Width = Rect.Height = 0;
				puTextSize( _Param1, " ", 0, &Rect );

				puGetIconSize( PUICON_CHECKBOX, PUSTATE_NORMAL, &Rect2 );

				Width = 80;
				puSetAttribute( _Object, PUA_CONTROL_MINWIDTH, Width );
			//	puSetAttribute( _Object, PUA_CONTROL_MAXWIDTH, Width );

				if( Rect.Height > 12 )
					Height = Rect.Height;
				else
					Height = 12;
				puSetAttribute( _Object, PUA_CONTROL_MINHEIGHT, Height );
				puSetAttribute( _Object, PUA_CONTROL_MAXHEIGHT, Height );

				puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 ); 
			}
			break;

		case PUM_CONTROL_RENDER:
			{
				pusRect Rect, TmpRect;

				puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
				puDoMethod( _Object, PUM_CONTROL_GETINNERRECT, ( PUU32 )&Rect, 0 );

				TmpRect.Y = Rect.Y;
				TmpRect.Height = Rect.Height;
				TmpRect.X = Rect.X + Rect.Width - Rect.Height;
				TmpRect.Width = Rect.Height;
				puDoMethod( _Object, PUM_CONTROL_CLEARBG, _Param1, ( PUU32 )&TmpRect );

				puSetFillPen( _Param1, 0, 0, 0 );
				puTriangle(  _Param1,
					TmpRect.X + 4, TmpRect.Y + 4,
					TmpRect.X + TmpRect.Width - 4, TmpRect.Y + 4,
					TmpRect.X + ( TmpRect.Width >> 1 ), TmpRect.Y + TmpRect.Height - 4 );

				TmpRect.Width = 1;
				TmpRect.X--;
				puSetFillPen( _Param1, 255, 255, 255 );
				puRect( _Param1, &TmpRect );

				TmpRect.X--;
				puSetFillPen( _Param1, 0, 0, 0 );
				puRect( _Param1, &TmpRect );

				TmpRect.X = Rect.X;
				TmpRect.Width = Rect.Width - Rect.Height - 2;
				puDoMethod( _Object, PUM_CONTROL_CLEARBG, _Param1, ( PUU32 )&TmpRect );

				if( pData->Table )
				{
					PULID Record;
					PUU8* pText;

					puSetClipping( _Param1, &TmpRect );

					Record = ( PULID )puDoMethod( pData->Table, PUM_TABLE_GETRECORD, pData->Selected, 0 );
					pText = ( PUU8* )puDoMethod( pData->Table, PUM_TABLE_GETFIELDVAL, Record, 0 );

					if( pText )
					{
						puTextSize( _Param1, pText, 0, &Rect );
						puDrawText( _Param1, pText, 0, TmpRect.X, TmpRect.Y + ( ( TmpRect.Height - Rect.Height ) >> 1 ) );
					}

					puSetClipping( _Param1, NULL );
				}
			}
			break;

		case PUM_CONTROL_MOUSEEVENT:
			if( pData->pCol )
			{
				PULID Window;
				pusRect WndRect, Rect;
				pusMouseEvent* pMouseEvent;
				pMouseEvent = ( pusMouseEvent* )_Param1;

				if( ( pMouseEvent->ButtonCode & ~PU_KEYDOUBLECLICK ) == PUMOUSE_LMB )
				{
					Window = puGetAttribute( _Object, PUA_CONTROL_WINDOW );
					puDoMethod( Window, PUM_WINDOW_GETINNERRECT, ( PUU32 )&WndRect, 0 );
					puDoMethod( _Object, PUM_CONTROL_GETOUTERRECT, ( PUU32 )&Rect, 0 );

					if( pData->Selected >= 0 )
						puSetAttribute( pData->Listview, PUA_LISTVIEW_SELECTED, pData->Selected );

					puSetAttribute( pData->PopupWindow, PUA_WINDOW_XPOS, WndRect.X + Rect.X );
					puSetAttribute( pData->PopupWindow, PUA_WINDOW_YPOS, WndRect.Y + Rect.Y + Rect.Height - 1 );
					puSetAttribute( pData->PopupWindow, PUA_WINDOW_WIDTH, Rect.Width );
					puSetAttribute( pData->PopupWindow, PUA_WINDOW_HEIGHT, 1 );
					puSetAttribute( pData->PopupWindow, PUA_WINDOW_OPENED, TRUE );
				}
			}
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		/* ComboBox methods */
		case PUM_COMBOBOX_NOTIFYSELECT:
			if( pData->Listview )
				puSetAttribute( _Object, PUA_COMBOBOX_SELECTED, puGetAttribute( pData->Listview, PUA_LISTVIEW_SELECTED ) );
			break;

		default:
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
	}

	return 0;
}


PUU32 puComboBoxGetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr )
{
	pusComboBoxClassData* pData;
	pData = ( pusComboBoxClassData* )_pData;

	switch( _Attr )
	{
		case PUA_COMBOBOX_SELECTED:
			return pData->Selected;

		default:
			return puDoSuperMethod( _Object, _Class, PUM_GET, _Attr, 0 );
	}
}


PUU32 puComboBoxSetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr, PUU32 _Val )
{
	pusComboBoxClassData* pData;
	pData = ( pusComboBoxClassData* )_pData;

	switch( _Attr )
	{
		case PUA_COMBOBOX_TABLE:
			if( pData->Table )
				puDoMethod( pData->Table, PUM_REMREF, 0, 0 );

			pData->Table = _Val;
			puDoMethod( pData->Table, PUM_ADDREF, 0, 0 );
			pData->Selected = 0;

			if( pData->Listview )
				puSetAttribute( pData->Listview, PUA_LISTVIEW_TABLE, _Val );
			break;

		case PUA_COMBOBOX_SELECTED:
			pData->Selected = _Val;
			puDoMethod( _Object, PUM_CONTROL_REDRAWINNER, 0, 0 );
			break;
	}

	return TRUE;
}