/* Poly
   Useful
   Lib */

#include "../include/pul.h"
#include <pul/pul.h>
#include <stdlib.h>
#include "listviewclass.h"

PUU32 puListViewGetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr );
PUU32 puListViewSetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr, PUU32 _Val );
void puListViewCalcMinMax( PULID _Object, pusListViewClassData* _pData, PUU32 _Param1, PUU32 _Param2 );
void puListViewLayout( PULID _Object, pusListViewClassData* _pData, PUU32 _Param1, PUU32 _Param2 );
void puListViewRender( PULID _Object, pusListViewClassData* _pData, PUU32 _Param1, PUU32 _Param2 );

void puListViewKeyboardEvent( PULID _Object, pusListViewClassData* _pData, PUU32 _Param1, PUU32 _Param2 );
void puListViewMouseEvent( PULID _Object, pusListViewClassData* _pData, PUU32 _Param1, PUU32 _Param2 );

PUU32 puListViewGetRecordHeight( pusListViewClassData* _pData, PULID _Record, PUU32 _RastPort );
void puListViewDrawEntry( pusListViewClassData* _pData, PULID _Record, PUU32 _RastPort, pusRect* _pRect, PUU32* _pYPos );

void puListViewMakeVisible( PULID _Object, pusListViewClassData* _Data );
void puListViewAdd( PULID _Object, pusListViewClassData* _pData );
void puListViewRemove( PULID _Object, pusListViewClassData* _pData );
void puListViewEdit( PULID _Object, pusListViewClassData* _pData );
void puListViewSelUp( PULID _Object, pusListViewClassData* _pData );
void puListViewSelDown( PULID _Object, pusListViewClassData* _pData );

PUU32 puListViewClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 )
{
	pusListViewClassData* pData;
	pData = ( pusListViewClassData* )_pData;

	switch( _MethodID )
	{
		case PUM_NEW:
			pData->Table = 0;
			pData->FirstDisplayedRecord = 0;
			pData->SelectedRecord = 0;
			pData->HorScroll = 0;
			pData->NumRecordNotif = 0;
			pData->YScroll = 0;
			pData->bEditable = FALSE;
			pData->bShowSelected = TRUE;
			pData->bMustSelect = FALSE;
			pData->EntryHeight = 1;
			pData->bEditMode = FALSE;
			pData->SelectedRecordIndex = -1;
			pData->FirstDisplayedRecordIndex = 0;
			pData->bNewEntry = FALSE;
			pData->MaxNumVisible = 0;
			pData->bHover = FALSE;

			if( pData->TextEntry = puNewObjectFromName( "TextEntry", NULL ) )
			{
				PUU32 Notify[8];

				Notify[0] = PUNOTIFY_CONDITION_ATTREQUALS;
				Notify[1] = PUA_TEXTENTRY_EDITMODE;
				Notify[2] = FALSE;
				Notify[3] = PUNOTIFY_ACTION_SETVAL;
				Notify[4] = _Object;
				Notify[5] = PUA_LISTVIEW_EDITMODE;
				puDoMethod( pData->TextEntry, PUM_ADDNOTIFICATION, ( PUU32 )Notify, NULL );
			}

			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		case PUM_DELETE:
			if( pData->TextEntry )
				puDeleteObject( pData->TextEntry );

			if( pData->Table )
				puDoMethod( pData->Table, PUM_REMREF, 0, 0 );
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		case PUM_GET:
			return puListViewGetAttr( _Object, _Class, _pData, _Param1 );

		case PUM_SET:
			puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
			return puListViewSetAttr( _Object, _Class, _pData, _Param1, _Param2 );

		/* Control methods */
		case PUM_CONTROL_CALCMINMAX:
			puListViewCalcMinMax( _Object, pData, _Param1, _Param2 );
			puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 ); 
			break;

		case PUM_CONTROL_LAYOUT:
			puListViewLayout( _Object, pData, _Param1, _Param2 );
			puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
			break;

		case PUM_CONTROL_RENDER:
			puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
			puListViewRender( _Object, pData, _Param1, _Param2 );
			break;

		case PUM_CONTROL_KEYBOARDEVENT:
			puListViewKeyboardEvent( _Object, pData, _Param1, _Param2 );
			break;

		/* Scrolling methods */
		case PUM_SCROLLING_MOUSEEVENT:
			puListViewMouseEvent( _Object, pData, _Param1, _Param2 );
			break;

		/* Listview methods */
		case PUM_LISTVIEW_ADD:
			puListViewAdd( _Object, pData );
			break;

		case PUM_LISTVIEW_REMOVE:
			puListViewRemove( _Object, pData );
			break;

		case PUM_LISTVIEW_EDIT:
			puListViewEdit( _Object, pData );
			break;

		case PUM_LISTVIEW_SELUP:
			puListViewSelUp( _Object, pData );
			break;

		case PUM_LISTVIEW_SELDOWN:
			puListViewSelDown( _Object, pData );
			break;

		default:
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
	}

	return 0;
}

