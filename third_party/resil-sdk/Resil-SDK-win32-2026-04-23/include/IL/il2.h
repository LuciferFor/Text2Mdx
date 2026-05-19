///////////////////////////////////////////////////////////////////////////////
// il2.h
// Declarations for ResIL API 2.0, designed to be thread safe
// Written by Björn Ganster in 2014
// Keep this header C compatible!
///////////////////////////////////////////////////////////////////////////////

#ifndef IL__2__H
#define IL__2__H

#ifdef il_h
#pragma message("Mixed usage of ResIL API 1 and 2 found")
#endif

#include <stdio.h>
#include <stdint.h>
#include <IL/il_constants.h>
#include <IL/il_types.h>

class ILpal;

typedef unsigned int   ILenum;
typedef unsigned int   ILbitfield;
typedef signed char    ILbyte;
typedef signed short   ILshort;
typedef int            ILint;
typedef size_t         ILsizei;
typedef unsigned char  ILubyte;
typedef unsigned short ILushort;
typedef unsigned int   ILuint;
typedef float          ILfloat;
typedef float          ILclampf;
typedef double         ILdouble;
typedef double         ILclampd;

#ifdef _MSC_VER
typedef __int64          ILint64;
typedef unsigned __int64 ILuint64;
#else
typedef long long int          ILint64;
typedef long long unsigned int ILuint64;
#endif

//
// Section shamelessly modified from the glut header.
//

#ifdef _MSC_VER
typedef __int64          ILint64;
typedef unsigned __int64 ILuint64;
#else
typedef long long int          ILint64;
typedef long long unsigned int ILuint64;
#endif

class ILimage;

// Callback functions for file reading and writing
typedef void* ILHANDLE;

// Callback functions for allocation and deallocation
typedef void* (ILAPIENTRY *mAlloc)(const ILsizei);
typedef void  (ILAPIENTRY *mFree) (const void* CONST_RESTRICT);

// The internal structs should be visible to the outside only as void*
typedef void IlSIOExtern;

// Initialize ResIL API
ILAPI void ILAPIENTRY il2Init(void);

// Generate a new image 
// Return value: handle used by API 2.x functions
// Use ilCloseImage to close the image later
ILAPI ILimage* ILAPIENTRY il2GenImage();

// Creates a new ILimage based on the specifications given
ILAPI ILimage* ILAPIENTRY il2NewImage(ILuint Width, ILuint Height, ILuint Depth, ILDataFormat format, ILDataType type);

// Delete an image and all associated data
// Returns IL_FALSE if image is unknown (i.e., not allocated via il2GenImage or il2NewImage)
ILAPI ILboolean ILAPIENTRY il2DeleteImage(ILimage * image);

// Allows you to override the default file-writing functions
//ILAPI void ILAPIENTRY il2SetWrite(ILimage* image, fOpenProc Open, fCloseProc Close, 
//	fPutcProc Putc, fSeekProc Seek, fTellProc Tell, fWriteProc Write);

// Restore file-based i/o functions
ILAPI void ILAPIENTRY il2ResetWrite(ILimage* aImageExt);

// Allows you to override the default file-reading functions.
//ILAPI void ILAPIENTRY il2SetRead(ILimage* image, fOpenProc aOpen, fCloseProc aClose, 
//	fEofProc aEof, fGetcProc aGetc, fReadProc aRead, fSeekProc aSeek, fTellProc aTell);

// Get current read/write position for a lump
ILAPI ILuint64 ILAPIENTRY il2GetLumpPos(ILimage* aImageExt);

// Determine file type for given file handle or lump
ILAPI ILFileFormat ILAPIENTRY il2DetermineTypeF(ILHANDLE File);
ILAPI ILFileFormat ILAPIENTRY il2DetermineTypeL(const void *Lump, ILuint Size);
ILAPI ILFileFormat ILAPIENTRY il2DetermineType(ILconst_string FileName);
ILAPI ILFileFormat ILAPIENTRY il2DetermineTypeFuncs(ILimage* imageExt);

