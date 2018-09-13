//Slice 2013

#include "Platform.h"

#ifndef DEBUG_ALL
#define DEBUG_EDID 1
#else
#define DEBUG_EDID DEBUG_ALL
#endif

#define DBG(...) DebugLog(1, __VA_ARGS__)

EFI_STATUS EFIAPI GetEdidImpl(
                              IN  EFI_EDID_OVERRIDE_PROTOCOL          *This,
                              IN  EFI_HANDLE                          *ChildHandle,
                              OUT UINT32                              *Attributes,
                              IN OUT UINTN                            *EdidSize,
                              IN OUT UINT8                            **Edid
                              )
{return 0;}

EFI_EDID_OVERRIDE_PROTOCOL gEdidOverride =
{
  GetEdidImpl
};

EFI_STATUS
InitializeEdidOverride ()
{return 0;}

//used only if VBiosPatchNeeded and if no CustomEDID
UINT8* getCurrentEdid (VOID)
{return 0;}

VOID DebugDumpEDID(CONST CHAR8 *Message, INTN N)
{}

//Used at OS start
// if EFI_SUCCESS then result in gSettings.CustomEDID != NULL
// first priority is CustomEDID
// second is UEFI EDID from EdidDiscoveredProtocol
EFI_STATUS GetEdidDiscovered(VOID)
{return 0;}
