/* Poly
   Useful
   Lib */

#include <pul/oo.h>
#include <stdlib.h>
#include <string.h>
#include "include/globals.h"

PULID puFindClass( char* _pClassName )
{
	pusNode* pCurrent;
	pusNode* pNext;
	pusClass* pClass;

	pCurrent = g_Globals.ClassList.pFirst;

	while( pNext = pCurrent->pNext )
	{
		pClass = ( pusClass* )pCurrent;

		if( pClass->pName )
			if( !strcmp( _pClassName, pClass->pName ) )
				return ( PULID )pClass;

		pCurrent = pNext;
	}

	return 0;
}

PULID puRegisterClass( char* _pClassName, PULHANDLER _Handler, unsigned long _DataSize, PULID _SuperClass )
{
	pusClass* pClass;
	pusClass* pSuperClass;

	PUU32 DataSize = 4, DataOffset = 4;
 
	if( !( pClass = malloc( sizeof( pusClass ) ) ) )
		return 0;

	pClass->pName = _pClassName;
	pClass->Handler = _Handler;

	pSuperClass = ( pusClass* )_SuperClass;
	pClass->pSuperClass = pSuperClass;

	if( pSuperClass )
		DataSize = DataOffset = pSuperClass->DataSize;

	pClass->DataSize = DataSize + _DataSize;
	pClass->DataOffset = DataOffset - 4;

	puAddTail( &g_Globals.ClassList, &pClass->Node );

	return ( PULID )pClass;
}

void puRemoveClass( PULID _Class )
{
	pusClass* pClass;

	pClass = ( pusClass* )_Class;

	if( pClass )
	{
		puRemoveNode( &pClass->Node );
		free( pClass );
	}
}

void puRemoveClassFromName( char* _pClassName )
{
	PULID Class;

	if( Class = puFindClass( _pClassName ) )
		puRemoveClass( Class );
}

PULID puNewObject( PULID _Class, PULTAG* _pTagList )
{
	pusClass* pClass;
	void** pObject;

	pClass = ( pusClass* )_Class;

	if( !( pObject = malloc( pClass->DataSize ) ) )
		return 0;

	*pObject = pClass;

	if( !puDoMethod( ( PULID )( pObject + 1 ), PUM_NEW, ( PUU32 )_pTagList, 0 ) )
	{
		free( pObject );
		return 0;
	}

	return ( PULID )( pObject + 1 );
}

PULID puNewObjectFromName( char* _pClassName, PULTAG* _pTagList )
{
	PULID Class;

	if( !( Class = puFindClass( _pClassName ) ) )
		return 0;

	return puNewObject( Class, _pTagList );
}

void puDeleteObject( PULID _Object )
{
	void* pObject;

	if( _Object == 0 )
		return;

	puDoMethod( _Object, PUM_DELETE, 0, 0 );

	pObject = ( void* )( _Object - 4 );
	free( pObject );
}