/*
 * $Log: guidef.c,v $
 * Revision 1.20  2004/12/27 17:28:13  gnarf37
 * Added Option for multiple missions, quick change
 *
 * Revision 1.19  2004/09/03 19:16:46  gnarf37
 * Version 2.3.1 AI Updates
 *
 * Revision 1.18  2004/08/28 18:04:08  gnarf37
 * Moved some GUI Options arounds, added Skip Rebuild option
 *
 * Revision 1.17  2004/02/22 05:41:13  gnarf37
 * Added Set Sliders button to the main window
 *
 * Revision 1.16  2004/01/25 19:36:05  gnarf37
 * 2.3.0 beta 3 - Shrunk Database a bit, added Item Value options, make options menu smaller a tad so that 800x600 might be able to use it again...
 *
 * Revision 1.15  2004/01/25 06:59:57  gnarf37
 * Added Folding Tabs to the options page to make CS fit in 800x600 again...
 *
 * Revision 1.14  2004/01/23 08:19:26  ibender
 * added mission slider settings
 *
 * Revision 1.13  2003/11/06 23:41:50  gnarf37
 * Version 2.3.0 beta 2 - Fixed issues with 15.2.0 and added an option for auto expand team missions
 *
 * Revision 1.12  2003/11/06 19:27:22  gnarf37
 * Changed URL
 *
 * Revision 1.11  2003/10/31 03:40:50  gnarf37
 * Saving/Loading Configurations
 *
 * Revision 1.10  2003/05/27 00:14:42  gnarf37
 * Added Checkbox to stop mouse movement, and cleaned up mission info parsing so it doesnt match stale missions
 *
 * Revision 1.9  2003/05/09 14:26:47  gnarf37
 * Version 2.2.9 beta 2
 *
 * Revision 1.8  2003/05/08 09:11:09  gnarf37
 * Fullscreen Mode
 *
 * Revision 1.7  2003/05/08 08:40:04  gnarf37
 * Added Logging to Missions
 *
 * Revision 1.6  2003/05/08 07:36:55  gnarf37
 * Added Sounds
 *
 * Revision 1.5  2003/05/07 14:04:41  gnarf37
 * Added Log commet at the top.
 *
 */
