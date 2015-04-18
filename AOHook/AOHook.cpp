/*
 * This DLL patches the DataBlockToMessage function
 * in AO's MessageProtocol.dll. It is injected by
 * ClickSaver into AO process.
 *
 * Note that you have to compile this project separately.
 * If you set it as dependance of ClickSaver, it will
 * tell the linker to link ClickSaver with AOHook.lib,
 * which doesn't exist.
 *
 * The output path for the dll is the clicksaver sources
 * directory (and not AOHook/Release|Debug), for easier
 * debugging ;)
 */
/*
AOHook.dll - AO process hook -  Anarchy Online mission helper
Copyright (C) 2002 Morb

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
#include "Detours\detours.h"

void*	g_pDataBlockToMessage = NULL;

// According to this documention of VC++ name-mangling
// http://www.kegel.com/mangle.html:
//
// ?DataBlockToMessage@@YAPAVMessage_t@@IPAX@Z
// is named DataBlockToMessage, returns a Message_t*,
// and takes an int and a void* as parameters.

// Just to be more readable.
// Message_t is actually a class of AO
// But just to define a pointer on it,
// we don't need to know what it is.
typedef void Message_t;

Message_t* (*pOriginalDataBlockToMessage)(int _Size, void* _pDataBlock);

Message_t* DataBlockToMessageHook(int _Size, void* _pDataBlock)
{
	unsigned long* pData, Temp;
	//FILE* fp;
	HWND hWnd;

	if (_Size > 0x40) {
		// For 14.2
	    // pData = ( unsigned long* )( ( char* )_pDataBlock + 0x34 );
		// For 14.4.0.2 on test...
		pData = (unsigned long*)((char*)_pDataBlock + 0x33);
		Temp = *pData;
		/*
		if (fp = fopen( "g:\\AOHook_Log.bin", "ab")) {
			fwrite( _pDataBlock, _Size, 1, fp );
			fprintf( fp, "********" );
			fclose( fp );
		}
		*/
		if (Temp == 0xc3da0000) {
			// Find ClickSaver's hook thread window and send the datas
			// using WM_COPYDATA
			if (hWnd = FindWindow("ClickSaverHookWindowClass", "ClickSaverHookWindow")) {
				COPYDATASTRUCT Data;
				Data.cbData = _Size;
				Data.lpData = _pDataBlock;
				SendMessage(hWnd, WM_COPYDATA, 0, (LPARAM)&Data);
			}
		}
	}
	return pOriginalDataBlockToMessage(_Size, _pDataBlock);
}

int ProcessAttach( HINSTANCE _hModule )
{
    // Hook ::DataBlockToMessage() (incoming)
    pOriginalDataBlockToMessage = (Message_t *(__cdecl*)(int,void*))::GetProcAddress(::GetModuleHandle("MessageProtocol.dll"), "?DataBlockToMessage@@YAPAVMessage_t@@IPAX@Z");
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach((PVOID*)&pOriginalDataBlockToMessage, DataBlockToMessageHook);
    DetourTransactionCommit();
    return TRUE;
}


int ProcessDetach( HINSTANCE _hModule )
{
    LONG res1 = DetourTransactionBegin();
    LONG res2 = DetourUpdateThread(GetCurrentThread());
    LONG res3 = DetourDetach((PVOID*)&pOriginalDataBlockToMessage, DataBlockToMessageHook);
    LONG res4 = DetourTransactionCommit();
    return TRUE;
}

BOOL APIENTRY DllMain(HINSTANCE _hModule, DWORD _dwReason, PVOID _lpReserved)
{
	switch (_dwReason)	{
		case DLL_PROCESS_ATTACH:
			return ProcessAttach( _hModule );
		case DLL_PROCESS_DETACH:
			return ProcessDetach( _hModule );
	}
	return TRUE;
}
