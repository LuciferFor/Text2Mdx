//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Copyright (C) 2000-2009 by Denton Woods
// Last modified by Björn Ganster in 2014
//
// Filename: il_constants.h
//
// Description: Constants shared by API 1.x and 2.x
//
//-----------------------------------------------------------------------------

#ifndef IL_CONSTANTS__H
#define IL_CONSTANTS__H

#include <climits>

#include <IL/config.h>

#define _IL_BUILD_LIBRARY

// API functions should use the calling convention defined in config.h
#define ILAPIENTRY API_CALL_CONV

#if (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED) || defined(__BORLANDC__) || defined(__LCC__)
#define IL_PACKSTRUCT
#elif defined( __GNUC__ )
// this should work for any of the above commented platforms 
// plus any platform using GCC
#define IL_PACKSTRUCT __attribute__ ((packed))
#else
#define IL_PACKSTRUCT
#endif

// Compiler defines to force export a symbol to a DLL or shared library
#ifdef _WIN32
       #define ILAPI extern "C" __declspec(dllexport)
#elif __APPLE__
       #define ILAPI extern
#else
       #define ILAPI
#endif

//this define controls if floats and doubles are clampled to [0..1]
//during conversion. It takes a little more time, but it is the correct
//way of doing this. If you are sure your floats are always valid,
//you can undefine this value...
#define CLAMP_HALF		1
#define CLAMP_FLOATS	1
#define CLAMP_DOUBLES	1

#ifdef _WIN32
	#if (defined(IL_USE_PRAGMA_LIBS)) && (!defined(_IL_BUILD_LIBRARY))
		#if defined(_MSC_VER) || defined(__BORLANDC__)
			#pragma comment(lib, "DevIL.lib")
		#endif
	#endif
#endif

#ifdef RESTRICT_KEYWORD
#define RESTRICT restrict
#define CONST_RESTRICT const restrict
#else
#define RESTRICT
#define CONST_RESTRICT const
#endif

#include <stdio.h>


#define IL_FALSE			false
#define IL_TRUE				true

//  Matches OpenGL's right now.
//! Data formats \link Formats Formats\endlink
enum ILDataFormat {
	IL_COLOUR_INDEX    = 0x1900,
	IL_COLOR_INDEX     = 0x1900,
	IL_ALPHA		   = 0x1906,
	IL_RGB             = 0x1907,
	IL_RGBA            = 0x1908,
	IL_BGR             = 0x80E0,
	IL_BGRA            = 0x80E1,
	IL_LUMINANCE       = 0x1909,
	IL_LUMINANCE_ALPHA = 0x190A,
	IL_INVALID_FORMAT  = 0 // used with the DDS images, for example. Indicates compressed Data. In this case, check DxtcFormat.
};

//! Data types \link Types Types\endlink
enum ILDataType {
	IL_BYTE           = 0x1400,
	IL_UNSIGNED_BYTE  = 0x1401,
	IL_SHORT          = 0x1402,
	IL_UNSIGNED_SHORT = 0x1403,
	IL_INT            = 0x1404,
	IL_UNSIGNED_INT   = 0x1405,
	IL_FLOAT          = 0x1406,
	IL_DOUBLE         = 0x140A,
	IL_HALF           = 0x140B
};


#define IL_MAX_BYTE		  		SCHAR_MAX
#define IL_MAX_UNSIGNED_BYTE  	UCHAR_MAX
#define IL_MAX_SHORT	  		SHRT_MAX
#define IL_MAX_UNSIGNED_SHORT 	USHRT_MAX
#define IL_MAX_INT		  		INT_MAX
#define IL_MAX_UNSIGNED_INT   	UINT_MAX

#define IL_LIMIT(x,m,M)		(x<m?m:(x>M?M:x))
#define IL_CLAMP(x) 		IL_LIMIT(x,0,1)

#define IL_VENDOR   0x1F00
#define IL_LOAD_EXT 0x1F01
#define IL_SAVE_EXT 0x1F02


//
// IL-specific #define's
//

#define IL_VERSION_1_8_3
#define IL_VERSION       183