// Load an image using a file pointer
// (implemented in il_io.cpp)
ILAPI ILboolean ILAPIENTRY il2LoadF(ILimage* image, ILFileFormat Type, ILHANDLE File);

// Load an image from a memory buffer
ILAPI ILboolean ILAPIENTRY il2LoadL(ILimage* image, ILFileFormat Type, const void *Lump, ILuint Size);

// Load an image, given the file name. File type is deduced automatically.
ILAPI ILboolean ILAPIENTRY il2LoadImage(ILimage* imageExt, ILconst_string FileName);

// Load an image using the current set of io functions 
// (implemented in il_io.cpp)
ILAPI ILboolean ILAPIENTRY il2LoadFuncs(ILimage* image, ILFileFormat type);

// Load image using the file system
ILAPI ILboolean ILAPIENTRY il2Load(ILimage* image, ILFileFormat Type, ILconst_string FileName);

ILAPI ILint64	ILAPIENTRY il2DetermineSize(ILimage* image, ILFileFormat Type);

// Apply color profile
ILAPI ILboolean ILAPIENTRY il2ApplyProfile(ILimage* image, ILstring InProfile, ILstring OutProfile);

ILAPI ILboolean ILAPIENTRY il2Blit(ILimage* aSource, ILimage* aTarget, ILint DestX, ILint DestY, ILint DestZ, 
	ILuint SrcX, ILuint SrcY, ILuint SrcZ, ILuint Width, ILuint Height, ILuint Depth);

// Fill the whole image with rgba
ILAPI ILboolean ILAPIENTRY il2ClearImage(ILimage* image, uint32_t rgba);

// Copy data and attributes of aSource into a new image
ILAPI ILimage* ILAPIENTRY il2CloneImage(ILimage* aSource);

ILAPI ILboolean ILAPIENTRY il2ConvertImage (ILimage *Image, ILDataFormat DestFormat, ILDataType DestType);

//! Converts the current image to the DestFormat format.
ILAPI ILboolean ILAPIENTRY il2ConvertPal(ILimage* image, ILPaletteType DestFormat);

//! Copies everything from source to dest
ILAPI ILboolean ILAPIENTRY il2CopyImage(ILimage* source, ILimage* dest);

// Copy part of an image into a new image
ILAPI ILimage* ILAPIENTRY il2CopyPixels(ILimage* image, ILuint XOff, ILuint YOff, ILuint ZOff,
	ILuint Width, ILuint Height, ILuint Depth);

// Removed - use il2AddFrame, il2AddMipmapLevel or il2AddLayer instead
//ILAPI ILuint ILAPIENTRY il2CreateSubImage(ILimage* image, ILenum Type, ILuint Num);

// Add a frame to an animation (if image already has frame(s), the new frame 
// is added to the end of the animation).
// Returns true on success. In that case, ownership of newImage is transferred to image. Do not free newImage independently.
ILAPI ILboolean ILAPIENTRY il2AddFrame(ILimage* image, ILimage* newImage);

// Add a new mipmap level (if image already has a mipmap sub image, add newImage to the end of the list).
// newImage must be provided by the user, and it is stored without being altered.
// Returns true on success. In that case, ownership of newImage is transferred to image. Do not free newImage independently.
ILAPI ILboolean ILAPIENTRY il2AddMipmapLevel(ILimage* image, ILimage* newImage);

// Add a new layer to the image (if image already has a layer sub image, add newImage to the end of the list)
// Returns true on success. In that case, ownership of newImage is transferred to image. Do not free newImage independently.
ILAPI ILboolean ILAPIENTRY il2AddLayer(ILimage* image, ILimage* newImage);

//! Creates an ugly 64x64 black and yellow checkerboard image.
ILAPI ILboolean ILAPIENTRY il2DefaultImage(ILimage* image);

//! Disables a mode
ILAPI ILboolean ILAPIENTRY il2Disable(ILenum Mode);

//! Enables a mode
ILAPI ILboolean ILAPIENTRY il2Enable(ILenum Mode);

ILAPI ILboolean il2FixImage(ILimage* image);

