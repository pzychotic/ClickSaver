/*
 * $Log: clicksaver.c,v $
 * Revision 1.16  2004/12/27 17:28:12  gnarf37
 * Added Option for multiple missions, quick change
 *
 * Revision 1.15  2004/09/03 19:16:46  gnarf37
 * Version 2.3.1 AI Updates
 *
 * Revision 1.14  2004/08/28 18:04:08  gnarf37
 * Moved some GUI Options arounds, added Skip Rebuild option
 *
 * Revision 1.13  2004/01/25 19:35:52  gnarf37
 * 2.3.0 beta 3 - Shrunk Database a bit, added Item Value options, make options menu smaller a tad so that 800x600 might be able to use it again...
 *
 * Revision 1.12  2004/01/23 08:19:09  ibender
 * added mission slider settings
 *
 * Revision 1.11  2003/11/06 23:41:50  gnarf37
 * Version 2.3.0 beta 2 - Fixed issues with 15.2.0 and added an option for auto expand team missions
 *
 * Revision 1.10  2003/10/31 03:40:50  gnarf37
 * Saving/Loading Configurations
 *
 * Revision 1.9  2003/10/25 21:33:32  gnarf37
 * Fixed date/time checking... Should get rid of the major problem everyone is havving
 *
 * Revision 1.8  2003/05/27 00:14:42  gnarf37
 * Added Checkbox to stop mouse movement, and cleaned up mission info parsing so it doesnt match stale missions
 *
 * Revision 1.7  2003/05/08 09:11:09  gnarf37
 * Fullscreen Mode
 *
 * Revision 1.6  2003/05/08 08:40:04  gnarf37
 * Added Logging to Missions
 *
 * Revision 1.5  2003/05/08 07:36:55  gnarf37
 * Added Sounds
 *
 * Revision 1.4  2003/05/07 14:05:28  gnarf37
 * *** empty log message ***
 *
 */
