
#ifndef mrShadingGroupFluid_h
#define mrShadingGroupFluid_h

#ifndef   mrShadingGroup_h
#include "mrShadingGroup.h"
#endif

class mrShape;

class mrShadingGroupFluid : public mrShadingGroup
{
   protected:
     mrShadingGroupFluid( const MFnDependencyNode& fn, const char* contain );

     void getData();

   public:

     virtual void forceIncremental();

     static mrShadingGroupFluid* factory(
					 const MFnDependencyNode& fn,
					 const char*  contain = NULL,
					 const mrShape* shape = NULL
					 );
};

#endif // mrShadingGroupFluid_h
