// il_types.h
// Basic abstract data types for IL

#pragma once

#include <stddef.h>
#include <string.h>

#include <IL/il_constants.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef _UNICODE
#include <wchar.h>
//if we use a define instead of a typedef,
//ILconst_string works as intended
#define ILchar wchar_t
#define ILstring wchar_t*
#define ILconst_string  wchar_t const *
#else
//if we use a define instead of a typedef,
//ILconst_string works as intended
#define ILchar char
#define ILstring char*
#define ILconst_string char const *
#endif //_UNICODE

typedef unsigned int   ILenum;
typedef bool           ILboolean;
typedef unsigned int   ILbitfield;
typedef signed char    ILbyte;
typedef void*          ILhandle;
typedef signed short   ILshort;
typedef int     	   ILint;
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

class ILimage;

