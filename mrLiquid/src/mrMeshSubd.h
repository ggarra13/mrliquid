//
//  Copyright (c) 2004, Gonzalo Garramuno
//
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are
//  met:
//  *       Redistributions of source code must retain the above copyright
//  notice, this list of conditions and the following disclaimer.
//  *       Redistributions in binary form must reproduce the above
//  copyright notice, this list of conditions and the following disclaimer
//  in the documentation and/or other materials provided with the
//  distribution.
//  *       Neither the name of Gonzalo Garramuno nor the names of
//  its other contributors may be used to endorse or promote products derived
//  from this software without specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//


#ifndef mrMeshSubd_h
#define mrMeshSubd_h

#include <map>

#ifndef mrMesh_h
#include "mrMesh.h"
#endif


class mrMeshSubd : public mrMeshBase
{
  friend class mrMeshBase;

  typedef std::map< unsigned int, double > EdgeCreases;

  enum Conversion
    {
      kNone,
      kQuadsToTriangles,
      kTrianglesToQuads
    };
     
protected:
     mrMeshSubd( const MDagPath& shape );

     void getData();
     virtual void   forceIncremental();
     virtual void           write_group( MRL_FILE* f );
     virtual void   write_approximation( MRL_FILE* f );

  struct VertexStore {
    unsigned  vid;
    MIntArray uvId;
  };

  typedef std::multimap< unsigned int, VertexStore* > VertexHash_t;
  VertexHash_t vertexHash;

  VertexStore* in_hash( const unsigned polyId,
			const unsigned i, const unsigned id,
			const unsigned*    uvbase,
			const MIntArray*   uvCounts,
			const MIntArray*   uvIds,
			const MFloatArray* u, const MFloatArray* v,
			const unsigned numUVSets,
			const MStringArray& uvsets );
     
public:
     virtual ~mrMeshSubd();
    
#ifdef GEOSHADER_H
     virtual void           write_group();
     virtual void   write_approximation();
#endif
     
protected:
  bool quads:1, tris:1, useCCMesh;
  short conversion;
};

#endif // mrMeshSubd_h
