ClickSaver 2.5.1
================

Update by Uragon for the 18.1 Anarchy Online patch (database changes)
and for 18.4 (new input system).

Update by darkbane for 16.3

Update by Gnarf
gnarf@gnarf.net
http://clicksaver.notumwars.com/

Originally by MORB
a.chavasse@free.fr
http://a.chavasse.free.fr/ClickSaver

Some parts are from AOMD 1.4 by Muzzleflash.
DLL patching done using Microsofts Detours.

Description
-----------

This is (yet) another AOMD like tool for Anarchy Online. It opens
a window that displays the details about all missions you take at
mission terminals in a single window.
Additionaly, it can search for substrings in item names and can
warn you when a mission's reward match one of the searched items.

This versions no longer use winpcap. It hooks on to a function
in a dll of the game to get mission information.
It means that it's now completly independant of your network
configuration, and so should work with just anything (lan,
adsl, modems, etc.)

Also, it now works properly with the new AO networking code that
compresses packets, which is enabled in some zones (like Newland).

However, there's still the possibility (and it's even probable)
that because of some bugs, or possible  incompatibilities between
the dll patching api (Detours) and some versions of windows.
However, if AOMD never worked for you, have a try at this one :)

Downside is that it's not possible anymore to run it on a different
machine than AO itself.

