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


#include "maya/MFnNumericData.h"
#include "maya/MFnDagNode.h"
#include "maya/MPlug.h"
#include "maya/MTime.h"

#include "mrFileAssembly.h"
#include "mrOptions.h"


extern int frame;



void mrFileAssembly::getData()
{
   MStatus status;
   MFnDagNode fn( path );
   MPlug p = fn.findPlug( "file", true );
   MString oldFile = miFile;
   p.getValue( miFile );

   MTime inTime;
   GET( inTime );

   int oldFrame = frame;
   frame = (int) inTime.as( MTime::uiUnit() );
   miFile = parseString( miFile );
   frame = oldFrame;

   if ( written != kNotWritten && miFile != oldFile )
      written = kIncremental;
}


mrFileAssembly::mrFileAssembly( const MDagPath& shape ) :
mrObject( shape )
#ifdef GEOSHADER_H
, function2( miNULLTAG )
#endif
{
   getData();
}


mrFileAssembly::~mrFileAssembly()
{
   // Delete any cached data here...
}


void mrFileAssembly::forceIncremental()
{
   DBG(name << "  mrFileAssembly::forceIncremental()");
   getData();
   mrObject::forceIncremental();
}

void mrFileAssembly::setIncremental( bool sameFrame )
{
   DBG(name << "  mrFileAssembly::setIncremental()");
   getData();
   mrObject::setIncremental( sameFrame );
}

mrFileAssembly* mrFileAssembly::factory( MDagPath& shape )
{
   mrFileAssembly* s;
   char written = mrNode::kNotWritten;
   mrShape* base = mrShape::find(shape);
   if ( base )
   {
      s = dynamic_cast< mrFileAssembly* >( base );
      if ( s != NULL ) return s;
      DBG("Warning:: redefined object type " << base->name);
      written = base->written;
      nodeList.remove( nodeList.find( base->name ) );
   }
   s = new mrFileAssembly( shape );
   s->written = written;
   nodeList.insert( s );
   return s;
}



// Extract bounding box from maya's bounding box plugs for object
void mrFileAssembly::calculateBoundingBoxes( BBox& box, BBox& mbox )
{
   DBG("fileassembly calc bbox");
   float x,y,z;
   MFnDagNode fn( path );
   MPlug p = fn.findPlug( "boxMin", true );

   MObject obj;
   p.getValue( obj );
   MFnNumericData fnNum( obj );
   fnNum.getData( x, y, z );
   box.min = MVector(x,y,z);
   
   p = fn.findPlug("boxMax", true);
   p.getValue( obj );
   fnNum.setObject( obj );
   fnNum.getData( x, y, z );
   box.max = MVector(x,y,z);

   p = fn.findPlug("motionBoxMin", true);
   p.getValue( obj );
   fnNum.setObject( obj );
   fnNum.getData( x, y, z );
   mbox.min = MVector(x,y,z);

   p = fn.findPlug("motionBoxMax", true);
   p.getValue( obj );
   fnNum.setObject( obj );
   fnNum.getData( x, y, z );
   mbox.max = MVector(x,y,z);

}


void mrFileAssembly::write( MRL_FILE* f )
{

   mrObject::BBox box, mbox;

   calculateBoundingBoxes( box, mbox );

   if ( box.min.x == box.max.x || 
	box.min.y == box.max.y ||
	box.min.z == box.max.z )
     {
       MString err = "Invalid bounding box for assembly \"";
       err += name;
       err += "\"";
       LOG_WARNING(err);
       return;
     }

   MString filename = file();
   if ( filename.length() == 0 )
     {
       MString err = "Invalid assembly file for \"";
       err += name;
       err += "\"";
       LOG_WARNING(err);
       return;
     }


   switch( written )
   {
      case kWritten:
	 return; break;
      case kIncremental:
	 MRL_PUTS( "incremental "); break;
   }
   
   MRL_FPRINTF(f, "assembly \"%s\"\n", name.asChar() );
   TAB(1);
   MRL_FPRINTF(f, "bbox %g %g %g  %g %g %g\n",
	       box.min.x, box.min.y, box.min.z,
	       box.max.x, box.max.y, box.max.z );
   
   if ( options->motionBlur != mrOptions::kMotionBlurOff )
     {
       TAB(1);
       MRL_FPRINTF(f, "mbox %g %g %g  %g %g %g\n",
		   mbox.min.x, mbox.min.y, mbox.min.z,
		   mbox.max.x, mbox.max.y, mbox.max.z );
     }

   TAB(1);
   MRL_FPRINTF(f, "file \"%s\"\n", filename.asChar() );

   MRL_PUTS( "end assembly\n" );
   NEWLINE();
   
   written = kWritten;
}