/*
ClickSaver GUI definitions -  Anarchy Online mission helper
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
#include "clicksaver.h"


PUU32 g_GUIDef[]=
{
// **** Mission tab ****
    PU_ACTION_OBJDEF, CS_MISSIONS_TAB, ( PUU32 )"VerGroup", 0,

        PUM_ADDCHILD, PU_ACTION_OBJDEF, CS_MISSION1, ( PUU32 )"CSMission",
            CSA_MISSION_TITLE, ( PUU32 )"Mission 1", 0, 0,
        PUM_ADDCHILD, PU_ACTION_OBJDEF, CS_MISSION2, ( PUU32 )"CSMission",
            CSA_MISSION_TITLE, ( PUU32 )"Mission 2", 0, 0,
        PUM_ADDCHILD, PU_ACTION_OBJDEF, CS_MISSION3, ( PUU32 )"CSMission",
            CSA_MISSION_TITLE, ( PUU32 )"Mission 3", 0, 0,
        PUM_ADDCHILD, PU_ACTION_OBJDEF, CS_MISSION4, ( PUU32 )"CSMission",
            CSA_MISSION_TITLE, ( PUU32 )"Mission 4", 0, 0,
        PUM_ADDCHILD, PU_ACTION_OBJDEF, CS_MISSION5, ( PUU32 )"CSMission",
            CSA_MISSION_TITLE, ( PUU32 )"Mission 5", 0, 0,

        PUM_ADDCHILD, PU_TITLE( NULL ),

        PUM_ADDCHILD, PU_ACTION_OBJDEF, CS_BUYINGAGENTFOLD, ( PUU32 )"Fold",
                PUA_FOLD_LABEL, ( PUU32 )"Buying agent",

            PUA_FOLD_CONTENTS, PU_HORGROUP,
                PUM_ADDCHILD, PU_LABEL( "Maximum tries:" ),
                PUM_ADDCHILD, PU_ACTION_OBJDEF, CS_BUYINGAGENTTRIES, ( PUU32 )"TextEntry",
                    PUA_CONTROL_WEIGHT, 1,
                    PUA_TEXTENTRY_NUMERIC, TRUE,
                    PUA_TEXTENTRY_MIN, 1,
                    PUA_TEXTENTRY_MAX, 10000,
                    PUA_TEXTENTRY_BUFFERSIZE, 5,
                    PUA_TEXTENTRY_VALUE, 10,
                0, 0,
                PUM_ADDCHILD, PU_LABEL( "# of Mish:" ),
                PUM_ADDCHILD, PU_ACTION_OBJDEF, CS_BUYINGAGENTMISH, ( PUU32 )"TextEntry",
                    PUA_CONTROL_WEIGHT, 1,
                    PUA_TEXTENTRY_NUMERIC, TRUE,
                    PUA_TEXTENTRY_MIN, 1,
                    PUA_TEXTENTRY_MAX, 30,
                    PUA_TEXTENTRY_BUFFERSIZE, 5,
                    PUA_TEXTENTRY_VALUE, 1,
                0, 0,
                    PUM_ADDCHILD, PU_ACTION_OBJDEF, 0, ( PUU32 )"Text",
                        PUA_TEXT_STRING, ( PUU32 )"Start Buying Agent",
                        PUA_CONTROL_FRAME, PUFRAME_BUTTON,
                        PUA_CONTROL_ISBUTTON, TRUE, 0,
                        PUM_ADDNOTIFICATION,
                            PUNOTIFY_CONDITION_ATTREQUALS, PUA_CONTROL_CLICKED, TRUE,
                            PUNOTIFY_ACTION_APPMSG, CSAM_PRESTARTBUYINGAGENT,
                        0,

            PU_ENDGROUP,
        0, 0,
        PUM_ADDCHILD, PU_HORGROUP,
            PUM_ADDCHILD, PU_ACTION_OBJDEF, 0, ( PUU32 )"Text",
                PUA_TEXT_STRING, ( PUU32 )"Start Fullscreen Mode",
                PUA_CONTROL_FRAME, PUFRAME_BUTTON,
                PUA_CONTROL_ISBUTTON, TRUE, 0,
                PUM_ADDNOTIFICATION,
                    PUNOTIFY_CONDITION_ATTREQUALS, PUA_CONTROL_CLICKED, TRUE,
                    PUNOTIFY_ACTION_APPMSG, CSAM_STARTFULLSCREEN,
                0,
            PUM_ADDCHILD, PU_ACTION_OBJDEF, 0, ( PUU32 )"Text",
                PUA_TEXT_STRING, ( PUU32 )"Set Sliders Now",
                PUA_CONTROL_FRAME, PUFRAME_BUTTON,
                PUA_CONTROL_ISBUTTON, TRUE, 0,
                PUM_ADDNOTIFICATION,
                    PUNOTIFY_CONDITION_ATTREQUALS, PUA_CONTROL_CLICKED, TRUE,
                    PUNOTIFY_ACTION_APPMSG, CSAM_SET_SLIDERS,
                0,
        PU_ENDGROUP,

    PU_ENDGROUP,
// ****

// **** Item list tab ****
    PU_ACTION_OBJDEF, CS_ITEMWATCH_TAB, ( PUU32 )"VerGroup", 0,
        PUM_ADDCHILD, PU_ACTION_OBJDEF, CS_ITEMWATCH_LISTVIEW, ( PUU32 )"ListView",
            PUA_CONTROL_HASKEYBOARDFOCUS, TRUE,
            PUA_LISTVIEW_EDITABLE, TRUE,
            PUA_LISTVIEW_TABLE,
                PU_ACTION_OBJDEF, CS_ITEMWATCH_LIST, ( PUU32 )"List",
                    PUA_TABLE_NUMFIELDS, 1, 0,
                    PUM_TABLE_SETFIELDTYPE, PUFIELD_TYPE_STRINGCOPY, 0,
            0, 0,

            PUM_ADDCHILD, PU_HORGROUP,
                PUM_ADDCHILD, PU_ACTION_OBJDEF, 0, ( PUU32 )"Text", PUA_TEXT_STRING, ( PUU32 )"Add",
                    PUA_CONTROL_FRAME, PUFRAME_BUTTON,
                    PUA_CONTROL_ISBUTTON, TRUE, 0,
                    PUM_ADDNOTIFICATION,
                        PUNOTIFY_CONDITION_ATTREQUALS, PUA_CONTROL_CLICKED, TRUE,
                        PUNOTIFY_ACTION_DOMETHOD, CS_ITEMWATCH_LISTVIEW, PUM_LISTVIEW_ADD,
                    0,
                PUM_ADDCHILD, PU_ACTION_OBJDEF, 0, ( PUU32 )"Text", PUA_TEXT_STRING, ( PUU32 )"Remove",
                    PUA_CONTROL_FRAME, PUFRAME_BUTTON,
                    PUA_CONTROL_ISBUTTON, TRUE, 0,
                    PUM_ADDNOTIFICATION,
                        PUNOTIFY_CONDITION_ATTREQUALS, PUA_CONTROL_CLICKED, TRUE,
                        PUNOTIFY_ACTION_DOMETHOD, CS_ITEMWATCH_LISTVIEW, PUM_LISTVIEW_REMOVE,
                    0,
            PU_ENDGROUP,
            PUM_ADDCHILD, PU_TITLE( "Seach Options" ),
            PUM_ADDCHILD, PU_HORGROUP,
                PUM_ADDCHILD, PU_CHECKBOX( CS_ALERTITEM_CB, "Match in Buying Agent  /  " ),
                PUM_ADDCHILD, PU_SET_CHECKBOX( CS_HIGHLIGHTITEM_CB, "Highlight Matches" ),
            PU_ENDGROUP,
    PU_ENDGROUP,
// ****

// **** Location list tab ****
    PU_ACTION_OBJDEF, CS_LOCWATCH_TAB, ( PUU32 )"VerGroup", 0,
        PUM_ADDCHILD, PU_ACTION_OBJDEF, CS_LOCWATCH_LISTVIEW, ( PUU32 )"ListView",
            PUA_CONTROL_HASKEYBOARDFOCUS, TRUE,
            PUA_LISTVIEW_EDITABLE, TRUE,
            PUA_LISTVIEW_TABLE,
                PU_ACTION_OBJDEF, CS_LOCWATCH_LIST, ( PUU32 )"List",
                    PUA_TABLE_NUMFIELDS, 1, 0,
                    PUM_TABLE_SETFIELDTYPE, PUFIELD_TYPE_STRINGCOPY, 0,
            0, 0,

            PUM_ADDCHILD, PU_HORGROUP,
                PUM_ADDCHILD, PU_ACTION_OBJDEF, 0, ( PUU32 )"Text", PUA_TEXT_STRING, ( PUU32 )"Add",
                    PUA_CONTROL_FRAME, PUFRAME_BUTTON,
                    PUA_CONTROL_ISBUTTON, TRUE, 0,
                    PUM_ADDNOTIFICATION,
                        PUNOTIFY_CONDITION_ATTREQUALS, PUA_CONTROL_CLICKED, TRUE,
                        PUNOTIFY_ACTION_DOMETHOD, CS_LOCWATCH_LISTVIEW, PUM_LISTVIEW_ADD,
                    0,
                PUM_ADDCHILD, PU_ACTION_OBJDEF, 0, ( PUU32 )"Text", PUA_TEXT_STRING, ( PUU32 )"Remove",
                    PUA_CONTROL_FRAME, PUFRAME_BUTTON,
                    PUA_CONTROL_ISBUTTON, TRUE, 0,
                    PUM_ADDNOTIFICATION,
                        PUNOTIFY_CONDITION_ATTREQUALS, PUA_CONTROL_CLICKED, TRUE,
                        PUNOTIFY_ACTION_DOMETHOD, CS_LOCWATCH_LISTVIEW, PUM_LISTVIEW_REMOVE,
                    0,
            PU_ENDGROUP,
            PUM_ADDCHILD, PU_TITLE( "Seach Options" ),
            PUM_ADDCHILD, PU_HORGROUP,
                PUM_ADDCHILD, PU_CHECKBOX( CS_ALERTLOC_CB, "Match in Buying Agent  /  " ),
                PUM_ADDCHILD, PU_SET_CHECKBOX( CS_HIGHLIGHTLOC_CB, "Highlight Matches" ),
            PU_ENDGROUP,

    PU_ENDGROUP,
// ****

// **** Mission Type list tab ****
    PU_ACTION_OBJDEF, CS_TYPEWATCH_TAB, ( PUU32 )"VerGroup", 0,
        PUM_ADDCHILD, PU_LABEL( "Choose Mission types you want highlighted" ),
        PUM_ADDCHILD, PU_LABEL( " " ),
        PUM_ADDCHILD, PU_LABEL( "Added by Gnarf37" ),
        PUM_ADDCHILD, PU_TITLE( "Mission Types" ),
        PUM_ADDCHILD, PU_LEFT_VERGROUP,
            PUM_ADDCHILD, PU_SET_CHECKBOX( CS_TYPEREPAIR_CB, "Repair" ),
            PUM_ADDCHILD, PU_SET_CHECKBOX( CS_TYPERETURN_CB, "Return Item" ),
            PUM_ADDCHILD, PU_SET_CHECKBOX( CS_TYPEFINDP_CB, "Find Person" ),
            PUM_ADDCHILD, PU_SET_CHECKBOX( CS_TYPEFINDI_CB, "Find Item" ),
            PUM_ADDCHILD, PU_SET_CHECKBOX( CS_TYPEASS_CB, "Kill Person" ),
        PU_ENDGROUP,
        PUM_ADDCHILD, PU_TITLE( "Seach Options" ),
        PUM_ADDCHILD, PU_HORGROUP,
            PUM_ADDCHILD, PU_CHECKBOX( CS_ALERTTYPE_CB, "Match in Buying Agent  /  " ),
            PUM_ADDCHILD, PU_SET_CHECKBOX( CS_HIGHLIGHTTYPE_CB, "Highlight Matches" ),
        PU_ENDGROUP,
        PUM_ADDCHILD, PU_SPACER,

    PU_ENDGROUP,
// ****

// **** Item Values ****
    PU_ACTION_OBJDEF, CS_ITEMVALUES_TAB, ( PUU32 )"VerGroup", 0,
        PUM_ADDCHILD, PU_LABEL( "Item Values" ),
        PUM_ADDCHILD, PU_LABEL( " " ),
        PUM_ADDCHILD, PU_LABEL( "Added by Gnarf37 with help from Jayde" ),
        PUM_ADDCHILD, PU_TITLE( "Item Value Settings" ),
        PUM_ADDCHILD, PU_LEFT_VERGROUP,
            PUM_ADDCHILD, PU_HORGROUP,
                PUM_ADDCHILD, PU_LABEL("Buy Mod:"),
                PUM_ADDCHILD, PU_ACTION_OBJDEF, CS_ITEMVALUE_BUYMOD, ( PUU32 )"TextEntry",
                    PUA_CONTROL_WEIGHT, 1,
                    PUA_TEXTENTRY_NUMERIC, TRUE,
                    PUA_TEXTENTRY_MIN, 0,
                    PUA_TEXTENTRY_MAX, 200,
                    PUA_TEXTENTRY_BUFFERSIZE, 5,
                    PUA_TEXTENTRY_VALUE, 4,
                0, 0,
            PU_ENDGROUP,
            PUM_ADDCHILD, PU_LABEL("     Clan: 4 / Omni: 5 / Trader Shop: 7"),
        PU_ENDGROUP,
        PUM_ADDCHILD, PU_TITLE( "Item Value Search Settings" ),
        PUM_ADDCHILD, PU_LABEL("A match in this section will count as an ITEM MATCH"),
        PUM_ADDCHILD, PU_LABEL("Make sure you have those enabled if you want to search for values"),
        PUM_ADDCHILD, PU_HORGROUP,
          PUM_ADDCHILD, PU_SPACER,
            PUM_ADDCHILD, PU_FRAMED_VERGROUP( PUFRAME_GROUP, "Single Item" ),
              PUM_ADDCHILD, PU_CHECKBOX( CS_ITEMVALUE_MSINGLE, "Match Single Items"),
              PUM_ADDCHILD, PU_HORGROUP,
                PUM_ADDCHILD, PU_LABEL("Value:"),
                PUM_ADDCHILD, PU_ACTION_OBJDEF, CS_ITEMVALUE_SINGLE, ( PUU32 )"TextEntry",
                    PUA_CONTROL_WEIGHT, 1,
                    PUA_TEXTENTRY_NUMERIC, TRUE,
                    PUA_TEXTENTRY_BUFFERSIZE, 9,
                    PUA_TEXTENTRY_VALUE, 0,
                    PUA_TEXTENTRY_MIN, 0,
                    PUA_TEXTENTRY_MAX, 999999999,
                    PUA_CONTROL_MINWIDTH, 80,
                0, 0,
              PU_ENDGROUP,
            PU_ENDGROUP,

          PUM_ADDCHILD, PU_SPACER,
            PUM_ADDCHILD, PU_FRAMED_VERGROUP( PUFRAME_GROUP, "Total Value" ),
              PUM_ADDCHILD, PU_CHECKBOX( CS_ITEMVALUE_MTOTAL, "Match Total Value"),
              PUM_ADDCHILD, PU_HORGROUP,
                PUM_ADDCHILD, PU_LABEL("Value:"),
                PUM_ADDCHILD, PU_ACTION_OBJDEF, CS_ITEMVALUE_TOTAL, ( PUU32 )"TextEntry",
                    PUA_CONTROL_WEIGHT, 1,
                    PUA_TEXTENTRY_NUMERIC, TRUE,
                    PUA_TEXTENTRY_BUFFERSIZE, 9,
                    PUA_TEXTENTRY_VALUE, 0,
                    PUA_TEXTENTRY_MIN, 0,
                    PUA_TEXTENTRY_MAX, 999999999,
                    PUA_CONTROL_MINWIDTH, 80,
                0, 0,
              PU_ENDGROUP,
            PU_ENDGROUP,
          PUM_ADDCHILD, PU_SPACER,
        PU_ENDGROUP,
        PUM_ADDCHILD, PU_SPACER,

    PU_ENDGROUP,


// **** Mission slider tab ****

    PU_ACTION_OBJDEF, CS_SLIDER_TAB, ( PUU32 )"VerGroup", 0,
        PUM_ADDCHILD, PU_LABEL( "Choose Mission slider amounts" ),
        PUM_ADDCHILD, PU_LABEL( "0 = full left, 100 = full right" ),
        PUM_ADDCHILD, PU_LABEL( "Added by Numbfreeze" ),
        PUM_ADDCHILD, PU_TITLE( "Mission Sliders" ),
        PUM_ADDCHILD, PU_LEFT_VERGROUP,
            //PUM_ADDCHILD, PU_SET_CHECKBOX( CS_TYPEREPAIR_CB, "Difficulty" ),
            //PUM_ADDCHILD, PU_SET_CHECKBOX( CS_TYPERETURN_CB, "Good/Bad" ),
            //PUM_ADDCHILD, PU_SET_CHECKBOX( CS_TYPEFINDP_CB, "Order/Chaos" ),
            //PUM_ADDCHILD, PU_SET_CHECKBOX( CS_TYPEFINDI_CB, "Open/Hidden" ),
            //PUM_ADDCHILD, PU_SET_CHECKBOX( CS_TYPEASS_CB, "Phys/Myst" ),
            //PUM_ADDCHILD, PU_SET_CHECKBOX( CS_TYPEASS_CB, "Money/XP" ),

            PUM_ADDCHILD, PU_LABEL( "Easy/Hard:" ),
            PUM_ADDCHILD, PU_ACTION_OBJDEF, CS_SLIDER_EASY_HARD, ( PUU32 )"TextEntry",
                PUA_CONTROL_WEIGHT, 1,
                PUA_TEXTENTRY_NUMERIC, TRUE,
                PUA_TEXTENTRY_MIN, 0,
                PUA_TEXTENTRY_MAX, 100,
                PUA_TEXTENTRY_BUFFERSIZE, 5,
                PUA_TEXTENTRY_VALUE, 50,
            0, 0,

            PUM_ADDCHILD, PU_LABEL( "Good/Bad:" ),
            PUM_ADDCHILD, PU_ACTION_OBJDEF, CS_SLIDER_GOOD_BAD, ( PUU32 )"TextEntry",
                PUA_CONTROL_WEIGHT, 1,
                PUA_TEXTENTRY_NUMERIC, TRUE,
                PUA_TEXTENTRY_MIN, 0,
                PUA_TEXTENTRY_MAX, 100,
                PUA_TEXTENTRY_BUFFERSIZE, 5,
                PUA_TEXTENTRY_VALUE, 50,
            0, 0,

            PUM_ADDCHILD, PU_LABEL( "Order/Chaos:" ),
            PUM_ADDCHILD, PU_ACTION_OBJDEF, CS_SLIDER_ORDER_CHAOS, ( PUU32 )"TextEntry",
                PUA_CONTROL_WEIGHT, 1,
                PUA_TEXTENTRY_NUMERIC, TRUE,
                PUA_TEXTENTRY_MIN, 0,
                PUA_TEXTENTRY_MAX, 100,
                PUA_TEXTENTRY_BUFFERSIZE, 5,
                PUA_TEXTENTRY_VALUE, 50,
            0, 0,

            PUM_ADDCHILD, PU_LABEL( "Open/Hidden:" ),
            PUM_ADDCHILD, PU_ACTION_OBJDEF, CS_SLIDER_OPEN_HIDDEN, ( PUU32 )"TextEntry",
                PUA_CONTROL_WEIGHT, 1,
                PUA_TEXTENTRY_NUMERIC, TRUE,
                PUA_TEXTENTRY_MIN, 0,
                PUA_TEXTENTRY_MAX, 100,
                PUA_TEXTENTRY_BUFFERSIZE, 5,
                PUA_TEXTENTRY_VALUE, 50,
            0, 0,

            PUM_ADDCHILD, PU_LABEL( "Phys/Myst:" ),
            PUM_ADDCHILD, PU_ACTION_OBJDEF, CS_SLIDER_PHYS_MYST, ( PUU32 )"TextEntry",
                PUA_CONTROL_WEIGHT, 1,
                PUA_TEXTENTRY_NUMERIC, TRUE,
                PUA_TEXTENTRY_MIN, 0,
                PUA_TEXTENTRY_MAX, 100,
                PUA_TEXTENTRY_BUFFERSIZE, 5,
                PUA_TEXTENTRY_VALUE, 50,
            0, 0,

            PUM_ADDCHILD, PU_LABEL( "Head On/Stealth:" ),
            PUM_ADDCHILD, PU_ACTION_OBJDEF, CS_SLIDER_HEADON_STEALTH, ( PUU32 )"TextEntry",
                PUA_CONTROL_WEIGHT, 1,
                PUA_TEXTENTRY_NUMERIC, TRUE,
                PUA_TEXTENTRY_MIN, 0,
                PUA_TEXTENTRY_MAX, 100,
                PUA_TEXTENTRY_BUFFERSIZE, 5,
                PUA_TEXTENTRY_VALUE, 50,
            0, 0,

            PUM_ADDCHILD, PU_LABEL( "Money/XP:" ),
            PUM_ADDCHILD, PU_ACTION_OBJDEF, CS_SLIDER_MONEY_XP, ( PUU32 )"TextEntry",
                PUA_CONTROL_WEIGHT, 1,
                PUA_TEXTENTRY_NUMERIC, TRUE,
                PUA_TEXTENTRY_MIN, 0,
                PUA_TEXTENTRY_MAX, 100,
                PUA_TEXTENTRY_BUFFERSIZE, 5,
                PUA_TEXTENTRY_VALUE, 50,
            0, 0,

            PUM_ADDCHILD, PU_ACTION_OBJDEF, 0, ( PUU32 )"Text",
                PUA_TEXT_STRING, ( PUU32 )"Set Sliders Now",
                PUA_CONTROL_FRAME, PUFRAME_BUTTON,
                PUA_CONTROL_ISBUTTON, TRUE, 0,
                PUM_ADDNOTIFICATION,
                    PUNOTIFY_CONDITION_ATTREQUALS, PUA_CONTROL_CLICKED, TRUE,
                    PUNOTIFY_ACTION_APPMSG, CSAM_SET_SLIDERS,
                0,


        PUM_ADDCHILD, PU_LABEL( "IMPORTANT: Sliders must be in default positions for this to work" ),
        PUM_ADDCHILD, PU_LABEL( "and the 'More Options' section of the mission window must be OPEN!" ),

        PU_ENDGROUP,
        PUM_ADDCHILD, PU_SPACER,

    PU_ENDGROUP,

// ****


// **** Options tab ****
    PU_ACTION_OBJDEF, CS_OPTIONS_TAB, ( PUU32 )"VerGroup", 0,
        PUM_ADDCHILD, PU_TITLE( "ClickSaver "CS_VERSION ),
        PUM_ADDCHILD, PU_LABEL( "Originally by MORB, Updates by Gnarf, Darkbane & Uragon" ),
        PUM_ADDCHILD, PU_LABEL( "https://github.com/pzychotic/ClickSaver/releases" ),
        PUM_ADDCHILD, PU_LABEL( " " ),
        PUM_ADDCHILD, PU_LABEL( "Some parts are from AOMD 1.4 by Muzzleflash" ),
        PUM_ADDCHILD, PU_LABEL( "DLL hooking done using Microsoft Detours" ),
        PUM_ADDCHILD, PU_TITLE( "Special thanks to:" ),
        PUM_ADDCHILD, PU_LABEL( "First Order (http://www.firstorder.net)" ),
        PUM_ADDCHILD, PU_LABEL( "All the helpful people on HQ forums" ),
        PUM_ADDCHILD, PU_TITLE( NULL ),

        PUM_ADDCHILD, PU_HORGROUP,
            PUM_ADDCHILD, PU_VERSPACER( 20 ),
            PUM_ADDCHILD, PU_FRAMED_VERGROUP( PUFRAME_GROUP, "Options" ),

/*              PUM_ADDCHILD, PU_ACTION_OBJDEF, CS_OPTIONSFOLD3, ( PUU32 )"Fold",
                    PUA_FOLD_LABEL, ( PUU32 )"General",
                    PUA_FOLD_FOLDED, 0,
                    PUA_FOLD_CONTENTS, */
            PUM_ADDCHILD,
                    PU_HORGROUP,
                        PUM_ADDCHILD, PU_LEFT_VERGROUP,
                            PUM_ADDCHILD, PU_CHECKBOX( CS_STARTMIN_CB, "Start minimized" ),
                            PUM_ADDCHILD, PU_SET_CHECKBOX( CS_MSGBOX_CB, "Alert box" ),
                            PUM_ADDCHILD, PU_SET_CHECKBOX( CS_MOUSEMOVE_CB, "Select Match" ),
                            PUM_ADDCHILD, PU_CHECKBOX( CS_SOUNDS_CB, "Sounds" ),
                            PUM_ADDCHILD, PU_CHECKBOX( CS_LOG_CB, "Logging" ),

                            PUM_ADDCHILD, PU_ACTION_OBJDEF, CS_BAINFO_CB, ( PUU32 )"CheckBox",
                                PUA_CHECKBOX_LABEL, ( PUU32 )"Show buying agent help",
                                PUA_CHECKBOX_CHECKED, TRUE, 0,
                                PUM_ADDNOTIFICATION,
                                    PUNOTIFY_CONDITION_ATTRCHANGE, PUA_CHECKBOX_CHECKED,
                                    PUNOTIFY_ACTION_SETVAL, CS_BAINFO2_CB, PUA_CHECKBOX_CHECKED,
                                0,

                            PUM_ADDCHILD, PU_CHECKBOX( CS_EXPAND_CB, "Auto Expand Team Missions" ),

                        PU_ENDGROUP,
                    PUM_ADDCHILD, PU_SPACER,
                    PU_ENDGROUP,