PUU32 puListViewGetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr )
{
	pusListViewClassData* pData;
	pData = ( pusListViewClassData* )_pData;

	switch( _Attr )
	{
		case PUA_LISTVIEW_SELECTED:
			return ( PUU32 )pData->SelectedRecordIndex;

		default:
			return puDoSuperMethod( _Object, _Class, PUM_GET, _Attr, 0 );
	}

	return 0;
}

PUU32 puListViewSetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr, PUU32 _Val )
{
	pusListViewClassData* pData;
	pData = ( pusListViewClassData* )_pData;

	switch( _Attr )
	{
		case PUA_LISTVIEW_HOVER:
			pData->bHover = ( PUU8 )_Val;
			break;

		case PUA_LISTVIEW_SELECTED:
			if( pData->Table && pData->SelectedRecordIndex != ( PUS32 )_Val )
			{
				pData->SelectedRecordIndex = _Val;
				pData->SelectedRecord = puDoMethod( pData->Table, PUM_TABLE_GETRECORD, pData->SelectedRecordIndex, 0 );
			}
			break;

		case PUA_LISTVIEW_MAXNUMVISIBLE:
			pData->MaxNumVisible = _Val;
			break;

		case PUA_LISTVIEW_SHOWSELECTED:
			pData->bShowSelected = _Val;

			if( !pData->bShowSelected )
				pData->bEditable = FALSE;
			break;

		case PUA_LISTVIEW_EDITABLE:
			pData->bEditable = _Val;

			if( !pData->TextEntry )
				pData->bEditable = FALSE;

			if( pData->bEditable )
				pData->bShowSelected = TRUE;
			break;

		case PUA_LISTVIEW_MUSTSELECT:
			pData->bMustSelect = _Val;
			break;

		case PUA_LISTVIEW_TABLE:
			if( pData->Table )
			{
				if( pData->NumRecordNotif )
					puDoMethod( pData->Table, PUM_REMNOTIFICATION, pData->NumRecordNotif, 0 );

				puDoMethod( pData->Table, PUM_REMREF, 0, 0 );

				pData->NumRecordNotif = 0;
			}

			pData->Table = _Val;

			if( pData->Table )
			{
				PUU32 Notify[8];

				puDoMethod( pData->Table, PUM_ADDREF, 0, 0 );
				pData->FirstDisplayedRecord = puDoMethod( pData->Table, PUM_TABLE_GETFIRSTRECORD, 0, 0 );
				pData->FirstDisplayedRecordIndex = 0;

				Notify[0] = PUNOTIFY_CONDITION_ATTRCHANGE;
				Notify[1] = PUA_TABLE_NUMRECORDS;
				Notify[2] = PUNOTIFY_ACTION_SETVAL;
				Notify[3] = _Object;
				Notify[4] = PUA_SCROLLING_HEIGHT;
				puDoMethod( pData->Table, PUM_ADDNOTIFICATION, ( PUU32 )Notify, 0 );

				pData->NumRecordNotif = puGetAttribute( pData->Table, PUA_NOTIFICATION );

				puSetAttribute( _Object, PUA_SCROLLING_HEIGHT, puGetAttribute( pData->Table, PUA_TABLE_NUMRECORDS ) );
			}

			pData->SelectedRecord = 0;
			pData->SelectedRecordIndex = -1;
			
			puDoMethod( _Object, PUM_CONTROL_RELAYOUT, 0, 0 );
			break;

		case PUA_SCROLLING_HEIGHT:
			if( _Val )
			{
				if( pData->Table )
				{
					if( !pData->FirstDisplayedRecord )
					{
						pData->FirstDisplayedRecord = puDoMethod( pData->Table, PUM_TABLE_GETFIRSTRECORD, 0, 0 );
						pData->FirstDisplayedRecordIndex = 0;

						if( pData->bMustSelect )
						{
							pData->SelectedRecord = pData->FirstDisplayedRecord;
							pData->SelectedRecordIndex = 0;
						}

						puDoMethod( _Object, PUM_CONTROL_RELAYOUT, 0, 0 );
					}
				}

				puDoMethod( _Object, PUM_SCROLLING_REDRAWINNER, 0, 0 );
			}
			else
			{
				pData->FirstDisplayedRecord = 0;
				pData->FirstDisplayedRecordIndex = 0;
				pData->SelectedRecord = 0;
				pData->SelectedRecordIndex = -1;
				puDoMethod( _Object, PUM_SCROLLING_REDRAWINNER, 0, 0 );
			}
			break;


		case PUA_SCROLLING_POSY:
			if( pData->Table )
			{
				PUS32 NewPos = ( PUS32 )_Val;
			 	PUU32 MaxPos = puGetAttribute( pData->Table, PUA_TABLE_NUMRECORDS ) - pData->NumVisibleEntries + 1;
				if( NewPos > ( PUS32 )MaxPos )
					NewPos = MaxPos;

				if( NewPos < 0 )
					NewPos = 0;

				if( pData->YScroll != NewPos )
				{
					pData->YScroll = NewPos;
					pData->FirstDisplayedRecord = puDoMethod( pData->Table, PUM_TABLE_GETRECORD, (PUU32 )NewPos, 0 );
					pData->FirstDisplayedRecordIndex = ( PUU32 )NewPos;
					puDoMethod( _Object, PUM_SCROLLING_REDRAWINNER, 0, 0 );
				}
			}
			break;

		case PUA_LISTVIEW_EDITMODE:
			if( _Val )
				puDoMethod( _Object, PUM_LISTVIEW_EDIT, 0, 0 );
			else if( pData->bEditMode )
			{
				PUU8* pEdited;
				PUU8 String[256], c;

				// Stop editing
				pData->bEditMode = FALSE;

				// Retrieve the edited buffer and replace the field string
				// If the edited string is empty, we don't commit the change
				pEdited = ( PUU8* )puGetAttribute( pData->TextEntry, PUA_TEXTENTRY_BUFFER );

				strcpy( String, pEdited );

				// Strip trailing spaces/tab
				pEdited = String + strlen( String );

				while( pEdited > String )
				{
					c = *--pEdited;
					if( c != ' ' && c != '\t' )
						break;
				}

				*( pEdited + 1 ) = 0;

				// Strip leading spaces/tab
				pEdited = String;

				while( c = *pEdited++ )
				{
					if( c != ' ' && c != '\t' )
						break;
				}

				pEdited--;

				if( *pEdited && pData->Table )
				{
					puSetAttribute( pData->Table, PUA_TABLE_CURRENTRECORD, pData->SelectedRecord );
					puSetAttribute( pData->Table, PUA_TABLE_CURRENTFIELD, 0 );
					puDoMethod( pData->Table, PUM_TABLE_SETFIELDVAL, ( PUU32 )pEdited, 0 );
				}

				// If it's a new entry and nothing was typed, remove it
				if( pData->bNewEntry && !*pEdited )
					puDoMethod( _Object, PUM_LISTVIEW_REMOVE, 0, 0 );

				pData->bNewEntry = FALSE;

				puDoMethod( _Object, PUM_SCROLLING_REDRAWINNER, 0, 0 );
			}
			break;
	}

	return TRUE;
}

