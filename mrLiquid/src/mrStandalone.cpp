/**
 * @file   mrStandalone.cpp
 * @author gga
 * @date   Thu Nov 15 12:24:07 2007
 * 
 * @brief  
 * 
 * 
 */

#undef USE_SFIO

#include <cstdio>
#include <cstdlib>

#if defined(_WIN32) || defined(_WIN64)
#  include "winsock2.h"
#  include "windows.h"
#else
#  include <errno.h>
#  include <sys/types.h>
#  include <sys/wait.h> 
#endif

#include "maya/MGlobal.h"
#include "maya/MString.h"

#include "mrStandalone.h"
#include "mrIO.h"
#include "mrPipe.h"
#include "mrHelpers.h"
#include "mrOptions.h"
#include "mrThread.h"

extern MString tempDir;


MR_THREAD_RETURN wait_for_ray( void* opaque )
{
  mrStandalone* ray = (mrStandalone*) opaque;
  ray->wait(); 

//   mrOptions* options = mrOptions::factory( "!mrOptions" );
//   if ( options ) options->IPR = false;

  delete ray;

  if ( options->IPR )
    {
      MGlobal::executeCommand("mentalIPRStop");
    }

  MR_THREAD_EXIT(0);
}


void mrStandalone::set_environment()
{
   ///////////////// Set environment for mental ray stand-alone
#if defined(WIN32) || defined(WIN64)
   MString sep = ";";
#else
   MString sep = ":";
#endif

   MString lib = "MI_LIBRARY_PATH=";
   MString inc = "MI_RAY_INCPATH=";

   MString path = mr_getenv("MI_CUSTOM_SHADER_PATH");

#if !defined(WIN32) && !defined(WIN64)
   // Change all ; for :
   MStringArray paths;
   path.split( ';', paths );

   unsigned num = paths.length();
   if ( num > 1 )
     {
       path = "";
       for ( unsigned i = 0; i < num; ++i )
	 {
	   path += paths[i];
	   path += ":";
	 }
     }
#endif

   if ( path.length() > 1 ) 
     {
       lib += path;
       inc += path;

       unsigned len = path.length() - 1;
       if ( path.substring( len, len ) != sep )
	 {
	   lib += sep;
	   inc += sep;
	 }
     }
   else 
     {
       LOG_WARNING("Environment variable MI_CUSTOM_SHADER_PATH is undefined.");
       LOG_WARNING("mrLiquid may have trouble finding mrLiquid/mrClasses's shaders.");
       LOG_WARNING("Please set it to point to mrLiquid's shaders.");
       LOG_WARNING("You can do this from maya's script editor using putenv():");
#if defined(WIN32) || defined(WIN64)
       LOG_WARNING("putenv(\"MI_CUSTOM_SHADER_PATH\", "
		   "\"C:/Program Files/filmaura/mrLiquid/0.8.0/Windows/win32/shaders/mentalray34\"); ");
#else
       LOG_WARNING("putenv(\"MI_CUSTOM_SHADER_PATH\", "
		   "\"/opt/filmaura/mrLiquid/0.8.0/Linux-x86/i686/shaders/mentalray34\"); ");
#endif
   }


   MString loc = mr_getenv("MI_ROOT");

   if ( loc.length() < 1 )
     {
       loc = mr_getenv("MAYA_LOCATION");
       if ( loc.length() < 1 )
	 {
	   LOG_ERROR("Environment variable MI_ROOT and MAYA_LOCATION are undefined.");
	   LOG_ERROR("mrLiquid may have trouble finding maya shaders.");
	   LOG_ERROR("Please set it to your maya install path.");
	   LOG_ERROR("You can do this from maya's script editor using putenv():");
#if defined(WIN32) || defined(WIN64)
	   LOG_ERROR("putenv(\"MAYA_LOCATION\", "
		     "\"C:/Program Files/Alias/Maya7.0/\"); ");
#else
	   LOG_ERROR("putenv(\"MAYA_LOCATION\", "
		     "\"/usr/autodesk/Maya7.0/\"); ");
#endif
	 }
       lib += loc + "/mentalray/lib";
       inc += loc + "/mentalray/include";
     }
   else
     {
       lib += loc + "/lib";
       inc += loc + "/include";
     }

   lib += sep;
   inc += sep;



   MString msg = "SPM_HOST=";
   LOG_MESSAGE(msg + mr_getenv("SPM_HOST"));
   LOG_MESSAGE(inc);
   LOG_MESSAGE(lib);


   PUTENV( STRDUP( (char*)inc.asChar() ) );
   PUTENV( STRDUP( (char*)lib.asChar() ) );
}

