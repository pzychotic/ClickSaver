/* Poly
   Useful
   Lib */

#include <pul/pul.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "textentryclass.h"
#include "../include/pul.h"

PUU32 puTextEntryGetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr );
PUU32 puTextEntrySetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr, PUU32 _Val );

void puTextEntryDrawCursor( pusTextEntryClassData* _pData, PUU32 _RastPort, pusRect* _pRect, PUS32 _Start, PUS32 _End );
void puTextEntryRender( PULID _Object, pusTextEntryClassData* _pData, PUU32 _Param1, PUU32 _Param2 );
void puTextEntryMouseEvent( PULID _Object, pusTextEntryClassData* _pData, PUU32 _Param1, PUU32 _Param2 );
void puTextEntryKeyboardEvent( PULID _Object, pusTextEntryClassData* _pData, PUU32 _Param1, PUU32 _Param2 );
void puCalcCharPos( PULID _Object, pusTextEntryClassData* _pData, PUU32 _RastPort, PUS32 XPos, PUU32* _pCharPos, PUU32* _pPixPos );
PUU32 puTextEntryManageScrolling( PULID _Object, pusTextEntryClassData* _pData, PUU32 _RastPort );

void puTextEntryDeleteSelection( pusTextEntryClassData* _pData );
void puTextEntryDeleteChar( pusTextEntryClassData* _pData );
void puTextEntryInsert( PULID _Object, pusTextEntryClassData* _pData, PUU8* _pString, PULID WindowObj );
void puTextEntryStartEdit( PULID _Object, pusTextEntryClassData* _pData, PULID WindowObj );

void puTextEntryCheckValue( PULID _Object, pusTextEntryClassData* _pData );

PUU32 puTextEntryClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 )
{
	pusTextEntryClassData* pData;
	pData = ( pusTextEntryClassData* )_pData;

	switch( _MethodID )
	{
		case PUM_NEW:
			if( !puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 ) )
				return FALSE;

			pData->BufferSize = 100;
			if( pData->pBuffer = malloc( pData->BufferSize + 1 ) )
				*pData->pBuffer = 0;
			pData->pFirstChar = pData->pBuffer;
			pData->bEditMode = FALSE;
			pData->LastCursorPos = 0;
			pData->CursorPixPos = 0;
			pData->SelectionEndPixPos = -1;
			pData->LastEndSelPos = 0;
			pData->bSelectionMode = FALSE;
			pData->bReadOnly = FALSE;
			pData->bHilight = FALSE;
			pData->Value = 0;
			pData->bNumeric = FALSE;
			pData->Min = 0;
			pData->Max = 100;
			pData->Format = PUFORMAT_DECIMAL;

			puSetAttribute( _Object, PUA_CONTROL_ACTIVEWEAK, TRUE );
			puSetAttribute( _Object, PUA_CONTROL_FRAME, PUFRAME_TEXTENTRY );
			return TRUE;

		case PUM_DELETE:
			if( pData->pBuffer )
				free( pData->pBuffer );
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		case PUM_SET:
				puTextEntrySetAttr( _Object, _Class, _pData, _Param1, _Param2 );
				return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
			 

		case PUM_GET:
			return puTextEntryGetAttr( _Object, _Class,_pData, _Param1 );

		/* Control methods */
		case PUM_CONTROL_CALCMINMAX:
			{
				pusRect Rect;
				
				Rect.Width = Rect.Height = 0;

				puTextSize( _Param1, " ", 0, &Rect );

				/* Min width */
				puSetAttribute( _Object, PUA_CONTROL_MINWIDTH, 40 );

				/* No max width */
				puSetAttribute( _Object, PUA_CONTROL_MAXWIDTH, 0 );

				/* Fixed height */
				puSetAttribute( _Object, PUA_CONTROL_MINHEIGHT, Rect.Height );
				puSetAttribute( _Object, PUA_CONTROL_MAXHEIGHT, Rect.Height );

				puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 ); 
			}
			break;

		case PUM_CONTROL_LAYOUT:
			if( pData->pBuffer )
			{
				pData->pFirstChar = pData->pBuffer;

				if( pData->bEditMode )
					puSetAttribute( _Object, PUA_TEXTENTRY_EDITMODE, FALSE );
			}

			puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 ); 
			break;

		case PUM_CONTROL_RENDER:
			if( !_Param2 )
				puDoMethod( _Object, PUM_CONTROL_CLEARBG, _Param1, 0 );

			puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
			puTextEntryRender( _Object, pData, _Param1, _Param2 );
			break;

		case PUM_CONTROL_MOUSEEVENT:
			puTextEntryMouseEvent( _Object, pData, _Param1, _Param2 );
			break;

		case PUM_CONTROL_KEYBOARDEVENT:
			puTextEntryKeyboardEvent( _Object, pData, _Param1, _Param2 );
			break;

		default:
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
	}

	return 0;
}


