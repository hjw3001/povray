//******************************************************************************
///
/// @file windows/povconfig/syspovconfig.h
///
/// Windows-specific general POV-Ray compile-time configuration.
///
/// This header file configures module-independent aspects of POV-Ray for
/// running properly on a Unix platform.
///
/// @author Christopher J. Cason
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2015 Persistence of Vision Raytracer Pty. Ltd.
///
/// POV-Ray is free software: you can redistribute it and/or modify
/// it under the terms of the GNU Affero General Public License as
/// published by the Free Software Foundation, either version 3 of the
/// License, or (at your option) any later version.
///
/// POV-Ray is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU Affero General Public License for more details.
///
/// You should have received a copy of the GNU Affero General Public License
/// along with this program.  If not, see <http://www.gnu.org/licenses/>.
///
/// ----------------------------------------------------------------------------
///
/// POV-Ray is based on the popular DKB raytracer version 2.12.
/// DKBTrace was originally written by David K. Buck.
/// DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
///
/// @endparblock
///
//******************************************************************************

#ifndef POVRAY_WINDOWS_SYSPOVCONFIG_H
#define POVRAY_WINDOWS_SYSPOVCONFIG_H

#ifdef BUILDING_AMD64
  #if !defined(_M_AMD64) && !defined(_M_X64)
    #error you are compiling the x64 project using a 32-bit compiler
  #endif
#else
  #if defined(_M_AMD64) || defined(_M_X64)
    #error you are compiling the 32-bit project using a 64-bit compiler
  #endif
#endif

// _CONSOLE must be defined when building the console version of POVWIN.
// failure to do so will lead to link errors.
// #define _CONSOLE

#include <cmath>
#include <cstdarg>
#include <cstdlib>

#include <exception>
#include <list>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/tr1/memory.hpp>

#include <io.h>
#include <fcntl.h>

#ifndef STD_TYPES_DECLARED
#define STD_TYPES_DECLARED

// the following types are used extensively throughout the POV source and hence are
// included and named here for reasons of clarity and convenience.

// when we say 'string' we mean std::string
using std::string;

// and vector is a std::vector
using std::vector;

// yup, list too
using std::list;

// runtime_error is the base of our Exception class, plus is referred
// to in a few other places.
using std::runtime_error;

// these may actually be the boost implementations, depending on what boost/tr1/memory.hpp has pulled in
using std::tr1::shared_ptr;
using std::tr1::weak_ptr;
using std::tr1::dynamic_pointer_cast;

#endif // STD_POV_TYPES_DECLARED

// the build command-line is expected to declare WIN32_LEAN_AND_MEAN, which will
// prevent Window's objidl.h from being pulled in (which dupes IStream)
// #include <windows.h>

//#define PROFILE_CALLS

#ifdef _WIN64
  #define POVRAY_PLATFORM_NAME "win64"
#else
  #define POVRAY_PLATFORM_NAME "win32"
#endif

#define ReturnAddress()           NULL

#include "../vfeconf.h"

#if defined(__MINGW32__)                    /* MinGW GCC */
  #error Currently not supported.
  #include "syspovconfig_mingw32.h"
#elif defined(__WATCOMC__)                  /* Watcom C/C++ C32 */
  #error Currently not supported.
  #include "syspovconfig_watcom.h"
#elif defined(__BORLANDC__)                 /* Borland C/C++ */
  #error Currently not supported.
  #include "syspovconfig_borland.h"
#elif defined(_MSC_VER)                     /* Microsoft and Intel C++ */
  #include "syspovconfig_msvc.h"
#else
  #error unknown compiler configuration
#endif

#ifdef BUILD_SSE2
  #define SSE2_INCLUDED "-sse2"
#else
  #define SSE2_INCLUDED ""
#endif

/////////////////////////////////////////////////////////////

#ifndef MAX
  #define MAX(a,b) ((a>b)?a:b)
#endif
#ifndef MIN
  #define MIN(a,b) ((a>b)?b:a)
#endif

#ifndef HEAPSHRINK
  #define HEAPSHRINK
#endif

#ifndef NAN
  #define NAN (10E100)
#endif

#ifndef __GENDEFS
  #define __GENDEFS
  typedef unsigned char     uchar;
  typedef unsigned short    ushort;
  typedef unsigned int      uint;
  typedef unsigned long     ulong;
  typedef unsigned __int64  uint64;
  typedef __int64           int64;
#endif

#ifdef __INTEL_COMPILER
// Intel C++ whines about the lack of a return on RDTSC() for each and every file
// it is included in. VC++ is smarter (or dumber, depending on your point of view).
#pragma warning(push)
#pragma warning(disable : 1011)
#endif

namespace povwin
{
  void WIN_Debug_Log(unsigned int from, const char *msg) ;
  void WIN32_DEBUG_OUTPUT(const char *format,...) ;
  void WIN32_DEBUG_FILE_OUTPUT(const char *format,...) ;

#ifndef _CONSOLE
  void *win_malloc(size_t size);
  void *win_malloc(size_t size, const void *ptr, int line);
  void *win_realloc(void *p, size_t size);
  void *win_realloc(void *p, size_t size, const void *ptr, int line);
  void win_free(void *p);
  void win_free(void *p, const void *ptr, int line);
  char *win_strdup(const char *s);
  char *win_strdup(const char *s, const void *ptr, int line);
  bool WinMemReport(bool global, uint64& allocs, uint64& frees, int64& current, uint64& peak, uint64& smallest, uint64& largest);
  void WinMemStage(bool BeginRender, void *cookie = NULL);
#endif

#ifndef _WIN64
  inline void DebugBreak() { _asm _emit 0cch } // rather than use the windows one
  inline POV_LONG RDTSC(){ _asm _emit 0Fh _asm _emit 31h }
  #define READ_PROFILE_TIMER RDTSC()
#else
  inline void DebugBreak() {}
  #define READ_PROFILE_TIMER 0
#endif
}
#ifdef __INTEL_COMPILER
#pragma warning(pop)
#endif

