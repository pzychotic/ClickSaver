/* Poly
   Useful
   Lib */

#include <pul/pul.h>
#include "groupclass.h"
#include "../include/pul.h"

PUU32 puGroupGetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr );
PUU32 puGroupSetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr, PUU32 _Val );


PUU32 puGroupClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 )
{
	pusGroupClassData* pData;
	pusList* pChildList;
	pusNode* pCurrentNode;
	pusNode* pNextNode;
	pusRect Rect;
	pusMouseEvent* pMouseEvent;
	pusKeyboardEvent* pKeyboardEvent;

	pData = ( pusGroupClassData* )_pData;

	switch( _MethodID )
	{
		/* Control methods */
		case PUM_CONTROL_RENDER:
			puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

			pChildList = ( pusList* )puDoMethod( _Object, PUM_GETCHILDLIST, 0, 0 );

			pCurrentNode = pChildList->pFirst;

			while( pNextNode = pCurrentNode->pNext )
			{
				if( !puGetAttribute( ( PULID )pCurrentNode, PUA_CONTROL_HIDDEN ) )
					puDoMethod( ( PULID )pCurrentNode, PUM_CONTROL_RENDER, _Param1, _Param2 );
				pCurrentNode = pNextNode;
			}
			break;

		case PUM_CONTROL_MOUSEEVENT:
			pMouseEvent = ( pusMouseEvent* )_Param1;
			pChildList = ( pusList* )puDoMethod( _Object, PUM_GETCHILDLIST, 0, 0 );

			pCurrentNode = pChildList->pFirst;

			while( pNextNode = pCurrentNode->pNext )
			{
				if( !puGetAttribute( ( PULID )pCurrentNode, PUA_CONTROL_HIDDEN ) )
				{
					Rect.X = puGetAttribute( ( PULID )pCurrentNode, PUA_CONTROL_POSX );
					Rect.Y = puGetAttribute( ( PULID )pCurrentNode, PUA_CONTROL_POSY );
					Rect.Width = puGetAttribute( ( PULID )pCurrentNode, PUA_CONTROL_WIDTH );
					Rect.Height = puGetAttribute( ( PULID )pCurrentNode, PUA_CONTROL_HEIGHT );
	 
					if( puCheckPointInRect( &Rect, pMouseEvent->MouseX, pMouseEvent->MouseY ) )
						puDoMethod( ( PULID )pCurrentNode, PUM_CONTROL_MOUSEEVENT, _Param1, _Param2 );
				}

				pCurrentNode = pNextNode;
			}
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		case PUM_CONTROL_KEYBOARDEVENT:
			pKeyboardEvent = ( pusKeyboardEvent* )_Param1;
			pChildList = ( pusList* )puDoMethod( _Object, PUM_GETCHILDLIST, 0, 0 );

			pCurrentNode = pChildList->pFirst;

			while( pNextNode = pCurrentNode->pNext )
			{
				if( !puGetAttribute( ( PULID )pCurrentNode, PUA_CONTROL_HIDDEN ) )
				{
					if( puGetAttribute( ( PULID )pCurrentNode, PUA_CONTROL_HASKEYBOARDFOCUS ) )
						puDoMethod( ( PULID )pCurrentNode, PUM_CONTROL_KEYBOARDEVENT, _Param1, _Param2 );
					else
					{
						Rect.X = puGetAttribute( ( PULID )pCurrentNode, PUA_CONTROL_POSX );
						Rect.Y = puGetAttribute( ( PULID )pCurrentNode, PUA_CONTROL_POSY );
						Rect.Width = puGetAttribute( ( PULID )pCurrentNode, PUA_CONTROL_WIDTH );
						Rect.Height = puGetAttribute( ( PULID )pCurrentNode, PUA_CONTROL_HEIGHT );
	 
						if( puCheckPointInRect( &Rect, pKeyboardEvent->MouseX, pKeyboardEvent->MouseY ) )	
							puDoMethod( ( PULID )pCurrentNode, PUM_CONTROL_KEYBOARDEVENT, _Param1, _Param2 );
					}
				}

				pCurrentNode = pNextNode;
			}
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		default:
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
	}

	return 0;
}