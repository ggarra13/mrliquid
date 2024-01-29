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



#if defined(WIN32) || defined(WIN64)
#include <winsock2.h>  // for htonl, etc.
#else
#include <netinet/in.h>
#endif

#include "maya/MFnDagNode.h"
#include "maya/MFnAttribute.h"
#include "maya/MFnNumericAttribute.h"
#include "maya/MFnTypedAttribute.h"
#include "maya/MFnIntArrayData.h"
#include "maya/MFnDoubleArrayData.h"
#include "maya/MFnDependencyNode.h"
#include "maya/MItDependencyNodes.h"
#include "maya/MFnVectorArrayData.h"
#include "maya/MFnData.h"
#include "maya/MStringArray.h"
#include "maya/MIntArray.h"
#include "maya/MTime.h"
#include "maya/MAnimControl.h"
#include "maya/MBoundingBox.h"
#include "maya/MVector.h"

#include "mrParticles.h"

#include "mrHelpers.h"
#include "mrIO.h"
#include "mrByteSwap.h"
#include "mrAttrAux.h"

extern MString partDir;
extern MString sceneName;
extern int frame;


const double kPIXEL_SIZE = 0.1;



int mrParticles::getFrameRate( const MTime::Unit unit )
{
   switch ( unit )
   {
      case MTime::kHours:
	 return 1; break;
      case MTime::kMinutes:
	 return 1; break;
      case MTime::kSeconds:
	 return 1; break;
      case MTime::kMilliseconds:
	 return 1000; break;
      case MTime::kGames:
	 return 15; break;
      case MTime::kFilm:
	 return 24; break;
      case MTime::kPALFrame:
	 return 25; break;
      case MTime::kNTSCFrame:
	 return 30; break;
      case MTime::kShowScan:
	 return 48; break;
      case MTime::kPALField:
	 return 50; break;
      case MTime::kNTSCField:
	 return 60; break;
      case MTime::k2FPS:
	 return 2; break;
      case MTime::k3FPS:
	 return 3; break;
      case MTime::k4FPS:
	 return 4; break;
      case MTime::k5FPS:
	 return 5; break;
      case MTime::k6FPS:
	 return 6; break;
      case MTime::k8FPS:		
	 return 8; break;
      case MTime::k10FPS:	
	 return 10; break;
      case MTime::k12FPS:
	 return 12; break;
      case MTime::k16FPS:
	 return 16; break;
      case MTime::k20FPS:
	 return 20; break;
      case MTime::k40FPS:
	 return 40; break;
      case MTime::k75FPS:
	 return 75; break;
      case MTime::k80FPS:
	 return 80; break;
      case MTime::k100FPS:
	 return 100; break;
      case MTime::k120FPS:
	 return 120; break;
      case MTime::k125FPS:
	 return 125; break;
      case MTime::k150FPS:
	 return 150; break;
      case MTime::k200FPS:
	 return 200; break;
      case MTime::k240FPS:
	 return 240; break;
      case MTime::k250FPS:
	 return 250; break;
      case MTime::k300FPS:
	 return 300; break;
      case MTime::k375FPS:
	 return 375; break;
      case MTime::k400FPS:
	 return 400; break;
      case MTime::k500FPS:
	 return 500; break;
      case MTime::k600FPS:
	 return 600; break;
      case MTime::k750FPS:
	 return 750; break;
      case MTime::k1200FPS:
	 return 1200; break;
      case MTime::k1500FPS:
	 return 1500; break;
      case MTime::k2000FPS:
	 return 2000; break;
      case MTime::k3000FPS:
	 return 3000; break;
      case MTime::k6000FPS:
	 return 6000; break;
      default:
	 return 240;
   }
}


MString mrParticles::getPDCFile() const
{
//    MTime t = MAnimControl::currentTime();
//    double time = t.as( unit );
   MTime::Unit unit = MTime::uiUnit();
   int frameRate = 6000 / getFrameRate( unit );
   long int pdcframe = ((long int) frame) * frameRate;
   char frameStr[15];
   sprintf( frameStr, "%ld", pdcframe);
   
   MString pdcFile = miPDCFile;
   pdcFile += ".";
   pdcFile += frameStr;
   pdcFile += ".pdc";
   return pdcFile;
}