// Attribute Bits
//#define IL_ORIGIN_BIT          0x00000001 // not used anymore
#define IL_FILE_BIT            0x00000002
#define IL_PAL_BIT             0x00000004
#define IL_FORMAT_BIT          0x00000008
#define IL_TYPE_BIT            0x00000010
#define IL_COMPRESS_BIT        0x00000020
#define IL_LOADFAIL_BIT        0x00000040
#define IL_FORMAT_SPECIFIC_BIT 0x00000080
#define IL_ALL_ATTRIB_BITS     0x000FFFFF


// Palette types
enum ILPaletteType {
	IL_PAL_NONE   = 0x0400,
	IL_PAL_RGB24  = 0x0401,
	IL_PAL_RGB32  = 0x0402,
	IL_PAL_RGBA32 = 0x0403,
	IL_PAL_BGR24  = 0x0404,
	IL_PAL_BGR32  = 0x0405,
	IL_PAL_BGRA32 = 0x0406
};

// Image types
enum ILFileFormat {
	IL_TYPE_UNKNOWN = 0x0000,
	IL_BMP = 0x0420,  //!< Microsoft Windows Bitmap - .bmp extension
	IL_CUT = 0x0421,  //!< Dr. Halo - .cut extension
	IL_DOOM = 0x0422,  //!< DooM walls - no specific extension
	IL_DOOM_FLAT = 0x0423,  //!< DooM flats - no specific extension
	IL_ICO = 0x0424,  //!< Microsoft Windows Icons and Cursors - .ico and .cur extensions
	IL_JPG = 0x0425,  //!< JPEG - .jpg, .jpe and .jpeg extensions
	IL_JFIF = 0x0425,  //!<
	IL_ILBM = 0x0426,  //!< Amiga IFF (FORM ILBM) - .iff, .ilbm, .lbm extensions
	IL_PCD = 0x0427,  //!< Kodak PhotoCD - .pcd extension
	IL_PCX = 0x0428,  //!< ZSoft PCX - .pcx extension
	IL_PIC = 0x0429,  //!< PIC - .pic extension
	IL_PNG = 0x042A,  //!< Portable Network Graphics - .png extension
	IL_PNM = 0x042B,  //!< Portable Any Map - .pbm, .pgm, .ppm and .pnm extensions
	IL_SGI = 0x042C,  //!< Silicon Graphics - .sgi, .bw, .rgb and .rgba extensions
	IL_TGA = 0x042D,  //!< TrueVision Targa File - .tga, .vda, .icb and .vst extensions
	IL_TIF = 0x042E,  //!< Tagged Image File Format - .tif and .tiff extensions
	IL_CHEAD = 0x042F,  //!< C-Style Header - .h extension
	IL_RAW = 0x0430,  //!< Raw Image Data - any extension
	IL_MDL = 0x0431,  //!< Half-Life Model Texture - .mdl extension
	IL_WAL = 0x0432,  //!< Quake 2 Texture - .wal extension
	IL_LIF = 0x0434,  //!< Homeworld Texture - .lif extension
	IL_MNG = 0x0435,  //!< Multiple-image Network Graphics - .mng extension
	IL_GIF = 0x0436,  //!< Graphics Interchange Format - .gif extension
	IL_DDS = 0x0437,  //!< DirectDraw Surface - .dds extension
	IL_DCX = 0x0438,  //!< ZSoft Multi-PCX - .dcx extension
	IL_PSD = 0x0439,  //!< Adobe PhotoShop - .psd extension
	IL_EXIF = 0x043A,  //!< 
	IL_PSP = 0x043B,  //!< PaintShop Pro - .psp extension
	IL_PIX = 0x043C,  //!< PIX - .pix extension
	IL_PXR = 0x043D,  //!< Pixar - .pxr extension
	IL_XPM = 0x043E,  //!< X Pixel Map - .xpm extension
	IL_HDR = 0x043F,  //!< Radiance High Dynamic Range - .hdr extension
	IL_ICNS = 0x0440,  //!< Macintosh Icon - .icns extension
	IL_JP2 = 0x0441,  //!< Jpeg 2000 - .jp2 extension
	IL_EXR = 0x0442,  //!< OpenEXR - .exr extension
	IL_WDP = 0x0443,  //!< Microsoft HD Photo - .wdp and .hdp extension
	IL_VTF = 0x0444,  //!< Valve Texture Format - .vtf extension
	IL_WBMP = 0x0445,  //!< Wireless Bitmap - .wbmp extension
	IL_SUN = 0x0446,  //!< Sun Raster - .sun, .ras, .rs, .im1, .im8, .im24 and .im32 extensions
	IL_IFF = 0x0447,  //!< Interchange File Format - .iff extension
	IL_TPL = 0x0448,  //!< Gamecube Texture - .tpl extension
	IL_FITS = 0x0449,  //!< Flexible Image Transport System - .fit and .fits extensions
	IL_DICOM = 0x044A,  //!< Digital Imaging and Communications in Medicine (DICOM) - .dcm and .dicom extensions
	IL_IWI = 0x044B,  //!< Call of Duty Infinity Ward Image - .iwi extension
	IL_BLP = 0x044C,  //!< Blizzard Texture Format - .blp extension
	IL_FTX = 0x044D,  //!< Heavy Metal: FAKK2 Texture - .ftx extension
	IL_ROT = 0x044E,  //!< Homeworld 2 - Relic Texture - .rot extension
	IL_TEXTURE = 0x044F,  //!< Medieval II: Total War Texture - .texture extension
	IL_DPX = 0x0450,  //!< Digital Picture Exchange - .dpx extension
	IL_UTX = 0x0451,  //!< Unreal (and Unreal Tournament) Texture - .utx extension
	IL_MP3 = 0x0452,  //!< MPEG-1 Audio Layer 3 - .mp3 extension
	IL_WEBP = 0x0453,  //!< WebP format
	IL_PNG_CGBI = 0x0454,  // PNGCrunched PNGs
	IL_JNG = 0x0455,  //!< 
	IL_AVIF = 0x456, // AV1 Image File Format (AVIF)
	IL_HEIC = 0x457,

