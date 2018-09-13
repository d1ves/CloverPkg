/*
 * libeg/image.c
 * Image handling functions
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


#define MAX_FILE_SIZE (1024*1024*1024)

#ifndef DEBUG_ALL
#define DEBUG_IMG 1
#else
#define DEBUG_IMG DEBUG_ALL
#endif

#define DBG(...) DebugLog(1, __VA_ARGS__)

//
// Basic image handling
//

EG_IMAGE * egCreateImage(IN INTN Width, IN INTN Height, IN BOOLEAN HasAlpha)
{return 0;}

EG_IMAGE * egCreateFilledImage(IN INTN Width, IN INTN Height, IN BOOLEAN HasAlpha, IN EG_PIXEL *Color)
{return 0;}

EG_IMAGE * egCopyImage(IN EG_IMAGE *Image)
{return 0;}

//Scaling functions
EG_IMAGE * egCopyScaledImage(IN EG_IMAGE *OldImage, IN INTN Ratio) //will be N/16
{return 0;}

BOOLEAN BigDiff(UINT8 a, UINT8 b)
{
  if (a > b) {
    if (!GlobalConfig.BackgroundDark) {
      return (a - b) > (UINT8)(0xFF - GlobalConfig.BackgroundSharp);
    }
  } else if (GlobalConfig.BackgroundDark) {
    return (b - a) > (UINT8)(0xFF - GlobalConfig.BackgroundSharp);
  }
  return 0;
}
//(c)Slice 2013
#define EDGE(P) \
do { \
  if (BigDiff(a11.P, a10.P)) { \
    if (!BigDiff(a11.P, a01.P) && !BigDiff(a11.P, a21.P)) { \
      a10.P = a11.P; \
    } else if (BigDiff(a11.P, a01.P)) { \
      if ((dx + dy) < cell) { \
        a11.P = a21.P = a12.P = (UINT8)((a10.P * (cell - dy + dx) + a01.P * (cell - dx + dy)) / (cell * 2)); \
      } else { \
        a10.P = a01.P = a11.P; \
      } \
    } else if (BigDiff(a11.P, a21.P)) { \
      if (dx > dy) { \
        a11.P = a01.P = a12.P = (UINT8)((a10.P * (cell * 2 - dy - dx) + a21.P * (dx + dy)) / (cell * 2)); \
      }else { \
        a10.P = a21.P = a11.P; \
      } \
    } \
  } else if (BigDiff(a11.P, a21.P)) { \
    if (!BigDiff(a11.P, a12.P)){ \
      a21.P = a11.P; \
    } else { \
      if ((dx + dy) > cell) { \
        a11.P = a01.P = a10.P = (UINT8)((a21.P * (cell + dx - dy) + a12.P * (cell - dx + dy)) / (cell * 2)); \
      } else { \
        a21.P = a12.P = a11.P; \
      } \
    } \
  } else if (BigDiff(a11.P, a01.P)) { \
    if (!BigDiff(a11.P, a12.P)){ \
      a01.P = a11.P; \
    } else { \
      if (dx < dy) { \
        a11.P = a21.P = a10.P = (UINT8)((a01.P * (cell * 2 - dx - dy) + a12.P * (dy + dx )) / (cell * 2)); \
      } else { \
        a01.P = a12.P = a11.P; \
      } \
    } \
  } else if (BigDiff(a11.P, a12.P)) { \
    a12.P = a11.P; \
  } \
} while(0)

#define SMOOTH(P) \
do { \
  norm = (INTN)a01.P + a10.P + 4 * a11.P + a12.P + a21.P; \
  if (norm == 0) { \
    Dest->P = 0; \
  } else { \
    Dest->P = (UINT8)(a11.P * 2 * (a01.P * (cell - dx) + a10.P * (cell - dy) + \
                      a21.P * dx + a12.P * dy + a11.P * 2 * cell) / (cell * norm)); \
  } \
} while(0)

#define SMOOTH2(P) \
do { \
     Dest->P = (UINT8)((a01.P * (cell - dx) * 3 + a10.P * (cell - dy) * 3 + \
                        a21.P * dx * 3 + a12.P * dy * 3 + a11.P * 2 * cell) / (cell * 8)); \
} while(0)


VOID  ScaleImage(OUT EG_IMAGE *NewImage, IN EG_IMAGE *OldImage)
{}
//

VOID egFreeImage(IN EG_IMAGE *Image)
{
  if (Image != NULL) {
    if (Image->PixelData != NULL) {
      FreePool(Image->PixelData);
      Image->PixelData = NULL; //FreePool will not zero pointer
    }
    FreePool(Image);
  }
}

//
// Basic file operations
//

EFI_STATUS egLoadFile(IN EFI_FILE_HANDLE BaseDir, IN CHAR16 *FileName,
                      OUT UINT8 **FileData, OUT UINTN *FileDataLength)
{
    EFI_STATUS          Status;
    EFI_FILE_HANDLE     FileHandle;
    EFI_FILE_INFO       *FileInfo;
    UINT64              ReadSize;
    UINTN               BufferSize;
    UINT8               *Buffer;

    Status = BaseDir->Open(BaseDir, &FileHandle, FileName, EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(Status))
        return Status;

    FileInfo = EfiLibFileInfo(FileHandle);
    if (FileInfo == NULL) {
        FileHandle->Close(FileHandle);
        return EFI_NOT_FOUND;
    }
    ReadSize = FileInfo->FileSize;
    if (ReadSize > MAX_FILE_SIZE)
        ReadSize = MAX_FILE_SIZE;
    FreePool(FileInfo);

    BufferSize = (UINTN)ReadSize;   // was limited to 1 GB above, so this is safe
    Buffer = (UINT8 *) AllocatePool (BufferSize);
    if (Buffer == NULL) {
        FileHandle->Close(FileHandle);
        return EFI_OUT_OF_RESOURCES;
    }

    Status = FileHandle->Read(FileHandle, &BufferSize, Buffer);
    FileHandle->Close(FileHandle);
    if (EFI_ERROR(Status)) {
        FreePool(Buffer);
        return Status;
    }

    *FileData = Buffer;
    *FileDataLength = BufferSize;
    return EFI_SUCCESS;
}
//Slice - this is gEfiPartTypeSystemPartGuid
//static EFI_GUID ESPGuid = { 0xc12a7328, 0xf81f, 0x11d2, { 0xba, 0x4b, 0x00, 0xa0, 0xc9, 0x3e, 0xc9, 0x3b } };
//there is assumed only one ESP partition. What if there are two HDD gpt formatted?
EFI_STATUS egFindESP(OUT EFI_FILE_HANDLE *RootDir)
{
    EFI_STATUS          Status;
    UINTN               HandleCount = 0;
    EFI_HANDLE          *Handles;

    Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiPartTypeSystemPartGuid, NULL, &HandleCount, &Handles);
    if (!EFI_ERROR(Status) && HandleCount > 0) {
        *RootDir = EfiLibOpenRoot(Handles[0]);
        if (*RootDir == NULL)
            Status = EFI_NOT_FOUND;
        FreePool(Handles);
    }
    return Status;
}
//if (NULL, ...) then save to EFI partition
EFI_STATUS egSaveFile(IN EFI_FILE_HANDLE BaseDir OPTIONAL, IN CHAR16 *FileName,
                      IN UINT8 *FileData, IN UINTN FileDataLength)
{
  EFI_STATUS          Status;
  EFI_FILE_HANDLE     FileHandle;
  UINTN               BufferSize;
  BOOLEAN             CreateNew = TRUE;
  CHAR16              *p = FileName + StrLen(FileName);
  CHAR16              DirName[256];
  UINTN               dirNameLen;

  if (BaseDir == NULL) {
    Status = egFindESP(&BaseDir);
    if (EFI_ERROR(Status))
      return Status;
  }
    
  // syscl - make directory if not exist
  while (*p != L'\\' && p >= FileName) {
    // find the first '\\' traverse from the end to head of FileName
    p -= 1;
  }
  dirNameLen = p - FileName;
  StrnCpy(DirName, FileName, dirNameLen);
  Status = BaseDir->Open(BaseDir, &FileHandle, DirName,
                           EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, EFI_FILE_DIRECTORY);
    
  if (EFI_ERROR(Status)) {
      // make dir
      Status = BaseDir->Open(BaseDir, &FileHandle, DirName,
                               EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE, EFI_FILE_DIRECTORY);
  }
  // end of folder checking

  // Delete existing file if it exists
  Status = BaseDir->Open(BaseDir, &FileHandle, FileName,
                         EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, 0);
  if (!EFI_ERROR(Status)) {
    Status = FileHandle->Delete(FileHandle);
    if (Status == EFI_WARN_DELETE_FAILURE) {
      //This is READ_ONLY file system
      CreateNew = FALSE; // will write into existing file
    }
  }

  if (CreateNew) {
    // Write new file
    Status = BaseDir->Open(BaseDir, &FileHandle, FileName,
                           EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE, 0);
    if (EFI_ERROR(Status))
      return Status;
  } else {
    //to write into existing file we must sure it size larger then our data
    EFI_FILE_INFO *Info = EfiLibFileInfo(FileHandle);
    if (Info) {
      if (Info->FileSize < FileDataLength) {
        return EFI_NOT_FOUND;
      }
      FreePool(Info);
    }
  }

  if (!FileHandle) {
    return EFI_NOT_FOUND;
  }

  BufferSize = FileDataLength;
  Status = FileHandle->Write(FileHandle, &BufferSize, FileData);
  FileHandle->Close(FileHandle);

  return Status;
}


EFI_STATUS egMkDir(IN EFI_FILE_HANDLE BaseDir OPTIONAL, IN CHAR16 *DirName)
{
  EFI_STATUS          Status;
  EFI_FILE_HANDLE     FileHandle;

  //DBG("Looking up dir assets (%s):", DirName);

  if (BaseDir == NULL) {
    Status = egFindESP(&BaseDir);
    if (EFI_ERROR(Status)) {
      //DBG(" %r\n", Status);
      return Status;
    }
  }

  Status = BaseDir->Open(BaseDir, &FileHandle, DirName,
                         EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, EFI_FILE_DIRECTORY);

  if (EFI_ERROR(Status)) {
    // Write new dir
    //DBG("%r, attempt to create one:", Status);
    Status = BaseDir->Open(BaseDir, &FileHandle, DirName,
                           EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE, EFI_FILE_DIRECTORY);
  }

  //DBG(" %r\n", Status);
  return Status;
}

/* always use egDecodePNG
//
// Loading images from files and embedded data
//
static EG_IMAGE * egDecodeAny(IN UINT8 *FileData, IN UINTN FileDataLength,
                              IN CHAR16 *Format, IN UINTN IconSize, IN BOOLEAN WantAlpha)
{
  EG_DECODE_FUNC  DecodeFunc;
  EG_IMAGE        *NewImage;

  if (Format) {
    // dispatch by extension
    DecodeFunc = NULL;

    if (StriCmp(Format, L"PNG") == 0){
      //DBG("decode format PNG\n");
      DecodeFunc = egDecodePNG;
    }
    else if (StriCmp(Format, L"ICNS") == 0){
      //DBG("decode format ICNS\n");
      DecodeFunc = egDecodeICNS;
    } else  if (StriCmp(Format, L"BMP") == 0) {
      //DBG("decode format BMP\n");
      DecodeFunc = egDecodeBMP;
    } 
    //  else if (StriCmp(Format, L"TGA") == 0)
    //    DecodeFunc = egDecodeTGA;

    if (DecodeFunc == NULL)
      return NULL;
    //  DBG("will decode data=%x len=%d icns=%d alpha=%c\n", FileData, FileDataLength, IconSize, WantAlpha?'Y':'N');
    // decode it
    NewImage = DecodeFunc(FileData, FileDataLength, IconSize, WantAlpha);
  } else {

    //automatic choose format
    NewImage = egDecodePNG(FileData, FileDataLength, IconSize, WantAlpha);
    if (!NewImage) {
      DBG(" ..png is wrong try to decode icns\n");
      NewImage = egDecodeICNS(FileData, FileDataLength, IconSize, WantAlpha);
    }  else {
 //     DBG(" ..decoded as png\n");
    } 

    if (!NewImage) {
      DBG(" ..png and icns is wrong try to decode bmp\n");
      NewImage = egDecodeBMP(FileData, FileDataLength, IconSize, WantAlpha);/    }
  }
#if DEBUG_IMG == 2
   PauseForKey(L"After egDecodeAny\n");
#endif
  return NewImage;
}
*/
//caller is responsible for free image
EG_IMAGE * egLoadImage(IN EFI_FILE_HANDLE BaseDir, IN CHAR16 *FileName, IN BOOLEAN WantAlpha)
{
  EFI_STATUS      Status;
  UINT8           *FileData = NULL;
  UINTN           FileDataLength = 0;
  EG_IMAGE        *NewImage;

  if (BaseDir == NULL || FileName == NULL)
    return NULL;

  // load file
  Status = egLoadFile(BaseDir, FileName, &FileData, &FileDataLength);
  //  DBG("File=%s loaded with status=%r length=%d\n", FileName, Status, FileDataLength);
  if (EFI_ERROR(Status))
    return NULL;
  //  DBG("   extension = %s\n", egFindExtension(FileName));
  // decode it
  NewImage = egDecodePNG(FileData, FileDataLength, WantAlpha);

  //  DBG("decoded\n");
  if (!NewImage) {
    DBG("%s not decoded\n", FileName);
  }
  FreePool(FileData);
  //   DBG("FreePool OK\n");
  return NewImage;
}
//caller is responsible for free image
EG_IMAGE * egLoadIcon(IN EFI_FILE_HANDLE BaseDir, IN CHAR16 *FileName, IN UINTN IconSize)
{
  EFI_STATUS      Status;
  UINT8           *FileData;
  UINTN           FileDataLength;
  EG_IMAGE        *NewImage;

  if (BaseDir == NULL || FileName == NULL)
    return NULL;

  //   DBG("egLoadIcon filename: %s\n", FileName);

  // load file
  Status = egLoadFile(BaseDir, FileName, &FileData, &FileDataLength);
  if (EFI_ERROR(Status)) {
//    DBG("egLoadIcon status=%r\n", Status);
    return NULL;
  }

  // decode it
  NewImage = egDecodePNG(FileData, FileDataLength, TRUE);
  if (!NewImage) {
//    DBG("not png, try icns\n");
    NewImage = egDecodeICNS(FileData, FileDataLength, IconSize, TRUE);
  }
  
  FreePool(FileData);

  return NewImage;
}
/*
EG_IMAGE * egDecodeImage(IN UINT8 *FileData, IN UINTN FileDataLength, IN CHAR16 *Format, IN BOOLEAN WantAlpha)
{
  return egDecodeAny(FileData, FileDataLength, Format, 128, WantAlpha);
}
 */
