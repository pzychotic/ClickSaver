/*
ClickSaver config file manager -  Anarchy Online mission helper
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
#include <pul/pul.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ClickSaver.h"

extern PULID g_ItemWatchList, g_LocWatchList;
extern PULID g_ItemWatchListview, g_LocWatchListview;
extern pusObjectCollection* g_pCol;

PULID g_ConfigList;
pusList g_ConfigNodes;
CSConfig g_DefaultConfig;

PUU8 InitConfig()
{
    CSConfig* pConfig;

    g_ConfigList = puGetObjectFromCollection( g_pCol, CS_CONFIGLIST );
    puInitList( &g_ConfigNodes );

    if( !( pConfig = NewConfig( "Default" ) ) )
        return FALSE;

    SelectConfig( pConfig );

    if( !( LoadConfigFile( "CS_SearchLists.txt" ) ) )
    {
        // New style config file not found, load
        // the old ones as default config

        // Load item list
        LoadWatchList( "CS_ItemWatch.txt", g_ItemWatchList );

        // Load location list
        LoadWatchList( "CS_LocWatch.txt", g_LocWatchList );
    }

    return TRUE;
}

static PUU32 puConfigColDef[]=
{
    PU_ACTION_OBJDEF, 1, ( PUU32 )"List",
        PUA_TABLE_NUMFIELDS, 1, 0,
        PUM_TABLE_SETFIELDTYPE, PUFIELD_TYPE_STRINGCOPY, 0,

    PU_ACTION_OBJDEF, 2, ( PUU32 )"List",
        PUA_TABLE_NUMFIELDS, 1, 0,
        PUM_TABLE_SETFIELDTYPE, PUFIELD_TYPE_STRINGCOPY, 0,
};

CSConfig* NewConfig( PUU8* _pName )
{
    CSConfig* pConfig;

    if( !( pConfig = malloc( sizeof( CSConfig ) ) ) )
        return NULL;

    if( !( pConfig->pCol = puCreateObjectCollection( puConfigColDef ) ) )
    {
        free( pConfig );
        return NULL;
    }

    pConfig->ItemList = puGetObjectFromCollection( pConfig->pCol, 1 );
    pConfig->LocList = puGetObjectFromCollection( pConfig->pCol, 2 );

    pConfig->RecordId = puDoMethod( g_ConfigList, PUM_TABLE_NEWRECORD, 0, 0 );
    puSetAttribute( g_ConfigList, PUA_TABLE_CURRENTFIELD, 1 );
    puDoMethod( g_ConfigList, PUM_TABLE_SETFIELDVAL, ( PUU32 )pConfig, 0 );
    puSetAttribute( g_ConfigList, PUA_TABLE_CURRENTFIELD, 0 );
    puDoMethod( g_ConfigList, PUM_TABLE_SETFIELDVAL, ( PUU32 )_pName, 0 );
    puDoMethod( g_ConfigList, PUM_TABLE_ADDRECORD, 0, 0 );

    puAddTail( &g_ConfigNodes, &pConfig->Node );

    return pConfig;
}

void DeleteConfig( CSConfig* _pConfig )
{
    SelectConfig( ( CSConfig* )g_ConfigNodes.pFirst );

    puRemoveNode( &_pConfig->Node );
    puDeleteObjectCollection( _pConfig->pCol );
    free( _pConfig );
}

void SelectConfig( CSConfig* _pConfig )
{
    g_ItemWatchList = _pConfig->ItemList;
    g_LocWatchList = _pConfig->LocList;

    puSetAttribute( g_ItemWatchListview, PUA_LISTVIEW_TABLE, g_ItemWatchList );
    puSetAttribute( g_LocWatchListview, PUA_LISTVIEW_TABLE, g_LocWatchList );
}

PUU8 LoadConfigFile( PUU8* _pFileName )
{
    FILE* fp;
    PUU8 String[256];
    PUU8* pString;
    PUU8 c;
    CSConfig* pConfig;
    PULID CurrentList;

    pConfig = ( CSConfig* )g_ConfigNodes.pFirst;
    CurrentList = pConfig->ItemList;

    if( !( fp = fopen( _pFileName, "r" ) ) )
        return FALSE;

    while( fscanf( fp, "%[^\n]\n", String ) != EOF )
    {
        // Strip trailing spaces/tab
        pString = String + strlen( String );

        while( pString > String )
        {
            c = *--pString;
            if( c != ' ' && c != '\t' )
                break;
        }

        *( pString + 1 ) = 0;

        // Strip leading spaces/tab
        pString = String;

        while( c = *pString++ )
        {
            if( c != ' ' && c != '\t' )
                break;
        }

        pString--;

        if( *pString )
        {
            if( *pString == '#' && *( pString + 1 ) != '#' )
            {
                // Create new config
                if( !( pConfig = NewConfig( pString + 1 ) ) )
                    return FALSE;
            }
            else if( !strcmp( "$item", pString ) )
                // Change list
                CurrentList = pConfig->ItemList;
            else if( !strcmp( "$loc", pString ) )
                CurrentList = pConfig->LocList;
            else
            {
                // Check for escaped '#' and '$' at begin of the string
                if( *pString == '#' && *( pString + 1 ) == '#' )
                    pString++;
                else if( *pString == '$' && *( pString + 1 ) == '$' )
                    pString++;

                puDoMethod( CurrentList, PUM_TABLE_NEWRECORD, 0, 0 );
                puDoMethod( CurrentList, PUM_TABLE_SETFIELDVAL, ( PUU32 )pString, 0 );
                puDoMethod( CurrentList, PUM_TABLE_ADDRECORD, 0, 0 );
            }
        }
    }

    fclose( fp );

    return TRUE;
}
/*
{
    FILE* fp;
    PUU32 Record;
    PUU8* pString;

    if( !( fp = fopen( _pFileName, "w" ) ) )
        return;

    Record = puDoMethod( _List, PUM_TABLE_GETFIRSTRECORD, 0, 0 );
    while( Record )
    {
        if( pString = ( PUU8* )puDoMethod( _List, PUM_TABLE_GETFIELDVAL, Record, 0 ) )
            fprintf( fp, "%s\n", pString );

        Record = puDoMethod( _List, PUM_TABLE_GETNEXTRECORD, Record, 0 );
    }

    fclose( fp );
}*/

