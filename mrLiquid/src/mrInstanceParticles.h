
#ifndef mrInstanceParticles_h
#define mrInstanceParticles_h

#include "mrInstanceObject.h"


class mrInstanceParticles : public mrInstanceObject
{
   public:
     static mrInstanceParticles* factory( const MDagPath& shape, 
					  mrShape* s );

   protected:
     mrInstanceParticles( const MDagPath& instDagPath, mrShape* s );
     
     virtual void getShaderObjects( MObjectArray& shaderObjs,
				    const MDagPath& shapePath );

     virtual void forceIncremental();
     virtual void setIncremental( bool sameFrame );

     void write_geo_shader( MRL_FILE* f );
     virtual void write( MRL_FILE* f );

#ifdef GEOSHADER_H
     void write_geo_shader();
     virtual void write();

   protected:
     miTag geoshader;
#endif

};

#endif // mrInstanceParticles_h
