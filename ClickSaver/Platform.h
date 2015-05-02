
#pragma once

// Set all minimum platform version defines to Windows XP
#ifndef WINVER
#define WINVER          0x0501
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT    0x0501
#endif

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS  0x0501
#endif

#ifndef NTDDI_VERSION
#define NTDDI_VERSION   0x05010000
#endif

#include <windows.h>
