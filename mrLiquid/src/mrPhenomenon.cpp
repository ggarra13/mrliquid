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

#include <ctime>

#include "maya/MPlug.h"
#include "maya/MPlugArray.h"
#include "maya/MFnDependencyNode.h"
#include "maya/MFnNumericAttribute.h"
#include "maya/MFnNumericData.h"
#include "maya/MFnEnumAttribute.h"
#include "maya/MDistance.h"
#include "maya/MAngle.h"
#include "maya/MTime.h"
#include "maya/MFileIO.h"

#include "mrVersion.h"
#include "mrOptions.h"
#include "mrPhenomenon.h"
#include "mrShadingGroup.h"
#include "mrShaderFactory.h"
#include "mrCustomText.h"
#include "mrAttrAux.h"

#define START_ATTR 5

mrPhenomenon::mrPhenomenon(const MFnDependencyNode& p) :
mrShader(p)
{
}


mrPhenomenon::~mrPhenomenon()
{
}



void mrPhenomenon::write_default_value( MRL_FILE* f, const MPlug& p )
{
   if ( p.isArray() ) return;

   const MString& attrName = p.partialName( false, false, false,
					    false, false, true );

   if ( p.isCompound() )
   {
      unsigned numChildren = p.numChildren();
      if ( numChildren < 4 )
      {
	 MRL_PUTS("\t#: default");
	 if ( attrName == "normalCamera" ||
	      attrName == "miNormalCamera" )
	 {
	    MRL_PUTS(" 1 1 1");
	    return;
	 }

	 for ( unsigned i = 0; i < numChildren; ++i )
	 {
	    MPlug cp = p.child(i);
	    double val;
	    cp.getValue(val);
	    MRL_FPRINTF(f, " %g", val);
	 }
	 if ( numChildren == 2 ) MRL_PUTS(" 0");
      }
   }
   else
   {
      const MObject& attrObj = p.attribute();
      if ( attrObj.hasFn( MFn::kEnumAttribute ) )
      {
	 short val;
	 p.getValue(val);
	 MRL_FPRINTF( f, "\t#: default %d enum \"", val);

	 MFnEnumAttribute fn( p.attribute() );
	 short	   iMin, iMax;
	 MString	fName;
	 fn.getMin( iMin );
	 fn.getMax( iMax );

	 for ( short j = iMin; j <= iMax; ++j )
	 {
	    if ( j > iMin ) MRL_PUTC(':');
	    MString fName = fn.fieldName( j );
	    MRL_FPRINTF( f, "%s=%d", fName.asChar(), j );
	 }
	 MRL_PUTC('"');
      }
      else if ( attrObj.hasFn( MFn::kNumericAttribute ) )
      {
	 double val;
	 p.getValue(val);
	 MRL_FPRINTF( f, "\t#: default %g", val);

	 MFnNumericAttribute fn( p.attribute() );
	 double dMin = 0.0, dMax = 0.0, dSoftMin, dSoftMax;
	 if ( fn.hasMin() )
	 {
	    fn.getMin( dMin );
	    MRL_FPRINTF( f, " min %g", dMin);
	 }
	 if ( fn.hasMax() )
	 {
	    fn.getMax( dMax );
	    MRL_FPRINTF( f, " max %g", dMax );
	 }
	 if ( fn.hasSoftMin() )
	 {
	    fn.getSoftMin( dSoftMin );
	    if ( dMin != dSoftMin )
	    {
	       MRL_FPRINTF( f, " softmin %g", dSoftMin);
	    }
	 }
	 if ( fn.hasSoftMax() )
	 {
	    fn.getSoftMax( dSoftMax );
	    if ( dMax != dSoftMax )
	    {
	       MRL_FPRINTF( f, " softmax %g", dSoftMax );
	    }
	 }
      }
      else if ( attrObj.apiType() == MFn::kDoubleLinearAttribute ||
		attrObj.apiType() == MFn::kFloatLinearAttribute )
      {
	 MDistance dst;
	 p.getValue(dst);
	 MRL_FPRINTF( f, "\t#: default %g", dst.as( MDistance::uiUnit() ));
      }
      else if (	attrObj.apiType() == MFn::kDoubleAngleAttribute ||
		attrObj.apiType() == MFn::kFloatAngleAttribute )
      {
	 MAngle ang;
	 p.getValue(ang);
	 MRL_FPRINTF( f, "\t#: default %g", ang.as( MAngle::uiUnit() ));
      }
      else if (	attrObj.apiType() == MFn::kTimeAttribute )
      {
	 MTime time;
	 p.getValue(time);
	 MRL_FPRINTF( f, "\t#: default %g", time.as( MTime::uiUnit() ));
      }
   }
}