void puListViewCalcMinMax( PULID _Object, pusListViewClassData* _pData, PUU32 _Param1, PUU32 _Param2 )
{
	PUU32 MinHeight = 30;

	puSetAttribute( _Object, PUA_CONTROL_MINWIDTH, 50 );

	if( _pData->MaxNumVisible && _pData->Table )
	{

		MinHeight = puGetAttribute( _pData->Table, PUA_TABLE_NUMRECORDS );

		if( MinHeight > _pData->MaxNumVisible )
			MinHeight = _pData->MaxNumVisible;

		MinHeight *= puListViewGetRecordHeight( _pData, 0, _Param1 );
	}

	puSetAttribute( _Object, PUA_CONTROL_MINHEIGHT, MinHeight );

	puDoMethod( _pData->TextEntry, PUM_CONTROL_CALCMINMAX, _Param1, _Param2 );
}

void puListViewLayout( PULID _Object, pusListViewClassData* _pData, PUU32 _Param1, PUU32 _Param2 )
{
	PUS32 MaxPos;
	pusRect Rect;

	if( _pData->FirstDisplayedRecord )
	{
		_pData->EntryHeight = puListViewGetRecordHeight( _pData, _pData->FirstDisplayedRecord, _Param1 );
		puDoMethod( _Object, PUM_SCROLLING_GETVIEWRECT, ( PUU32 )&Rect, 0 );

		_pData->NumVisibleEntries = ( Rect.Height / _pData->EntryHeight + 1 );

		// Check max vertical position
		MaxPos = puGetAttribute( _pData->Table, PUA_TABLE_NUMRECORDS ) - _pData->NumVisibleEntries + 1;

		if( MaxPos < 0 )
			MaxPos = 0;

		if( _pData->YScroll > MaxPos )
		{
			_pData->YScroll = MaxPos;
			_pData->FirstDisplayedRecord = puDoMethod( _pData->Table, PUM_TABLE_GETRECORD, _pData->YScroll, 0 );
			_pData->FirstDisplayedRecordIndex = _pData->YScroll;
		}

		// Trigger notifications
		puSetAttribute( _Object, PUA_LISTVIEW_NUMVISIBLE, _pData->NumVisibleEntries );

		// Set scrolling area height
		puSetAttribute( _Object, PUA_SCROLLING_VERVIS, _pData->NumVisibleEntries - 1 );
	}
	else
	{
		_pData->NumVisibleEntries = 0;
		puSetAttribute( _Object, PUA_SCROLLING_VERVIS, 0 );
	}

	// Stop edition
	if( _pData->bEditMode )
		puSetAttribute( _Object, PUA_LISTVIEW_EDITMODE, FALSE );
}