/*
ClickSaver -  Anarchy Online mission helper
Copyright (C) 2001, 2002 Morb
Some parts Copyright (C) 2003, 2004 gnarf
Some parts Copyright (C) 2012 Darkbane, Adjuster

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

#include "Platform.h"

#include <pul/pul.h>

#include <winuser.h>
#include <shlobj.h>
#include <stdio.h>
#include <string.h>
#include "clicksaver.h"
#include "resource.h"
#include "AODB/AODB.h"

#include "BerkeleyDB/db.h"

void CleanUp();
void ImportSettings( char* filename );
void ExportSettings( char* filename );

void DisplayErrorMessage( PUU8* _pMessage, PUU32 _bAsynchronous );

void GetFolder( HWND hWndOwner, char *strTitle, char *strPath );
BOOL GetFile( HWND hWndOwner, BOOL saving, char *buffer, int buffersize );

int BuyingAgent();
void EndBuyingAgent();

extern PUU32 g_GUIDef[];
pusObjectCollection* g_pCol;
PULID g_ItemWatchList, g_LocWatchList, g_MainWin;

void _setSliders( int easy_hard, int good_bad, int order_chaos, int open_hidden, int phys_myst, int headon_stealth, int money_xp );

PUU32 g_BuyingAgentCount = 0;
PUU32 g_BuyingAgentMissions = 0;
PUU32 g_bFirstRound = TRUE;
PUU8 g_MishNumber = 0, g_FoundMish = -1;
PUU8 g_bFullscreen = 0;

char g_CurrentPacket[ 65536 ];

char g_AODir[ MAX_PATH ] = { 0 };
char g_CSDir[ MAX_PATH ] = { 0 };

HANDLE g_Mutex = INVALID_HANDLE_VALUE;
HANDLE g_Thread = INVALID_HANDLE_VALUE;
DWORD WINAPI HookManagerThread( void *pParam );

DB* g_pDB = NULL;


typedef enum ImportSettingsMode
{
    ISM_CONFIG,
    ISM_LOCWATCH,
    ISM_ITEMWATCH,
    ISM_SLIDERS,
    ISM_DONE,
} ImportSettingsMode;


int main( int argc, char** argv )
{
    pusAppMessage* pAppMsg;
    void* pMissionData;
    PULID MissionControls[ 5 ];
    FILE* fp;
    char AOExePath[ 256 ];
    DWORD dwThreadID;
    HANDLE hOrigDB, hLocalDB;
    int bUpdateDB = FALSE;
    char DBPath[ 256 * 2 ];

    // Set main thread of clicksaver on a priority above normal
    // Helps a lot. Refreshing of missions infos is much faster.
    SetThreadPriority( GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL );

    // Initialise PUL
    if( !puInit() )
    {
        return -1;
    }

    // Register mission control class
    if( !RegisterMissionClass() )
    {
        CleanUp();
        return -1;
    }

    // Create the windows
    if( !( g_pCol = puCreateObjectCollection( g_GUIDef ) ) )
    {
        CleanUp();
        return -1;
    }

    g_MainWin = puGetObjectFromCollection( g_pCol, CS_MAIN_WINDOW );
    SendMessage( (HWND)g_MainWin, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon( NULL, MAKEINTRESOURCE( IDI_ICON1 ) ) );
    SendMessage( (HWND)g_MainWin, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon( NULL, MAKEINTRESOURCE( IDI_ICON1 ) ) );
    g_ItemWatchList = puGetObjectFromCollection( g_pCol, CS_ITEMWATCH_LIST );
    g_LocWatchList = puGetObjectFromCollection( g_pCol, CS_LOCWATCH_LIST );

    // Get current directory
    GetCurrentDirectory( MAX_PATH, g_CSDir );

    ImportSettings( "LastSettings.cs" );

    if( puGetAttribute( puGetObjectFromCollection( g_pCol, CS_STARTMIN_CB ), PUA_CHECKBOX_CHECKED ) )
        puSetAttribute( g_MainWin, PUA_WINDOW_ICONIFIED, TRUE );

    sprintf( AOExePath, "%s\\anarchy.exe", g_AODir );
    if( !( fp = fopen( AOExePath, "r" ) ) )
    {
        GetFolder( NULL, "Please locate the AO directory:", g_AODir );

        if( !g_AODir[ 0 ] )
        {
            CleanUp();
            return -1;
        }

        sprintf( AOExePath, "%s\\anarchy.exe", g_AODir );
        if( !( fp = fopen( AOExePath, "r" ) ) )
        {
            DisplayErrorMessage( "This is not AO's directory.", FALSE );
            CleanUp();
            return -1;
        }
    }

    fclose( fp );

    /* Check if local database is up to date */
    hLocalDB = CreateFile( "AODatabase.bdb", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );

    if( hLocalDB != INVALID_HANDLE_VALUE )
    {
        sprintf( DBPath, "%s\\cd_image\\data\\db\\ResourceDatabase.dat", g_AODir );
        hOrigDB = CreateFile( DBPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );

        if( hOrigDB != INVALID_HANDLE_VALUE )
        {
            FILETIME OrigTime, LocalTime;

            GetFileTime( hLocalDB, NULL, NULL, &LocalTime );
            GetFileTime( hOrigDB, NULL, NULL, &OrigTime );

            if( CompareFileTime( &OrigTime, &LocalTime ) >= 0 )
                bUpdateDB = TRUE;

            CloseHandle( hOrigDB );
        }

        CloseHandle( hLocalDB );
    }
    else
    {
        bUpdateDB = TRUE;
    }

    if( bUpdateDB )
    {
        puSetAttribute( puGetObjectFromCollection( g_pCol, CS_DBCOPYMSGBOX ), PUA_WINDOW_OPENED, TRUE );

        pAppMsg = puWaitAppMessages();

        if( pAppMsg->Message == CSAM_QUIT )
        {
            CleanUp();
            return -1;
        }

        puSetAttribute( puGetObjectFromCollection( g_pCol, CS_DBCOPYMSGBOX ), PUA_WINDOW_OPENED, FALSE );

        if( pAppMsg->Message == CSAM_OK )
        {

            puSetAttribute( puGetObjectFromCollection( g_pCol, CS_CREATINGDBMSGBOX ), PUA_WINDOW_OPENED, TRUE );
            puCheckMessages();

            if( !CreateLocalDatabase() )
            {
                puSetAttribute( puGetObjectFromCollection( g_pCol, CS_CREATINGDBMSGBOX ), PUA_WINDOW_OPENED, FALSE );

                // Delete the database in case it was partly created,
                // so we're not using a partial database on next
                // execution.
                DeleteFile( "AODatabase.bdb" );
                g_pDB = NULL;   // It has been already closed by createlocaldatabase

                DisplayErrorMessage( "Failed to create local database.", FALSE );
                CleanUp();
                return -1;
            }

            puSetAttribute( puGetObjectFromCollection( g_pCol, CS_CREATINGDBMSGBOX ), PUA_WINDOW_OPENED, FALSE );
        }

    }

    // Create Berkeley DB handle
    if( db_create( &g_pDB, NULL, 0 ) )
    {
        DisplayErrorMessage( "Couldn't create Berkeley DB handle.", FALSE );
        CleanUp();
        return -1;
    }

    g_pDB->set_alloc( g_pDB, malloc, realloc, free );

    if( !OpenLocalDB() )
    {
        DisplayErrorMessage( "Couldn't open local database.", FALSE );
        CleanUp();
        return -1;
    }

    // Create mutex
    if( ( g_Mutex = CreateMutex( NULL, FALSE, "ClickSaver" ) ) == INVALID_HANDLE_VALUE )
    {
        DisplayErrorMessage( "Couldn't create mutex.", FALSE );
        ReleaseAODatabase();
        CleanUp();
        return -1;
    }
    if( GetLastError() == ERROR_ALREADY_EXISTS )
    {
        HWND hWnd;
        if( hWnd = FindWindow( "ClickSaverHookWindowClass", "ClickSaverHookWindow" ) )
        {
            // send some message
            return -1;
        }
    }
    // Starts dll hook management thread
    if( ( g_Thread = CreateThread( NULL, 0, &HookManagerThread, NULL, 0, &dwThreadID ) ) == INVALID_HANDLE_VALUE )
    {
        DisplayErrorMessage( "Couldn't create hook thread.", FALSE );
        ReleaseAODatabase();
        CleanUp();
        return -1;
    }

    MissionControls[ 0 ] = puGetObjectFromCollection( g_pCol, CS_MISSION1 );
    MissionControls[ 1 ] = puGetObjectFromCollection( g_pCol, CS_MISSION2 );
    MissionControls[ 2 ] = puGetObjectFromCollection( g_pCol, CS_MISSION3 );
    MissionControls[ 3 ] = puGetObjectFromCollection( g_pCol, CS_MISSION4 );
    MissionControls[ 4 ] = puGetObjectFromCollection( g_pCol, CS_MISSION5 );
    //puSetAttribute( puGetObjectFromCollection( g_pCol, CS_OPTIONSFOLD3 ), PUA_FOLD_FOLDED, TRUE);
    puSetAttribute( g_MainWin, PUA_WINDOW_OPENED, TRUE );

    do
    {
        pAppMsg = puWaitAppMessages();

        switch( pAppMsg->Message )
        {
        case CSAM_STOPBUYINGAGENT:
            g_BuyingAgentCount = 0;
            g_BuyingAgentMissions = 0;
            EndBuyingAgent();

            // Fall through

        case CSAM_NEWMISSIONS:
            if( !g_BuyingAgentCount && g_bFullscreen )
            {
                g_BuyingAgentCount = puGetAttribute( puGetObjectFromCollection( g_pCol, CS_BUYINGAGENTTRIES ), PUA_TEXTENTRY_VALUE );
            }
            if( g_BuyingAgentCount )
            {
                g_BuyingAgentCount--;
                pMissionData = g_CurrentPacket;

                WaitForSingleObject( g_Mutex, INFINITE );
                g_FoundMish = 255;
                for( g_MishNumber = 0; g_MishNumber < 5; g_MishNumber++ )
                {
                    if( !( pMissionData = (void*)puDoMethod( MissionControls[ g_MishNumber ], CSM_MISSION_PARSEMISSION, (PUU32)pMissionData, 0 ) ) )
                    {
                        break;
                    }
                }
                ReleaseMutex( g_Mutex );

                //if( pMissionData )
                //{
                    if( g_BuyingAgentCount )
                    {
                        BuyingAgent();
                    }
                    else
                    {
                        EndBuyingAgent();
                    }
                //}
            }

            if( !g_BuyingAgentCount )
            {
                pMissionData = g_CurrentPacket;
                puSetAttribute( g_MainWin, PUA_WINDOW_DEFERUPDATE, TRUE );

                WaitForSingleObject( g_Mutex, INFINITE );
                g_FoundMish = 255;
                for( g_MishNumber = 0; g_MishNumber < 5; g_MishNumber++ )
                {
                    void *pLastMissionData;
                    pLastMissionData = pMissionData;
                    if( !( pMissionData = (void*)puDoMethod( MissionControls[ g_MishNumber ], CSM_MISSION_PARSEMISSION, (PUU32)pMissionData, 0 ) ) )
                    {
                        pMissionData = pLastMissionData;
                    }
                }

                ReleaseMutex( g_Mutex );

                if( pMissionData && !g_bFullscreen )
                {
                    puSetAttribute( puGetObjectFromCollection( g_pCol, CS_ERROR_WINDOW ), PUA_WINDOW_OPENED, FALSE );

                    // Select mission tab
                    puSetAttribute( puGetObjectFromCollection( g_pCol, CS_TABS ), PUA_REGISTER_CURRENTTAB, 0 );

                    // Uniconify window
                    puSetAttribute( g_MainWin, PUA_WINDOW_ICONIFIED, FALSE );
                }

                puSetAttribute( g_MainWin, PUA_WINDOW_DEFERUPDATE, FALSE );

                if( PUL_GET_CB( CS_SOUNDS_CB ) )
                {
                    if( g_FoundMish == 255 ) // Not Found
                    {
                        PlaySound( "notfound.wav", NULL, SND_FILENAME | SND_NODEFAULT );
                    }
                    else
                    {
                        PlaySound( "found.wav", NULL, SND_FILENAME | SND_NODEFAULT );
                    }
                }
                if( PUL_GET_CB( CS_MOUSEMOVE_CB ) || g_BuyingAgentMissions )
                {
                    HWND AOWnd;
                    POINT MousePos;
                    LPARAM lParam;

                    WriteLog( NULL );

                    // Find AO window
                    if( !( AOWnd = FindWindow( "Anarchy client", NULL ) ) )
                    {
                        DisplayErrorMessage( "Anarchy Online is not running.", TRUE );
                        g_BuyingAgentCount = 0;
                    }

                    if( g_FoundMish != 255 && !( pAppMsg->Message == CSAM_STOPBUYINGAGENT ) )
                    { // Move mouse and select mission that finished our Buying Agent
                        MousePos.x = 44 + ( ( g_FoundMish % 3 ) * 58 );
                        MousePos.y = 57 + ( ( g_FoundMish / 3 ) * 57 );
                        lParam = MousePos.y << 16 | MousePos.x;

                        ClientToScreen( AOWnd, &MousePos );
                        SetCursorPos( MousePos.x, MousePos.y );

                        SendMessage( AOWnd, WM_LBUTTONDOWN, 0, lParam );
                        Sleep( 500 );
                        SendMessage( AOWnd, WM_LBUTTONUP, 0, lParam );

                        Sleep( 2010 );

                        MousePos.x = 76; MousePos.y = 321;
                        lParam = MousePos.y << 16 | MousePos.x;
                        ClientToScreen( AOWnd, &MousePos );
                        SetCursorPos( MousePos.x, MousePos.y );
                        if( g_BuyingAgentMissions )
                        {
                            SendMessage( AOWnd, WM_LBUTTONDOWN, 0, lParam );
                            Sleep( 500 );
                            SendMessage( AOWnd, WM_LBUTTONUP, 0, lParam );

                            Sleep( 2010 );

                            SendMessage( AOWnd, WM_KEYDOWN, 0x45, 0 );
                            Sleep( 500 );
                            SendMessage( AOWnd, WM_KEYUP, 0x45, 0 );

                            Sleep( 2010 );

                            {
                                int easy_hard = puGetAttribute( puGetObjectFromCollection( g_pCol, CS_SLIDER_EASY_HARD ), PUA_TEXTENTRY_VALUE );
                                int good_bad = puGetAttribute( puGetObjectFromCollection( g_pCol, CS_SLIDER_GOOD_BAD ), PUA_TEXTENTRY_VALUE );
                                int order_chaos = puGetAttribute( puGetObjectFromCollection( g_pCol, CS_SLIDER_ORDER_CHAOS ), PUA_TEXTENTRY_VALUE );
                                int open_hidden = puGetAttribute( puGetObjectFromCollection( g_pCol, CS_SLIDER_OPEN_HIDDEN ), PUA_TEXTENTRY_VALUE );
                                int phys_myst = puGetAttribute( puGetObjectFromCollection( g_pCol, CS_SLIDER_PHYS_MYST ), PUA_TEXTENTRY_VALUE );
                                int headon_stealth = puGetAttribute( puGetObjectFromCollection( g_pCol, CS_SLIDER_HEADON_STEALTH ), PUA_TEXTENTRY_VALUE );
                                int money_xp = puGetAttribute( puGetObjectFromCollection( g_pCol, CS_SLIDER_MONEY_XP ), PUA_TEXTENTRY_VALUE );

                                _setSliders( easy_hard, good_bad, order_chaos, open_hidden, phys_myst, headon_stealth, money_xp );
                            }

                            g_bFirstRound = TRUE;
                            g_BuyingAgentCount = puGetAttribute( puGetObjectFromCollection( g_pCol, CS_BUYINGAGENTTRIES ), PUA_TEXTENTRY_VALUE );
                            BuyingAgent();
                            g_BuyingAgentMissions--;
                        }
                    }
                }
                WriteLog( NULL ); // Close log file at this point
            }
            break;

        case CSAM_PRESTARTBUYINGAGENT:
            if( puGetAttribute( puGetObjectFromCollection( g_pCol, CS_BAINFO_CB ), PUA_CHECKBOX_CHECKED ) )
            {
                puSetAttribute( puGetObjectFromCollection( g_pCol, CS_BUYINGAGENT_INFOWINDOW ), PUA_WINDOW_OPENED, TRUE );
                break;
            }

            // Fall through

        case CSAM_STARTBUYINGAGENT:
            puSetAttribute( puGetObjectFromCollection( g_pCol, CS_BUYINGAGENT_INFOWINDOW ), PUA_WINDOW_OPENED, FALSE );

            if( !g_BuyingAgentCount )
            {
                PUU32 bItemListOk = FALSE, bLocListOk = FALSE, bTypeListOk = FALSE;
                PUU32 bWarnItem, bWarnLoc, bWarnType;
                PUU32 bReadyToGo = FALSE;

                puSetAttribute( puGetObjectFromCollection( g_pCol, CS_ERROR_WINDOW ), PUA_WINDOW_OPENED, FALSE );

                // Make sure that there's something in the relevant watch list
                // (depending on the mission condition settings)
                // before starting eating up player's credits for nothing :)
                bWarnItem = puGetAttribute( puGetObjectFromCollection( g_pCol, CS_ALERTITEM_CB ), PUA_CHECKBOX_CHECKED );
                bWarnLoc = puGetAttribute( puGetObjectFromCollection( g_pCol, CS_ALERTLOC_CB ), PUA_CHECKBOX_CHECKED );
                bWarnType = puGetAttribute( puGetObjectFromCollection( g_pCol, CS_ALERTTYPE_CB ), PUA_CHECKBOX_CHECKED );

                if( puGetAttribute( g_ItemWatchList, PUA_TABLE_NUMRECORDS ) )
                {
                    bItemListOk = TRUE;
                }

                if( puGetAttribute( g_LocWatchList, PUA_TABLE_NUMRECORDS ) )
                {
                    bLocListOk = TRUE;
                }

                if( puGetAttribute( puGetObjectFromCollection( g_pCol, CS_TYPEREPAIR_CB ), PUA_CHECKBOX_CHECKED ) ||
                    puGetAttribute( puGetObjectFromCollection( g_pCol, CS_TYPEFINDP_CB ), PUA_CHECKBOX_CHECKED ) ||
                    puGetAttribute( puGetObjectFromCollection( g_pCol, CS_TYPEFINDI_CB ), PUA_CHECKBOX_CHECKED ) ||
                    puGetAttribute( puGetObjectFromCollection( g_pCol, CS_TYPERETURN_CB ), PUA_CHECKBOX_CHECKED ) ||
                    puGetAttribute( puGetObjectFromCollection( g_pCol, CS_TYPEASS_CB ), PUA_CHECKBOX_CHECKED ) )
                {
                    bTypeListOk = TRUE;
                }

                bReadyToGo = bWarnLoc || bWarnItem || bWarnType;
                if( bWarnItem ) bReadyToGo = bReadyToGo && bItemListOk;
                if( bWarnLoc ) bReadyToGo = bReadyToGo && bLocListOk;
                if( bWarnType ) bReadyToGo = bReadyToGo && bTypeListOk;

                if( bReadyToGo )
                {
                    g_BuyingAgentMissions = puGetAttribute( puGetObjectFromCollection( g_pCol, CS_BUYINGAGENTMISH ), PUA_TEXTENTRY_VALUE ) - 1;
                    g_BuyingAgentCount = puGetAttribute( puGetObjectFromCollection( g_pCol, CS_BUYINGAGENTTRIES ), PUA_TEXTENTRY_VALUE );
                    g_bFirstRound = TRUE;
                    BuyingAgent();
                }
                else
                {
                    // Complain to the user that he/she hasn't told us what to search for
                    DisplayErrorMessage( "I won't ever find any mission with your current settings and watch lists.", TRUE );
                }
            }
            break;

        case CSAM_EXPORTSETTINGS:
        {
            char buffer[ 2000 ];
            if( GetFile( (HWND)puGetAttribute( puGetObjectFromCollection( g_pCol, CS_MAIN_WINDOW ), PUA_WINDOW_HANDLE )
                , TRUE, buffer, 2000 ) )
            {
                ExportSettings( buffer );
            }
            SetCurrentDirectory( g_CSDir );
        }
        break;

        case CSAM_IMPORTSETTINGS:
        {
            char buffer[ 2000 ];
            if( GetFile( (HWND)puGetAttribute( puGetObjectFromCollection( g_pCol, CS_MAIN_WINDOW ), PUA_WINDOW_HANDLE )
                , FALSE, buffer, 2000 ) )
            {
                ImportSettings( buffer );
            }
            SetCurrentDirectory( g_CSDir );
        }
        break;

        case CSAM_STOPFULLSCREEN:
            g_bFullscreen = 0;
            puSetAttribute( puGetObjectFromCollection( g_pCol, CS_FULLSCREEN_WINDOW ), PUA_WINDOW_OPENED, FALSE );
            puSetAttribute( puGetObjectFromCollection( g_pCol, CS_MAIN_WINDOW ), PUA_WINDOW_OPENED, TRUE );
            break;

        case CSAM_STARTFULLSCREEN:
        {
            PUU32 bItemListOk = FALSE, bLocListOk = FALSE, bTypeListOk = FALSE;
            PUU32 bWarnItem, bWarnLoc, bWarnType;
            PUU32 bReadyToGo = FALSE;

            puSetAttribute( puGetObjectFromCollection( g_pCol, CS_ERROR_WINDOW ), PUA_WINDOW_OPENED, FALSE );

            // Make sure that there's something in the relevant watch list
            // (depending on the mission condition settings)
            // before starting eating up player's credits for nothing :)
            bWarnItem = puGetAttribute( puGetObjectFromCollection( g_pCol, CS_ALERTITEM_CB ), PUA_CHECKBOX_CHECKED );
            bWarnLoc = puGetAttribute( puGetObjectFromCollection( g_pCol, CS_ALERTLOC_CB ), PUA_CHECKBOX_CHECKED );
            bWarnType = puGetAttribute( puGetObjectFromCollection( g_pCol, CS_ALERTTYPE_CB ), PUA_CHECKBOX_CHECKED );

            if( puGetAttribute( g_ItemWatchList, PUA_TABLE_NUMRECORDS ) )
            {
                bItemListOk = TRUE;
            }

            if( puGetAttribute( g_LocWatchList, PUA_TABLE_NUMRECORDS ) )
            {
                bLocListOk = TRUE;
            }

            if( puGetAttribute( puGetObjectFromCollection( g_pCol, CS_TYPEREPAIR_CB ), PUA_CHECKBOX_CHECKED ) ||
                puGetAttribute( puGetObjectFromCollection( g_pCol, CS_TYPEFINDP_CB ), PUA_CHECKBOX_CHECKED ) ||
                puGetAttribute( puGetObjectFromCollection( g_pCol, CS_TYPEFINDI_CB ), PUA_CHECKBOX_CHECKED ) ||
                puGetAttribute( puGetObjectFromCollection( g_pCol, CS_TYPERETURN_CB ), PUA_CHECKBOX_CHECKED ) ||
                puGetAttribute( puGetObjectFromCollection( g_pCol, CS_TYPEASS_CB ), PUA_CHECKBOX_CHECKED ) )
            {
                bTypeListOk = TRUE;
            }

            bReadyToGo = bWarnLoc || bWarnItem || bWarnType;
            if( bWarnItem ) bReadyToGo = bReadyToGo && bItemListOk;
            if( bWarnLoc ) bReadyToGo = bReadyToGo && bLocListOk;
            if( bWarnType ) bReadyToGo = bReadyToGo && bTypeListOk;

            if( bReadyToGo )
            {
                puSetAttribute( puGetObjectFromCollection( g_pCol, CS_FULLSCREEN_WINDOW ), PUA_WINDOW_OPENED, TRUE );
                puSetAttribute( puGetObjectFromCollection( g_pCol, CS_MAIN_WINDOW ), PUA_WINDOW_OPENED, FALSE );
                g_bFullscreen = 1;
            }
            else
            {
                // Complain to the user that he/she hasn't told us what to search for
                DisplayErrorMessage( "I won't ever find any mission with your current settings and watch lists.", TRUE );
            }
        }
        break;

        case CSAM_SET_SLIDERS:
        {
            int easy_hard = puGetAttribute( puGetObjectFromCollection( g_pCol, CS_SLIDER_EASY_HARD ), PUA_TEXTENTRY_VALUE );
            int good_bad = puGetAttribute( puGetObjectFromCollection( g_pCol, CS_SLIDER_GOOD_BAD ), PUA_TEXTENTRY_VALUE );
            int order_chaos = puGetAttribute( puGetObjectFromCollection( g_pCol, CS_SLIDER_ORDER_CHAOS ), PUA_TEXTENTRY_VALUE );
            int open_hidden = puGetAttribute( puGetObjectFromCollection( g_pCol, CS_SLIDER_OPEN_HIDDEN ), PUA_TEXTENTRY_VALUE );
            int phys_myst = puGetAttribute( puGetObjectFromCollection( g_pCol, CS_SLIDER_PHYS_MYST ), PUA_TEXTENTRY_VALUE );
            int headon_stealth = puGetAttribute( puGetObjectFromCollection( g_pCol, CS_SLIDER_HEADON_STEALTH ), PUA_TEXTENTRY_VALUE );
            int money_xp = puGetAttribute( puGetObjectFromCollection( g_pCol, CS_SLIDER_MONEY_XP ), PUA_TEXTENTRY_VALUE );

            _setSliders( easy_hard, good_bad, order_chaos, open_hidden, phys_myst, headon_stealth, money_xp );
        }
        break;
        }
    }
    while( pAppMsg->Message != CSAM_QUIT );

    WriteDebug( NULL );

    SetCurrentDirectory( g_CSDir );

    ExportSettings( "LastSettings.cs" );


    //ReleaseAODatabase();
    CleanUp();
    return 0;
}


