//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2009 by Denton Woods
// Modified by Björn Ganster since then
//
// Filename: IL/il.h
//
// Description: The main include file for DevIL (API version 1)
//
//-----------------------------------------------------------------------------

#ifndef IL__H
#define IL__H

#include <IL/il_types.h>
#include <IL/il_constants.h>

#include <stdint.h>

#ifdef IL_API1_SUPPORT

#ifdef IL__2__h
#pragma message("Mixed usage of ResIL API 1 and 2 found")
#endif

// Callback functions for allocation and deallocation
typedef void* (ILAPIENTRY *mAlloc)(const ILsizei);
typedef void  (ILAPIENTRY *mFree) (const void* p);

// Registered format procedures
typedef ILenum (ILAPIENTRY *IL_LOADPROC)(ILconst_string);
typedef ILenum (ILAPIENTRY *IL_SAVEPROC)(ILconst_string);

// ImageLib Functions
ILAPI ILboolean ILAPIENTRY ilActiveFace(ILuint Number);
ILAPI ILboolean ILAPIENTRY ilActiveImage(ILuint Number);
ILAPI ILboolean ILAPIENTRY ilActiveLayer(ILuint Number);
ILAPI ILboolean ILAPIENTRY ilActiveMipmap(ILuint Number);
ILAPI ILboolean ILAPIENTRY ilApplyProfile(ILstring InProfile, ILstring OutProfile);
ILAPI void		ILAPIENTRY ilBindImage(ILuint Image);
ILAPI ILboolean ILAPIENTRY ilBlit(ILuint Source, ILint DestX, ILint DestY, ILint DestZ, ILuint SrcX, ILuint SrcY, ILuint SrcZ, ILuint Width, ILuint Height, ILuint Depth);
ILAPI ILboolean ILAPIENTRY ilClearImage(uint32_t rgba);
ILAPI ILuint    ILAPIENTRY ilCloneCurImage(void);
ILAPI ILboolean ILAPIENTRY ilCompressFunc(ILenum Mode);
ILAPI ILboolean ILAPIENTRY ilConvertImage(ILDataFormat DestFormat, ILDataType DestType);
ILAPI ILboolean ILAPIENTRY ilConvertPal(ILPaletteType DestFormat);
ILAPI ILboolean ILAPIENTRY ilCopyImage(ILuint Src);
ILAPI ILboolean ILAPIENTRY ilDefaultImage();
ILAPI void		ILAPIENTRY ilDeleteImage(const ILuint Num);
ILAPI void      ILAPIENTRY ilDeleteImages(ILsizei Num, const ILuint *Images);
ILAPI ILint64	ILAPIENTRY ilDetermineSize(ILFileFormat Type);
ILAPI ILFileFormat ILAPIENTRY ilDetermineType(ILconst_string FileName);
ILAPI ILFileFormat ILAPIENTRY ilDetermineTypeFuncs();
ILAPI ILboolean ILAPIENTRY ilDisable(ILenum Mode);
ILAPI ILboolean ILAPIENTRY ilEnable(ILenum Mode);
ILAPI ILboolean ILAPIENTRY ilFormatFunc(ILenum Mode);
ILAPI ILboolean ILAPIENTRY ilGenImages(ILsizei Num, ILuint *Images);
ILAPI ILuint	ILAPIENTRY ilGenImage(void);
ILAPI ILubyte*  ILAPIENTRY ilGetAlpha(ILDataType Type);
ILAPI ILboolean ILAPIENTRY ilGetBoolean(ILenum Mode);
ILAPI void      ILAPIENTRY ilGetBooleanv(ILenum Mode, ILboolean *Param);
ILAPI ILimage*  ILAPIENTRY ilGetCurImage();
ILAPI ILubyte*  ILAPIENTRY ilGetData(void);
ILAPI ILchar*   ILAPIENTRY ilGetErrorString(void); // Get error string for current image
ILAPI size_t    ILAPIENTRY ilGetDXTCData(void *Buffer, size_t BufferSize, DX_PixFormat DXTCFormat);
ILAPI ILenum    ILAPIENTRY ilGetError(void);
ILAPI void ILAPIENTRY ilGetImageInteger(ILenum Mode, ILint *Param);
ILAPI ILint     ILAPIENTRY ilGetInteger(ILenum Mode);
ILAPI ILboolean ILAPIENTRY ilGetIntegerv(ILenum Mode, ILint *Param);
ILAPI ILubyte*  ILAPIENTRY ilGetPalette(void);
ILAPI ILconst_string  ILAPIENTRY ilGetString(ILenum StringName);
ILAPI ILboolean ILAPIENTRY ilHint(ILenum Target, ILenum Mode);
ILAPI void      ILAPIENTRY ilInit(void);
ILAPI ILboolean ILAPIENTRY ilIsDisabled(ILenum Mode);
ILAPI ILboolean ILAPIENTRY ilIsEnabled(ILenum Mode);
ILAPI ILboolean ILAPIENTRY ilIsImage(ILuint Image);
ILAPI ILboolean ILAPIENTRY ilIsValid(ILenum Type, ILconst_string FileName);
ILAPI ILboolean ILAPIENTRY ilIsValidF(ILenum Type, ILhandle File);
ILAPI ILboolean ILAPIENTRY ilIsValidL(ILenum Type, void *Lump, ILuint Size);
ILAPI void      ILAPIENTRY ilKeyColour(ILclampf Red, ILclampf Green, ILclampf Blue, ILclampf Alpha);
ILAPI ILboolean ILAPIENTRY ilLoad(ILFileFormat Type, ILconst_string FileName);
ILAPI ILboolean ILAPIENTRY ilLoadF(ILFileFormat Type, ILhandle File);
ILAPI ILboolean ILAPIENTRY ilLoadFuncs(ILFileFormat Type);
ILAPI ILboolean ILAPIENTRY ilLoadImage(ILconst_string FileName);
ILAPI ILboolean ILAPIENTRY ilLoadL(ILFileFormat Type, const void *Lump, ILuint Size);
ILAPI ILboolean ILAPIENTRY ilLoadPal(ILconst_string FileName);
ILAPI ILboolean ILAPIENTRY ilOverlayImage(ILuint Source, ILint XCoord, ILint YCoord, ILint ZCoord);
ILAPI ILboolean ILAPIENTRY ilPopAttrib(void);
ILAPI ILboolean ILAPIENTRY ilPushAttrib(ILuint Bits);
ILAPI void      ILAPIENTRY ilResetMemory(void);
ILAPI ILboolean ILAPIENTRY ilSave(ILFileFormat Type, ILconst_string FileName);
ILAPI ILuint    ILAPIENTRY ilSaveF(ILFileFormat Type, ILhandle File);
ILAPI ILboolean ILAPIENTRY ilSaveFuncs(ILFileFormat type);
ILAPI ILboolean ILAPIENTRY ilSaveImage(ILconst_string FileName);
ILAPI ILint64    ILAPIENTRY ilSaveL(ILFileFormat Type, void *Lump, ILuint Size);
ILAPI ILboolean ILAPIENTRY ilSetData(void *Data);
ILAPI void ILAPIENTRY ilSetImageInteger(ILenum Mode, ILint Param);
ILAPI void      ILAPIENTRY ilSetInteger(ILenum Mode, ILint Param);
ILAPI void      ILAPIENTRY ilSetMemory(mAlloc, mFree);
ILAPI void      ILAPIENTRY ilSetPixels(ILint XOff, ILint YOff, ILint ZOff, 
	ILuint Width, ILuint Height, ILuint Depth, ILDataFormat Format, ILDataType Type, void *Data);
ILAPI ILboolean ILAPIENTRY ilSetString(ILenum Mode, const char *String);
ILAPI void      ILAPIENTRY ilShutDown(void);
ILAPI ILboolean ILAPIENTRY ilSurfaceToDxtcData(DX_PixFormat Format);
ILAPI ILboolean ILAPIENTRY ilTexImage(ILuint Width, ILuint Height, ILuint Depth, ILDataFormat Format, ILDataType Type, const void *Data);
ILAPI ILFileFormat ILAPIENTRY ilTypeFromExt(ILconst_string FileName);
ILAPI ILboolean ILAPIENTRY ilTypeFunc(ILenum Mode);

// For all those weirdos that spell "colour" without the 'u'.
#define ilClearColor	ilClearColour
#define ilKeyColor      ilKeyColour

#endif // IL_API1_SUPPORT

#endif // __il_h__
