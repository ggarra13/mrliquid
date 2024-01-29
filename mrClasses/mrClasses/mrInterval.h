
#ifdef mrMacros_h
#include "mrMacros.h"
#endif


BEGIN_NAMESPACE( mr )

template< typename T >
class interval
{
     T a, b;
   public:
     interval();
     interval( const T& A, const T& B );

     
    //--------------------------------
    //  Operators:  we get != from STL
    //--------------------------------
    
    bool                        operator == (const interval<T> &src) const;

    //------------------
    //	interval manipulation
    //------------------

    void			makeEmpty();
    void			extendBy(const T& point);
    void			extendBy(const interval<T>& b);

    //---------------------------------------------------
    //	Query functions - these compute results each time
    //---------------------------------------------------

    T				size() const;
    T				center() const;
    bool			intersects(const T& point) const;
    bool			intersects(const interval<T>& b) const;

    //----------------
    //	Classification
    //----------------

    bool			hasVolume() const;
    bool			isEmpty() const;
};

END_NAMESPACE( mr )

