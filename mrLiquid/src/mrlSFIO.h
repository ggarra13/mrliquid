/**
 * @file   mrlSFIO.h
 * @author gga
 * @date   Fri Aug 10 18:30:57 2007
 * 
 * @brief  This file contains includes and defines for the sfio library
 * 
 * 
 */

#ifndef mrlSFIO_h
#define mrlSFIO_h

extern "C" {

#  include "sfio_t.h"
typedef Sfio_t		_sfFILE;
extern ssize_t	_stdfwrite _ARG_((const Void_t*, size_t, size_t, Sfio_t*));
extern Sfio_t*	_stdfopen _ARG_((const char*, const char*));
extern int	_stdfflush _ARG_((Sfio_t*));
extern int	_stdfprintf _ARG_((Sfio_t* f, const char*, ...));
extern int	_stdfputc _ARG_((int, Sfio_t*));
extern ssize_t	_stdfwrite _ARG_((const Void_t*, size_t, size_t, Sfio_t*));
extern int	_stdfflush _ARG_((Sfio_t*));

extern char*	_stdgets _ARG_((Sfio_t*, char*, int n, int isgets));
extern ssize_t	_stdfread _ARG_((Void_t*, size_t, size_t, Sfio_t*));

} // extern "C"


#  define MRL_FILE    _sfFILE
#  define MRL_FOPEN   _stdfopen
#  define MRL_FFLUSH  _stdfflush 
#  define MRL_FCLOSE  sfclose
#  define MRL_PUTS(x) sfputr( f, x, -1)
#  define MRL_PUTC(x) sfputc( f, x )
#  define MRL_FPRINTF _stdfprintf
#  define MRL_FWRITE  _stdfwrite

#  define MRL_FREAD   _stdfread
#  define MRL_FEOF    sfeof

#define MRL_stdin	(&_Sfstdin)
#define MRL_stdout	(&_Sfstdout)
#define MRL_stderr	(&_Sfstderr)

#endif // mrlSFIO_h