//              0, 0, /// End Fold


            PU_ENDGROUP,
            PUM_ADDCHILD, PU_VERSPACER( 20 ),
        PU_ENDGROUP,
        PUM_ADDCHILD, PU_TITLE("Export Options"),
        PUM_ADDCHILD, PU_HORGROUP,
            PUM_ADDCHILD, PU_ACTION_OBJDEF, 0, ( PUU32 )"Text", PUA_TEXT_STRING, ( PUU32 )"Export Settings",
                PUA_CONTROL_FRAME, PUFRAME_BUTTON,
                PUA_CONTROL_ISBUTTON, TRUE, 0,
                PUM_ADDNOTIFICATION,
                    PUNOTIFY_CONDITION_ATTREQUALS, PUA_CONTROL_CLICKED, TRUE,
                    PUNOTIFY_ACTION_APPMSG, CSAM_EXPORTSETTINGS,
                0,
            PUM_ADDCHILD, PU_ACTION_OBJDEF, 0, ( PUU32 )"Text", PUA_TEXT_STRING, ( PUU32 )"Import Settings",
                PUA_CONTROL_FRAME, PUFRAME_BUTTON,
                PUA_CONTROL_ISBUTTON, TRUE, 0,
                PUM_ADDNOTIFICATION,
                    PUNOTIFY_CONDITION_ATTREQUALS, PUA_CONTROL_CLICKED, TRUE,
                    PUNOTIFY_ACTION_APPMSG, CSAM_IMPORTSETTINGS,
                0,
        PU_ENDGROUP,
        PUM_ADDCHILD, PU_TITLE( "" ),
        PUM_ADDCHILD, PU_LABEL( "Some things money can't buy..." ),
        PUM_ADDCHILD, PU_LABEL( "For everything else, there's ClickSaver." ),
        PUM_ADDCHILD, PU_LABEL( "-Nanonik" ),

        PUM_ADDCHILD, PU_SPACER,
    PU_ENDGROUP,