void CleanUp()
{
    if( g_Thread != INVALID_HANDLE_VALUE )
    {
        TerminateThread( g_Thread, 0 );
    }

    if( g_Mutex != INVALID_HANDLE_VALUE )
    {
        CloseHandle( g_Mutex );
    }

    if( g_pDB )
    {
        g_pDB->close( g_pDB, 0 );
    }

    puDeleteObjectCollection( g_pCol );
    puClear();
}


enum
{
    CFG_AODIR,
    CFG_WINDOWX,
    CFG_WINDOWY,
    CFG_WINDOWWIDTH,
    CFG_STARTMINIMIZED,
    CFG_WATCHMSGBOX,
    CFG_ALERTITEM,
    CFG_ALERTLOC,
    CFG_ALERTTYPE,
    CFG_BUYINGAGENTMAXTRIES,
    CFG_BUYINGAGENTHIDE,
    CFG_BUYINGAGENTSHOWHELP,
    CFG_MISSIONTYPES,
    CFG_HIGHLIGHTOPTS,
    CFG_SOUNDS,
    CFG_LOG,
    CFG_MOUSEMOVE,
    CFG_EXPAND,
    CFG_ITEMVALUE,

    CFG_SLIDER_EASY_HARD,
    CFG_SLIDER_GOOD_BAD,
    CFG_SLIDER_ORDER_CHAOS,
    CFG_SLIDER_OPEN_HIDDEN,
    CFG_SLIDER_PHYS_MYST,
    CFG_SLIDER_HEADON_STEALTH,
    CFG_SLIDER_MONEY_XP,

