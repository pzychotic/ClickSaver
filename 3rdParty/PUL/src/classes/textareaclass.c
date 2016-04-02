/* Poly
   Useful
   Lib */

/*

TODO:
	- Layout (rewrap buffer ds prelayout, calc scroll vertical)
	- Render (comme listview)

  puis:
	- Scrolling vertical (voir listview)
	- Mode hauteur forcée (puis s'occuper du layout en deux passes dans groupe vertical)

    * Gestion curseur
		- fonction: position x,y -> num ligne non wrappée + num char
		- gestion affichage curseur/sélection (maintient position dans
		  espace non-wrappé + routine de calcul position wrappée, appellée
		  lors des rewrap, même partiels (pas se faire chier))
		- gestion mouvements curseur/sélection (souris)
		- gestion mouvement curseur clavier (+ kill sélection)
	
	- Puis: manipulation du buffer non-wrappé (avec mise à jour buffer wrappé): insertion,
	  suppression, split, merge, multi-line insert, multi-line delete	
  */

#include "../include/pul.h"
#include <pul/pul.h>
#include <stdlib.h>
#include "textareaclass.h"

PUU32 puTextAreaGetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr );
PUU32 puTextAreaSetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr, PUU32 _Val );
void puTextAreaCalcMinMax( PULID _Object, pusTextAreaClassData* _pData, PUU32 _Param1, PUU32 _Param2 );
void puTextAreaPreLayout( PULID _Object, pusTextAreaClassData* _pData, PUU32 _Param1, PUU32 _Param2 );
void puTextAreaLayout( PULID _Object, pusTextAreaClassData* _pData, PUU32 _Param1, PUU32 _Param2 );
void puTextAreaRender( PULID _Object, pusTextAreaClassData* _pData, PUU32 _Param1, PUU32 _Param2 );
void puTextAreaDrawLine( pusTextAreaClassData* _pData, pusTextAreaLine* _pLine, PUU32 _RastPort, pusRect* _pRect, PUU32 _YPos );

void puTextAreaKeyboardEvent( PULID _Object, pusTextAreaClassData* _pData, PUU32 _Param1, PUU32 _Param2 );
void puTextAreaMouseEvent( PULID _Object, pusTextAreaClassData* _pData, PUU32 _Param1, PUU32 _Param2 );

void puTextAreaDeleteBuffer( pusTextAreaClassData* _pData );
void puTextAreaDeleteWrappedBuffer( pusTextAreaClassData* _pData );
PUU8 puTextAreaImportBuffer( pusTextAreaClassData* _pData, PUU8* _pBuffer );
PUU8* puTextAreaExportBuffer( pusTextAreaClassData* _pData );
PUS32 puTextAreaWrapBuffer( PULID _Object, PUU32 _Rastport, pusTextAreaClassData* _pData, PUU8 _bDryRun );
PUU8* puTextAreaFitLine( PULID _Object, PUU32 _Rastport, PUU8* _pLineText, PUU32* _pLength );


void pTextAreaPixelToPos( PUU32 _RastPort, pusTextAreaPos* _pPos );
void puTextAreaPosToPixel( PUU32 _RastPort, pusTextAreaPos* _pPos );
void puTextAreaUnwrapPos( pusTextAreaPos* _pPos );
void puTextAreaWrapPos( PUU32 _RastPort, pusTextAreaPos* _pPos );