//! Returns true if stand-alone is running.
bool mrStandalone::is_running()
{
#if defined(WIN32) || defined(WIN64)
   if ( id == NULL ) return false;
   // @todo: use wait for single object to check handle 
   //        for a millisecond?
#else
   if ( id == 0 ) return false;
#endif
   return true;
}


void mrStandalone::wait()
{
#if defined(WIN32) || defined(WIN64)
  if ( id == NULL ) return;

  WaitForSingleObject(id, INFINITE);
  CloseHandle( id );
#else
  if ( id == 0 ) return;

  int status;
  int err = waitpid( id, &status, 0);
  if ( err < 1 )
    {
      LOG_ERROR( strerror( errno ) );
    }
  else
    {
      if ( WIFEXITED(status) ) return;
      else if ( WIFSIGNALED(status) )
	{
	  MString err = "Uncaught signal: "; err += WTERMSIG(status);
	  LOG_ERROR( err );
	}
      else if ( WIFSTOPPED(status) )
	{
	  MString err = "Stopped: "; err += WSTOPSIG(status);
	  LOG_ERROR( err );
	}
      else
	{
	  MString err = "Exit: "; err += WEXITSTATUS(status);
	  LOG_ERROR( err );
	}
    }
#endif
}

bool mrStandalone::kill()
{
#if defined(WIN32) || defined(WIN64)
   if ( id == NULL )
   {
      LOG_ERROR("mentalray stand-alone is no longer running");
      return false;
   }

   BOOL ok = TerminateProcess( id, 0 );  CloseHandle(id); id = NULL;
   return ( ok == TRUE );
#else
   if ( id == 0 )
   {
      LOG_ERROR("mentalray stand-alone is no longer running");
      return false;
   }
#endif
   return true;
}



mrStandalone::mrStandalone() :
#if defined(WIN32) || defined(WIN64)
id( NULL )
#else
id( 0 )
#endif
{
}


mrStandalone::~mrStandalone()
{
#if defined(WIN32) || defined(WIN64)
   if ( id != NULL ) CloseHandle( id );
#endif
}


bool mrStandalone::start( FILE*& rayin, FILE*& rayout, FILE*& rayerr )
{
   if ( id )
   {
      const char* name = "mrStandalone: ";
      mrTHROW("this instance of the stand-alone already running.");
   }

   set_environment();

   //////////////////// Call ray executable
   MString ray = mr_getenv("MRL_RAY");
   if ( ray == "" )
   {
      LOG_MESSAGE("MRL_RAY environment variable is undefined. "
                  "Using 'ray' to invoke mentalray stand-alone.");
      MString path = "PATH=" + mr_getenv("PATH");
      PUTENV( (char*) path.asChar() );
      LOG_MESSAGE(path);
      ray = "ray";
   }
   else
   {
      LOG_MESSAGE(MString("MRL_RAY=") + ray);
   }

   MString cmd = "\"";
   cmd += ray;
   cmd += "\" -texture_continue on";

   if ( options->previewFinalGatherTiles )
     {
       cmd += " -finalgather_display on";
     }

   if ( options->renderThreads > 0 )
   {
      cmd += " -threads ";
      cmd += options->renderThreads;
   }

   if ( options->miDiskSwapLimit > 0 )
   {
      cmd += " -swap_limit ";
      cmd += options->miDiskSwapLimit;
   }

   if ( options->miDiskSwapDir != "" )
   {
      cmd += " -swap_dir \"";
      cmd += options->miDiskSwapDir;
      cmd += "\"";
   }


   LOG_MESSAGE(cmd);
#if 1
   if ( MGlobal::mayaState() == MGlobal::kInteractive )
   {
      if ( options->preview || options->IPR )
	 cmd += " -imgpipe 1";
      if (! mr_popen3(id, cmd.asChar(), rayin, rayout, rayerr,
		      true, true, true ) )
	 return false;

      MR_THREAD tid;
      mr_new_thread( tid, wait_for_ray, this );
   }
   else
#endif
   {
#if defined(WIN32) || defined(WIN64)
#define popen _popen
#endif
      rayout = rayerr = NULL;
      rayin = popen( cmd.asChar(), "wb" );
      if ( rayin == NULL ) return false;
   }
   return true;
}
