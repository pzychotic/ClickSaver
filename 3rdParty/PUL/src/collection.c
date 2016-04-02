/* Poly
   Useful
   Lib */

#include <stdlib.h>
#include "include/pul.h"
#include "include/collection.h"

PUU32* puActionSetAttributes( pusObjectCollection* _pCol, PULID _Object, PUU32* _pDef );
PUU32* puCallMethods( pusObjectCollection* _pCol, PULID _Object, PUU32* _pDef );
PUU32* puParseObjDef( pusObjectCollection* _pCol, PUU32* _pDef, PUU32* _pResult );

void puDeleteObjectCollection( pusObjectCollection* _pCollection )
{
	pusNode* pCurrentNode;
	pusNode* pNextNode;
	pusCollectionReference* pColRef;

	if( !_pCollection )
		return;

	pCurrentNode = _pCollection->ObjectList.pFirst;

	while( pNextNode = pCurrentNode->pNext )
	{
		pColRef = ( pusCollectionReference* )pCurrentNode;
		puDoMethod( pColRef->Object, PUM_REMREF, 0, 0 );

		free( pColRef );

		pCurrentNode = pNextNode;
	}

	free( _pCollection );
}

PULID puGetObjectFromCollection( pusObjectCollection* _pCollection, PUU32 _ObjId )
{
	pusNode* pCurrentNode;
	pusNode* pNextNode;
	pusCollectionReference* pColRef;

	pCurrentNode = _pCollection->ObjectList.pFirst;

	while( pNextNode = pCurrentNode->pNext )
	{
		pColRef = ( pusCollectionReference* )pCurrentNode;
		if( pColRef->ObjId == _ObjId )
			return pColRef->Object;

		pCurrentNode = pNextNode;
	}

	return 0;
}

pusObjectCollection* puCreateObjectCollection( PUU32* _pCollectionDef )
{
	pusObjectCollection* pCol;

	if( !( pCol = malloc( sizeof( pusObjectCollection ) ) ) )
		return NULL;

	puInitList( &pCol->ObjectList );

	if( !( puAppendObjectCollection( pCol, _pCollectionDef ) ) )
	{
		puDeleteObjectCollection( pCol );
		return NULL;
	}

	return pCol;
}

int puAppendObjectCollection( pusObjectCollection* _pCol, PUU32* _pCollectionDef )
{
	PUU32* pDef = _pCollectionDef, Action, ObjId;
	PULID Object;

	/* Parse actions */
	while( ( Action = *pDef++ ) != PU_ACTION_END )
	{
		switch( Action )
		{
			case PU_ACTION_OBJDEF:
				if( !( pDef = puParseObjDef( _pCol, pDef, NULL ) ) )
					return FALSE;
				break;

			case PU_ACTION_SETATTRIBUTES:
				ObjId = *pDef++;

				if( !( Object = puGetObjectFromCollection( _pCol, ObjId ) ) )
					return FALSE;

				pDef = puActionSetAttributes( _pCol, Object, pDef );
				break;

			case PU_ACTION_DOMETHODS:
				ObjId = *pDef++;

				if( !( Object = puGetObjectFromCollection( _pCol, ObjId ) ) )
					return FALSE;

				pDef = puCallMethods( _pCol, Object, pDef );
				break;
		}
	}

	return TRUE;
}

PUU32* puParseObjRef( pusObjectCollection* _pCol, PUU32* _pDef, PUU32* _pResult )
{
	PUU32 RefType, ObjId;

	RefType = *_pDef++;

	switch( RefType )
	{
		case PU_ACTION_OBJDEF:
			return puParseObjDef( _pCol, _pDef, _pResult );

		case PU_OBJID:
			ObjId = *_pDef++;
			if( !( *_pResult = puGetObjectFromCollection( _pCol, ObjId ) ) )
				return NULL;
			break;
	}

	return _pDef;
}

PUU32* puActionSetAttributes( pusObjectCollection* _pCol, PULID _Object, PUU32* _pDef )
{
	PUU32 Attribute;
	PUU32 Value;

	while( Attribute = *_pDef++ )
	{
		// Check expected data type
		switch( Attribute & PU_DATATYPE_MASK )
		{
			case PU_DATATYPE_OBJREF:
				if( !( _pDef = puParseObjRef( _pCol, _pDef, &Value ) ) )
					return NULL;

				if( _Object && Value )
					puSetAttribute( _Object, Attribute, Value );
				break;

			case PU_DATATYPE_DESCSTREAM:
				_pDef = ( PUU32* )puSetAttribute( _Object, Attribute, ( PUU32 )_pDef );
				break;

			case PU_DATATYPE_STRING:
			default:
				Value = *_pDef++;
				if( _Object )
					puSetAttribute( _Object, Attribute, Value );
				break;
		}
	}

	return _pDef;
}

PUU32* puCallMethods( pusObjectCollection* _pCol, PULID _Object, PUU32* _pDef )
{
	PUU32 Method;
	PUU32 Param;

	while( Method = *_pDef++ )
	{
		// Check expected data type
		switch( Method & PU_DATATYPE_MASK )
		{
			case PU_DATATYPE_OBJREF:
				if( !( _pDef = puParseObjRef( _pCol, _pDef, &Param ) ) )
					return NULL;

				if( _Object && Param )
					puDoMethod( _Object, Method, Param, ( PUU32 )_pCol );
				break;

			case PU_DATATYPE_DESCSTREAM:
				_pDef = ( PUU32* )puDoMethod( _Object, Method, ( PUU32 )_pDef, ( PUU32 )_pCol );
				break;

			default:
				Param = *_pDef++;
				if( _Object )
					puDoMethod( _Object, Method, Param, ( PUU32 )_pCol );
				break;
		}
	}

	return _pDef;
}

PUU32* puParseObjDef( pusObjectCollection* _pCol, PUU32* _pDef, PUU32* _pResult )
{
	char* pClassName;
	PULID Object;
	pusCollectionReference* pColRef;
	PUU32 ObjId;

	ObjId = ( PUU32 )*_pDef++;
	pClassName = ( char* )*_pDef++;
	if( Object = puNewObjectFromName( pClassName, NULL ) )
	{
		if( !( pColRef = malloc( sizeof( pusCollectionReference ) ) ) )
		{
			puDeleteObject( Object );
			return NULL;
		}
		else
		{
			puDoMethod( Object, PUM_ADDREF, 0, 0 );
			pColRef->Object = Object;
			pColRef->ObjId = ObjId;
			puAddTail( &_pCol->ObjectList, &pColRef->Node );
		}
	}
	else
		return NULL;

	if( _pResult )
		*_pResult = Object;

	if( !( _pDef = puActionSetAttributes( _pCol, Object, _pDef ) ) )
		return NULL;

	return puCallMethods( _pCol, Object, _pDef );
}