bool mrPhenomenon::write_output_material( MRL_FILE* f, const MPlug& cp )
{
   MPlugArray plugs;
   cp.connectedTo( plugs, true, false );
   if ( plugs.length() == 0 )
      return false;

   const MObject& sgObj = plugs[0].node();
   if ( !sgObj.hasFn( MFn::kShadingEngine ) )
   {
      MString err = name;
      err += ": rootMaterial plug not connected to a shading group.";
      LOG_ERROR( err );
      return false;
   }

   MRL_PUTS("material ");
   return true;
}


void mrPhenomenon::write_output( MRL_FILE* f, const MFnDependencyNode& dep )
{
   MPlug p = dep.findPlug( "rootMaterial", true );
   if ( write_output_material( f, p ) )
      return;

   p = dep.findPlug( "root", true );
   unsigned num = p.numConnectedElements();
   if ( num == 0 ) 
   {
      MString err = name;
      err += ": no root node found.";
      LOG_ERROR(err);
      return;
   }

   MPlug cp = p.connectionByPhysicalIndex( 0 );
   MPlugArray plugs;
   cp.connectedTo( plugs, true, false );
   if ( plugs.length() == 0 )
   {
      MString err = name;
      err += ": root node is not connected.";
      LOG_ERROR(err);
      return;
   }

   const MObject& shaderObj = plugs[0].node();
   MFnDependencyNode fn( shaderObj );

   // Count the number of out attributes
   typedef  std::vector< unsigned > mrAttrIndices;
   mrAttrIndices indices;

   unsigned inc;
   unsigned numAttrs = fn.attributeCount();
   unsigned i;
   for ( i = START_ATTR; i < numAttrs; i += inc )
   {
      const MObject& attrObj = fn.attribute(i);
      MPlug ap( shaderObj, attrObj );

      inc = 1;
      if ( ap.isCompound() ) inc += ap.numChildren();

      
      const MObject& attr = ap.attribute();
      MFnAttribute fnAttr(attr);
      if ( fnAttr.isHidden() ) {
	 DBG( name << "  hidden attribute: " << ap.info() );
	 continue;
      }

      if ( fnAttr.isReadable() && (!fnAttr.isWritable()) )
      {
	 const MString& attrName = ap.partialName( false, true, true,
						   false, true, true );
	 int len = attrName.length();
	 if ( len > 4 )
	 {
	    const char* attrChar = attrName.asChar();
	    if ( strcmp( attrChar + len - 4, "_str" ) == 0 )
	       continue; // tex_str, lightmap_str, etc.
	 }
	 if ( attrName != "miFactoryNode" &&
	      attrName != "miForwardDefinition" )
	 {
	    DBG( name << "  out attribute: " << attrName );
	    indices.push_back( i );
	 }
      }
   }

   unsigned numIndices = (unsigned) indices.size();
   if ( numIndices == 0 )
   {
      MString err = name;
      err += ": no outputs found";
      LOG_ERROR(err);
      return;
   }

   if ( numIndices == 1 )
   {
      const MObject& attrObj = fn.attribute( indices[0] );
      MPlug p( shaderObj, attrObj );
      write_type( f, p );
   }
   else
   {
      TAB(1); MRL_PUTS( "struct {" );
      for ( i = 0; i < numIndices; ++i )
      {
	 const MObject& attrObj = fn.attribute( indices[i] );
	 MPlug p( shaderObj, attrObj );
	 if ( i > 0 ) MRL_PUTC(',');
	 MRL_PUTC('\n');
	 write_interface_parameter( f, p, false );
      }
      MRL_PUTC('\n');
      TAB(1); MRL_PUTC( '}' );
   }
   MRL_PUTC(' ');
}