PUU32 puTextEntryGetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr )
{
	pusTextEntryClassData* pData;
	pData = ( pusTextEntryClassData* )_pData;

	switch( _Attr )
	{
		case PUA_TEXTENTRY_BUFFERSIZE:
			return pData->BufferSize;

		case PUA_TEXTENTRY_BUFFER:
			return ( PUU32 )pData->pBuffer;

		case PUA_TEXTENTRY_VALUE:
			return pData->Value;

		default:
			return puDoSuperMethod( _Object, _Class, PUM_GET, _Attr, 0 );
	}
}


PUU32 puTextEntrySetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr, PUU32 _Val )
{
	pusTextEntryClassData* pData;
	pData = ( pusTextEntryClassData* )_pData;

	switch( _Attr )
	{
		case PUA_TEXTENTRY_BUFFERSIZE:
			{
				PUU8* pNewBuffer;

				if( pNewBuffer = malloc( _Val + 1 ) )
				{
					if( pData->pBuffer )
					{
						strncpy( pNewBuffer, pData->pBuffer, _Val );
						free( pData->pBuffer );
					}

					pData->pBuffer = pNewBuffer;
					pData->BufferSize = _Val;
					pData->pFirstChar = pData->pBuffer;
					puDoMethod( _Object, PUM_CONTROL_REDRAWINNER, 0, 0 );
				}
			}
			break;

		case PUA_TEXTENTRY_BUFFER:
			if( pData->pBuffer )
			{
				PUU8* pString = ( PUU8* )_Val;
				if( pString )
					strncpy( pData->pBuffer, pString, pData->BufferSize );
				else
					*pData->pBuffer = 0;
				puSetAttribute( _Object, PUA_TEXTENTRY_EDITMODE, FALSE );
				puDoMethod( _Object, PUM_CONTROL_REDRAWINNER, 0, 0 );
			}
			break;

		case PUA_TEXTENTRY_READONLY:
			pData->bReadOnly = _Val;

			if( pData->bReadOnly )
				puSetAttribute( _Object, PUA_CONTROL_FRAME, PUFRAME_READONLYTEXTENTRY );
			else
				puSetAttribute( _Object, PUA_CONTROL_FRAME, PUFRAME_TEXTENTRY );
			break;

		case PUA_TEXTENTRY_EDITMODE:
			if( _Val )
				puTextEntryStartEdit( _Object, pData, puGetAttribute( _Object, PUA_CONTROL_WINDOW ) );
			else
			{
				pData->bEditMode = FALSE;
				pData->pFirstChar = pData->pBuffer;

				puDoMethod( _Object, PUM_CONTROL_REDRAWINNER, 0, 0 );
				puSetAttribute( _Object, PUA_CONTROL_STATE, PUSTATE_NORMAL );
				puTextEntryCheckValue( _Object, _pData );
			}
			break;

		case PUA_TEXTENTRY_HILIGHT:
			pData->bHilight = _Val;
			puDoMethod( _Object, PUM_CONTROL_REDRAWINNER, 0, 0 );
			break;

		case PUA_TEXTENTRY_NUMERIC:
			pData->bNumeric = _Val;
			puSetAttribute( _Object, PUA_TEXTENTRY_VALUE, pData->Value );
			break;

		case PUA_TEXTENTRY_VALUE:
			pData->Value = _Val;

			if( pData->bNumeric )
			{
				char Tmp[256];

				if( pData->Value < pData->Min )
					pData->Value = pData->Min;

				if( pData->Value > pData->Max )
					pData->Value = pData->Max;

				switch( pData->Format )
				{
					case PUFORMAT_DECIMAL:
						sprintf( Tmp, "%d", pData->Value );
						break;

					case PUFORMAT_HEX:
						sprintf( Tmp, "%X", pData->Value );
						break;

					default:
						strcpy( Tmp, "<invalid format attribute>" );
				}

				puSetAttribute( _Object, PUA_TEXTENTRY_BUFFER, ( PUU32 )Tmp );
			}
			break;

		case PUA_TEXTENTRY_MIN:
			pData->Min = _Val;
			break;

		case PUA_TEXTENTRY_MAX:
			pData->Max = _Val;
			break;

		case PUA_TEXTENTRY_FORMAT:
			if( pData->Format != _Val )
			{
				pData->Format = _Val;

				// Force buffer reformatting
				puSetAttribute( _Object, PUA_TEXTENTRY_VALUE, pData->Value );
			}
			break;
	}

	return TRUE;
}