/*
EG_IMAGE * egPrepareEmbeddedImage(IN EG_EMBEDDED_IMAGE *EmbeddedImage, IN BOOLEAN WantAlpha)
{
  EG_IMAGE            *NewImage;
  UINT8               *CompData;
  UINTN               CompLen;
  UINTN               PixelCount;

  // sanity check
  if (EmbeddedImage->PixelMode > EG_MAX_EIPIXELMODE ||
      (EmbeddedImage->CompressMode != EG_EICOMPMODE_NONE && EmbeddedImage->CompressMode != EG_EICOMPMODE_RLE))
    return NULL;

  // allocate image structure and pixel buffer
  NewImage = egCreateImage(EmbeddedImage->Width, EmbeddedImage->Height, WantAlpha);
  if (NewImage == NULL)
    return NULL;

  CompData = (UINT8 *)EmbeddedImage->Data;   // drop const
  CompLen  = EmbeddedImage->DataLength;
  PixelCount = EmbeddedImage->Width * EmbeddedImage->Height;

  // FUTURE: for EG_EICOMPMODE_EFICOMPRESS, decompress whole data block here

  if (EmbeddedImage->PixelMode == EG_EIPIXELMODE_GRAY ||
      EmbeddedImage->PixelMode == EG_EIPIXELMODE_GRAY_ALPHA) {

    // copy grayscale plane and expand
    if (EmbeddedImage->CompressMode == EG_EICOMPMODE_RLE) {
      egDecompressIcnsRLE(&CompData, &CompLen, PLPTR(NewImage, r), PixelCount);
    } else {
      egInsertPlane(CompData, PLPTR(NewImage, r), PixelCount);
      CompData += PixelCount;
    }
    egCopyPlane(PLPTR(NewImage, r), PLPTR(NewImage, g), PixelCount);
    egCopyPlane(PLPTR(NewImage, r), PLPTR(NewImage, b), PixelCount);

  } else if (EmbeddedImage->PixelMode == EG_EIPIXELMODE_COLOR ||
             EmbeddedImage->PixelMode == EG_EIPIXELMODE_COLOR_ALPHA) {

    // copy color planes
    if (EmbeddedImage->CompressMode == EG_EICOMPMODE_RLE) {
      egDecompressIcnsRLE(&CompData, &CompLen, PLPTR(NewImage, r), PixelCount);
      egDecompressIcnsRLE(&CompData, &CompLen, PLPTR(NewImage, g), PixelCount);
      egDecompressIcnsRLE(&CompData, &CompLen, PLPTR(NewImage, b), PixelCount);
    } else {
      egInsertPlane(CompData, PLPTR(NewImage, r), PixelCount);
      CompData += PixelCount;
      egInsertPlane(CompData, PLPTR(NewImage, g), PixelCount);
      CompData += PixelCount;
      egInsertPlane(CompData, PLPTR(NewImage, b), PixelCount);
      CompData += PixelCount;
    }

  } else {

    // set color planes to black
    egSetPlane(PLPTR(NewImage, r), 0, PixelCount);
    egSetPlane(PLPTR(NewImage, g), 0, PixelCount);
    egSetPlane(PLPTR(NewImage, b), 0, PixelCount);

  }

  if (WantAlpha && (EmbeddedImage->PixelMode == EG_EIPIXELMODE_GRAY_ALPHA ||
                    EmbeddedImage->PixelMode == EG_EIPIXELMODE_COLOR_ALPHA ||
                    EmbeddedImage->PixelMode == EG_EIPIXELMODE_ALPHA)) {

    // copy alpha plane
    if (EmbeddedImage->CompressMode == EG_EICOMPMODE_RLE) {
      egDecompressIcnsRLE(&CompData, &CompLen, PLPTR(NewImage, a), PixelCount);
    } else {
      egInsertPlane(CompData, PLPTR(NewImage, a), PixelCount);
      //            CompData += PixelCount;
    }

  } else {
    egSetPlane(PLPTR(NewImage, a), WantAlpha ? 255 : 0, PixelCount);
  }

  return NewImage;
}
*/
//
// Compositing
//

