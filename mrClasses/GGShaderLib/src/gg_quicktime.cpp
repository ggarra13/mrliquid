/******************************************************************************
 * Created:	07.05.03
 * Module:	gg_quicktime
 *
 * Exports:
 *      gg_quicktime()   _init(), _exit(), _version()
 *      gg_cquicktime()  _init(), _exit(), _version()
 *
 * Requires:
 *      mrClasses
 *
 * History:
 *      07.05.03: initial version
 *
 * Description:
 *      Quicktime returning either a scalar or a color.
 *
 *****************************************************************************/


#ifdef OSX
#endif

#ifdef WIN32
#include <windows.h>
#include <QTML.h>
#include <Movies.h>
#include <Quickdraw.h>
#include <QDOffscreen.h>
#include <FixMath.h>
#endif


#include "mrGenerics.h"
#include "mrRman.h"
#include "mrRman_macros.h"

//! Constant for the texture cache (x 1024 bytes)
const miUint kMEMORY_LIMIT = 16384;


using namespace mr;
using namespace rsl;

struct gg_quicktime_t
{
  miTag     filename;
  miInteger uvSet;
};


// DrawCompleteProc - After the frame has been drawn QuickTime calls us to do some work
static pascal OSErr DrawCompleteProc(Movie theMovie, long refCon)
{
  GWorldPtr	offWorld = (GWorldPtr)refCon;
  Rect		bounds;
  Ptr			baseAddr;
  long		rowBytes;

  // get the information we need from the GWorld
  GetPixBounds(GetGWorldPixMap(offWorld), &bounds);
  baseAddr = GetPixBaseAddr(GetGWorldPixMap(offWorld));
  rowBytes = QTGetPixMapHandleRowBytes(GetGWorldPixMap(offWorld));
  return noErr;
}


struct quicktimeCache
{
  quicktimeCache( Movie& mov ) :
    thePlayer( NULL ),
    theMovie( mov ),
    myDrawCompleteProc( NULL )
  {	
    myDrawCompleteProc = NewMovieDrawingCompleteUPP(DrawCompleteProc);

    Rect		bounds;
    GetMovieBox(theMovie, &bounds);
    QTNewGWorld(&offWorld, k32ARGBPixelFormat, &bounds, NULL, NULL, 0);
    LockPixels(GetGWorldPixMap(offWorld));
    SetGWorld(offWorld, NULL);

    thePlayer = NewMovieController(theMovie, &bounds, mcTopLeftMovie | mcNotVisible);
    SetMovieGWorld(theMovie, offWorld, NULL);
    SetMovieActive(theMovie, true);
    SetMovieDrawingCompleteProc(theMovie, movieDrawingCallWhenChanged,
				myDrawCompleteProc, (long)offWorld); 
    MCDoAction(thePlayer, mcActionPrerollAndPlay, (void *)Long2Fix(1));
  }

  ~quicktimeCache()
  {
    if (thePlayer) DisposeMovieController(thePlayer);
    if (theMovie) DisposeMovie(theMovie);
    if (myDrawCompleteProc) DisposeMovieDrawingCompleteUPP(myDrawCompleteProc);
  }

  MovieController  thePlayer;
  Movie		   theMovie;
  GWorldPtr	offWorld;
  MovieDrawingCompleteUPP  myDrawCompleteProc;
  miInteger     uvSet;
};





EXTERN_C DLLEXPORT int gg_quicktime_version(void) {return(1);}


EXTERN_C DLLEXPORT void
gg_quicktime_init(
	      miState* const        state,
	      struct gg_quicktime_t* p,
	      miBoolean* req_inst
	      )
{
  if ( !p ) 
    {  // global shader init, request per instance init
#ifdef WIN32
      InitializeQTML(0);
#endif
      EnterMovies();
      *req_inst = miTRUE; return;
    }
  
   miTag fileNameTag = *mi_eval_tag( &p->filename );
   
   const char* name = (char*) mi_db_access( fileNameTag );
   
   OSError result;
   Movie		theMovie = NULL;

#ifdef WIN32
   FSSpec theFSSpec;
   short  resRefNum = -1;
   result = NativePathNameToFSSpec(name, &theFSSpec, 0);
   if (result)
     {
       mi_error("NativePathNameToFSSpec failed %d\n", result);
       mi_db_unpin( fileNameTag );
       return;
     }

   result = OpenMovieFile(&theFSSpec, &resRefNum, 0);
   if (result) 
     {
       mi_error("OpenMovieFile failed %d\n", result);
       mi_db_unpin( fileNameTag );
       return;
     }
   
   short actualResId = DoTheRightThing;
   result = NewMovieFromFile(&theMovie, resRefNum, &actualResId, 
			     (unsigned char *) 0, 0, (Boolean *) 0);
   if (result) 
     {
       mi_error("NewMovieFromFile failed %d\n", result); 
       mi_db_unpin( fileNameTag );
       return; 
     }
   
   if (resRefNum != -1)
     CloseMovieFile(resRefNum);
#else
   // Using Data Reference calls now
   OSType      myDataRefType;
   Handle      myDataRef = NULL;
   CFStringRef inPath;

   // Convert movie path to CFString
   inPath = CFStringCreateWithCString(NULL, name, CFStringGetSystemEncoding());
   if (!inPath) 
     { 
       mi_error("Could not get CFString \n"); 
       mi_db_unpin( fileNameTag );
       return;
     }

   // create the data reference
   result = QTNewDataReferenceFromFullPathCFString(inPath,
						   kQTNativeDefaultPathStyle,
						   0, &myDataRef, 
						   &myDataRefType);
   if (result) 
     { 
       mi_error("Could not get DataRef %d\n", result); 
       mi_db_unpin( fileNameTag );
       return; 
     }

   // get the Movie
   result = NewMovieFromDataRef(&theMovie, newMovieActive,
				&actualResId, myDataRef, myDataRefType);
   if (result) 
     { 
       mi_error("Could not get Movie from DataRef %d\n", result); 
       mi_db_unpin( fileNameTag );
       return; 
     }

   // dispose the data reference handle - we no longer need it
   DisposeHandle(myDataRef);
#endif

   mi_db_unpin( fileNameTag );
   

   quicktimeCache* cache = new quicktimeCache( theMovie );
   cache->uvSet = mr_eval( p->uvSet );
   
   void **user;
   mi_query(miQ_FUNC_USERPTR, state, 0, &user);
   *user = cache;
}

EXTERN_C DLLEXPORT void
gg_quicktime_exit(
	      miState* const        state,
	      struct gg_quicktime_t* p
	      )
{
  if ( !p )
  {
     return;
  }
  
  void **user;
  mi_query(miQ_FUNC_USERPTR, state, 0, &user);
  
  quicktimeCache* cache = static_cast<quicktimeCache*>( *user );
  if (cache) delete cache;
}


EXTERN_C DLLEXPORT miBoolean 
gg_quicktime(
	 miScalar* const result,
	 miState* const state,
	 struct gg_quicktime_t* p
	 )
{
   void **user;
   mi_query(miQ_FUNC_USERPTR, state, 0, &user);
   quicktimeCache* cache = static_cast<quicktimeCache*>( *user );
   if (!cache)
   {
      *result = 0.0f;
      return miTRUE;
   }

   // This is done to match maya's texture orientation
   state->tex.x = state->tex_list[cache->uvSet].x;
   state->tex.y = 1.0f - state->tex_list[cache->uvSet].y;

   
   return(miTRUE);
}








#endif

