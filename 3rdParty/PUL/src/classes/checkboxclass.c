/* Poly
   Useful
   Lib */

#include <pul/pul.h>
#include "checkboxclass.h"
#include "../include/pul.h"

PUU32 puCheckBoxGetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr );
PUU32 puCheckBoxSetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr, PUU32 _Val );


PUU32 puCheckBoxClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 )
{
	pusCheckBoxClassData* pData;
	pData = ( pusCheckBoxClassData* )_pData;

	switch( _MethodID )
	{
		case PUM_NEW:
			pData->pLabel = NULL;
			pData->bChecked = FALSE;
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		case PUM_SET:
			puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
			return puCheckBoxSetAttr( _Object, _Class, _pData, _Param1, _Param2 );

		case PUM_GET:
			return puCheckBoxGetAttr( _Object, _Class,_pData, _Param1 );

		/* Control methods */
		case PUM_CONTROL_CALCMINMAX:
			{
				pusRect Rect, Rect2;
				PUU32 Width, Height;
				
				Rect.Width = Rect.Height = 0;

				if( pData->pLabel )
					puTextSize( _Param1, pData->pLabel, 0, &Rect );

				puGetIconSize( PUICON_CHECKBOX, PUSTATE_NORMAL, &Rect2 );

				/* Fixed width */
				Width = Rect.Width + Rect2.Width + 2;
				puSetAttribute( _Object, PUA_CONTROL_MINWIDTH, Width );
				puSetAttribute( _Object, PUA_CONTROL_MAXWIDTH, Width );

				/* Fixed height */
				if( Rect.Height > Rect2.Height )
					Height = Rect.Height;
				else
					Height = Rect2.Height;
				puSetAttribute( _Object, PUA_CONTROL_MINHEIGHT, Height );
				puSetAttribute( _Object, PUA_CONTROL_MAXHEIGHT, Height );

				puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 ); 
			}
			break;

		case PUM_CONTROL_RENDER:
			puDoMethod( _Object, PUM_CONTROL_CLEARBG, _Param1, 0 );

			{
				pusRect Rect1, Rect2, Rect3;

				puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
				puDoMethod( _Object, PUM_CONTROL_GETINNERRECT, ( PUU32 )&Rect1, 0 );

				puGetIconSize( PUICON_CHECKBOX, PUSTATE_NORMAL, &Rect2 );
				puDrawIcon( _Param1, PUICON_CHECKBOX, pData->bChecked ? PUSTATE_ACTIVE : PUSTATE_NORMAL, Rect1.X, Rect1.Y );

				if( pData->pLabel )
				{
					puSetDrawPen( _Param1, 0, 0, 0 );
					puTextSize( _Param1, pData->pLabel, 0, &Rect3 );
					puDrawText( _Param1, pData->pLabel, 0,
						Rect1.X + Rect2.Width + 2,
						Rect1.Y + ( ( Rect1.Height - Rect3.Height ) >> 1 ) );
				}

			}
			break;

		case PUM_CONTROL_MOUSEEVENT:
			{
				pusMouseEvent* pMouseEvent;
				pMouseEvent = ( pusMouseEvent* )_Param1;

				if( ( pMouseEvent->ButtonCode & ~PU_KEYDOUBLECLICK ) == PUMOUSE_LMB )
					puSetAttribute( _Object, PUA_CHECKBOX_CHECKED, pData->bChecked ^ 1 );
			}
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		default:
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
	}

	return 0;
}


PUU32 puCheckBoxGetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr )
{
	pusCheckBoxClassData* pData;
	pData = ( pusCheckBoxClassData* )_pData;

	switch( _Attr )
	{
		case PUA_CHECKBOX_CHECKED:
			return pData->bChecked;

		default:
			return puDoSuperMethod( _Object, _Class, PUM_GET, _Attr, 0 );
	}
}


PUU32 puCheckBoxSetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr, PUU32 _Val )
{
	pusCheckBoxClassData* pData;
	pData = ( pusCheckBoxClassData* )_pData;

	switch( _Attr )
	{
		case PUA_CHECKBOX_CHECKED:
			pData->bChecked = _Val;

			puDoMethod( _Object, PUM_CONTROL_REDRAWINNER, 0, 0 );
			break;

		case PUA_CHECKBOX_LABEL:
			pData->pLabel = ( PUU8* )_Val;
			break;
	}

	return TRUE;
}