/* Poly
   Useful
   Lib */

#ifndef __PUL_SRC_COMBOBOXCLASS_H__
#define __PUL_SRC_COMBOBOXCLASS_H__

#include <pul/pul.h>

typedef struct
{
	PULID	Table;
	PULID	PopupWindow;
	PULID	Listview;
	pusObjectCollection* pCol;

	PUS32	Selected;
} pusComboBoxClassData;

PUU32 puComboBoxClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 );

#endif