void puTextEntryRender( PULID _Object, pusTextEntryClassData* _pData, PUU32 _Param1, PUU32 _Param2 )
{
	if( _pData->pBuffer )
	{
		pusRect Rect;
		puDoMethod( _Object, PUM_CONTROL_GETINNERRECT, ( PUU32 )&Rect, 0 );
		puSetClipping( _Param1, &Rect );

		if( !_Param2 )
		{
			if( _pData->bHilight )
				puSetDrawPen( _Param1, 255, 0, 0 );
			else
				puSetDrawPen( _Param1, 0, 0, 0 );

			puDrawText( _Param1, _pData->pFirstChar, 0, Rect.X, Rect.Y );
		}

		if( _Param2 && _pData->LastCursorPos >= 0 && 
		  ( ( _pData->CursorPixPos != _pData->LastCursorPos ) ||
		  ( _pData->SelectionEndPixPos != _pData->LastEndSelPos ) ) )
			puTextEntryDrawCursor( _pData, _Param1, &Rect, _pData->LastCursorPos, _pData->LastEndSelPos );

		if( _pData->bEditMode )
		{
			if( !_Param2 || _pData->CursorPixPos != _pData->LastCursorPos || _pData->SelectionEndPixPos != _pData->LastEndSelPos )
			{
				puTextEntryDrawCursor( _pData, _Param1, &Rect, _pData->CursorPixPos, _pData->SelectionEndPixPos );
				_pData->LastCursorPos = _pData->CursorPixPos;
				_pData->LastEndSelPos = _pData->SelectionEndPixPos;
			}
		}

		puSetClipping( _Param1, NULL );
	}
}

void puTextEntryDrawCursor( pusTextEntryClassData* _pData, PUU32 _RastPort, pusRect* _pRect, PUS32 _Start, PUS32 _End )
{
	pusRect Rect;

	Rect.X = _pRect->X + _Start;
	Rect.Y = _pRect->Y;

	if( _End >= 0 )
		Rect.Width = _End - _Start;
	else
		Rect.Width = 1;

	Rect.Height = _pRect->Height;

	if( !_pData->bReadOnly || _End >= 0 )
		puInvRect( _RastPort, &Rect );
}

