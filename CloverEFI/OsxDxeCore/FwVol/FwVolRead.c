/** @file
  Implements functions to read firmware file

Copyright (c) 2006 - 2012, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "DxeMain.h"
#include "FwVolDriver.h"

/**
Required Alignment             Alignment Value in FFS         Alignment Value in
(bytes)                        Attributes Field               Firmware Volume Interfaces
1                                    0                                     0
16                                   1                                     4
128                                  2                                     7
512                                  3                                     9
1 KB                                 4                                     10
4 KB                                 5                                     12
32 KB                                6                                     15
64 KB                                7                                     16
**/
UINT8 mFvAttributes[] = {0, 4, 7, 9, 10, 12, 15, 16};

/**
  Convert the FFS File Attributes to FV File Attributes

  @param  FfsAttributes              The attributes of UINT8 type.

  @return The attributes of EFI_FV_FILE_ATTRIBUTES

**/
EFI_FV_FILE_ATTRIBUTES
FfsAttributes2FvFileAttributes (
  IN EFI_FFS_FILE_ATTRIBUTES FfsAttributes
  )
{
  UINT8                     DataAlignment;
  EFI_FV_FILE_ATTRIBUTES    FileAttribute;

  DataAlignment = (UINT8) ((FfsAttributes & FFS_ATTRIB_DATA_ALIGNMENT) >> 3);
  ASSERT (DataAlignment < 8);

  FileAttribute = (EFI_FV_FILE_ATTRIBUTES) mFvAttributes[DataAlignment];

  if ((FfsAttributes & FFS_ATTRIB_FIXED) == FFS_ATTRIB_FIXED) {
    FileAttribute |= EFI_FV_FILE_ATTRIB_FIXED;
}

  return FileAttribute;
}

