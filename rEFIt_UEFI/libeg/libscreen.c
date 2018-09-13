/*
 * libeg/screen.c
 * Screen handling functions
 *
 * Copyright (c) 2006 Christoph Pfisterer
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

#if defined(LODEPNG)
#include "lodepng.h"
#endif //LODEPNG
VOID egEncodeBMP(IN EG_IMAGE *Image, OUT UINT8 **FileDataReturn, OUT UINTN *FileDataLengthReturn);
//#include <efiUgaDraw.h>
#include <Protocol/GraphicsOutput.h>
//#include <Protocol/efiConsoleControl.h>

// Console defines and variables

static EFI_GUID ConsoleControlProtocolGuid = EFI_CONSOLE_CONTROL_PROTOCOL_GUID;
static EFI_CONSOLE_CONTROL_PROTOCOL *ConsoleControl = NULL;

static EFI_GUID UgaDrawProtocolGuid = EFI_UGA_DRAW_PROTOCOL_GUID;
static EFI_UGA_DRAW_PROTOCOL *UgaDraw = NULL;

static EFI_GUID GraphicsOutputProtocolGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
static EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphicsOutput = NULL;

static BOOLEAN egHasGraphics = FALSE;
static UINTN egScreenWidth  = 0; //1024;
static UINTN egScreenHeight = 0; //768;

static BOOLEAN IgnoreConsoleSetMode = FALSE;
static EFI_CONSOLE_CONTROL_SCREEN_MODE CurrentForcedConsoleMode = EfiConsoleControlScreenText;
static EFI_CONSOLE_CONTROL_PROTOCOL_GET_MODE ConsoleControlGetMode = NULL;
static EFI_CONSOLE_CONTROL_PROTOCOL_SET_MODE ConsoleControlSetMode = NULL;

static EFI_STATUS GopSetModeAndReconnectTextOut();

//
// Wrapped ConsoleControl GetMode() implementation - for blocking resolution switch when changing modes
//
EFI_STATUS EFIAPI
egConsoleControlGetMode(IN EFI_CONSOLE_CONTROL_PROTOCOL *This, OUT EFI_CONSOLE_CONTROL_SCREEN_MODE *Mode, OUT BOOLEAN *GopUgaExists, OPTIONAL OUT BOOLEAN *StdInLocked OPTIONAL) {
    if (IgnoreConsoleSetMode) {
        *Mode = CurrentForcedConsoleMode;
        if (GopUgaExists)
            *GopUgaExists = TRUE;
        if (StdInLocked)
            *StdInLocked = FALSE;
        return EFI_SUCCESS;
    }

    return ConsoleControlGetMode(This, Mode, GopUgaExists, StdInLocked);
}

EFI_STATUS EFIAPI
egConsoleControlSetMode(IN EFI_CONSOLE_CONTROL_PROTOCOL *This, IN EFI_CONSOLE_CONTROL_SCREEN_MODE Mode) {
    // Pretend that we updated our console mode but do not call SetMode itself to avoid breaking the resolution.
    // Please note, that it is also relevant for proper boot.efi progress bar rendering with FileVault 2.
    // See more details in refit/main.c
    if (IgnoreConsoleSetMode) {
        CurrentForcedConsoleMode = Mode;
        return EFI_SUCCESS;
    }

    return ConsoleControlSetMode(This, Mode);
}

//
// Screen handling
//
/*
VOID egDumpGOPVideoModes(VOID)
{
    EFI_STATUS  Status;
    UINT32      MaxMode;
    UINT32      Mode;
    UINTN       SizeOfInfo;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;
    CHAR16      *PixelFormatDesc;
    
    if (GraphicsOutput == NULL) {
        return;
    }
    
    // get dump
    MaxMode = GraphicsOutput->Mode->MaxMode;
    Mode = GraphicsOutput->Mode->Mode;
//    MsgLog("Available graphics modes for refit.conf screen_resolution:\n");
//    MsgLog("Curr. Mode = %d, Modes = %d, FB = %lx, FB size=0x%x\n",
//           Mode, MaxMode, GraphicsOutput->Mode->FrameBufferBase, GraphicsOutput->Mode->FrameBufferSize);
    
    for (Mode = 0; Mode < MaxMode; Mode++) {
        Status = GraphicsOutput->QueryMode(GraphicsOutput, Mode, &SizeOfInfo, &Info);
        if (Status == EFI_SUCCESS) {
            
            switch (Info->PixelFormat) {
                case PixelRedGreenBlueReserved8BitPerColor:
                    PixelFormatDesc = L"8bit RGB";
                    break;
                    
                case PixelBlueGreenRedReserved8BitPerColor:
                    PixelFormatDesc = L"8bit BGR";
                    break;
                    
                case PixelBitMask:
                    PixelFormatDesc = L"BITMASK";
                    break;
                    
                case PixelBltOnly:
                    PixelFormatDesc = L"NO FB";
                    break;
                    
                default:
                    PixelFormatDesc = L"invalid";
                    break;
            }
            
            MsgLog("- Mode %d: %dx%d PixFmt = %s, PixPerScanLine = %d\n",
                  Mode, Info->HorizontalResolution, Info->VerticalResolution, PixelFormatDesc, Info->PixelsPerScanLine);
        } else {
            MsgLog("- Mode %d: %r\n", Mode, Status);
        }
    }
    
}
*/
VOID egDumpSetConsoleVideoModes(VOID)
{}

EFI_STATUS egSetMaxResolution()
{return 0;}

EFI_STATUS egSetMode(INT32 Next)
{return 0;}

EFI_STATUS egSetScreenResolution(IN CHAR16 *WidthHeight)
{return 0;}

VOID egInitScreen(IN BOOLEAN SetMaxResolution)
{}

VOID egGetScreenSize(OUT INTN *ScreenWidth, OUT INTN *ScreenHeight)
{}

CHAR16 * egScreenDescription(VOID)
{
    return L"";
}

BOOLEAN egHasGraphicsMode(VOID)
{
    return egHasGraphics;
}

BOOLEAN egIsGraphicsModeEnabled(VOID)
{
    EFI_CONSOLE_CONTROL_SCREEN_MODE CurrentMode;

    if (ConsoleControl != NULL) {
        ConsoleControl->GetMode(ConsoleControl, &CurrentMode, NULL, NULL);
        return (CurrentMode == EfiConsoleControlScreenGraphics) ? TRUE : FALSE;
    }
    
    return FALSE;
}

VOID egSetGraphicsModeEnabled(IN BOOLEAN Enable)
{}

//
// Drawing to the screen
//

VOID egClearScreen(IN EG_PIXEL *Color)
{}
    
VOID egDrawImageArea(IN EG_IMAGE *Image,
                     IN INTN AreaPosX, IN INTN AreaPosY,
                     IN INTN AreaWidth, IN INTN AreaHeight,
                     IN INTN ScreenPosX, IN INTN ScreenPosY)
{}
// Blt(this, Buffer, mode, srcX, srcY, destX, destY, w, h, deltaSrc);
VOID egTakeImage(IN EG_IMAGE *Image, INTN ScreenPosX, INTN ScreenPosY,
                 IN INTN AreaWidth, IN INTN AreaHeight)
{}

//
// Make a screenshot
//

EFI_STATUS egScreenShot(VOID)
{return 0;}

//
// Sets mode via GOP protocol, and reconnects simple text out drivers
//

static EFI_STATUS GopSetModeAndReconnectTextOut(IN UINT32 ModeNumber)
{return 0;}

/* EOF */
