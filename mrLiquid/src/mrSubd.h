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

#ifndef mrSubd_h
#define mrSubd_h

#include <map>

class MFnSubd;
class MUint64Array;

#ifndef mrObject_h
#include "mrObject.h"
#endif


class mrSubd : public mrObject
{
public:
     static mrSubd* factory( const MDagPath& shape );

     virtual void forceIncremental();
     virtual void setIncremental( bool sameFrame );
     virtual void getMotionBlur( const char step );
     
  enum TextureInterpolation
    {
      kSubdivisionInterpolation,
      kFaceInterpolation,
      kVertexInterpolation,
    };

protected:
     void getData( bool sameFrame );

     void deleteSubd( MObject& node );

     void skip_detail(
		      unsigned& vidx,
		      const MFnSubd& m,
		      const MUint64 face,
		      const int idx = -1
		      );

     void write_detail(
		       MRL_FILE* f,
		       unsigned& vidx,
		       const MFnSubd& m,
		       const MUint64 face,
		       const int idx = -1
		       );
     void writeVertexChildren( MRL_FILE* f,
			       unsigned& uvId, 
			       MFnSubd& m,
			       const MUint64 face,
			       const short child,
			       const unsigned motionOffset,
			       const unsigned numMeshVerts );

     void writeUVChildren( MRL_FILE* f,
			      unsigned& motionOffset,
			      const MFnSubd& m,
			      const MUint64 face );


     void getMaterialIds(
			 MUint64Array& faces,
			 MIntArray& materialId,
			 const MFnSubd& m,
			 const unsigned numPolygons
			 );
     
     virtual void           write_group( MRL_FILE* f );
     virtual void   write_approximation( MRL_FILE* f );
     
     mrSubd( const MDagPath& shape );
     virtual ~mrSubd();


#ifdef GEOSHADER_H

     void write_detail(
		       unsigned& vidx,
		       const MFnSubd& m,
		       const MUint64 face,
		       const int idx
		       );

     void writeVertexChildren( unsigned& uvId, 
			       const MFnSubd& m,
			       const MUint64 face,
			       const short child,
			       const unsigned motionOffset,
			       const unsigned numMeshVerts );

     void writeUVChildren( unsigned& motionOffset,
			   const MFnSubd& m,
			   const MUint64 face );
     
     virtual void           write_group();
     virtual void   write_approximation();
#endif

     typedef std::map< MUint64, unsigned >  MrayVertId;
     MrayVertId mrayId;


  MObject levelOneObj;
  bool levelOne;
  bool hasReferenceObject;
};

#endif // mrSubd_h
