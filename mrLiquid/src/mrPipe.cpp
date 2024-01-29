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
/**
 * @file   mrPipe.cpp
 * @author gga
 * @date   Tue Sep 27 15:53:49 2005
 * 
 * @brief  Utility functions to open a pipe to a subprocess and
 *         get the child process' file descriptors.
 * 
 */

#undef USE_SFIO
#include <cstring>

#include "mrIO.h"
#include "mrPipe.h"


// Use this to send logs to maya console.  Sadly, maya's displayInfo is
// very fucked up and will not flush the messages properly, leading them
// to be in the wrong order.
//
// #define MR_LOG_TO_MAYA_CONSOLE



#define MRL_SAFE_CLOSE(x) \
  if ( close(x) < 0 ) \
    std::cerr << "Could not close " #x << endl;

/** 
 * Callback function for a new thread to read from a FILE descriptor
 * and send messages to maya console.
 * 
 * @param v a FILE* (cast to void*)
 * 
 * @return 0
 */
MR_THREAD_RETURN _mr_error_reader_cb( void* v )
{
   char  line[1024];
   line[1023] = 0;

   FILE* f = (FILE*)v;


   while( fgets( line, 1023, f ) != NULL )
   {
#ifdef MR_LOG_TO_MAYA_CONSOLE
      if ( MGlobal::mayaState() != MGlobal::kInteractive )
      {
	 MRL_FPRINTF( stderr, "%s", line );
	 fflush( stderr );
	 continue;
      }

      int idx = strlen( line );
      line[idx] = 0; // remove "\n"
      MGlobal::displayInfo( line );
#else
      MRL_FPRINTF( stderr, "%s", line );
      fflush( stderr );
#endif
   }


   MR_THREAD_EXIT(0);
}

bool mr_start_error_reader( MR_THREAD& id, FILE* f )
{
   return mr_new_thread( id, _mr_error_reader_cb, f );
}



#if defined(WIN32) || defined(WIN64)

#include <fcntl.h>
#include <io.h>
#include <windows.h>

// Turn last windows error into an MString for printing
MString win32_error_to_MString() 
{ 
    LPVOID lpMsgBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    MString msg( (LPTSTR) lpMsgBuf );
    LocalFree(lpMsgBuf);
    return msg;
}


/** 
 * Create a new child process
 * 
 * @param cmd Command to run
 * 
 * @return TRUE or FALSE to indicate success or failure
 */
BOOL _create_child_process( HANDLE& processId,
			    const char* cmd,
			    HANDLE hChildStdIn,
			    HANDLE hChildStdOut,
			    HANDLE hChildStdErr )
{   
   PROCESS_INFORMATION piProcInfo;
   STARTUPINFO siStartInfo;
   BOOL bFuncRetn = FALSE; 
 
   // Set up members of the PROCESS_INFORMATION structure. 
   ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );
 
   // Set up members of the STARTUPINFO structure. 
   ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
   siStartInfo.cb = sizeof(STARTUPINFO); 
   siStartInfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
   siStartInfo.hStdOutput = hChildStdOut;
   siStartInfo.hStdInput  = hChildStdIn;
   siStartInfo.hStdError  = hChildStdErr;
   // set the window display to HIDE
   siStartInfo.wShowWindow = SW_HIDE;

   // Create the child process. 
   bFuncRetn = CreateProcess(NULL, 
      (char*)cmd,    // command line 
      NULL,          // process security attributes 
      NULL,          // primary thread security attributes 
      TRUE,          // handles are inherited 
      0,             // creation flags 
      NULL,          // use parent's environment 
      NULL,          // use parent's current directory 
      &siStartInfo,  // STARTUPINFO pointer 
      &piProcInfo);  // receives PROCESS_INFORMATION 
   
   if (bFuncRetn == 0) 
   {
      processId = NULL;
      LOG_ERROR(cmd);
      LOG_ERROR("CreateProcess failed with:\n" + win32_error_to_MString());
      return 0;
   } 
   else 
   {
      processId = piProcInfo.hProcess;
//       CloseHandle(piProcInfo.hProcess);
      CloseHandle(piProcInfo.hThread);
      return bFuncRetn;
   }
   
}

/** 
 * Duplicate a windows handle to make it non-inheritable
 * 
 * @param handle 
 * 
 * @return the new non-inheritable handle
 */
