/* Poly
   Useful
   Lib */

#include <pul/pul.h>
#include "sliderclass.h"
#include "../include/pul.h"

#define MIN_KNOB_SIZE 8

PUU32 puSliderGetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr );
PUU32 puSliderSetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr, PUU32 _Val );
void RecalcSliderParams( pusSliderClassData* pData );

PUU32 puSliderClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 )
{
	pusSliderClassData* pData;
	pData = ( pusSliderClassData* )_pData;

	switch( _MethodID )
	{
		case PUM_NEW:
			pData->bVertical = 0;
			pData->Min = 0;
			pData->Max = 100;
			pData->Vis = 10;
			pData->Val = 0;
			pData->KnobSize = 10;
			pData->bLockKnobPos = FALSE;
			pData->TotalSize = 0;
			if( !puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 ) )
				return FALSE;
			puSetAttribute( _Object, PUA_CONTROL_FRAME, PUFRAME_BOX );
			return TRUE;

		case PUM_SET:
			puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
			return puSliderSetAttr( _Object, _Class, _pData, _Param1, _Param2 );

		case PUM_GET:
			return puSliderGetAttr( _Object, _Class,_pData, _Param1 );

		/* Control methods */
		case PUM_CONTROL_CALCMINMAX:
			{
				PUU32 MinLength = 30;
				PUU32 Thickness = 12;

				if( pData->bVertical )
				{
					puSetAttribute( _Object, PUA_CONTROL_MINWIDTH, Thickness );
					puSetAttribute( _Object, PUA_CONTROL_MAXWIDTH, Thickness );
					puSetAttribute( _Object, PUA_CONTROL_MINHEIGHT, MinLength );
				}
				else
				{
					puSetAttribute( _Object, PUA_CONTROL_MINHEIGHT, Thickness );
					puSetAttribute( _Object, PUA_CONTROL_MAXHEIGHT, Thickness );
					puSetAttribute( _Object, PUA_CONTROL_MINWIDTH, MinLength );
				}
			}
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		case PUM_CONTROL_LAYOUT:
			{
				pusRect Rect;
				puDoMethod( _Object, PUM_CONTROL_GETINNERRECT, ( PUU32 )&Rect, 0 );

				if( pData->bVertical )
					pData->TotalSize = Rect.Height;
				else
					pData->TotalSize = Rect.Width;

				RecalcSliderParams( pData );
			}

			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		case PUM_CONTROL_RENDER:
			{
				pusRect Rect;
				PUU32 FrameType;

				puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
				puDoMethod( _Object, PUM_CONTROL_GETINNERRECT, ( PUU32 )&Rect, 0 );
				puSetClipping( _Param1, &Rect );
				puSetFillPen( _Param1, 200, 200, 200 );
				puRect( _Param1, &Rect );

				if( pData->bVertical )
				{
					Rect.Y = pData->KnobPos + Rect.Y;
					Rect.Height = pData->KnobSize;
					FrameType = PUFRAME_VSLIDER;
				}
				else
				{
					Rect.X = pData->KnobPos + Rect.X;
					Rect.Width = pData->KnobSize;
					FrameType = PUFRAME_HSLIDER;
				}

				puDrawFrame( _Param1, FrameType, puGetAttribute( _Object, PUA_CONTROL_STATE ), &Rect, NULL );

				puSetClipping( _Param1, NULL );
			}
			break;

		case PUM_CONTROL_MOUSEEVENT:
			{
				pusMouseEvent* pMouseEvent;
				PUS32 MousePos, NewKnobPos;
				pusRect Rect;
				pMouseEvent = ( pusMouseEvent* )_Param1;

				puDoMethod( _Object, PUM_CONTROL_GETINNERRECT, ( PUU32 )&Rect, 0 );

				if( pData->bVertical )
					MousePos = pMouseEvent->MouseY - Rect.Y;
				else
					MousePos = pMouseEvent->MouseX - Rect.X;

				if( puGetAttribute( _Object, PUA_CONTROL_STATE ) == PUSTATE_ACTIVE )
				{
					switch( pMouseEvent->ButtonCode )
					{
						case PUMOUSE_LMB | PU_KEYUP:
							puSetAttribute( _Object, PUA_CONTROL_STATE, PUSTATE_NORMAL );
							puDoMethod( _Object, PUM_CONTROL_REDRAWINNER, TRUE, 0 );
							break;

						case PUMOUSE_RMB:
							puSetAttribute( _Object, PUA_CONTROL_STATE, PUSTATE_NORMAL );
							pData->KnobPos = pData->BackupPos;
							pData->Val = pData->BackupVal;
							puSetAttribute( _Object, PUA_SLIDER_VALUE, pData->Val );
							puDoMethod( _Object, PUM_CONTROL_REDRAWINNER, TRUE, 0 );
							break;

						default:
							if( pData->bVertical )
								MousePos = pMouseEvent->MouseY;
							else
								MousePos = pMouseEvent->MouseX;

							NewKnobPos = MousePos - pData->BackupMousePos + pData->BackupPos;

							if( NewKnobPos < 0 )
								NewKnobPos = 0;

							if( NewKnobPos > pData->KnobMaxPos )
								NewKnobPos = pData->KnobMaxPos;

							if( NewKnobPos != pData->KnobPos )
							{
								pData->KnobPos = NewKnobPos;

								pData->Val = pData->Min + pData->KnobPos * ( pData->Max - pData->Min - pData->Vis + 1 ) / pData->KnobMaxPos;
								pData->bLockKnobPos = TRUE;
								puSetAttribute( _Object, PUA_SLIDER_VALUE, pData->Val );
								pData->bLockKnobPos = FALSE;
							}
							break;
					}
				}
				else if( ( pMouseEvent->ButtonCode & ~PU_KEYDOUBLECLICK ) == PUMOUSE_LMB )
				{
					/* Check where the mouse is respectively to the knob
					   (either on it, before it or after it) */
				   if( MousePos < pData->KnobPos )
				   {
						// Click before knob: decrement value
						pData->Val -= pData->Vis - 1;

						if( pData->Val < pData->Min )
							pData->Val = pData->Min;

						// It looks stupid, but it's for the sake of notification
						puSetAttribute( _Object, PUA_SLIDER_VALUE, pData->Val );
				   }
				   else if( MousePos > ( pData->KnobPos + pData->KnobSize - 1 ) )
				   {
						// Click after knob: increment value
					   pData->Val += pData->Vis - 1;
					   if( pData->Val > ( pData->Max - ( pData->Vis - 1 ) ) )
						   pData->Val = pData->Max - ( pData->Vis - 1 );

						// It looks stupid, but it's for the sake of notification
						puSetAttribute( _Object, PUA_SLIDER_VALUE, pData->Val );
				   }
				   else
				   {
						// Click on knob: go in active state
						puSetAttribute( _Object, PUA_CONTROL_STATE, PUSTATE_ACTIVE );
						puDoMethod( _Object, PUM_CONTROL_REDRAWINNER, TRUE, 0 );

					   // Backup slider pos and value
					   pData->BackupVal = pData->Val;
					   pData->BackupPos = pData->KnobPos;

					   if( pData->bVertical )
						   pData->BackupMousePos = pMouseEvent->MouseY;
					   else
						   pData->BackupMousePos = pMouseEvent->MouseX;
				   }
				}
			}
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		default:
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
	}

	return 0;
}