// ****

// **** Main window ***
    PU_ACTION_OBJDEF, CS_MAIN_WINDOW,
    ( PUU32 )"Window",
    PUA_WINDOW_TITLE, ( PUU32 )"ClickSaver",
    PUA_WINDOW_ICONIFYBOX, TRUE,
    PUA_WINDOW_FORCETOP, TRUE,
    PUA_WINDOW_WIDTH, 290,
    PUA_WINDOW_CONTENTS,
        PU_ACTION_OBJDEF, CS_TABS, ( PUU32 )"Register",
            PUA_REGISTER_FIXEDHEIGHT, TRUE, 0,
            PUM_REGISTER_ADDTAB, ( PUU32 )"Missions",
            PUM_REGISTER_ADDTAB, ( PUU32 )"Items",
            PUM_REGISTER_ADDTAB, ( PUU32 )"Locations",
            PUM_REGISTER_ADDTAB, ( PUU32 )"Type",
            PUM_REGISTER_ADDTAB, ( PUU32 )"Values",

            PUM_REGISTER_ADDTAB, ( PUU32 )"Sliders",
            PUM_REGISTER_ADDTAB, ( PUU32 )"Options",

            PUM_REGISTER_SETTABOBJECT, PU_OBJID, CS_MISSIONS_TAB,
            PUM_REGISTER_SETTABOBJECT, PU_OBJID, CS_ITEMWATCH_TAB,
            PUM_REGISTER_SETTABOBJECT, PU_OBJID, CS_LOCWATCH_TAB,
            PUM_REGISTER_SETTABOBJECT, PU_OBJID, CS_TYPEWATCH_TAB,
            PUM_REGISTER_SETTABOBJECT, PU_OBJID, CS_ITEMVALUES_TAB,

            PUM_REGISTER_SETTABOBJECT, PU_OBJID, CS_SLIDER_TAB,
            PUM_REGISTER_SETTABOBJECT, PU_OBJID, CS_OPTIONS_TAB,
            0,
    0,
    PUM_ADDNOTIFICATION,
        PUNOTIFY_CONDITION_ATTREQUALS, PUA_WINDOW_CLOSE, TRUE,
        PUNOTIFY_ACTION_APPMSG, CSAM_QUIT,
    0,