HANDLE _dup_handle(HANDLE handle, BOOL inherit = FALSE)
{
   HANDLE duphandle;
   if (! DuplicateHandle(GetCurrentProcess(), handle,
			 GetCurrentProcess(), 
			 &duphandle,
			 0, inherit,
			 DUPLICATE_SAME_ACCESS) )
      LOG_ERROR("DuplicateHandle failed");
   CloseHandle(handle);
   return duphandle;
}

/** 
 * Start a new piped process
 * 
 * @param cmd    process to start
 * @param f      stdin of child process
 * @param rayerr stderr of child process
 */
bool mr_popen3( 
	       MR_PROCESS& processId,
	       const char*  cmd,
	       FILE*& cmd_stdin,
	       FILE*& cmd_stdout,
	       FILE*& cmd_stderr,
	       bool doStdin,
	       bool doStdout,
	       bool doStderr
	       )
{
   // Temporary microsoft handles used below
   HANDLE in_read, in_write, err_read, err_write, out_read, out_write;

   cmd_stdin = cmd_stdout = cmd_stderr = NULL;

   SECURITY_ATTRIBUTES saAttr; 
   saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
   saAttr.bInheritHandle = TRUE; 
   saAttr.lpSecurityDescriptor = NULL; 

   processId = NULL;
   in_read   = NULL; //GetStdHandle(STD_INPUT_HANDLE); 
   out_write = NULL; //GetStdHandle(STD_OUTPUT_HANDLE); 
   err_write = NULL; //GetStdHandle(STD_ERROR_HANDLE); 

   // Create a pipe for stdin
   if ( doStdin )
   {
      if (!CreatePipe(&in_read, &in_write, &saAttr, 32000))
      {
	 LOG_ERROR("Create pipe stdin handles failed with:\n"+
                    win32_error_to_MString());
	 return false;
      }
      // Duplicate the write handle to the pipe so it is not inherited. 
      in_write = _dup_handle(in_write);
   }

   // Create a pipe for stdout
   if ( doStdout )
   {
      if (!CreatePipe(&out_read, &out_write, &saAttr, 0))
      {
	 LOG_ERROR("Create pipe stdout handles failed with:\n"+
                    win32_error_to_MString());
	 if ( doStdin )
	 {
	    CloseHandle(in_write);
	    CloseHandle(in_read);
	 }
	 CloseHandle(out_write);
	 CloseHandle(out_read);
	 return false;
      }

      // Duplicate the read handle to the pipe so it is not inherited. 
      out_write = _dup_handle(out_write, TRUE);
      out_read  = _dup_handle(out_read);
   }


   // Create a pipe for stderr
   if ( doStderr )
   {
      if (!CreatePipe(&err_read, &err_write, &saAttr, 0))
      {
	 LOG_ERROR("Create pipe stderr handles failed with:\n"+
                    win32_error_to_MString());
	 if ( doStdin )
	 {
	    CloseHandle(in_write);
	    CloseHandle(in_read);
	 }
	 if ( doStdout )
	 {
	    CloseHandle(out_write);
	    CloseHandle(out_read);
	 }
	 return false;
      }

      // Duplicate the read error handle so it is not inherited. 
      err_write = _dup_handle(err_write, TRUE);
      err_read  = _dup_handle(err_read);
   }

   if (! _create_child_process(processId, cmd, 
			       in_read, out_write, err_write) )
   {
      processId = NULL;
      return false;
   }


   int fd;
   // Close write error handle as we don't need it anymore
   if (doStderr)
   {
      CloseHandle(err_write);
      fd = _open_osfhandle((long) err_read, _O_RDONLY|_O_TEXT);
      cmd_stderr = _fdopen( fd, "r");
      // make stderr unbuffered
      setvbuf( cmd_stderr, NULL, _IONBF, 0);
   }

   // Close write stdout handle as we don't need it anymore
   if (doStdout)
   {
      CloseHandle(out_write);
      fd = _open_osfhandle((long) out_read, _O_RDONLY);
      cmd_stdout = _fdopen( fd, "rb");
   }

   // Close read stdin handle as we don't need it anymore
   if (doStdin)
   {
      CloseHandle(in_read);
      fd = _open_osfhandle((long) in_write, _O_APPEND);
      cmd_stdin = _fdopen( fd, "wb");
      // NOTE:: DO *NOT* MAKE STDIN UNBUFFERED!!!
   }

   return true;
}

