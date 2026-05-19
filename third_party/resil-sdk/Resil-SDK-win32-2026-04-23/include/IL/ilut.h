//-----------------------------------------------------------------------------
//
// ImageLib Utility Toolkit Sources
// Copyright (C) 2000-2009 by Denton Woods
// Modified by Björn Ganster since at least 2020
//
// Filename: IL/ilut.h
//
// Description: The main include file for ILUT
//
//-----------------------------------------------------------------------------

#ifndef ILUT__H
#define ILUT__H

#include <IL/il2.h>
#include <IL/ilu2.h>

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------

#define ILUT_VERSION_1_9_0 1
#define ILUT_VERSION       190


// Attribute Bits
#define ILUT_OPENGL_BIT      0x00000001
#define ILUT_D3D_BIT         0x00000002
#define ILUT_ALL_ATTRIB_BITS 0x000FFFFF


// Error Types
#define ILUT_OK                  0x500
#define ILUT_INVALID_ENUM        0x0501
#define ILUT_OUT_OF_MEMORY       0x0502
#define ILUT_INVALID_VALUE       0x0505
#define ILUT_ILLEGAL_OPERATION   0x0506
#define ILUT_INVALID_PARAM       0x0509
#define ILUT_COULD_NOT_OPEN_FILE 0x050A
#define ILUT_STACK_OVERFLOW      0x050E
#define ILUT_STACK_UNDERFLOW     0x050F
#define ILUT_BAD_DIMENSIONS      0x0511
#define ILUT_NOT_SUPPORTED       0x0550


// State Definitions
#define ILUT_PALETTE_MODE         0x0600
//#define ILUT_OPENGL_CONV          0x0610 // unused since Nov. 2025
#define ILUT_D3D_MIPLEVELS        0x0620
#define ILUT_MAXTEX_WIDTH         0x0630
#define ILUT_MAXTEX_HEIGHT        0x0631
#define ILUT_MAXTEX_DEPTH         0x0632
#define ILUT_GL_USE_S3TC          0x0634
#define ILUT_D3D_USE_DXTC         0x0634
#define ILUT_GL_GEN_S3TC          0x0635
#define ILUT_D3D_GEN_DXTC         0x0635
#define ILUT_S3TC_FORMAT          0x0705
#define ILUT_DXTC_FORMAT          0x0705
#define ILUT_D3D_POOL             0x0706
#define ILUT_D3D_ALPHA_KEY_COLOR  0x0707
#define ILUT_D3D_ALPHA_KEY_COLOUR 0x0707
#define ILUT_FORCE_INTEGER_FORMAT 0x0636

//This new state does automatic texture target detection
//if enabled. Currently, only cubemap detection is supported.
//if the current image is no cubemap, the 2d texture is chosen.
#define ILUT_GL_AUTODETECT_TEXTURE_TARGET 0x0807


// Values
#define ILUT_VERSION_NUM IL_VERSION_NUM
#define ILUT_VENDOR      IL_VENDOR

// The different rendering api's...more to be added later?
#define ILUT_OPENGL     0
#define ILUT_ALLEGRO    1
#define ILUT_WIN32      2
#define	ILUT_DIRECT3D10 6


#ifdef ILUT_USE_WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#endif


#ifdef ILUT_USE_SDL
//	#include <SDL.h>
#endif


//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// ImageLib Utility Toolkit Functions
ILAPI ILboolean		ILAPIENTRY ilutDisable(ILenum Mode);
ILAPI ILboolean		ILAPIENTRY ilutEnable(ILenum Mode);
ILAPI ILboolean		ILAPIENTRY ilutGetBoolean(ILenum Mode);
ILAPI void          ILAPIENTRY ilutGetBooleanv(ILenum Mode, ILboolean *Param);
ILAPI ILint			ILAPIENTRY ilutGetInteger(ILenum Mode);
ILAPI void          ILAPIENTRY ilutGetIntegerv(ILenum Mode, ILint *Param);
ILAPI const ILstring ILAPIENTRY ilutGetString(ILenum StringName);
ILAPI ILboolean     ILAPIENTRY ilutIsDisabled(ILenum Mode);
ILAPI ILboolean     ILAPIENTRY ilutIsEnabled(ILenum Mode);
ILAPI bool          ILAPIENTRY ilutPopAttrib(void);
ILAPI bool          ILAPIENTRY ilutPushAttrib(ILuint Bits);
ILAPI bool          ILAPIENTRY ilutSetInteger(ILenum Mode, ILint Param);