// ****

// **** Watch messagebox
    PU_ACTION_OBJDEF, CS_WATCH_MSGBOX,
    ( PUU32 )"Window",
    PUA_WINDOW_TITLE, ( PUU32 )"ClickSaver",
    PUA_WINDOW_NOCLOSEBOX, TRUE,
    PUA_WINDOW_FORCETOP, TRUE,
    PUA_WINDOW_HORCENTER, TRUE,
    PUA_WINDOW_VERCENTER, TRUE,
    PUA_WINDOW_CONTENTS,
        PU_VERGROUP,
            PUM_ADDCHILD, PU_HORSPACER( 20 ),
            PUM_ADDCHILD, PU_HORGROUP,
                PUM_ADDCHILD, PU_FIXSPACER( 20, 1 ),
                PUM_ADDCHILD, PU_LABEL( "One or more of the missions matches your search criterias." ),
                PUM_ADDCHILD, PU_FIXSPACER( 20, 1 ),
            PU_ENDGROUP,
            PUM_ADDCHILD, PU_HORSPACER( 20 ),
            PUM_ADDCHILD, PU_ACTION_OBJDEF, 0, ( PUU32 )"Text", PUA_TEXT_STRING, ( PUU32 )"Ok",
                PUA_CONTROL_HASKEYBOARDFOCUS, TRUE,
                PUA_CONTROL_FRAME, PUFRAME_BUTTON,
                PUA_CONTROL_ISBUTTON, TRUE, 0,
                PUM_ADDNOTIFICATION,
                    PUNOTIFY_CONDITION_ATTREQUALS, PUA_CONTROL_CLICKED, TRUE,
                    PUNOTIFY_ACTION_SET, CS_WATCH_MSGBOX, PUA_WINDOW_OPENED, FALSE,
                0,
        PU_ENDGROUP,
    0, 0,