// Copy the alpha channel to a buffer that is returned
ILAPI ILubyte* ILAPIENTRY il2GetAlpha(ILimage *Image, ILDataType Type);

//! Returns the current value of the Mode
ILAPI ILboolean ILAPIENTRY il2GetBoolean(ILenum Mode);

//! Sets Param equal to the current value of the Mode
ILAPI void ILAPIENTRY il2GetBooleanv(ILenum Mode, ILboolean *Param);

//! Returns a pointer to the image's raster data.
/*! The pointer to the image data returned by this function is only valid until any
    operations are done on the image.  After any operations, this function should be
	called again.  The pointer can be cast to other types for images that have more
	than one byte per channel for easier access to data. 
	\param image The image to use the data
	\exception IL_ILLEGAL_OPERATION if image==NULL
	\return ILubyte pointer to image data.*/
ILAPI ILubyte* ILAPIENTRY il2GetData(ILimage* image);

ILAPI size_t ILAPIENTRY il2GetDXTCData(ILimage* curImage, void *Buffer, size_t BufferSize, DX_PixFormat DXTCFormat);

//! Gets the last error and sets the last error to IL_NO_ERROR
ILAPI ILenum ILAPIENTRY il2GetError(ILimage* image);

// Set error string
ILAPI void ILAPIENTRY ilSetErrorA(ILimage* image, ILenum Error, const char* errorStr);
ILAPI void ILAPIENTRY ilSetErrorW(ILimage* image, ILenum Error, const wchar_t* errorStr);
ILAPI void ILAPIENTRY ilAddErrorA(ILimage* image, const char* errorStr);
ILAPI void ILAPIENTRY ilAddErrorW(ILimage* image, const wchar_t* errorStr);
ILAPI void ILAPIENTRY ilAddErrorI(ILimage* image, int64_t num, int base);
ILAPI void ILAPIENTRY ilAddStackTrace(ILimage* image);
ILAPI const ILstring ILAPIENTRY il2GetErrorString(ILimage* image);

// Get descriptive string for an error
ILconst_string ILAPIENTRY il2ErrorString(ILenum Error);

// Clear error string
ILAPI void ILAPIENTRY ilClearError(ILimage* image);

//! Used for querying facea of a cubemap
ILAPI ILimage* ILAPIENTRY il2GetFace(ILimage* image, ILuint Number);

// Used for querying the current image if it is an animation.
// (Replacement for ilActiveImage)
ILAPI ILimage* ILAPIENTRY il2GetFrame(ILimage* image, ILuint Number);

//! Returns the current value of the Mode
ILAPI ILint ILAPIENTRY il2GetInteger(ILenum Mode);

// Get image-related integer values
ILAPI void ILAPIENTRY il2GetImageInteger(ILimage *Image, ILenum Mode, ILint *Param);

//! Sets Param equal to the current value of the Mode
ILAPI ILboolean ILAPIENTRY il2GetIntegerv(ILenum Mode, ILint *Param);

//! Get specific image layer, represented by an image
ILAPI ILimage* ILAPIENTRY il2GetLayer(ILimage* image, ILuint Number);

//! Obtain a mipmap level
ILAPI ILimage* ILAPIENTRY il2GetMipmap(ILimage* image, ILuint Number);

//! Returns a pointer to the current image's palette data.
/*! The pointer to the image palette data returned by this function is only valid until
	any operations are done on the image. After any operations, this function should be
	called again.
	\param image The image to use the data
	\exception IL_ILLEGAL_OPERATION No currently bound image
	\return ILubyte pointer to image palette data.*/
ILAPI ILubyte* ILAPIENTRY il2GetPalette(ILimage* image);

// Query single palette entry
ILAPI ILboolean ILAPIENTRY il2GetPalRGBA(ILimage* image, int i, ILubyte r, ILubyte g, ILubyte b, ILubyte a);

//! Returns a constant string detailing aspects about this library.
ILAPI ILconst_string ILAPIENTRY il2GetString(ILenum StringName);