void puListViewRender( PULID _Object, pusListViewClassData* _pData, PUU32 _Param1, PUU32 _Param2 )
{
	pusRect Rect;
	PULID CurrentRecord, OldCurrentRecord;
	PUU32 Count;
	PUU32 YPos;

	puDoMethod( _Object, PUM_SCROLLING_GETVIEWRECT, ( PUU32 )&Rect, 0 );
	puDoMethod( _Object, PUM_CONTROL_CLEARBG, _Param1, ( PUU32 )&Rect );

	YPos = Rect.Y;

	Count = _pData->NumVisibleEntries;
	CurrentRecord = _pData->FirstDisplayedRecord;

	OldCurrentRecord = puGetAttribute( _pData->Table, PUA_TABLE_CURRENTRECORD );

	if( _pData->Table )
	{
		for( ; Count; Count-- )
		{
			puListViewDrawEntry( _pData, CurrentRecord, _Param1, &Rect, &YPos );
			if( !( CurrentRecord = puDoMethod( _pData->Table, PUM_TABLE_GETNEXTRECORD, CurrentRecord, 0 ) ) )
				break;
		}
	}

	if( _pData->bEditMode )
		puDoMethod( _pData->TextEntry, PUM_CONTROL_RENDER, _Param1, _Param2 );

	puSetAttribute( _pData->Table, PUA_TABLE_CURRENTRECORD, OldCurrentRecord );
}