bool mr_popen3( 
	       const char*  cmd,
	       FILE*& cmd_stdin,
	       FILE*& cmd_stdout,
	       FILE*& cmd_stderr,
	       bool doStdin,
	       bool doStdout,
	       bool doStderr
	       )
{
   MR_PROCESS processId;
   bool ok = mr_popen3( processId, cmd, cmd_stdin, cmd_stdout, cmd_stderr,
			doStdin, doStdout, doStderr );
   if ( ok )
   {
     CloseHandle( processId );
   }
   return ok;
}

bool mr_system( const char* cmd )
{
   FILE* in, *out, *err;
   if ( ! mr_popen3( cmd, in, out, err, false, true, true ) )
      return false;

   MR_THREAD errId;
   if ( ! mr_start_error_reader( errId, err ) )
   {
      fclose(out); fclose(err);
      return false;
   }
   _mr_error_reader_cb( (void*) out );

   return true;
}


#else


#include <cstdio>
#include <errno.h>

bool mr_system( const char* cmd )
{
   return ( system( cmd ) == 0 );
}




/** 
 * Split a command into individual components for execvp compatability.
 * 
 * @param buf  a string containing the command and its arguments
 * @param args a pre-allocated char* buffer (must be big enough)
 *
 */
void _parse_cmd_into_args(char* buf, char** args)
{
    while (*buf != 0) {


        /*
         * Strip whitespace.  Use 0, so
         * that the previous argument is terminated
         * automatically.
         */
         while ((*buf == ' ') || (*buf == '\t'))
            *buf++ = 0;

	 if ( *buf == 0 ) break;

	 // If we begin with a quote, we skip until end of quote.
	 if ( *buf == '"' )
	   {
	     ++buf;

	     if ( *buf != 0 && *buf != '"' ) *args = buf++;

	     while (*buf != 0 && *buf != '"')
	       buf++;
	     
	     if ( *buf == '"' ) {
	       *buf = 0;
	       ++buf;
	     }
	   }
	 else
	   {
	     /*
	      * Save the argument.
	      */
	     *args = buf++;
	   }


        /*
         * Skip over the argument.
         */

	if ((*args)[0] == '"')
	{
	}
	else
	{
	   // we skip until whitespace
	   while ((*buf != 0) && (*buf != ' ') && (*buf != '\t'))
	      buf++;
	}

	args++; // go to next arg
    }

    *args = NULL;  // last arg must be NULL for execvp
}


