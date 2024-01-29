/**
 * @file   mrTimer.h
 * @author gga
 * @date   Thu Aug 30 06:50:38 2007
 * 
 * @brief  This file is a simple timer class to profile small code areas.
 *         @todo: rdtsc should no longer be used on multicpu machines.  Fix.
 * 
 */

#ifndef mrTimer_h
#define mrTimer_h


namespace mr
{

inline void rdtsc( unsigned long long& ts )
{
  __asm__ __volatile__ ("rdtsc" : "=A" (ts) );
}



class mrTimer {

  static unsigned long long overhead;
  unsigned long long ts1, ts2, _total, _count;

public:
  mrTimer() :
    _total( 0 ),
    _count( 0 )
  {
    static bool calibrated = false;
    if ( !calibrated )
      {
	calibrated = true;
	overhead = 0;
	for ( int i = 0; i < 10; ++i ) { start(); stop(); }
	overhead = _total / 10; _total = 0; _count = 0;
      }
  }

  inline void start() { rdtsc(ts1); ++_count; }
  inline void stop()  { rdtsc(ts2); _total += ts2 - ts1 - overhead; }


  inline unsigned long long count() { return _count; }
  inline unsigned long long total() { return _total; }
  
};


typedef std::map< std::string, mrTimer > mrTimerMap;


extern mrTimerMap* timers;

mrTimer& getTimer( const char* name )
{
  return (*timers)[name];
}

void startTimer( const char* name )
{
  getTimer( name ).start();
}

void stopTimer( const char* name )
{
  getTimer( name ).stop();
}


} // namespace mr


#endif // mrTimer_h