void mrParticles::getData()
{
   {  // Get all stuff possible from MFnParticleSystem
      MFnParticleSystem fn( path );
      visible    = fn.primaryVisibility();
#if MAYA_API_VERSION >= 650
      if ( renderType >= MFnParticleSystem::kBlobby )
      {
	 reflection  = 1 + fn.visibleInReflections();
	 refraction  = 1 + fn.visibleInRefractions();
      }
      else
      {
	 reflection = 3;
	 refraction = 3;
      }
      transparency = 3;
      shadow = fn.castsShadows() + 2 * fn.receiveShadows();
#else
      if ( renderType == MFnParticleSystem::kHardware )
	 trace = true;
      else
	 trace = fn.visibleInReflections() || fn.visibleInRefractions();
      shadow     = fn.castsShadows() || fn.receiveShadows();
#endif
      renderType = fn.renderType();

      volumetric = false;

      // For hardware (hair) particles, no need to save them to disk
#if MAYA_API_VERSION >= 650
      if ( renderType > MFnParticleSystem::kBlobby && 
	   renderType != MFnParticleSystem::kSprites )
	 return;
#else
      if ( renderType == MFnParticleSystem::kHardware )
	 return;
#endif

      DBG(name << ":  getData() -- write particle cache for volume");

      if ( renderType != MFnParticleSystem::kSprites )
	 volumetric = true;

      MStatus status; MPlug p;
      miPDCFile = "";
      GET_OPTIONAL( miPDCFile );
      if ( miPDCFile != "" )
      {
	 int last = (int) miPDCFile.length() - 1;
	 // If user placed a .pdc file, remove that extension
	 if ( last > 3 && miPDCFile.substring( last-3 , last ) == ".pdc" )
	    miPDCFile = miPDCFile.substring( 0, last-4 );
	 // If user placed a xxx.0250.pdc file, remove the numbers, too.
	 int period = miPDCFile.rindex('.');
	 if ( period != -1 )
	 {
	    const char* c = miPDCFile.asChar();
	    while (  c[last] >= '0' && c[last] <= '9' && last != period )
	       --last;
	    if ( last == period )
	       miPDCFile = miPDCFile.substring( 0, period-1 );
	 }
	 const char* froot = miPDCFile.asChar();
	 if ( froot[0] != '/' && froot[1] != ':' )
	    miPDCFile = partDir + sceneName + "/" + miPDCFile;
	 return;
      }

      
      miPDCFile  = partDir + sceneName + "/";
      checkOutputDirectory( miPDCFile, true );

      miPDCFile += dagPathToFilename( fn.partialPathName() );

//       MString partial = fn.partialPathName().asChar();
//       int idx = partial.rindex('|');
//       const char* partName = partial.asChar() + idx + 1;
//       miPDCFile += partName;

      MItDependencyNodes it( MFn::kDynGlobals );
      const MObject& dynObj = it.item();
      MFnDependencyNode fnDyn( dynObj );
      bool useParticleDiskCache = false;
      if ( dynObj != MObject::kNullObj )
      {
#define fn fnDyn
	 GET( useParticleDiskCache );
#undef fn
      }
      
      // We are already using a particle cache, no need to respit ours
      if ( useParticleDiskCache && fileExists( getPDCFile() ) ) return;

      miPDCFile += "-mray";
      
      // We will allow to spit out any arbitrary attribute into a PDC file in
      // the current project.  For doing so, the string attribute miAttributes
      // will contain a list of dynamic attibutes, separated by a space.
      // For example:
      //
      //  position radius lifespanPP
      //
      // The order of the attributes is relevant.
      //
      // if miAttributes is not present, we just spit out:
      //  position opacity rgb opacityPP rgbPP
      //
      //  + radius(es), tailSize depending on particle render type
      //  + velocity in case of motion blur.

      bool reportMissingAttr = true;
      
      MString miAttributes;
      GET_OPTIONAL( miAttributes );
      if ( miAttributes == "" )
      {
	 reportMissingAttr = false;
	 miAttributes = "position opacity rgb opacityPP rgbPP";

	 switch( renderType )
	 {
	    case MFnParticleSystem::kCloud:
	    case MFnParticleSystem::kBlobby:
	       miAttributes += " radius radiusPP";
	       break;
	    case MFnParticleSystem::kTube:
	       miAttributes += " radius0 radius0PP radius1 radius1PP "
	                       "tailSize tailSizePP rotationPP";
	       break;
	    case MFnParticleSystem::kSprites:
	       miAttributes  = "position spriteNum spriteNumPP "
	                       "spriteScaleX spriteScaleXPP "
	                       "spriteScaleY spriteScaleYPP "
                               "spriteTwist  spriteTwisPP";
	       break;
	    default:
	       break;
	 }

	 if ( options->motionBlur != mrOptions::kMotionBlurOff )
	    miAttributes += " velocity worldVelocity";
      }

      count      = fn.count();
      miAttributes.split(' ', attributes);
      
      unsigned numAttrs = attributes.length();
      
      // Make sure the position attribute is listed.
      bool hasPosition = false;
      for (unsigned i = 0; i < numAttrs; ++i )
      {
	 if ( attributes[i] == "position" )
	 {
	    hasPosition = true; break;
	 }
      }
      if ( !hasPosition )
	 attributes.append( "position" );

      // Verify all attributes do exist and are valid for a pdc file
      attributeTypes.setLength(numAttrs);

      // We scan attributes backwards so we can safely remove one
      // if needed
      for (int i = numAttrs-1; i >= 0; --i )
      {
	 p = fn.findPlug( attributes[i], &status );
	 if ( status == MS::kSuccess )
	 {
	    const MObject& attr = p.attribute();
	    attributeTypes[i] = -1;
	    switch( attr.apiType() )
	    {
	       case MFn::kNumericAttribute:
		  {
		     MFnNumericAttribute fnAttr( attr );
		     switch ( fnAttr.unitType() )
		     {
			case MFnNumericData::kInt:
			   attributeTypes[i] = 0;
			   break;
			case MFnNumericData::kFloat:
			case MFnNumericData::kDouble:
			   attributeTypes[i] = 2;
			   break;
			case MFnNumericData::k3Float:
			case MFnNumericData::k3Double:
			   attributeTypes[i] = 4;
			   break;
			default:
			   break;
		     }
		     break;
		  }
	       case MFn::kTypedAttribute:
		  {
		     MFnTypedAttribute fnAttr( attr );
		     MFnData::Type t = fnAttr.attrType();
		     switch( t )
		     {
			case MFnData::kDoubleArray:
			   attributeTypes[i] = 3;
			   break;
			case MFnData::kIntArray:
			   attributeTypes[i] = 1;
			   break;
			case MFnData::kVectorArray:
			   attributeTypes[i] = 5;
			   break;
			default:
			   break;
		     }
		     break;
		  }
	       default:
		  break;
	    }

	    if ( attributeTypes[i] == -1 )
	    {
	       MString err = name;
	       err += ": Attribute \"";
	       err += attributes[i];
	       err += "\" is not a valid type (";
	       err += attr.apiTypeStr();
	       err += ") for pdc file.  Not spit.";
	       LOG_ERROR(err);
	       attributes.remove(i);
	       attributeTypes.remove(i);
	    }
	    continue;
	 }

	 if ( reportMissingAttr )
	 {
	    MString err = name;
	    err += ": Attribute \"";
	    err += attributes[i];
	    err += "\" does not exist in particle shape.  Not spit.";
	    LOG_ERROR(err);
	 }

	 attributes.remove(i);
	 attributeTypes.remove(i);
      }

      writePDCFile();
   }
}




