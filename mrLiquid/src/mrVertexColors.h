
#ifndef mrVertexColors_h
#define mrVertexColors_h

#ifndef mrShaderMaya_h
#include "mrShaderMaya.h"
#endif

class mrVertexColors : public mrShaderMaya
{
   public:
     mrVertexColors( const MFnDependencyNode& fn );
     ~mrVertexColors() {};

     void write_shader_parameters( MRL_FILE* f,
				   MFnDependencyNode& dep,
				   const ConnectionMap& connNames );

#ifdef GEOSHADER_H
     void write_shader_parameters( MFnDependencyNode& dep,
				   const ConnectionMap& connNames );
#endif

//      static mrVertexColors* factory( const MFnDependencyNode& fn,
// 				     const char* const container );
};

#endif // mrVertexColors_h