void mrPhenomenon::write_type( MRL_FILE* f, const MPlug& p )
{
   const MString& attrName = p.partialName( false, false, false,
					    false, false, true );
   TAB(2);
   if ( p.isArray() )
   {
      MRL_PUTS("array ");
   }
   
   if ( p.isCompound() )
   {
      MPlug cp = p.child(0);
      char comp = getComponent( cp );
      if ( comp == 'R' )
      {
	 MRL_PUTS("color");
      }
      else
      {
	 MRL_PUTS("vector");
      }
   }
   else
   {
      const MObject& attrObj = p.attribute();
      if ( attrObj.hasFn( MFn::kMessageAttribute ) )
      {
	 if ( attrName == "lights" || attrName == "light" )
	 {
	    MRL_PUTS("light");
	 }
	 else if ( attrName == "geometry" || attrName == "object" ||
		   attrName == "objects"  || attrName == "geometries" )
	 {
	    MRL_PUTS("geometry");
	 }
	 else if ( attrName == "shader" || attrName == "shaders" ||
		   attrName == "remap" )
	 {
	    MRL_PUTS("shader");
	 }
	 else
	 {
	    MRL_PUTS("color texture");
	 }
      }
      else if ( attrObj.hasFn( MFn::kEnumAttribute ) )
      {
	 MRL_PUTS("integer");
      }
      else if ( attrObj.hasFn( MFn::kNumericAttribute ) )
      {
	 MFnNumericAttribute n( attrObj );
	    
	 switch( n.unitType() )
	 {
	    case MFnNumericData::kBoolean:
	       {
		  MRL_PUTS("boolean");
		  break;
	       }
	    case MFnNumericData::kShort:
	    case MFnNumericData::kInt:
	       {
		  MRL_PUTS("integer");
		  break;
	       }
	    case MFnNumericData::kFloat:
	    case MFnNumericData::kDouble:
	       {
		  MRL_PUTS("scalar");
		  break;
	       }
	    default:
	       mrTHROW("Unhandled numeric type");
	 }
      }
      else if ( attrObj.hasFn( MFn::kTypedAttribute ) )
      {
	 MRL_PUTS("string");
      }
      else if ( attrObj.hasFn( MFn::kMatrixAttribute ) ||
		attrObj.hasFn( MFn::kFloatMatrixAttribute ) )
      {
	 MRL_PUTS("matrix");
      }
      else if ( attrObj.apiType() == MFn::kDoubleLinearAttribute ||
		attrObj.apiType() == MFn::kFloatLinearAttribute  ||
		attrObj.apiType() == MFn::kDoubleAngleAttribute ||
		attrObj.apiType() == MFn::kFloatAngleAttribute ||
		attrObj.apiType() == MFn::kTimeAttribute )
      {
	 MRL_PUTS("scalar");
      }
      else if ( attrObj.hasFn( MFn::kGenericAttribute ) )
      {
	 MRL_PUTS("string");
      }
      else
      {
	 MString err = "Unknown parameter type ";
	 err += attrName;
	 mrTHROW(err);
      }
   }
}

void mrPhenomenon::write_interface_parameter( MRL_FILE* f, const MPlug& p,
					      bool nodeName )
{
   write_type( f, p );
   MString attrName = p.partialName( nodeName, false, false,
				     false, false, true );
   char* dummy = STRDUP( attrName.asChar() );
   char* s = dummy;
   for ( ; *s != 0; ++s )
   {
      if ( *s == '[' || *s == ']' || *s == '.' ) *s = '_';
   }

   MRL_FPRINTF(f," \"%s\"", dummy );
   free( dummy );
}



void mrPhenomenon::write_interface( MRL_FILE* f, const MFnDependencyNode& dep )
{
   TAB(1); MRL_FPRINTF(f, "\"%s\" (", name.asChar() );
   MPlug p = dep.findPlug( "interface", true );
   unsigned numConnectedElements = p.numConnectedElements();
   MPlugArray plugs;
   for ( unsigned i = 0; i < numConnectedElements; ++i )
   {
      MPlug cp = p.connectionByPhysicalIndex( i );
      cp.connectedTo( plugs, false, true );
      unsigned numDest = plugs.length();
      if ( numDest == 0 ) continue;
      for ( unsigned j = 0; j < numDest; ++j )
      {
	 const MObject& attrObj = plugs[j].attribute();
	 if ( attrObj.hasFn( MFn::kMatrixAttribute ) ||
	      attrObj.hasFn( MFn::kFloatMatrixAttribute ) )
	 {
	    MString err = name;
	    err += ": cannot write matrix attribute ";
	    err += plugs[j].info();
	    err += " as interface parameter.";
	    LOG_ERROR(err);
	    continue;
	 }

	 MRL_PUTC('\n');
	 write_interface_parameter( f, plugs[j], true );
	 if ( (j != numDest-1) || (i != numConnectedElements-1) ) MRL_PUTC(',');
	 write_default_value( f, plugs[j] );
      }
   }
   MRL_PUTC('\n');
   TAB(1); MRL_PUTS( ")\n" );
   NEWLINE();
}


