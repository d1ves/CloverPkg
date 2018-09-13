/*
 * refit/screen.c
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

#ifndef DEBUG_ALL
#define DEBUG_SCR 1
#else
#define DEBUG_SCR DEBUG_ALL
#endif

#define DBG(...) DebugLog(1, __VA_ARGS__)

// Console defines and variables

UINTN ConWidth;
UINTN ConHeight;
CHAR16 *BlankLine = NULL;

static VOID SwitchToText(IN BOOLEAN CursorEnabled);
static VOID SwitchToGraphics(VOID);
static VOID DrawScreenHeader(IN CHAR16 *Title);
static VOID UpdateConsoleVars(VOID);
static INTN ConvertEdgeAndPercentageToPixelPosition(INTN Edge, INTN DesiredPercentageFromEdge, INTN ImageDimension, INTN ScreenDimension);
static INTN CalculateNudgePosition(INTN Position, INTN NudgeValue, INTN ImageDimension, INTN ScreenDimension);
//INTN RecalculateImageOffset(INTN AnimDimension, INTN ValueToScale, INTN ScreenDimensionToFit, INTN ThemeDesignDimension);
static BOOLEAN IsImageWithinScreenLimits(INTN Value, INTN ImageDimension, INTN ScreenDimension);
static INTN RepositionFixedByCenter(INTN Value, INTN ScreenDimension, INTN DesignScreenDimension);
static INTN RepositionRelativeByGapsOnEdges(INTN Value, INTN ImageDimension, INTN ScreenDimension, INTN DesignScreenDimension);
static INTN HybridRepositioning(INTN Edge, INTN Value, INTN ImageDimension, INTN ScreenDimension, INTN DesignScreenDimension);

// UGA defines and variables

INTN   UGAWidth;
INTN   UGAHeight;
BOOLEAN AllowGraphicsMode;

EG_RECT  BannerPlace = {0, 0, 0, 0};

EG_PIXEL StdBackgroundPixel   = { 0xbf, 0xbf, 0xbf, 0xff};
EG_PIXEL MenuBackgroundPixel  = { 0x00, 0x00, 0x00, 0x00};
EG_PIXEL InputBackgroundPixel = { 0xcf, 0xcf, 0xcf, 0x80};
EG_PIXEL BlueBackgroundPixel  = { 0x7f, 0x0f, 0x0f, 0xff};
EG_PIXEL EmbeddedBackgroundPixel  = { 0xaa, 0xaa, 0xaa, 0x80};

EG_IMAGE *BackgroundImage = NULL;
EG_IMAGE *Banner = NULL;
EG_IMAGE *BigBack = NULL;

static BOOLEAN GraphicsScreenDirty;

// general defines and variables

static BOOLEAN haveError = FALSE;

//
// Screen initialization and switching
//

VOID InitScreen(IN BOOLEAN SetMaxResolution)
{}

VOID SetupScreen(VOID)
{}

static VOID SwitchToText(IN BOOLEAN CursorEnabled)
{}

static VOID SwitchToGraphics(VOID)
{}

//
// Screen control for running tools
//
VOID BeginTextScreen(IN CHAR16 *Title)
{}

VOID FinishTextScreen(IN BOOLEAN WaitAlways)
{}

VOID BeginExternalScreen(IN BOOLEAN UseGraphicsMode, IN CHAR16 *Title)
{}

VOID FinishExternalScreen(VOID)
{}

VOID TerminateScreen(VOID)
{}

static VOID DrawScreenHeader(IN CHAR16 *Title)
{}

//
// Keyboard input
//

BOOLEAN ReadAllKeyStrokes(VOID)
{return 0;}

VOID PauseForKey(CHAR16* msg)
{}

#if REFIT_DEBUG > 0
VOID DebugPause(VOID)
{
    // show console and wait for key
    SwitchToText(FALSE);
    PauseForKey(L"");
    
    // reset error flag
    haveError = FALSE;
}
#endif

VOID EndlessIdleLoop(VOID)
{}

//
// Error handling
//
/*
VOID
StatusToString (
				OUT CHAR16      *Buffer,
				EFI_STATUS      Status
				)
{
	UnicodeSPrint(Buffer, 64, L"EFI Error %r", Status);
}*/


BOOLEAN CheckFatalError(IN EFI_STATUS Status, IN CHAR16 *where)
{return 0;}

BOOLEAN CheckError(IN EFI_STATUS Status, IN CHAR16 *where)
{return 0;}

//
// Graphics functions
//

VOID SwitchToGraphicsAndClear(VOID)
{}

/*
typedef struct {
  INTN     XPos;
  INTN     YPos;
  INTN     Width;
  INTN     Height;
} EG_RECT;
*/


VOID BltClearScreen(IN BOOLEAN ShowBanner) //ShowBanner always TRUE
{}

VOID BltImage(IN EG_IMAGE *Image, IN INTN XPos, IN INTN YPos)
{}

VOID BltImageAlpha(IN EG_IMAGE *Image, IN INTN XPos, IN INTN YPos, IN EG_PIXEL *BackgroundPixel, INTN Scale)
{}