#define lseek64(handle,offset,whence) _lseeki64(handle,offset,whence)

namespace pov_base
{
  // declare these to avoid warnings in image.cpp, rather than turn off the deprecation warnings.
  static inline int open(const char *name, int flags, int mode) { return _open(name, flags|_O_BINARY, mode); }
  static inline int close(int handle) { return _close(handle); }
  static inline int write(int handle, const void *data, int count) { return _write(handle, data, count); }
  static inline int read(int handle, void *data, int count) { return _read(handle, data, count); }
}

#define S_IRUSR                             _S_IREAD
#define S_IWUSR                             _S_IWRITE

#define ALTMAIN
#define LITTLE_ENDIAN
#define FILENAME_SEPARATOR                  '\\'
#define POV_FILE_SEPARATOR                  '\\'
#define POV_FILE_SEPARATOR_2                '/'
#define DEFAULT_OUTPUT_FORMAT               kPOVList_FileType_PNG
#define POV_IS1                             ".bmp"
#define POV_IS2                             ".BMP"
#define DEFAULT_DISPLAY_GAMMA_TYPE          kPOVList_GammaType_SRGB
#define DEFAULT_DISPLAY_GAMMA               2.2
#define DEFAULT_FILE_GAMMA_TYPE             kPOVList_GammaType_SRGB
#define DEFAULT_FILE_GAMMA                  2.2
#define RENAME_FILE(orig,new)               rename(orig,new)
#define DELETE_FILE(name)                   _unlink(name)
#define NEW_LINE_STRING                     "\r\n"
#define POV_SYS_FILE_EXTENSION              ".bmp"
#define SYS_TO_STANDARD                     BMP
#define vsnprintf                           _vsnprintf
#define snprintf                            _snprintf
#define FILE_NAME_LENGTH                    _MAX_PATH
#define POV_NAME_MAX                        _MAX_FNAME
#define IFF_SWITCH_CAST                     (long)
#define USE_OFFICIAL_BOOST                  1

#define POV_MEMMOVE(dst,src,len)            memmove((dst),(src),(len))
#define POV_MEMCPY(dst,src,len)             memcpy((dst),(src),(len))

#ifdef _CONSOLE

#define POV_MALLOC(size,msg)                malloc (size)
#define POV_REALLOC(ptr,size,msg)           realloc ((ptr), (size))
#define POV_FREE(ptr)                       do { free (static_cast<void *>(ptr)); (ptr) = NULL; } while(false)
#define POV_STRDUP(str)                     strdup(str)

#define NO_RTR                              1
#define MEM_STATS                           0

#else // not _CONSOLE

#define MEM_STATS                           0
#define POV_MEM_STATS                       0
#define WIN_MEM_TRACKING                    0

#ifdef _DEBUG
  #define POV_MALLOC(size,msg)              povwin::win_malloc ((size), __FILE__, __LINE__)
  #define POV_REALLOC(ptr,size,msg)         povwin::win_realloc ((ptr), (size), __FILE__, __LINE__)
  #define POV_FREE(ptr)                     do { povwin::win_free (static_cast<void *>(ptr), __FILE__, __LINE__); (ptr) = NULL; } while(false)
  #define POV_STRDUP(str)                   povwin::win_strdup(str, __FILE__, __LINE__)
#else
  #define POV_MALLOC(size,msg)              povwin::win_malloc (size)
  #define POV_REALLOC(ptr,size,msg)         povwin::win_realloc ((ptr), (size))
  #define POV_FREE(ptr)                     do { povwin::win_free (static_cast<void *>(ptr)); (ptr) = NULL; } while(false)
  #define POV_STRDUP(str)                   povwin::win_strdup(str)
#endif

#define POV_GLOBAL_MEM_STATS(a,f,c,p,s,l)   povwin::WinMemReport(true, a, f, c, p, s, l)
#define POV_THREAD_MEM_STATS(a,f,c,p,s,l)   povwin::WinMemReport(false, a, f, c, p, s, l)
#define POV_MEM_STATS_RENDER_BEGIN()        povwin::WinMemStage(true)
#define POV_MEM_STATS_RENDER_END()          povwin::WinMemStage(false)
#define POV_MEM_STATS_COOKIE                void *

#define POV_IMPLEMENT_RTR                   1

#define SYS_IMAGE_HEADER                    "syspovimage.h"
#define POV_VIDCAP_IMPL                     pov::VideoCaptureImpl
namespace pov
{
  class VideoCaptureImpl;
}
#endif // end of not _CONSOLE

// see RLP comment in 3.6 windows config.h
#undef HUGE_VAL

// use a larger buffer for more efficient parsing
#define DEFAULT_ITEXTSTREAM_BUFFER_SIZE 65536

// this adds some useful debugging information to each POV-Ray SDL object
#if defined _DEBUG
  #define OBJECT_DEBUG_HELPER
#endif

#ifndef MAX_PATH
  #define MAX_PATH _MAX_PATH
#endif

#define NEED_INVHYP

#ifndef NO_RTR
  #define RTR_HACK
  #define RTR_SUPPORT
#endif

#define HAVE_NAN
#define HAVE_INF
#define POV_ISNAN(x) _isnan(x)
#define POV_ISINF(x) _isinf(x)

#endif // POVRAY_WINDOWS_SYSPOVCONFIG_H