	IL_JASC_PAL = 0x0475,  //!< PaintShop Pro Palette
	IL_HALO_PAL = 0x0476  //!< Dr. Halo Palette
};


// Error Types
#define IL_NO_ERROR             0x0000
#define IL_INVALID_ENUM         0x0501
#define IL_OUT_OF_MEMORY        0x0502
#define IL_FORMAT_NOT_SUPPORTED 0x0503
#define IL_INTERNAL_ERROR       0x0504
#define IL_INVALID_VALUE        0x0505
#define IL_ILLEGAL_OPERATION    0x0506
#define IL_ILLEGAL_FILE_VALUE   0x0507
#define IL_INVALID_FILE_HEADER  0x0508
#define IL_INVALID_PARAM        0x0509
#define IL_COULD_NOT_OPEN_FILE  0x050A
#define IL_INVALID_EXTENSION    0x050B
#define IL_FILE_ALREADY_EXISTS  0x050C
#define IL_OUT_FORMAT_SAME      0x050D
#define IL_STACK_OVERFLOW       0x050E
#define IL_STACK_UNDERFLOW      0x050F
#define IL_INVALID_CONVERSION   0x0510
#define IL_BAD_DIMENSIONS       0x0511
#define IL_FILE_READ_ERROR      0x0512
#define IL_FILE_WRITE_ERROR     0x0513 // was also 0x0512 until Nov. 2025
#define IL_CPP_EXCEPTION_CAUGHT 0x0514

#define IL_LIB_GIF_ERROR  0x05E1
#define IL_LIB_JPEG_ERROR 0x05E2
#define IL_LIB_PNG_ERROR  0x05E3
#define IL_LIB_TIFF_ERROR 0x05E4
#define IL_LIB_MNG_ERROR  0x05E5
#define IL_LIB_JP2_ERROR  0x05E6
#define IL_LIB_EXR_ERROR  0x05E7
#define IL_UNKNOWN_ERROR  0x05FF


// Previously, only two origins were allowed for an image: IL_ORIGIN_LOWER_LEFT = 0x601, IL_ORIGIN_UPPER_LEFT = 0x602
// However, TIFF, EXIF and XMP allow many more image orientations.
// The defines below tell where data row 0 and data column 0 should be put for display
#define IL_ORIENTATION_ROW0_TOP_COL0_LEFT 1
#define IL_ORIENTATION_ROW0_TOP_COL0_RIGHT 2
#define IL_ORIENTATION_ROW0_BOTTOM_COL0_RIGHT 3
#define IL_ORIENTATION_ROW0_BOTTOM_COL0_LEFT 4
#define IL_ORIENTATION_ROW0_LEFT_COL0_TOP 5
#define IL_ORIENTATION_ROW0_RIGHT_COL0_TOP 6
#define IL_ORIENTATION_ROW0_RIGHT_COL0_BOTTOM 7
#define IL_ORIENTATION_ROW0_LEFT_COL0_BOTTOM 8

