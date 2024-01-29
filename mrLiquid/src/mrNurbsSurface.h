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

#ifndef mrNurbsSurface_h
#define mrNurbsSurface_h

#include <set>

#ifndef mrObject_h
#include "mrObject.h"
#endif


class mrShaderBall;

class mrNurbsSurface : public mrObject
{
   public:
     friend class mrShaderBall;

   public:
     static mrNurbsSurface* factory( const MDagPath& shape );

     virtual void forceIncremental();
     virtual void setIncremental( bool sameFrame );
     virtual void getMotionBlur( const char step );

   protected:

     mrNurbsSurface( const MString& name );
     mrNurbsSurface( const MDagPath& shape );
     virtual ~mrNurbsSurface();

     
     virtual void getData( bool sameFrame );
     void write_knots( MRL_FILE* f,
		       const int form,
		       const MDoubleArray& knots ) const;

     virtual void        write_properties( MRL_FILE* f );
     virtual void        write_group( MRL_FILE* f );
     virtual void   write_approximation( MRL_FILE* f );

#ifdef GEOSHADER_H
     miDlist* create_knots( const int form,
			    MDoubleArray& knots ) const;
     
     virtual void        write_properties( miObject* o );
     virtual void        write_group();
     virtual void   write_approximation();
#endif
     
     
     bool trims, hasReferenceObject;

     typedef std::set< int > BasisList;
     BasisList      basis;
     int degreeU, degreeV;
};

#endif // mrNurbsSurface_h
