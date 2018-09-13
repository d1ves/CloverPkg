/*
 * refit/icns.c
 * Loader for .icns icon files
 *
 * Copyright (c) 2006-2007 Christoph Pfisterer
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

#include "libegint.h"
//#include "../include/egemb_icons.h"

#ifndef DEBUG_ALL
#define DEBUG_ICNS 1
#else
#define DEBUG_ICNS DEBUG_ALL
#endif

#define DBG(...) DebugLog(1, __VA_ARGS__)
//
// well-known icons
//

BUILTIN_ICON BuiltinIconTable[BUILTIN_ICON_COUNT] = {
  { NULL, L"selection_big"                 , L"png",  144 }
};

//#define DEC_BUILTIN_ICON(id, ico) BuiltinIconTable[id].Image = egDecodePNG(ico, sizeof(ico), BuiltinIconTable[id].PixelSize, TRUE)
//#define DEC_BUILTIN_ICON(id, ico) BuiltinIconTable[id].Image = egDecodePNG(&ico[0], SZ_##ico, TRUE)
#define DEC_BUILTIN_ICON(id, ico) BuiltinIconTable[id].Image = egDecodePNG(ACCESS_EMB_DATA(ico), ACCESS_EMB_SIZE(ico), TRUE)

CHAR16 * GetIconsExt(IN CHAR16 *Icon, IN CHAR16 *Def)
{return NULL;}
/*
EG_IMAGE * GetSmallHover(IN UINTN Id)
{
  CHAR16 *Path = AllocateZeroPool(256);
  Path = GetIconsExt(PoolPrint(L"%s_hover", BuiltinIconTable[Id].Path), BuiltinIconTable[Id].Format);
  return egLoadIcon(ThemeDir, Path, BuiltinIconTable[Id].PixelSize);
}
*/
EG_IMAGE * BuiltinIcon(IN UINTN Id)
{return NULL;}

//
// Load an icon for an operating system
//

EG_IMAGE * LoadOSIcon(IN CHAR16 *OSIconName OPTIONAL, IN CHAR16 *FallbackIconName, IN UINTN PixelSize, IN BOOLEAN BootLogo, IN BOOLEAN WantDummy)
{return NULL;}
/*
EG_IMAGE * LoadHoverIcon(IN CHAR16 *OSIconName, IN UINTN PixelSize)
{
  EG_IMAGE        *Image = NULL;

  if (GlobalConfig.TextOnly)      // skip loading if it's not used anyway
    return NULL;

  Image = egLoadIcon(ThemeDir, OSIconName, PixelSize);

  return Image;
}
*/
//
// Load an image from a .icns file
//

EG_IMAGE * LoadIcns(IN EFI_FILE_HANDLE BaseDir, IN CHAR16 *FileName, IN UINTN PixelSize)
{return NULL;}

static EG_PIXEL BlackPixel  = { 0x00, 0x00, 0x00, 0 };
//static EG_PIXEL YellowPixel = { 0x00, 0xff, 0xff, 0 };

EG_IMAGE * DummyImage(IN UINTN PixelSize)
{return NULL;}

EG_IMAGE * LoadIcnsFallback(IN EFI_FILE_HANDLE BaseDir, IN CHAR16 *FileName, IN UINTN PixelSize)
{return NULL;}