PUU32 puTextAreaClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 )
{
	PUU32 Res;
	pusTextAreaClassData* pData;
	pData = ( pusTextAreaClassData* )_pData;

	switch( _MethodID )
	{
		case PUM_NEW:
			puInitList( &pData->UnwrappedLines );
			puInitList( &pData->WrappedLines );

			pData->pDisplayLines = &pData->WrappedLines;

			pData->pFirstDisplayLine = NULL;

			pData->WrapMode = PUWRAPMODE_WORD;
			pData->bForceHeight = FALSE;
			pData->bReadOnly = FALSE;
			pData->bNoCutPaste = FALSE;

			pData->NumDisplayLines = 0;
			pData->NumBufferLines = 0;
			pData->ScrollWidth = 0;

			pData->ScrollX = 0;
			pData->ScrollY = 0;

			pData->CenterMode = PUTA_CENTERMODE_LEFT;
			pData->Font = PUFONT_NORMAL;

			Res = puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

			puSetAttribute( _Object, PUA_SCROLLING_AUTOHIDESCROLLERS, FALSE );
			puSetAttribute( _Object, PUA_SCROLLING_NOFRAMES, TRUE );
			puSetAttribute( _Object, PUA_CONTROL_FRAME, PUFRAME_TEXTENTRY );

			return Res;

		case PUM_DELETE:
			puTextAreaDeleteWrappedBuffer( pData );
			puTextAreaDeleteBuffer( pData );
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		case PUM_GET:
			return puTextAreaGetAttr( _Object, _Class, _pData, _Param1 );

		case PUM_SET:
			puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
			return puTextAreaSetAttr( _Object, _Class, _pData, _Param1, _Param2 );

		/* Control methods */
		case PUM_CONTROL_CALCMINMAX:
			puTextAreaCalcMinMax( _Object, pData, _Param1, _Param2 );
			puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 ); 
			break;

		case PUM_CONTROL_PRELAYOUT:
			if( !_Param2 )
				puSetAttribute( _Object, PUA_CONTROL_DEFERRELAYOUT, TRUE );
//			puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
			puTextAreaPreLayout( _Object, pData, _Param1, _Param2 );
			puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
			break;

		case PUM_CONTROL_LAYOUT:
			puTextAreaLayout( _Object, pData, _Param1, _Param2 );
			puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

			// For test
		//	puTextAreaPreLayout( _Object, pData, _Param1, _Param2 );
			puSetAttribute( _Object, PUA_CONTROL_DEFERRELAYOUT, FALSE );
			break;

		case PUM_CONTROL_RENDER:
			puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
			puTextAreaRender( _Object, pData, _Param1, _Param2 );
			break;

		case PUM_CONTROL_KEYBOARDEVENT:
			puTextAreaKeyboardEvent( _Object, pData, _Param1, _Param2 );
			break;

		/* Scrolling methods */
		case PUM_SCROLLING_MOUSEEVENT:
			puTextAreaMouseEvent( _Object, pData, _Param1, _Param2 );
			break;

		/* TextArea methods */
		case PUM_TEXTAREA_APPENDBUFFER:
			puTextAreaImportBuffer( pData, ( PUU8* )_Param1 );
			puDoMethod( _Object, PUM_CONTROL_RELAYOUT, 0, 0 );
			puDoMethod( _Object, PUM_CONTROL_REDRAW, 0, 0 );
			break;

		default:
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
	}

	return 0;
}

PUU32 puTextAreaGetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr )
{
	pusTextAreaClassData* pData;
	pData = ( pusTextAreaClassData* )_pData;

	switch( _Attr )
	{
		case PUA_TEXTAREA_BUFFER:
			return ( PUU32 )puTextAreaExportBuffer( pData );

		default:
			return puDoSuperMethod( _Object, _Class, PUM_GET, _Attr, 0 );
	}

	return 0;
}

PUU32 puTextAreaSetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr, PUU32 _Val )
{
	pusTextAreaClassData* pData;
	pData = ( pusTextAreaClassData* )_pData;

	switch( _Attr )
	{
		case PUA_TEXTAREA_WRAPMODE:
			pData->WrapMode = _Val;
			break;

		case PUA_TEXTAREA_FORCEHEIGHT:
			pData->bForceHeight = ( PUU8 )_Val;
			break;

		case PUA_TEXTAREA_READONLY:
			pData->bReadOnly = ( PUU8 )_Val;
			puSetAttribute( _Object, PUA_CONTROL_FRAME, PUFRAME_READONLYTEXTENTRY );
			break;

		case PUA_TEXTAREA_NOCUTPASTE:
			pData->bNoCutPaste = ( PUU8 )_Val;
			break;

		case  PUA_TEXTAREA_CENTERMODE:
			pData->CenterMode = ( PUU8 )_Val;
			break;

		case PUA_TEXTAREA_FONT:
			pData->Font = ( PUU8 )_Val;
			break;

		case PUA_TEXTAREA_BUFFER:
			puTextAreaDeleteBuffer( pData );
			puTextAreaImportBuffer( pData, ( PUU8* )_Val );
			break;

		case PUA_SCROLLING_POSY:
			// Get first line address
			// lazy way
			{
				pusNode* pCurrent;
				pusNode* pNext;
				PUS32 NewPos = ( PUS32 )_Val;

			 	PUU32 MaxPos = pData->NumDisplayLines - pData->NumVisibleLines;
				if( NewPos > ( PUS32 )MaxPos )
					NewPos = MaxPos;

				if( NewPos < 0 )
					NewPos = 0;


				if( pData->ScrollY != ( PUU32 )NewPos )
				{
					pData->ScrollY = NewPos;

					pCurrent = pData->pDisplayLines->pFirst;

					while( pNext = pCurrent->pNext )
					{
						if( !NewPos-- )
							break;
						pCurrent = pNext;
					}

					if( !pNext )
						return TRUE;

					pData->pFirstDisplayLine = ( pusTextAreaLine* )pCurrent;
					puDoMethod( _Object, PUM_SCROLLING_REDRAWINNER, 0, 0 );
				}
			}
			break;
	}

	return TRUE;
}

