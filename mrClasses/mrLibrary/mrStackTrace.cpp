


#if defined(_WIN32) || defined(_WIN64)
#include "mrStackTrace_win32.cpp"
#endif

#ifdef LINUX
#include "mrStackTrace_linux.cpp"
#endif


MR_LIB_EXPORT mr::ExceptionHandler* gExceptionHandler;