// ****

// **** Buying agent help window
    PU_ACTION_OBJDEF, CS_BUYINGAGENT_INFOWINDOW,
    ( PUU32 )"Window",
    PUA_WINDOW_TITLE, ( PUU32 )"Buying agent help - Please read",
    PUA_WINDOW_NOCLOSEBOX, TRUE,
    PUA_WINDOW_FORCETOP, TRUE,
    PUA_WINDOW_HORCENTER, TRUE,
    PUA_WINDOW_VERCENTER, TRUE,
    PUA_WINDOW_CONTENTS,
        PU_VERGROUP,
            PUM_ADDCHILD, PU_HORSPACER( 20 ),
            PUM_ADDCHILD, PU_HORGROUP,
                PUM_ADDCHILD, PU_FIXSPACER( 20, 1 ),

                // Gotta implement a word-wrapping text display class in PUL...
                // Too lazy atm to do it though ;)
                PUM_ADDCHILD, PU_VERGROUP,
                    PUM_ADDCHILD, PU_LABEL( "The buying agent will generate missions from" ),
                    PUM_ADDCHILD, PU_LABEL( "the terminal automatically until it finds" ),
                    PUM_ADDCHILD, PU_LABEL( "a mission that matches your search criterias." ),
                    PUM_ADDCHILD, PU_LABEL( " " ),
                    PUM_ADDCHILD, PU_LABEL( "You have to open the mission terminal window" ),
                    PUM_ADDCHILD, PU_LABEL( "and put it completly in the upper left corner" ),
                    PUM_ADDCHILD, PU_LABEL( "before starting the buying agent." ),
                    PUM_ADDCHILD, PU_LABEL( " " ),
                    PUM_ADDCHILD, PU_LABEL( "Be sure to set up a reasonnable amount of" ),
                    PUM_ADDCHILD, PU_LABEL( "maximum number tries before starting." ),
                PU_ENDGROUP,

                PUM_ADDCHILD, PU_FIXSPACER( 20, 1 ),
            PU_ENDGROUP,
            PUM_ADDCHILD, PU_HORSPACER( 20 ),

            PUM_ADDCHILD, PU_ACTION_OBJDEF, CS_BAINFO2_CB, ( PUU32 )"CheckBox",
                PUA_CHECKBOX_LABEL, ( PUU32 )"Show this window next time",
                PUA_CHECKBOX_CHECKED, TRUE, 0,
                PUM_ADDNOTIFICATION,
                    PUNOTIFY_CONDITION_ATTRCHANGE, PUA_CHECKBOX_CHECKED,
                    PUNOTIFY_ACTION_SETVAL, CS_BAINFO_CB, PUA_CHECKBOX_CHECKED,
                0,

            PUM_ADDCHILD, PU_HORGROUP,
                PUM_ADDCHILD, PU_ACTION_OBJDEF, 0, ( PUU32 )"Text", PUA_TEXT_STRING, ( PUU32 )"Start",
                    PUA_CONTROL_HASKEYBOARDFOCUS, TRUE,
                    PUA_CONTROL_FRAME, PUFRAME_BUTTON,
                    PUA_CONTROL_ISBUTTON, TRUE, 0,
                    PUM_ADDNOTIFICATION,
                        PUNOTIFY_CONDITION_ATTREQUALS, PUA_CONTROL_CLICKED, TRUE,
                        PUNOTIFY_ACTION_APPMSG, CSAM_STARTBUYINGAGENT,
                    0,
                PUM_ADDCHILD, PU_ACTION_OBJDEF, 0, ( PUU32 )"Text", PUA_TEXT_STRING, ( PUU32 )"Cancel",
                    PUA_CONTROL_FRAME, PUFRAME_BUTTON,
                    PUA_CONTROL_ISBUTTON, TRUE, 0,
                    PUM_ADDNOTIFICATION,
                        PUNOTIFY_CONDITION_ATTREQUALS, PUA_CONTROL_CLICKED, TRUE,
                        PUNOTIFY_ACTION_SET, CS_BUYINGAGENT_INFOWINDOW, PUA_WINDOW_OPENED, FALSE,
                    0,
            PU_ENDGROUP,
        PU_ENDGROUP,
    0, 0,