void mrPhenomenon::write_one_root( MRL_FILE* f, const char* name,
				   const MPlug& p
				   )
{
   MPlugArray plugs;

   if ( p.isArray() )
   {
      unsigned i = p.numConnectedElements();
      if ( i > 0 )
      {
	 TAB(1);
	 MRL_FPRINTF( f, "%s\n", name );
      }

      while ( i > 0 )
      {
	 --i;
	 MPlug cp = p.connectionByPhysicalIndex( i );
	 cp.connectedTo( plugs, true, false );
	 unsigned num = plugs.length();
	 if ( num == 0 ) continue;

	 for ( unsigned j = 0; j < num; ++j )
	 {
	    const MObject& node = plugs[j].node();
	    MFnDependencyNode fn( node );
	    if ( node.hasFn( MFn::kShadingEngine ) )
	    {
	       TAB(2); MRL_FPRINTF( f, " \"%s\"\n", fn.name().asChar() );
	    }
	    else
	    {
	       TAB(2);
	       MRL_FPRINTF( f, "= \"%s\"\n", fn.name().asChar() );
	    }
	 }
      }
   }
   else
   {
      p.connectedTo( plugs, true, false );
      unsigned num = plugs.length();
      if ( num == 0 ) return;

      TAB(1);
      MRL_FPRINTF( f, "%s\n", name );
      for ( unsigned j = 0; j < num; ++j )
      {
	 const MObject& node = plugs[j].node();
	 MFnDependencyNode fn( node );
	 if ( node.hasFn( MFn::kShadingEngine ) )
	 {
	    TAB(2); MRL_FPRINTF( f, " \"%s\"\n", fn.name().asChar() );
	 }
	 else
	 {
	    MRL_FPRINTF( f, "= \"%s\"\n", fn.name().asChar() );
	 }
      }
   }
}

void mrPhenomenon::write_shader_network( MRL_FILE* f, const MPlug& p )
{
   MPlugArray plugs;
   if ( p.isArray() )
   {
      unsigned num = p.numConnectedElements();
      unsigned i = num;
      while ( i > 0 )
      {
	 --i;
	 MPlug cp = p.connectionByPhysicalIndex( i );
	 cp.connectedTo( plugs, true, false );

	 unsigned num = plugs.length();
	 if ( num == 0 ) continue;

	 for ( unsigned j = 0; j < num; ++j )
	 {
	    const MObject& node = plugs[j].node();
	    if ( node.hasFn( MFn::kShadingEngine ) )
	    {
	       MFnDependencyNode fn( node );
	       mrShadingGroup* sg = mrShadingGroup::factory( fn );
	       if (sg) sg->write( f );
	    }
	    else
	    {
	       mrShader* s = mrShader::factory( node );
	       if (s) s->write( f );
	    }
	 }
      };
   }
   else
   {
      p.connectedTo( plugs, true, false );
      unsigned num = plugs.length();
      for ( unsigned j = 0; j < num; ++j )
      {
	 const MObject& node = plugs[j].node();
	 if ( node.hasFn( MFn::kShadingEngine ) )
	 {
	    MFnDependencyNode fn( node );
	    mrShadingGroup* sg = mrShadingGroup::factory( fn );
	    if (sg) sg->write( f );
	 }
	 else
	 {
	    mrShader* s = mrShader::factory( node );
	    if (s) s->write( f );
	 }
      }
   }
}