PUU32 puListViewGetRecordHeight( pusListViewClassData* _pData, PULID _Record, PUU32 _RastPort )
{
	// For now I handle only records with only one textual field.
	// Some more work is needed on the Table class to support more complex datatypes.
	// Also record height calculation should probably be done by Table,
	// or maybe a separate "recordview" class should be implemented, then Table manage
	// exclusively storage/access of data, another class (recordview) manages the layout/rendering of
	// a record, and the listview manage the display of several records, dispatch mouse events, and
	// do the scrolling (this would allow tables with very complex records and complex record edition
	// within the listview -> recordviewclass should support both display and edit modes).
	// Also all the records should have the same height, determined by the recordview object
	// with the listview.
	// This will be rather complicated, so some predefined class that create a set of
	// listview/recordview/table with only text fields (possibly several of them) at once
	// would be useful for most case where you just want to put up a listview with text entries.

	PUU8*	pText;
	pusRect	Rect;

	if( _pData->Table )
	{
		// Just get the first field of the record and get its size
		pText = " "; //( PUU8* )puDoMethod( _pData->Table, PUM_TABLE_GETFIELDVAL, _Record, 0 );
		if( pText )
		{
			puTextSize( _RastPort, pText, 0, &Rect );

			if( _pData->bEditable )
			{
				PUS32 Height;
				Height = puGetAttribute( _pData->TextEntry, PUA_CONTROL_MINHEIGHT );

				if( Height > Rect.Height )
					return ( Height + Rect.Height ) / 2;
			}

			return Rect.Height + 2;
		}
		else
			return 1;
	}
	else
		return 1;
}

void puListViewDrawEntry( pusListViewClassData* _pData, PULID _Record, PUU32 _RastPort, pusRect* _pRect, PUU32* _pYPos )
{
	PUU8*	pText;
	pusRect	Rect;

	if( _pData->Table )
	{
		puSetClipping( _RastPort, _pRect );
		pText = ( PUU8* )puDoMethod( _pData->Table, PUM_TABLE_GETFIELDVAL, _Record, 0 );
		if( pText )
		{
			if( !( _Record == _pData->SelectedRecord && _pData->bEditMode ) )
			{
				if( _Record == _pData->SelectedRecord && _pData->bShowSelected )
				{
					Rect.X = _pRect->X;
					Rect.Y = *_pYPos; // + ( _pData->EntryHeight - Rect.Height / 2 );
					Rect.Width = _pRect->Width;
					Rect.Height = _pData->EntryHeight;
					puSetFillPen( _RastPort, 60 + 40, 100 + 40, 128 + 40 );
					puRect( _RastPort, &Rect );

					puSetDrawPen( _RastPort, 255, 255, 255 );
				}
				else
					puSetDrawPen( _RastPort, 0, 0, 0 );

				puTextSize( _RastPort, pText, 0, &Rect );
				puDrawText( _RastPort, pText, 0, _pRect->X - _pData->HorScroll, *_pYPos + ( _pData->EntryHeight - Rect.Height ) / 2 );
			}

			*_pYPos += _pData->EntryHeight;
		}
		puSetClipping( _RastPort, NULL );
		puSetDrawPen( _RastPort, 0, 0, 0 );
	}
}

void puListViewKeyboardEvent( PULID _Object, pusListViewClassData* _pData, PUU32 _Param1, PUU32 _Param2 )
{
	pusKeyboardEvent* pKeyboardEvent;
	pKeyboardEvent = ( pusKeyboardEvent* )_Param1;
	
	if( pKeyboardEvent->Type == PUKEYBOARD_RAWKEY )
	{
		switch( pKeyboardEvent->Code )
		{
			case PUKEY_DEL:
				if( _pData->bEditable )
					puDoMethod( _Object, PUM_LISTVIEW_REMOVE, 0, 0 );
				break;

			case PUKEY_UP:
				puDoMethod( _Object, PUM_LISTVIEW_SELUP, 0, 0 );
				break;

			case PUKEY_DOWN:
				puDoMethod( _Object, PUM_LISTVIEW_SELDOWN, 0, 0 );
				break;
		}
	}
	else if( pKeyboardEvent->Type == PUKEYBOARD_ASCII )
	{
		switch( pKeyboardEvent->Code )
		{
			case 0xd:	// Return
				puSetAttribute( _Object, PUA_LISTVIEW_ENTERPRESSED, TRUE );
				if( _pData->bEditable )
				{
					if( pKeyboardEvent->Qualifiers & PUQUAL_SHIFT )
						puDoMethod( _Object, PUM_LISTVIEW_EDIT, 0, 0 );
					else
						puDoMethod( _Object, PUM_LISTVIEW_ADD, 0, 0 );
				}
				else if( _pData->bShowSelected && _pData->SelectedRecord )
					puSetAttribute( _Object, PUA_LISTVIEW_DOUBLECLICKED, TRUE );
				break;
		}
	}
}

