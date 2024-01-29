#ifndef mrStandalone_h
#define mrStandalone_h

#include "mrPipe.h"

class mrTranslator;

//
// Functions used for controlling the mray stand-alone
//
class mrStandalone
{
   public:
     mrStandalone();
     ~mrStandalone();

     //! Set the mentalray stand-alone environment properly.
     void set_environment();

     //! Returns true if stand-alone is running.
     bool is_running();

     //! Wait for the mentalray stand-alone to exit.
     void wait();

     //! Kill the mentalray stand-alone.
     bool kill();

     //! Start a new mentalray stand-alone.  Return its file descriptors.
     bool start(FILE*& rayin, FILE*& rayout, FILE*& rayerr); 

   protected:
     MR_PROCESS id;
};


#endif // mrStandalone_h
