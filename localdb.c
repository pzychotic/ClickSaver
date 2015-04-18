/*
 * $Log: localdb.c,v $
 * Revision 1.2  2004/01/25 19:36:05  gnarf37
 * 2.3.0 beta 3 - Shrunk Database a bit, added Item Value options, make options menu smaller a tad so that 800x600 might be able to use it again...
 *
 */
/*
ClickSaver local database manager -  Anarchy Online mission helper
Copyright (C) 2001, 2002 Morb
Some parts Copyright (C) 2003, 2004 gnarf

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <shlobj.h>
#include "BerkeleyDB/db.h"
#include "clicksaver.h"
#include "AODB/AODB.h"
#include "AODB/ctree.h"

extern char g_AODir[ 256 ];
extern DB* g_pDB;

extern IFIL udtAODBfile;

int ImportRecords( PUU32 _Type );


int CreateLocalDatabase()
{
    DWORD dwVersion, dwWindowsMajorVersion;
    // Delete old files from CS 2.2.2
    DeleteFile( "Resource.idx" );
    DeleteFile( "Resource.dat" );

    // On some setups the copies were named like the original
    // (as they were supposed to be anyway, I don't even want to
    // know why SHFileOperation was truncating it to 8 chars on
    // most setups including mine)
    DeleteFile( "ResourceDatabase.idx" );
    DeleteFile( "ResourceDatabase.dat" );

    // Delete old database
    DeleteFile( "AODatabase.bdb" );
    DeleteFile( "tmp.bdb" );

    // Open AO's ctree databasse
    if( InitAODatabase( g_AODir ) != AODB_ERR_NOERROR )
    {
        return FALSE;
    }

    // Create the local database
    if( db_create( &g_pDB, NULL, 0 ) )
    {
        ReleaseAODatabase();
        return FALSE;
    }

    // Under XP, for some reasons it's a lot faster
    // when forcing a 512 bytes page size
    // but it's not the case for 98/ME...
    dwVersion = GetVersion();
    dwWindowsMajorVersion = (DWORD)( LOBYTE( LOWORD( dwVersion ) ) );

    if( !( dwVersion >= 0x80000000 && dwWindowsMajorVersion >= 4 ) )
    {
        g_pDB->set_pagesize( g_pDB, 512 );
    }

    // Use a big cache to try to get decent performances
    // under win98/ME...
    g_pDB->set_cachesize( g_pDB, 0, 1024 * 1024 * 32, 64 );

    if( g_pDB->open( g_pDB, "tmp.bdb", NULL, DB_HASH, DB_CREATE, 0 ) )
    {
        g_pDB->close( g_pDB, 0 );
        ReleaseAODatabase();
        return FALSE;
    }


    // Import playfield records in local DB
    if( !ImportRecords( AODB_TYP_PF ) )
    {
        g_pDB->close( g_pDB, 0 );
        ReleaseAODatabase();
        return FALSE;
    }

    // Import item records in local DB
    if( !ImportRecords( AODB_TYP_ITEM ) )
    {
        g_pDB->close( g_pDB, 0 );
        ReleaseAODatabase();
        return FALSE;
    }

    // Import icon records in local DB
    if( !ImportRecords( AODB_TYP_ICON ) )
    {
        g_pDB->close( g_pDB, 0 );
        ReleaseAODatabase();
        return FALSE;
    }


    g_pDB->close( g_pDB, 0 );
    ReleaseAODatabase();

    rename( "tmp.bdb", "AODatabase.bdb" );

    return TRUE;
}


int ImportRecords( PUU32 _Type )
{
    PUU32 Key[ 2 ];
    DBT bdbKey, bdbData;
    PUU32 Size;
    PUU8* pBuffer;
    PUU32* pKey;
    MissionItem sItem;

    PUU32 numItems = 0, numBytes = 0;
    char ps[ 256 ];

    // Alloc a temporary buffer to store the record
    // A bit oversized but then i'm sure that everything
    // will fit, and I feel lazy about first getting
    // the record size and then allocating the proper amount of ram,
    // at least for database conversion purpose.
    if( !( pBuffer = malloc( 524288 ) ) )
    {
        return FALSE;
    }

    memset( &bdbKey, 0, sizeof( DBT ) );
    memset( &bdbData, 0, sizeof( DBT ) );

    // Get first record of requested type
    Key[ 0 ] = EndianSwap32( _Type );
    Key[ 1 ] = 0;

    Size = 524288;

    if( GetGTEVRecord( (COUNT)( udtAODBfile.tfilno + 1 ), &Key, pBuffer, &Size ) )
    {
        free( pBuffer );
        return FALSE;
    }

    pKey = (PUU32*)pBuffer;
    bdbKey.data = pKey;
    bdbKey.size = 8;

    switch( _Type )
    {
    case AODB_TYP_ITEM:
        bdbData.data = &sItem;
        bdbData.size = sizeof( sItem );
        break;
    case AODB_TYP_PF:
        bdbData.data = pBuffer + 0x20;
        break;
    default:
        bdbData.data = pBuffer + 0x18;
    }

    // Read the records in sequence until a different type (or no more records)
    // is reached, and store them in the local db
    while( *pKey == _Type )
    {
        puCheckMessages();
        switch( _Type )
        {
        case AODB_TYP_ITEM:
        {
            PUU8 *a_xData = pBuffer + 0x18;
            PUU8 *a_xDataPtr = a_xData;
            // Reducing size of database here...
            /* Set ptr to first slot */
            a_xDataPtr = a_xData + 0xc; // + 0x24;
            memset( &sItem, 0, sizeof( sItem ) );
            /* Process the slots we require */
            while( a_xDataPtr + 8 <= a_xData + Size )
            {
                PUU32 lStat = *( (unsigned long *)a_xDataPtr );
                PUU32 lValue = *( (unsigned long *)a_xDataPtr + 1 );
                a_xDataPtr += 0x8;

                switch( lStat )
                {
                    /* Quality */
                case 0x36:
                    sItem.QL = (unsigned short)lValue;
                    break;

                    /* Icon */
                case 0x4F:
                    sItem.IconKey = lValue;
                    break;

                    /* Item Value */
                case 0x4A:
                    sItem.Value = lValue;
                    break;

                    /* Item name and description */
                case 0x15:
                    if( lValue == 0x21 )
                    {
                        PUU16 lNameLen = *( (unsigned short *)a_xDataPtr );
                        PUU16 lDescLen = *( (unsigned short *)a_xDataPtr + 1 );
                        a_xDataPtr += 0x4;

                        /* Store the item name */
                        if( lNameLen > AODB_MAX_NAME_LEN )
                            lNameLen = AODB_MAX_NAME_LEN;
                        memcpy( sItem.pName, a_xDataPtr, lNameLen );
                        sItem.pName[ lNameLen ] = 0;
                    }
                    break;
                }
            }
        }
        break;

        case AODB_TYP_PF:
            bdbData.size = strlen( bdbData.data ) + 1;
            break;
        default:
            bdbData.size = Size - 0x18;
        }
        if( g_pDB->put( g_pDB, NULL, &bdbKey, &bdbData, 0 ) )
        {
            return FALSE;
        }
        numItems++;
        numBytes += bdbData.size;
        Size = 524288;
        if( NextVRecord( (COUNT)( udtAODBfile.tfilno + 1 ), pBuffer, &Size ) )
        {
            break;
        }
    }

    free( pBuffer );
    sprintf( ps, "Recorded %d records, %d total size\n", numItems, numBytes );
    WriteDebug( ps );

    return TRUE;
}


int OpenLocalDB()
{
    if( g_pDB->open( g_pDB, "AODatabase.bdb", NULL, DB_UNKNOWN, DB_RDONLY, 0 ) )
    {
        return FALSE;
    }

    return TRUE;
}


void* GetDataChunk( PUU32 _KeyHi, PUU32 _KeyLo, PUU32* _pSize )
{
    DBT bdbKey, bdbData;
    PUU32 Key[ 2 ];

    Key[ 0 ] = _KeyHi;
    Key[ 1 ] = _KeyLo;

    memset( &bdbKey, 0, sizeof( DBT ) );
    memset( &bdbData, 0, sizeof( DBT ) );

    bdbKey.data = Key;
    bdbKey.size = 8;

    bdbData.flags = DB_DBT_MALLOC;

    if( g_pDB->get( g_pDB, NULL, &bdbKey, &bdbData, 0 ) )
    {
        return NULL;
    }

    if( _pSize )
    {
        *_pSize = bdbData.size;
    }

    return bdbData.data;
}
