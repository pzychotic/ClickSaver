/*
ClickSaver process hooking -  Anarchy Online mission helper
Copyright (C) 2001, 2002 Morb
Parts Copyright (C) 2003, 2004 gnarf

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
#include <windows.h>
#include <madcodehooklib.h>
#include <stdio.h>
#include "clicksaver.h"

extern HANDLE g_Mutex;
extern char g_CSDir[256];
extern char g_CurrentPacket[65536];

void Inject()
{
	HWND AOWnd;
	DWORD AOProcessId;
	HANDLE AOProcessHnd;

	if( AOWnd = FindWindow( "Anarchy client", "Anarchy Online" ) )
	{
		// Get process id
		GetWindowThreadProcessId( AOWnd, &AOProcessId );

		// Get a handle on the process
		// XP Required privileges: PROCESS_SUSPEND_RESUME|PROCESS_VM_WRITE|PROCESS_VM_READ|PROCESS_VM_OPERATION|PROCESS_CREATE_THREAD
		// Vista requires some more
		// XP Maximum possible privileges to request: 0x0fff
		if( AOProcessHnd = OpenProcess(	0x0fff,	FALSE, AOProcessId ) )
		{
			char Temp[256];

			// Inject the dlls in clientr process
			sprintf( Temp, "%s\\madCodeHookLib.dll", g_CSDir );
			InjectLibrary( AOProcessHnd, Temp);

			sprintf( Temp, "%s\\AOHook.dll", g_CSDir );
			InjectLibrary( AOProcessHnd, Temp);

			CloseHandle( AOProcessHnd );
		}
	}
}

LRESULT CALLBACK HookWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	COPYDATASTRUCT* pData;

	switch (msg)
	{
		case WM_TIMER:
			Inject();
			break;

		case WM_COPYDATA:
			pData = ( PCOPYDATASTRUCT )lParam;

			WaitForSingleObject( g_Mutex, INFINITE );
			memset(g_CurrentPacket, 0, 65536);
			memcpy( g_CurrentPacket, pData->lpData, pData->cbData );
			ReleaseMutex( g_Mutex );

			puSendAppMessage( CSAM_NEWMISSIONS, 0 );
			break;

		default:
			return( DefWindowProc( hWnd, msg, wParam, lParam ) );
	}

	return 0;
}


DWORD WINAPI HookManagerThread( void *_pParam )
{
	WNDCLASSEX HookWndClass;
	HWND hWnd;
	int RetCode;
	MSG msg;
	UINT Timer;

	SetThreadPriority( GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL );

	HookWndClass.cbSize = sizeof( WNDCLASSEX ); 
	HookWndClass.style = 0;
	HookWndClass.lpfnWndProc = HookWndProc;
	HookWndClass.cbClsExtra = 0;
	HookWndClass.cbWndExtra = 0;
	HookWndClass.hInstance = NULL;
	HookWndClass.hIcon = NULL;
	HookWndClass.hCursor = NULL;
	HookWndClass.hbrBackground = NULL;
	HookWndClass.lpszMenuName = NULL;
	HookWndClass.lpszClassName = "ClickSaverHookWindowClass";
	HookWndClass.hIconSm = NULL;

	RegisterClassEx( &HookWndClass );

	/* Create window */
	hWnd = CreateWindowEx( 0, "ClickSaverHookWindowClass", "ClickSaverHookWindow",
		WS_MINIMIZE, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, NULL, NULL);

	Inject();

	Timer = SetTimer( hWnd, 0, 5000, NULL );

	while( RetCode = GetMessage( &msg, NULL, 0, 0 ) )
	{
		if( RetCode != -1 )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg ); 
		}
	}

	KillTimer( hWnd, Timer );
	DestroyWindow(hWnd);

	return TRUE;
}
