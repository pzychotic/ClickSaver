/* Poly
   Useful
   Tools
   Environement */

#ifndef __PUL_SRC_COLLECTION_H__
#define __PUL_SRC_COLLECTION_H__

#include <pul/types.h>
#include <pul/list.h>
#include <pul/classes.h>

typedef struct
{
	pusNode	Node;
	PUU32	ObjId;
	PULID	Object;
} pusCollectionReference;

#endif