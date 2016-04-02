/* Poly
   Useful
   Lib */

#ifndef __PUL_SRC_BASECLASS_H__
#define __PUL_SRC_BASECLASS_H__

#include <pul/pul.h>

typedef struct
{
	pusNode	Node;
	PULID	ParentObject;
	pusList ChildList;
	PUU32	RefCount;

	PULID	LastNotification;

	pusList	NotificationList;
} pusBaseClassData;

typedef struct
{
	pusNode					Node;
	pusObjectCollection*	pCol;
	PUU32					bActive;	// If this flag is set, the notification is being triggered already
										// so we don't trigger it again (so circular notifications aren't a problem)
} pusNotification;

PUU32 puBaseClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 );

#endif