void mrParticles::writeParticleFile( MRL_FILE* f ) const
{
   DBG("write Particle file in maya2mr's format");
   MString pdcFile = getPDCFile();
   
   if ( options->exportVerbosity > 3 )
   {
      MString msg = "Saving particle cache file \"";
      msg += pdcFile;
      msg += "\".";
      LOG_MESSAGE(msg);
   }

   f = MRL_FOPEN( pdcFile.asChar(), "wb" );
   if (!f) { 
      MString err = name + ": Could not create particle cache file";
      LOG_ERROR(err);
      return;
   }

   MRL_PUTS("Maya");  // Magic number


   MPlug p; MStatus status;
   MFnDagNode fn( path );

   // we retrieve numAttrs again in case it was byteswapped
   int tmp;
   unsigned numAttrs = attributes.length();
   for ( unsigned i = 0; i < numAttrs; ++i )
   {
      int len = tmp = attributes[i].length();
      SAVE_INT(tmp);
      MRL_FWRITE( attributes[i].asChar(), sizeof(char), len, f );
      tmp = attributeTypes[i];
      SAVE_INT( tmp );
      p = fn.findPlug( attributes[i] );
      
      MObject o;
      switch( attributeTypes[i] )
      {
	 case 0:
	    {
	       int v;
	       p.getValue(v);
	       SAVE_INT(v);
	    }
	    break;
	 case 1:
	    {
	       p.getValue(o);
	       MFnIntArrayData fnAttr( o );
	       const MIntArray& v = fnAttr.array();
	       unsigned num = v.length();
	       assert( num == count );
	       for ( unsigned j = 0; j < num; ++j )
	       {
		  tmp = v[j];
		  SAVE_INT( tmp );
	       }
	    }
	    break;
	 case 2:
	    {
	       double v;
	       p.getValue(v);
	       SAVE_DOUBLE(v);
	    }
	    break;
	 case 3:
	    {
	       p.getValue(o);
	       MFnDoubleArrayData fnAttr( o );
	       const MDoubleArray& v = fnAttr.array();
	       unsigned num = v.length();
	       assert( num == count );
	       for ( unsigned j = 0; j < num; ++j )
	       {
		  double t = v[j];
		  SAVE_DOUBLE( t );
	       }
	    }
	    break;
	 case 4:
	    {
	       MVector v; 
	       p.getValue(o);
	       MFnNumericData fnAttr(o);
	       fnAttr.getData( v.x, v.y, v.z );
	       SAVE_DVECTOR( v );
	    }
	    break;
	 case 5:
	    {
	       p.getValue(o);
	       MFnVectorArrayData fnAttr( o );
	       const MVectorArray& v = fnAttr.array();
	       unsigned num = v.length();

	       for ( unsigned j = 0; j < num; ++j )
	       {
		  MVector t = v[j];
		  SAVE_DVECTOR( t );
	       }
	    }
	    break;
      }
   }
   if (f) MRL_FCLOSE(f);
}


