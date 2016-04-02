/* Poly
   Useful
   Lib */

#include <pul/pul.h>
#include "textclass.h"
#include "../include/pul.h"

PUU32 puTextGetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr );
PUU32 puTextSetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr, PUU32 _Val );

PUU32 puTextClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 )
{
	pusTextClassData* pData;
	pData = ( pusTextClassData* )_pData;

	switch( _MethodID )
	{
		case PUM_NEW:
			pData->pString = NULL;
			pData->TextX = 0;
			pData->TextY = 0;
			pData->MinWidth = 0;
			pData->CenterMode = PU_TEXT_CENTER;
			pData->bFixedWidth = FALSE;
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		case PUM_SET:
			puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
			return puTextSetAttr( _Object, _Class, _pData, _Param1, _Param2 );

		case PUM_GET:
			return puTextGetAttr( _Object, _Class,_pData, _Param1 );

		/* Control methods */
		case PUM_CONTROL_CALCMINMAX:
			{
				pusRect Rect;
				
				Rect.Width = Rect.Height = 0;

				if( pData->pString )
					puTextSize( _Param1, pData->pString, 0, &Rect );

				/* Min width */
				if( pData->MinWidth )
				{
					puSetAttribute( _Object, PUA_CONTROL_MINWIDTH, pData->MinWidth );
					Rect.Width = pData->MinWidth;
				}
				else
					puSetAttribute( _Object, PUA_CONTROL_MINWIDTH, Rect.Width );

				/* No max width */
				if( pData->bFixedWidth )
					puSetAttribute( _Object, PUA_CONTROL_MAXWIDTH, Rect.Width );
				else
					puSetAttribute( _Object, PUA_CONTROL_MAXWIDTH, 0 );

				/* Fixed height */
				puSetAttribute( _Object, PUA_CONTROL_MINHEIGHT, Rect.Height );
				puSetAttribute( _Object, PUA_CONTROL_MAXHEIGHT, Rect.Height );

				puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 ); 
			}
			break;

		case PUM_CONTROL_LAYOUT:
			if( pData->pString )
			{
				pusRect Rect1, Rect2;
				puDoMethod( _Object, PUM_CONTROL_GETINNERRECT, ( PUU32 )&Rect1, 0 );

				puTextSize( _Param1, pData->pString, 0, &Rect2 );

				switch( pData->CenterMode )
				{
					case PU_TEXT_CENTER:
						pData->TextX = Rect1.X + ( ( Rect1.Width - Rect2.Width ) >> 1 );
						break;

					case PU_TEXT_RIGHT:
						pData->TextX = Rect1.X + Rect1.Width - Rect2.Width;
						break;

					case PU_TEXT_LEFT:
					default:
						pData->TextX = Rect1.X;
						break;
				}

				pData->TextY = Rect1.Y + ( ( Rect1.Height - Rect2.Height ) >> 1 );
			}

			puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 ); 
			break;

		case PUM_CONTROL_RENDER:
			puDoMethod( _Object, PUM_CONTROL_CLEARBG, _Param1, 0 );
			puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

			if( pData->pString )
			{
				if( pData->MinWidth )
				{
					pusRect Rect;
					puDoMethod( _Object, PUM_CONTROL_GETINNERRECT, ( PUU32 )&Rect, 0 );
					puSetClipping( _Param1, &Rect );
				}

				puSetDrawPen( _Param1, 0, 0, 0 );
				puDrawText( _Param1, pData->pString, 0, pData->TextX, pData->TextY );

				if( pData->MinWidth )
					puSetClipping( _Param1, NULL );
			}
			break;

		case PUM_CONTROL_PRELAYOUT:
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );


		default:
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
	}

	return 0;
}


PUU32 puTextGetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr )
{
	pusTextClassData* pData;
	pData = ( pusTextClassData* )_pData;

	switch( _Attr )
	{
	/*	case PUA_CONTROL_STATE:
			return pData->State;*/

		default:
			return puDoSuperMethod( _Object, _Class, PUM_GET, _Attr, 0 );
	}
}


PUU32 puTextSetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr, PUU32 _Val )
{
	pusTextClassData* pData;
	pData = ( pusTextClassData* )_pData;

	switch( _Attr )
	{
		case PUA_TEXT_STRING:
			pData->pString = ( PUU8* )_Val;
			puDoMethod( _Object, PUM_CONTROL_RELAYOUT, 0, 0 );
			puDoMethod( _Object, PUM_CONTROL_REDRAWINNER, 0, 0 );
			break;

		case PUA_TEXT_CENTERMODE:
			pData->CenterMode = _Val;
			break;

		case PUA_TEXT_MINWIDTH:
			pData->MinWidth = _Val;
			break;

		case PUA_TEXT_FIXEDWIDTH:
			pData->bFixedWidth = _Val;
			break;
	}

	return TRUE;
}