// Format and Type Mode Definitions
#define IL_FORMAT_SET  0x0610
#define IL_FORMAT_MODE 0x0611
#define IL_TYPE_SET    0x0612
#define IL_TYPE_MODE   0x0613


// File definitions
#define IL_FILE_OVERWRITE	0x0620
#define IL_FILE_MODE		0x0621


// Palette definitions
#define IL_CONV_PAL			0x0630


// Load fail definitions
#define IL_DEFAULT_ON_FAIL	0x0632


// Definition for alpha processing
#define IL_BLIT_BLEND		0x0636


// Interlace definitions
#define IL_SAVE_INTERLACED	0x0639
#define IL_INTERLACE_MODE	0x063A


// Quantization definitions
#define IL_QUANTIZATION_MODE 0x0640
#define IL_WU_QUANT          0x0641
#define IL_NEU_QUANT         0x0642
#define IL_NEU_QUANT_SAMPLE  0x0643
#define IL_MAX_QUANT_INDEXS  0x0644 //XIX : ILint : Maximum number of colors to reduce to, default of 256. and has a range of 2-256
#define IL_MAX_QUANT_INDICES 0x0644 // Redefined, since the above #define is misspelled


// Hints
#define IL_FASTEST          0x0660
#define IL_LESS_MEM         0x0661
#define IL_DONT_CARE        0x0662
#define IL_MEM_SPEED_HINT   0x0665
#define IL_USE_COMPRESSION  0x0666
#define IL_NO_COMPRESSION   0x0667
#define IL_COMPRESSION_HINT 0x0668


// Compression
#define IL_NVIDIA_COMPRESS	0x0670
#define IL_SQUISH_COMPRESS	0x0671


// Subimage types
#define IL_SUB_NEXT   0x0680
#define IL_SUB_MIPMAP 0x0681
#define IL_SUB_LAYER  0x0682

// These values are initialized to 0, meaning no limit
// (value 0 would not make sense, don't load an image then)
#define IL_MAX_WIDTH  0x0683
#define IL_MAX_HEIGHT 0x0684
#define IL_MAX_FRAMES 0x0685

// Compression definitions
#define IL_COMPRESS_MODE 0x0700
#define IL_COMPRESS_NONE 0x0701
#define IL_COMPRESS_RLE  0x0702
#define IL_COMPRESS_LZO  0x0703
#define IL_COMPRESS_ZLIB 0x0704


// File format-specific values
#define IL_TGA_CREATE_STAMP        0x0710
#define IL_JPG_QUALITY             0x0711
#define IL_PNG_INTERLACE           0x0712
#define IL_TGA_RLE                 0x0713
#define IL_BMP_RLE                 0x0714
#define IL_SGI_RLE                 0x0715
#define IL_TGA_ID_STRING           0x0717
#define IL_TGA_AUTHNAME_STRING     0x0718
#define IL_TGA_AUTHCOMMENT_STRING  0x0719
#define IL_PNG_AUTHNAME_STRING     0x071A
#define IL_PNG_TITLE_STRING        0x071B
#define IL_PNG_DESCRIPTION_STRING  0x071C
#define IL_TIF_DESCRIPTION_STRING  0x071D
#define IL_TIF_HOSTCOMPUTER_STRING 0x071E
#define IL_TIF_DOCUMENTNAME_STRING 0x071F
#define IL_TIF_AUTHNAME_STRING     0x0720
#define IL_JPG_SAVE_FORMAT         0x0721
#define IL_CHEAD_HEADER_STRING     0x0722
#define IL_PCD_PICNUM              0x0723
#define IL_PNG_ALPHA_INDEX 0x0724 //XIX : ILint : the color in the palette at this index value (0-255) is considered transparent, -1 for no trasparent color
#define IL_JPG_PROGRESSIVE         0x0725
#define IL_VTF_COMP                0x0726


// DXTC definitions - use DX_PixFormat, below ...
#define IL_DXTC_FORMAT      0x0705
#define IL_DXT_NO_COMP      0x070B
#define IL_KEEP_DXTC_DATA   0x070C
#define IL_DXTC_DATA_FORMAT 0x070D

