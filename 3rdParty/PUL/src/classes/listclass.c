/* Poly
   Useful
   Lib */

#include <pul/pul.h>
#include <stdlib.h>
#include <string.h>
#include "listclass.h"

PUU32 puListClassHandler( PULID _Object, PULID _Class, void* _pData, PUU32 _MethodID, PUU32 _Param1, PUU32 _Param2 )
{
	pusListClassData* pData;
	pusNode* pRecordNode;
	PUU32* pRecord;

	pData = ( pusListClassData* )_pData;

	switch( _MethodID )
	{
		case PUM_NEW:
			puInitList( &pData->List );
			pData->NumRecords = 0;
			pData->pCurrentRecord = NULL;
			pData->pNewRecord = NULL;
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		case PUM_DELETE:
			{
				PUU32* pRecord;
				pusNode* pCurrent;
				pusNode* pNext;

				pCurrent = pData->List.pFirst;

				while( pNext = pCurrent->pNext )
				{
					pRecord = ( PUU32*)( pCurrent + 1 );

					// Erm... TODO: Move management of the record payload (fields) in table class
					// It will rapidly become painful to redo the handling of every datatype
					// for each new table storage subclass
					switch( puDoMethod( _Object, PUM_TABLE_GETFIELDTYPE, 0, 0 ) )
					{
						case PUFIELD_TYPE_STRINGCOPY:
							{
								PUU8* pString;

								if( pString = ( PUU8* )pRecord[ 0 ] )
									free( pString );
							}
							break;
					}

					free( pCurrent );
					pCurrent = pNext;
				}
			}
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		case PUM_GET:
			switch( _Param1 )
			{
				case PUA_TABLE_CURRENTRECORD:
					return ( PUU32 )pData->pCurrentRecord;

				case PUA_TABLE_NUMRECORDS:
					return pData->NumRecords;
			}
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		case PUM_SET:
			switch( _Param1 )
			{
				case PUA_TABLE_CURRENTRECORD:
					pData->pCurrentRecord = ( pusNode* )_Param2;
					break;

				case PUA_TABLE_CURRENTFIELD:
					pData->CurrentField = _Param2;
					break;
			}
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );

		/* List methods */
		case PUM_TABLE_GETFIELDVAL:
			if( _Param1 && _Param2 < puGetAttribute( _Object, PUA_TABLE_NUMFIELDS ) )
			{
				pRecordNode = ( pusNode* )_Param1;
				pRecord = ( PUU32*)( pRecordNode + 1 );
				return pRecord[ _Param2 ];
			}
			break;

		case PUM_TABLE_GETFIRSTRECORD:
			if( pData->NumRecords )
				return ( PUU32 )pData->List.pFirst;
			else
				return 0;

		case PUM_TABLE_GETLASTRECORD:
			if( pData->NumRecords )
				return ( PUU32 )pData->List.pLast;
			else
				return 0;

		case PUM_TABLE_GETRECORD:
			{
				pusNode* pCurrent;

				pCurrent = pData->List.pFirst;

				if( !pCurrent->pNext )
					return NULL;

				if( !_Param1 )
				{
					if( pCurrent->pNext )
						return ( PUU32 )pCurrent;
					else
						return NULL;
				}

				do
				{
					pCurrent = pCurrent->pNext;
					_Param1--;
				} while( pCurrent->pNext && _Param1 );

				if( pCurrent->pNext )
				{
					pData->pCurrentRecord = pCurrent;
					return ( PUU32 )pCurrent;
				}
			}
			break;

		case PUM_TABLE_GETNEXTRECORD:
			if( !( pRecordNode = ( pusNode* )_Param1 ) )
				return 0;

			pRecordNode = pRecordNode->pNext;
			if( pRecordNode->pNext )
			{
				pData->pCurrentRecord = pRecordNode;
				return ( PUU32 )pRecordNode;
			}
			break;

		case PUM_TABLE_GETPREVRECORD:
			pRecordNode = ( pusNode* )_Param1;
			pRecordNode = pRecordNode->pPrev;
			if( pRecordNode->pPrev )
			{
				pData->pCurrentRecord = pRecordNode;
				return ( PUU32 )pRecordNode;
			}
			break;

		case PUM_TABLE_NEWRECORD:
			if( !pData->pNewRecord )
			{
				if( !( pRecordNode = ( pusNode* )malloc( puGetAttribute( _Object, PUA_TABLE_NUMFIELDS ) * 4 + sizeof( pusNode ) ) ) )
					return 0;

				memset( pRecordNode, 0, puGetAttribute( _Object, PUA_TABLE_NUMFIELDS ) * 4 + sizeof( pusNode ) );

				pData->pCurrentRecord = pData->pNewRecord = pRecordNode;
				pData->CurrentField = 0;
				return ( PUU32 )pRecordNode;
			}
			return NULL;

		case PUM_TABLE_ADDRECORD:
			if( pData->pNewRecord )
			{
				puAddTail( &pData->List, pData->pNewRecord );
				pData->pNewRecord = NULL;
				pData->NumRecords++;

				// To trigger notifications regarding numrecords (actually sets nothing :))
				puSetAttribute( _Object, PUA_TABLE_NUMRECORDS, pData->NumRecords );
			}
			return 1;

		case PUM_TABLE_REMRECORD:
			pRecordNode = ( pusNode* )_Param1;

			if( pRecordNode )
			{
				puRemoveNode( pRecordNode );
				pRecord = ( PUU32*)( pRecordNode + 1 );

				// Erm... TODO: Move management of the record payload (fields) in table class
				// It will rapidly become painful to redo the handling of every datatype
				// for each new table storage subclass
				switch( puDoMethod( _Object, PUM_TABLE_GETFIELDTYPE, 0, 0 ) )
				{
					case PUFIELD_TYPE_STRINGCOPY:
						{
							PUU8* pString;

							if( pString = ( PUU8* )pRecord[ 0 ] )
								free( pString );
						}
						break;
				}

				free( pRecordNode );
				pData->NumRecords--;

				// To trigger notifications regarding numrecords (actually sets nothing)
				puSetAttribute( _Object, PUA_TABLE_NUMRECORDS, pData->NumRecords );
			}
			break;

		case PUM_TABLE_SETFIELDVAL:
			if( pData->CurrentField < puGetAttribute( _Object, PUA_TABLE_NUMFIELDS ) )
			{
				pRecord = ( PUU32*)( pData->pCurrentRecord + 1 );

				switch( puDoMethod( _Object, PUM_TABLE_GETFIELDTYPE, pData->CurrentField, 0 ) )
				{
					case PUFIELD_TYPE_STRINGCOPY:
						{
							PUU8* pString;
							PUU8* pCopy;

							if( pString = ( PUU8* )pRecord[ pData->CurrentField ] )
								free( pString );

							pString = ( PUU8* )_Param1;

							if( pCopy = malloc( strlen( pString ) + 1 ) )
								strcpy( pCopy, pString );

							pRecord[ pData->CurrentField ] = ( PUU32 )pCopy;
						}
						break;

					default:
						pRecord[ pData->CurrentField ] = _Param1;
						break;
				}

				// Get the listview to redraw by triggering notifications...
				// not a proper way to do it, but the whole list/table thing
				// needs an overhaul anyway.
				puSetAttribute( _Object, PUA_TABLE_NUMRECORDS, pData->NumRecords );

				pData->CurrentField++;
			}
			break;

		case PUM_TABLE_CLEAR:
			{
				PUU32* pRecord;
				pusNode* pCurrent;
				pusNode* pNext;

				pCurrent = pData->List.pFirst;

				while( pNext = pCurrent->pNext )
				{
					puRemoveNode( pCurrent );

					pRecord = ( PUU32*)( pCurrent + 1 );

					// Erm... TODO: Move management of the record payload (fields) in table class
					// It will rapidly become painful to redo the handling of every datatype
					// for each new table storage subclass
					switch( puDoMethod( _Object, PUM_TABLE_GETFIELDTYPE, 0, 0 ) )
					{
						case PUFIELD_TYPE_STRINGCOPY:
							{
								PUU8* pString;

								if( pString = ( PUU8* )pRecord[ 0 ] )
									free( pString );
							}
							break;
					}

					pData->NumRecords = 0;
					// To trigger notifications regarding numrecords (actually sets nothing :))
					puSetAttribute( _Object, PUA_TABLE_NUMRECORDS, pData->NumRecords );


					free( pCurrent );
					pCurrent = pNext;
				}
			}
			break;

		default:
			return puDoSuperMethod( _Object, _Class, _MethodID, _Param1, _Param2 );
	}

	return 0;
}

/*
#define PUA_TABLE_NUMRECORDS		PU_TABLECLASS_ATT + 2

#define PUM_TABLE_GETFIELDVAL		PU_TABLECLASS_MTD + 6	// Param1: record, Param2: field num
#define PUM_TABLE_GETFIRSTRECORD	PU_TABLECLASS_MTD + 7
#define PUM_TABLE_GETLASTRECORD		PU_TABLECLASS_MTD + 8
#define PUM_TABLE_GETRECORD			PU_TABLECLASS_MTD + 9	// Param: key
#define PUM_TABLE_GETNEXTRECORD		PU_TABLECLASS_MTD + 10	// Param: record
#define PUM_TABLE_GETPREVRECORD		PU_TABLECLASS_MTD + 11	// Param: record
#define PUM_TABLE_ADDRECORD			PU_TABLECLASS_MTD + 12	// Param: key
#define PUM_TABLE_SETFIELDVAL		PU_TABLECLASS_MTD + 13	// Param1: record, Param2: value
*/