    CFG_BUYMOD,
};


struct
{
    int id;
    char* keyword;
} CfgKeywords[] =
{
    { CFG_AODIR, "AODIR" },
    { CFG_SOUNDS, "SOUNDS" },
    { CFG_MOUSEMOVE, "MOUSEMOVE" },
    { CFG_LOG, "LOG" },
    { CFG_WINDOWX, "WINDOWX" },
    { CFG_WINDOWY, "WINDOWY" },
    { CFG_WINDOWWIDTH, "WINDOWWIDTH" },
    { CFG_STARTMINIMIZED, "STARTMINIMIZED" },
    { CFG_WATCHMSGBOX, "WATCHMSGBOX" },
    { CFG_BUYINGAGENTSHOWHELP, "BUYINGAGENTSHOWHELP" },
    { CFG_ALERTLOC, "ALERTLOC" },
    { CFG_ALERTITEM, "ALERTITEM" },
    { CFG_ALERTTYPE, "ALERTTYPE" },
    { CFG_BUYINGAGENTMAXTRIES, "BUYINGAGENTMAXTRIES" },
    { CFG_BUYINGAGENTHIDE, "BUYINGAGENTHIDE" },
    { CFG_MISSIONTYPES, "MISHTYPES" },
    { CFG_HIGHLIGHTOPTS, "HIGHLIGHTOPTS" },
    { CFG_EXPAND, "EXPAND" },

    { CFG_SLIDER_EASY_HARD, "SLIDER_EASY_HARD" },
    { CFG_SLIDER_GOOD_BAD, "SLIDER_GOOD_BAD" },
    { CFG_SLIDER_ORDER_CHAOS, "SLIDER_ORDER_CHAOS" },
    { CFG_SLIDER_OPEN_HIDDEN, "SLIDER_OPEN_HIDDEN" },
    { CFG_SLIDER_PHYS_MYST, "SLIDER_PHYS_MYST" },
    { CFG_SLIDER_HEADON_STEALTH, "SLIDER_HEADON_STEALTH" },
    { CFG_SLIDER_MONEY_XP, "SLIDER_MONEY_XP" },

    { CFG_ITEMVALUE, "ITEMVALUE" },

    { CFG_BUYMOD, "BUYMOD" },

    { 0, NULL }
};


