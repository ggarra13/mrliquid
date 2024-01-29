
#ifndef mrShadingGroupFur_h
#define mrShadingGroupFur_h

#ifndef   mrShadingGroup_h
#include "mrShadingGroup.h"
#endif

class mrShape;

class mrShadingGroupFur : public mrShadingGroup
{
protected:
  mrShadingGroupFur( const MFnDependencyNode& fn, const mrShape* shape,
		     const char* contain );
  
  void getData();

public:
  virtual void forceIncremental();

  static mrShadingGroupFur* factory(
				    const MFnDependencyNode& fn,
				    const char*  contain = NULL,
				    const mrShape* shape = NULL
				    );
  
protected:
  const mrShape* shape;
};

#endif // mrShadingGroupFur_h
