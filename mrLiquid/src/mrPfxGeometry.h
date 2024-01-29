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
#ifndef mrPfxGeometry_h
#define mrPfxGeometry_h


#include "mrPfxBase.h"

class mrMesh;
class MRenderLineArray;

class mrPfxGeometry : public mrPfxBase
{
   public:

     enum BrushType
     {
     kPaint = 0,
     kSmear,
     kBlur,
     kErase,
     kThinLine,
     kMesh = 5
     };


     static mrPfxGeometry* factory( const MDagPath& shape );

     virtual void getMotionBlur( const char );
     virtual void setIncremental( bool sameFrame );
     virtual void forceIncremental();

     inline BrushType brushType() { return (BrushType) brushType_; };

     virtual void write_group( MRL_FILE* f ) {};
     virtual void write( MRL_FILE* f );

#ifdef GEOSHADER_H
     virtual void write_group() {};
     virtual void write();
#endif

     void getPfxInfo();
     bool   hasMain()  { return true; }
     bool hasLeaves()  { return leaves; }
     bool hasFlowers() { return flowers; }

   protected:
     //! Get data of particle node
     virtual void getData();


     //! Constructor
     mrPfxGeometry( const MDagPath& shape );

     //! Destructor
     ~mrPfxGeometry();

     void createMesh( MPlug& p );

     /** 
      * Writes up to 3 .hr files for main lines, leaf lines and
      * flower lines.
      */
     void write_hr_files();


     void getPfxMoblur( char step );

     bool leaves, flowers;

     int spitMain;
     int spitLeaf;
     int spitFlower;

     mrMesh*   mesh;
     mrMesh*   leafMesh;
     mrMesh*   flowerMesh;

     unsigned mainVerts;
     unsigned leafVerts;
     unsigned flowerVerts;

     short brushType_;
};


#endif // mrPfxGeometry_h
