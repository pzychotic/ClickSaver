/*
ClickSaver process hooking -  Anarchy Online mission helper
Copyright (C) 2001, 2002 Morb
Parts Copyright (C) 2003, 2004 gnarf
Parts Copyright (C) 2012 Darkbane, Adjuster

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
#include <stdio.h>
#include <cmath>
#include <string>
#include "clicksaver.h"

extern "C" HANDLE g_Mutex;
extern "C" char g_CSDir[ MAX_PATH ];
extern "C" char g_CurrentPacket[ 65536 ];

std::string to_ascii_copy( std::wstring const& input )
{
    int len_buffer = input.length() + 1;
    char* abuffer = (char*)malloc( len_buffer );
    ZeroMemory( abuffer, len_buffer );
    WideCharToMultiByte( CP_ACP, NULL, input.c_str(), input.length(), abuffer, len_buffer, NULL, NULL );
    std::string result( abuffer );
    free( abuffer );
    return result;
}


inline std::string to_ascii_copy( std::string const& input )
{
    return input;
}


bool InjectDLL( DWORD ProcessID, std::string const& dllName )
{
    if( !ProcessID )
    {
        //LOG("No process ID specified.");
        return false;
    }
    //LOG("Attempting to inject '" << dllName << "' into process " << ProcessID);
    // Clear error status.
    SetLastError( 0 );
    HANDLE Proc = OpenProcess( PROCESS_ALL_ACCESS, FALSE, ProcessID );
    if( !Proc )
    {
        //LOG("Failed to open AO client process. Error: " << GetLastError());
        return false;
    }
    std::string dllNameA = to_ascii_copy( dllName );
    LPVOID RemoteString = (LPVOID)VirtualAllocEx( Proc, NULL, dllNameA.length(), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
    if( !RemoteString )
    {
        //LOG("Failed to allocate memory in AO client. Error: " << GetLastError());
        CloseHandle( Proc );
        return false;
    }
    if( !WriteProcessMemory( Proc, (LPVOID)RemoteString, dllNameA.c_str(), dllNameA.length(), NULL ) )
    {
        //LOG("Failed to send DLL name to AO client. Error: " << GetLastError());
        CloseHandle( Proc );
        return false;
    }
    LPVOID LoadLibAddy = (LPVOID)GetProcAddress( GetModuleHandle( "kernel32.dll" ), "LoadLibraryA" );
    if( !CreateRemoteThread( Proc, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibAddy, (LPVOID)RemoteString, NULL, NULL ) )
    {
        //LOG("Failed to start hook in AO client. Error: " << GetLastError());
        CloseHandle( Proc );
        return false;
    }
    CloseHandle( Proc );
    return true;
}


void Inject()
{
    HWND AOWnd;
    DWORD AOProcessId;
    //HANDLE AOProcessHnd;

    if( AOWnd = FindWindow( "Anarchy client", NULL ) )
    {
        char Temp[ 256 ];
        // Get process id
        GetWindowThreadProcessId( AOWnd, &AOProcessId );
        sprintf( Temp, "%s\\AOHook.dll", g_CSDir );
        if( !InjectDLL( AOProcessId, Temp ) )
        {
            // error...
        }
    }
}


LRESULT CALLBACK HookWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    COPYDATASTRUCT* pData;

    switch( msg )
    {
    case WM_TIMER:
        Inject();
        break;
    case WM_COPYDATA:
        pData = (PCOPYDATASTRUCT)lParam;
        WaitForSingleObject( g_Mutex, INFINITE );
        memset( g_CurrentPacket, 0, 65536 );
        memcpy( g_CurrentPacket, pData->lpData, pData->cbData );
        ReleaseMutex( g_Mutex );
        puSendAppMessage( CSAM_NEWMISSIONS, 0 );
        break;
    default:
        return DefWindowProc( hWnd, msg, wParam, lParam );
    }
    return 0;
}


extern "C" DWORD WINAPI HookManagerThread( void* _pParam )
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
    hWnd = CreateWindowEx( 0, "ClickSaverHookWindowClass", "ClickSaverHookWindow", WS_MINIMIZE, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, NULL, NULL );
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
    DestroyWindow( hWnd );
    return TRUE;
}
