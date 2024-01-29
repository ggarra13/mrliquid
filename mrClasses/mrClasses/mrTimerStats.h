
#ifndef mrTimerStats_h
#define mrTimerStats_h

#include "mrTimer.h"

namespace mr
{

//!
//! Functor used to sort timers based on their execution time
//!
struct mrTimerSort
{
  bool operator()( const mrTimerMap::iterator& a, 
		   const mrTimerMap::iterator& b )
  {
    return a->second.total() > b->second.total();
  }
};


struct mrTimerStats
{
  unsigned long long startTicks;
  double startSecs;

  mrTimerStats()
  {
    rdtsc( startTicks );
    struct timeval tv; gettimeofday( &tv, 0 );
    startSecs = tv.tv_sec + 1e-6 * tv.tv_usec;
  }

  ~mrTimerStats()
  {
    unsigned long long stopTicks;
    rdtsc( stopTicks );
    struct timeval tv; gettimeofday( &tv, 0 );
    double stopSecs = tv.tv_sec + 1e-6 * tv.tv_usec;
    double totalSecs = stopSecs - startSecs;
    double secsPerTick = totalSecs / (stopTicks - startTicks );

    std::vector< mrTimerMap::iterator > sorted;
    for ( mrTimerMap::iterator i = timers->begin();
	  i != timers->end(); ++i )
      {
	sorted.push_back( i );
      }
    
    if ( sorted.empty() ) return;
    std::sort( sorted.begin(), sorted.end(), mrTimerSort() );

    // print entries more than 1% ( at least 10 entries )
    double smin = sorted[ std::min( 9, int( sorted.size() - 1 ) )]->second.total() * secsPerTick;
    double threshold = std::min( totalSecs / 100, smin );
    mi_info( "%-37s %-16s %s\n", 
	     "Timer Name", " Seconds  ( pct )", "Count" ); 
    std::vector< mrTimerMap::iterator > iter = sorted.begin();
    for ( ; iter != sorted.end(); ++iter )
      {
	const char* name = (*iter)->first.c_str();
	double secs = (*iter)->second->total() * secsPerTick;
	unsigned long long count = (*iter)->second->count();
	double percent = secs / totalSecs * 100;
	if ( secs >= threshold )
	  {
	    mi_info( "  %-35s %8.2f (%4.1f%%) %10.10g\n", 
		     name, secs, percent, double(count) ); 
	  }
      }
  } // ~mrTimerStats

} // class mrTimerStats

} // namespace mr

#endif // mrTimerStats_h