// ****

// **** Fullscreen help window
    PU_ACTION_OBJDEF, CS_FULLSCREEN_WINDOW,
    ( PUU32 )"Window",
    PUA_WINDOW_TITLE, ( PUU32 )"Fullscreen mode - Please read",
    PUA_WINDOW_NOCLOSEBOX, TRUE,
    PUA_WINDOW_FORCETOP, TRUE,
    PUA_WINDOW_HORCENTER, TRUE,
    PUA_WINDOW_VERCENTER, TRUE,
    PUA_WINDOW_CONTENTS,
        PU_VERGROUP,
            PUM_ADDCHILD, PU_HORSPACER( 20 ),
            PUM_ADDCHILD, PU_HORGROUP,
                PUM_ADDCHILD, PU_FIXSPACER( 20, 1 ),

                // Gotta implement a word-wrapping text display class in PUL...
                // Too lazy atm to do it though ;)
                PUM_ADDCHILD, PU_VERGROUP,
                    PUM_ADDCHILD, PU_LABEL( "Fullscreen mode will leave this window here"),
                    PUM_ADDCHILD, PU_LABEL( "not update your GUI for clicksaver, and also"),
                    PUM_ADDCHILD, PU_LABEL( "Will automatically start the buying agent"),
                    PUM_ADDCHILD, PU_LABEL( "When you roll the first mission"),
                    PUM_ADDCHILD, PU_LABEL( " "),
                    PUM_ADDCHILD, PU_LABEL( "The buying agent will generate missions from" ),
                    PUM_ADDCHILD, PU_LABEL( "the terminal automatically until it finds" ),
                    PUM_ADDCHILD, PU_LABEL( "a mission that matches your search criterias." ),
                    PUM_ADDCHILD, PU_LABEL( " " ),
                    PUM_ADDCHILD, PU_LABEL( "You have to open the mission terminal window" ),
                    PUM_ADDCHILD, PU_LABEL( "and put it completly in the upper left corner" ),
                    PUM_ADDCHILD, PU_LABEL( "before starting the buying agent." ),
                    PUM_ADDCHILD, PU_LABEL( " " ),
                    PUM_ADDCHILD, PU_LABEL( "Be sure to set up a reasonnable amount of" ),
                    PUM_ADDCHILD, PU_LABEL( "maximum number tries before starting." ),
                PU_ENDGROUP,

                PUM_ADDCHILD, PU_FIXSPACER( 20, 1 ),
            PU_ENDGROUP,
            PUM_ADDCHILD, PU_HORSPACER( 20 ),
            PUM_ADDCHILD, PU_ACTION_OBJDEF, 0, ( PUU32 )"Text",
                PUA_TEXT_STRING, ( PUU32 )"Exit Fullscreen",
                PUA_CONTROL_FRAME, PUFRAME_BUTTON,
                PUA_CONTROL_HASKEYBOARDFOCUS, TRUE,
                PUA_CONTROL_ISBUTTON, TRUE, 0,
                PUM_ADDNOTIFICATION,
                    PUNOTIFY_CONDITION_ATTREQUALS, PUA_CONTROL_CLICKED, TRUE,
                        PUNOTIFY_ACTION_APPMSG, CSAM_STOPFULLSCREEN,
                0,

        PU_ENDGROUP,
    0, 0,
// ****


// **** Buying agent window
    PU_ACTION_OBJDEF, CS_BUYINGAGENT_WINDOW,
    ( PUU32 )"Window",
    PUA_WINDOW_TITLE, ( PUU32 )"ClickSaver",
    PUA_WINDOW_NOCLOSEBOX, TRUE,
    PUA_WINDOW_FORCETOP, TRUE,
    PUA_WINDOW_HORCENTER, TRUE,
    PUA_WINDOW_VERCENTER, TRUE,
    PUA_WINDOW_CONTENTS,
        PU_VERGROUP,
            PUM_ADDCHILD, PU_HORSPACER( 20 ),
            PUM_ADDCHILD, PU_HORGROUP,
                PUM_ADDCHILD, PU_FIXSPACER( 20, 1 ),
                PUM_ADDCHILD, PU_LABEL( "Buying agent is running..." ),
                PUM_ADDCHILD, PU_FIXSPACER( 20, 1 ),
            PU_ENDGROUP,
            PUM_ADDCHILD, PU_HORSPACER( 20 ),
            PUM_ADDCHILD, PU_ACTION_OBJDEF, 0, ( PUU32 )"Text",
                PUA_TEXT_STRING, ( PUU32 )"Stop",
                PUA_CONTROL_FRAME, PUFRAME_BUTTON,
                PUA_CONTROL_HASKEYBOARDFOCUS, TRUE,
                PUA_CONTROL_ISBUTTON, TRUE, 0,
                PUM_ADDNOTIFICATION,
                    PUNOTIFY_CONDITION_ATTREQUALS, PUA_CONTROL_CLICKED, TRUE,
                        PUNOTIFY_ACTION_APPMSG, CSAM_STOPBUYINGAGENT,
                0,
        PU_ENDGROUP,
    0, 0,
// ****