void puTextEntryMouseEvent( PULID _Object, pusTextEntryClassData* _pData, PUU32 _Param1, PUU32 _Param2 )
{
	pusMouseEvent* pMouseEvent;
	PUU32 RastPort;
	pusRect Rect;

	pMouseEvent = ( pusMouseEvent* )_Param1;

	switch( pMouseEvent->ButtonCode )
	{
		case PUMOUSE_LMB:
			if( puGetAttribute( _Object, PUA_CONTROL_STATE ) == PUSTATE_ACTIVE )
			{
				puDoMethod( _Object, PUM_CONTROL_GETINNERRECT, ( PUU32 )&Rect, 0 );

				if( !puCheckPointInRect( &Rect, pMouseEvent->MouseX, pMouseEvent->MouseY ) )
				{
					puSetAttribute( _Object, PUA_TEXTENTRY_EDITMODE, FALSE );

					/* Resend the event */
					puDoMethod( _Param2, PUM_WINDOW_MOUSEEVENT, _Param1, 0 );
					break;
				}
			}

			if( !_pData->pFirstChar )
				break;

			if( !_pData->bEditMode )
			{
				puTextEntryStartEdit( _Object, _pData, _Param2 );
				break;
			}


			_pData->bSelectionMode = TRUE;
			puSetAttribute( _Object, PUA_CONTROL_ACTIVEWEAK, FALSE );

			_pData->SelectionEndCharPos = 0;
			_pData->SelectionEndPixPos = -1;

			if( RastPort = puDoMethod( _Param2, PUM_WINDOW_GETRASTPORT, 0, 0 ) )
			{
				puCalcCharPos( _Object, _pData, RastPort, pMouseEvent->MouseX, &_pData->CursorCharPos, &_pData->CursorPixPos );
			//	puDoMethod( _Param2, PUM_WINDOW_RELEASERASTPORT, 0, 0 );
				puDoMethod( _Object, PUM_CONTROL_REDRAWINNER, 0, 0 );

			}
			else
				puSetAttribute( _Object, PUA_TEXTENTRY_EDITMODE, FALSE );
			break;

		case PUMOUSE_LMB | PU_KEYUP:
			_pData->bSelectionMode = FALSE;
			puSetAttribute( _Object, PUA_CONTROL_ACTIVEWEAK, TRUE );

			// Make sure that the leftmost side of the selection is first
			if( _pData->SelectionEndPixPos >= 0 )
			{
				if( _pData->CursorCharPos > _pData->SelectionEndCharPos )
				{
					PUU32 CharPos, PixPos;
					CharPos = _pData->SelectionEndCharPos;
					PixPos = _pData->SelectionEndPixPos;
					_pData->SelectionEndCharPos = _pData->CursorCharPos;
					_pData->SelectionEndPixPos = _pData->CursorPixPos;
					_pData->CursorCharPos = CharPos;
					_pData->CursorPixPos = PixPos;
				}
			}
			break;

		case PUMOUSE_NONE:
			if( _pData->bSelectionMode )
				if( RastPort = puDoMethod( _Param2, PUM_WINDOW_GETRASTPORT, 0, 0 ) )
				{
					puCalcCharPos( _Object, _pData, RastPort, pMouseEvent->MouseX, &_pData->SelectionEndCharPos, &_pData->SelectionEndPixPos );
//					puDoMethod( _Param2, PUM_WINDOW_RELEASERASTPORT, 0, 0 );

					if( _pData->SelectionEndCharPos == _pData->CursorCharPos )
						_pData->SelectionEndPixPos = -1;

					puDoMethod( _Object, PUM_CONTROL_REDRAWINNER, TRUE, 0 );

					puSetAttribute( _Object, PUA_CONTROL_STATE, PUSTATE_ACTIVE );
				}			
			break;
	}
}

