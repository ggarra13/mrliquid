/**
 * @file   mrExports.h
 * @author gga
 * @date   Tue Jan 23 09:32:52 2007
 * 
 * @brief  Simple file with proper macros for exporting symbols'
 *         visibility of the .so file
 * 
 * 
 */

#ifdef _MSC_VER
#  ifndef DLLEXPORT
#    define DLLEXPORT __declspec(dllexport)
#  endif
#    define DLLLOCAL
#else
#  ifndef DLLEXPORT
#    define DLLEXPORT __attribute__ ((visibility("default")))
#  endif
#    define DLLLOCAL  __attribute__ ((visibility("hidden")))
#endif
