/* Poly
   Useful
   Lib */

#ifndef __PUL_NOTIFICATION_H__
#define __PUL_NOTIFICATION_H__

#include <pul/types.h>
#include <pul/list.h>
#include <pul/classes.h>

enum
{
	PUNOTIFY_CONDITION_ATTRCHANGE = 0x90000000,
	PUNOTIFY_CONDITION_ATTREQUALS,
};

enum
{
	PUNOTIFY_ACTION_CALLBACK = 0xa0000000,
	PUNOTIFY_ACTION_SET,
	PUNOTIFY_ACTION_SETVAL,
	PUNOTIFY_ACTION_SETNOTVAL,
	PUNOTIFY_ACTION_TOGGLE,
	PUNOTIFY_ACTION_DOMETHOD,
	PUNOTIFY_ACTION_APPMSG
};

typedef void ( * PUNOTCALLBACK )( PUU32 _Attr, PUU32 _Val );

/*
	Notification definition:
		<condition>
		<action>

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

	Set attribute to !(notified attribute value):
		PUNOTIFY_ACTION_SETNOTVAL, <obj>, <attr id>

	Toggle attribute between 0 and 1:
		PUNOTIFY_ACTION_TOGGLE, <obj>, <attr id>

	Call a method without params:
		PUNOTIFY_ACTION_DOMETHOD, <obj>, <method id>

	Send an appmessage with notified attribute value as param:
		PUNOTIFY_ACTION_APPMSG, <message id>
*/

#endif