ILAPI ILboolean     ILAPIENTRY ilutRenderer(ILenum Renderer);

// ImageLib Utility Toolkit's OpenGL Functions
#ifdef ILUT_USE_OPENGL
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

// Absolutely *have* to call this if planning on using the image library with OpenGL.
// Call this after OpenGL has been initialized.
ILAPI ILboolean ILAPIENTRY ilutGLInit();

// Register texture with OpenGL
ILAPI bool ILAPIENTRY ilutGLRegisterTexture(ILimage* image, bool useMipmaps);

// Bind an ILimage to OpenGL
ILAPI ILboolean ILAPIENTRY ilutGLBindTexImage(ILimage* Image);

// Set image as the active cubemap
ILAPI ILenum ILAPIENTRY ilutGLRegisterCubemap(ILimage* image, GLuint Level);

// Query OpenGL/hardware capabilities
ILAPI ILboolean ILAPIENTRY ilutGLCubemapSupport();
ILAPI ILboolean ILAPIENTRY ilutGLNonPowerOfTwoSupport();
ILAPI ILint ILAPIENTRY ilutGLGetMaxTextureDimension();

#endif//ILUT_USE_OPENGL


// ImageLib Utility Toolkit's SDL Functions
#ifdef ILUT_USE_SDL
	ILAPI struct SDL_Surface* ILAPIENTRY ilutConvertToSDLSurface(unsigned int flags);
	ILAPI struct SDL_Surface* ILAPIENTRY ilutSDLSurfaceLoadImage(ILstring FileName);
	ILAPI ILboolean ILAPIENTRY ilutSDLSurfaceFromBitmap(struct SDL_Surface *Bitmap);
#endif//ILUT_USE_SDL


// ImageLib Utility Toolkit's Win32 GDI Functions
#ifdef ILUT_USE_WIN32
	ILAPI HBITMAP	ILAPIENTRY ilutConvertToHBitmap(HDC hDC);
	ILAPI HBITMAP	ILAPIENTRY ilutConvertSliceToHBitmap(HDC hDC, ILuint slice);
	ILAPI void	ILAPIENTRY ilutFreePaddedData(ILubyte *Data);
	ILAPI void	ILAPIENTRY ilutGetBmpInfo(BITMAPINFO *Info);
	ILAPI HPALETTE	ILAPIENTRY ilutGetHPal(ILimage* image);
	ILAPI ILubyte*	ILAPIENTRY ilutGetPaddedData(void);
	ILAPI ILboolean	ILAPIENTRY ilutGetWinClipboard(void);
	ILAPI ILimage*	ILAPIENTRY ilutLoadResource(HINSTANCE hInst, ILint ID, ILstring ResourceType, ILFileFormat Type);
	ILAPI ILboolean	ILAPIENTRY ilutSetHBitmap(ILimage* image, HBITMAP Bitmap);
	ILAPI ILboolean	ILAPIENTRY ilutSetHPal(HPALETTE Pal);
	ILAPI ILboolean	ILAPIENTRY ilutSetWinClipboard(void);
	ILAPI ILboolean	ILAPIENTRY ilutWinLoadUrl(ILimage* image, ILstring Url);
	ILAPI ILboolean ILAPIENTRY ilutWinPrint(ILuint XPos, ILuint YPos, ILuint Width, ILuint Height, HDC hDC);
	ILAPI HBITMAP ILAPIENTRY ilut2ConvertToHBitmap(ILimage* image, HDC hDC);
	ILAPI HBITMAP ILAPIENTRY ilut2ConvertSliceToHBitmap(ILimage* image, HDC hDC, ILuint slice);
#endif//ILUT_USE_WIN32

#endif // ilut_h
