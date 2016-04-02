/* Poly
   Useful
   Lib */

#include <pul/pul.h>
#include "imageclass.h"
#include "../include/pul.h"

PUU32 puImageGetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr );
PUU32 puImageSetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr, PUU32 _Val );

PUU32 puImageClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 )
{
	pusImageClassData* pData;
	pData = ( pusImageClassData* )_pData;

	switch( _MethodID )
	{
		case PUM_NEW:
			pData->Width = 16;
			pData->Height = 16;
			pData->Image = 0;
			pData->PixFormat = 16;
			pData->BGRed = pData->BGGreen = pData->BGBlue = 100;
			pData->pData = NULL;
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		case PUM_DELETE:
			if( pData->Image )
				puFreeBitmap( pData->Image );
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		case PUM_SET:
			puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
			return puImageSetAttr( _Object, _Class, _pData, _Param1, _Param2 );

		case PUM_GET:
			return puImageGetAttr( _Object, _Class,_pData, _Param1 );

		/* Control methods */
		case PUM_CONTROL_CALCMINMAX:
			{
				puSetAttribute( _Object, PUA_CONTROL_MINWIDTH, pData->Width );
				puSetAttribute( _Object, PUA_CONTROL_MAXWIDTH, pData->Width );
				puSetAttribute( _Object, PUA_CONTROL_MINHEIGHT, pData->Height );
				puSetAttribute( _Object, PUA_CONTROL_MAXHEIGHT, pData->Height );

				puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 ); 
			}
			break;

		case PUM_CONTROL_LAYOUT:
			puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 ); 
			break;

		case PUM_CONTROL_RENDER:
	//		puDoMethod( _Object, PUM_CONTROL_CLEARBG, _Param1, 0 );

			{
				pusRect Rect;
				puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
				puDoMethod( _Object, PUM_CONTROL_GETINNERRECT, ( PUU32 )&Rect, 0 );
			
				if( !pData->Image && pData->pData )
					pData->Image = puAllocBitmap( _Param1, pData->Width, pData->Height, pData->PixFormat, pData->pData );

				if( !pData->Image )
				{
					puSetFillPen( _Param1, pData->BGRed, pData->BGGreen, pData->BGBlue );
					puRect( _Param1, &Rect );
				}
				else
					puDrawBitmap( _Param1, pData->Image, &Rect );

			//	if( pData->Image )
			//		puDrawBitmap( _Param1, pData->Image, &Rect );
			}
			break;

		/* Image methods */
	/*	case PUM_IMAGE_SETDATA:
			pData->
			if( pData->Image )
				puFreeBitmap( pData->Image );

			WindowObj = puGetAttribute( _Object, PUA_CONTROL_WINDOW );

			if( RastPort = puDoMethod( WindowObj, PUM_WINDOW_GETRASTPORT, 0, 0 ) )
			{
				pData->Image = puAllocBitmap( RastPort, pData->Width, pData->Height, pData->PixFormat, ( void* )_Param1 );
				puDoMethod( WindowObj, PUM_WINDOW_RELEASERASTPORT, 0, 0 );
			}
			break;*/

		default:
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
	}

	return 0;
}


PUU32 puImageGetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr )
{
	pusImageClassData* pData;
	pData = ( pusImageClassData* )_pData;

	switch( _Attr )
	{
	/*	case PUA_CONTROL_STATE:
			return pData->State;*/

		default:
			return puDoSuperMethod( _Object, _Class, PUM_GET, _Attr, 0 );
	}
}


PUU32 puImageSetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr, PUU32 _Val )
{
	pusImageClassData* pData;
	pData = ( pusImageClassData* )_pData;

	switch( _Attr )
	{
		case PUA_IMAGE_WIDTH:
			pData->Width = _Val;
			break;

		case PUA_IMAGE_HEIGHT:
			pData->Height = _Val;
			break;

		case PUA_IMAGE_PIXFORMAT:
			if( _Val == 16 || _Val == 24 || _Val == 32 )
				pData->PixFormat = _Val;
			break;

		case PUA_IMAGE_DATA:
			if( pData->Image )
			{
				puFreeBitmap( pData->Image );
				pData->Image = 0;
			}

			pData->pData = ( void* )_Val;

			puDoMethod( _Object, PUM_CONTROL_REDRAWINNER, 0, 0 );
			break;
	}

	return TRUE;
}