void puTextAreaCalcMinMax( PULID _Object, pusTextAreaClassData* _pData, PUU32 _Param1, PUU32 _Param2 )
{
	puSetAttribute( _Object, PUA_CONTROL_MINWIDTH, 50 );
	puSetAttribute( _Object, PUA_CONTROL_MINHEIGHT, 30 );
}

void puTextAreaPreLayout( PULID _Object, pusTextAreaClassData* _pData, PUU32 _Param1, PUU32 _Param2 )
{
	pusRect Rect;
	PUU32 Height;
	PUS32 NumLines;

//	puSetAttribute( _Object, PUA_CONTROL_DEFERRELAYOUT, TRUE );

	if( !_pData->bForceHeight )
		return;

	puSetFont( _Param1, _pData->Font );

	// Delete current wrapped buffer
//	puTextAreaDeleteWrappedBuffer( _pData );
//	_pData->pFirstDisplayLine = NULL;

	// Wrap buffer
	NumLines = puTextAreaWrapBuffer( _Object, _Param1, _pData, TRUE );

	if( NumLines < 0 )
	{
		puSetFont( _Param1, PUFONT_NORMAL );
		return;
	}

	// Calc scrolling height
	puTextSize( _Param1, " ", 0, &Rect );
	Height = Rect.Height * NumLines;

	// Set the wanted height
	puSetAttribute( _Object, PUA_CONTROL_MINHEIGHT, Height );
	puSetAttribute( _Object, PUA_CONTROL_MAXHEIGHT, Height );

	puSetFont( _Param1, PUFONT_NORMAL );
}

void puTextAreaLayout( PULID _Object, pusTextAreaClassData* _pData, PUU32 _Param1, PUU32 _Param2 )
{
	pusRect TxtRect;
	pusRect Rect;

	puSetFont( _Param1, _pData->Font );

	// Calc number of visible entries
	puTextSize( _Param1, " ", 0, &TxtRect );
	puDoMethod( _Object, PUM_SCROLLING_GETVIEWRECT, ( PUU32 )&Rect, 0 );
	
	_pData->NumVisibleLines = Rect.Height / TxtRect.Height;

	puTextAreaDeleteWrappedBuffer( _pData );
	_pData->pFirstDisplayLine = NULL;

	// Wrap buffer without the Vertical scroller
	puSetAttribute( _Object, PUA_SCROLLING_SHOWVSCROLLER, FALSE );
	puTextAreaWrapBuffer( _Object, _Param1, _pData, FALSE );

	if( !_pData->bForceHeight )
	{

		if( _pData->NumDisplayLines > _pData->NumVisibleLines )
		{
			// It doesn't fit, do it again with the scroller 
			puSetAttribute( _Object, PUA_SCROLLING_SHOWVSCROLLER, TRUE );

			puTextAreaDeleteWrappedBuffer( _pData );
			_pData->pFirstDisplayLine = NULL;

			// Wrap buffer
			puTextAreaWrapBuffer( _Object, _Param1, _pData, FALSE );
		}
	}

	puSetAttribute( _Object, PUA_SCROLLING_VERVIS, _pData->NumVisibleLines );
	puSetAttribute( _Object, PUA_SCROLLING_HEIGHT, _pData->NumDisplayLines );

	// TODO: keep proper vertical position after relayout
	// (must be recomputed)
//	if( !_pData->pFirstDisplayLine )
	{
		_pData->pFirstDisplayLine = ( pusTextAreaLine* )_pData->pDisplayLines->pFirst;

		if( !_pData->pFirstDisplayLine->Node.pNext )
			_pData->pFirstDisplayLine = NULL;
	}

	_pData->ScrollY++;
	puSetAttribute( _Object, PUA_SCROLLING_POSY, _pData->ScrollY - 1 );

	puSetFont( _Param1, PUFONT_NORMAL );
}