void puTextEntryKeyboardEvent( PULID _Object, pusTextEntryClassData* _pData, PUU32 _Param1, PUU32 _Param2 )
{
	pusKeyboardEvent* pKeyboardEvent;
	PUU32 RastPort;
	pusRect Rect, Rect2;
	PUU8* pCBText;

	pKeyboardEvent = ( pusKeyboardEvent* )_Param1;

	if( !_pData->bEditMode )
		return;

	/* Manage cut&paste */
	if( pKeyboardEvent->Type == PUKEYBOARD_ASCII )
	{
		switch( pKeyboardEvent->Code )
		{
			case 'X' - 64:
				if( _pData->SelectionEndPixPos >= 0 )
				{
					puPutClipboardText( _pData->pBuffer + _pData->CursorCharPos, _pData->SelectionEndCharPos - _pData->CursorCharPos );
					
					if( !_pData->bReadOnly && !_pData->bNumeric )
					{
						puTextEntryDeleteSelection( _pData );
						puDoMethod( _Object, PUM_CONTROL_REDRAWINNER, 0, 0 );
					}
				}
				break;

			case 'C' - 64:
				if( _pData->SelectionEndPixPos >= 0 )
					puPutClipboardText( _pData->pBuffer + _pData->CursorCharPos, _pData->SelectionEndCharPos - _pData->CursorCharPos );
				return;

			case 'V' - 64:
				if( _pData->bReadOnly || _pData->bNumeric )
					break;

				if( pCBText = puGetClipboardText() )
				{
					if( _pData->SelectionEndPixPos >= 0 )
						puTextEntryDeleteSelection( _pData );

					puTextEntryInsert( _Object, _pData, pCBText, _Param2 );

					free( pCBText );
				}
				return;
		}
	}

	if( _pData->bReadOnly )
		return;

	puDoMethod( _Object, PUM_CONTROL_GETINNERRECT, ( PUU32 )&Rect, 0 );

	if( pKeyboardEvent->Type == PUKEYBOARD_ASCII )
	{
		char Key[2];
		Key[1] = 0;

		switch( pKeyboardEvent->Code )
		{
			case 8:		// Backspace
				if( _pData->SelectionEndPixPos >= 0 )
				{
					puTextEntryDeleteSelection( _pData );
					puDoMethod( _Object, PUM_CONTROL_REDRAWINNER, 0, 0 );
				}
				else if( _pData->CursorCharPos > 0 )
				{
					_pData->CursorCharPos--;
					puTextEntryDeleteChar( _pData );
					if( _pData->CursorCharPos )
					{
						if( RastPort = puDoMethod( _Param2, PUM_WINDOW_GETRASTPORT, 0, 0 ) )
						{
							puTextSize( RastPort, _pData->pFirstChar, _pData->CursorCharPos - ( _pData->pFirstChar - _pData->pBuffer ), &Rect2 );
//							puDoMethod( _Param2, PUM_WINDOW_RELEASERASTPORT, 0, 0 );
							_pData->CursorPixPos = Rect2.Width;
						}
					}
					else
						_pData->CursorPixPos = 0;

					puDoMethod( _Object, PUM_CONTROL_REDRAWINNER, 0, 0 );
					if( RastPort = puDoMethod( _Param2, PUM_WINDOW_GETRASTPORT, 0, 0 ) )
					{
						while( puTextEntryManageScrolling( _Object, _pData, RastPort ) );
					//	puDoMethod( _Param2, PUM_WINDOW_RELEASERASTPORT, 0, 0 );
					}
				}
				break;

			case 9:		// Tab
			case 0xd:	// Return
			case 0x1b:	// Escape
				puSetAttribute( _Object, PUA_TEXTENTRY_EDITMODE, FALSE );
				break;

			default:
				if( pKeyboardEvent->Code >= 32 )
				{
					PUU32 bValidEntry = TRUE;

					if( _pData->bNumeric )
					{
						switch( _pData->Format )
						{
							case PUFORMAT_DECIMAL:
								if( !isdigit( pKeyboardEvent->Code ) )
									bValidEntry = FALSE;
								break;

							case PUFORMAT_HEX:
								{
									char c;

									if( !isdigit( pKeyboardEvent->Code ) )
									{
										c = pKeyboardEvent->Code & 0xdf;

										if( c < 'A' || c > 'F' )
											bValidEntry = FALSE;
										else
											pKeyboardEvent->Code &= 0xdf;
									}
								}
								break;
						}
					}

					if( !bValidEntry )
						break;

					if( _pData->SelectionEndPixPos >= 0 )
						puTextEntryDeleteSelection( _pData );
					Key[0] = pKeyboardEvent->Code;
					puTextEntryInsert( _Object, _pData, Key, _Param2 );
				}
				break;
		}
	}
	else if( pKeyboardEvent->Type == PUKEYBOARD_RAWKEY )
	{
		switch( pKeyboardEvent->Code )
		{
			case PUKEY_DEL:
				if( _pData->SelectionEndPixPos >= 0 )
					puTextEntryDeleteSelection( _pData );
				else
					puTextEntryDeleteChar( _pData );

				puDoMethod( _Object, PUM_CONTROL_REDRAWINNER, 0, 0 );
				break;

			case PUKEY_LEFT:
				// If there's a selection, we just remove it and put the cursor at the begin
				if( _pData->LastEndSelPos >= 0 )
				{
					_pData->SelectionEndPixPos = -1;
					puDoMethod( _Object, PUM_CONTROL_REDRAWINNER, TRUE, 0 );
					if( RastPort = puDoMethod( _Param2, PUM_WINDOW_GETRASTPORT, 0, 0 ) )
					{
						puTextEntryManageScrolling( _Object, _pData, RastPort );
//						puDoMethod( _Param2, PUM_WINDOW_RELEASERASTPORT, 0, 0 );
					}
					return;
				}

				if( _pData->CursorCharPos > 0 )
				{
					_pData->CursorCharPos--;
					if( _pData->CursorCharPos )
					{
						if( RastPort = puDoMethod( _Param2, PUM_WINDOW_GETRASTPORT, 0, 0 ) )
						{
							puTextSize( RastPort, _pData->pFirstChar, _pData->CursorCharPos - ( _pData->pFirstChar - _pData->pBuffer ), &Rect2 );
					//		puDoMethod( _Param2, PUM_WINDOW_RELEASERASTPORT, 0, 0 );
							_pData->CursorPixPos = Rect2.Width;
						}
					}
					else
						_pData->CursorPixPos = 0;

					puDoMethod( _Object, PUM_CONTROL_REDRAWINNER, TRUE, 0 );
					if( RastPort = puDoMethod( _Param2, PUM_WINDOW_GETRASTPORT, 0, 0 ) )
					{
						puTextEntryManageScrolling( _Object, _pData, RastPort );
					//	puDoMethod( _Param2, PUM_WINDOW_RELEASERASTPORT, 0, 0 );
					}
				}
				break;

			case PUKEY_RIGHT:
				// If there's a selection, we just remove it and put the cursor at the end
				if( _pData->LastEndSelPos >= 0 )
				{
					_pData->CursorCharPos = _pData->SelectionEndCharPos;
					_pData->CursorPixPos = _pData->SelectionEndPixPos;
					_pData->SelectionEndPixPos = -1;
					puDoMethod( _Object, PUM_CONTROL_REDRAWINNER, TRUE, 0 );
					if( RastPort = puDoMethod( _Param2, PUM_WINDOW_GETRASTPORT, 0, 0 ) )
					{
						while( puTextEntryManageScrolling( _Object, _pData, RastPort ) );
					//	puDoMethod( _Param2, PUM_WINDOW_RELEASERASTPORT, 0, 0 );
					}
					return;
				}

				if( _pData->CursorCharPos < strlen( _pData->pBuffer ) )
				{
					_pData->CursorCharPos++;
					if( RastPort = puDoMethod( _Param2, PUM_WINDOW_GETRASTPORT, 0, 0 ) )
					{
						puTextSize( RastPort, _pData->pFirstChar, _pData->CursorCharPos - ( _pData->pFirstChar - _pData->pBuffer ), &Rect2 );

						_pData->CursorPixPos = Rect2.Width;
						puDoMethod( _Object, PUM_CONTROL_REDRAWINNER, TRUE, 0 );

						puTextEntryManageScrolling( _Object, _pData, RastPort );
					//	puDoMethod( _Param2, PUM_WINDOW_RELEASERASTPORT, 0, 0 );
					}
				}

				break;
		}
	}
}

