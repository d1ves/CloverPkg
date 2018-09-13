/*
 * refit/scan/tool.c
 *
 * Copyright (c) 2006-2010 Christoph Pfisterer
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *  * Neither the name of Christoph Pfisterer nor the names of the
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "entry_scan.h"

//
// Clover File location to boot from on removable media devices
//
#define CLOVER_MEDIA_FILE_NAME_IA32    L"\\EFI\\CLOVER\\CLOVERIA32.EFI"
#define CLOVER_MEDIA_FILE_NAME_IA64    L"\\EFI\\CLOVER\\CLOVERIA64.EFI"
#define CLOVER_MEDIA_FILE_NAME_X64     L"\\EFI\\CLOVER\\CLOVERX64.EFI"
#define CLOVER_MEDIA_FILE_NAME_ARM     L"\\EFI\\CLOVER\\CLOVERARM.EFI"

#if   defined (MDE_CPU_IA32)
#define CLOVER_MEDIA_FILE_NAME   CLOVER_MEDIA_FILE_NAME_IA32
#elif defined (MDE_CPU_IPF)
#define CLOVER_MEDIA_FILE_NAME   CLOVER_MEDIA_FILE_NAME_IA64
#elif defined (MDE_CPU_X64)
#define CLOVER_MEDIA_FILE_NAME   CLOVER_MEDIA_FILE_NAME_X64
#elif defined (MDE_CPU_EBC)
#elif defined (MDE_CPU_ARM)
#define CLOVER_MEDIA_FILE_NAME   CLOVER_MEDIA_FILE_NAME_ARM
//#else
//#error Unknown Processor Type
#endif

#ifndef DEBUG_ALL
#define DEBUG_SCAN_TOOL 1
#else
#define DEBUG_SCAN_TOOL DEBUG_ALL
#endif

#define DBG(...) DebugLog(1, __VA_ARGS__)

extern EMU_VARIABLE_CONTROL_PROTOCOL *gEmuVariableControl;

STATIC BOOLEAN AddToolEntry(IN CHAR16 *LoaderPath, IN CHAR16 *FullTitle, IN CHAR16 *LoaderTitle,
                            IN REFIT_VOLUME *Volume, IN EG_IMAGE *Image,
                            IN CHAR16 ShortcutLetter, IN CHAR16 *Options)
{return 0;}

STATIC VOID AddCloverEntry(IN CHAR16 *LoaderPath, IN CHAR16 *LoaderTitle, IN REFIT_VOLUME *Volume)
{}

VOID ScanTool(VOID)
{}

// Add custom tool entries
VOID AddCustomTool(VOID)
{}