void puTextAreaRender( PULID _Object, pusTextAreaClassData* _pData, PUU32 _Param1, PUU32 _Param2 )
{
	pusRect Rect;
	pusRect TmpRect;
	pusNode* pNext;
	pusTextAreaLine* pLine;
	PUU32 YPos;
	PUU32 Count;
	PUU32 LineHeight;

	puSetFont( _Param1, _pData->Font );
	puTextSize( _Param1, " ", 0, &Rect );
	LineHeight = Rect.Height;
	puDoMethod( _Object, PUM_SCROLLING_GETVIEWRECT, ( PUU32 )&Rect, 0 );

	if( !_pData->pFirstDisplayLine )
	{
		if( !_pData->NumBufferLines )
			puDoMethod( _Object, PUM_CONTROL_CLEARBG, _Param1, ( PUU32 )&Rect );
		puSetFont( _Param1, PUFONT_NORMAL );
		return;
	}

	puSetClipping( _Param1, &Rect );
	puSetDrawPen( _Param1, 0, 0, 0 );

	TmpRect.X = Rect.X;
	TmpRect.Y = Rect.Y;
	TmpRect.Width = Rect.Width;
	TmpRect.Height = LineHeight;

	YPos = Rect.Y;

	Count = _pData->NumVisibleLines;
	pLine = _pData->pFirstDisplayLine;

	for( ; Count; Count-- )
	{
		if( !( pNext = pLine->Node.pNext ) )
			break;

		puDoMethod( _Object, PUM_CONTROL_CLEARBG, _Param1, ( PUU32 )&TmpRect );

		puTextAreaDrawLine( _pData, pLine, _Param1, &Rect, YPos );
		YPos += LineHeight;
		TmpRect.Y += LineHeight;

		pLine = ( pusTextAreaLine* )pNext;
	}

	TmpRect.Height = Rect.Height - ( TmpRect.Y - Rect.Y );
	puDoMethod( _Object, PUM_CONTROL_CLEARBG, _Param1, ( PUU32 )&TmpRect );

	puSetClipping( _Param1, NULL );
	puSetFont( _Param1, PUFONT_NORMAL );
}

void puTextAreaDrawLine( pusTextAreaClassData* _pData, pusTextAreaLine* _pLine, PUU32 _RastPort, pusRect* _pRect, PUU32 _YPos )
{
	pusRect Rect;

	if( !_pLine->pText )
		return;

	switch( _pData->CenterMode )
	{
		case PUTA_CENTERMODE_LEFT:
			puDrawText( _RastPort, _pLine->pText, _pLine->Length, _pRect->X /*- _pData->HorScroll*/, _YPos );
			break;

		case PUTA_CENTERMODE_CENTER:
			puTextSize( _RastPort, _pLine->pText, _pLine->Length, &Rect );
			puDrawText( _RastPort, _pLine->pText, _pLine->Length, _pRect->X + ( ( _pRect->Width - Rect.Width ) >> 1 ), _YPos );
			break;
	}
}



void puTextAreaKeyboardEvent( PULID _Object, pusTextAreaClassData* _pData, PUU32 _Param1, PUU32 _Param2 )
{
}

void puTextAreaMouseEvent( PULID _Object, pusTextAreaClassData* _pData, PUU32 _Param1, PUU32 _Param2 )
{

}

void puTextAreaDeleteBuffer( pusTextAreaClassData* _pData )
{
	pusNode* pCurrent;
	pusNode* pNext;
	pusTextAreaLine* pLine;

	pCurrent = _pData->UnwrappedLines.pFirst;

	while( pNext = pCurrent->pNext )
	{
		pLine = ( pusTextAreaLine* )pCurrent;
		
		if( pLine->pText )
			free( pLine->pText );
		free( pLine );

		pCurrent = pNext;
	}

	puInitList( &_pData->UnwrappedLines );
	_pData->NumBufferLines = 0;
}

void puTextAreaDeleteWrappedBuffer( pusTextAreaClassData* _pData )
{
	pusNode* pCurrent;
	pusNode* pNext;
	pusTextAreaLine* pLine;

	pCurrent = _pData->WrappedLines.pFirst;

	while( pNext = pCurrent->pNext )
	{
		pLine = ( pusTextAreaLine* )pCurrent;
		
		if( pLine->pText )
			free( pLine->pText );
		free( pLine );

		pCurrent = pNext;
	}

	puInitList( &_pData->WrappedLines );
	_pData->NumDisplayLines = 0;
}

