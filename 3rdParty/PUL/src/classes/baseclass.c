/* Poly
   Useful
   Lib */

#include <pul/pul.h>
#include <stdlib.h>
#include <string.h>
#include "baseclass.h"

PUU32* puBaseClassAddNotify( pusBaseClassData* _pData, PUU32* _pDef, pusObjectCollection* _pCol );
void puBaseClassCheckNotification( pusNotification* _pNotification, PUU32 _AttrID, PUU32 _Val );
void puBaseClassCheckNotifications( pusBaseClassData* _pData, PUU32 _AttrID, PUU32 _Val );
PUU32* puBaseClassParseObjRef( PUU32* _pDef, pusObjectCollection* _pCol, PULID* _pPULID );


PUU32 puBaseClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 )
{
	pusBaseClassData* pData;
	pData = ( pusBaseClassData* )_pData;

	switch( _MethodID )
	{
		case PUM_NEW:
			pData->RefCount = 0;
			pData->ParentObject = NULL;
			pData->LastNotification = 0;
			puInitList( &pData->ChildList );
			puInitList( &pData->NotificationList );
			return TRUE;

		case PUM_DELETE:
			{
				pusNode* pCurrent;
				pusNode* pNext;

				// Unreference all child objects
				pCurrent = pData->ChildList.pFirst;

				while( pNext = pCurrent->pNext )
				{
					puDoMethod( ( PULID )pCurrent, PUM_REMREF, 0, 0 );
					pCurrent = pNext;
				}

				// Free notifications
				pCurrent = pData->NotificationList.pFirst;

				while( pNext = pCurrent->pNext )
				{
					free( pCurrent );
					pCurrent = pNext;
				}
			}
			break;

		case PUM_ADDREF:
			pData->RefCount++;
			break;

		case PUM_REMREF:
			if( pData->RefCount )
				pData->RefCount--;

			if( !pData->RefCount )
				puDeleteObject( _Object );
			break;

		case PUM_SET:
			switch( _Param1 )
			{
				case PUA_PARENT:
					pData->ParentObject = _Param2;
					break;

				default:
					puBaseClassCheckNotifications( pData, _Param1, _Param2 );
					break;
			}
			return TRUE;

		case PUM_GET:
			switch( _Param1 )
			{
				case PUA_PARENT:
					return pData->ParentObject;

				case PUA_NOTIFICATION:
					return pData->LastNotification;
			}

			return 0;

		case PUM_ADDCHILD:
			puAddTail( &pData->ChildList, ( pusNode* )_Param1 );
			puSetAttribute( _Param1, PUA_PARENT, _Object );
			puDoMethod( _Param1, PUM_ADDREF, 0, 0 );
			break;

		case PUM_GETCHILDLIST:
			return ( PUU32 )&pData->ChildList;

		case PUM_ADDNOTIFICATION:
			return ( PUU32 )puBaseClassAddNotify( pData, ( PUU32* )_Param1, ( pusObjectCollection* )_Param2 );

		case PUM_REMNOTIFICATION:
			{
				pusNode* pNode;
				pNode = ( pusNode* )_Param1;
				puRemoveNode( pNode );
				free( pNode );
			}
			return TRUE;
	}

	return 0;
}

/*
	Notification definition:
		<condition>
		<action>
		PUNOTIFY_END

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
*/

PUU32* puBaseClassAddNotify( pusBaseClassData* _pData, PUU32* _pDef, pusObjectCollection* _pCol )
{
	pusNotification* pNotification;
	PUU32 Temp[ 16 ], Type;
	PUU32* pDest = Temp;

	/* Parse the notification, and expand object references */
	
	/* Parse condition */
	Type = *_pDef++;
	*pDest++ = Type;

	switch( Type )
	{
		case PUNOTIFY_CONDITION_ATTRCHANGE:
			// Copy attr id
			*pDest++ = *_pDef++;
			break;

		case PUNOTIFY_CONDITION_ATTREQUALS:
			// Copy attr id
			*pDest++ = *_pDef++;

			// Copy value
			*pDest++ = *_pDef++;
			break;
	}

	/* Parse action */
	Type = *_pDef++;
	*pDest++ = Type;

	switch( Type )
	{
		case PUNOTIFY_ACTION_CALLBACK:
		case PUNOTIFY_ACTION_APPMSG:
			// Copy callback address
			*pDest++ = *_pDef++;
			break;

		case PUNOTIFY_ACTION_SET:
			// Copy obj ref, attr id and value
			*pDest++ = *_pDef++;
			*pDest++ = *_pDef++;
			*pDest++ = *_pDef++;
			break;

		case PUNOTIFY_ACTION_SETVAL:
		case PUNOTIFY_ACTION_SETNOTVAL:
		case PUNOTIFY_ACTION_TOGGLE:
		case PUNOTIFY_ACTION_DOMETHOD:
			// Copy obj, and attr/mtd id
			*pDest++ = *_pDef++;
			*pDest++ = *_pDef++;
			break;

		default:
			return FALSE;
	}

	// Alloc notification info
	if( pNotification = malloc( ( pDest - Temp ) * 4 + sizeof( pusNotification ) ) )
	{
		pNotification->pCol = _pCol;
		pNotification->bActive = FALSE;
		memcpy( pNotification + 1, Temp, ( pDest - Temp ) * 4 );

		puAddTail( &_pData->NotificationList, &pNotification->Node );

		_pData->LastNotification = ( PULID )pNotification;
	}

	return _pDef;
}