VOID BltImageComposite(IN EG_IMAGE *BaseImage, IN EG_IMAGE *TopImage, IN INTN XPos, IN INTN YPos)
{}

/*
  --------------------------------------------------------------------
  Pos                           : Bottom    -> Mid        -> Top
  --------------------------------------------------------------------
   GlobalConfig.SelectionOnTop  : MainImage -> Badge      -> Selection
  !GlobalConfig.SelectionOnTop  : Selection -> MainImage  -> Badge
*/

VOID BltImageCompositeBadge(IN EG_IMAGE *BaseImage, IN EG_IMAGE *TopImage, IN EG_IMAGE *BadgeImage, IN INTN XPos, IN INTN YPos, INTN Scale)
{}
    
#define MAX_SIZE_ANIME 256

VOID FreeAnime(GUI_ANIME *Anime)
{}

/* Replaced for now with Reposition* below
INTN RecalculateImageOffset(INTN AnimDimension, INTN ValueToScale, INTN ScreenDimensionToFit, INTN ThemeDesignDimension)
{
    INTN SuppliedGapDimensionPxDesigned=0;
    INTN OppositeGapDimensionPxDesigned=0;
    INTN OppositeGapPcDesigned=0;
    INTN ScreenDimensionLessAnim=0;
    INTN GapNumTimesLarger=0;
    INTN GapNumFinal=0;
    INTN NewSuppliedGapPx=0;
    INTN NewOppositeGapPx=0;
    INTN ReturnValue=0;
    
    SuppliedGapDimensionPxDesigned = (ThemeDesignDimension * ValueToScale) / 100;
    OppositeGapDimensionPxDesigned = ThemeDesignDimension - (SuppliedGapDimensionPxDesigned + AnimDimension);
    OppositeGapPcDesigned = (OppositeGapDimensionPxDesigned * 100)/ThemeDesignDimension;
    ScreenDimensionLessAnim = (ScreenDimensionToFit - AnimDimension);
    if (ValueToScale > OppositeGapPcDesigned) {
      GapNumTimesLarger = (ValueToScale * 100)/OppositeGapPcDesigned;
      GapNumFinal = GapNumTimesLarger + 100;
      NewOppositeGapPx = (ScreenDimensionLessAnim * 100)/GapNumFinal;
      NewSuppliedGapPx = (NewOppositeGapPx * GapNumTimesLarger)/100;
    } else if (ValueToScale < OppositeGapPcDesigned) {
      GapNumTimesLarger = (OppositeGapPcDesigned * 100)/ValueToScale;
      GapNumFinal = (GapNumTimesLarger + 100);
      NewSuppliedGapPx = (ScreenDimensionLessAnim * 100)/GapNumFinal;
      NewOppositeGapPx = (NewSuppliedGapPx * GapNumTimesLarger)/100;
    } else if (ValueToScale == OppositeGapPcDesigned) {
      NewSuppliedGapPx = (ScreenDimensionLessAnim * 100)/200;
      NewOppositeGapPx = (NewSuppliedGapPx * 100)/100;
    }
    ReturnValue = (NewSuppliedGapPx * 100)/ScreenDimensionToFit;
    
    if (ReturnValue>0 && ReturnValue<100) {
      //DBG("Different screen size being used. Adjusted original anim gap to %d\n",ReturnValue);
      return ReturnValue;
    } else {
      DBG("Different screen size being used. Adjusted value %d invalid. Returning original value %d\n",ReturnValue, ValueToScale);
      return ValueToScale;
    }
}
*/

static INTN ConvertEdgeAndPercentageToPixelPosition(INTN Edge, INTN DesiredPercentageFromEdge, INTN ImageDimension, INTN ScreenDimension)
{return 0;}

static INTN CalculateNudgePosition(INTN Position, INTN NudgeValue, INTN ImageDimension, INTN ScreenDimension)
{return 0;}

static BOOLEAN IsImageWithinScreenLimits(INTN Value, INTN ImageDimension, INTN ScreenDimension)
{return 0;}

static INTN RepositionFixedByCenter(INTN Value, INTN ScreenDimension, INTN DesignScreenDimension)
{return 0;}

static INTN RepositionRelativeByGapsOnEdges(INTN Value, INTN ImageDimension, INTN ScreenDimension, INTN DesignScreenDimension)
{return 0;}

static INTN HybridRepositioning(INTN Edge, INTN Value, INTN ImageDimension, INTN ScreenDimension, INTN DesignScreenDimension)
{return 0;}

static EG_IMAGE *AnimeImage = NULL;

VOID UpdateAnime(REFIT_MENU_SCREEN *Screen, EG_RECT *Place)
{}


VOID InitAnime(REFIT_MENU_SCREEN *Screen)
{}

BOOLEAN GetAnime(REFIT_MENU_SCREEN *Screen)
{return 0;}

//
// Sets next/previous available screen resolution, according to specified offset
//

VOID SetNextScreenMode(INT32 Next)
{}

//
// Updates console variables, according to ConOut resolution 
// This should be called when initializing screen, or when resolution changes
//

static VOID UpdateConsoleVars()
{}