PUU8 puTextAreaImportBuffer( pusTextAreaClassData* _pData, PUU8* _pBuffer )
{
	PUU8* pTmp;
	PUU8 c;
	PUU16 LineLength;
	pusTextAreaLine* pLine;

	while( 1 )
	{
		pTmp = _pBuffer;

		do
		{
			c = *pTmp++;
		} while( c && c != '\n' );

		LineLength = pTmp - _pBuffer;

		if( LineLength <= 1 && c != '\n' )
			return TRUE;

		if( !( pLine = malloc( sizeof( pusTextAreaLine ) ) ) )
			return FALSE;

		if( LineLength > 1 )
		{
			if( !( pLine->pText = malloc( LineLength ) ) )
			{
				free( pLine );
				return FALSE;
			}
		}
		else
			pLine->pText = NULL;

		LineLength--;
		memcpy( pLine->pText, _pBuffer, LineLength );

		if( pLine->pText )
			pLine->pText[ LineLength ] = 0;

		pLine->Length = LineLength;
		pLine->pSibling = NULL;
		pLine->RefCount = 0;

		puAddTail( &_pData->UnwrappedLines, &pLine->Node );

		_pData->NumBufferLines++;

		_pBuffer = pTmp;

		if( !*( pTmp - 1 ) )
			break;
	}

	return TRUE;
}

PUU8* puTextAreaExportBuffer( pusTextAreaClassData* _pData )
{
	pusNode* pCurrent;
	pusNode* pNext;
	pusTextAreaLine* pLine;
	PUU32 BufferSize = 1;
	PUU8* pBuffer;
	PUU8* pTmp;

	pCurrent = _pData->UnwrappedLines.pFirst;

	while( pNext = pCurrent->pNext )
	{
		pLine = ( pusTextAreaLine* )pCurrent;
		BufferSize += pLine->Length + 1;
		pCurrent = pNext;
	}

	if( !( pBuffer = malloc( BufferSize ) ) )
		return NULL;

	pTmp = pBuffer - 1;
	pCurrent = _pData->UnwrappedLines.pFirst;

	while( pNext = pCurrent->pNext )
	{
		pTmp++;
		pLine = ( pusTextAreaLine* )pCurrent;
		if( pLine->pText )
			memcpy( pTmp, pLine->pText, pLine->Length );
		pTmp += pLine->Length;
		*pTmp = '\n';
		pCurrent = pNext;
	}

	*pTmp = 0;

	return pBuffer;
}

PUS32 puTextAreaWrapBuffer( PULID _Object, PUU32 _Rastport, pusTextAreaClassData* _pData, PUU8 _bDryRun )
{
	pusNode* pCurrent;
	pusNode* pNext;
	pusTextAreaLine* pLine;
	pusTextAreaLine* pWrappedLine;
	PUU8* pTmp;
	PUU8* pTmp2;
	PUU32 Length;
	PUS32 NumLines = 0;

	pCurrent = _pData->UnwrappedLines.pFirst;

	while( pNext = pCurrent->pNext )
	{
		pLine = ( pusTextAreaLine* )pCurrent;

		pTmp = pLine->pText;
		while( 1 )
		{
			if( pLine->pText )
				if( !*pTmp )
					break;

			if( pLine->pText )
				pTmp2 = puTextAreaFitLine( _Object, _Rastport, pTmp, &Length );

			if( !_bDryRun )
			{
				pWrappedLine = malloc( sizeof( pusTextAreaLine ) );

				if( !pWrappedLine )
					return -1;

				if( pLine->pText )
				{
					pWrappedLine->pText = malloc( Length + 1 );
					memcpy( pWrappedLine->pText, pTmp, Length );
					pWrappedLine->pText[ Length ] = 0;
				}
				else
					pWrappedLine->pText = NULL;

				pWrappedLine->Length = ( PUU16 )Length;
				pWrappedLine->pSibling = pLine;

				if( !pLine->pSibling )
					pLine->pSibling = pWrappedLine;

				pLine->RefCount++;

				puAddTail( &_pData->WrappedLines, &pWrappedLine->Node );
			}

			NumLines++;

			if( !pLine->pText )
				break;

			pTmp = pTmp2;
		}

		pCurrent = pNext;
	}

	if( !_bDryRun )
		_pData->NumDisplayLines = NumLines;

	return NumLines;
}