void mrPhenomenon::write_shaders( MRL_FILE* f, const MFnDependencyNode& dep )
{
   MStatus status;
   MPlug p;

#define WRITE_SHADERS(x) \
   p = dep.findPlug( #x, true, &status ); \
   if ( status == MS::kSuccess ) write_shader_network( f, p );

   WRITE_SHADERS( root );
   WRITE_SHADERS( rootLight );
   WRITE_SHADERS( rootMaterial );
   WRITE_SHADERS( rootLens );
   WRITE_SHADERS( rootVolume );
   WRITE_SHADERS( rootEnvironment );
   WRITE_SHADERS( rootGeometry );
   WRITE_SHADERS( rootOutput );
   WRITE_SHADERS( rootLightMap );
   WRITE_SHADERS( rootContour );

#undef WRITE_SHADERS

}

void mrPhenomenon::write_roots( MRL_FILE* f, const MFnDependencyNode& fn )
{
   MStatus status;
   MPlug p;
   MPlugArray plugs;

#define WRITE_ROOT( plug, name ) \
   p = fn.findPlug( #plug, true, &status ); \
   if ( status == MS::kSuccess ) write_one_root( f, name, p );

   WRITE_ROOT( root, "root" );
   WRITE_ROOT( rootLight, "light" );
   WRITE_ROOT( rootMaterial, "root material" );
   WRITE_ROOT( rootLens, "lens" );
   WRITE_ROOT( rootVolume, "volume" );
   WRITE_ROOT( rootEnvironment, "environment" );
   WRITE_ROOT( rootGeometry, "geometry" );
   WRITE_ROOT( rootOutput, "output" );
   WRITE_ROOT( rootLightMap, "lightmap" );
   WRITE_ROOT( rootContourStore, "contour store" );
   WRITE_ROOT( rootContourContrast, "contour contrast" );

#undef WRITE_ROOT

   MRL_PUTS("\n\n");
   TAB(1); MRL_PUTS( "version 1\n" );


#define HAS_ROOT( plug ) \
   hasRoot = false; \
   p = fn.findPlug( #plug, true, &status ); \
   if ( status == MS::kSuccess ) { \
     if ( p.isArray() ) \
     { \
        hasRoot = ( p.numConnectedElements() > 0 ); \
     } \
     else \
     { \
        p.connectedTo( plugs, true, false ); \
        hasRoot = ( plugs.length() > 0 ); \
     } \
   }

   bool hasRoot;
   HAS_ROOT( rootMaterial );

   if ( !hasRoot )
     {
       MString apply = "apply ";

       bool comma = false;
       HAS_ROOT( root );
       if ( hasRoot )
	 {
	   apply += "texture, material, photon, lightmap";
	   comma = true;
	 }

       HAS_ROOT( rootLens );
       if ( hasRoot )
	 {
	   if ( comma ) apply += ", ";
	   apply += "lens";
	   comma = true;
	 }

       HAS_ROOT( rootVolume );
       if ( hasRoot )
	 {
	   if ( comma ) apply += ", ";
	   apply += "volume";
	   comma = true;
	 }

       HAS_ROOT( rootEnvironment );
       if ( hasRoot )
	 {
	   if ( comma ) apply += ", ";
	   apply += "environment";
	   comma = true;
	 }

       HAS_ROOT( rootOutput );
       if ( hasRoot )
	 {
	   if ( comma ) apply += ", ";
	   apply += "output";
	   comma = true;
	 }

       HAS_ROOT( rootGeometry );
       if ( hasRoot )
	 {
	   if ( comma ) apply += ", ";
	   apply += "geometry";
	   comma = true;
	 }

       HAS_ROOT( rootLight );
       if ( hasRoot )
	 {
	   if ( comma ) apply += ", ";
	   apply += "light";
	   comma = true;
	 }

       HAS_ROOT( rootLightMap );
       if ( hasRoot )
	 {
	   if ( comma ) apply += ", ";
	   apply += "lightmap";
	   comma = true;
	 }

       HAS_ROOT( rootContour );
       if ( hasRoot )
	 {
	   if ( comma ) apply += ", ";
	   // apply += "contour";
	   comma = true;
	 }

       TAB(1); MRL_PUTS( apply.asChar() );
     }

#undef HAS_ROOT


   MRL_PUTS("\n");

   short reqDerivative;
   GET( reqDerivative );
   switch( reqDerivative )
   {
      case 3:
	 MRL_PUTS( "derivative 1 2\n"); break;
      case 2:
      case 1:
	 MRL_FPRINTF( f, "derivative %d\n", reqDerivative );
	 break;
      case 0:
      default:
	 break;
   }

   short reqFace;
   GET( reqFace );
   switch( reqFace )
   {
      case 3:
	 MRL_PUTS( "face both\n"); break;
      case 2:
	 MRL_PUTS( "face back\n"); break;
      case 1:
	 MRL_PUTS( "face front\n"); break;
	 break;
      case 0:
      default:
	 break;
   }

   short reqScanline;
   GET( reqScanline );
   switch( reqScanline )
   {
      case 2:
	 MRL_PUTS( "scanline on\n"); break;
      case 1:
	 MRL_PUTS( "scanline off\n"); break;
      case 0:
      default:
	 break;
   }

   short reqShadow;
   GET( reqShadow );
   switch( reqShadow )
   {
      case 4:
	 MRL_PUTS( "shadow segments\n"); break;
      case 3:
	 MRL_PUTS( "shadow sort\n"); break;
      case 2:
	 MRL_PUTS( "shadow on\n"); break;
      case 1:
	 MRL_PUTS( "shadow off\n"); break;
      case 0:
      default:
	 break;
   }

   short reqSpace;
   GET( reqSpace );
   switch( reqSpace )
   {
      case 3:
	 MRL_PUTS( "space mixed\n"); break;
      case 2:
	 MRL_PUTS( "space object\n"); break;
      case 1:
	 MRL_PUTS( "space camera\n"); break;
      case 0:
      default:
	 break;
   }

   short reqTrace;
   GET( reqTrace );
   switch( reqTrace )
   {
      case 2:
	 MRL_PUTS( "trace on\n"); break;
      case 1:
	 MRL_PUTS( "trace off\n"); break;
      case 0:
      default:
	 break;
   }

   MRL_PUTS("\n\n");
   TAB(1); MRL_PUTS( "#\n" );
   unsigned nodeid = mrShaderFactory::newShaderId();
   TAB(1);
   MRL_FPRINTF( f, "#: nodeid %u\n", nodeid);
   TAB(1); MRL_PUTS( "#\n" );
}

void mrPhenomenon::write_footer( MRL_FILE* f )
{
   const char* n = name.asChar();
   MRL_FPRINTF( f, "\n\n$endif # %s\n", n );
}

void mrPhenomenon::write_header( MRL_FILE* f )
{
#if defined(WIN32) || defined(WIN64)
   const char* username = getenv("USERNAME");
#else
   const char* username = getenv("USER");
#endif
   MString fullName = MFileIO::currentFile();
   // move backwards across the string until we hit a directory / and
   // take the info from there on
   int i = fullName.rindex( '/' );
   MString projectDir = fullName.substring(0, i);
   fullName = fullName.substring(i+1, fullName.length()-1);

   time_t t = time(NULL);
   MRL_FPRINTF(f,
	   "# mrLiquid %s %s (%s)\n"
	   "#\n"
	   "#  Auto-Generated mental ray Phenomenon\n"
	   "#\n"
	   "#      Project: %s\n"
	   "#        Scene: %s\n"
	   "#         User: %s\n"
	   "#      Created: %s"
	   "#  Description: *** Please document me ***\n"
	   "#\n\n",
	   MRL_VERSION, MRL_RELEASE_DATE, PLUGIN_DEBUG,
	   projectDir.asChar(), fullName.asChar(), 
	   username, ctime( &t )
	   );

   const char* n = name.asChar();
   MRL_FPRINTF( f, "$ifndef \"%s\"\nset \"%s\" \"t\"\n\n", n, n );
}

void mrPhenomenon::write_link_lines( MRL_FILE* f, const MPlug& p )
{
   MPlugArray plugs;
   if ( p.isArray() )
   {
      unsigned num = p.numConnectedElements();
      unsigned i = num;
      while ( i > 0 )
      {
	 --i;
	 MPlug cp = p.connectionByPhysicalIndex( i );
	 cp.connectedTo( plugs, true, false );

	 unsigned num = plugs.length();
	 if ( num == 0 ) continue;

	 for ( unsigned j = 0; j < num; ++j )
	 {
	    const MObject& node = plugs[j].node();
	    if ( node.hasFn( MFn::kShadingEngine ) )
	       continue;

	    mrShader* s = mrShader::factory( node );
	    if (s) s->write_link_lines( f );
	 }
      };
   }
   else
   {
      p.connectedTo( plugs, true, false );
      unsigned num = plugs.length();
      for ( unsigned j = 0; j < num; ++j )
      {
	 const MObject& node = plugs[j].node();
	 if ( node.hasFn( MFn::kShadingEngine ) )
	    continue;
	 mrShader* s = mrShader::factory( node );
	 if (s) s->write_link_lines( f );
      }
   }
}


void mrPhenomenon::write_links( MRL_FILE* f, const MFnDependencyNode& dep )
{
  MRL_FPRINTF( f, "# Namespace is needed so any textures spit will be found inside phenomenon.\n");
  MRL_FPRINTF( f, "# This is kind of a hack around a mray limitation.\n" );

  MRL_FPRINTF( f, "\nnamespace \"%s\"\n\n", name.asChar() );

   MStatus status;
   MPlug p;
#define WRITE_LINKS(x) \
   p = dep.findPlug( #x, true, &status ); \
   if ( status == MS::kSuccess ) write_link_lines( f, p );

   WRITE_LINKS( root );
   WRITE_LINKS( rootLight );
   WRITE_LINKS( rootMaterial );
   WRITE_LINKS( rootLens );
   WRITE_LINKS( rootVolume );
   WRITE_LINKS( rootEnvironment );
   WRITE_LINKS( rootGeometry );
   WRITE_LINKS( rootOutput );
   WRITE_LINKS( rootLightMap );
   WRITE_LINKS( rootContour );
#undef WRITE_LINKS

   MRL_PUTS("\nend namespace\n\n");
}

void mrPhenomenon::write_ae_header( MRL_FILE* f )
{
#if defined(WIN32) || defined(WIN64)
   const char* username = getenv("USERNAME");
#else
   const char* username = getenv("USER");
#endif
   MString fullName = MFileIO::currentFile();
   // move backwards across the string until we hit a directory / and
   // take the info from there on
   int i = fullName.rindex( '/' );
   MString projectDir = fullName.substring(0, i);
   fullName = fullName.substring(i+1, fullName.length()-1);

   time_t t = time(NULL);
   MRL_FPRINTF(f,
	   "// mrLiquid %s %s (%s)\n"
	   "//\n"
	   "//  Auto-Generated mental ray Phenomenon\n"
	   "//\n"
	   "//      Project: %s\n"
	   "//        Scene: %s\n"
	   "//         User: %s\n"
	   "//      Created: %s"
	   "//  Description: *** Please document me ***\n"
	   "//\n\n",
	   MRL_VERSION, MRL_RELEASE_DATE, PLUGIN_DEBUG,
	   projectDir.asChar(), fullName.asChar(), 
	   username, ctime( &t )
	   );

   MRL_PUTS("source \"gg_mray_aux.mel\";\n\n");

   MRL_FPRINTF( f, "global proc AE%sTemplate( string $node )\n{\n\n",
	    name.asChar() );

   MRL_PUTS("   AEswatchDisplay $node;\n"
	"   editorTemplate -beginScrollLayout;\n\n");
}

void mrPhenomenon::write_ae_parameter( MRL_FILE* f, const MPlug& p )
{
   MString attrName = p.partialName( true, false, false,
				     false, false, true );
   char* control = STRDUP( attrName.asChar() );
   char* d = control;
   for ( ; *d != 0; ++d )
   {
      if ( *d == '[' || *d == ']' || *d == '.' ) *d = '_';
   }

   attrName = p.partialName( false, false, false,
			     false, false, true );
   char label[256];
   const char* s = attrName.asChar();
   d = label;
   for ( ; *s != 0; ++s )
   {
      char c = *s;
      if ( c == '[' || c == ']' || c == '.' || c == '_' ) c = ' ';
      if ( c >= 'A' && c <= 'Z' && (*(d-1)) > 'Z' ) *d++ = ' ';

      if ( c >= 'a' && c <= 'z' && ( d == label || (*(d-1)) == ' ' ) )
	 c = c - 32;

      *d++ = c;
   }
   *d = 0; // NULL terminate string

   MRL_FPRINTF(f,"         editorTemplate -label \"%s\"\n"
	     "                   -addControl \"%s\";\n", label, control );
   free( control );
}

void mrPhenomenon::write_ae_footer( MRL_FILE* f )
{
   MRL_PUTS("      AEdependNodeTemplate $node;\n\n"
	"      editorTemplate -addExtraControls;\n\n"
	"   editorTemplate -endScrollLayout;\n\n" 
	"\n}\n\n");
}

void mrPhenomenon::write_ae_body( MRL_FILE* f )
{
   MStatus status;
   MFnDependencyNode dep( nodeHandle.objectRef() );
   MPlug p = dep.findPlug( "interface", true );
   unsigned numConnectedElements = p.numConnectedElements();
   if ( numConnectedElements == 0 ) return;

   MPlugArray plugs;

   MString node = "";
   for ( unsigned i = 0; i < numConnectedElements; ++i )
   {
      MPlug cp = p.connectionByPhysicalIndex( i );
      cp.connectedTo( plugs, false, true );
      unsigned numDest = plugs.length();
      if ( numDest == 0 ) continue;
      for ( unsigned j = 0; j < numDest; ++j )
      {
	 const MObject& attrObj = plugs[j].attribute();
	 if ( attrObj.hasFn( MFn::kMatrixAttribute ) ||
	      attrObj.hasFn( MFn::kFloatMatrixAttribute ) )
	    continue;

	 MFnDependencyNode fn( plugs[j].node() );
	 MString name = fn.name();
	 if ( name != node )
	 {
	    if ( node != "" )
	    {
	       MRL_PUTS("      editorTemplate -endLayout;\n\n");
	    }

	    node = name;
	    MRL_FPRINTF( f, 
		     "      editorTemplate -beginLayout \"%s\" -collapse 1;\n",
		     name.asChar() );
	 }

	 write_ae_parameter( f, plugs[j] );
      }
   }
   MRL_PUTS("      editorTemplate -endLayout;\n\n");
}

void mrPhenomenon::write_ae_template()
{
   char filename[256];
   MString path;
   MGlobal::executeCommand("internalVar -userScriptDir", path );
   sprintf( filename, "%s/AE%sTemplate.mel", path.asChar(), name.asChar() );

   MString msg = "Saving AE";
   msg += name;
   msg += "Template.mel in \"";
   msg += path;
   msg += "\"";
   LOG_MESSAGE(msg);

   MRL_FILE* f = MRL_FOPEN(filename, "wb");
   write_ae_header(f);
   write_ae_body(f);
   write_ae_footer(f);
   MRL_FCLOSE(f);

#if MAYA_API_VERSION >= 600
   MGlobal::executeCommand( "rehash" );
#endif
}


void mrPhenomenon::write( MRL_FILE* f )
{
   if ( written != kNotWritten ) 
      return;

   if ( options->exportFilter & mrOptions::kPhenomenizers )
      return;


   write_header(f);

   if ( miText && miText->mode == mrCustomText::kReplace )
   {
      miText->write(f); 
      write_footer(f);
      return;
   }
   
   MFnDependencyNode dep( nodeHandle.objectRef() );
   int oldExportFilter = options->exportFilter;
   int allShaders = (oldExportFilter &
		     (mrOptions::kLink + 
		      mrOptions::kInclude +
		      mrOptions::kTextures));
   options->exportFilter = oldExportFilter - allShaders;
   write_links(f, dep);

   if ( miText && miText->mode == mrCustomText::kPreface )
   {
      miText->write(f);
   }

   allShaders = (oldExportFilter &
		 (mrOptions::kShaderDef + 
		  mrOptions::kObjects +
		  mrOptions::kGroups +
		  mrOptions::kLights +
		  mrOptions::kCameras +
		  mrOptions::kMaterials +
		  mrOptions::kObjectInstances +
		  mrOptions::kGroupInstances +
		  mrOptions::kLightInstances +
		  mrOptions::kCameraInstances +
		  mrOptions::kCustomTextBoxes +
		  mrOptions::kFactoryShaders));

   options->exportFilter = oldExportFilter - allShaders;
   written = kInProgress;

   MRL_PUTS( "declare phenomenon\n" );
   MRL_PUTS("\n#\n# Interface\n#\n\n");
   write_output(f, dep);
   write_interface(f, dep);
   MRL_PUTS("\n#\n# Shading Network\n#\n\n");
   write_shaders( f, dep );
   MRL_PUTS("\n#\n# Roots\n#\n\n");
   write_roots( f, dep );
   MRL_PUTS( "end declare\n" );
   
   if ( miText && miText->mode == mrCustomText::kAppend )
   {
      miText->write(f);
   }
   
   NEWLINE();

   write_footer(f);

   write_ae_template();

   options->exportFilter = oldExportFilter;
   written = kWritten;
}

mrPhenomenon* mrPhenomenon::factory( const MPlug& op,
				     const char* container )
{
   MPlugArray plugs;
   op.connectedTo( plugs, true, false );
   MPlug cp = plugs[0];
   const MObject& obj = cp.node();
   
   MStatus status;
   MFnDependencyNode fn( obj );

   MString name = fn.name();   
   if ( container )
   {
      name += "-";
      name += container;
   }

   mrNodeList::iterator i = nodeList.find( name );
   if ( i != nodeList.end() )
   {
      return dynamic_cast< mrPhenomenon* >( i->second );
   }
   
   mrPhenomenon* s = new mrPhenomenon( fn );

   if ( container ) s->name = name;
   nodeList.insert( s );

   return s;
}



#ifdef GEOSHADER_H
#include "raylib/mrPhenomenon.inl"
#endif