void ImportSettings( char* filename )
{
    FILE* fp;
    PUU32 Record;
    PUU8* pString;
    char buffer[ 1000 ];
    PUU8 Keyword[ 256 ], Value[ 256 ];
    int Id, i;
    PUU32 Val;
    int mode = ISM_DONE;
    char c;

    Record = puDoMethod( g_LocWatchList, PUM_TABLE_GETFIRSTRECORD, 0, 0 );
    while( Record )
    {
        puDoMethod( g_LocWatchList, PUM_TABLE_REMRECORD, Record, 0 );
        Record = puDoMethod( g_LocWatchList, PUM_TABLE_GETFIRSTRECORD, 0, 0 );
    }
    Record = puDoMethod( g_ItemWatchList, PUM_TABLE_GETFIRSTRECORD, 0, 0 );
    while( Record )
    {
        puDoMethod( g_ItemWatchList, PUM_TABLE_REMRECORD, Record, 0 );
        Record = puDoMethod( g_ItemWatchList, PUM_TABLE_GETFIRSTRECORD, 0, 0 );
    }

    if( !( fp = fopen( filename, "r" ) ) )
    {
        return;
    }

    while( fgets( buffer, 1000, fp ) )
    {
        if( sscanf( buffer, "::%s", &buffer ) == 1 )
        {
            strtok( buffer, ":" );
            if( !_stricmp( buffer, "Config" ) ) mode = ISM_CONFIG;
            if( !_stricmp( buffer, "LocWatch" ) ) mode = ISM_LOCWATCH;
            if( !_stricmp( buffer, "ItemWatch" ) ) mode = ISM_ITEMWATCH;

            if( !_stricmp( buffer, "Sliders" ) ) mode = ISM_SLIDERS;
            if( !_stricmp( buffer, "Done" ) ) mode = ISM_DONE;
            continue;
        }
        switch( mode )
        {
        case ISM_DONE:
            break;

        case ISM_CONFIG:
            if( sscanf( buffer, "%[^:]::%[^\n]\n", Keyword, Value ) != EOF )
            {
                i = 0, Id = -1;
                while( CfgKeywords[ i ].keyword )
                {
                    if( !strcmp( Keyword, CfgKeywords[ i ].keyword ) )
                    {
                        Id = CfgKeywords[ i ].id;
                        break;
                    }

                    i++;
                }

                switch( Id )
                {
                case CFG_AODIR:
                    strcpy( g_AODir, Value );
                    break;

                case CFG_WINDOWX:
                    sscanf( Value, "%u", &Val );
                    if( Val < 16384 )
                    {
                        puSetAttribute( g_MainWin, PUA_WINDOW_XPOS, Val );
                    }
                    break;

                case CFG_WINDOWY:
                    sscanf( Value, "%u", &Val );
                    if( Val < 16384 )
                    {
                        puSetAttribute( g_MainWin, PUA_WINDOW_YPOS, Val );
                    }
                    break;

                case CFG_WINDOWWIDTH:
                    sscanf( Value, "%u", &Val );
                    puSetAttribute( g_MainWin, PUA_WINDOW_WIDTH, Val );
                    break;

                case CFG_STARTMINIMIZED:
                    sscanf( Value, "%u", &Val );
                    puSetAttribute( puGetObjectFromCollection( g_pCol, CS_STARTMIN_CB ), PUA_CHECKBOX_CHECKED, ( Val ? TRUE : FALSE ) );
                    break;

                case CFG_WATCHMSGBOX:
                    sscanf( Value, "%u", &Val );
                    puSetAttribute( puGetObjectFromCollection( g_pCol, CS_MSGBOX_CB ), PUA_CHECKBOX_CHECKED, ( Val ? TRUE : FALSE ) );
                    break;

                case CFG_BUYINGAGENTSHOWHELP:
                    sscanf( Value, "%u", &Val );
                    puSetAttribute( puGetObjectFromCollection( g_pCol, CS_BAINFO_CB ), PUA_CHECKBOX_CHECKED, ( Val ? TRUE : FALSE ) );
                    break;

                case CFG_SOUNDS:
                    sscanf( Value, "%u", &Val );
                    puSetAttribute( puGetObjectFromCollection( g_pCol, CS_SOUNDS_CB ), PUA_CHECKBOX_CHECKED, ( Val ? TRUE : FALSE ) );
                    break;

                case CFG_MOUSEMOVE:
                    sscanf( Value, "%u", &Val );
                    puSetAttribute( puGetObjectFromCollection( g_pCol, CS_MOUSEMOVE_CB ), PUA_CHECKBOX_CHECKED, ( Val ? TRUE : FALSE ) );
                    break;

                case CFG_EXPAND:
                    sscanf( Value, "%u", &Val );
                    puSetAttribute( puGetObjectFromCollection( g_pCol, CS_EXPAND_CB ), PUA_CHECKBOX_CHECKED, ( Val ? TRUE : FALSE ) );
                    break;

                case CFG_LOG:
                    sscanf( Value, "%u", &Val );
                    puSetAttribute( puGetObjectFromCollection( g_pCol, CS_LOG_CB ), PUA_CHECKBOX_CHECKED, ( Val ? TRUE : FALSE ) );
                    break;

                case CFG_ALERTITEM:
                    sscanf( Value, "%u", &Val );
                    puSetAttribute( puGetObjectFromCollection( g_pCol, CS_ALERTITEM_CB ), PUA_CHECKBOX_CHECKED, ( Val ? TRUE : FALSE ) );
                    break;

                case CFG_ALERTLOC:
                    sscanf( Value, "%u", &Val );
                    puSetAttribute( puGetObjectFromCollection( g_pCol, CS_ALERTLOC_CB ), PUA_CHECKBOX_CHECKED, ( Val ? TRUE : FALSE ) );
                    break;

                case CFG_ALERTTYPE:
                    sscanf( Value, "%u", &Val );
                    puSetAttribute( puGetObjectFromCollection( g_pCol, CS_ALERTTYPE_CB ), PUA_CHECKBOX_CHECKED, ( Val ? TRUE : FALSE ) );
                    break;

                case CFG_BUYINGAGENTMAXTRIES:
                    sscanf( Value, "%d", &Val );
                    puSetAttribute( puGetObjectFromCollection( g_pCol, CS_BUYINGAGENTTRIES ), PUA_TEXTENTRY_VALUE, Val );
                    break;

                case CFG_BUYINGAGENTHIDE:
                    sscanf( Value, "%u", &Val );
                    puSetAttribute( puGetObjectFromCollection( g_pCol, CS_BUYINGAGENTFOLD ), PUA_FOLD_FOLDED, Val );
                    break;

                case CFG_MISSIONTYPES:
                    sscanf( Value, "%u", &Val );
                    puSetAttribute( puGetObjectFromCollection( g_pCol, CS_TYPEREPAIR_CB ), PUA_CHECKBOX_CHECKED, ( Val & 0x01 ? TRUE : FALSE ) );
                    puSetAttribute( puGetObjectFromCollection( g_pCol, CS_TYPERETURN_CB ), PUA_CHECKBOX_CHECKED, ( Val & 0x02 ? TRUE : FALSE ) );
                    puSetAttribute( puGetObjectFromCollection( g_pCol, CS_TYPEFINDP_CB ), PUA_CHECKBOX_CHECKED, ( Val & 0x04 ? TRUE : FALSE ) );
                    puSetAttribute( puGetObjectFromCollection( g_pCol, CS_TYPEFINDI_CB ), PUA_CHECKBOX_CHECKED, ( Val & 0x08 ? TRUE : FALSE ) );
                    puSetAttribute( puGetObjectFromCollection( g_pCol, CS_TYPEASS_CB ), PUA_CHECKBOX_CHECKED, ( Val & 0x10 ? TRUE : FALSE ) );
                    break;

                case CFG_HIGHLIGHTOPTS:
                    sscanf( Value, "%u", &Val );
                    puSetAttribute( puGetObjectFromCollection( g_pCol, CS_HIGHLIGHTITEM_CB ), PUA_CHECKBOX_CHECKED, ( Val & 0x01 ? TRUE : FALSE ) );
                    puSetAttribute( puGetObjectFromCollection( g_pCol, CS_HIGHLIGHTLOC_CB ), PUA_CHECKBOX_CHECKED, ( Val & 0x02 ? TRUE : FALSE ) );
                    puSetAttribute( puGetObjectFromCollection( g_pCol, CS_HIGHLIGHTTYPE_CB ), PUA_CHECKBOX_CHECKED, ( Val & 0x04 ? TRUE : FALSE ) );
                    break;

                case CFG_SLIDER_EASY_HARD:
                case CFG_SLIDER_GOOD_BAD:
                case CFG_SLIDER_ORDER_CHAOS:
                case CFG_SLIDER_OPEN_HIDDEN:
                case CFG_SLIDER_PHYS_MYST:
                case CFG_SLIDER_HEADON_STEALTH:
                case CFG_SLIDER_MONEY_XP:
                    sscanf( Value, "%u", &Val );
                    puSetAttribute( puGetObjectFromCollection( g_pCol, CS_SLIDER_EASY_HARD + ( Id - CFG_SLIDER_EASY_HARD ) ), PUA_TEXTENTRY_VALUE, Val );
                    break;

                case CFG_BUYMOD:
                    sscanf( Value, "%u", &Val );
                    puSetAttribute( puGetObjectFromCollection( g_pCol, CS_ITEMVALUE_BUYMOD ), PUA_TEXTENTRY_VALUE, Val );
                    break;

                case CFG_ITEMVALUE:
                {
                    PUU32 a, b, c, d;
                    sscanf( Value, "%u::%u::%u::%u", &a, &b, &c, &d );
                    puSetAttribute( puGetObjectFromCollection( g_pCol, CS_ITEMVALUE_SINGLE ), PUA_TEXTENTRY_VALUE, a );
                    puSetAttribute( puGetObjectFromCollection( g_pCol, CS_ITEMVALUE_TOTAL ), PUA_TEXTENTRY_VALUE, b );
                    puSetAttribute( puGetObjectFromCollection( g_pCol, CS_ITEMVALUE_MSINGLE ), PUA_CHECKBOX_CHECKED, c );
                    puSetAttribute( puGetObjectFromCollection( g_pCol, CS_ITEMVALUE_MTOTAL ), PUA_CHECKBOX_CHECKED, d );
                }
                break;
                }
            }
            break;

        case ISM_ITEMWATCH:
        case ISM_LOCWATCH:

            pString = buffer + strlen( buffer );

            while( pString > buffer )
            {
                c = *--pString;
                if( c != ' ' && c != '\t' && c != '\n' )
                {
                    break;
                }
            }

            *( pString + 1 ) = 0;

            // Strip leading spaces/tab
            pString = buffer;

            while( c = *pString++ )
            {
                if( c != ' ' && c != '\t' )
                {
                    break;
                }
            }

            pString--;

            // If the resulting string isn't empty, add it to the list
            if( *pString )
            {
                puDoMethod( ( mode == ISM_ITEMWATCH ? g_ItemWatchList : g_LocWatchList ), PUM_TABLE_NEWRECORD, 0, 0 );
                puDoMethod( ( mode == ISM_ITEMWATCH ? g_ItemWatchList : g_LocWatchList ), PUM_TABLE_ADDRECORD, 0, 0 );
                puDoMethod( ( mode == ISM_ITEMWATCH ? g_ItemWatchList : g_LocWatchList ), PUM_TABLE_SETFIELDVAL, (PUU32)pString, 0 );
            }
            break;

        }
    }
}


