//-----------------------------------------------------------------------------
//
// ImageLib Utility Sources
// Copyright (C) 2000-2009 by Denton Woods
// Last modified by Björn Ganster
//
// Description: The main include file for ILU
//
//-----------------------------------------------------------------------------

#ifndef ILU2__H

#define ILU2__H

#include "il2.h"


#define ILU_VERSION_1_9_0 1
#define ILU_VERSION       190


#define ILU_FILTER         0x2600
#define ILU_NEAREST        0x2601
#define ILU_LINEAR         0x2602
#define ILU_BILINEAR       0x2603
#define ILU_SCALE_BOX      0x2604
#define ILU_SCALE_TRIANGLE 0x2605
#define ILU_SCALE_BELL     0x2606
#define ILU_SCALE_BSPLINE  0x2607
#define ILU_SCALE_LANCZOS3 0x2608
#define ILU_SCALE_MITCHELL 0x2609


// Error types
#define ILU_INVALID_ENUM      0x0501
#define ILU_OUT_OF_MEMORY     0x0502
#define ILU_INTERNAL_ERROR    0x0504
#define ILU_INVALID_VALUE     0x0505
#define ILU_ILLEGAL_OPERATION 0x0506
#define ILU_INVALID_PARAM     0x0509


// Values
#define ILU_PLACEMENT          0x0700
#define ILU_LOWER_LEFT         0x0701
#define ILU_LOWER_RIGHT        0x0702
#define ILU_UPPER_LEFT         0x0703
#define ILU_UPPER_RIGHT        0x0704
#define ILU_CENTER             0x0705
#define ILU_CONVOLUTION_MATRIX 0x0710
  
#define ILU_VERSION_NUM IL_VERSION_NUM
#define ILU_VENDOR      IL_VENDOR


// Languages
#define ILU_ENGLISH            0x0800
#define ILU_ARABIC             0x0801
#define ILU_DUTCH              0x0802
#define ILU_JAPANESE           0x0803
#define ILU_SPANISH            0x0804
#define ILU_GERMAN             0x0805
#define ILU_FRENCH             0x0806

typedef struct ILpointf {
	ILfloat x;
	ILfloat y;
} ILpointf;

typedef struct ILpointi {
	ILint x;
	ILint y;
} ILpointi;

ILAPI ILboolean      ILAPIENTRY ilu2BlurAvg(ILimage* image, ILuint Iter, ILimage* mask);
ILAPI ILboolean      ILAPIENTRY ilu2BlurGaussian(ILimage* image, ILuint Iter, ILimage* mask);
ILAPI ILboolean      ILAPIENTRY ilu2BuildMipmaps(ILimage* image, ILenum filter);

// Count number of colors. bitCount gives the number of bits to take into account, per channel.
// Please note that high bit counts and 4 channels may use extreme amounts of memory!
// (moved to ResIL, il_devil.cpp, in Oct. 2025)
//ILAPI ILuint         ILAPIENTRY ilu2ColoursUsed(ILimage* image);
#define ilu2ColoursUsed il2ColorsUsed

// Checks how much two images are equal
//ILAPI ILboolean      ILAPIENTRY ilu2CompareImage(ILimage* imageA, ILimage* imageB);
#define ilu2CompareImage il2CompareImage

// Adjust image brightness and contrast via a single function
// brightness is simply added to the pixel value
// contrast applies as a factor
// clamp controls whether or not values should be clamped (only supported for floating point)
ILboolean ilu2ContrastBrightness(ILimage* image, ILfloat contrast, ILfloat brightness, bool clamp);

//ILAPI ILboolean      ILAPIENTRY ilu2Contrast(ILimage* image, ILfloat Contrast);
//ILAPI ILboolean ILAPIENTRY ilu2Convolution(ILimage* image, ILint *matrix, ILint scale, ILint bias);
ILAPI ILboolean      ILAPIENTRY ilu2Crop(ILimage* image, ILuint XOff, ILuint YOff, ILuint ZOff, ILuint Width, ILuint Height, ILuint Depth);
ILAPI ILboolean      ILAPIENTRY ilu2EnlargeCanvas(ILimage* image, ILuint Width, ILuint Height, ILuint Depth);
ILAPI ILboolean      ILAPIENTRY ilu2EnlargeImage(ILimage* image, ILfloat XDim, ILfloat YDim, ILfloat ZDim, ILenum filter);
ILAPI ILboolean      ILAPIENTRY ilu2Equalize(ILimage* image);

// Simple redirect to il2FlipImage
// Flips an image over its x axis
// ILboolean ILAPIENTRY ilu2FlipImage(ILimage* image) 
#define ilu2FlipImage il2FlipImage

ILAPI ILboolean      ILAPIENTRY ilu2GammaCorrect(ILimage* image, ILfloat Gamma);
ILAPI ILboolean      ILAPIENTRY ilu2InvertChannels(ILimage* image, bool invertR, bool invertG, bool invertB, bool invertA);
ILAPI ILimage* ILAPIENTRY ilu2LoadImage(ILconst_string FileName);

// Simple redirect to il2MirrorImage
// Mirrors an image over its y axis
// ILboolean ILAPIENTRY ilu2Mirror(ILimage* image)
#define ilu2Mirror il2Mirror

//ILAPI ILboolean      ILAPIENTRY ilu2Negative(ILimage* image); // use ilu2InvertChannels instead
ILAPI ILboolean      ILAPIENTRY ilu2Noisify(ILimage* image, ILclampf Tolerance, ILimage* mask);
ILAPI ILboolean      ILAPIENTRY ilu2Pixelize(ILimage* image, ILuint PixSize, ILimage* mask);
ILAPI ILboolean      ILAPIENTRY ilu2ReplaceColour(ILimage* image, ILubyte minRed, ILubyte minGreen, ILubyte minBlue,
	ILubyte maxRed, ILubyte maxGreen, ILubyte maxBlue,
	ILubyte newRed, ILubyte newGreen, ILubyte newBlue);
ILAPI ILboolean      ILAPIENTRY ilu2Rotate(ILimage* image, ILfloat Angle);
ILAPI ILboolean      ILAPIENTRY ilu2Scale(ILimage* image, ILuint Width, ILuint Height, ILuint Depth, ILenum filter);

// Scale channels
// scale must provide one factor for each channel of image (use 1.0 to keep channels unaltered)
ILboolean ILAPIENTRY ilu2ScaleChannels(ILimage* image, ILfloat* scale);

// ilu2SwapColours is a duplicate of il2SwapColours, but il2SwapColours is more optimized
// ILAPI ILboolean      ILAPIENTRY ilu2SwapColours(ILimage* image);
#define ilu2SwapColours il2SwapColours

ILAPI ILboolean      ILAPIENTRY ilu2Wave(ILimage* image, ILfloat Angle);

#define ilu2ColorsUsed   ilu2ColoursUsed
#define ilu2SwapColors   ilu2SwapColours
#define ilu2ReplaceColor ilu2ReplaceColour
#define ilu2ScaleColor   ilu2ScaleColour

// Build mask image from polygon with float coordinates
// For every coordinate requested via coordCount, coords must contain two ILfloats
ILimage* ilu2BuildMaskFromPolygon(ILuint width, ILuint height, ILuint depth,
	ILfloat* coords, size_t coordCount);

#endif // __ILU2_H__