/**
  Given the input key, search for the next matching file in the volume.

  @param  This                       Indicates the calling context.
  @param  Key                        Key is a pointer to a caller allocated
                                     buffer that contains implementation specific
                                     data that is used to track where to begin
                                     the search for the next file. The size of
                                     the buffer must be at least This->KeySize
                                     bytes long. To reinitialize the search and
                                     begin from the beginning of the firmware
                                     volume, the entire buffer must be cleared to
                                     zero. Other than clearing the buffer to
                                     initiate a new search, the caller must not
                                     modify the data in the buffer between calls
                                     to GetNextFile().
  @param  FileType                   FileType is a pointer to a caller allocated
                                     EFI_FV_FILETYPE. The GetNextFile() API can
                                     filter it's search for files based on the
                                     value of *FileType input. A *FileType input
                                     of 0 causes GetNextFile() to search for
                                     files of all types.  If a file is found, the
                                     file's type is returned in *FileType.
                                     *FileType is not modified if no file is
                                     found.
  @param  NameGuid                   NameGuid is a pointer to a caller allocated
                                     EFI_GUID. If a file is found, the file's
                                     name is returned in *NameGuid.  *NameGuid is
                                     not modified if no file is found.
  @param  Attributes                 Attributes is a pointer to a caller
                                     allocated EFI_FV_FILE_ATTRIBUTES.  If a file
                                     is found, the file's attributes are returned
                                     in *Attributes. *Attributes is not modified
                                     if no file is found.
  @param  Size                       Size is a pointer to a caller allocated
                                     UINTN. If a file is found, the file's size
                                     is returned in *Size. *Size is not modified
                                     if no file is found.

  @retval EFI_SUCCESS                Successfully find the file.
  @retval EFI_DEVICE_ERROR           Device error.
  @retval EFI_ACCESS_DENIED          Fv could not read.
  @retval EFI_NOT_FOUND              No matching file found.
  @retval EFI_INVALID_PARAMETER      Invalid parameter

**/
EFI_STATUS
EFIAPI
FvGetNextFile (
  IN CONST   EFI_FIRMWARE_VOLUME2_PROTOCOL *This,
  IN OUT     VOID                          *Key,
  IN OUT     EFI_FV_FILETYPE               *FileType,
  OUT        EFI_GUID                      *NameGuid,
  OUT        EFI_FV_FILE_ATTRIBUTES        *Attributes,
  OUT        UINTN                         *Size
  )
{
  EFI_STATUS                                  Status;
  FV_DEVICE                                   *FvDevice;
  EFI_FV_ATTRIBUTES                           FvAttributes;
  EFI_FFS_FILE_HEADER                         *FfsFileHeader;
  UINTN                                       *KeyValue;
  LIST_ENTRY                                  *Link;
  FFS_FILE_LIST_ENTRY                         *FfsFileEntry;

  FvDevice = FV_DEVICE_FROM_THIS (This);

  Status = FvGetVolumeAttributes (This, &FvAttributes);
  if (EFI_ERROR (Status)){
    return Status;
  }

  //
  // Check if read operation is enabled
  //
  if ((FvAttributes & EFI_FV2_READ_STATUS) == 0) {
    return EFI_ACCESS_DENIED;
  }

  if (*FileType > EFI_FV_FILETYPE_SMM_CORE) {
    //
    // File type needs to be in 0 - 0x0D
    //
    return EFI_NOT_FOUND;
  }

  KeyValue = (UINTN *)Key;
  for (;;) {
    if (*KeyValue == 0) {
      //
      // Search for 1st matching file
      //
      Link = &FvDevice->FfsFileListHeader;
    } else {
      //
      // Key is pointer to FFsFileEntry, so get next one
      //
      Link = (LIST_ENTRY *)(*KeyValue);
    }

    if (Link->ForwardLink == &FvDevice->FfsFileListHeader) {
      //
      // Next is end of list so we did not find data
      //
      return EFI_NOT_FOUND;
    }

    FfsFileEntry = (FFS_FILE_LIST_ENTRY *)Link->ForwardLink;
    FfsFileHeader = (EFI_FFS_FILE_HEADER *)FfsFileEntry->FfsHeader;

    //
    // remember the key
    //
    *KeyValue = (UINTN)FfsFileEntry;

    if (FfsFileHeader->Type == EFI_FV_FILETYPE_FFS_PAD) {
      //
      // we ignore pad files
      //
      continue;
    }

    if (*FileType == EFI_FV_FILETYPE_ALL) {
      //
      // Process all file types so we have a match
      //
      break;
    }

    if (*FileType == FfsFileHeader->Type) {
      //
      // Found a matching file type
      //
      break;
    }

  }

  //
  // Return FileType, NameGuid, and Attributes
  //
  *FileType = FfsFileHeader->Type;
  CopyGuid (NameGuid, &FfsFileHeader->Name);
  *Attributes = FfsAttributes2FvFileAttributes (FfsFileHeader->Attributes);
  if ((FvDevice->FwVolHeader->Attributes & EFI_FVB2_MEMORY_MAPPED) == EFI_FVB2_MEMORY_MAPPED) {
    *Attributes |= EFI_FV_FILE_ATTRIB_MEMORY_MAPPED;
  }

  //
  // we need to substract the header size
  //
  if (IS_FFS_FILE2 (FfsFileHeader)) {
    *Size = FFS_FILE2_SIZE (FfsFileHeader) - sizeof (EFI_FFS_FILE_HEADER2);
  } else {
    *Size = FFS_FILE_SIZE (FfsFileHeader) - sizeof (EFI_FFS_FILE_HEADER);
  }

  return EFI_SUCCESS;
}



