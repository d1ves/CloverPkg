//
//  Pointer.c
//  
//
//  Created by Slice on 23.09.12.
//
// Initial idea comes from iBoot project by OS_Ninja and Ujen
// their sources are under GNU License but I don't know what is the subject for licensing here.
// my sources are quite different while Mouse/Events interfaces comes from Tiano,
// for example ConSplitterDxe or BdsDxe/FrontPage
// anyway thanks for good tutorial how to do and how not to do
// 
// Any usage for SMBIOS here?
/// Built-in Pointing Device (Type 21).

//#include "Platform.h"
#include "libegint.h"   //this includes platform.h 

#ifndef DEBUG_ALL
#define DEBUG_MOUSE 1
#else
#define DEBUG_MOUSE DEBUG_ALL
#endif

#define DBG(...) DebugLog(1, __VA_ARGS__)


// make them theme dependent? No, 32 is good value for all.
#define POINTER_WIDTH  32
#define POINTER_HEIGHT 32

ACTION gAction;
UINTN  gItemID;

POINTERS gPointer = {NULL, NULL, NULL, NULL,
                     {0, 0, POINTER_WIDTH, POINTER_HEIGHT},
                     {0, 0, POINTER_WIDTH, POINTER_HEIGHT}, 0,
                     {0, 0, 0, FALSE, FALSE}, NoEvents};

VOID HidePointer()
{}

VOID DrawPointer()
{}

VOID RedrawPointer()
{}

EFI_STATUS MouseBirth()
{return 0;}

VOID KillMouse()
{}

// input - tsc
// output - milliseconds
// the caller is responsible for t1 > t0
UINT64 TimeDiff(UINT64 t0, UINT64 t1) 
{
  return DivU64x64Remainder((t1 - t0), DivU64x32(gCPUStructure.TSCFrequency, 1000), 0);
}

VOID PrintPointerVars(
                      INT32     RelX,
                      INT32     RelY,
                      INTN      ScreenRelX,
                      INTN      ScreenRelY,
                      INTN      XPosPrev,
                      INTN      YPosPrev,
                      INTN      XPos,
                      INTN      YPos
                      )
{}

//static INTN PrintCount = 0;

VOID UpdatePointer()
{}

BOOLEAN MouseInRect(EG_RECT *Place)
{
  return  ((gPointer.newPlace.XPos >= Place->XPos) &&
           (gPointer.newPlace.XPos < (Place->XPos + (INTN)Place->Width)) &&
           (gPointer.newPlace.YPos >= Place->YPos) &&
           (gPointer.newPlace.YPos < (Place->YPos + (INTN)Place->Height)));
}

EFI_STATUS CheckMouseEvent(REFIT_MENU_SCREEN *Screen)
{}

#define ONE_SECOND  10000000
#define ONE_MSECOND    10000

// TimeoutDefault for a wait in seconds
// return EFI_TIMEOUT if no inputs
EFI_STATUS WaitForInputEventPoll(REFIT_MENU_SCREEN *Screen, UINTN TimeoutDefault)
{return 0;}