void puListViewMouseEvent( PULID _Object, pusListViewClassData* _pData, PUU32 _Param1, PUU32 _Param2 )
{
	PUU32 State;
	PUS32 LastSelected;
	pusMouseEvent* pMouseEvent;
	pusRect Rect;

	pMouseEvent = ( pusMouseEvent* )_Param1;
	State = puGetAttribute( _Object, PUA_CONTROL_STATE );

	puDoMethod( _Object, PUM_SCROLLING_GETVIEWRECT, ( PUU32 )&Rect, 0 );

	LastSelected = _pData->SelectedRecordIndex;
	
	switch( pMouseEvent->ButtonCode )
	{
		case PUMOUSE_NONE:
			if( !_pData->bHover )
				break;

			if( !puCheckPointInRect( &Rect, pMouseEvent->MouseX, pMouseEvent->MouseY ) )
				break;

		case PUMOUSE_LMB:
			if( pMouseEvent->ButtonCode == PUMOUSE_LMB )
				puSetAttribute( _Object, PUA_LISTVIEW_CLICKED, TRUE );
			if( _pData->Table )
			{
				_pData->SelectedRecordIndex = _pData->YScroll + ( pMouseEvent->MouseY - Rect.Y ) / _pData->EntryHeight;
				if( !( _pData->SelectedRecord = puDoMethod( _pData->Table, PUM_TABLE_GETRECORD, _pData->SelectedRecordIndex, 0 ) ) )
				{
					if( _pData->bMustSelect )
					{
						if( _pData->SelectedRecord = puDoMethod( _pData->Table, PUM_TABLE_GETLASTRECORD, 0, 0 ) )
							_pData->SelectedRecordIndex = puGetAttribute( _pData->Table, PUA_TABLE_NUMRECORDS );
					}
				}

				if( !_pData->bEditable && ( _pData->SelectedRecordIndex - _pData->YScroll ) == ( PUS32 )( _pData->NumVisibleEntries - 1 ) )
					puSetAttribute( _Object, PUA_SCROLLING_POSY, _pData->YScroll + 1 );
				else if( _pData->bShowSelected && LastSelected != _pData->SelectedRecordIndex )
					puDoMethod( _Object, PUM_SCROLLING_REDRAWINNER, 0, 0 );

				// For notification
				if( LastSelected != _pData->SelectedRecordIndex )
					puSetAttribute( _Object, PUA_LISTVIEW_SELECTED, _pData->SelectedRecordIndex );
			}
			break;

		case PUMOUSE_LMB | PU_KEYDOUBLECLICK:
			if( _pData->bShowSelected && !_pData->bEditable )
			{
				PUU32 Index, Record;
				Index = _pData->YScroll + ( pMouseEvent->MouseY - Rect.Y ) / _pData->EntryHeight;
				Record = puDoMethod( _pData->Table, PUM_TABLE_GETRECORD, Index, 0 );

				if( Record )
					puSetAttribute( _Object, PUA_LISTVIEW_DOUBLECLICKED, TRUE );
			}
			else if( _pData->Table && _pData->bEditable && _pData->TextEntry )
			{
				_pData->SelectedRecordIndex = _pData->YScroll + ( pMouseEvent->MouseY - Rect.Y ) / _pData->EntryHeight;
				_pData->SelectedRecord = puDoMethod( _pData->Table, PUM_TABLE_GETRECORD, _pData->SelectedRecordIndex, 0 );

				puDoMethod( _Object, PUM_LISTVIEW_EDIT, 0, 0 );
			}
			break;
	}
}

