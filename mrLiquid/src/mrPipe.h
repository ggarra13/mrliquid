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
 * @file   mrPipe.h
 * @author gga
 * @date   Tue Sep 27 15:53:49 2005
 * 
 * @brief  Utility functions to open a pipe to a subprocess and
 *         read the stdin/stdout.
 * 
 */

#include <cstdlib>
#include <cstdio>

#include "mrThread.h"

#if defined(WIN32) || defined(WIN64)
#  define MR_PROCESS HANDLE
#else
#  define MR_PROCESS pid_t
#endif

/** 
 * Like system(), but it does not open a console and
 * output always goes to stderr.
 * 
 * @param cmd Command to run
 * 
 * @return true on success, false on failure
 */
bool mr_system( const char*  cmd );


/** 
 * Open a pipe to a program and return all of its file descriptors.
 * 
 * @param cmd        command to run
 * @param cmd_stdin  stdin  for command
 * @param cmd_stdout stdout for command
 * @param cmd_stderr stderr for command
 * @param doStdin    whether to open a pipe for stdin
 * @param doStdout   whether to open a pipe for stdout
 * @param doStderr   whether to open a pipe for stderr
 *
 * @return  true on success, false on failure.
 * @return  processId  OS specific handle to the new process.
 *                     On Windows, this macro is a handle is a HANDLE and 
 *                     MUST be eventually closed with CloseHandle( processId )
 *                     On POSIX OSes, this is just a pid_t (int).
 *
 */
bool mr_popen3(
	       MR_PROCESS&  processId,
	       const char*  cmd,
	       FILE*& cmd_stdin,
	       FILE*& cmd_stdout,
	       FILE*& cmd_stderr,
	       bool doStdin,
	       bool doStdout,
	       bool doStderr
	       );

/** 
 * Open a pipe to a program and return all of its file descriptors.
 * 
 * @param cmd        command to run
 * @param cmd_stdin  stdin  for command
 * @param cmd_stdout stdout for command
 * @param cmd_stderr stderr for command
 * @param doStdin    whether to open a pipe for stdin
 * @param doStdout   whether to open a pipe for stdout
 * @param doStderr   whether to open a pipe for stderr
 *
 * @return  true on success, false on failure.
 *
 */
bool mr_popen3( const char*  cmd,
		FILE*& cmd_stdin,
		FILE*& cmd_stdout,
		FILE*& cmd_stderr,
		bool doStdin,
		bool doStdout,
		bool doStderr
		);


/** 
 * Start a new thread for reading the contents of a file descriptor
 * 
 * @param id Thread id for reader.
 * @param f  A read file descriptor.
 * 
 * @return true on success, false on failure.
 */
bool mr_start_error_reader( MR_THREAD& id, FILE* f );
