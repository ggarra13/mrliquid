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
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
#ifndef mrPfxHair_h
#define mrPfxHair_h


#include "mrPfxBase.h"
#include "mrHairInfo.h"


class MRenderLineArray;

class mrPfxHair : public mrPfxBase
{
   public:
     enum HairTypes
     {
       kHairInvalid     = -1,
       kHairGuides      = 0,
       kHairEmulate     = 1,
       kHairInterpolate = 2,
       kHairPaintFX     = 3
     };

   public:
     static mrPfxHair* factory( const MDagPath& shape );

     virtual void getMotionBlur( const char );
     virtual void setIncremental( bool sameFrame );
     virtual void forceIncremental();

   protected:
     //! Get data of hair node
     virtual void getData();

     //! Get the filename for .hr file
     MString getHR() const;

     //! Given an object, a curve and its corresponding follicle, add the hair
     //! as a guide hair.
     void addHairGuide(
		       const MDagPath& objPath,
		       const MDagPath& curvePath,
		       const MDagPath& folliclePath
		       );

     void addHairMoblur(
			unsigned idx,
			const MDagPath& objPath,
			const MDagPath& curvePath,
			const MDagPath& folliclePath,
			char step
			);
     void getHairSystemMoblur( const MObject& hairSystemObj, char step );
     void getHairSystemInfo( const MObject& hairSystemObj );

     virtual void write_group( MRL_FILE* f ) {};
     virtual void write( MRL_FILE* f );

     void write_hr_files();

#ifdef GEOSHADER_H
     virtual void write_group() {};
     virtual void write();
#endif

     //! Constructor
     mrPfxHair( const MDagPath& shape );



#if MAYA_API_VERSION >= 650
     void getPfxInfo();
     void getPfxMoblur( char step );
#endif

   protected:
     hairSystem h;
};


#endif // mrPfxHair_h