void ExportSettings( char* filename )
{
    FILE* fp;
    pusRect Rect;
    PUU32 Record;
    PUU8* pString;
    unsigned int Val = 0;
    char* myfilename;

    myfilename = malloc( strlen( filename ) + 5 );
    strcpy( myfilename, filename );

    if( !strstr( myfilename, ".cs" ) )
    {
        strcat( myfilename, ".cs" );
    }

    if( !( fp = fopen( myfilename, "w" ) ) )
    {
        free( myfilename );
        return;
    }
    free( myfilename );
    fprintf( fp, "::Config::\n" );
    fprintf( fp, "AODIR::%s\n", g_AODir );

    puDoMethod( g_MainWin, PUM_WINDOW_GETRECT, (PUU32)&Rect, 0 );
    fprintf( fp, "WINDOWX::%d\nWINDOWY::%d\nWINDOWWIDTH::%d\n", Rect.X, Rect.Y, Rect.Width );

    fprintf( fp, "STARTMINIMIZED::%u\n",
             puGetAttribute( puGetObjectFromCollection( g_pCol, CS_STARTMIN_CB ), PUA_CHECKBOX_CHECKED ) );

    fprintf( fp, "WATCHMSGBOX::%u\n",
             puGetAttribute( puGetObjectFromCollection( g_pCol, CS_MSGBOX_CB ), PUA_CHECKBOX_CHECKED ) );

    fprintf( fp, "BUYINGAGENTSHOWHELP::%u\n",
             puGetAttribute( puGetObjectFromCollection( g_pCol, CS_BAINFO_CB ), PUA_CHECKBOX_CHECKED ) );

    fprintf( fp, "SOUNDS::%u\n",
             puGetAttribute( puGetObjectFromCollection( g_pCol, CS_SOUNDS_CB ), PUA_CHECKBOX_CHECKED ) );

    fprintf( fp, "EXPAND::%u\n",
             puGetAttribute( puGetObjectFromCollection( g_pCol, CS_EXPAND_CB ), PUA_CHECKBOX_CHECKED ) );

    fprintf( fp, "MOUSEMOVE::%u\n",
             puGetAttribute( puGetObjectFromCollection( g_pCol, CS_MOUSEMOVE_CB ), PUA_CHECKBOX_CHECKED ) );

    fprintf( fp, "LOG::%u\n",
             puGetAttribute( puGetObjectFromCollection( g_pCol, CS_LOG_CB ), PUA_CHECKBOX_CHECKED ) );

    fprintf( fp, "ALERTITEM::%u\n",
             puGetAttribute( puGetObjectFromCollection( g_pCol, CS_ALERTITEM_CB ), PUA_CHECKBOX_CHECKED ) );

    fprintf( fp, "ALERTLOC::%u\n",
             puGetAttribute( puGetObjectFromCollection( g_pCol, CS_ALERTLOC_CB ), PUA_CHECKBOX_CHECKED ) );

    fprintf( fp, "ALERTTYPE::%u\n",
             puGetAttribute( puGetObjectFromCollection( g_pCol, CS_ALERTTYPE_CB ), PUA_CHECKBOX_CHECKED ) );

    fprintf( fp, "BUYINGAGENTMAXTRIES::%u\n",
             puGetAttribute( puGetObjectFromCollection( g_pCol, CS_BUYINGAGENTTRIES ), PUA_TEXTENTRY_VALUE ) );

    fprintf( fp, "BUYINGAGENTHIDE::%u\n",
             puGetAttribute( puGetObjectFromCollection( g_pCol, CS_BUYINGAGENTFOLD ), PUA_FOLD_FOLDED ) );

    if( puGetAttribute( puGetObjectFromCollection( g_pCol, CS_TYPEREPAIR_CB ), PUA_CHECKBOX_CHECKED ) ) Val |= 0x01;
    if( puGetAttribute( puGetObjectFromCollection( g_pCol, CS_TYPERETURN_CB ), PUA_CHECKBOX_CHECKED ) ) Val |= 0x02;
    if( puGetAttribute( puGetObjectFromCollection( g_pCol, CS_TYPEFINDP_CB ), PUA_CHECKBOX_CHECKED ) ) Val |= 0x04;
    if( puGetAttribute( puGetObjectFromCollection( g_pCol, CS_TYPEFINDI_CB ), PUA_CHECKBOX_CHECKED ) ) Val |= 0x08;
    if( puGetAttribute( puGetObjectFromCollection( g_pCol, CS_TYPEASS_CB ), PUA_CHECKBOX_CHECKED ) ) Val |= 0x10;

    fprintf( fp, "MISHTYPES::%u\n", Val );

    Val = 0;
    if( puGetAttribute( puGetObjectFromCollection( g_pCol, CS_HIGHLIGHTITEM_CB ), PUA_CHECKBOX_CHECKED ) ) Val |= 0x01;
    if( puGetAttribute( puGetObjectFromCollection( g_pCol, CS_HIGHLIGHTLOC_CB ), PUA_CHECKBOX_CHECKED ) ) Val |= 0x02;
    if( puGetAttribute( puGetObjectFromCollection( g_pCol, CS_HIGHLIGHTTYPE_CB ), PUA_CHECKBOX_CHECKED ) ) Val |= 0x04;

    fprintf( fp, "HIGHLIGHTOPTS::%u\n", Val );

    fprintf( fp, "SLIDER_EASY_HARD::%u\n",
             puGetAttribute( puGetObjectFromCollection( g_pCol, CS_SLIDER_EASY_HARD ), PUA_TEXTENTRY_VALUE ) );

    fprintf( fp, "SLIDER_GOOD_BAD::%u\n",
             puGetAttribute( puGetObjectFromCollection( g_pCol, CS_SLIDER_GOOD_BAD ), PUA_TEXTENTRY_VALUE ) );

    fprintf( fp, "SLIDER_ORDER_CHAOS::%u\n",
             puGetAttribute( puGetObjectFromCollection( g_pCol, CS_SLIDER_ORDER_CHAOS ), PUA_TEXTENTRY_VALUE ) );

    fprintf( fp, "SLIDER_OPEN_HIDDEN::%u\n",
             puGetAttribute( puGetObjectFromCollection( g_pCol, CS_SLIDER_OPEN_HIDDEN ), PUA_TEXTENTRY_VALUE ) );

    fprintf( fp, "SLIDER_PHYS_MYST::%u\n",
             puGetAttribute( puGetObjectFromCollection( g_pCol, CS_SLIDER_PHYS_MYST ), PUA_TEXTENTRY_VALUE ) );

    fprintf( fp, "SLIDER_HEADON_STEALTH::%u\n",
             puGetAttribute( puGetObjectFromCollection( g_pCol, CS_SLIDER_HEADON_STEALTH ), PUA_TEXTENTRY_VALUE ) );

    fprintf( fp, "SLIDER_MONEY_XP::%u\n",
             puGetAttribute( puGetObjectFromCollection( g_pCol, CS_SLIDER_MONEY_XP ), PUA_TEXTENTRY_VALUE ) );

    fprintf( fp, "BUYMOD::%u\n",
             puGetAttribute( puGetObjectFromCollection( g_pCol, CS_ITEMVALUE_BUYMOD ), PUA_TEXTENTRY_VALUE ) );

    fprintf( fp, "ITEMVALUE::%u::%u::%u::%u\n",
             puGetAttribute( puGetObjectFromCollection( g_pCol, CS_ITEMVALUE_SINGLE ), PUA_TEXTENTRY_VALUE ),
             puGetAttribute( puGetObjectFromCollection( g_pCol, CS_ITEMVALUE_TOTAL ), PUA_TEXTENTRY_VALUE ),
             puGetAttribute( puGetObjectFromCollection( g_pCol, CS_ITEMVALUE_MSINGLE ), PUA_CHECKBOX_CHECKED ),
             puGetAttribute( puGetObjectFromCollection( g_pCol, CS_ITEMVALUE_MTOTAL ), PUA_CHECKBOX_CHECKED ) );

    fprintf( fp, "::ItemWatch::\n" );
    Record = puDoMethod( g_ItemWatchList, PUM_TABLE_GETFIRSTRECORD, 0, 0 );
    while( Record )
    {
        if( pString = (PUU8*)puDoMethod( g_ItemWatchList, PUM_TABLE_GETFIELDVAL, Record, 0 ) )
        {
            fprintf( fp, "%s\n", pString );
        }

        Record = puDoMethod( g_ItemWatchList, PUM_TABLE_GETNEXTRECORD, Record, 0 );
    }
    fprintf( fp, "::LocWatch::\n" );
    Record = puDoMethod( g_LocWatchList, PUM_TABLE_GETFIRSTRECORD, 0, 0 );
    while( Record )
    {
        if( pString = (PUU8*)puDoMethod( g_LocWatchList, PUM_TABLE_GETFIELDVAL, Record, 0 ) )
        {
            fprintf( fp, "%s\n", pString );
        }

        Record = puDoMethod( g_LocWatchList, PUM_TABLE_GETNEXTRECORD, Record, 0 );
    }
    fprintf( fp, "::END::\n" );

    fclose( fp );
}