void mrParticles::writePDCFile() const
{
   DBG("write PDC file");
   MString pdcFile = getPDCFile();
   
   if ( options->exportVerbosity > 3 )
   {
      MString msg = "Saving particle cache file \"";
      msg += pdcFile;
      msg += "\".";
      LOG_MESSAGE(msg);
   }

   MRL_FILE* f = MRL_FOPEN( pdcFile.asChar(), "wb" );
   if (!f) { 
      MString err = name + ": Could not create particle cache file";
      LOG_ERROR(err);
      return;
   }

   MRL_PUTS("PDC ");
   int pdcVersion = 1;
   SAVE_INT(pdcVersion);
   int endian = 1;
   SAVE_INT(endian);
   int tmp = 0;
   SAVE_INT(tmp);
   SAVE_INT(tmp);
   tmp = count;
   SAVE_INT(tmp);
   int numAttrs = attributes.length();
   SAVE_INT(numAttrs);

   MPlug p; MStatus status;
   MFnDagNode fn( path );

   // we retrieve numAttrs again in case it was byteswapped
   numAttrs = attributes.length();
   for ( int i = 0; i < numAttrs; ++i )
   {
      int len = tmp = attributes[i].length();
      SAVE_INT(tmp);
      MRL_FWRITE( attributes[i].asChar(), sizeof(char), len, f );
      tmp = attributeTypes[i];
      SAVE_INT( tmp );
      p = fn.findPlug( attributes[i] );
      
      MObject o;
      switch( attributeTypes[i] )
      {
	 case 0:
	    {
	       int v;
	       p.getValue(v);
	       SAVE_INT(v);
	    }
	    break;
	 case 1:
	    {
	       p.getValue(o);
	       MFnIntArrayData fnAttr( o );
	       const MIntArray& v = fnAttr.array();
	       unsigned num = v.length();
	       assert( num == count );
	       for ( unsigned j = 0; j < num; ++j )
	       {
		  tmp = v[j];
		  SAVE_INT( tmp );
	       }
	    }
	    break;
	 case 2:
	    {
	       double v;
	       p.getValue(v);
	       SAVE_DOUBLE(v);
	    }
	    break;
	 case 3:
	    {
	       p.getValue(o);
	       MFnDoubleArrayData fnAttr( o );
	       const MDoubleArray& v = fnAttr.array();
	       unsigned num = v.length();
	       assert( num == count );
	       for ( unsigned j = 0; j < num; ++j )
	       {
		  double t = v[j];
		  SAVE_DOUBLE( t );
	       }
	    }
	    break;
	 case 4:
	    {
	       MVector v; 
	       p.getValue(o);
	       MFnNumericData fnAttr(o);
	       fnAttr.getData( v.x, v.y, v.z );
	       SAVE_DVECTOR( v );
	    }
	    break;
	 case 5:
	    {
	       p.getValue(o);
	       MFnVectorArrayData fnAttr( o );
	       const MVectorArray& v = fnAttr.array();
	       unsigned num = v.length();

	       for ( unsigned j = 0; j < num; ++j )
	       {
		  MVector t = v[j];
		  SAVE_DVECTOR( t );
	       }
	    }
	    break;
      }
   }
   if (f) MRL_FCLOSE(f);
}

mrParticles::mrParticles( const MDagPath& shape ) :
mrObject( getMrayName( shape ) ),
pdcWritten( mrNode::kNotWritten )
{
   shapeAnimated = true;
   path = shape;
   getData();
}