void puCalcCharPos( PULID _Object, pusTextEntryClassData* _pData, PUU32 _RastPort, PUS32 XPos, PUU32* _pCharPos, PUU32* _pPixPos )
{
	PUU32 CharPos, PixPos;
	pusRect Rect, Rect2;

	puDoMethod( _Object, PUM_CONTROL_GETINNERRECT, ( PUU32 )&Rect, 0 );

	if( XPos < Rect.X )
	{
		PixPos = 0;
		CharPos = 0;
	}
	else
	{
		if( CharPos = puTextFit( _RastPort, _pData->pFirstChar, 0, XPos - Rect.X ) )
		{
			puTextSize( _RastPort, _pData->pFirstChar, CharPos, &Rect2 );
			PixPos = Rect2.Width;
		}
		else
			PixPos = 0;
	}

	*_pCharPos = CharPos + ( _pData->pFirstChar - _pData->pBuffer );
	*_pPixPos = PixPos;
}

PUU32 puTextEntryManageScrolling( PULID _Object, pusTextEntryClassData* _pData, PUU32 _RastPort )
{
	pusRect Rect, Rect2;
	PUU8* pOldFirstChar;

	pOldFirstChar = _pData->pFirstChar;
	puDoMethod( _Object, PUM_CONTROL_GETINNERRECT , ( PUU32 )&Rect, 0 );

	puTextSize( _RastPort, _pData->pBuffer, 0, &Rect2 );

	if( Rect2.Width <= Rect.Width )
		return FALSE;

	// Check if cursor is outside to the left
	if( _pData->CursorPixPos < 14 )
	{
		_pData->pFirstChar -= 5;

		if( _pData->pFirstChar < _pData->pBuffer )
			_pData->pFirstChar = _pData->pBuffer;
	}

	// Check if cursor is outside to the right
	if( _pData->CursorPixPos > ( Rect.Width - 14 ) )
	{
		_pData->pFirstChar += 5;

		if( _pData->pFirstChar > ( _pData->pBuffer + strlen( _pData->pBuffer ) ) )
			_pData->pFirstChar = _pData->pBuffer + strlen( _pData->pBuffer );
	}

	if( _pData->pFirstChar != pOldFirstChar )
	{
		// recalc cursor end pix position
		puTextSize( _RastPort, _pData->pFirstChar, _pData->CursorCharPos - ( _pData->pFirstChar - _pData->pBuffer ), &Rect2 );
		_pData->CursorPixPos = Rect2.Width;

		puDoMethod( _Object, PUM_CONTROL_REDRAWINNER, 0, 0 );

		return TRUE;
	}

	return FALSE;
}