VOID egRestrictImageArea(IN EG_IMAGE *Image,
                         IN INTN AreaPosX, IN INTN AreaPosY,
                         IN OUT INTN *AreaWidth, IN OUT INTN *AreaHeight)
{
  if (!Image || !AreaWidth || !AreaHeight) {
    return;
  }

  if (AreaPosX >= Image->Width || AreaPosY >= Image->Height) {
    // out of bounds, operation has no effect
    *AreaWidth  = 0;
    *AreaHeight = 0;
  } else {
    // calculate affected area
    if (*AreaWidth > Image->Width - AreaPosX)
      *AreaWidth = Image->Width - AreaPosX;
    if (*AreaHeight > Image->Height - AreaPosY)
      *AreaHeight = Image->Height - AreaPosY;
  }
}

VOID egFillImage(IN OUT EG_IMAGE *CompImage, IN EG_PIXEL *Color)
{}

VOID egFillImageArea(IN OUT EG_IMAGE *CompImage,
                     IN INTN AreaPosX, IN INTN AreaPosY,
                     IN INTN AreaWidth, IN INTN AreaHeight,
                     IN EG_PIXEL *Color)
{}

VOID egRawCopy(IN OUT EG_PIXEL *CompBasePtr, IN EG_PIXEL *TopBasePtr,
               IN INTN Width, IN INTN Height,
               IN INTN CompLineOffset, IN INTN TopLineOffset)
{}

