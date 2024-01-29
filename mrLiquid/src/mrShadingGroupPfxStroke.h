
#ifndef mrShadingGroupPfxStroke_h
#define mrShadingGroupPfxStroke_h

#ifndef   mrShadingGroup_h
#include "mrShadingGroup.h"
#endif

class mrShape;

class mrShadingGroupPfxStroke : public mrShadingGroup
{
   protected:
     mrShadingGroupPfxStroke( const MFnDependencyNode& fn, const char* contain );

     void getData();

   public:

     virtual void forceIncremental();
     virtual void write( MRL_FILE* f );

     static mrShadingGroupPfxStroke* factory(
					   const MFnDependencyNode& fn,
					   const char*  contain = NULL,
					   const mrShape* shape = NULL
					   );

#ifdef GEOSHADER_H
     virtual void write();
#endif

};

#endif // mrShadingGroupPfxStroke_h
