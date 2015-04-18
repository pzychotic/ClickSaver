/* Poly
   Useful
   Lib */

#ifndef __PUL_OO_H__
#define __PUL_OO_H__

#include <pul/types.h>
#include <pul/list.h>
#include <pul/classes.h>
#include <pul/log.h>

/* Datatype field for attributes and methods (7 bits) */
#define PU_DATATYPE_MASK		( 0x7f << 24 )

/* Type: reference to another object */
#define PU_DATATYPE_OBJREF		( 1 << 24 )

/* Type: stream of PUU32 */
#define PU_DATATYPE_DESCSTREAM	( 2 << 24 )

/* Type: string */
#define PU_DATATYPE_STRING		( 4 << 24 )

#define PU_TAG_USER				( 1 << 23 )

typedef PUU32 ( *PULHANDLER )( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 );

typedef struct _pusClass pusClass;

struct _pusClass
{
	pusNode		Node;
	char*		pName;
	pusClass*	pSuperClass;
	PULHANDLER	Handler;
	PUU32		DataOffset;
	PUU32		DataSize;
};

PULID puFindClass( char* _pClassName );
PULID puRegisterClass( char* _pClassName, PULHANDLER _Handler, PUU32 _DataSize, PULID _SuperClass );
void puRemoveClass( PULID _Class );
void puRemoveClassFromName( char* _pClassName );

/* Object creation */
PULID puNewObject( PULID _Class, PULTAG* _TagList );
PULID puNewObjectFromName( char* _pClassName, PULTAG* _TagList );

void puDeleteObject( PULID _Object );

static __inline PUU32 puDoMethod( PULID _Object, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 )
{
	PUU32* pObject;
	pusClass* pClass;
	void* pData;
	PUU32 Result;

	pObject = ( PUU32* )_Object;
	pClass = ( pusClass* )*( pObject - 1 );

	pData = ( ( PUU8* )pObject ) + pClass->DataOffset;

/*#ifdef _DEBUG
	if( _MethodID != PUM_SET && _MethodID != PUM_GET )
		PULOG( "METHODCALL", "Object %08x, Method %08x, Param1 %x, Param2 %x", _Object, _MethodID, _Param1, _Param2 );
#endif*/

	Result = pClass->Handler( _Object, ( PULID )pClass, pData, _MethodID, _Param1, _Param2 );

/*#ifdef _DEBUG
	if( _MethodID != PUM_SET && _MethodID != PUM_GET )
		PULOG( "METHODCALL", "Result %x", Result, 0, 0, 0 );
#endif*/

	return Result;
}

static __inline PUU32 puDoSuperMethod( PULID _Object, PULID _Class, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 )
{
	PUU8* pObject;
	pusClass* pClass;
	void* pData;

	pObject = ( PUU8* )_Object;
	pClass = ( pusClass* )_Class;
	pClass = pClass->pSuperClass;

	pData = pObject + pClass->DataOffset;

	return pClass->Handler( _Object, ( PULID )pClass, pData, _MethodID, _Param1, _Param2 );
}

static __inline PUU32 puSetAttribute( PULID _Object, PUU32 _AttributeID, PUU32 _Value )
{
/*#ifdef _DEBUG
	if( ( _AttributeID & PU_DATATYPE_MASK ) == PU_DATATYPE_STRING )
	{
		PULOG( "SETATTR", "Object %08x, Attr %08x, Value %x \"%s\"", _Object, _AttributeID, _Value, _Value );
	}
	else
	{
		PULOG( "SETATTR", "Object %08x, Attr %08x, Value %x", _Object, _AttributeID, _Value, 0 );
	}
#endif*/

	return puDoMethod( _Object, PUM_SET, _AttributeID, _Value );
}

static __inline PUU32 puGetAttribute( PULID _Object, PUU32 _AttributeID )
{
	PUU32 Result;
	Result = puDoMethod( _Object, PUM_GET, _AttributeID, 0 );

//	PULOG( "GETATTR", "Object %08x, Attr %08x, Value %x", _Object, _AttributeID, Result, 0 );

	return Result;
}

#endif