void puTextEntryDeleteSelection( pusTextEntryClassData* _pData )
{
	if( _pData->SelectionEndPixPos >= 0 )
	{
		strcpy( _pData->pBuffer + _pData->CursorCharPos, _pData->pBuffer + _pData->SelectionEndCharPos );
		_pData->SelectionEndPixPos = -1;
	}
}

void puTextEntryDeleteChar( pusTextEntryClassData* _pData )
{
	strcpy( _pData->pBuffer + _pData->CursorCharPos, _pData->pBuffer + _pData->CursorCharPos + 1 );
}

void puTextEntryInsert( PULID _Object, pusTextEntryClassData* _pData, PUU8* _pString, PULID WindowObj )
{
	PUU32 Count;
	PUU8* pSrc;
	PUU8* pDest;
	PUU32 StringLen;
	PUU32 RastPort;
	pusRect Rect2;

	StringLen = strlen( _pString );

	if( strlen( _pData->pBuffer ) + StringLen > _pData->BufferSize )
		return;

	pSrc = _pData->pBuffer + strlen( _pData->pBuffer );
	pDest = pSrc + StringLen;
	
	Count = strlen( _pData->pBuffer ) - _pData->CursorCharPos + 1;

	for( ; Count > 0; Count-- )
		*pDest-- = *pSrc--;

	memcpy( pDest - StringLen + 1, _pString, StringLen );

	_pData->CursorCharPos += StringLen;

	if( RastPort = puDoMethod( WindowObj, PUM_WINDOW_GETRASTPORT, 0, 0 ) )
	{
		puTextSize( RastPort, _pData->pFirstChar, _pData->CursorCharPos - ( _pData->pFirstChar - _pData->pBuffer ), &Rect2 );

		_pData->CursorPixPos = Rect2.Width;

		while( puTextEntryManageScrolling( _Object, _pData, RastPort ) );
//		puDoMethod( WindowObj, PUM_WINDOW_RELEASERASTPORT, 0, 0 );
	}

	puDoMethod( _Object, PUM_CONTROL_REDRAWINNER, 0, 0 );
}

void puTextEntryStartEdit( PULID _Object, pusTextEntryClassData* _pData, PULID WindowObj )
{
	PUU32 RastPort;
	pusRect Rect;

	_pData->bEditMode = TRUE;
	_pData->LastCursorPos = -1;
	_pData->LastEndSelPos = -1;

	/* Select all the text */
	_pData->pFirstChar = _pData->pBuffer;
	_pData->CursorCharPos = 0;
	_pData->SelectionEndCharPos = strlen( _pData->pBuffer );
	_pData->CursorPixPos = 0;
	if( RastPort = puDoMethod( WindowObj, PUM_WINDOW_GETRASTPORT, 0, 0 ) )
	{
		puTextSize( RastPort, _pData->pBuffer, 0, &Rect );
//		puDoMethod( WindowObj, PUM_WINDOW_RELEASERASTPORT, 0, 0 );
		if( Rect.Width )
			_pData->SelectionEndPixPos = Rect.Width;
		else
			_pData->SelectionEndPixPos = -1;
		puSetAttribute( _Object, PUA_CONTROL_STATE, PUSTATE_ACTIVE );
	}
	puDoMethod( _Object, PUM_CONTROL_REDRAW, TRUE, 0 );
}


void puTextEntryCheckValue( PULID _Object, pusTextEntryClassData* _pData )
{
	PUU32 OldVal;

	if( !_pData->bNumeric )
		return;

	OldVal = _pData->Value;

	switch( _pData->Format )
	{
		case PUFORMAT_DECIMAL:
			_pData->Value = atoi( _pData->pBuffer );
			break;

		case PUFORMAT_HEX:
			sscanf( _pData->pBuffer, "%x", &_pData->Value );
			break;
	}

	if( _pData->Value < _pData->Min )
		_pData->Value = _pData->Min;

	if( _pData->Value > _pData->Max )
		_pData->Value = _pData->Max;

	if( _pData->Value != OldVal )
		puSetAttribute( _Object, PUA_TEXTENTRY_VALUE, _pData->Value );
}
