/*
 * $Log: clicksaver.h,v $
 * Revision 1.15  2004/12/27 17:28:13  gnarf37
 * Added Option for multiple missions, quick change
 *
 * Revision 1.14  2004/09/03 19:16:46  gnarf37
 * Version 2.3.1 AI Updates
 *
 * Revision 1.13  2004/08/28 18:04:08  gnarf37
 * Moved some GUI Options arounds, added Skip Rebuild option
 *
 * Revision 1.12  2004/01/25 19:35:53  gnarf37
 * 2.3.0 beta 3 - Shrunk Database a bit, added Item Value options, make options menu smaller a tad so that 800x600 might be able to use it again...
 *
 * Revision 1.11  2004/01/23 08:19:12  ibender
 * added mission slider settings
 *
 * Revision 1.10  2003/11/06 23:41:50  gnarf37
 * Version 2.3.0 beta 2 - Fixed issues with 15.2.0 and added an option for auto expand team missions
 *
 * Revision 1.9  2003/10/31 03:40:50  gnarf37
 * Saving/Loading Configurations
 *
 * Revision 1.8  2003/05/27 00:14:42  gnarf37
 * Added Checkbox to stop mouse movement, and cleaned up mission info parsing so it doesnt match stale missions
 *
 * Revision 1.7  2003/05/09 14:26:47  gnarf37
 * Version 2.2.9 beta 2
 *
 */
/*
ClickSaver -  Anarchy Online mission helper
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

#ifndef __CLICKSAVER_H__
#define __CLICKSAVER_H__

#define CS_VERSION "2.5.3"

#include "mission.h"

// GUI object IDs
enum
{
    CS_MAIN_WINDOW = 1,
    CS_TABS,
    CS_OPTIONS_TAB,
    CS_MISSIONS_TAB,

    CS_ITEMWATCH_TAB,
    CS_ITEMWATCH_LIST,
    CS_ITEMWATCH_LISTVIEW,

    CS_LOCWATCH_TAB,
    CS_LOCWATCH_LIST,
    CS_LOCWATCH_LISTVIEW,

    CS_TYPEWATCH_TAB,
    CS_TYPEREPAIR_CB,
    CS_TYPERETURN_CB,
    CS_TYPEFINDP_CB,
    CS_TYPEFINDI_CB,
    CS_TYPEASS_CB,

    CS_ITEMVALUES_TAB,
    CS_ITEMVALUE_BUYMOD,
    CS_ITEMVALUE_SINGLE,
    CS_ITEMVALUE_TOTAL,
    CS_ITEMVALUE_MSINGLE,
    CS_ITEMVALUE_MTOTAL,

    CS_SLIDER_TAB,
    CS_SLIDER_EASY_HARD,
    CS_SLIDER_GOOD_BAD,
    CS_SLIDER_ORDER_CHAOS,
    CS_SLIDER_OPEN_HIDDEN,
    CS_SLIDER_PHYS_MYST,
    CS_SLIDER_HEADON_STEALTH,
    CS_SLIDER_MONEY_XP,

    CS_MISSION1,
    CS_MISSION2,
    CS_MISSION3,
    CS_MISSION4,
    CS_MISSION5,

    CS_ERROR_WINDOW,
    CS_ERROR_TEXT,

    CS_STARTMIN_CB,
    CS_MSGBOX_CB,
    CS_BAINFO_CB,
    CS_BAINFO2_CB,

    CS_ALERTITEM_CB,
    CS_ALERTLOC_CB,
    CS_ALERTTYPE_CB,

    CS_SOUNDS_CB,
    CS_MOUSEMOVE_CB,
    CS_LOG_CB,
    CS_EXPAND_CB,

    CS_HIGHLIGHTITEM_CB,
    CS_HIGHLIGHTLOC_CB,
    CS_HIGHLIGHTTYPE_CB,

    CS_BUYINGAGENTFOLD,
    CS_BUYINGAGENTTRIES,
    CS_BUYINGAGENTMISH,

    CS_BUYINGAGENT_INFOWINDOW,
    CS_BUYINGAGENT_WINDOW,
    CS_FULLSCREEN_WINDOW,

    CS_WATCH_MSGBOX,

    CS_DBCOPYMSGBOX,
    CS_CREATINGDBMSGBOX,

    CS_OPTIONSFOLD3,
};

// App messages
enum
{
    CSAM_QUIT = 1,
    CSAM_SKIP,
    CSAM_OK,
    CSAM_CANCEL,
    CSAM_NEWMISSIONS,
    CSAM_PRESTARTBUYINGAGENT,
    CSAM_STARTBUYINGAGENT,
    CSAM_STOPBUYINGAGENT,
    CSAM_STARTFULLSCREEN,
    CSAM_STOPFULLSCREEN,
    CSAM_EXPORTSETTINGS,
    CSAM_IMPORTSETTINGS,

    CSAM_SET_SLIDERS
};

extern PULID g_ItemWatchList;
extern PULID g_LocWatchList;
extern PULID g_TypeWatchList;
extern pusObjectCollection* g_pCol;
extern PUU32 g_BuyingAgentCount;
extern PULID g_MainWin;
extern PUU8 g_MishNumber, g_FoundMish;
extern PUU8 g_bFullscreen;

// Endianness macros
#define EndianSwap16(x) ( ( x ) >> 8 | ( x ) << 8 )
#define EndianSwap32(x) ( ( x ) << 24 | ( ( x ) & 0xff00 ) << 8 | ( ( x ) >> 8 ) & 0xff00 | ( x ) >> 24 )

// PUL MACROS
#define PUL_GET_CB(x) puGetAttribute( puGetObjectFromCollection( g_pCol, (x) ), PUA_CHECKBOX_CHECKED )
#define PUL_SET_CB(x,y) puSetAttribute( puGetObjectFromCollection( g_pCol, (x) ), PUA_CHECKBOX_CHECKED, ( (y) ? 1 : 0) )

// Database functions
int CreateLocalDatabase();
int OpenLocalDB();
void* GetDataChunk( PUU32 _KeyHi, PUU32 _KeyLo, PUU32* _pSize );
void DebugPacket( void* pData, unsigned int length );
void WriteLog( const char* Format, ... );
void WriteDebug( const char* txt );

#endif