void puListViewMakeVisible( PULID _Object, pusListViewClassData* _pData )
{
	if( _pData->SelectedRecordIndex < _pData->FirstDisplayedRecordIndex )
		puSetAttribute( _Object, PUA_SCROLLING_POSY, _pData->SelectedRecordIndex );

	if( ( PUS32 )( _pData->FirstDisplayedRecordIndex + _pData->NumVisibleEntries - 1 ) > 0 )
		if( _pData->SelectedRecordIndex >= ( PUS32 )( _pData->FirstDisplayedRecordIndex + _pData->NumVisibleEntries - 1 ) )
			puSetAttribute( _Object, PUA_SCROLLING_POSY, _pData->SelectedRecordIndex - ( _pData->NumVisibleEntries - 2 ) );
}

void puListViewAdd( PULID _Object, pusListViewClassData* _pData )
{
	PULID Record;

	if( _pData->bEditable && _pData->TextEntry && _pData->Table )
	{
		// Add record
		if( Record = puDoMethod( _pData->Table, PUM_TABLE_NEWRECORD, 0, 0 ) )
		{
			puDoMethod( _pData->Table, PUM_TABLE_SETFIELDVAL, ( PUU32 )"", 0 );
			puDoMethod( _pData->Table, PUM_TABLE_ADDRECORD, 0, 0 );

			// Select record
			_pData->SelectedRecord = Record;
			_pData->SelectedRecordIndex = puGetAttribute( _pData->Table, PUA_TABLE_NUMRECORDS ) - 1;

			puDoMethod( _Object, PUM_CONTROL_RELAYOUT, 0, 0 );

			// Edit
			_pData->bNewEntry = TRUE;
			puDoMethod( _Object, PUM_LISTVIEW_EDIT, 0, 0 );
		}
	}
}

void puListViewRemove( PULID _Object, pusListViewClassData* _pData )
{
	PULID Record, RemoveRecord;

	if( _pData->bShowSelected && _pData->Table && _pData->SelectedRecord )
	{
		RemoveRecord = _pData->SelectedRecord;

		// Select next record
		if( Record = puDoMethod( _pData->Table, PUM_TABLE_GETNEXTRECORD, _pData->SelectedRecord, 0 ) )
		{
			_pData->SelectedRecord = Record;
			if( RemoveRecord == _pData->FirstDisplayedRecord )
				_pData->FirstDisplayedRecord = Record;
		}
		// If no next record, try to select the previous record
		else if( Record = puDoMethod( _pData->Table, PUM_TABLE_GETPREVRECORD, _pData->SelectedRecord, 0 ) )
		{
			_pData->SelectedRecord = Record;
			_pData->SelectedRecordIndex--;
		}
		else
		{
			_pData->SelectedRecord = NULL;
			_pData->SelectedRecordIndex = -1;
			_pData->FirstDisplayedRecord = NULL;
		}

		// Remove record
		puDoMethod( _pData->Table, PUM_TABLE_REMRECORD, RemoveRecord, 0 );
		puSetAttribute( _Object, PUA_SCROLLING_POSY, _pData->YScroll );
		puListViewMakeVisible( _Object, _pData );
		puDoMethod( _Object, PUM_SCROLLING_REDRAWINNER, 0, 0 );
	}
}