// Environment map definitions
#define IL_CUBEMAP_POSITIVEX 0x00000400
#define IL_CUBEMAP_NEGATIVEX 0x00000800
#define IL_CUBEMAP_POSITIVEY 0x00001000
#define IL_CUBEMAP_NEGATIVEY 0x00002000
#define IL_CUBEMAP_POSITIVEZ 0x00004000
#define IL_CUBEMAP_NEGATIVEZ 0x00008000
#define IL_SPHEREMAP         0x00010000


// Values
#define IL_VERSION_NUM           0x0DE2
#define IL_IMAGE_WIDTH           0x0DE4
#define IL_IMAGE_HEIGHT          0x0DE5
#define IL_IMAGE_DEPTH           0x0DE6
#define IL_IMAGE_SIZE_OF_DATA    0x0DE7
#define IL_IMAGE_BPP             0x0DE8
#define IL_IMAGE_BYTES_PER_PIXEL 0x0DE8
#define IL_IMAGE_BPP             0x0DE8
#define IL_IMAGE_BITS_PER_PIXEL  0x0DE9
#define IL_IMAGE_FORMAT          0x0DEA
#define IL_IMAGE_TYPE            0x0DEB
#define IL_PALETTE_TYPE          0x0DEC
#define IL_PALETTE_SIZE          0x0DED
#define IL_PALETTE_BPP           0x0DEE
#define IL_PALETTE_NUM_COLS      0x0DEF
#define IL_PALETTE_BASE_TYPE     0x0DF0
#define IL_NUM_FACES             0x0DE1
#define IL_NUM_IMAGES            0x0DF1
#define IL_NUM_MIPMAPS           0x0DF2
#define IL_NUM_LAYERS            0x0DF3
#define IL_ACTIVE_IMAGE          0x0DF4
#define IL_ACTIVE_MIPMAP         0x0DF5
#define IL_ACTIVE_FACE           0x0E00
#define IL_IMAGE_DURATION        0x0DF8
#define IL_IMAGE_PLANESIZE       0x0DF9
#define IL_IMAGE_BPC             0x0DFA
#define IL_IMAGE_OFFX            0x0DFB
#define IL_IMAGE_OFFY            0x0DFC
#define IL_IMAGE_CUBEFLAGS       0x0DFD
#define IL_IMAGE_ORIENTATION          0x0DFE
#define IL_IMAGE_CHANNELS        0x0DFF
#define IL_IMAGE_ROWS_READ       0x0E01

#define IL_SEEK_SET	0
#define IL_SEEK_CUR	1
#define IL_SEEK_END	2
#define IL_EOF		-1

// Moved here from il_dds.h, replaces previous IL_* defines for DXTC
enum DX_PixFormat
{
	PF_ARGB,
	PF_RGB,
	PF_DXT1,
	//PF_DXT1A, // PF_DXT1A is a historical name for DXT1 with 1-bit alpha
	PF_DXT2,
	PF_DXT3,
	PF_DXT4,
	PF_DXT5,
	PF_3DC,
	PF_ATI1N,
	PF_LUMINANCE,
	PF_LUMINANCE_ALPHA,
	PF_RXGB, //Doom3 normal maps
	PF_A16B16G16R16,
	PF_R16F,
	PF_G16R16F,
	PF_A16B16G16R16F,
	PF_R32F,
	PF_G32R32F,
	PF_A32B32G32R32F,
	//PF_R16FG16FB16FA16F, // not currently used
	PF_RESIL = 0xFE, // encoding is defined by ILimage members format and type
	PF_UNKNOWN = 0xFF,
	PF_DXT_NO_COMP
};

