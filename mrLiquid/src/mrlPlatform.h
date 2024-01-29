

#ifndef mrlPlatform_h
#define mrlPlatform_h

#if defined(WIN32) || defined(WIN64)

#define STRICMP(a,b) _stricmp(a,b)
#define ACCESS(a,b) _access(a,b)
#define GETENV(x) _getenv(x)
#define PUTENV(x) _putenv(x)
#define UNLINK(x) _unlink(x)
#define STRDUP(x) _strdup(x)

#else

#define STRICMP(a,b) strcasecmp(a,b)
#define ACCESS(a,b) access(a,b)
#define GETENV(x) getenv(x)
#define PUTENV(x) putenv(x)
#define UNLINK(x) unlink(x)
#define STRDUP(x) strdup(x)

#endif


#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif

#endif // mrlPlatform_h