void SaveConfigFile( PUU8* _pFileName )
{
    FILE* fp;
    pusNode* pCurrent;
    pusNode* pNext;
    CSConfig* pConfig;
    PULID Record;
    PUU32 Index = 0;
    PUU8* pString;

    if( !( fp = fopen( _pFileName, "w" ) ) )
        return;

    pCurrent = g_ConfigNodes.pFirst;

    while( pNext = pCurrent->pNext )
    {
        pConfig = ( CSConfig* )pCurrent;

        if( Index )
        {
            Record = puDoMethod( g_ConfigList, PUM_TABLE_GETRECORD, Index, 0 );

            if( pString = ( PUU8* )puDoMethod( g_ConfigList, PUM_TABLE_GETFIELDVAL, Record, 0 ) )
                fprintf( fp, "#%s\n\n", pString );
        }

        if( Record = puDoMethod( pConfig->ItemList, PUM_TABLE_GETFIRSTRECORD, 0, 0 ) )
            fprintf( fp, "$item\n" );

        while( Record )
        {
            if( pString = ( PUU8* )puDoMethod( pConfig->ItemList, PUM_TABLE_GETFIELDVAL, Record, 0 ) )
            {
                if( *pString == '#' )
                    fprintf( fp, "#" );
                else if( *pString == '$' )
                    fprintf( fp, "$" );
                fprintf( fp, "%s\n", pString );
            }

            Record = puDoMethod( pConfig->ItemList, PUM_TABLE_GETNEXTRECORD, Record, 0 );
        }

        if( Record = puDoMethod( pConfig->LocList, PUM_TABLE_GETFIRSTRECORD, 0, 0 ) )
            fprintf( fp, "\n$loc\n" );

        while( Record )
        {
            if( pString = ( PUU8* )puDoMethod( pConfig->LocList, PUM_TABLE_GETFIELDVAL, Record, 0 ) )
            {
                if( *pString == '#' )
                    fprintf( fp, "#" );
                else if( *pString == '$' )
                    fprintf( fp, "$" );
                fprintf( fp, "%s\n", pString );
            }

            Record = puDoMethod( pConfig->LocList, PUM_TABLE_GETNEXTRECORD, Record, 0 );
        }

        fprintf( fp, "\n\n" );

        Index++;
        pCurrent = pNext;
    }

    fclose( fp );
}

void LoadWatchList( PUU8* _pFileName, PULID _List )
{
    FILE* fp;
    PUU8 String[256];
    PUU8* pString;
    PUU8 c;

    if( !( fp = fopen( _pFileName, "r" ) ) )
        return;

    while( fscanf( fp, "%[^\n]\n", String ) != EOF )
    {
        // Strip trailing spaces/tab
        pString = String + strlen( String );

        while( pString > String )
        {
            c = *--pString;
            if( c != ' ' && c != '\t' )
                break;
        }

        *( pString + 1 ) = 0;

        // Strip leading spaces/tab
        pString = String;

        while( c = *pString++ )
        {
            if( c != ' ' && c != '\t' )
                break;
        }

        pString--;

        // If the resulting string isn't empty, add it to the list
        if( *pString )
        {
            puDoMethod( _List, PUM_TABLE_NEWRECORD, 0, 0 );
            puDoMethod( _List, PUM_TABLE_SETFIELDVAL, ( PUU32 )pString, 0 );
            puDoMethod( _List, PUM_TABLE_ADDRECORD, 0, 0 );
        }
    }

    fclose( fp );
}
