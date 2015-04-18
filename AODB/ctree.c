/*
ClickSaver -  Anarchy Online mission helper
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
#define _CTREE_CPP
#include <windows.h>
#include "ctree.h"

// Handle of the Ctree DLL.  NULL until it is loaded.
static HINSTANCE hDll = NULL;

#define DYNLINK(a,b) \
	if(!(fnDll = GetProcAddress(hDll, (b)))) goto exit_error;\
	memcpy(&(a), &fnDll, sizeof(void *));	

// Perform dynamic linking of ctreestd.dll functions
int CTreeStd_LinkDll(char *DllPath) {
	FARPROC fnDll;

	if (hDll) {
		// Already linked. Signal success.
		return 1;
	}

	// Load the dll file into memory
	hDll = LoadLibrary(DllPath);
	if (hDll == NULL) {
		// Unable to load library file. Signal failure.
		return 0;
	}
		
	// Look up functions in the dll and set their pointers
	// On failure, jump to exit_error.
	DYNLINK(InitISAM, "INTISAM");
	DYNLINK(CloseISAM, "CLISAM");
	DYNLINK(OpenIFile, "OPNIFIL");
	DYNLINK(CloseIFile, "CLIFIL");
	DYNLINK(GetRecord, "EQLREC");
	DYNLINK(GetVRecord, "EQLVREC");
	DYNLINK(VRecordLength, "GETVLEN");
	DYNLINK(ReReadVRecord, "REDVREC");

	DYNLINK(FirstKey, "FRSKEY");
	DYNLINK(NextKey, "NXTKEY");
	DYNLINK(GetGTEKey, "GTEKEY");
	DYNLINK(ReWriteVRecord, "RWTVREC");
	DYNLINK(NextVRecord, "NXTVREC" );
	DYNLINK(GetGTEVRecord, "GTEVREC" );
 
	return 1; // No Failures.  Signal success.

exit_error:
	// Function name not found in the dynamic link table by 
	// GetProcAddress.  Unload the dll and signal failure.
	FreeLibrary(hDll);
	hDll = NULL;
	return 0;
}


// Unlink ctreestd.dll functions and unload the dll
void CTreeStd_UnlinkDll() {
	if (!hDll) {
		// It isn't linked yet, nothing to do.
		return;
	}

	// Set all function pointers back to NULL so that
	// if the user tries to use any of these no longer
	// linked functions he'll get an exception error.
	InitISAM = NULL;
	CloseISAM = NULL;
	OpenIFile = NULL;
	CloseIFile = NULL;
	GetRecord = NULL;
	GetVRecord = NULL;
	VRecordLength = NULL;
	ReReadVRecord = NULL;

	// Unload the dll library from memory
	FreeLibrary(hDll);
	hDll = NULL;
}