And remember that you must run AO in windowed mode ;) (well, it's not
really mandatory, but alt-tabbing to go look at clicksaver window
isn't that useful :p)

Installation
------------
There's pretty much nothing needed to install ClickSaver itself.
Just copy it somewhere. Everything must be in the same directory
(it's recommended to put it all in it's own directory though).

On first run, it asks for AO directory and creates the items database.
Note that AO has to be closed whenever CS wants to create a new items database.

Usage
-----
The "missions" tab shows various datas for each mission (Playfield,
coordinates, QL, reward item, find item, item icon and cash)
Items matching the "item watch" criterias are displayed in red.
In addition, a message box can be displayed.

The "item list" tab shows a list where you can enter substring to
search among item names. Ex: "battle suit", "hand axe", etc.
In addition to the add and remove buttons, there's several
shortcuts to edit the list:

 - Arrow keys: select entry
 - Return: add entry
 - Shift+return or double click: edit entry
 - DEL: remove entry

The "location list" tab is the same as the "item list" tab, but
for the location field. You can use this to search for playfields
(ie "Pleasant Meadows", "Broken Shores"...), as well as coordinates,
or both.

The "Mission Type" tab lets you select a type of mission you wish to be alarmed on.

The "Options" tab:
 - Start Minimized: if this is checked, ClickSaver start in minimized
   state.
 - Alert box: if this is checked, an alert box is displayed when
   one or more mission is found, depending is the mission search
   conditions (see below).
 - Sounds: This turns on/off the sounds.. It plays found.wav and notfound.wav when running buying agent.
 - Select Match: This turns on/off the mouse movement to select the first matching mission when using buying agent


Mission search conditions:
 - Listed item is found: if this is checked, a mission must have
   an item search match (red).
 - Listed location is found: if this is checked, a mission must
   have a location search match (red).
 - Mission Type Found: if this is checked, a mission must
   have a mission type search match (red).

A mission must match all checked criteria for it to alert.

Highligh:
 - Turn these off to disable the red highlighting of certain types
   of matches. (if you only want to highlight items for example)

The window is resizable horizontally. It's size and position are
saved when exiting the program (as well as others config details
and item watch list).

You can minimize the window. It will maximize itself automatically
when new missions are received.

Buying Agent
------------

Like AOMD2.0, this function gets missions automatically until it
finds a mission matching your mission search conditions.

You have to have a mission terminal window already opened ingame
and to snap it in the upper left corner. You setup the sliders
and the difficulty, then you select options tab in clicksaver,
enter the maximum number of times the buying agent will get new
missions until it stops, and you just hit start.

It will stop as soon as it finds a mission that matches your searching
conditions, or until the maximum number of tries has been reached.

It will refuse to work if your search conditions and search list are
setup in a way that means that no mission will ever match (eg. you're
searching only missions with matching items, but item list is empty)

If you start the buying agent with the mission booth window misplaced,
it will wait forever for missions to appear, and it won't get out of
buying agent mode (which basically means that it won't display missions
anymore and that it will move the mouse around everytime you generate
new missions from a mission terminal)

There's two things you can do in this case: put the mission booth window
properly in the upper left corner, generate a mission and the buying
agent will kick in.

Or, quit clicksaver and launch it again. (use alt+tab or the taskbar to
show up clicksaver window again)

Note that unlike AOMD2.0, clicksaver doesn't care about the placement of
the AO window on the screen. Note also that clicking positions are
preset and not configurable. It works by clicking twice on the leftmost
pixel of the difficulty slider, then twice on the rightmost pixel of
that slider, then it clicks on request missions. It means that if you
use it with the difficulty slider either fully to the left or fully to
the right, it will actually get off of one pixel or two from the position
where you have put it.


History
-------
2.5.1       - Database won't be deleted until the new one is correctly created
            - Fixed file handle leak
            - Some minor fixes
            - Binaries build with VS 2015 Update 3
2.5.0       - First public release by pzychotic
            - Updated to VS2015
            - Updated PUL to latest available version, now also built with VS2015
2.4.1       - Updated so Detours will also work for WinXP, so the same executable can be used on all Windows versions
2.4.0       - Updated for 18.5 for Win7+ by Darkbane
            - Switched hooking method to Microsoft Detours
2.3.6       - Updated for 18.5 for WinXP by Darkbane
2.3.5       - Updated for 18.4
2.3.4       - Bug fixes
2.3.3           - Updated for 18.1
        Database code changed to enable clicksaver to work with the new item database

2.3.2           - Updated for 16.3
2.3.1           - updated for 15.9
2.3.0 beta 3    - Added Sliders Tab (thanks Numbfreeze)
                - Added Item Values (thanks to Jayde for a little inspiration here)
            - Made the Copy of the Database clicksaver makes remarkably smaller.
2.3.0 beta 2    - Should work with 15.2.0 now
                - Added option called "Auto Expand Team Mission" should be self explanitory
            - Added extra field to the log file that should help determine what type of terminal was used to roll the mission
2.3.0 beta 1    - Fixed the Rebuilding the DB every time bug (hopefully)
                - Added Export/Import Settings to the options tab
            - Settings file is NOT compatible with earlier versions
2.2.9       - "Google Like" search strings:
            - Item Search, to allow web-search-like constructors "<text>", -<text> and word-match
        Examples:
        Searching for 'decus -gloves' will match all decus items except gloves;
        Searching for 'decus armor' will match on 'decus body armor', and 'decus armor boots'
        Searching for '"decus armor"' will match on 'decus armor boots' but not on 'decus body armor'
        Searching for '"primus decus" -gloves -boots -body' will match on all primus decus armor except for gloves, boots and body
        - Location Search, to allow as above, plus location range search
        Examples:
        Searching for 'athen -shire' will match on 'west athens' and 'old athens'
        Searching for 'athen (100-200,500-600)' will match on any athen mission with coords x from 100 to 200, y from 500 to 600.
        Searching for 'athen (100.2,200.3)' will match on any athen mission with coords x and y exactly 100.2 and 200.3 respectively
        Searching for 'athen (0-500,3000-999999)' will match on any athen mission with coords x <=500, y>=3000 (but less than 999999)
        - Made clicksaver a little smarter about only getting 3 or 4 missions from the terminal.
        - Added a checkbox to disable the auto-selection of matching mission when your in buying agent.

2.2.9 beta  - Fixed a bug with 14.8 where missions without rewards would stop clicksaver from reading the other missions after it
            - Added Mission XP reward to display
                - Added logging. I'm working on a log parser later, and I will release the log format as well, but this is an emergency release to fix the bug with 14.8.
                - Added Sounds.. There is now "found.wav" and "notfound.wav" if someone wants to make some sounds for it, e-mail em to me, right now it just uses some sounds I hijacked from the windows media directory
                - Fullscreen mode - If you hit this, it starts a mode where it will automatically start buying agent everytime you roll the first mission. It also has a "Fullscreen mode" UI box that never switches windows, which should get rid of some window swapping issues
2.2.8       - New Maintainer.. Added mission types/time into the display/search.
            Also made it select the mission you want selected before it ends buying agent
2.2.7       - Fixed (for good, it seems, I actually tested it this time...) the PUL bug that was causing the crash with team missions.
            - Slightly tweaked the layout of folding controls, to make the team mission mode a bit clearer.
2.2.6       - Fixed (I hope, because I didn't do any test :)) the crash with team missions.
            - Linked with latest version of PUL, so the interface redrawing is much better handled now (especially when resizing).
2.2.5       - AO DB was opened in read/write mode (because I mixed up CS sources with
        those of another program I worked on at the time), which was sometimes
        changing it's data/time in such a way that CS thought its local DB was outdated.
2.2.4       - Sets a big (32Mb) cache for berkeley database when creating the local database.
            - No longer forces the db page size to 512 bytes under win98. The default size seems to give much better performance, although it makes the         database file a little bigger. Under winXP however, forcing a small page size gives much better performance...
        - Processes the windows messages while duplicating the database so that task manager no longers says that clicksaver isn't answering while
        it creates the local database.
      - The database is now created in a temporary file and renamed when it's
    complete. This is to avoid to have clicksaver use an incomplete database
    if you killed it while it was creating the DB (if it displays bogus and
    incomplete stuff, like unknown item: xxxx, please delete the
    AODatabase.bdb file in CS' directory and start it again).
2.2.3 - Fixed for 14.4.
      - No longer duplicates the whole AO resource database. Instead, it now
    creates a local database and store only the needed stuff from AO database
    (playfield infos, items, and icons). It take much less space on hard
    disk and take much less time than the whole copy. Also, it should fix
    the "couldn't open AO database" message that some people had.
      - Reads playfield names from database instead of having them hardcoded.
      - Now properly exits after a fatal error.
      - Buying agent now has a fixed delay of 2010 ms between missions because
    of the 14.4 minimal delay of 2 seconds for mission generator.
    Removed the delay option. I don't think that it should be needed
    to slow it down even more than that for it to work properly...
      - Buying agent no longer move the difficulty slider forward and back.
    It's no longer neccessary in 14.4 because the request button now
    ungrays itself automatically.
    Additionaly, buying agent now move the mouse over the request button
    only once, and not move the mouse anymore after, it only clicks.
    It makes it possible to abort the buying agent easily.
    Also, the mouse is now moved to the lower right corner of the request
    button, so that buying agent works even if the mission window is
    off from the upper left corner by a few pixels.
2.2.2 - Fixed a (stupid) bug in PUL when getting the size/position of a
    window. That's what messed up everything under win98/me. Thanks
    to Entalia for the help.
    Next time I'll try to rush less and to read the MSDN more
    carefully, but I was eager to play 14.2 ;)
      - Now deletes resource.dat and resource.idx before copying them back
    from AO directory (in case there's more recent ones in AO directory)
      - Added (at last) a delay option for the buying agent, in order to
    slow it down if it goes too fast and you have problems. It's in
    option tab, default to 500 milliseconds.
2.2.1 - Adapted for 14.2: item infos in database are no longer crypted,
    and since database is open in exclusive mode by AO, it nows
    duplicate it in it's own directory...
      - Fixed buying agent bug, hopefully. It should now stop at the right
    time and not skip over the matching mission (I constitently
    reproduced the problem while playing on test server, I think I
    identified it and now it seems to work proper. I'm not adding
    a delay option because I don't think it will help to work around
    the bug)
      - Fixed window size bug in config file when exiting clicksaver
    while it's iconified.
2.2   - Implemented team missions support.
      - Fixed a bug with timer creation/destruction.
      - Now reset window position if stored position in configuration
    appears o be invalid (it's a workaround for a bug that I'm too
    lazy to try to reproduce and fix ;))
2.1.1 - Fixed bug with alert box always appearing regardless of
    the alert box setting.
      - Added buffer overrun checking when parsing missions. I doubt
    that it's what makes ClickSaver crash for some people but it
    won't hurt.
      - Fixed a bug with missions without reward item causing ClickSaver
    to crash sometimes.
      - Moved buying agent to mission tab.
      - Added a buying agent help window for those bad boys and bad girls
    who don't read the ReadMEs...
      - Added a window that shows that buying agent is working, with a
    stop button. Should solve the problem with clicksaver stuck in
    buying agent mode when you move the mouse while it's working (and
    so it provides a way of stopping it while it's running...)
      - Reworked the way buying agent generate mouse clicks. Seems to work
    better: it actually clicks twice on each side of the difficulty
    slider (on some level/mission difficulty setting, you have to click
    twice on diff slider to actually enable the request mission button).
    It should also avoid the problem with difficulty slider moving
    off one pixel or two from where it was set at. And maybe the
    problem with buying agent not starting at all sometimes...
    As a side effect, buying agent is much, much faster now (hope it
    won't cause too much overload on FC mission servers...)
    Downside is I'm not sure it will work properly on every config
    because of framerate/timing problems, so be sure to keep the
    previous version around just in case...
    I don't know about the "one click too much" bug though, maybe
    it's gone (who knows... :)). However, the workaround of placing the
    AO window so that alert box appears over mission terminal window
    won't work anymore because of the way it generate clicks now.
2.1   - Implemented buying agent.
2.0   - Dropped winpcap. Now works by injecting a dll in AO process
    that patches an internal function of AO, in order to get
    packets after they're decompressed.
    Also now sets ClickSaver's thread priority to "above normal",
    which makes the refresh of mission display much faster.
1.1.4 - Fixed another problem of non-freed gdi objects in PUL. Was
    causing display to get screwed up completly on win98 after the
    display of a certain number of missions (and it was trashing
    display very quickly when playing with resizing...)
    I tested it myself on win98 this time and it seems to be stable.
1.1.3 - Now strips leading and trailing spaces of watch entries. Was
    causing problems when cut&pasting item lists from web pages
    into the watch list file, because for some reasons it often
    adds a space at the end of the lines...
    Fixed unknown items appearing sometimes. It was actually doing
    that on missions without reward item. Note that AOMD has this
    bug too ;)
1.1.2 - Fixed bug in PUL: it wasn't actually freeing the pens and brushes,
    which was causing it to literally explode under win95/98/me.
    Fixed resizing bug under win95/98/me.
    Now opens network adapter in promiscuous mode... I think
    that's why it didn't work for some people for whom AOMD works.
    It should makes also possible to run it on a separate machine
    on the same hub that doesn't route the packets to the machine
    running AO, like it was possible with AOMD (some people do that...)
    It probably didn't work in non promiscuous mode.
1.1.1 - Added some logging in debug version to at last locate that
    *** of a ****ing ***** bug under win98/me. Found a very small
    thing in the process that might be the cause of the problem
    and fixed it. If it was that, I'm gonna hang myself.
1.1   - Implemented location watch and related options.
    Fixed a stupid bug with mission parser that made ClickSaver
    crash whenever the sentence "credits were deducted from your
    account" was said on any chat channel ;)
1.0.1 - Fixed a bug causing adapter selection to crash. Also reverted
        some portions of code to AOMD code, I hope it will make
    ClickSaver work on win98/ME at last.
1.0   - Implemented minimize, watch item message box, ability to
    change network adapter setting without deleting the config
    file, and fixed some window centering issues.
0.9.1 - Fixed a mess with the network adapter list that may have
        prevented ClickSaver to work on win98/ME.
0.9   - First released version