mrParticles* mrParticles::factory( const MDagPath& shape )
{
   mrParticles* p;
   char written = mrNode::kNotWritten;
   mrShape* base = mrShape::find( shape );
   if ( base )
   { 
      p = dynamic_cast< mrParticles* >( base );
      if ( p ) return p;
      DBG("Warning:: redefined object type " << base->name);
      written = base->written;
      nodeList.remove( nodeList.find( base->name ) );
   }

   p = new mrParticles( shape );
   p->written = written;
   nodeList.insert( p );
   return p;
}


void mrParticles::write_volumetric_group( MRL_FILE* f )
{
   MStatus status;  MPlug p;
   MFnParticleSystem fn( path );

   MDoubleArray radius;
   MVectorArray v;
   fn.position( v );
      
   const MMatrix& m = path.inclusiveMatrixInverse();

   unsigned num = v.length();
   for ( unsigned i = 0; i < num; ++i )
   {
      MPoint p( v[i] );
      p *= m;
      v[i] = p;
   }


   switch( renderType )
   {
      case MFnParticleSystem::kBlobby:
      case MFnParticleSystem::kCloud:
	 fn.radius( radius );
	 break;
      case MFnParticleSystem::kTube:
	 {
	    MDoubleArray radius0;
	    MDoubleArray radius1;
	    fn.radius0( radius0 );
	    fn.radius1( radius1 );
	    unsigned num = radius0.length();
	    for ( unsigned i = 0; i < num; ++i )
	    {
	       if ( radius0[i] > radius1[i] ) radius.append( radius0[i] );
	       else radius.append( radius1[i] );
	    }
	    break;
	 }
      default:
	 renderType = MFnParticleSystem::kBlobby;
	 fn.radius( radius );
	 mrERROR("Unhandled particle type");
   }

   MVectorArray vel;
   if ( options->motionBlur != mrOptions::kMotionBlurOff )
   {
      GET_OPTIONAL_VECTOR_ARRAY_ATTR( vel, "velocity" );
      num = vel.length();

      double t = getFrameRate( MTime::uiUnit() );

      for ( unsigned i = 0; i < num; ++i )
      {
	 vel[i] /= t;
	 vel[i]  = vel[i] * m;
      }
   }
   
   num = v.length();
   if ( vel.length() < num ) vel.setLength( num );
   if ( radius.length() < num ) radius.setLength( num );

   float front = 0.5f;
   float back  = 0.5f;
   
   MBoundingBox b;
   double kEPS = 1e-5;
   for ( unsigned i = 0; i < num; ++i )
   {
      MPoint tmp1(
		  v[i].x + radius[i] + kEPS,
		  v[i].y + radius[i] + kEPS,
		  v[i].z + radius[i] + kEPS
		  );
      MPoint tmp2(
		  v[i].x - radius[i] - kEPS,
		  v[i].y - radius[i] - kEPS,
		  v[i].z - radius[i] - kEPS
		  );
      b.expand( tmp1 );
      b.expand( tmp2 );
      if ( options->motionBlur != mrOptions::kMotionBlurOff )
      {
	 MPoint tmp( tmp2 );
	 tmp -= vel[i] * back;
	 b.expand( tmp );
	 tmp  = tmp2;
	 tmp += vel[i] * front;
	 b.expand( tmp );

	 tmp  = tmp1;
	 tmp -= vel[i] * back;
	 b.expand( tmp );
	 tmp  = tmp1;
	 tmp += vel[i] * front;
	 b.expand( tmp );
      }
   }
   
   v.clear();
   MVector bmin = b.min();
   MVector bmax = b.max();

   v.append( MVector( bmax.x, bmax.y, bmax.z ) );
   v.append( MVector( bmin.x, bmax.y, bmax.z ) );
   v.append( MVector( bmax.x, bmin.y, bmax.z ) );
   v.append( MVector( bmin.x, bmin.y, bmax.z ) );
   v.append( MVector( bmax.x, bmax.y, bmin.z ) );
   v.append( MVector( bmin.x, bmax.y, bmin.z ) );
   v.append( MVector( bmax.x, bmin.y, bmin.z ) );
   v.append( MVector( bmin.x, bmin.y, bmin.z ) );

   TAB(2); COMMENT("# point vectors\n");
   write_vectors( f, v );

   TAB(2); COMMENT("# vertices\n");
   for ( short i = 0; i < 8; ++i )
   {
      TAB(2);
      MRL_FPRINTF(f, "v %d\n", i );
   }
   TAB(2); COMMENT("# polygons\n");
   TAB(2); MRL_PUTS("c 0 0 1 3 2\n");
   TAB(2); MRL_PUTS("c 0 0 1 5 4\n");
   TAB(2); MRL_PUTS("c 0 0 4 6 2\n");
   TAB(2); MRL_PUTS("c 0 1 5 7 3\n");
   TAB(2); MRL_PUTS("c 0 2 3 7 6\n");
   TAB(2); MRL_PUTS("c 0 5 4 6 7\n");
}


