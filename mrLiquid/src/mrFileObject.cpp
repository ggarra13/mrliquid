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

#include "mrFileObject.h"
#include "mrOptions.h"
#include "mentalFileObjectShape.h"


extern int frame;



void mrFileObject::getRealName(bool& useNamespace, short& nameType)
{ 
   DBG(name << "  mrFileObject::getRealName()");

   MPlug p;
   MStatus status;
   MFnDagNode fn( path );
   GET( nameType );

   switch(nameType)
   {
      case mentalFileObjectShape::kObject:
	 realName = fn.name();
	 break;
      case mentalFileObjectShape::kShortPath:
	 realName = fn.partialPathName();
	 break;
      case mentalFileObjectShape::kFullPath:
	 realName = fn.fullPathName();
	 break;
   }

   GET( useNamespace );

   if ( useNamespace && nameType != mentalFileObjectShape::kFullPath )
   {
      MString nameSpace = fn.fullPathName();
      int idx = nameSpace.rindex(':');
      if ( idx > 0 )
      {
	 realName = nameSpace.substring(0, idx) + realName;
      }
   }
   else if ( !useNamespace && nameType != mentalFileObjectShape::kObject )
   {
      int idx = realName.rindex(':');
      if ( idx > 0 )
      {
	 realName = realName.substring(idx+1, realName.length()-1);
      }
   }
}


void mrFileObject::getData()
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


mrFileObject::mrFileObject( const MDagPath& shape ) :
mrObject( shape )
#ifdef GEOSHADER_H
, function2( miNULLTAG )
#endif
{
   getRealName(oldUseNamespace, oldNameType);
   getData();
}


mrFileObject::~mrFileObject()
{
   // Delete any cached data here...
}


void mrFileObject::forceIncremental()
{
   DBG(name << "  mrFileObject::forceIncremental()");
   bool useNamespace;
   short nameType;
   getRealName(useNamespace, nameType);
   getData();
   mrObject::forceIncremental();
   if ( (oldUseNamespace != useNamespace) ||
	(oldNameType != nameType) )
   {
      MString msg = name;
      msg += ": Changing namespace or name type while in IPR"
      " can lead to wrong behavior";
      LOG_WARNING(msg);
      oldUseNamespace = useNamespace;
      oldNameType = nameType;
      written = kNotWritten;
   }
}

void mrFileObject::setIncremental( bool sameFrame )
{
   DBG(name << "  mrFileObject::setIncremental()");
   getData();
   mrObject::setIncremental( sameFrame );
}

mrFileObject* mrFileObject::factory( MDagPath& shape )
{
   mrFileObject* s;
   char written = mrNode::kNotWritten;
   mrShape* base = mrShape::find(shape);
   if ( base )
   {
      s = dynamic_cast< mrFileObject* >( base );
      if ( s != NULL ) return s;
      DBG("Warning:: redefined object type " << base->name);
      written = base->written;
      nodeList.remove( nodeList.find( base->name ) );
   }
   s = new mrFileObject( shape );
   s->written = written;
   nodeList.insert( s );
   return s;
}


void mrFileObject::getMotionBlur( const char step )
{
// file objects already come with moblur baked in, so nothing to do here...
}


// Extract bounding box from maya's bounding box plugs for object
void mrFileObject::calculateBoundingBoxes( BBox& box, BBox& mbox )
{
   DBG("fileobj calc bbox");
   MStatus status;
   MFnDagNode fn( path );
   MPlug p = fn.findPlug( "boundingBoxMin", true );
   MObject obj;
   p.getValue( obj );
   MFnNumericData fnNum( obj );
   fnNum.getData( box.min.x, box.min.y, box.min.z );
   
   p = fn.findPlug("boundingBoxMax", true);
   p.getValue( obj );
   fnNum.setObject( obj );
   fnNum.getData( box.max.x, box.max.y, box.max.z );


   valid = false;
   if ( box.min.x != box.max.x || box.min.y != box.max.y ||
	box.min.z != box.max.z ) valid = true;

   float x,y,z;
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



void mrFileObject::write( MRL_FILE* f )
{
   if ( options->exportFilter & mrOptions::kObjects )
      return;
   
   // If we are just spitting a portion of the scene 
   // (ie. just objects and associated stuff), spit
   //  objects incrementally.
   // ... and with bounding boxes (in write_properties)
   if ( options->exportFilter & kIncFilter )
   {
      LOG_WARNING("You cannot have incremental/recursive file objects");
      written = kIncremental;
   }
   
   switch( written )
   {
      case kWritten:
	 return; break;
      case kIncremental:
	 MRL_PUTS( "incremental "); break;
   }
   
   MRL_FPRINTF(f, "object \"%s\"\n", realName.asChar() );

   // Set it to fragment export, so we get the bounding 
   // box of the object...
   bool oldExport = options->fragmentExport;
   options->fragmentExport = true;
   write_properties(f);
   options->fragmentExport = oldExport;

   if ( valid )
   {
      TAB(1);
      MRL_FPRINTF(f, "file \"%s\"\n", miFile.asChar() );
   }

   MRL_PUTS( "end object\n" );
   NEWLINE();
   
   written = kWritten;
}



#ifdef GEOSHADER_H
#include "raylib/mrFileObject.inl"
#endif
