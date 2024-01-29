//
// To compile under Windows:
// 
// cl dlopen.cpp -DWIN32 -EHsc
//
// under Linux:
//
// g++ dlopen.cpp -ldl -o dlopen
//

#include <iostream>

#ifdef WIN32
#include <windows.h> 
#else 
#include <dlfcn.h>
#endif

using namespace std;

int help()
{
   cerr << "dlopen - open a dll/.so for verification of symbols" << endl
	<< endl
	<< "This tool can be used to verify that a .so/.dll/.mll file " << endl
	<< "is not missing any library, without having to load and run" << endl
	<< "the actual application (Maya, mentalray, etc)" << endl << endl 
	<< "Usage:" << endl << endl
	<< "dlopen DLLFILE" << endl << endl
	<< "Example:" << endl << endl
	<< "dlopen mrLibrary.dll" << endl << endl;
   return 1;
}

extern "C" int main(int argc, const char* argv[])
{
   if ( argc != 2 ) return help();

   cerr << "Trying to open:" << endl << "\"" << argv[1] << "\"..." << endl;

#if WIN32
   HMODULE dl_handle = LoadLibrary( argv[1] );
#else
   void* dl_handle = dlopen( argv[1], RTLD_LAZY );
#endif

   if ( dl_handle == NULL )
   {
#if WIN32
      char* lpMsgBuf;
      DWORD err = GetLastError(); 

      FormatMessage(
		    FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		    FORMAT_MESSAGE_FROM_SYSTEM,
		    NULL,
		    err,
		    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		    (LPTSTR) &lpMsgBuf,
		    0, NULL );

      cerr << "Cannot open library: " << lpMsgBuf << endl;
      LocalFree(lpMsgBuf);
#else
      int err = 1;
      cerr << "Cannot open library: " << dlerror() << endl;
#endif
      return err;
   }

   cerr << "Success." << endl;

#if WIN32
   FreeLibrary( dl_handle ); 
#else
   dlclose( dl_handle );
#endif
}