void DisplayErrorMessage( PUU8* _pMessage, PUU32 _bAsynchronous )
{
    puSetAttribute( puGetObjectFromCollection( g_pCol, CS_ERROR_TEXT ), PUA_TEXT_STRING, (PUU32)_pMessage );
    puSetAttribute( puGetObjectFromCollection( g_pCol, CS_ERROR_WINDOW ), PUA_WINDOW_OPENED, TRUE );

    if( !_bAsynchronous )
    {
        puWaitAppMessages();
    }
}

/* Prompt user for folder
   (from AOMD)
*/
void GetFolder( HWND hWndOwner, char *strTitle, char *strPath )
{
    BROWSEINFO udtBI;
    ITEMIDLIST *udtIDList;

    /* Initialise */
    udtBI.hwndOwner = hWndOwner;
    udtBI.pidlRoot = NULL;
    udtBI.pszDisplayName = NULL;
    udtBI.lpszTitle = strTitle;
    udtBI.ulFlags = BIF_RETURNONLYFSDIRS;
    udtBI.lpfn = NULL;
    udtBI.lParam = 0;
    udtBI.iImage = 0;

    /* Prompt user for folder */
    udtIDList = SHBrowseForFolder( &udtBI );

    /* Extract pathname */
    if( !SHGetPathFromIDList( udtIDList, strPath ) )
    {
        strPath[ 0 ] = 0; // Zero-length if failure
    }
}


BOOL GetFile( HWND hWndOwner, BOOL saving, char* buffer, int buffersize )
{
    OPENFILENAME ofn;

    ZeroMemory( &ofn, sizeof( ofn ) );

    /* Initialise */
    ofn.hwndOwner = hWndOwner;
    ofn.lStructSize = sizeof( OPENFILENAME );
    if( saving )
    {
        ofn.Flags = OFN_HIDEREADONLY;
    }
    else
    {
        ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
    }

    ofn.lpstrFilter = "Clicksaver Files\0*.CS\0";
    ofn.lpstrFile = buffer;
    ofn.lpstrFile[ 0 ] = '\0';
    ofn.nMaxFile = buffersize;
    ofn.nFilterIndex = 0;
    ofn.lpstrInitialDir = ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;

    /* Prompt user for folder */
    if( saving )
    {
        return GetSaveFileName( &ofn );
    }
    else
    {
        return GetOpenFileName( &ofn );
    }

    return FALSE;
}