void mrParticles::write_hair_group( MRL_FILE* f )
{
   DBG(name << " write group");

   MStatus status;  MPlug p;
   MFnParticleSystem fn( path );

   RenderType particleRenderType = (RenderType) renderType;
#if MAYA_API_VERSION < 700
   GET( particleRenderType );
#endif

   MVectorArray v0;
   fn.position( v0 );
   unsigned numParts = v0.length();
   
   MDoubleArray id;
   GET_DOUBLE_ARRAY( id );

   MVectorArray velocity;
   GET_OPTIONAL_VECTOR_ARRAY( velocity );

   if (velocity.length() < numParts) {
      DBG(name << "  Empty velocity");
      velocity.setLength(numParts);
   }
   
   MVectorArray v1(numParts);
   double radius = 1.0;
   MDoubleArray radiusPP;

   
   double pointSize = 1;
   double tailSize = 1;
   GET_OPTIONAL( tailSize );
   tailSize *= 2.5;
   short lineWidth = 1;
   GET_OPTIONAL( lineWidth );
   
   MDoubleArray pointSizePP, tailSizePP, lineWidthPP;
   
   GET_OPTIONAL_DOUBLE_ARRAY( pointSizePP );
   GET_OPTIONAL_DOUBLE_ARRAY( tailSizePP );
   unsigned numTails = tailSizePP.length();
   for ( unsigned i = 0; i < numTails; ++i )
      tailSizePP[i] *= 2.5;
   GET_OPTIONAL_DOUBLE_ARRAY( lineWidthPP );   

   int multiCount = 1;
   if ( particleRenderType == kMultiPoint ||
	particleRenderType == kMultiStreak )
   {
     multiCount = 10;
     GET_OPTIONAL( multiCount );

     float multiRadius = 0.3f;
     GET_OPTIONAL( multiRadius );
      
     MVectorArray vorg     = v0;
     MVectorArray velorg   = velocity;
     MDoubleArray psizeorg = pointSizePP;
     MDoubleArray tailorg  = tailSizePP;
     MDoubleArray lineorg  = lineWidthPP;

     unsigned newNumParts = numParts * multiCount;
     v0.setLength( newNumParts );
     v1.setLength( newNumParts );
     velocity.setLength( newNumParts );

     bool hasPointSizePP = (pointSizePP.length() > 0);
     if ( hasPointSizePP )  pointSizePP.setLength( newNumParts );
      
     bool hasTailSizePP = (tailSizePP.length() > 0);
     if ( hasTailSizePP )  tailSizePP.setLength( newNumParts );
     
     bool hasLineWidthPP = (lineWidthPP.length() > 0);
     if ( hasLineWidthPP )  lineWidthPP.setLength( newNumParts );

      
     double ONE_OVER_RAND_MAX = 1.0 / (double) RAND_MAX;
     unsigned idx = 0;
     for ( unsigned i = 0; i < numParts; ++i )
       {
	 // Seed the random number generator using the particle ID
	 // (this ensures that the multi-points won't jump during animations,
	 //  and won't jump when other particles die)
	 //
	 srand( (int) id[i] );
	 for ( int j = 0; j < multiCount; ++j, ++idx )
	   {
	     double r1 = rand() * ONE_OVER_RAND_MAX - 0.5;
	     double r2 = rand() * ONE_OVER_RAND_MAX - 0.5;
	     double r3 = rand() * ONE_OVER_RAND_MAX - 0.5;
	     MPoint offset( multiRadius * r1, multiRadius * r2,
			    multiRadius * r3 );
	     v0[idx]  = vorg[i];
	     v0[idx] += offset;
	     velocity[idx] = velorg[i];
	     
	     if ( hasPointSizePP ) pointSizePP[idx] = psizeorg[i];
	     if ( hasTailSizePP )  tailSizePP[idx]  = tailorg[i];
	     if ( hasLineWidthPP ) lineWidthPP[idx] = lineorg[i];
	   }
       }
     numParts = newNumParts;
   }
   
   
   MDoubleArray spriteNumPP, spriteScaleXPP, spriteScaleYPP, spriteTwistPP;
   int spriteNum = 1;
   double spriteScaleX = 1.0;
   double spriteScaleY = 1.0;
   double spriteTwist  = 0.0;

   double miPixelSize = kPIXEL_SIZE;
   GET_OPTIONAL( miPixelSize );
   
   MVectorArray dir;
   if ( particleRenderType == kSprites )
   {
      GET_OPTIONAL( spriteNum );
      GET_OPTIONAL( spriteScaleX );
      GET_OPTIONAL( spriteScaleY );
      GET_OPTIONAL( spriteTwist );
   }
   else
   {
      dir = velocity;
      unsigned numDir = dir.length();
      for ( unsigned i = 0; i < numDir; ++i )
	 dir[i].normalize();
   }


   switch( particleRenderType )
   {
      case kMultiPoint:
      case kPoints:
      case kSprites:
      case kMultiStreak:
      case kStreak:
	 break;
      default:
	 MString err = name + ": particle render type ";
	 err += particleRenderType;
	 err += " is not supported.  Using points instead.";
	 LOG_ERROR(err);
	 particleRenderType = kPoints;
	 break;
   }

   
   switch( particleRenderType )
   {
      case kMultiPoint:
      case kPoints:
	 {
	    DBG(name << "   point/multipoint type");
	    if ( pointSizePP.length() == 0 )
	    {
	       GET_OPTIONAL( pointSize );
	       pointSize *= miPixelSize;
	       radius = pointSize;
	       for ( unsigned i = 0; i < numParts; ++i )
	       {
		  MVector tmpV = pointSize * 0.5 * velocity[i];
		  if ( tmpV.length() == 0 ) tmpV.y = 0.00001;
		  v1[i] = v0[i] + tmpV;
	       }
	    }
	    else
	    {
	       assert( numParts == pointSizePP.length() );
	       radiusPP.setSizeIncrement( numParts );
	       for ( unsigned i = 0; i < numParts; ++i )
	       {
		  MVector tmpV = ( pointSizePP[i] * miPixelSize *
				   0.5 * velocity[i] );
		  if ( tmpV.length() == 0 ) tmpV.y = 0.00001;
		  v1[i] = v0[i] + tmpV;
		  radiusPP.append( pointSizePP[i] );
	       }
	    }
	    
	    break;
	 }
      case kSprites:
	 {
	    DBG(name << "   sprites type");
	    GET_OPTIONAL_DOUBLE_ARRAY( spriteNumPP );  
	    GET_OPTIONAL_DOUBLE_ARRAY( spriteScaleXPP );  
	    GET_OPTIONAL_DOUBLE_ARRAY( spriteScaleYPP );  
	    GET_OPTIONAL_DOUBLE_ARRAY( spriteTwistPP );

	    lineWidthPP.clear();
	    miPixelSize = 1;
	    pointSize = 1 * spriteScaleX;
	    unsigned num;

	    num = spriteScaleXPP.length();
	    if ( num > 0 )
	    {
	       lineWidthPP.setLength( num );
	       for ( unsigned i = 0; i < num; ++i )
		  lineWidthPP[i] = spriteScaleXPP[i];
	    }

	    tailSize = -spriteScaleY;
	    num = spriteScaleYPP.length();
	    if ( num > 0 )
	    {
	       tailSizePP.setLength( num );
	       for ( unsigned i = 0; i < num; ++i )
		  tailSizePP[i] = -spriteScaleYPP[i];
	    }

	    bool hasSpriteTwistPP = (spriteTwistPP.length() > 0 );
	    dir.setLength( numParts );
	    for ( unsigned i = 0; i < numParts; ++i )
	    {
	       MVector d = MVector(0,1,0);  // all sprites pointing up.
	       double twist = spriteTwist;
	       if ( hasSpriteTwistPP ) twist = spriteTwistPP[i];
	       dir[i] = d.rotateBy( 0, 0, twist, MSpace::kObject );
	    }

	    // ...keep handling it as a streak particle...
	 }
      case kMultiStreak:
      case kStreak:
	 {
	    DBG(name << "   streak/multistreak type");
	    if ( tailSizePP.length() == 0 )
	    {
	       for ( unsigned i = 0; i < numParts; ++i )
	       {
		  MVector tmpV = tailSize * dir[i] * miPixelSize;
		  if ( tmpV.length() == 0 ) tmpV.y = 0.00001;
		  v1[i] = v0[i] + tmpV;
	       }
	    }
	    else
	    {
	       assert( numParts == tailSizePP.length() );
	       for ( unsigned i = 0; i < numParts; ++i )
	       {
		  MVector tmpV = tailSizePP[i] * dir[i] * miPixelSize;
		  if ( tmpV.length() == 0 ) tmpV.y = 0.00001;
		  v1[i] = v0[i] + tmpV;
	       }
	    }

	    if ( lineWidthPP.length() == 0 )
	    {
	       radius = (float)lineWidth * miPixelSize * spriteScaleX;
	    }
	    else
	    {
	       assert( numParts == lineWidthPP.length() );
	       radiusPP.setSizeIncrement( numParts );
	       for ( unsigned i = 0; i < numParts; ++i )
		  radiusPP[i] = lineWidthPP[i] * miPixelSize;
	    }
	    break;
	 }
      default:  // will never get here...
	 break;
   }

   unsigned long int numScalars = 6 * numParts;  // for 3*2 positions

   unsigned numRadius = radiusPP.length();
   if ( numRadius == 0 )
   {
      TAB(2); MRL_FPRINTF(f, "radius %g\n", radius );
   }
   else
   {
      numScalars += numRadius;
      TAB(2); MRL_FPRINTF(f, "hair radius\n" );
   }
   
   if ( options->motionBlur == mrOptions::kMotionBlurExact )
   {
      TAB(2); MRL_PUTS("hair m 1\n");
      numScalars += 3 * numParts;
   }

   numScalars += numParts;
   TAB(2); MRL_PUTS("hair u 1\n" );  // for particle id

   TAB(2); MRL_FPRINTF( f, "scalar [ %ld ]\n", numScalars );

   std::vector<unsigned long int>    offsets;
   MDoubleArray scalars;
   scalars.setSizeIncrement( numScalars );
   offsets.reserve( numParts + 1 );
   
   for (unsigned i = 0; i < numParts; ++i )
   {
      offsets.push_back( scalars.length() );
      if ( options->motionBlur == mrOptions::kMotionBlurExact )
      {
	 scalars.append( velocity[i].x );
	 scalars.append( velocity[i].y );
	 scalars.append( velocity[i].z );
      }
      if (numRadius) scalars.append( radiusPP[i] );

      scalars.append( id[i / multiCount] );
      
      scalars.append( v0[i].x );
      scalars.append( v0[i].y );
      scalars.append( v0[i].z );
      scalars.append( v1[i].x );
      scalars.append( v1[i].y );
      scalars.append( v1[i].z );
   }
   
   offsets.push_back( scalars.length() );
   
   assert( scalars.length() == numScalars );

//     write_hair_scalars( f, scalars );
   for (unsigned i = 0; i < numScalars; ++i )
   {
      TAB(2);
      MRL_FPRINTF( f, "%g\n", scalars[i]);
   }

//     write_hair_offsets( f, offsets );
   unsigned long int numHairs = (unsigned long int) offsets.size();
   TAB(2); MRL_FPRINTF( f, "hair [ %ld ]\n", numHairs);
   for (unsigned i = 0; i < numHairs; ++i )
   {
      TAB(2);
      MRL_FPRINTF( f, "%ld\n", offsets[i]);
   }

}