VOID egRawCompose(IN OUT EG_PIXEL *CompBasePtr, IN EG_PIXEL *TopBasePtr,
                  IN INTN Width, IN INTN Height,
                  IN INTN CompLineOffset, IN INTN TopLineOffset)
{}

// This is simplified image composing on solid background. egComposeImage will decide which method to use
VOID egRawComposeOnFlat(IN OUT EG_PIXEL *CompBasePtr, IN EG_PIXEL *TopBasePtr,
                  IN INTN Width, IN INTN Height,
                  IN INTN CompLineOffset, IN INTN TopLineOffset)
{}

VOID egComposeImage(IN OUT EG_IMAGE *CompImage, IN EG_IMAGE *TopImage, IN INTN PosX, IN INTN PosY)
{}

EG_IMAGE * egEnsureImageSize(IN EG_IMAGE *Image, IN INTN Width, IN INTN Height, IN EG_PIXEL *Color)
{return 0;}

//
// misc internal functions
//

VOID egInsertPlane(IN UINT8 *SrcDataPtr, IN UINT8 *DestPlanePtr, IN UINTN PixelCount)
{}

VOID egSetPlane(IN UINT8 *DestPlanePtr, IN UINT8 Value, IN UINT64 PixelCount)
{}

VOID egCopyPlane(IN UINT8 *SrcPlanePtr, IN UINT8 *DestPlanePtr, IN UINTN PixelCount)
{}

