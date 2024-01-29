
#ifndef mrShadingGroupPfxHair_h
#define mrShadingGroupPfxHair_h

#ifndef   mrShadingGroup_h
#include "mrShadingGroup.h"
#endif

class mrShape;

class mrShadingGroupPfxHair : public mrShadingGroup
{
   protected:
     mrShadingGroupPfxHair( const MFnDependencyNode& fn, const char* contain );

     void getData();

   public:

     virtual void forceIncremental();

     static mrShadingGroupPfxHair* factory(
					   const MFnDependencyNode& fn,
					   const char*  contain = NULL,
					   const mrShape* shape = NULL
					   );
};

#endif // mrShadingGroupPfxHair_h
