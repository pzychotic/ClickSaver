// ***************************************************************
//  madCodeHook.h             version:  1.3a  ·  date: 2002-01-21
//  -------------------------------------------------------------
//  API hooking, code hooking
//  -------------------------------------------------------------
//  Copyright (C) 1999 - 2002 www.madshi.net, All Rights Reserved
// ***************************************************************

// 2002-01-21 1.3a ProcessHandleToID exported
// 2001-07-08 1.3  new functions (1) AllocMemEx & FreeMemEx
//                               (2) CopyFunction
//                               (3) CreateRemoteThread and
//                               (4) InjectLibrary added
// 2001-04-20 1.2a you can now force HookCode/API to use the mixture mode
// 2001-04-16 1.2  new function CreateProcessEx -> dll injecting

#ifdef __cplusplus

#define madCodeHookApi(rt) extern "C" __declspec(dllimport) rt WINAPI

// hook any code or a specific API
// under win9x you can hook code system wide, if it begins > $80000000
// for a system wide hook the callback function is automatically copied to shared memory
// use only kernel32.dll APIs in such a system wide hook callback function (!!)
madCodeHookApi(BOOL) HookCode(
  PVOID  pCode,
  PVOID  pCallbackFunc,
  PVOID  *pNextHook,
  PDWORD pdwErrorNo            = NULL,
  PSTR   pszErrorStr           = NULL,
  BOOL   bSystemWide           = FALSE,
  BOOL   bAcceptUnknownTargets = FALSE,
  BOOL   bForceMixtureMode     = FALSE
);
madCodeHookApi(BOOL) HookAPI(
  PSTR   pszModule,
  PSTR   pszFuncName,
  PVOID  pCallbackFunc,
  PVOID  *pNextHook,
  PDWORD pdwErrorNo            = NULL,
  PSTR   pszErrorStr           = NULL,
  BOOL   bSystemWide           = FALSE,
  BOOL   bAcceptUnknownTargets = FALSE,
  BOOL   bForceMixtureMode     = FALSE
);
madCodeHookApi(BOOL) UnhookCode(
  PVOID  *pNextHook
);

// memory allocation in the specified processes (shared memory in win9x)
// if the processHandle is 0, the memory is allocated or freed in the shared
// area (in win9x) or in our own process (in winNT)
madCodeHookApi(PVOID) AllocMemEx(
  DWORD  dwSize,
  HANDLE hProcess = NULL
);
madCodeHookApi(BOOL) FreeMemEx(
  PVOID  pMem,
  HANDLE hProcess = NULL
);

// copy (and relocate) any function to a new location in any process
// if the processHandle is 0, the function is copied to shared area (in win9x)
// or to another memory location in our own process (in winNT)
// don't forget to free the function with FreeMemEx, if you don't it anymore
madCodeHookApi(PVOID) CopyFunction(
  PVOID  pFunction,
  HANDLE hProcess              = 0,
  BOOL   bAcceptUnknownTargets = FALSE,
  PVOID  *pBuffer              = NULL
);

#else

#define madCodeHookApi(rt) __declspec(dllimport) rt WINAPI

madCodeHookApi(BOOL) HookCode(
  PVOID  pCode,
  PVOID  pCallbackFunc,
  PVOID  *pNextHook,
  PDWORD pdwErrorNo,
  PSTR   pszErrorStr,
  BOOL   bSystemWide,
  BOOL   bAcceptUnknownTargets,
  BOOL   bForceMixtureMode
);
madCodeHookApi(BOOL) HookAPI(
  PSTR   pszModule,
  PSTR   pszFuncName,
  PVOID  pCallbackFunc,
  PVOID  *pNextHook,
  PDWORD pdwErrorNo,
  PSTR   pszErrorStr,
  BOOL   bSystemWide,
  BOOL   bAcceptUnknownTargets,
  BOOL   bForceMixtureMode
);
madCodeHookApi(BOOL) UnhookCode(
  PVOID  *pNextHook
);

madCodeHookApi(LPVOID) AllocMemEx(
  DWORD  dwSize,
  HANDLE hProcess
);
madCodeHookApi(BOOL) FreeMemEx(
  PVOID  pMem,
  HANDLE hProcess
);

madCodeHookApi(PVOID) CopyFunction(
  PVOID  pFunction,
  HANDLE hProcess,
  BOOL   bAcceptUnknownTargets,
  PVOID  *ppBuffer
);

#endif

// same as CreateProcess
// additionally the dll "loadLibrary" is injected into the newly created process
// the dll is loaded right before the entry point of the exe module is called
madCodeHookApi(BOOL) CreateProcessEx(
  LPSTR                 lpApplicationName,
  LPSTR                 lpCommandLine,
  LPSECURITY_ATTRIBUTES lpProcessAttributes,
  LPSECURITY_ATTRIBUTES lpThreadAttributes,
  BOOL                  bInheritHandles,
  DWORD                 dwCreationFlags,
  LPVOID                lpEnvironment,
  LPSTR                 lpCurrentDirectory,
  LPSTARTUPINFO         lpStartupInfo,
  LPPROCESS_INFORMATION lpProcessInformation,
  LPSTR                 lpLoadLibrary
);

// same as kernel32.CreateRemoteThread, but this one also works perfectly in win9x!!
madCodeHookApi(HANDLE) CreateRemoteThread(
  HANDLE                 hProcess,
  LPSECURITY_ATTRIBUTES  lpThreadAttributes,
  DWORD                  dwStackSize,
  LPTHREAD_START_ROUTINE lpStartAddress,
  LPVOID                 lpParameter,
  DWORD                  dwCreationFlags,
  LPDWORD                lpThreadId
);

// same as LoadLibrary, but this one is able to load the library in any process
madCodeHookApi(BOOL) InjectLibrary(
  HANDLE hProcess,
  PSTR   pLibFileName
);

// which processID belongs to the specified process handle?
// undocumented function, works in all windows 32 bit systems
madCodeHookApi(DWORD) ProcessHandleToID(
  DWORD  dwProcessHandle
);
