/* Poly
   Useful
   Lib */

#ifndef __PUL_COLLECTION_H__
#define __PUL_COLLECTION_H__

#include <pul/types.h>
#include <pul/list.h>
#include <pul/classes.h>

typedef struct
{
	pusList ObjectList;
} pusObjectCollection;

pusObjectCollection*	puCreateObjectCollection( PUU32* _pCollectionDef );
int						puAppendObjectCollection( pusObjectCollection* _pCol, PUU32* _pCollectionDef );
void					puDeleteObjectCollection( pusObjectCollection* _pCollection );
PULID					puGetObjectFromCollection( pusObjectCollection* _pCollection, PUU32 _ObjId );

/* Markers for puCreateObjectCollection */
enum
{
	PU_ACTION_END = 0x80000000,
	PU_ACTION_OBJDEF,
	PU_ACTION_SETATTRIBUTES,
	PU_ACTION_DOMETHODS,
	PU_OBJID
};

/*
	Object collection definition:
		<action>
		<action>
		.
		.
		PU_ACTION_END	

	<action>:
		Create object:
			PU_ACTION_OBJDEF, <objid>, <objdef>

		Set attributes:
			PU_ACTION_SETATTRIBUTES, <objid>, <attributes>

		Do methods:
			PU_ACTION_DOMETHODS, <objid>, <method calls>

	<objdef>:
		"Class", <attributes>, <method calls>

	<attributes>:
		<ATTRIBUTEID>, <value> or <objref> (for some attributes only)
		<ATTRIBUTEID>, <value> or <objref> (for some attributes only)
		.
		.
		0

	<method calls>
		<METHODID>, <value> or <objref> (for some methods only)
		<METHODID>, <value> or <objref> (for some methods only)
		.
		.
		0

	<objref>
		PU_ACTION_OBJDEF, <objid>, <objdef>
		or
		PU_OBJID, <objid>
*/

#endif