void puListViewEdit( PULID _Object, pusListViewClassData* _pData )
{
	pusRect Rect;

	if( _pData->Table && _pData->bEditable && _pData->TextEntry )
	{
		if( _pData->SelectedRecord )
		{
			PUS32 Height, YPos;

			puListViewMakeVisible( _Object, _pData );
			puDoMethod( _Object, PUM_SCROLLING_GETVIEWRECT, ( PUU32 )&Rect, 0 );

			/* Setup the textentry control */
			puSetAttribute( _pData->TextEntry, PUA_TEXTENTRY_BUFFER, puDoMethod( _pData->Table, PUM_TABLE_GETFIELDVAL, _pData->SelectedRecord, 0 ) );

			Height = puGetAttribute( _pData->TextEntry, PUA_CONTROL_MINHEIGHT );
			puSetAttribute( _pData->TextEntry, PUA_CONTROL_HEIGHT, Height );
			puSetAttribute( _pData->TextEntry, PUA_CONTROL_WIDTH, Rect.Width );
			puSetAttribute( _pData->TextEntry, PUA_CONTROL_POSX, Rect.X );

			YPos = _pData->EntryHeight * ( _pData->SelectedRecordIndex - _pData->YScroll );

			if( _pData->YScroll != _pData->SelectedRecordIndex )
				YPos += ( ( PUS32 )_pData->EntryHeight - Height ) / 2;

			if( ( YPos + Height ) > Rect.Height )
				YPos = Rect.Height - Height;

			puSetAttribute( _pData->TextEntry, PUA_CONTROL_POSY, YPos + Rect.Y );

			puDoMethod( _pData->TextEntry, PUM_CONTROL_RELAYOUT, 0, 0 );
			puSetAttribute( _pData->TextEntry, PUA_TEXTENTRY_EDITMODE, TRUE );
			_pData->bEditMode = TRUE;

			puDoMethod( _Object, PUM_SCROLLING_REDRAWINNER, 0, 0 );
		}
	}
}

void puListViewSelUp( PULID _Object, pusListViewClassData* _pData )
{
	PULID Record;

	if( _pData->bShowSelected )
	{
		if( _pData->Table )
		{
			if( _pData->SelectedRecord )
			{
				if( Record = puDoMethod( _pData->Table, PUM_TABLE_GETPREVRECORD, _pData->SelectedRecord, 0 ) )
				{
					_pData->SelectedRecord = Record;
					_pData->SelectedRecordIndex--;
					puListViewMakeVisible( _Object, _pData );
					puDoMethod( _Object, PUM_SCROLLING_REDRAWINNER, 0, 0 );
				}
			}
			else
			{
				_pData->SelectedRecord = puDoMethod( _pData->Table, PUM_TABLE_GETFIRSTRECORD, 0, 0 );
				_pData->SelectedRecordIndex = 0;
				puSetAttribute( _Object, PUA_SCROLLING_POSY, 0 );
				puDoMethod( _Object, PUM_SCROLLING_REDRAWINNER, 0, 0 );
			}

			// For notification
			puSetAttribute( _Object, PUA_LISTVIEW_SELECTED, _pData->SelectedRecordIndex );
		}
	}
	else if( _pData->YScroll )
		puSetAttribute( _Object, PUA_SCROLLING_POSY, _pData->YScroll - 1 );
}

void puListViewSelDown( PULID _Object, pusListViewClassData* _pData )
{
	PULID Record;

	if( _pData->bShowSelected )
	{
		if( _pData->Table )
		{
			if( _pData->SelectedRecord )
			{
				if( Record = puDoMethod( _pData->Table, PUM_TABLE_GETNEXTRECORD, _pData->SelectedRecord, 0 ) )
				{
					_pData->SelectedRecord = Record;
					_pData->SelectedRecordIndex++;
					puListViewMakeVisible( _Object, _pData );
					puDoMethod( _Object, PUM_SCROLLING_REDRAWINNER, 0, 0 );
				}
			}
			else
			{
				_pData->SelectedRecord = puDoMethod( _pData->Table, PUM_TABLE_GETFIRSTRECORD, 0, 0 );
				_pData->SelectedRecordIndex = 0;
				puSetAttribute( _Object, PUA_SCROLLING_POSY, 0 );
				puDoMethod( _Object, PUM_SCROLLING_REDRAWINNER, 0, 0 );
			}

			// For notification
			puSetAttribute( _Object, PUA_LISTVIEW_SELECTED, _pData->SelectedRecordIndex );
		}
	}
	else
		puSetAttribute( _Object, PUA_SCROLLING_POSY, _pData->YScroll + 1 );
}