PUU32* puBaseClassParseObjRef( PUU32* _pDef, pusObjectCollection* _pCol, PULID* _pPULID )
{
	PUU32 ObjRef;

	ObjRef = *_pDef++;

	if( _pCol )
		*_pPULID = puGetObjectFromCollection( _pCol, ObjRef );
	else
		*_pPULID = ObjRef;

	return _pDef;
}

void puBaseClassCheckNotifications( pusBaseClassData* _pData, PUU32 _AttrID, PUU32 _Val )
{
	pusNode* pCurrent;
	pusNode* pNext;

	pCurrent = _pData->NotificationList.pFirst;

	while( pNext = pCurrent->pNext )
	{
		puBaseClassCheckNotification( ( pusNotification* )pCurrent, _AttrID, _Val );
		pCurrent = pNext;
	}
}

void puBaseClassCheckNotification( pusNotification* _pNotification, PUU32 _AttrID, PUU32 _Val )
{
	PUU32* pDef, Type, AttrID, MtdID, Val;
	PULID Obj;

	// If the notification is already active, don't do anything
	// (circular notifications)
	if( _pNotification->bActive )
		return;

	pDef = ( PUU32* )( _pNotification + 1 );

	Type = *pDef++;
	switch( Type )
	{
		case PUNOTIFY_CONDITION_ATTRCHANGE:
			AttrID = *pDef++;

			if( AttrID != _AttrID )
				return;
			break;

		case PUNOTIFY_CONDITION_ATTREQUALS:
			AttrID = *pDef++;

			if( AttrID != _AttrID )
				return;

			Val = *pDef++;

			if( Val != _Val )
				return;
			break;

		default:
			return;
	}

	// Flag the notification as active
	_pNotification->bActive = TRUE;

	Type = *pDef++;
	switch( Type )
	{
		case PUNOTIFY_ACTION_CALLBACK:
			{
				PUNOTCALLBACK pCallBack;

				pCallBack = ( PUNOTCALLBACK )*pDef++;
				pCallBack( _AttrID, _Val );
			}
			break;

		case PUNOTIFY_ACTION_SET:
			// Get object
			pDef = puBaseClassParseObjRef( pDef, _pNotification->pCol, &Obj );

			if( !Obj )
				break;

			AttrID = *pDef++;
			Val = *pDef++;

			puSetAttribute( Obj, AttrID, Val );
			break;

		case PUNOTIFY_ACTION_SETVAL:
			// Get object
			pDef = puBaseClassParseObjRef( pDef, _pNotification->pCol, &Obj );

			if( !Obj )
				break;

			AttrID = *pDef++;

			puSetAttribute( Obj, AttrID, _Val );
			break;

		case PUNOTIFY_ACTION_SETNOTVAL:
			// Get object
			pDef = puBaseClassParseObjRef( pDef, _pNotification->pCol, &Obj );

			if( !Obj )
				break;

			AttrID = *pDef++;

			puSetAttribute( Obj, AttrID, !_Val );
			break;

		case PUNOTIFY_ACTION_TOGGLE:
			// Get object
			pDef = puBaseClassParseObjRef( pDef, _pNotification->pCol, &Obj );

			if( !Obj )
				break;

			AttrID = *pDef++;

			Val = puGetAttribute( Obj, AttrID );
			puSetAttribute( Obj, AttrID, !Val );
			break;

		case PUNOTIFY_ACTION_DOMETHOD:
			// Get object
			pDef = puBaseClassParseObjRef( pDef, _pNotification->pCol, &Obj );

			if( !Obj )
				break;

			MtdID = *pDef++;

			puDoMethod( Obj, MtdID, 0, 0 );
			break;

		case PUNOTIFY_ACTION_APPMSG:
			puPostAppMessage( *pDef++, _Val );
			break;
	}

	// Unflag the notification now that we're done with it
	_pNotification->bActive = FALSE;
}