void mrParticles::write_user_data( MRL_FILE* f )
{
   if ( written == kWritten ) return;

   mrObject::write_user_data(f);

   if ( renderType != MFnParticleSystem::kSprites &&
	!volumetric ) return;

   if ( pdcWritten == kIncremental ) MRL_PUTS("incremental ");
   MRL_FPRINTF(f, "data \"%s:pdc\" \"%s\"\n",
	   name.asChar(), getPDCFile().asChar() );
   NEWLINE();
   pdcWritten = kWritten;
}


void mrParticles::write_properties( MRL_FILE* f )
{
   mrObject::write_properties(f);
   if ( volumetric )
   {
      TAB(1);
      MRL_FPRINTF(f, "data \"%s:pdc\"\n", name.asChar() );
   }
}

void mrParticles::write_hair_object_definition( MRL_FILE* f )
{
   TAB(1); MRL_PUTS( "hair\n" );
   TAB(2); MRL_PUTS("approximate 1\n" );
   TAB(2); MRL_PUTS("degree 1\n" );
   write_hair_group(f);
   TAB(1); MRL_PUTS( "end hair\n");
}

void mrParticles::write_object_definition( MRL_FILE* f )
{
   if ( !volumetric ) 
   {
      write_hair_object_definition(f);
   }
   else
   {
      TAB(1); MRL_PUTS( "group\n" );
      write_volumetric_group(f);
      TAB(1); MRL_PUTS( "end group\n");
   }
}


void mrParticles::write_group( MRL_FILE* f )
{
}


void mrParticles::forceIncremental()
{
   DBG(name << ": mrParticles::forceIncremental()");
   mrObject::forceIncremental();
   getData();
   if ( pdcWritten == kWritten ) pdcWritten = kIncremental;
}


void mrParticles::setIncremental( bool sameFrame )
{
   DBG(name << ": mrParticles::setIncremental()");
   mrObject::setIncremental( sameFrame );
   if ( !sameFrame )
   {
      getData();
      if ( pdcWritten == kWritten ) pdcWritten = kIncremental;
   }
}

#ifdef GEOSHADER_H
#include "raylib/mrParticles.inl"
#endif
