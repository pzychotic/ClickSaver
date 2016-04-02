/* Poly
   Useful
   Lib */

#include <stdlib.h>
#include <string.h>
#include <pul/pul.h>
#include "include/pul.h"
#include "include/globals.h"
#include "classes/baseclass.h"
#include "classes/tableclass.h"
#include "classes/listclass.h"
#include "classes/containerclass.h"
#include "classes/windowclass.h"
#include "classes/controlclass.h"
#include "classes/spacerclass.h"
#include "classes/scrollingclass.h"
#include "classes/groupclass.h"
#include "classes/horgroupclass.h"
#include "classes/vergroupclass.h"
#include "classes/textclass.h"
#include "classes/checkboxclass.h"
#include "classes/sliderclass.h"
#include "classes/registerclass.h"
#include "classes/listviewclass.h"
#include "classes/textentryclass.h"
#include "classes/imageclass.h"
#include "classes/foldclass.h"
#include "classes/textareaclass.h"
#include "classes/comboboxclass.h"

PUU32 puInit()
{
	PULID BaseClass, TableClass, ListClass;
	PULID ContainerClass, WindowClass, ControlClass, SpacerClass, ScrollingClass;
	PULID GroupClass, HorGroupClass, VerGroupClass;
	PULID TextClass, CheckBoxClass, SliderClass, RegisterClass, ListViewClass;
	PULID TextEntryClass, ImageClass, FoldClass, TextAreaClass, ComboBoxClass;

	puInitList( &g_Globals.ClassList );
	memset( &g_Globals.AppMessageFIFO, 0, sizeof( pusAppMessage ) * PU_MAX_APPMESSAGES );
	g_Globals.FirstAppMessage = 0;
	g_Globals.LastAppMessage = 0;
	g_Globals.NumAppMessages = 0;
	g_Globals.pAppMessCallBack = NULL;

	// Init log
#ifdef _DEBUG
	puInitLog();
#endif

	if( !puSysInit() )
		return FALSE;

	if( !( BaseClass = puRegisterClass( "Base", puBaseClassHandler, sizeof( pusBaseClassData ), 0 ) ) )
		return FALSE;

	if( !( TableClass = puRegisterClass( "Table", puTableClassHandler, sizeof( pusTableClassData ), BaseClass ) ) )
		return FALSE;

	if( !( ListClass = puRegisterClass( "List", puListClassHandler, sizeof( pusListClassData ), TableClass ) ) )
		return FALSE;

	if( !( WindowClass = puRegisterClass( "Window", puWindowClassHandler, sizeof( pusWindowClassData ), BaseClass ) ) )
		return FALSE;

	if( !( ControlClass = puRegisterClass( "Control", puControlClassHandler, sizeof( pusControlClassData ), BaseClass ) ) )
		return FALSE;

	if( !( ContainerClass = puRegisterClass( "Container", puContainerClassHandler, sizeof( pusContainerClassData ), ControlClass ) ) )
		return FALSE;

	if( !( SpacerClass = puRegisterClass( "Spacer", puSpacerClassHandler, sizeof( pusSpacerClassData ), ControlClass ) ) )
		return FALSE;

	if( !( ScrollingClass = puRegisterClass( "Scrolling", puScrollingClassHandler, sizeof( pusScrollingClassData ), ControlClass ) ) )
		return FALSE;

	if( !( GroupClass = puRegisterClass( "Group", puGroupClassHandler, sizeof( pusGroupClassData ), ControlClass ) ) )
		return FALSE;

	if( !( HorGroupClass = puRegisterClass( "HorGroup", puHorGroupClassHandler, sizeof( pusHorGroupClassData ), GroupClass ) ) )
		return FALSE;

	if( !( VerGroupClass = puRegisterClass( "VerGroup", puVerGroupClassHandler, sizeof( pusVerGroupClassData ), GroupClass ) ) )
		return FALSE;

	if( !( TextClass = puRegisterClass( "Text", puTextClassHandler, sizeof( pusTextClassData ), ControlClass ) ) )
		return FALSE;

	if( !( CheckBoxClass = puRegisterClass( "CheckBox", puCheckBoxClassHandler, sizeof( pusCheckBoxClassData ), ControlClass ) ) )
		return FALSE;

	if( !( SliderClass = puRegisterClass( "Slider", puSliderClassHandler, sizeof( pusSliderClassData ), ControlClass ) ) )
		return FALSE;

	if( !( RegisterClass = puRegisterClass( "Register", puRegisterClassHandler, sizeof( pusRegisterClassData ), ControlClass ) ) )
		return FALSE;

	if( !( ListViewClass = puRegisterClass( "ListView", puListViewClassHandler, sizeof( pusListViewClassData ), ScrollingClass ) ) )
		return FALSE;

	if( !( TextEntryClass = puRegisterClass( "TextEntry", puTextEntryClassHandler, sizeof( pusTextEntryClassData ), ControlClass ) ) )
		return FALSE;

	if( !( ImageClass = puRegisterClass( "Image", puImageClassHandler, sizeof( pusImageClassData ), ControlClass ) ) )
		return FALSE;

	if( !( FoldClass = puRegisterClass( "Fold", puFoldClassHandler, sizeof( pusFoldClassData ), ControlClass ) ) )
		return FALSE;

	if( !( TextAreaClass = puRegisterClass( "TextArea", puTextAreaClassHandler, sizeof( pusTextAreaClassData ), ScrollingClass ) ) )
		return FALSE;

	if( !( ComboBoxClass = puRegisterClass( "ComboBox", puComboBoxClassHandler, sizeof( pusComboBoxClassData ), ControlClass ) ) )
		return FALSE;

	return TRUE;
}

void puClear()
{
	pusNode* pCurrent;
	pusNode* pNext;
	pusClass* pClass;

	pCurrent = g_Globals.ClassList.pFirst;

	while( pNext = pCurrent->pNext )
	{
		pClass = ( pusClass* )pCurrent;
		free( pClass );
		pCurrent = pNext;
	}

#ifdef _DEBUG
	puCloseLog();
#endif
}

PUU32 puCheckPointInRect( pusRect* _pRect, PUS32 _X, PUS32 _Y )
{
	if( _X < _pRect->X )
		return FALSE;

	if( _Y < _pRect->Y )
		return FALSE;

	if( _X >= ( _pRect->X + _pRect->Width ) )
		return FALSE;

	if( _Y >= ( _pRect->Y + _pRect->Height ) )
		return FALSE;

	return TRUE;
}