//! Checks whether the mode is enabled.
ILAPI ILboolean ILAPIENTRY il2IsEnabled(ILenum Mode);

//! Loads a palette from FileName into the current image's palette.
ILAPI ILboolean ILAPIENTRY il2LoadPal(ILimage* image, ILconst_string FileName);

//! Overlays the image found in Src on top of the current bound image at the coords specified.
ILAPI ILboolean ILAPIENTRY il2OverlayImage(ILimage* aSource, ILimage* aTarget, ILint aXCoord, ILint aYCoord, 
	ILint aZCoord);

// Adds an opaque alpha channel to a 3-channel image
// ilAddAlphaKey was removed, combine il2AddAlpha and il2ReplaceColor for achieving the same effect
ILAPI ILboolean	il2AddAlpha(ILimage* image);

// Removes the alpha channel from an image
ILAPI ILboolean ILAPIENTRY il2RemoveAlpha(ILimage* image);

// Blend transparent pixels with a color and remove the alpha channel (variant for paletted images)
// Converts the image to IL_RGB
ILAPI ILboolean ILAPIENTRY il2BlendAlpha(ILimage* image, ILubyte r, ILubyte g, ILubyte b);

//! Save image, caller specifies file type
ILAPI ILboolean ILAPIENTRY il2Save(ILimage* image, ILFileFormat type, ILconst_string FileName);

// Save image using the current set of io functions
ILAPI ILboolean ILAPIENTRY il2SaveFuncs(ILimage* image, ILFileFormat type);

// Save image, determines file type from extension
ILAPI ILboolean ILAPIENTRY il2SaveImage(ILimage* image, ILconst_string FileName);

// Save image, using caller's FILE*
ILAPI ILuint ILAPIENTRY il2SaveF(ILimage* image, ILFileFormat type, ILHANDLE File);

// Save image to lump
ILAPI ILint64 ILAPIENTRY il2SaveL(ILimage* image, ILFileFormat Type, void *Lump, ILuint Size);

//! Uploads Data of the same size to replace the image's data.
/*! 
	\param image The image to use the data
	\param Data New image data to update the image
	\exception IL_ILLEGAL_OPERATION No currently bound image
	\exception IL_INVALID_PARAM Data was NULL.
	\return Boolean value of failure or success
*/
ILAPI ILboolean ILAPIENTRY il2SetData(ILimage* image, void *Data);

ILAPI void ILAPIENTRY il2RegisterPal(ILimage* image, void* Pal, ILuint Size, ILPaletteType Type);

ILAPI ILboolean ILAPIENTRY il2SetDuration(ILimage* image, ILuint Duration);

// Sets the current error
ILAPI void ILAPIENTRY il2SetError(ILimage* image, ILenum Error);

// Set global state
ILAPI ILboolean ILAPIENTRY il2SetInteger(ILenum Mode, ILint Param);

// Set image-related integer values
ILAPI ILboolean ILAPIENTRY il2SetImageInteger(ILimage* image, ILenum Mode, ILint Param);

// Copy image from Data to image (partial copy if the target image is not large enough)
// (XOff, YOff, ZOff) give target coordinates in image
ILAPI ILboolean ILAPIENTRY il2SetPixels(ILimage* image, ILuint XOff, ILuint YOff, ILuint ZOff,
	ILuint Width, ILuint Height, ILuint Depth, ILDataFormat Format, ILDataType Type, void *Data);

ILAPI ILboolean il2SwapColours(ILimage* image);

/*!
	\param Image - the image affected by this operation
	\param Width Specifies the new image width.  This cannot be 0.
	\param Height Specifies the new image height.  This cannot be 0.
	\param Depth Specifies the new image depth.  This cannot be 0.
	\param Format Enum of the desired format.  Any format values are accepted.
	\param Type Enum of the desired type.  Any type values are accepted.
	\param Data Specifies data that should be copied to the new image. If this parameter is NULL, no data is copied, and the new image data consists of undefined values.
	\exception IL_ILLEGAL_OPERATION No currently bound image.
	\exception IL_INVALID_PARAM One of the parameters is incorrect, such as one of the dimensions being 0.
	\exception IL_OUT_OF_MEMORY Could not allocate enough memory.
	\return Boolean value of failure or success*/
