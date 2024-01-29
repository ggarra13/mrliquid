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


#ifndef mrMesh_h
#define mrMesh_h

class MFnMesh;

#include <vector>
#include <map>

#ifndef mrObject_h
#include "mrObject.h"
#endif

#ifndef mrUserDataVertex_h
#include "mrUserDataVertex.h"
#endif

class MItMeshPolygon;

//! Base class used for polygonal meshes and for polygonal
//! meshes with subd approximations.
class mrMeshBase : public mrObject
{

public:
  static mrMeshBase* factory( const MDagPath& shape );
  virtual ~mrMeshBase();

  virtual void forceIncremental();
  virtual void setIncremental( bool sameFrame );
  virtual void getMotionBlur(  const char step );
     
protected:
  mrMeshBase( const MString& name );
  mrMeshBase( const MDagPath& shape );

     
  unsigned        write_user( MRL_FILE* f );

  void getInfo();
  void getData( bool sameFrame );

protected:     
  bool trilist;
  bool hasReferenceObject;

  mrUserDataVertex* vxdata;

  std::vector< unsigned > meshTriangles;
     
#ifdef GEOSHADER_H
  miTag function2;
  
  unsigned             write_user();
#endif
};



class mrMesh : public mrMeshBase
{
  friend class mrMeshBase;
     
public:
  virtual ~mrMesh();
  
  void write_lightmap_data( MRL_FILE* f,
			    const MString& material );
  
  void write_trilist( MRL_FILE* f );

protected:
  mrMesh( const MString& name );
  mrMesh( const MDagPath& shape );

  struct VertexStore {
    unsigned id;
    unsigned vid;
    unsigned nid;
    MIntArray uvId;
  };
  typedef std::multimap< unsigned int, VertexStore* > VertexHash_t;
  VertexHash_t vertexHash;

  struct holeStore {
    holeStore( unsigned num, unsigned off ) : 
      numVerts(num), offset(off) 
    {}

    unsigned numVerts;
    unsigned offset;
  };

  typedef std::multimap< unsigned int, holeStore* >    HolesHash_t;
  HolesHash_t holesHash;
  MIntArray   holeVerts;

  typedef std::vector< VertexStore* >                 TriList_t;
  TriList_t triList;

  void getLocalIndex( const unsigned numVerts,
		      const unsigned vertbase,
		      const MIntArray& vertexIds,
		      const int getTriangles[3],
		      int indices[3] );


  VertexStore* in_hash( const unsigned polyId,
			const unsigned i, const unsigned id,
			const MFloatVectorArray& n, const unsigned nid,
			const unsigned*    uvbase,
			const MIntArray*   uvCounts,
			const MIntArray*   uvIds,
			const MFloatArray* u, const MFloatArray* v,
			const unsigned      numUVSets,
			const MStringArray& uvsets );


  void hash_trilist_vertex( const unsigned polyId,
			    const unsigned lidx,
			    const unsigned id,
			    const MFloatVectorArray& n,
			    const unsigned nid,
			    const unsigned*    uvbase,
			    const MIntArray*   uvCounts,
			    const MIntArray*   uvIds,
			    const MFloatArray* u,
			    const MFloatArray* v,
			    const unsigned      numUVSets,
			    const MStringArray& uvsets );

  void write_trilist_vertex( MRL_FILE* f, 
			     const VertexStore* const s,
			     const MFloatVectorArray& n,
			     const unsigned numUVSets,
			     const MFloatArray* u,
			     const MFloatArray* v,
			     const MPointArray& pref );
  
  void calculateDerivatives( const MFloatArray& u,
			     const MFloatArray& v,
			     const MFloatVectorArray& normals,
			     const MIntArray& vertexCounts,
			     const MIntArray& vertexIds,
			     const MIntArray& normalCounts,
			     const MIntArray& normalIds,
			     const MIntArray& uvCounts,
			     const MIntArray& uvIds );


  virtual void   write_object_definition( MRL_FILE* f );
  virtual void        write_group( MRL_FILE* f );
  virtual void   write_approximation( MRL_FILE* f );

  MFloatVectorArray dPdu;
  MFloatVectorArray dPdv;


#ifdef GEOSHADER_H
public:
  void write_lightmap_data( const MString& material );

protected:
  void write_trilist_vertex( std::vector< miVector >& vectors, 
			     const VertexStore* const s,
			     const MFloatVectorArray& n,
			     const unsigned numUVSets,
			     const MFloatArray* u,
			     const MFloatArray* v,
			     const MPointArray& pref );

  void write_trilist();
  virtual void   write_object_definition( miObject* o );
  virtual void        write_group();
  virtual void   write_approximation();
#endif
};


class mrMeshInternal : public mrMesh
{
   public:
     static mrMeshInternal* factory( const MString& name,
				     const MObject& meshObj );

   protected:
#if MAYA_API_VERSION >= 600
     MObjectHandle handle;
     virtual MObject node() const throw() { return handle.object(); }
#else
     MObject handle;
     virtual MObject node() const throw() { return handle; }
#endif

   protected:
     mrMeshInternal( const MString& name, const MObject& obj );

};

#endif // mrMesh_h
