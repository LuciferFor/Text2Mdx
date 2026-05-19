//-----------------------------------------------------------------------------
//
// ResIL Sources
// Written by Björn Ganster in 2014
//
// Description: class for palette access
//
//-----------------------------------------------------------------------------

#ifndef IL_PALETTE_H
#define IL_PALETTE_H

#include "IL/il_types.h"

class IIO;

//#define FILE_NUMBER FILE_NO_IL_PALETTE_H

// A lot of functions here are inline, because that is the easiest way to use them across several DLLs
// and it helps the compiler to actually inline functions where the last bit of performance matters ...
class ILpal
{
public:
	// Construct empty palette
	ILpal();

	// Copy palette
	ILpal(const ILpal& other);

	// Destructor
	~ILpal();

	// Use palette: copy all data from another palette into this one, without conversion
	ILboolean use(ILint aNumCols, ILubyte* aPal, ILPaletteType aPalType);

	// Set palette entry
	ILboolean setRGBA(ILuint index, ILubyte red, ILubyte green, ILubyte blue, ILubyte alpha);
	ILboolean setRGB(ILuint index, ILubyte red, ILubyte green, ILubyte blue);

	// Get palette entry
	ILboolean getRGBA(ILuint index, ILubyte& red, ILubyte& green, ILubyte& blue, 
		ILubyte& alpha) const;
	ILboolean getRGB(ILuint index, ILubyte& red, ILubyte& green, ILubyte& blue) const;

	void clear();

	ILPaletteType getPalType() const
	{
		return mPalType;
	}

	ILuint getPalSize() const
	{
		return mPalSize;
	}

	ILboolean readFromFile(IIO* io);

	ILboolean writeToFile(IIO* io);

	ILboolean hasPalette() const
	{
		if (mPalette == nullptr || mPalSize == 0) {
			return IL_FALSE;
		}

		switch (mPalType) {
		case IL_PAL_RGB24:
		case IL_PAL_RGB32:
		case IL_PAL_RGBA32:
		case IL_PAL_BGR24:
		case IL_PAL_BGR32:
		case IL_PAL_BGRA32:
			return IL_TRUE;
		case IL_PAL_NONE:
		default:
			return IL_FALSE;
		}
	}

	void operator=(const ILpal& other)
	{
		if (other.hasPalette()) {
			use(other.mNumCols, other.mPalette, other.mPalType);
		} else {
			clear();
		}
	}

	ILuint getNumCols() const
	{
		return mNumCols;
	}

	// Return a pointer to the internal palette - USE CAUTIOUSLY!
	// The returned pointer should not be used to manipulate the palette
	ILubyte* getPalette()
	{
		return mPalette;
	}

	// Converts the palette to the DestFormat format
	ILboolean convertPal(ILPaletteType DestFormat);

private:
	ILubyte* mPalette; //!< the image palette (if any)
	ILuint   mPalSize; //!< size of the palette (in bytes)
	ILPaletteType   mPalType; //!< palette type
	ILuint   mNumCols; //!< number of colors in the palette, 
	ILbyte  mNumComponents, mRedOffset, mGreenOffset, mBlueOffset, mAlphaOffset; // negative values indicate unused components
};

//#undef FILE_NUMBER

#endif
