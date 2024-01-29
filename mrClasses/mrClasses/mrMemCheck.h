
#ifndef mrMemCheck_h
#define mrMemCheck_h

#ifdef MR_MEM_CHECK

#if defined(WIN32) || defined(WIN64)
#define new     new( __FILE__, __LINE__ )
#else
#define new(...)      new( __VA_ARGS__, __FILE__, __LINE__ )
#endif

#endif // MR_MEM_CHECK

#endif