/**
  Locates a file in the firmware volume and
  copies it to the supplied buffer.

  @param  This                       Indicates the calling context.
  @param  NameGuid                   Pointer to an EFI_GUID, which is the
                                     filename.
  @param  Buffer                     Buffer is a pointer to pointer to a buffer
                                     in which the file or section contents or are
                                     returned.
  @param  BufferSize                 BufferSize is a pointer to caller allocated
                                     UINTN. On input *BufferSize indicates the
                                     size in bytes of the memory region pointed
                                     to by Buffer. On output, *BufferSize
                                     contains the number of bytes required to
                                     read the file.
  @param  FoundType                  FoundType is a pointer to a caller allocated
                                     EFI_FV_FILETYPE that on successful return
                                     from Read() contains the type of file read.
                                     This output reflects the file type
                                     irrespective of the value of the SectionType
                                     input.
  @param  FileAttributes             FileAttributes is a pointer to a caller
                                     allocated EFI_FV_FILE_ATTRIBUTES.  On
                                     successful return from Read(),
                                     *FileAttributes contains the attributes of
                                     the file read.
  @param  AuthenticationStatus       AuthenticationStatus is a pointer to a
                                     caller allocated UINTN in which the
                                     authentication status is returned.

  @retval EFI_SUCCESS                Successfully read to memory buffer.
  @retval EFI_WARN_BUFFER_TOO_SMALL  Buffer too small.
  @retval EFI_NOT_FOUND              Not found.
  @retval EFI_DEVICE_ERROR           Device error.
  @retval EFI_ACCESS_DENIED          Could not read.
  @retval EFI_INVALID_PARAMETER      Invalid parameter.
  @retval EFI_OUT_OF_RESOURCES       Not enough buffer to be allocated.

**/
EFI_STATUS
EFIAPI
FvReadFile (
  IN CONST EFI_FIRMWARE_VOLUME2_PROTOCOL *This,
  IN CONST EFI_GUID                      *NameGuid,
  IN OUT   VOID                          **Buffer,
  IN OUT   UINTN                         *BufferSize,
  OUT      EFI_FV_FILETYPE               *FoundType,
  OUT      EFI_FV_FILE_ATTRIBUTES        *FileAttributes,
  OUT      UINT32                        *AuthenticationStatus
  )
{
  EFI_STATUS                        Status;
  FV_DEVICE                         *FvDevice;
  EFI_GUID                          SearchNameGuid;
  EFI_FV_FILETYPE                   LocalFoundType;
  EFI_FV_FILE_ATTRIBUTES            LocalAttributes;
  UINTN                             FileSize;
  UINT8                             *SrcPtr;
  EFI_FFS_FILE_HEADER               *FfsHeader;
  UINTN                             InputBufferSize;

  if (NameGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  FvDevice = FV_DEVICE_FROM_THIS (This);


  //
  // Keep looking until we find the matching NameGuid.
  // The Key is really an FfsFileEntry
  //
  FvDevice->LastKey = 0;
  do {
    LocalFoundType = 0;
    Status = FvGetNextFile (
              This,
              &FvDevice->LastKey,
              &LocalFoundType,
              &SearchNameGuid,
              &LocalAttributes,
              &FileSize
              );
    if (EFI_ERROR (Status)) {
      return EFI_NOT_FOUND;
    }
  } while (!CompareGuid (&SearchNameGuid, NameGuid));

  //
  // Get a pointer to the header
  //
  FfsHeader = FvDevice->LastKey->FfsHeader;

  //
  // Remember callers buffer size
  //
  InputBufferSize = *BufferSize;

  //
  // Calculate return values
  //
  *FoundType = FfsHeader->Type;
  *FileAttributes = FfsAttributes2FvFileAttributes (FfsHeader->Attributes);
   if ((FvDevice->FwVolHeader->Attributes & EFI_FVB2_MEMORY_MAPPED) == EFI_FVB2_MEMORY_MAPPED) {
     *FileAttributes |= EFI_FV_FILE_ATTRIB_MEMORY_MAPPED;
   }
  *AuthenticationStatus = 0;
  *BufferSize = FileSize;

  if (Buffer == NULL) {
    //
    // If Buffer is NULL, we only want to get the information collected so far
    //
    return EFI_SUCCESS;
  }

  //
  // Skip over file header
  //
  if (IS_FFS_FILE2 (FfsHeader)) {
    SrcPtr = ((UINT8 *) FfsHeader) + sizeof (EFI_FFS_FILE_HEADER2);
  } else {
    SrcPtr = ((UINT8 *) FfsHeader) + sizeof (EFI_FFS_FILE_HEADER);
  }

  Status = EFI_SUCCESS;
  if (*Buffer == NULL) {
    //
    // Caller passed in a pointer so allocate buffer for them
    //
    *Buffer = AllocatePool (FileSize);
    if (*Buffer == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
  } else if (FileSize > InputBufferSize) {
    //
    // Callers buffer was not big enough
    //
    Status = EFI_WARN_BUFFER_TOO_SMALL;
    FileSize = InputBufferSize;
  }

  //
  // Copy data into callers buffer
  //
  CopyMem (*Buffer, SrcPtr, FileSize);

  return Status;
}



/**
  Locates a section in a given FFS File and
  copies it to the supplied buffer (not including section header).

  @param  This                       Indicates the calling context.
  @param  NameGuid                   Pointer to an EFI_GUID, which is the
                                     filename.
  @param  SectionType                Indicates the section type to return.
  @param  SectionInstance            Indicates which instance of sections with a
                                     type of SectionType to return.
  @param  Buffer                     Buffer is a pointer to pointer to a buffer
                                     in which the file or section contents or are
                                     returned.
  @param  BufferSize                 BufferSize is a pointer to caller allocated
                                     UINTN.
  @param  AuthenticationStatus       AuthenticationStatus is a pointer to a
                                     caller allocated UINT32 in which the
                                     authentication status is returned.

  @retval EFI_SUCCESS                Successfully read the file section into
                                     buffer.
  @retval EFI_WARN_BUFFER_TOO_SMALL  Buffer too small.
  @retval EFI_NOT_FOUND              Section not found.
  @retval EFI_DEVICE_ERROR           Device error.
  @retval EFI_ACCESS_DENIED          Could not read.
  @retval EFI_INVALID_PARAMETER      Invalid parameter.

**/
EFI_STATUS
EFIAPI
FvReadFileSection (
  IN CONST  EFI_FIRMWARE_VOLUME2_PROTOCOL  *This,
  IN CONST  EFI_GUID                       *NameGuid,
  IN        EFI_SECTION_TYPE               SectionType,
  IN        UINTN                          SectionInstance,
  IN OUT    VOID                           **Buffer,
  IN OUT    UINTN                          *BufferSize,
  OUT       UINT32                         *AuthenticationStatus
  )
{
  EFI_STATUS                        Status;
  FV_DEVICE                         *FvDevice;
  EFI_FV_FILETYPE                   FileType;
  EFI_FV_FILE_ATTRIBUTES            FileAttributes;
  UINTN                             FileSize;
  UINT8                             *FileBuffer;
  FFS_FILE_LIST_ENTRY               *FfsEntry;

  if (NameGuid == NULL || Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  FvDevice = FV_DEVICE_FROM_THIS (This);

  //
  // Read the whole file into buffer
  //
  FileBuffer = NULL;
  Status = FvReadFile (
            This,
            NameGuid,
            (VOID **)&FileBuffer,
            &FileSize,
            &FileType,
            &FileAttributes,
            AuthenticationStatus
            );
  //
  // Get the last key used by our call to FvReadFile as it is the FfsEntry for this file.
  //
  FfsEntry = (FFS_FILE_LIST_ENTRY *) FvDevice->LastKey;

  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Check to see that the file actually HAS sections before we go any further.
  //
  if (FileType == EFI_FV_FILETYPE_RAW) {
    Status = EFI_NOT_FOUND;
    goto Done;
  }

  //
  // Use FfsEntry to cache Section Extraction Protocol Information
  //
  if (FfsEntry->StreamHandle == 0) {
    Status = OpenSectionStream (
               FileSize,
               FileBuffer,
               &FfsEntry->StreamHandle
               );
    if (EFI_ERROR (Status)) {
      goto Done;
    }
  }

  //
  // If SectionType == 0 We need the whole section stream
  //
  Status = GetSection (
             FfsEntry->StreamHandle,
             (SectionType == 0) ? NULL : &SectionType,
             NULL,
             (SectionType == 0) ? 0 : SectionInstance,
             Buffer,
             BufferSize,
             AuthenticationStatus,
             FvDevice->IsFfs3Fv
             );

  if (!EFI_ERROR (Status)) {
    //
    // Inherit the authentication status.
    //
    *AuthenticationStatus |= FvDevice->AuthenticationStatus;
  }

  //
  // Close of stream defered to close of FfsHeader list to allow SEP to cache data
  //

Done:
  CoreFreePool (FileBuffer);

  return Status;
}