ILAPI ILboolean ILAPIENTRY il2TexImage(ILimage * image, ILuint Width, ILuint Height, 
	ILuint Depth, ILDataFormat Format, ILDataType Type, const void *Data);

ILAPI void ILAPIENTRY il2SetPal(ILimage* image, ILpal* Pal);
ILAPI ILboolean ILAPIENTRY il2SetPalRGBA(ILimage* image, int i, ILubyte r, ILubyte g, ILubyte b, ILubyte a);
ILAPI ILboolean ILAPIENTRY il2CopyPal(ILimage* dest, ILimage* src);

// Flip image: flips the image data vertically and stores the changed image origin
ILAPI ILboolean ILAPIENTRY il2FlipImage(ILimage* image);

// Mirrors an image over its y axis
ILAPI ILboolean ILAPIENTRY il2MirrorImage(ILimage* image);

// Get pixel data
ILAPI ILuint ILAPIENTRY il2GetRGBA(ILimage* image, int x, int y, int z);

// Set pixel data
ILAPI void ILAPIENTRY il2SetRGBA(ILimage* image, int x, int y, int z, ILuint rgba);

// Replace pxiels or palette entries in an image
// Checks colors component-wise
// For image with a palette, returns the number of palette entries replaced
// For non-paletted images, the number of pixels changed is returned
ILAPI ILuint il2ReplaceColor(ILimage* image, ILubyte rMin, ILubyte gMin, ILubyte bMin, ILubyte aMin,
	ILubyte rMax, ILubyte gMax, ILubyte bMax, ILubyte aMax,
	ILubyte rNew, ILubyte gNew, ILubyte bNew, ILubyte aNew);

// Fix an image to use rotation IL_ORIENTATION_ROW0_TOP_COL0_LEFT
ILAPI void ILAPIENTRY il2FixOrientation(ILimage* image);

// Add a frame to an animation
ILAPI ILboolean ILAPIENTRY il2AddFrame(ILimage* base, ILimage* nextFrame);

// Move data from one image to another
// flags is an or-composite of the following flags
#define ILimage_move_basic_data 1
#define ILimage_move_mipmaps 2
#define ILimage_move_next_frame 4
#define ILimage_move_faces 8
#define ILimage_move_layers 16
//#define ILimage_move_animation 32 // unused
#define ILimage_move_profile 64
#define ILimage_move_errors 128
#define ILimage_move_dxtc_data 256 // part of basic data?
//#define ILimage_move_IO 512 // unused: IO does not get transferred
#define ILimage_move_delete_other 2048
ILAPI ILboolean ILAPIENTRY il2MoveData(ILimage* target, ILimage* source, int flags);

// Calls func for every pixel in image
// Sums up the value of func after every call so that the return value
// can be used for counting
typedef int (ILAPIENTRY * ILpixelProcessor)(ILimage* image, void* pixel, void* userData);
ILAPI int ILAPIENTRY il2ProcessPixels(ILimage* image, ILpixelProcessor func, void* userData);

// Checks how much two images are equal
ILAPI ILdouble ILAPIENTRY il2CompareImage(ILimage* imageA, ILimage* imageB);

// Count number of colors. bitCount gives the number of bits to take into account, per channel.
// Please note that high bit counts and 4 channels may use extreme amounts of memory!
ILAPI ILuint ILAPIENTRY il2ColorsUsed(ILimage* image, int bitCount);

// If API 1 is active, there is a currently bound image that may need to be updated
// when it gets deleted. So ilCloseImage should be used everywhere. However, when
// API 1 is not present, we simply reroute it to il2DeleteImage instead.
#ifndef IL_API1_SUPPORT
#define ilCloseImage il2DeleteImage
#endif

// Remove palette from image. Preserves the alpha channel if present.
ILAPI ILboolean ILAPIENTRY il2RemovePalette(ILimage* image);

#endif