PUU8 puTextAreaRewrapLine( pusTextAreaClassData* _pData, pusTextAreaLine* _pLine )
{
	// Rewrapper la ligne en parcourant les wrapped lines associées en séquence et en
	// les mettant à jour si nécéssaire. En ajouter à la suite ou en enlever s'il
	// y en a en trop.

	return TRUE;
}

PUU8* puTextAreaFitLine( PULID _Object, PUU32 _Rastport, PUU8* _pLineText, PUU32* _pLength )
{
	pusRect Rect;
	PUU32 NumChars;
	PUU8* pTmp;
	PUU8* pRes;

	puDoMethod( _Object, PUM_SCROLLING_GETVIEWRECT, ( PUU32 )&Rect, 0 );

	NumChars = puTextFit( _Rastport, _pLineText, 0, Rect.Width );

	if( NumChars == strlen( _pLineText ) )
	{
		if( _pLength )
			*_pLength = NumChars;

		return _pLineText + NumChars;
	}

	// Search for last space before the cutting point
	pTmp = _pLineText + NumChars;

	while( pTmp > _pLineText )
	{
		if( *--pTmp == ' ' )
			break;
	}

	if( *pTmp != ' ' )
	{
		// Word longer than a line
		if( _pLength )
			*_pLength = NumChars;

		return _pLineText + NumChars;
	}

	pRes = pTmp + 1;

	while( pTmp > _pLineText )
	{
		if( *--pTmp != ' ' )
			break;
	}

	if( *pTmp == ' ' )
	{
		// Line starting with one or more space
		if( _pLength )
			*_pLength = NumChars;

		return _pLineText + NumChars;
	}

	pTmp++;

	if( _pLength )
		*_pLength = pTmp - _pLineText;

	return pRes;
}

void puTextAreaDeleteWrapLine( pusTextAreaClassData* _pData, pusTextAreaLine* _pWrapLine )
{
	// Deleter la wrap line. Gérer le compteur de référence de la ligne non wrappée associée.
	// Deleter la ligne non wrappée si nécessaire.
}

// TODO: fonctions pour manipuler les lignes non wrappées (split, merge, insert, delete)

// TODO: fonctions pour gérer la sélection (collapse, delete, etc.)

// TODO: fonctions pour gérer le curseur

// Position functions
void pTextAreaPixelToPos( PUU32 _RastPort, pusTextAreaPos* _pPos )
{
	_pPos->CharPos = ( PUS16 )puTextFit( _RastPort, _pPos->pLine->pText, _pPos->pLine->Length, _pPos->PixelPos );
}

void puTextAreaPosToPixel( PUU32 _RastPort, pusTextAreaPos* _pPos )
{
	pusRect Rect;
	puTextSize( _RastPort, _pPos->pLine->pText, _pPos->pLine->Length, &Rect );
	_pPos->PixelPos = ( PUU16 )Rect.Width;
}
//void	puTextSize( PUU32 _RastPort, PUU8* _pText, PUU32 _Length, pusRect* _pRect );

void puTextAreaUnwrapPos( pusTextAreaPos* _pPos )
{
	_pPos->CharPos += _pPos->pLine->pText - _pPos->pLine->pSibling->pText;
	_pPos->pLine = _pPos->pLine->pSibling;
}

void puTextAreaWrapPos( PUU32 _RastPort, pusTextAreaPos* _pPos )
{
	PUU8* pTmp;
	pusNode* pCurrent;
	pusNode* pNext;
	pusTextAreaLine* pLine;


	pCurrent = &_pPos->pLine->pSibling->Node;
	pTmp = _pPos->pLine->pText + _pPos->CharPos;

	while( pNext = pCurrent->pNext )
	{
		pLine = ( pusTextAreaLine* )pCurrent;

		if( pTmp > pLine->pText )
			break;

		pCurrent = pNext;
	}

	_pPos->CharPos -= pLine->pText - _pPos->pLine->pText;
	_pPos->pLine = pLine;

	puTextAreaPosToPixel( _RastPort, _pPos );
}

/*typedef struct
{
	pusTextAreaLine* pLine;
	PUU16 CharPos;
	PUU16 PixelPos;
} pusTextAreaPos;*/