// Generate a mouse movement and button click sequence
// to make AO generate new missions.
// All coordinates are hardcoded because I'm too
// lazy to make them configurable.
// However, it's not that much of a problem, since
// I use coordinates relative to AO window.
// So, it will work regardless of where the AO window is,
// and with the mission window in AO snapped to the
// upper left corner.
int BuyingAgent()
{
    HWND AOWnd, BAWnd;
    POINT MousePos;
    LPARAM lParam;

    // Find AO window
    if( !( AOWnd = FindWindow( "Anarchy client", NULL ) ) )
    {
        DisplayErrorMessage( "Anarchy Online is not running.", TRUE );
        g_BuyingAgentCount = 0;
        g_BuyingAgentMissions = 0;
        return FALSE;
    }

    // Close main window
    if( !g_bFullscreen )
    {
        puSetAttribute( g_MainWin, PUA_WINDOW_OPENED, FALSE );

        // Open buying agent window
        puSetAttribute( puGetObjectFromCollection( g_pCol, CS_BUYINGAGENT_WINDOW ), PUA_WINDOW_OPENED, TRUE );

        // Set keyboard focus on buying agent window
        BAWnd = (HWND)puGetAttribute( puGetObjectFromCollection( g_pCol, CS_BUYINGAGENT_WINDOW ), PUA_WINDOW_HANDLE );
        SetFocus( BAWnd );
    }

    // Delay
    //Sleep( puGetAttribute( puGetObjectFromCollection( g_pCol, CS_BUYINGAGENTDELAY ), PUA_TEXTENTRY_VALUE ) );
    Sleep( 2010 );

    // Force AO on top
    SetForegroundWindow( AOWnd );

    // Now we have the absolute position of the upper-left corner
    // of AO display area on screen. We can now use that
    // as a basis to generate mouse positions relative to
    // the AO window.

    // Click on "request mission"

    // Now that we don't have to move the cursor
    // around to ungray the request button, we
    // move the mouse only once, to make it
    // easy to abort the buying agent while
    // it's running
    MousePos.x = 99;
    MousePos.y = 180;
    lParam = MousePos.y << 16 | MousePos.x;

    if( g_bFirstRound )
    {
        ClientToScreen( AOWnd, &MousePos );
        SetCursorPos( MousePos.x, MousePos.y );
        g_bFirstRound = FALSE;
    }

    SendMessage( AOWnd, WM_LBUTTONDOWN, 0, lParam );
    SendMessage( AOWnd, WM_LBUTTONUP, 0, lParam );

    return TRUE;
}


void EndBuyingAgent()
{

    if( !g_bFullscreen )
    {
        // Remove keyboard focus
        SetFocus( NULL );

        // Close buying agent window
        puSetAttribute( puGetObjectFromCollection( g_pCol, CS_BUYINGAGENT_WINDOW ), PUA_WINDOW_OPENED, FALSE );

        // Open main window
        puSetAttribute( g_MainWin, PUA_WINDOW_OPENED, TRUE );
    }

}


void DebugPacket( void* pData, unsigned int length )
{
    unsigned int x;
    unsigned char *data = (char *)pData;
    char ps[ 70 ];
    for( x = 0; x < length; x++ )
    {
        sprintf( &( ps[ x % 16 * 3 ] ), "%02X", data[ x ] );
        ps[ x % 16 * 3 + 2 ] = ' ';
        ps[ x % 16 + 48 ] = ( data[ x ] >= 32 && data[ x ] <= 127 ? data[ x ] : '.' );
        ps[ x % 16 + 49 ] = '\n';
        ps[ x % 16 + 50 ] = 0;
        if( x % 16 == 15 ) WriteDebug( ps );
    }

    if( x % 16 != 0 )
    {
        for( x = x % 16; x < 16; x++ )
        {
            sprintf( &( ps[ x % 16 * 3 ] ), "  " );
            ps[ x % 16 * 3 + 2 ] = ' ';
        }
        WriteDebug( ps );
    }
}


void WriteLog( const char* Format, ... )
{
    /**/
    va_list argptr;
    static FILE *fp = NULL;
    if( Format == NULL )
    {
        if( fp )
        {
            fclose( fp );
            fp = NULL;
        }
        return;
    }
    if( PUL_GET_CB( CS_LOG_CB ) )
    {
        if( !fp )
        {
            fp = fopen( "clicksaver.log", "a" );
        }
        va_start( argptr, Format );
        vfprintf( fp, Format, argptr );
        va_end( argptr );
    }
    /**/
}


void WriteDebug( const char* txt )
{
#ifdef _DEBUG
    static FILE *fp = NULL;
    if( txt == NULL )
    {
        if( fp )
        {
            fclose( fp );
            fp = NULL;
        }
        return;
    }
    if( !fp )
    {
        fp = fopen( "clicksaver.debug", "a" );
    }
    fprintf( fp, "%s", txt );
#endif // _DEBUG
}


//slider setting functions

void _dragMouse( int x0, int y0, int x1, int y1 )
{
    POINT MousePos;
    LPARAM lParam;
    HWND AOWnd;

    // Find AO window
    if( !( AOWnd = FindWindow( "Anarchy client", NULL ) ) )
    {
        DisplayErrorMessage( "Anarchy Online is not running.", TRUE );
        g_BuyingAgentCount = 0;
        g_BuyingAgentMissions = 0;
        return;
    }
    MousePos.x = x0;
    MousePos.y = y0;
    lParam = MousePos.y << 16 | MousePos.x;
    ClientToScreen( AOWnd, &MousePos );
    SetCursorPos( MousePos.x, MousePos.y );
    SendMessage( AOWnd, WM_LBUTTONDOWN, 0, lParam );
    Sleep( 250 );
    MousePos.x = x1;
    MousePos.y = y1;
    lParam = MousePos.y << 16 | MousePos.x;
    ClientToScreen( AOWnd, &MousePos );
    SetCursorPos( MousePos.x, MousePos.y );
    SendMessage( AOWnd, WM_MOUSEMOVE, 0, lParam );
    Sleep( 250 );
    SendMessage( AOWnd, WM_LBUTTONUP, 0, lParam );
    Sleep( 250 );
}


/*
these coords are from my initial observation with a macro program. they are ofset slightly.
; options button
; 200, 185
; difficulty slider
; 110, 165
; 1st slider
; 110, 210
; then add 15 pixels in Y for each subsequent row
; full left slider
; X = 60
; full right slider
; X = 170
*/


float _linIinterp( float lo, float hi, float ratio )
{
    return ( hi - lo )*ratio + lo;
}


void _setSliders( int easy_hard, int good_bad, int order_chaos, int open_hidden, int phys_myst, int headon_stealth, int money_xp )
{
    int ypos = 210;

    //_dragMouse(200, 165, 200, 165);
    if( easy_hard != 50 ) _dragMouse( 102, 160, (int)_linIinterp( 64, 141, easy_hard / 100.0f ), 160 );
    if( good_bad != 50 ) _dragMouse( 102, ypos, (int)_linIinterp( 64, 141, good_bad / 100.0f ), ypos );
    ypos += 18;
    if( order_chaos != 50 ) _dragMouse( 102, ypos, (int)_linIinterp( 64, 141, order_chaos / 100.0f ), ypos );
    ypos += 18;
    if( open_hidden != 50 ) _dragMouse( 102, ypos, (int)_linIinterp( 64, 141, open_hidden / 100.0f ), ypos );
    ypos += 18;
    if( phys_myst != 50 ) _dragMouse( 102, ypos, (int)_linIinterp( 64, 141, phys_myst / 100.0f ), ypos );
    ypos += 18;
    if( headon_stealth != 50 ) _dragMouse( 102, ypos, (int)_linIinterp( 64, 141, headon_stealth / 100.0f ), ypos );
    ypos += 18;
    if( money_xp != 50 ) _dragMouse( 102, ypos, (int)_linIinterp( 64, 141, money_xp / 100.0f ), ypos );
}
