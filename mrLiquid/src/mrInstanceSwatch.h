
#include "mrInstanceObject.h"

class mrInstanceSwatch : public mrInstanceObject
{
   public:
     enum SwatchType
     {
     kBall,
     kCube,
     kPlane,
     kCylinder,
     kCone,
     kTorus,
     };

   protected:
     mrInstanceSwatch( const MString& name );

     void write_geoshader( MRL_FILE* f );
     virtual void write_properties( MRL_FILE* f ) throw();
   
   public:
     virtual void write( MRL_FILE* f );

     virtual void forceIncremental();

   public:
     static mrInstanceSwatch* factory( const MString& name );

   public:
     SwatchType type;


#ifdef GEOSHADER_H
   protected:
     void write_geoshader();
     virtual void write_properties( miInstance* i ) throw();

   public:
     //! Output instance to mray's api
     virtual void write();

   protected:
     miTag function;
#endif
};
