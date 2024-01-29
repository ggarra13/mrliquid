//
//  Copyright (c) 2004, Gonzalo Garramuno
//
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are
//  met:
//  *       Redistributions of source code must retain the above copyright
//  notice, this list of conditions and the following disclaimer.
//  *       Redistributions in binary form must reproduce the above
//  copyright notice, this list of conditions and the following disclaimer
//  in the documentation and/or other materials provided with the
//  distribution.
//  *       Neither the name of Gonzalo Garramuno nor the names of
//  its other contributors may be used to endorse or promote products derived
//  from this software without specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//


#ifndef mrIO_h
#define mrIO_h

#ifndef mrlPlatform_h
#  include "mrlPlatform.h"
#endif

#include "maya/MMatrix.h"
#include "maya/MGlobal.h"


#if !defined(WIN32) && !defined(WIN64)

#ifdef USE_SFIO
#  include "mrlSFIO.h"
#endif  // USE_SFIO

#endif


// Simple file with some macros to spit out strings to a stream.
// Based on OS, we might change the function called for best performance.
//
// In general, putc is slow, fprintf is better and fputs should be fastest.

#ifndef MRL_FILE
#  define MRL_FILE    FILE
#  define MRL_FOPEN   fopen
#  define MRL_FFLUSH  fflush
#  define MRL_FCLOSE  fclose
#  define MRL_PUTS(x) fputs( x,f)
#  define MRL_PUTC(x) putc(x,f)
#  define MRL_FPRINTF fprintf
#  define MRL_FWRITE  fwrite
#  define MRL_FREAD   fread
#  define MRL_FEOF    feof
#  define MRL_stdin   stdin
#  define MRL_stdout  stdout
#  define MRL_stderr  stderr
#endif

#ifdef DEBUG
#  define COMMENT(x)   MRL_PUTS(x)
#  define TAB(x)       write_tabs(f,x)
#  define NEWLINE()    MRL_PUTC('\n')
#else
#  define COMMENT(x)
#  define TAB(x)
#  define NEWLINE()
#endif


#define LOG_BUG( msg ) \
  do { \
      cerr << "(mrLiquid) [BUG] " << msg << endl; \
      if ( MGlobal::mayaState() == MGlobal::kInteractive ) \
      { \
         MString m_err = "(mrLiquid) [BUG] "; \
         m_err += msg; \
	 MGlobal::displayError( m_err ); \
      } \
    } while(0);


#define LOG_ERROR( msg ) \
  do { \
      cerr << "(mrLiquid) Error: " << msg << endl; \
      if ( MGlobal::mayaState() == MGlobal::kInteractive ) \
      { \
         MString m_err = "(mrLiquid) "; \
         m_err += msg; \
	 MGlobal::displayError( m_err ); \
      } \
    } while(0);


#define LOG_WARNING( msg ) \
  do { \
      cerr << "(mrLiquid) Warning: " << msg << endl; \
      if ( MGlobal::mayaState() == MGlobal::kInteractive ) \
      { \
         MString m_err = "(mrLiquid) "; \
         m_err += msg; \
	 MGlobal::displayWarning( m_err ); \
      } \
    } while(0);


#define LOG_MESSAGE( msg ) \
  do { \
      if ( MGlobal::mayaState() != MGlobal::kInteractive ) \
      { \
	 cerr << "(mrLiquid) " << msg << endl; \
      } \
      else \
      { \
         MString m_err = "(mrLiquid) "; \
         m_err += msg; \
	 MGlobal::displayInfo( m_err ); \
      } \
    } while(0);


inline void write_tabs( MRL_FILE* f, int t )
{
   static const char* const tabs[] = {
   "",
   "\t",
   "\t\t",
   "\t\t\t",
   "\t\t\t\t",
   "\t\t\t\t\t",
   "\t\t\t\t\t\t",
   "\t\t\t\t\t\t\t",
   "\t\t\t\t\t\t\t\t",
   "\t\t\t\t\t\t\t\t\t",
   "\t\t\t\t\t\t\t\t\t\t",
   "\t\t\t\t\t\t\t\t\t\t\t",
   "\t\t\t\t\t\t\t\t\t\t\t\t",
   };
   MRL_PUTS( tabs[t] );
}

extern char kMatrixParameterSpec[128];
extern char kMatrixLineSpec[32];
extern char kMatrixSpec[128];

inline void write_matrix_parameter( MRL_FILE* f, const MMatrix& m )
{
   MRL_FPRINTF(f, kMatrixParameterSpec,
	       m[0][0], m[0][1], m[0][2], m[0][3],
	       m[1][0], m[1][1], m[1][2], m[1][3],
	       m[2][0], m[2][1], m[2][2], m[2][3],
	       m[3][0], m[3][1], m[3][2], m[3][3]
	       );
}

inline void write_matrix( MRL_FILE* f, const MMatrix& m )
{
#ifdef DEBUG
   TAB(1);
   MRL_FPRINTF(f, kMatrixLineSpec, m[0][0], m[0][1], m[0][2], m[0][3]);
   TAB(1);
   MRL_FPRINTF(f, kMatrixLineSpec, m[1][0], m[1][1], m[1][2], m[1][3]);
   TAB(1);
   MRL_FPRINTF(f, kMatrixLineSpec, m[2][0], m[2][1], m[2][2], m[2][3]);
   TAB(1);
   MRL_FPRINTF(f, kMatrixLineSpec, m[3][0], m[3][1], m[3][2], m[3][3]);
#else
   MRL_FPRINTF(f, kMatrixSpec,
	       m[0][0], m[0][1], m[0][2], m[0][3],
	       m[1][0], m[1][1], m[1][2], m[1][3],
	       m[2][0], m[2][1], m[2][2], m[2][3],
	       m[3][0], m[3][1], m[3][2], m[3][3]
	       );
#endif
}

#endif // mrIO_h