bool mr_popen3( 
	       MR_PROCESS&  pid,
	       const char* cmd,
	       FILE*& cmd_stdin,
	       FILE*& cmd_stdout,
	       FILE*& cmd_stderr,
	       bool doStdin,
	       bool doStdout,
	       bool doStderr
	       )
{
   // Temporary Unix file descriptor handles used below
   int in_fd[2], err_fd[2], out_fd[2];

   cmd_stdin = cmd_stdout = cmd_stderr = NULL;


   if ( pipe(in_fd) == -1 )
   {
      LOG_ERROR("Pipe creation for stdin failed");
      return false;
   }

   if ( pipe(out_fd) == -1 )
   {
      LOG_ERROR("Pipe creation for stdout failed");
      MRL_SAFE_CLOSE(in_fd[0]);
      MRL_SAFE_CLOSE(in_fd[1]);
      return false;
   }

   if ( pipe(err_fd) == -1 )
   {
      LOG_ERROR("Pipe creation for stderr failed");
      MRL_SAFE_CLOSE(in_fd[0]);
      MRL_SAFE_CLOSE(in_fd[1]);
      MRL_SAFE_CLOSE(out_fd[0]);
      MRL_SAFE_CLOSE(out_fd[1]);
      return false;
   }

   // Fork a new process
   pid = fork();  
   if (pid == -1) { 
      MRL_SAFE_CLOSE(out_fd[0]);
      MRL_SAFE_CLOSE(out_fd[1]);
      MRL_SAFE_CLOSE(err_fd[0]);
      MRL_SAFE_CLOSE(err_fd[1]);
      MRL_SAFE_CLOSE(in_fd[0]);
      MRL_SAFE_CLOSE(in_fd[1]);
      LOG_ERROR("Fork error - cannot create child process");
      return false;
   }

   if ( pid == 0 )
   {

     if ( (dup2( in_fd[0],  STDIN_FILENO )  == -1 ) )
       {
	cerr << "Child could not redirect stdin" << endl;
	exit(1);
       }

     if ( (dup2( out_fd[1], STDOUT_FILENO ) == -1) )
       {
	cerr << "Child could not redirect stdout" << endl;
	exit(1);
       }

      // Code for child
      // Duplicate pipes to change stdin, stdout, stderr
      if ( (dup2( err_fd[1], STDERR_FILENO ) == -1) )
      {
	cerr << "Child could not redirect stdin, stdout, stderr." << endl;
	exit(1);
      }

      // Close descriptors
      MRL_SAFE_CLOSE(out_fd[0]);
      MRL_SAFE_CLOSE(out_fd[1]);
      MRL_SAFE_CLOSE(err_fd[0]);
      MRL_SAFE_CLOSE(err_fd[1]);
      MRL_SAFE_CLOSE(in_fd[0]);
      MRL_SAFE_CLOSE(in_fd[1]);

      // Okay, split the command into multiple chars*
      char** args = (char**) malloc( sizeof(char*) * 128 );
      memset( args, 0, 128*sizeof(char*) );


      char* cmdcpy = strdup( cmd );

      _parse_cmd_into_args( cmdcpy, args );

      execvp(args[0], args); 

      // still around?  exec failed 
      // NOTE: do not call LOG_ERROR here
      cerr << "Could not start command:" << endl << "  ";
      char** c = args;
      for ( ; *c != 0; ++c )
	cerr << *c << " ";
      cerr << endl;
      cerr << "Reason: " << strerror( errno ) << endl;

      free( cmdcpy );
      free( args );

      // @bug:
      //
      // Child should exit with _exit not exit, but doing so
      // seems to crash Maya (Xlib).
      //
      // Doing exit also seems problematic, albeit a tad less,
      // as it crashes Xlib on first or second run.
      exit(1);
      // _exit(1);
   }
   else
   {       
     // Parent process

     // Close unused file descriptors
     MRL_SAFE_CLOSE(in_fd[0]);
     MRL_SAFE_CLOSE(out_fd[1]);
     MRL_SAFE_CLOSE(err_fd[1]);

     // Attach FILE* from descriptors
     if ( doStdin )
       {
	 cmd_stdin  = fdopen( in_fd[1], "w" );
	 if ( !cmd_stdin )
	   {
	     // NOTE: do not call LOG_ERROR here
	     cerr << "Could not set stdin FILE*" << endl;
	     MRL_SAFE_CLOSE(out_fd[0]);
	     MRL_SAFE_CLOSE(err_fd[0]);
	     MRL_SAFE_CLOSE(in_fd[1]);
	     return false;
	   }
       }
     else
       {
	 MRL_SAFE_CLOSE(in_fd[1]);
       }

      if ( doStdout )
	{
	  cmd_stdout = fdopen( out_fd[0], "r");
	  if ( !cmd_stdout )
	    {
	     // NOTE: do not call LOG_ERROR here
	      cerr << "Could not set stdout FILE*" << endl;
	      MRL_SAFE_CLOSE(out_fd[0]);
	      MRL_SAFE_CLOSE(err_fd[0]);
	      MRL_SAFE_CLOSE(in_fd[1]);
	      return false;
	    }
	}
      else
	{
	  MRL_SAFE_CLOSE( out_fd[0] );
	}

      if ( doStderr )
	{
	  cmd_stderr = fdopen( err_fd[0], "r");
	  if ( !cmd_stderr )
	    {
	     // NOTE: do not call LOG_ERROR here
	      cerr << "Could not set stderr FILE*" << endl;
	      MRL_SAFE_CLOSE(out_fd[0]);
	      MRL_SAFE_CLOSE(err_fd[0]);
	      MRL_SAFE_CLOSE(in_fd[1]);
	      return false;
	    }
	}
      else
	{
	  MRL_SAFE_CLOSE( err_fd[0] );
	}


      return true;
   }
}



bool mr_popen3( const char* cmd,
		FILE*& cmd_stdin,
		FILE*& cmd_stdout,
		FILE*& cmd_stderr,
		bool doStdin,
		bool doStdout,
		bool doStderr )
{
  MR_PROCESS processId;
  bool ok = mr_popen3( processId, cmd, cmd_stdin, cmd_stdout, cmd_stderr,
		       doStdin, doStdout, doStderr );
  return ok;
}


#endif
