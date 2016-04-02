/* Poly
   Useful
   Lib */

#include <pul/pul.h>
#include "spacerclass.h"
#include "../include/pul.h"

//PUU32 puSpacerGetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr );
PUU32 puSpacerSetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr, PUU32 _Val );


PUU32 puSpacerClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 )
{
	pusSpacerClassData* pData;
	pData = ( pusSpacerClassData* )_pData;

	switch( _MethodID )
	{
		case PUM_NEW:
			pData->MinWidth = 0;
			pData->MaxWidth = 0;
			pData->MinHeight = 0;
			pData->MaxHeight = 0;
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		case PUM_SET:
			puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
			return puSpacerSetAttr( _Object, _Class, _pData, _Param1, _Param2 );

		/* Control methods */
		case PUM_CONTROL_CALCMINMAX:
			puSetAttribute( _Object, PUA_CONTROL_MINWIDTH, pData->MinWidth );
			puSetAttribute( _Object, PUA_CONTROL_MAXWIDTH, pData->MaxWidth );
			puSetAttribute( _Object, PUA_CONTROL_MINHEIGHT, pData->MinHeight );
			puSetAttribute( _Object, PUA_CONTROL_MAXHEIGHT, pData->MaxHeight );
			puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
			break;

		case PUM_CONTROL_RENDER:
			puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
			puDoMethod( _Object, PUM_CONTROL_CLEARBG, _Param1, 0 );
			break;

		default:
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
	}

	return 0;
}

PUU32 puSpacerSetAttr( PULID _Object, PULID _Class, void* _pData, PUU32 _Attr, PUU32 _Val )
{
	pusSpacerClassData* pData;
	pData = ( pusSpacerClassData* )_pData;

	switch( _Attr )
	{
		case PUA_SPACER_WIDTH:
			pData->MinWidth = _Val;
			pData->MaxWidth = _Val;
			break;

		case PUA_SPACER_HEIGHT:
			pData->MinHeight = _Val;
			pData->MaxHeight = _Val;
			break;

		case PUA_SPACER_MINWIDTH:
			pData->MinWidth = _Val;
			break;

		case PUA_SPACER_MINHEIGHT:
			pData->MinHeight = _Val;
			break;

		case PUA_SPACER_MAXWIDTH:
			pData->MaxWidth = _Val;
			break;

		case PUA_SPACER_MAXHEIGHT:
			pData->MaxHeight = _Val;
			break;
	}

	return TRUE;
}