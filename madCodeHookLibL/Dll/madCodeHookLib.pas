// ***************************************************************
//  madCodeHookLib.pas        version:  1.3a  ·  date: 2002-01-21
//  -------------------------------------------------------------
//  API hooking, code hooking
//  -------------------------------------------------------------
//  Copyright (C) 1999 - 2002 www.madshi.net, All Rights Reserved
// ***************************************************************

unit madCodeHookLib;

interface

uses Windows;

// ***************************************************************

type TPCardinal = ^cardinal;
     TPPointer  = ^pointer;

const CMadCodeHookLib = 'madCodeHookLib.dll';

// ***************************************************************

function HookCode (code                 : pointer;
                   callbackFunc         : pointer;
                   out nextHook         : pointer;
                   errorNo              : TPCardinal = nil;
                   errorStr             : pchar      = nil;
                   systemWide           : boolean    = false;
                   acceptUnknownTargets : boolean    = false;
                   forceMixtureMode     : boolean    = false) : longBool; stdcall;
         external CMadCodeHookLib;

function HookAPI (module, api          : pchar;
                  callbackFunc         : pointer;
                  out nextHook         : pointer;
                  errorNo              : TPCardinal = nil;
                  errorStr             : pchar      = nil;
                  systemWide           : boolean    = false;
                  acceptUnknownTargets : boolean    = false;
                  forceMixtureMode     : boolean    = false) : longBool; stdcall;
         external CMadCodeHookLib;

function UnhookCode (var nextHook: pointer) : longBool; stdcall;
         external CMadCodeHookLib;

// ***************************************************************

function CreateProcessEx (applicationName, commandLine : pchar;
                          processAttr, threadAttr      : PSecurityAttributes;
                          inheritHandles               : longBool;
                          creationFlags                : cardinal;
                          environment                  : pointer;
                          currentDirectory             : pchar;
                          const startupInfo            : TStartupInfo;
                          var processInfo              : TProcessInformation;
                          loadLibrary                  : pchar              ) : longBool; stdcall;
         external CMadCodeHookLib;

// ***************************************************************

function AllocMemEx (size          : cardinal;
                     processHandle : cardinal = 0) : pointer; stdcall;
         external CMadCodeHookLib;

function FreeMemEx (mem           : pointer;
                    processHandle : cardinal = 0) : boolean; stdcall;
         external CMadCodeHookLib;

// ***************************************************************

function CopyFunction (func                 : pointer;
                       processHandle        : cardinal       = 0;
                       acceptUnknownTargets : boolean        = false;
                       buffer               : TPPointer      = nil  ) : pointer; stdcall;
         external CMadCodeHookLib;

// ***************************************************************

function CreateRemoteThread (processHandle : cardinal;
                             threadAttr    : PSecurityAttributes;
                             stackSize     : integer;
                             startAddr     : pointer;
                             params        : pointer;
                             creationFlags : cardinal;
                             var threadID  : cardinal           ) : cardinal; stdcall;
         external CMadCodeHookLib;

// ***************************************************************

function InjectLibrary (processHandle : cardinal;
                        libFileName   : pchar   ) : longBool; stdcall;
         external CMadCodeHookLib;

// ***************************************************************

// which processID belongs to the specified process handle?
// undocumented function, works in all windows 32 bit systems
function ProcessHandleToID (processHandle : cardinal) : cardinal; stdcall;
         external CMadCodeHookLib;

// ***************************************************************

implementation

end.