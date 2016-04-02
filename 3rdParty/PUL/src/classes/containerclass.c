/* Poly
   Useful
   Lib */

#include <pul/pul.h>
#include "containerclass.h"

PUU32 puContainerClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 )
{
	pusContainerClassData* pData;
	pData = ( pusContainerClassData* )_pData;

	switch( _MethodID )
	{
		case PUM_NEW:
			pData->Contents = 0;
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		case PUM_DELETE:
			if( pData->Contents )
				puDoMethod( pData->Contents, PUM_REMREF, 0, 0 );
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		/* Reference counting should apply to the container object, not its contents, so we catch
		   PUM_ADDREF and PUM_REMREF */
		case PUM_ADDREF:
		case PUM_REMREF:
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		/* Forward notification to contents object */
		case PUM_ADDNOTIFICATION:
		case PUM_REMNOTIFICATION:
			return puDoMethod( pData->Contents, PUM_SET, _Param1, _Param2 );

		case PUM_SET:
			/* Catch SET on container specific attributes */
			if( _Param1 == PUA_CONTAINER_CONTENTS )
			{
				if( pData->Contents != _Param2 )
				{
					PULID WindowObj;

					if( _Param2 )
						puDoMethod( _Param2, PUM_ADDREF, 0, 0 );

					if( pData->Contents )
						puDoMethod( pData->Contents, PUM_REMREF, 0, 0 );
					pData->Contents = _Param2;

				//	puSetAttribute( pData->Contents, PUA_CONTROL_WINDOW, puGetAttribute( _Object, PUA_CONTROL_WINDOW ) );
					if( WindowObj = puGetAttribute( _Object, PUA_CONTROL_WINDOW ) )
					{
						PUU32 Rastport;

						Rastport = puDoMethod( WindowObj, PUM_WINDOW_GETRASTPORT, 0, 0 );

						if( Rastport && pData->Contents )
							puDoMethod( pData->Contents, PUM_CONTROL_CALCMINMAX, Rastport, WindowObj );

						puDoMethod( WindowObj, PUM_WINDOW_RECALC, 0, 0 );
					}
				}
			}
			/* Forward the rest to contained object */
			else if( pData->Contents )
				return puDoMethod( pData->Contents, PUM_SET, _Param1, _Param2 );
			return TRUE;

		case PUM_GET:
			/* Catch get on window */
			if( _Param1 == PUA_CONTROL_WINDOW )
				return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
			else if( pData->Contents )
				return puDoMethod( pData->Contents, _MethodID, _Param1, _Param2 );
			else if( _Param1 == PUA_CONTROL_HIDDEN )
				return TRUE;

			return 0;

		case PUM_CONTROL_CALCMINMAX:
			/* Forward this to superclass to get window object handle */
			puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

			/* Fall through */

		/* Forward everything else to contained object */
		default:
			if( pData->Contents )
				return puDoMethod( pData->Contents, _MethodID, _Param1, _Param2 );
	}

	return 0;
}