// **** Error messagebox
    PU_ACTION_OBJDEF, CS_ERROR_WINDOW,
    ( PUU32 )"Window",
    PUA_WINDOW_TITLE, ( PUU32 )"ClickSaver error",
    PUA_WINDOW_NOCLOSEBOX, TRUE,
    PUA_WINDOW_FORCETOP, TRUE,
    PUA_WINDOW_HORCENTER, TRUE,
    PUA_WINDOW_VERCENTER, TRUE,
    PUA_WINDOW_CONTENTS,
        PU_VERGROUP,
            PUM_ADDCHILD, PU_HORSPACER( 20 ),
            PUM_ADDCHILD, PU_HORGROUP,
                PUM_ADDCHILD, PU_FIXSPACER( 20, 1 ),
                PUM_ADDCHILD, PU_ACTION_OBJDEF, CS_ERROR_TEXT, ( PUU32 )"Text", PUA_TEXT_FIXEDWIDTH, TRUE, 0, 0,
                PUM_ADDCHILD, PU_FIXSPACER( 20, 1 ),
            PU_ENDGROUP,
            PUM_ADDCHILD, PU_HORSPACER( 20 ),

            PUM_ADDCHILD, PU_ACTION_OBJDEF, 0, ( PUU32 )"Text", PUA_TEXT_STRING, ( PUU32 )"Ok",
                PUA_CONTROL_HASKEYBOARDFOCUS, TRUE,
                PUA_CONTROL_FRAME, PUFRAME_BUTTON,
                PUA_CONTROL_ISBUTTON, TRUE, 0,
                PUM_ADDNOTIFICATION,
                    PUNOTIFY_CONDITION_ATTREQUALS, PUA_CONTROL_CLICKED, TRUE,
                    PUNOTIFY_ACTION_SET, CS_ERROR_WINDOW, PUA_WINDOW_OPENED, FALSE,
                PUM_ADDNOTIFICATION,
                    PUNOTIFY_CONDITION_ATTREQUALS, PUA_CONTROL_CLICKED, TRUE,
                    PUNOTIFY_ACTION_APPMSG, CSAM_OK,
                0,
        PU_ENDGROUP,
    0, 0,
// ****


// **** Database copy message
    PU_ACTION_OBJDEF, CS_DBCOPYMSGBOX,
    ( PUU32 )"Window",
    PUA_WINDOW_TITLE, ( PUU32 )"Clicksaver",
    PUA_WINDOW_NOCLOSEBOX, TRUE,
    PUA_WINDOW_FORCETOP, TRUE,
    PUA_WINDOW_HORCENTER, TRUE,
    PUA_WINDOW_VERCENTER, TRUE,
    PUA_WINDOW_CONTENTS,
        PU_VERGROUP,
            PUM_ADDCHILD, PU_HORSPACER( 20 ),
            PUM_ADDCHILD, PU_HORGROUP,
                PUM_ADDCHILD, PU_FIXSPACER( 20, 1 ),

                PUM_ADDCHILD, PU_VERGROUP,
                    PUM_ADDCHILD, PU_LABEL( "The local AO items database is missing or outdated." ),
                    PUM_ADDCHILD, PU_LABEL( " " ),
                    PUM_ADDCHILD, PU_LABEL( "ClickSaver will extract item informations from AO's resource database" ),
                    PUM_ADDCHILD, PU_LABEL( "and store them in it's own database." ),
                    PUM_ADDCHILD, PU_LABEL( " " ),
                    PUM_ADDCHILD, PU_LABEL( "It will take some time and a bit of harddisk space." ),
                    PUM_ADDCHILD, PU_LABEL( " " ),
                    PUM_ADDCHILD, PU_LABEL( "Make sure that AO isn't running, or it won't work." ),
                    PUM_ADDCHILD, PU_LABEL( " " ),
                    PUM_ADDCHILD, PU_LABEL( "You can Skip Building the new database, but be warned," ),
                    PUM_ADDCHILD, PU_LABEL( "it may cause some instability if there are any new items" ),
                    PUM_ADDCHILD, PU_LABEL( "in the mission terminals" ),
                PU_ENDGROUP,

                PUM_ADDCHILD, PU_FIXSPACER( 20, 1 ),
            PU_ENDGROUP,
            PUM_ADDCHILD, PU_HORSPACER( 20 ),

            PUM_ADDCHILD, PU_HORGROUP,
                PUM_ADDCHILD, PU_ACTION_OBJDEF, 0, ( PUU32 )"Text", PUA_TEXT_STRING, ( PUU32 )"Continue",
                    PUA_CONTROL_HASKEYBOARDFOCUS, TRUE,
                    PUA_CONTROL_FRAME, PUFRAME_BUTTON,
                    PUA_CONTROL_ISBUTTON, TRUE, 0,
                    PUM_ADDNOTIFICATION,
                        PUNOTIFY_CONDITION_ATTREQUALS, PUA_CONTROL_CLICKED, TRUE,
                        PUNOTIFY_ACTION_APPMSG, CSAM_OK,
                    0,

                PUM_ADDCHILD, PU_ACTION_OBJDEF, 0, ( PUU32 )"Text", PUA_TEXT_STRING, ( PUU32 )"Skip",
                    PUA_CONTROL_FRAME, PUFRAME_BUTTON,
                    PUA_CONTROL_ISBUTTON, TRUE, 0,
                    PUM_ADDNOTIFICATION,
                        PUNOTIFY_CONDITION_ATTREQUALS, PUA_CONTROL_CLICKED, TRUE,
                        PUNOTIFY_ACTION_APPMSG, CSAM_SKIP,
                    0,

                PUM_ADDCHILD, PU_ACTION_OBJDEF, 0, ( PUU32 )"Text", PUA_TEXT_STRING, ( PUU32 )"Quit",
                    PUA_CONTROL_FRAME, PUFRAME_BUTTON,
                    PUA_CONTROL_ISBUTTON, TRUE, 0,
                    PUM_ADDNOTIFICATION,
                        PUNOTIFY_CONDITION_ATTREQUALS, PUA_CONTROL_CLICKED, TRUE,
                        PUNOTIFY_ACTION_APPMSG, CSAM_QUIT,
                    0,
            PU_ENDGROUP,
        PU_ENDGROUP,
    0, 0,
// ****

// **** Database creation window
    PU_ACTION_OBJDEF, CS_CREATINGDBMSGBOX,
    ( PUU32 )"Window",
    PUA_WINDOW_TITLE, ( PUU32 )"ClickSaver",
    PUA_WINDOW_NOCLOSEBOX, TRUE,
    PUA_WINDOW_HORCENTER, TRUE,
    PUA_WINDOW_VERCENTER, TRUE,
    PUA_WINDOW_CONTENTS,
        PU_VERGROUP,
            PUM_ADDCHILD, PU_HORSPACER( 20 ),
            PUM_ADDCHILD, PU_HORGROUP,
                PUM_ADDCHILD, PU_FIXSPACER( 20, 1 ),
                PUM_ADDCHILD, PU_LABEL( "Creating local database, please wait..." ),
                PUM_ADDCHILD, PU_FIXSPACER( 20, 1 ),
            PU_ENDGROUP,
            PUM_ADDCHILD, PU_HORSPACER( 20 ),
        PU_ENDGROUP,
    0, 0,
// ****

    PU_ACTION_END
};