// It should be easy to look up numbers here, so every identifier should be defined by a number
// These numbers should be kept stable between releases
// Values for files in ResIL should not overlap with numbers for other libraries
enum ResILFileID {
	//FILE_NO_BGBASE_CPP = 1001,
	//FILE_NO_BGCPSTRING_H = 1002,
	//FILE_NO_BGCPSTRING_CPP = 1003,
	//FILE_NO_MEMORYBLOCK_H = 1004, // old value: 9 (before Dec. 2025)
	//FILE_NO_MEMORYBLOCK_CPP = 1005, // old value: 10 (before Dec. 2025)
	//FILE_NO_BGABSTRACTSTORAGE_H = 5,
	//FILE_NO_BGABSTRACTSTORAGE_CPP = 6,
	//FILE_NO_BGSTRINGTEMPLATES_H = 101, // defined by the file itself
	//FILE_NO_ILUT_WIN32_CPP = 1005, // not used since Dec. 2022
	FILE_NO_IL_PALETTE_H = 1006,
	FILE_NO_IL_PALETTE_CPP = 1007,
	FILE_NO_IL_DEVIL_CPP = 1008, //0x08f9477, // main entry point should always have the hash of the commit that the compile is based on - and shipped builds always should use a committed version of the code!
	FILE_NO_IL_MANIP_CPP = 1009,
	FILE_NO_IL_STACK_CPP = 1010,
	FILE_NO_IL_API1_CPP = 1011,
	FILE_NO_IL_STATES_CPP = 1012,
	FILE_NO_IL_IO_CPP = 1013,
	FILE_NO_IL_ERROR_CPP = 1014,
	FILE_NO_IL_CONVERT_CPP = 1015,
	FILE_NO_IL_BMP_CPP = 1016,
	FILE_NO_IL_PNG_CPP = 1017,
	FILE_NO_IL_JPG_CPP = 1018,
	FILE_NO_IL_GIF_CPP = 1019,
	// FILE_NO_ILU_SCALE = 20, // not used since May, 2021
	FILE_NO_IL_DICOM_CPP = 1021,
	FILE_NO_IL_WEBP_CPP = 1022,
	FILE_NO_ILIMAGE_CPP = 1023,
	FILE_NO_IL_REGISTER_CPP = 1024,
	FILE_NO_IL_RAWDATA_CPP = 1025,
	FILE_NO_IL_PCX_CPP = 1026,
	FILE_NO_IL_TIFF_CPP = 1027,
	FILE_NO_IL_FASTCONV_CPP = 1028,
	FILE_NO_IL_CONVBUFF_CPP = 1029,
	FILE_NO_IL_INTERNAL_CPP = 1030,
	FILE_NO_IL_ICO_CPP = 1031,
	FILE_NO_IL_LUMPIO_CPP = 1032, // used to be 1028, a duplicate of FILE_NO_IL_FASTCONV_CPP, ouch!
	FILE_NO_IL_HDR_CPP = 1033,
	FILE_NO_IL_PSD_CPP = 1034,
	FILE_NO_IL_PIXEL_ACCESS_H = 1035,
	FILE_NO_IL_PIXEL_ACCESS_CPP = 1036,
	FILE_NO_IL_SIZE_CPP = 1037,
	FILE_NO_IL_HEIF_CPP = 1038,
	FILE_NO_BGBASE_H = 1039,
	FILE_NO_ILU_SCALE_CPP = 1200,
	FILE_NO_ILU_MANIP_CPP = 1201,
	FILE_NO_ILU_MATRIX_CPP = 1202,
	FILE_NO_ILU_ROTATE_CPP = 1203,
	FILE_NO_ILU_MAIN_CPP = 1204,
	FILE_NO_ILU_FILTER_RCG_CPP = 1205,
	FILE_NO_ILU_FILTER_CPP = 1206,
	FILE_NO_ILU_MIPMAP_CPP = 1207,
	FILE_NO_ILU_NOISE_CPP = 1208,
	FILE_NO_ILUT_WIN32_CPP = 1300,
	FILE_NO_ILUT_OPENGL_CPP = 1301,
	FILE_NO_ILUT_DIRECTX_CPP = 1302,
	FILE_NO_SDL_TEST_CPP = 1400,
	FILE_NO_TEST_IL_BASE_CPP = 1401
};

// Enum for classes - while FileIDs are specific for .h/.cpp, these IDs are shared
// Changes here must be reflected in VirtualClass::getClassName()
#define RESIL_CLASS_ID_LIST \
	X(GenericImagePixelSetterID, 6, "Generic image pixel setter") \
	X(TextureID, 7, "Texture") \
	X(ILimageID, 8, "ILimage")

enum ResILClassIDs
{
#define X(symbol,id,name) symbol = id,
	RESIL_CLASS_ID_LIST
#undef X
};

#endif
