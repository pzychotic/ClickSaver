/* Poly
   Useful
   Lib */

#include <pul/pul.h>
#include <stdlib.h>
#include "tableclass.h"

PUU32 puTableClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 )
{
	pusTableClassData* pData;
	pData = ( pusTableClassData* )_pData;

	switch( _MethodID )
	{
		case PUM_NEW:
			pData->NumFields = 0;
			pData->pFields = 0;
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		case PUM_DELETE:
			if( pData->pFields )
				free( pData->pFields );
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		case PUM_GET:
			switch( _Param1 )
			{
				case PUA_TABLE_NUMFIELDS:
					return pData->NumFields;

			}
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		case PUM_SET:
			switch( _Param1 )
			{
				case PUA_TABLE_NUMFIELDS:
					if( !pData->pFields )
					{
						if( pData->pFields = ( pusTableField* )malloc( _Param2 * sizeof( pusTableField ) ) )
							pData->NumFields = _Param2;
					}
					break;
			}
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		/* Table methods */
		case PUM_TABLE_SETFIELDNAME:
			if( _Param1 < pData->NumFields )
				pData->pFields[ _Param1 ].pName = ( PUU8* )_Param2;
			break;

		case PUM_TABLE_SETFIELDTYPE:
			pData->pFields[ 0 ].Type = _Param1;
			break;

		case PUM_TABLE_GETFIELDS:
			return ( PUU32 )pData->pFields;

		case PUM_TABLE_GETFIELDNAME:
			if( _Param1 < pData->NumFields )
				return ( PUU32 )( pData->pFields[ _Param1 ].pName );
			else
				return NULL;

		case PUM_TABLE_GETFIELDTYPE:
			if( _Param1 < pData->NumFields )
				return pData->pFields[ _Param1 ].Type;
			else
				return NULL;

		default:
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
	}

	return 0;
}

/*
*#define PUA_TABLE_NUMFIELDS			PU_TABLECLASS_ATT + 1
#define PUA_TABLE_NUMRECORDS		PU_TABLECLASS_ATT + 2

*#define PUM_TABLE_SETFIELDNAME		PU_TABLECLASS_MTD + 1
*#define PUM_TABLE_SETFIELDTYPE		PU_TABLECLASS_MTD + 2
*#define PUM_TABLE_GETFIELDS			PU_TABLECLASS_MTD + 3
*#define PUM_TABLE_GETFIELDNAME		PU_TABLECLASS_MTD + 4	// Param: field num
*#define PUM_TABLE_GETFIELDTYPE		PU_TABLECLASS_MTD + 5	// Param: field num
#define PUM_TABLE_GETFIELDVAL		PU_TABLECLASS_MTD + 6	// Param1: record, Param2: field num
#define PUM_TABLE_GETFIRSTRECORD	PU_TABLECLASS_MTD + 7
#define PUM_TABLE_GETLASTRECORD		PU_TABLECLASS_MTD + 8
#define PUM_TABLE_GETRECORD			PU_TABLECLASS_MTD + 9	// Param: key
#define PUM_TABLE_GETNEXTRECORD		PU_TABLECLASS_MTD + 10	// Param: record
#define PUM_TABLE_GETPREVRECORD		PU_TABLECLASS_MTD + 11	// Param: record
#define PUM_TABLE_ADDRECORD			PU_TABLECLASS_MTD + 12	// Param: key
*/