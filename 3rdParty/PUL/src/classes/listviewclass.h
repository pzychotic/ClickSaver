/* Poly
   Useful
   Lib */

#ifndef __PUL_SRC_LISTVIEWCLASS_H__
#define __PUL_SRC_LISTVIEWCLASS_H__

#include <pul/pul.h>

typedef struct
{
	PULID	Table;
	PULID	FirstDisplayedRecord;
	PULID	SelectedRecord;
	PULID	NumRecordNotif;
	PULID	TextEntry;
	PUU32	NumVisibleEntries;
	PUU32	HorScroll;
	PUS32	YScroll;
	PUU32	bShowSelected, bEditable, bMustSelect;
	PUU32	EntryHeight;
	PUU32	bEditMode;
	PUU32	bNewEntry;
	PUS32	SelectedRecordIndex;
	PUS32	FirstDisplayedRecordIndex;
	PUU32	MaxNumVisible;
	PUU8	bHover;
} pusListViewClassData;

PUU32 puListViewClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 );

#endif