PUU32 puSliderGetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr )
{
	pusSliderClassData* pData;
	pData = ( pusSliderClassData* )_pData;

	switch( _Attr )
	{
		case PUA_SLIDER_VALUE:
			return pData->Val;

		default:
			return puDoSuperMethod( _Object, _Class, PUM_GET, _Attr, 0 );
	}
}


PUU32 puSliderSetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr, PUU32 _Val )
{
	pusSliderClassData* pData;
	pData = ( pusSliderClassData* )_pData;

	switch( _Attr )
	{
		case PUA_SLIDER_ISVERTICAL:
			pData->bVertical = _Val;
			break;

		case PUA_SLIDER_MIN:
			pData->Min = _Val;
			RecalcSliderParams( pData );
			puDoMethod( _Object, PUM_CONTROL_REDRAWINNER, TRUE, 0 );
			break;

		case PUA_SLIDER_MAX:
			if( pData->Val > ( pData->Max - pData->Vis - 1 ) 
				&& ( PUS32 )_Val > pData->Max )
				pData->Val = _Val;

			pData->Max = _Val;
			RecalcSliderParams( pData );
			puDoMethod( _Object, PUM_CONTROL_REDRAWINNER, TRUE, 0 );
			break;

		case PUA_SLIDER_VISIBLE:
			pData->Vis = _Val;
			RecalcSliderParams( pData );
			puDoMethod( _Object, PUM_CONTROL_REDRAWINNER, TRUE, 0 );
			break;

		case PUA_SLIDER_VALUE:
			pData->Val = _Val;
			RecalcSliderParams( pData );
			puDoMethod( _Object, PUM_CONTROL_REDRAWINNER, TRUE, 0 );
			break;
	}

	return TRUE;
}

void RecalcSliderParams( pusSliderClassData* pData )
{
	if( pData->Vis < ( pData->Max - pData->Min ) )
	{
		pData->KnobSize = ( pData->Vis * pData->TotalSize ) / ( pData->Max - pData->Min + 1 );

		if( pData->Val > ( pData->Max - pData->Vis + 1 ) )
			pData->Val = pData->Max - pData->Vis + 1;
	}
	else
		pData->KnobSize = pData->TotalSize;

	if( pData->KnobSize < MIN_KNOB_SIZE )
		pData->KnobSize = MIN_KNOB_SIZE;
	pData->KnobMaxPos = pData->TotalSize - pData->KnobSize;

	if( !pData->bLockKnobPos )
	{
		if( pData->Max > pData->Min && ( pData->Vis < ( pData->Max - pData->Min ) ) )
			pData->KnobPos = ( ( pData->Val - pData->Min ) * pData->KnobMaxPos ) / ( pData->Max - pData->Min - pData->Vis + 1 );
		else
			pData->KnobPos = 0;

		if( pData->KnobPos > pData->KnobMaxPos )
			pData->KnobPos = pData->KnobMaxPos;
	}
}