#if defined(LODEPNG)
EG_IMAGE * egDecodePNG(IN UINT8 *FileData, IN UINTN FileDataLength, IN BOOLEAN WantAlpha) {
  EG_IMAGE *NewImage = NULL;
  UINTN Error, i, ImageSize, Width, Height;
  EG_PIXEL *PixelData;
  EG_PIXEL *Pixel, *PixelD;

  Error = eglodepng_decode((UINT8**) &PixelData, &Width, &Height, (CONST UINT8*) FileData, (UINTN) FileDataLength);

  if (Error) {
    /*
     * Error 28 incorrect PNG signature ok, because also called on ICNS files
     */
    if (Error != 28U) {
      DBG("egDecodePNG(%p, %lu, %c): eglodepng_decode failed with error %lu\n",
          FileData, FileDataLength, WantAlpha?'Y':'N', Error);
    }
    return NULL;
  }
  if (!PixelData || Width > 4096U || Height > 4096U) {
    DBG("egDecodePNG(%p, %lu, %c): eglodepng_decode returned suspect values, PixelData %p, Width %lu, Height %lu\n",
        FileData, FileDataLength, WantAlpha?'Y':'N', PixelData, Width, Height);
  }

  NewImage = egCreateImage(Width, Height, WantAlpha);
  if (NewImage == NULL) return NULL;

  ImageSize = (Width * Height);
//  CopyMem(NewImage->PixelData, PixelData, sizeof(EG_PIXEL) * ImageSize);
//  lodepng_free(PixelData);

  Pixel = (EG_PIXEL*)NewImage->PixelData;
  PixelD = PixelData;
  for (i = 0; i < ImageSize; i++) {
/*      UINT8 Temp;
      Temp = Pixel->b;
      Pixel->b = Pixel->r;
      Pixel->r = Temp; */
    Pixel->b = PixelD->r; //change r <-> b
    Pixel->r = PixelD->b;
    Pixel->g = PixelD->g;
    Pixel->a = PixelD->a; // 255 is opaque, 0 - transparent
    Pixel++;
    PixelD++;
  }

  lodepng_free(PixelData);
  return NewImage;
}
#endif //LODEPNG

/* EOF */
