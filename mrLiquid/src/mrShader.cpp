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

#include <set>
#include <cassert>

#include "maya/MPlug.h"
#include "maya/MPlugArray.h"
#include "maya/MStringArray.h"
#include "maya/MFnNumericAttribute.h"
#include "maya/MFnNumericData.h"
#include "maya/MFnMatrixData.h"
#include "maya/MFnSet.h"
#include "maya/MMatrix.h"
#include "maya/MFnDagNode.h"
#include "maya/MTime.h"
#include "maya/MDistance.h"
#include "maya/MAngle.h"
#include "maya/MGlobal.h"
#include "maya/MAnimUtil.h"
#include "maya/MSelectionList.h"

#ifndef mrShader_h
#include "mrShader.h"
#endif

#ifndef mrShaderMaya_h
#include "mrShaderMaya.h"
#endif

#ifndef mrShaderMayaFile_h
#include "mrShaderMayaFile.h"
#endif

#ifndef mrShaderMayaParticleFile_h
#include "mrShaderMayaParticleFile.h"
#endif

#ifndef mrShaderMayaPSD_h
#include "mrShaderMayaPSD.h"
#endif

#ifndef mrShaderMayaUVChooser_h
#include "mrShaderMayaUVChooser.h"
#endif

#ifndef mrShaderLight_h
#include "mrShaderLight.h"
#endif

#ifndef mrShaderLightSpot_h
#include "mrShaderLightSpot.h"
#endif

#ifndef mrShaderAnimCurveTU_h
#include "mrShaderAnimCurveTU.h"
#endif

#ifndef mrShaderLightInfo_h
#include "mrShaderLightInfo.h"
#endif

#ifndef mrPhenomenon_h
#include "mrPhenomenon.h"
#endif

#ifndef mrTexture_h
#include "mrTexture.h"
#endif

#ifndef mrHash_h
#include "mrHash.h"
#endif

#ifndef mrHelpers_h
#include "mrHelpers.h"
#endif

#ifndef mrIds_h
#include "mrIds.h"
#endif

#ifndef mrCustomText_h
#include "mrCustomText.h"
#endif

#ifndef mrLightProfile_h
#include "mrLightProfile.h"
#endif

#ifndef mrVertexColors_h
#include "mrVertexColors.h"
#endif

#ifndef mrGroupInstance_h
#include "mrGroupInstance.h"
#endif

#ifndef mrCamera_h
#include "mrCamera.h"
#endif

#ifndef mrShaderLightCustom_h
#include "mrShaderLightCustom.h"
#endif

#ifndef mrObjectEmpty_h
#include "mrObjectEmpty.h"
#endif

#ifndef mrShadingGroup_h
#include "mrShadingGroup.h"
#endif

#ifndef mrUserData_h
#include "mrUserData.h"
#endif


#include "mrAttrAux.h"


#ifdef MR_OPTIMIZE_SHADERS
#  ifndef mrShaderFactory_h
#    include "mrShaderFactory.h"
#  endif
#endif



// This is the first VALID attribute for all shader nodes,
// skipping standard stuff like message, etc.
// This value could potentially change from maya version to maya version.
static const unsigned START_ATTR = 5;


extern MObject     currentNode;
extern MDagPath currentObjPath;


// Auxiliary macro to obtain and write child channel value or connection
#define GET_COMP( NAME, IDX ) \
	TAB(2); MRL_FPRINTF(f, "\"" #NAME "\" "); \
	cp = p.child( IDX ); \
	if ( cp.isConnected() ) \
	{ \
	   cp.connectedTo( plugs, true, false ); \
	   if ( plugs.length() == 0 ) \
	   { \
	      cp.getValue( x ); \
	      MRL_FPRINTF(f, "%g", x); \
	   } \
	   else \
	   { \
               MString destName; \
               MFnDependencyNode d( plugs[0].node() ); \
               if ( isComponentPlug( plugs[0] ) ) \
               { \
		  attrName = getStringForChildPlug( plugs[0], ">" ); \
                  char* destChannel = getLowercaseComponent( plugs[0] ); \
		  destName = d.name() + ">" + attrName + "." + destChannel; \
	       } \
	       else \
	       { \
                  if ( s->numOutAttrs > 1 ) \
		  { \
	            if ( !container ) \
	            { \
		       destName = plugs[0].info(); \
	            } \
	            else \
	            { \
		       MString plugName = plugs[0].info(); \
		       int idx = plugName.rindex('.'); \
		       destName = plugName.substring(0, idx-1); \
		       destName += "-"; \
		       destName += container; \
		       destName += plugName.substring(idx,  \
						      plugName.length()-1); \
	            } \
		  } \
		  else { \
		     destName = d.name(); \
		   } \
	       } \
	       MRL_FPRINTF(f, "= \"%s\"", destName.asChar() ); \
	    } \
	 } \
	 else \
	 { \
	    cp.getValue( x ); \
	    MRL_FPRINTF(f, "%g", x); \
	 }


const unsigned kAnimCurveTUId = 1346589781;
const unsigned kAnimCurveUUId = 1346590037;
const unsigned kPSDFileId     = 1347634260;
const unsigned kLightInfoId   = 1380731214;
const unsigned kFileId        = 1381254740;
const unsigned kUVChooserId   = 1431716680;

const unsigned kMAYA_SHADER_IDS[] = {
1431519815, // useBackground
1396985168, // shadingMap
1381253960, // checker
// 1381254740, // file (handled by special class)
1381253717, // bulge
1381253964, // cloth
1381249606, // fractal
1381254980, // grid
1381256532, // mountain
1381256534, // movie  ****not in mayabase yet***
1381256755, // noise
1381257793, // ramp
1381257027, // ocean
1381259073, // water
1381253956, // cloud
1381184560, // crater
1381254994, // granite
1381256261, // leather
1381256530, // marble
1381257803, // rock
1381259076, // wood
1381258803, // volumeNoise
1381254707, // solidFractal
1381258062, // snow
1381192520, // surfaceShader
1381185072, // stucco
1381126227, // rampShader
1380994098, // place2dTexture -> changes name to place2dtex
1380993103, // phong
1380993093, // phongE
1280922195, // layeredShader
1280922196, // layeredTexture
1380929619, // oceanShader
1380729165, // lambert
1380471874, // hairTubeShader
1380463186, // hsvToRgb
1380142926, // contrast
1380401485, // gamma
1380336205, // brownian
1380270668, // envBall
1380270920, // envChrome
1380270914, // envCube
1380275019, // envSky
1380275024, // envSphere
1380271687, // envFog
1380142131, // clamp
1380076594, // blendColors  - changes name to blend
1380076622, // blinn
1380011593, // anisotropic
1179407444, // fuild2d texture - NOT SUPPORTED YET
1179407448, // fuild3d texture - NOT SUPPORTED YET
1380734285, // luminance
1381118536, // rgbToHsv
1381190741, // surfaceLuminance
1380994369, // plusMinusAverage - changes name to plusminusavg
1380731214, // lightInfo **CHECK**
1380795465, // multiplyDivide  - changes name to multdiv
1381127763, // reverse
1380142660, // condition
1381189966, // samplerInfo
1398229041, // singleShandingSwitch  - changes name to singleswitch
1398229042, // doubleShandingSwitch  - changes name to doubleswitch
1398229043, // tripleShandingSwitch  - changes name to tripleswitch
1398229044, // quadShandingSwitch  - changes name to quadswitch
1381258068, // stencil
1381125703, // setRange
1380995658, // projection
1381385539, // vectorProduct
1380078925, // bump2d
1380078899, // bump3d
1380209480, // displacementShader - changes name to displace
1346587716, // particleCloud
1381385799, // volumeFog
#if MAYA_API_VERSION >= 700
1145324116, // distanceBetween
1380994116, // place3dtexture -- hmm... this is really not used (maya2mr bug?)
1380795212, // remapColor
1380796499, // remapHsv
1380800076, // remapValue
1146441300, // unitConversion
// maya_vertexcolors??
#endif
1380339527, // lightFog
};

const unsigned NUM_MAYA_SHADERS = ( sizeof( kMAYA_SHADER_IDS ) / 
				    sizeof( unsigned ) );



//! Check if plug passed is a component plug, like vectorX, colorR, UVW, etc.
//! To do so, we match if plug is a child AND if the last letter of
//! the plug name is uppercase.
bool mrShader::isComponentPlug( const MPlug& p )
{
   if ( !p.isChild() ) return false;

   MPlug pp = p.parent();
   const MObject& attr = pp.attribute();
   if (attr.hasFn( MFn::kNumericAttribute ))
   {
      MFnNumericAttribute n( attr );
      switch( n.unitType() )
      {
	 case MFnNumericData::k2Short:
	 case MFnNumericData::k3Short:
	 case MFnNumericData::k2Long:
	 case MFnNumericData::k3Long:
	 case MFnNumericData::k2Float:
	 case MFnNumericData::k3Float:
	 case MFnNumericData::k2Double:
	 case MFnNumericData::k3Double:
	    return true;
	 default:
	    return false;
      }
   }
   return false;
}


//!
//! As mray uses colors with alpha, we need to check if this
//! compound was a color.  If it was, we need to pass alpha
//! as 1, which is what maya2mr does.
//!
//! Also, miVector2d is not exposed in .mi scene format, so
//! all vectors are passed as triplets (WASTE OF MEMORY!!!!)
//! in maya shaders.
//!
void mrShader::dealWithComponent( MRL_FILE* f,
				  const ConnectionMap& connNames,
				  const unsigned numChildren,
				  const MPlug& ap )
{
   DBG2("dealWithComponent " << ap.info());
   // Verify no child is connected individually
   bool connected = false;
   MPlug cp;
   for ( unsigned j = 0; j < numChildren; ++j )
   {
      cp = ap.child( j );
      if ( cp.isConnected() )
      {
	 connected = true; break;
      }
   }
   if ( connected )
   {
      std::string key( cp.name().asChar() );
      ConnectionMap::const_iterator it = connNames.find( key );
      if ( it != connNames.end() )
      {
	 const MString& conn = it->second;
	 MRL_FPRINTF(f, " = \"%s\"", conn.asChar());
// 	 i += numChildren - j;
	 return;
      }
   }


   if ( numChildren == 2 )
   { 
      // dealing with a uv vector
      MVector vec;
      cp = ap.child(0);
      cp.getValue( vec.x );
      cp = ap.child(1);
      cp.getValue( vec.y );
      MRL_FPRINTF(f, " %g %g 0", vec.x, vec.y );
   }
   else if ( numChildren == 3 )
   {
      MColor col;
      cp = ap.child(0);
      cp.getValue( col[0] );
      cp = ap.child(1);
      cp.getValue( col[1]);
      cp = ap.child(2);
      cp.getValue( col[2]);
      MRL_FPRINTF(f, " %g %g %g", col[0], col[1], col[2] );
      // If we are dealing with a color parameter in
      // a non-maya custom shader, look for a 'A' plug for alpha 
      // or pass alpha as 1
      if (
	  dynamic_cast< mrShaderMaya* >( this ) == NULL &&
	  dynamic_cast< mrShaderLightSpot* >( this ) == NULL &&
	  dynamic_cast< mrShaderLight* >( this ) == NULL
	  )
      {
	 MString attrName = ap.child(2).partialName( false, true, true,
						     false, true, true );
	 if ( attrName.asChar()[ attrName.length() - 1 ] == 'B' )
	 {
	    MFnDependencyNode fn( ap.node() );
	    attrName = attrName.substring(0, attrName.length() - 1 ) + "A";
	    MStatus status;
	    MPlug p = fn.findPlug(attrName, true, &status);
	    if ( status == MS::kSuccess )
	    {
	       p.getValue( col[3] );
	       MRL_FPRINTF(f, " %g", col[3] );
	    }
	    else
	    {
	       MRL_PUTS(" 1");
	    }
	 }
      }
   }
   else
   {
      MString err = name;
      err += ": not a component plug for ";
      err += ap.info();
      LOG_ERROR(err);
   }
}

//! Print out an error message regarding an invalid connection
void mrShader::invalid_connection( const MPlug& from, const MPlug& to ) const
{
   MString msg = name;
   msg += ": Unknown or invalid shader connection for \"";
   msg += from.info();
   msg += "\" to \"";
   msg += to.info();
   msg += "\", type ";
   msg += to.node().apiTypeStr();
   LOG_ERROR( msg );
}


mrShader::mrShader( const MString& n, const MString& shader ) :
mrNode(n),
shaderName( shader ),
miText(NULL),
progressiveLast( -1.0f ),
numOutAttrs( 1 ),
isAnimated( false )
{
  options->passAlphaThrough = true;
}

mrShader::mrShader( const MString& shader, const MDagPath& path ) :
mrNode(path),
nodeHandle( path.node() ),
shaderName( shader ),
miText(NULL),
progressiveLast( -1.0f ),
numOutAttrs( 1 ),
isAnimated( MAnimUtil::isAnimated( path ) )
{
   MFnDependencyNode fn( nodeHandle.objectRef() );
   DBG( shaderName << " => " << fn.typeId().id() );
   MStatus status; MPlug p;
   GET_CUSTOM_TEXT( miText );
}


mrShader::mrShader( const MFnDependencyNode& fn ) :
mrNode(fn),
nodeHandle( fn.object() ),
miText(NULL),
progressiveLast( -1.0f ),
numOutAttrs( 0 ),
isAnimated( MAnimUtil::isAnimated( fn.object() ) )
{

   auxShaders.reserve(10);
   auxiliaryShaders.reserve(10);

   shaderName = fn.typeName();
   DBG( "\t\t" << shaderName << " => " << fn.typeId().id() );
   MStatus status; MPlug p;
   GET_CUSTOM_TEXT( miText );

   // Count the number of out attributes
   unsigned inc;
   unsigned numAttrs = fn.attributeCount();
   const MObject& shaderObj = nodeRef();
   for ( unsigned i = START_ATTR; i < numAttrs; i += inc )
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
	    ++numOutAttrs;
	 }
      }
   }

   DBG(name << "  mrShader::constructor " << numOutAttrs);
}

//@todo: speed up by using binary search and listing all ids in order.
bool mrShader::isMayaShader( unsigned id )
{
   for ( unsigned i = 0; i < NUM_MAYA_SHADERS; ++i )
      if ( id == kMAYA_SHADER_IDS[i] ) return true;
   return false;
}


mrShader* mrShader::factory( const MObject& obj,
			     const char* const container )
{
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
      return dynamic_cast< mrShader* >( i->second );
   }

   mrShader* s;
   unsigned id = fn.typeId().id();
   if ( id == kMentalrayPhenomenon )
   {
      s = new mrPhenomenon( fn );
   }
   else if ( id == kFileId )
   {
     if ( currentObjPath.hasFn( MFn::kParticle ) )
       {
	 s = new mrShaderMayaParticleFile( fn );
       }
     else
       {
	 s = new mrShaderMayaFile( fn );
       }
   }
   else if ( id == kPSDFileId )
   {
      s = new mrShaderMayaPSD( fn );
   }
   else if ( id == kUVChooserId )
   {
      s = new mrShaderMayaUVChooser( fn );
   }
   else if ( id == kAnimCurveUUId )
   {
      s = new mrShaderAnimCurveUU( fn );
   }
   else if ( id == kAnimCurveTUId )
   {
      s = new mrShaderAnimCurveTU( fn );
   }
   else if ( id == kLightInfoId )
   {
      s = new mrShaderLightInfo( fn );
   }
   else if ( id == kMentalrayVertexColors )
   {
      s = new mrVertexColors( fn );
   }
   else if ( isMayaShader( id ) )
   {
      s = new mrShaderMaya( fn, id );
   }
   else
   {
     s = new mrShader( fn );
   }

   
   if ( container ) s->name = name;

   nodeList.insert( s );
   return s;
}


mrShader* mrShader::factory( const MPlug& op,
			     const char* container )
{
   MPlugArray plugs;
   op.connectedTo( plugs, true, false );
   if ( plugs.length() < 1 )
     {
       MString err = op.name();
       err += ": not connected to a shader.";
       LOG_ERROR( err );
       return NULL;
     }

   MPlug cp = plugs[0];

   const MObject& obj = cp.node();
   
   MStatus status; MPlug p;
   MFnDependencyNode fn( obj );

   bool miMultiShader = (container != NULL);
   GET_OPTIONAL( miMultiShader );
   if ( !miMultiShader )  container = NULL;
   
   return mrShader::factory( obj, container );
}

#ifdef _DEBUG
static unsigned lastParam = 999999999;
#endif


/** 
 * The big mamma, that handles writing a paramater for a plug
 * 
 * @param f         MRL_FILE*
 * @param i         attribute index of parameter
 * @param ap        MPlug of parameter
 * @param connNames mapping of inputs<-outputs in this shader 
 * @param skip      if set, the attribute is not written, just skipped
 */
void mrShader::handleParameter( MRL_FILE* f,
				unsigned& i, const MPlug& ap,
				const ConnectionMap& connNames,
				const bool skip )
{
   DBG2("handleParameter " << i << " " << ap.info() << " skip: " << skip);
   // This was a simple check to catch parser's bug if handleParameter
   // is called recursively in an infinite way.
#ifdef _DEBUG
   assert( i != lastParam );
   lastParam = i;
#endif
   
   if ( ap.isConnected() ||
	( ap.isCompound() && ap.numConnectedChildren() > 0) )
   {
      std::string key( ap.name().asChar() );
      ConnectionMap::const_iterator it = connNames.find( key );
      if ( it != connNames.end() )
      {
	 const MString& conn = it->second;
	 if ( ap.isElement() || 
	      ap.attribute().hasFn( MFn::kMessageAttribute) )
	    MRL_FPRINTF(f, " \"%s\"", conn.asChar());
	 else
	    MRL_FPRINTF(f, " = \"%s\"", conn.asChar());
	 
	 ++i;
	 if ( ap.isCompound() )
	    i += ap.numChildren();
	 return;
      }
      else
      {
	 MPlugArray plugs;
	 ap.connectedTo( plugs, true, false );
	 if ( plugs.length() > 0 )
	 {
	    const MObject& node = plugs[0].node();
	    if ( node.hasFn( MFn::kPluginDependNode ) )
	    {
	       MFnDependencyNode fn( node );
	       if ( fn.typeId().id() == kMentalrayPhenomenon )
	       {
		  MString attrName = ap.partialName( true, false, false,
						     false, false, true );

		  char* dummy = STRDUP( attrName.asChar() );
		  char* s = dummy;
		  for ( ; *s != 0; ++s )
		  {
		     if ( *s == '[' || *s == ']' || *s == '.' ) *s = '_';
		  }
		  MRL_FPRINTF( f, " = interface \"%s\"", dummy );
		  free(dummy);

		  ++i;
		  if ( ap.isCompound() )
		     i += ap.numChildren();
		  return;
	       }
	    }
	    
	    const MObject& attrObj = ap.attribute();
	    if ( !skip && options->exportVerbosity > 5 &&
		 (! ( node.hasFn( MFn::kAnimCurve ) ||
		      node.hasFn( MFn::kExpression ) ||
		      node.hasFn( MFn::kTime ) ||
		      node.hasFn( MFn::kUnitConversion ) ||
		      attrObj.hasFn( MFn::kMatrixAttribute ) ||
		      attrObj.hasFn( MFn::kFloatMatrixAttribute ) )
		  ) )
	    {
	       MString err( ap.info() );
	       err += " was connected but connection was not matched.  "
	       "Current value used.";
	       LOG_WARNING( err );
	    }
	 }
      }
   }
   
   const MObject& attrObj = ap.attribute();
   
   if ( ap.isArray() )
   {
      bool compound = ap.isCompound();
      if (!skip) MRL_PUTS(" [");
      unsigned numElements = ap.numElements();
      unsigned numConnectedElements = ap.numConnectedElements();

      unsigned ij = i + 1;  // needed for numElements == 0
      
      if ( numConnectedElements == 0 )
      {
	 if ( attrObj.hasFn( MFn::kMessageAttribute ) )
	 {
	    MString attrName = ap.partialName( false, true, true,
					       false, true, true );
	    unsigned len = attrName.length();
	    const char* attrEnd = attrName.asChar() + len;
	    if ( strcmp( attrEnd - 6, "lights" ) == 0 ||
		 strcmp( attrEnd - 5, "light" ) == 0 )
	    {
#ifdef MR_LIGHT_GROUPS
	       MRL_PUTS("\n\"!AllLights:inst\"");
#else
	       MRL_PUTC('\n');
	       mrGroupInstance* allLights = mrGroupInstance::factory( "!AllLights" );
	       mrGroupInstance::iterator i = allLights->begin();
	       mrGroupInstance::iterator e = allLights->end();
	       bool comma = false;
	       for ( ; i != e; ++i )
	       {
		  if ( comma ) MRL_PUTS(",");
		  MRL_FPRINTF( f, " \"%s\"", (*i)->name.asChar() );
		  comma = true;
	       }
#endif
	    }
	 }
      }

      if ( numElements == 0 && compound )
      {
	 // We need to still skip parameters as if there
	 // was an element in the array as
	 // maya leaves a [-1] index as stub.
	 unsigned numChildren = ap.numChildren();
	 for ( unsigned c = 0; c < numChildren; ++c )
	 {
	    MPlug cp = ap.child( c );
	    handleParameter( f, ij, cp, connNames, true );
	 }
      }
      
      bool comma = false;
      if ( attrObj.hasFn( MFn::kMessageAttribute ) )
      {
	 for (unsigned j = 0; j < numElements; ++j )
	 {
	    MPlug pp = ap.elementByPhysicalIndex( j );
	    if ( ! pp.isConnected() ) continue;
	    
	    if ( !skip && comma )   MRL_PUTC(',');
	    comma = true;
	    
	    ij = i + 1;  // For array attributes, we reset on each iteration
	    MRL_PUTC('\n');
	    handleParameter( f, ij, pp, connNames, skip );
	 }
	 if (!skip) {
	    MRL_PUTC('\n');
	    MRL_PUTC(']');
	 }
	 i = ij;
	 return;
      }
      
      for (unsigned j = 0; j < numElements; ++j )
      {
	 MPlug pp = ap.elementByPhysicalIndex( j );
	 if ( pp.isNull() ) continue;
	 
	 bool skipchild = skip;
	 unsigned idx = pp.logicalIndex();
	 if ( idx == 0xffffffff ) skipchild = true;


	 if ( !skipchild && comma )   MRL_PUTC(',');
	 comma = true;
	    
	 ij = i + 1;  // For array attributes, we reset this on each iteration

	 if ( compound )
	 {
	    
	    unsigned numChildren = ap.numChildren();
	    
	    MPlug cp = pp.child( 0 );
	    if ( isComponentPlug( cp ) )
	    {
	       if (!skipchild) dealWithComponent( f, connNames, 
						  numChildren, pp );
	       ij += numChildren;
	    }
	    else
	    {
	       if (!skipchild) MRL_PUTS("{\n");
	       for ( unsigned c = 0; c < numChildren; ++c )
	       {
		  MPlug cp = pp.child( c );
		  if ( !skipchild )
		  { 
		     if ( c != 0 ) MRL_PUTS(",\n");
		     MString  attrName = cp.partialName( false, false, false,
							 false, false, true );
		     MStringArray split;
		     attrName.split( '_', split);
		     if ( split.length() < 2 )
			attrName.split( '.', split);
		     if ( split.length() > 1 )
			attrName = split[ split.length() - 1 ];
		     attrName = ( attrName.substring(0,0).toLowerCase() +
				  attrName.substring(1, attrName.length()-1) );
		     TAB(3);
		     MRL_FPRINTF(f, "\"%s\"", attrName.asChar() );
		  }
		  handleParameter( f, ij, cp, connNames, skipchild );
	       }
	       if (!skipchild) {
		  MRL_PUTC('\n');
		  TAB(3); MRL_PUTC('}');
	       }
	    }
	 }
	 else
	 {
	    if (!skipchild) MRL_PUTC('\n');
	    handleParameter( f, ij, pp, connNames, skipchild );
	 }
      }
      i = ij;
      
      if (!skip)
      {
	 MRL_PUTC('\n');
	 TAB(2); MRL_PUTC(']');
      }
      return;
   }


   if ( ap.isCompound() )
   {
      i += 1;
      unsigned numChildren = ap.numChildren();
	    
      MPlug cp = ap.child( 0 );
      if ( isComponentPlug( cp ) )
      {
	 if (!skip) dealWithComponent( f, connNames, numChildren, ap );
	 i += numChildren;
      }
      else
      {
	 if (!skip) MRL_PUTS("{\n");
	 for ( unsigned c = 0; c < numChildren; ++c )
	 {
	    cp = ap.child( c );
	    if ( c != 0 && !skip ) MRL_PUTS(",\n");
	    MString  attrName = cp.partialName( false, false, false,
						false, false, true );
	    MStringArray split;
	    attrName.split( '_', split);
	    if ( split.length() < 2 )
	       attrName.split( '.', split);
	    if ( split.length() > 1 )
	       attrName = split[ split.length() - 1 ];
	    attrName = ( attrName.substring(0,0).toLowerCase() +
			 attrName.substring(1, attrName.length()-1) );
	    if ( !skip )
	    {
	       TAB(3);
	       MRL_FPRINTF(f, "\"%s\"", attrName.asChar() );
	    }
	    handleParameter( f, i, cp, connNames, skip );
	 }
	 if (!skip) { TAB(3); MRL_PUTS("\n}"); }
      }

      return;
   }


   if ( skip )
   {
      i++;
      return;
   }

   if ( attrObj.hasFn( MFn::kEnumAttribute ) )
   {
      short valS;
      ap.getValue( valS );
      MRL_FPRINTF(f, " %d", valS );
   }
   else if ( attrObj.hasFn( MFn::kNumericAttribute ) )
   {
      MFnNumericAttribute n( attrObj );
	    
      switch( n.unitType() )
      {
	 case MFnNumericData::kBoolean:
	    {
	       bool  valB;
	       ap.getValue( valB );
	       MRL_FPRINTF(f, " %s", valB ? "on" : "off" );
	       break;
	    }
	 case MFnNumericData::kShort:
	 case MFnNumericData::kInt:
	    {
	       int   valI;
	       ap.getValue( valI );
	       // During progressive IPR, update attributes
	       // that end in 'samples', 'Samples' or 'Rays'.
	       if ( options->progressive && valI > 1 )
	       {
		  MString attrName = ap.partialName( false, true, true,
						     false, true, true );
		  int len = attrName.length();
		  if ( len >= 4 )
		  {
		     const char* s = attrName.asChar() + len - 4;
		     if ( strcmp( s, "Rays" ) == 0 )
		     {
			valI = (int) (valI * options->progressivePercent);
			if ( valI < 1 ) valI = 1;
			progressiveLast = options->progressivePercent;
		     }
		     else if ( len >= 7 )
		     {
			s -= 3;
			if ( (strcmp( s, "Samples" ) == 0) ||
			     (strcmp( s, "samples" ) == 0) )
			{
			   valI = (int) (valI * options->progressivePercent);
			   if ( valI < 1 ) valI = 1;
			   progressiveLast = options->progressivePercent;
			}
		     }
		  }
	       }
	       MRL_FPRINTF(f, " %d", valI );
	       break;
	    }
	 case MFnNumericData::kFloat:
	 case MFnNumericData::kDouble:
	    {
	       float valF;
	       ap.getValue( valF );
	       MRL_FPRINTF(f, " %g", valF );
	       break;
	    }
	 default:
	    mrTHROW("Invalid numeric type");
      }
   }
   else if ( attrObj.hasFn( MFn::kTypedAttribute ) )
   {
      MString valString;
      ap.getValue( valString );

      currentNode = nodeRef();
      MRL_FPRINTF(f, " \"%s\"", parseString(valString).asChar() );
   }
   else if ( attrObj.hasFn( MFn::kMessageAttribute ) )
   {
      MRL_PUTS(" \"\"" );
   }
   // Note hasFn( MFn::FloatAngleAttribute ) does not work, we check for
   // equality.
   else if ( attrObj.apiType() == MFn::kDoubleAngleAttribute ||
	     attrObj.apiType() == MFn::kFloatAngleAttribute )
   {
      MAngle t;
      ap.getValue( t );
      MRL_FPRINTF(f, " %g", t.asRadians() );
   }
   // Note hasFn( MFn::kTimeAttribute ) does not work, we check for
   // equality.
   else if ( attrObj.apiType() == MFn::kTimeAttribute )
   {
      MTime t;
      ap.getValue( t );
      MRL_FPRINTF(f, " %g", t.as( MTime::uiUnit() ) );
   }
   // Note hasFn( MFn::kDoubleLinearAttribute ) does not work, we check for
   // equality.
   else if ( attrObj.apiType() == MFn::kDoubleLinearAttribute ||
	     attrObj.apiType() == MFn::kFloatLinearAttribute )
   {
      MDistance t;
      ap.getValue( t );
      MRL_FPRINTF(f, " %g", t.as( MDistance::uiUnit() ) );
   }
   else if ( attrObj.hasFn( MFn::kMatrixAttribute ) ||
	     attrObj.hasFn( MFn::kFloatMatrixAttribute ) )
   {
      MObject t;
      ap.getValue( t );
      MFnMatrixData fn( t );
      const MMatrix& m = fn.matrix();
      write_matrix_parameter( f, m );
   }
   else if ( attrObj.hasFn( MFn::kGenericAttribute ) )
   {
      MPlugArray plugs;
      ap.connectedTo( plugs, true, false );

      const MObject& sourceObj = plugs[0].attribute();
      if ( sourceObj.hasFn( MFn::kNumericAttribute ) )
      {
	 MFnNumericAttribute n( sourceObj );
	    
	 switch( n.unitType() )
	 {
	    case MFnNumericData::kBoolean:
	       {
		  bool  valB;
		  ap.getValue( valB );
		  MRL_FPRINTF(f, " %s", valB ? "on" : "off" );
		  break;
	       }
	    case MFnNumericData::kShort:
	    case MFnNumericData::kInt:
	       {
		  int   valI;
		  ap.getValue( valI );
		  MRL_FPRINTF(f, " %d", valI );
		  break;
	       }
	    case MFnNumericData::kFloat:
	    case MFnNumericData::kDouble:
	       {
		  float valF;
		  ap.getValue( valF );
		  MRL_FPRINTF(f, " %g", valF );
		  break;
	       }
	    default:
	       mrTHROW("Invalid numeric type");
	 }
	 float tmp;
	 ap.getValue( tmp );
	 MRL_FPRINTF(f, " %g", tmp );
      }
      else if ( sourceObj.hasFn( MFn::kEnumAttribute ) )
      {
	 int tmp;
	 ap.getValue( tmp );
	 MRL_FPRINTF(f, " %d", tmp );
      }
      else if ( sourceObj.hasFn( MFn::kTimeAttribute ) )
      {
	 MTime t;
	 ap.getValue( t );
	 MRL_FPRINTF(f, " %g", t.as( MTime::uiUnit() ) );
      }
      else if ( sourceObj.hasFn( MFn::kUnitAttribute ) )
      {
	 float tmp;
	 ap.getValue( tmp );
	 MRL_FPRINTF(f, " %g", tmp );
      }
      else if ( sourceObj.hasFn( MFn::kTypedAttribute ) )
      {
	 MString valString;
	 ap.getValue( valString );
	 MRL_FPRINTF(f, " \"%s\"", parseString(valString).asChar() );
      }
      else if ( sourceObj.hasFn( MFn::kDoubleAngleAttribute ) ||
		sourceObj.hasFn( MFn::kFloatAngleAttribute )
		)
      {
	 MAngle tmp;
	 ap.getValue( tmp );
	 MRL_FPRINTF(f, " %g", tmp.asRadians() );
      }
      else if ( sourceObj.hasFn( MFn::kDoubleLinearAttribute ) ||
		sourceObj.hasFn( MFn::kFloatLinearAttribute )
		)
      {
	 MDistance t;
	 ap.getValue( t );
	 MRL_FPRINTF(f, " %g", t.as( MDistance::uiUnit() ) );
      }
      else if ( sourceObj.hasFn( MFn::kMessageAttribute ) )
      {
	 MObject tmp;
	 ap.getValue( tmp );
	 MFnDependencyNode d( tmp );
	 MRL_FPRINTF(f, " \"%s\"", d.name().asChar() );
      }
      else
      {
	 MString msg = name;
	 msg += ": Unknown connection for generic attribute \"";
	 msg += ap.info();
	 msg += "\", type ";
	 msg += sourceObj.apiTypeStr();
	 LOG_ERROR(msg);
	 MRL_PUTS(" 0");
      }
   }
   else
   {
      MString err( "Unknown attribute type in shader: " );
      err += ap.name() + " " + attrObj.apiType() + " " + attrObj.apiTypeStr();
      LOG_ERROR( err );
   }
   
   ++i;
}

/** 
 * Find the plug for a named attribute and also return its index.
 * 
 * @param i     Index of attribute in node
 * @param p     MPlug of attribute
 * @param fn    Dependency node function set
 * @param attr  Attribute to find in Dependency node function set
 * 
 * @return    MS::kSuccess if found, MS::kFailure if not
 */
MStatus mrShader::outputAttr(
			     MRL_FILE* f,
			     const MFnDependencyNode& fn, 
			     const ConnectionMap& connNames,
			     const char* attr
			     )
{
   unsigned numAttrs = fn.attributeCount();
   const MObject& shaderObj = nodeRef();
   unsigned inc;
   for ( unsigned i = START_ATTR; i < numAttrs; i += inc )
   {
      const MObject& attrObj = fn.attribute(i);
      MPlug p( shaderObj, attrObj );

      inc = 1;
      if ( p.isCompound() ) inc += p.numChildren();

      MFnAttribute fnAttr(attrObj);
      if ( fnAttr.isHidden() || !fnAttr.isReadable() || !fnAttr.isWritable() )
	 continue;


      const MString& attrName = p.partialName( false, true, true,
					       false, true, true );
      if ( attrName == attr ) 
      {
	 TAB(2);
	 MRL_FPRINTF(f, "\"%s\"", attrName.asChar() );
	 handleParameter( f, i, p, connNames );
	 return MS::kSuccess;
      }
   }
   return MS::kFailure;
}


/** 
 * Main virtual routine to write shader parameters
 * 
 * @param f 
 * @param dep 
 * @param connNames 
 */
void mrShader::write_shader_parameters( MRL_FILE* f,
					MFnDependencyNode& dep,
					const ConnectionMap& connNames )
{
   DBG2("write_shader_parameter");
   // Find parameters...
   unsigned numAttrs = dep.attributeCount();
   bool     comma = false;  // Whether to print a comma before attribute
   unsigned inc;

   const MObject& shaderObj = nodeRef();
   for ( unsigned i = START_ATTR; i < numAttrs; i += inc )
   {
      const MObject& attrObj = dep.attribute(i);
      MPlug ap( shaderObj, attrObj );
      
      inc = 1;
      if ( ap.isCompound() ) inc += ap.numChildren();

      MFnAttribute fnAttr(attrObj);
      if ( fnAttr.isHidden() || !fnAttr.isReadable() || !fnAttr.isWritable() )
	 continue;

      const MString& attrName = ap.partialName( false, true, true,
						false, true, true );
      
      inc = 0;
      if ( comma ) MRL_PUTS(",\n");
      comma = true;
      TAB(2);
      MRL_FPRINTF(f, "\"%s\"", attrName.asChar() );
      handleParameter( f, i, ap, connNames );
   }

}

/** 
 * Return the type of bridge needed for a child of a compound plug
 * 
 * @param p MPlug of a child plug
 * 
 * @return BridgeType enum
 */
mrShader::BridgeType mrShader::getBridgeType( const MPlug& p ) const
{
   int last;
   const MString& A = p.child(0).partialName( false, false, false,
					      false, false, true );
   if ( A == "uCoord" ) return kUVW;

   const MString& B = p.child(1).partialName( false, false, false,
					      false, false, true );
   MString C = "W";
   if ( p.numChildren() == 3 )
   {
      C = p.child(2).partialName( false, false, false,
				  false, false, true );
   }
   last = A.length() - 1;
   char a = A.asChar()[last];
   last = B.length() - 1;
   char b = B.asChar()[last];
   last = C.length() - 1;
   char c = C.asChar()[last];
   if ( a == 'R' && b == 'G' && c == 'B' ) return kRGB;
   if ( a == 'H' && b == 'S' && c == 'V' ) return kHSV;
   if ( a == 'U' && b == 'V' && c == 'W' ) return kUVW; 
   return kXYZ;
}


/** 
 * Crate a new out shader like shaderA>shaderB
 * 
 * @param f MRL_FILE*
 * @param p MPlug of out shader
 * @param container optional container object (for shaderA>shaderB-nurbSphere1)
 * 
 * @return MString of new shader name
 */
MString mrShader::newOutShader( MRL_FILE* f, const MPlug& p,
				const char* container )
{
   DBG2("newOutShader " << p.info());
   MPlug pp = p.parent();
   MString attrName = getStringForPlug( pp, ">" );
   MFnDependencyNode d( p.node() );
   
   MString newOutShader = d.name();
   if ( container )
   {
      newOutShader += "-";
      newOutShader += container;
   }
   newOutShader += ">";
   newOutShader += attrName;

   AuxShaders::iterator ie = auxShaders.end();
   if ( std::find( auxShaders.begin(), ie, pp ) != ie )
      return newOutShader;  // we already did this shader with another plug
   auxShaders.push_back( pp );
   
   MString newOutConn = d.name() + "." + attrName;

   if ( written == kIncremental ) MRL_PUTS("incremental ");
   MRL_FPRINTF(f, "shader \"%s\"\n", newOutShader.asChar() );

   // Now, we get the last letter of compound to help us
   // identify its type.
   BridgeType type = getBridgeType( pp );

   TAB(1);
   switch( type )
   {
      case kRGB:
	 MRL_PUTS("\"maya_color_to_rgb\" (\n");
	 TAB(2);
	 MRL_FPRINTF(f, "\"color\" = \"%s\"\n", newOutConn.asChar());
	 TAB(1);
	 MRL_PUTS(")\n");
	 break;
      case kHSV:
	 MRL_PUTS("\"maya_color_to_hsv\" (\n");
	 TAB(2);
	 MRL_FPRINTF(f, "\"color\" = \"%s\"\n", newOutConn.asChar());
	 TAB(1);
	 MRL_PUTS(")\n");
	 break;
      case kXYZ:
	 MRL_PUTS("\"maya_vector_to_xyz\" (\n");
	 TAB(2);
	 MRL_FPRINTF(f, "\"vector\" = \"%s\"\n", newOutConn.asChar());
	 TAB(1);
	 MRL_PUTS(")\n");
	 break;
      case kUVW:
	 MRL_PUTS("\"maya_vector_to_uvw\" (\n");
	 TAB(2);
	 MRL_FPRINTF(f, "\"vector\" = \"%s\"\n", newOutConn.asChar());
	 TAB(1);
	 MRL_PUTS(")\n");
	 break;
   }

   NEWLINE();
   return newOutShader;
}


/** 
 * From a plug like colorR, return the component as a character.
 * (ie. colorR -> 'R', colorG -> 'G', etc)
 * 
 * @param p Plug with a component name
 * 
 * @return component as an MString
 */
char mrShader::getComponent( const MPlug& p ) const
{
   const MString& destComp = p.partialName( false, false, false,
					    false, false, true );
   if ( destComp == "uCoord" ) return 'U';
   if ( destComp == "vCoord" ) return 'V';
   int last = destComp.length() - 1;
   return destComp.asChar()[last];
}


/** 
 * From a plug like colorR, return the component as a lowercase character.
 * (ie. colorR -> "r", colorG -> "g", etc)
 * 
 * @param p Plug with a component name
 * 
 * @return component as an MString
 */
char* mrShader::getLowercaseComponent( const MPlug& p ) const
{
   static char ret[2] = {0, 0};
   ret[0] = getComponent(p);
   if ( ret[0] >= 65 && ret[0] <= 97 )
      ret[0] += 32;
   else {
      // these are needed to handle single outputs of switch shaders
      switch( ret[0] )
      {
	 case '1':
	    ret[0] = 'x'; break;
	 case '2':
	    ret[0] = 'y'; break;
	 case '3':
	    ret[0] = 'z'; break;
      }
   }
   return ret;
}


/** 
 * Given a plug used in a bridge, return the corresponding name
 * to appear in the mi file.
 * 
 * @param p    plug that the bridge points to (should be child plug).
 * @param dir  direction of the bridge ("<" or ">")
 * 
 * @return     name of connection
 */
MString mrShader::getStringForPlug( const MPlug& pp,
				    const char* dir ) const
{
   MString attrName = pp.partialName( false, false, false,
				      false, false, true );
   int idx;
   while( (idx = attrName.index('.')) > 0 )
   {
      attrName = attrName.substring(0,     idx-1) + dir + 
                 attrName.substring(idx+1, attrName.length());
   }
   return attrName;
}


/** 
 * Given a plug used in a bridge, return the corresponding name
 * to appear in the mi file.
 * 
 * @param p    plug that the bridge points to (should be child plug).
 * @param dir  direction of the bridge ("<" or ">")
 * 
 * @return     name of connection
 */
MString mrShader::getStringForChildPlug( const MPlug& p,
					 const char* dir ) const
{
   return getStringForPlug( p.parent(), dir );
}



void mrShader::getChildShaders( NodeSet& nodes,
				const MFnDependencyNode& dep )
{
  MPlugArray connAttrs;
  dep.getConnections( connAttrs );

  int cont = name.rindex('-');
  const char* container = NULL;
  if ( cont >= 0 ) container = name.asChar() + cont + 1;

  unsigned numConn = connAttrs.length();
  if ( numConn == 0 ) return;

  MPlugArray plugs;
  for ( unsigned i = 0; i < numConn; ++i )
    {
      connAttrs[i].connectedTo( plugs, true, false );
      if ( plugs.length() == 0 ) continue;

      DBG(name << ": getChildShaders for plug " << plugs[0].info());

      const MObject& newObj = plugs[0].node();

      if ( newObj.hasFn( MFn::kDagNode ) )
	{
	  switch( newObj.apiType() )
	    {
	    case MFn::kFluidTexture2D:
	    case MFn::kFluidTexture3D:
	      {
		container = NULL;
		break;
	      }
	    case MFn::kCamera:
	      {
		MDagPath camPath;
		MDagPath::getAPathTo( newObj, camPath );
		mrCamera* cam = mrCamera::factory( NULL,
						   camPath, 
						   MObject::kNullObj,
						   MObject::kNullObj );
		mrInstanceCamera* inst = mrInstanceCamera::factory( camPath, 
								    cam );
		nodes.insert( inst );
		continue;
	      }
	    case MFn::kLightSource:
	    case MFn::kTransform:
	    case MFn::kMesh:
	    case MFn::kSubdiv:
	    case MFn::kNurbsSurface:
	    case MFn::kNurbsCurve:
	    default:
	      {
		continue;
	      }
	    }
	}
      else if ( newObj.hasFn( MFn::kPluginDependNode ) )
	{
	  MFnDependencyNode fn( newObj );

	  unsigned id = fn.typeId().id();
	  switch( id )
	    {
	    case kMentalrayLightProfile:
	      {
		mrLightProfile* s = mrLightProfile::factory( fn );
		nodes.insert( s );
		continue;
	      }
	    case kMentalrayVertexColors:
	      break;
	    case kMentalrayUserData:
	    case kFurGlobals:
	    case kMentalrayPhenomenon:
	      {
		continue;
	      }
	    default:
	      {
		MStatus status;
		MPlug tp = fn.findPlug("miFactoryNode", false, &status);
		if ( status != MS::kSuccess )
		  continue;
		break;
	      }
	    }
	}
      else if ( options->motionBlur == mrOptions::kMotionBlurExact && 
		options->scanline != mrOptions::kRapid &&
		newObj.hasFn( MFn::kAnimCurveTimeToUnitless ) )
	{
	  container = NULL;
	}
      else if ( newObj.hasFn( MFn::kAnimCurve ) ||
		newObj.hasFn( MFn::kExpression ) ||
		newObj.hasFn( MFn::kTime ) )
	{
	  continue;
	}
      else if ( newObj.hasFn( MFn::kPlace2dTexture ) )
	{
	  // We assume this node is the leaf of a shading network, so
	  // no container needed.
	  container = NULL;
	}
      else if ( newObj.hasFn( MFn::kPlace3dTexture ) )
	{
	  // We assume this node is the leaf of a shading network, so
	  // no container needed.
	  container = NULL;
	}
#if MAYA_API_VERSION >= 600
      else if ( newObj.hasFn( MFn::kMentalRayTexture ) )
	{
	  MFnDependencyNode fn( newObj );
	  mrTextureNode* s = mrTextureNode::factory( fn );
	  nodes.insert( s );
	  // 	 s->setIncremental(sameFrame);
	  continue;
	}
#endif
#if MAYA_API_VERSION >= 700
      else if ( newObj.hasFn( MFn::kUnitConversion ) )
	{ // empty on purpose
	  MFnDependencyNode fn( newObj );
	  MPlug p = fn.findPlug("input");
	  if ( !p.isConnected() )
	    continue;
	}
#endif
      else if ( newObj.hasFn( MFn::kShadingEngine ) )
      {
	 // Handle materials connected as mental ray parameters
	 MFnDependencyNode fn( newObj );
	 mrShadingGroup* g = mrShadingGroup::factory(fn);
	 nodes.insert( g );
	 continue;
      }
      else if (
	       newObj.hasFn( MFn::kSet )
	       )
      {
	 // Handle light/object sets as mental ray instance groups
	 MFnSet fn( newObj );
	 mrGroupInstance* s = mrGroupInstance::factory( fn );
	 MString name = fn.name() + ":inst";
	 mrInstanceGroup* g = mrInstanceGroup::factory( name, s );
	 nodes.insert( g );
	 continue;
      }
      else if (
	       newObj.hasFn( MFn::kDependencyNode )
	       )
	{
	  MFnDependencyNode fn( newObj );
	  const MString& klass = fn.classification( fn.typeName() );
	  if ( klass.length() < 1 ) continue;  // not a shader, skip it
	}
      else
	{
	  // invalid or unknown connection
	  continue;
	}

      mrShader* s = mrShader::factory( newObj, container );
      if ( !s || s->written == kInProgress ) continue; 

      nodes.insert( s );

    } // for (i... numConn)


}


void mrShader::setWrittenChildShaders( const WriteMode val )
{
   DBG(name << ": setWrittenChildShaders");
   MFnDependencyNode dep( nodeRef() );


   NodeSet nodes;
   getChildShaders( nodes, dep );

   
   NodeSet::iterator i = nodes.begin();
   NodeSet::iterator e = nodes.end();
   for ( ; i != e; ++i )
     {
       if ( val == kIncremental ) 
	 {
	   (*i)->forceIncremental();
	 }
       else
	 {
	   (*i)->written = val;
	 }
     }
}

void mrShader::setIncrementalChildShaders( bool sameFrame )
{
  DBG(name << ": setIncrementalChildShaders");

  NodeSet nodes;
  MFnDependencyNode dep( nodeRef() );
  getChildShaders( nodes, dep );
   
  NodeSet::iterator i = nodes.begin();
  NodeSet::iterator e = nodes.end();
  for ( ; i != e; ++i )
    {
      if ( dynamic_cast< mrInstanceCamera* >( *i ) != NULL )
	{
	  (*i)->setIncremental( false );
	}
      else
	{
	  (*i)->setIncremental( sameFrame );
	}
    }

}



/** 
 * Recurse thru all the connections to this shader and update other
 * child shaders.
 * 
 * @param f     MRL_FILE*
 * @param dep   MFnDependencyNode pointing to this shader.
 */
void mrShader::updateChildShaders( MRL_FILE* f,
				   const MFnDependencyNode& dep )
{

  NodeSet nodes;

  getChildShaders( nodes, dep );

  NodeSet::iterator i = nodes.begin();
  NodeSet::iterator e = nodes.end();
  for ( ; i != e; ++i )
    {
      (*i)->write( f );
    }
}


/** 
 * Iterate thru all connections and create a mapping of source.plug->connection
 * stored in connNames.  Create any connected child shaders or bridge 
 * shaders as are needed.
 * 
 * @param f 
 * @param connNames 
 * @param dep 
 */
void mrShader::getConnectionNames( MRL_FILE* f, ConnectionMap& connNames,
				   const MFnDependencyNode& dep )
{
   DBG(name << "  mrShader::getConnectionNames");
   
   MPlugArray connAttrs;
   dep.getConnections( connAttrs );

   const char* container = NULL;
   {
      int cont = name.rindex('-');
      if ( cont >= 0 ) container = name.asChar() + cont + 1;
   }
   
   ////////////////////////////////////////////////////////////////////////
   // OK, first we iterate thru all the connections to this node
   // and we store a mapping of them.  We need to do this as not all
   // connections result in spawning new shaders (for example, connections
   // may be message connections to lights or objects).
   //
   // Also, in the case of mental ray, mray's phenomena trees are not
   // as flexible as maya and as such, only connections of identical type
   // are allowed.  So, for a color component, you can only connect another
   // color.  In the maya hypergraph, however, the user can connect each r,g,b
   // channel of the color individually.  To solve the issue, maya2mr relies
   // on auxiliary shader nodes that will turn a color into a three struct
   // component which can be connected individually.
   // This is, overall, a PAIN in the butt and makes the code below 10 times
   // more complex than it should be.
   //
   // Note that mray ONLY allows connections with OUT parameters of shaders,
   // like outColor, outValue, etc.  This is a another annoying limitation
   // of phenomena, too, albeit more from the user's pov.
   //
   ////////////////////////////////////////////////////////////////////////
   unsigned numConn = connAttrs.length();
   MPlugArray plugs;
   for ( unsigned i = 0; i < numConn; ++i )
   {
      connAttrs[i].connectedTo( plugs, true, false );
      if ( plugs.length() == 0 )
	 continue;

      DBG(name << " getConnectionNames for plug " << plugs[0].info() );

      MObject newObj = plugs[0].node();
      std::string key( connAttrs[i].name().asChar() );

      if ( newObj.hasFn( MFn::kDagNode ) )
      {
	 switch( newObj.apiType() )
	 {
	    case MFn::kCamera:
	       {
		  MDagPath camPath;
		  MDagPath::getAPathTo( newObj, camPath );
		  mrCamera* cam = mrCamera::factory( NULL,
						     camPath, 
						     MObject::kNullObj,
						     MObject::kNullObj );
		  mrInstanceCamera* inst = mrInstanceCamera::factory( camPath, 
								      cam );
		  inst->write(f);
		  connNames.insert( std::make_pair( key, inst->name ) );
		  continue;
	       }
	    case MFn::kLightSource:
	       {
		  MDagPath inst;
		  MDagPath::getAPathTo( plugs[0].node(), inst );
		  inst.pop();
		  connNames.insert( std::make_pair( key,
						    getMrayName( inst ) ) );
		  continue;
	       }
	    case MFn::kMesh:
	    case MFn::kSubdiv:
	    case MFn::kNurbsSurface:
	       {
		  if ( dynamic_cast< mrShaderMaya* >(this) == NULL )
		  {
		     MDagPath objPath;
		     MDagPath::getAPathTo( plugs[0].node(), objPath );
		     mrObject* obj = mrObjectEmpty::factory( objPath );
		     assert( obj != NULL );
		     if (obj->written == kNotWritten)
			obj->write(f);
		     connNames.insert( std::make_pair( key, obj->name ) );
		     continue;
		  }
		  // else... single/dual/tripleswitch shader... 
		  MDagPath instPath;
		  MDagPath::getAPathTo( newObj, instPath );

		  instPath.pop();
		  newObj = instPath.node();
		  // deal as if it was a transform... no break here
	       }
	    case MFn::kTransform:
	       {
		  MDagPath instPath;
		  MDagPath::getAPathTo( newObj, instPath );

		  unsigned numShapes;
		  MRL_NUMBER_OF_SHAPES( instPath, numShapes );
		  if ( numShapes == 0 )
		  {
		     MString err = name;
		     err += ": attached to empty transform \"";
		     err += instPath.fullPathName();
		     err += "\"";
		     LOG_ERROR(err);
		     continue;
		  }

		  if ( numShapes > 1 )
		  {
		     MString err = name;
		     err += ": attached to transform with multiple shapes \"";
		     err += instPath.fullPathName();
		     err += "\"";
		     LOG_ERROR(err);
		     continue;
		  }

		  MDagPath shape = instPath;
		  MRL_EXTEND_TO_SHAPE_DIRECTLY_BELOW( shape, 0, numShapes );

		  mrInstance* inst = mrInstance::factory( shape, NULL );
		  assert( inst != NULL );
		  if (inst->written == kNotWritten)
		  {
		     DBG("++++++++++++DUMMY INSTANCE+++++++++++++")
		     MRL_FPRINTF(f, "instance \"%s\"\nend instance\n",
			     inst->name.asChar());
		     inst->written = kIncremental;
		     NEWLINE();
		  }

		  connNames.insert( std::make_pair( key, inst->name ) );
		  continue;
	       }
	    case MFn::kFluidTexture2D:
	    case MFn::kFluidTexture3D:
	       {
		  container = NULL;
		  break;
	       }
	    default:
	       {
		  continue;
	       }
	 }
      }
#if MAYA_API_VERSION >= 600
      else if ( newObj.hasFn( MFn::kMentalRayTexture ) )
      {
	 MFnDependencyNode fn( newObj );
	 mrTextureNode* s = mrTextureNode::factory( fn );
	 s->write(f);
	 connNames.insert( std::make_pair( key, s->name ) );
	 continue;
      }
#endif
      else if ( newObj.hasFn( MFn::kPluginDependNode ) )
      {
	 MFnDependencyNode fn( newObj );

	 unsigned id = fn.typeId().id();
	 switch( id )
	 {
	    case kMentalrayLightProfile:
	       {
		  mrLightProfile* s = mrLightProfile::factory( fn );
		  s->write(f);
		  connNames.insert( std::make_pair( key, s->name ) );
		  continue;
	       }
	       break;
	    case kMentalrayUserData:
	       {
		  mrUserData* s = mrUserData::factory( fn );
		  s->write(f);
		  connNames.insert( std::make_pair( key, s->name ) );
		  continue;
	       }
	    case kFurGlobals:
	    case kMentalrayPhenomenon:
	    case kMentalrayVertexColors:
	       {
		  // we don't add to connection names
		  continue;
	       }
	    default:
	       {
		  MStatus status;
		  MPlug tp = fn.findPlug("miFactoryNode", false, &status);
		  if ( status != MS::kSuccess )
		  {
		     MString msg = name;
		     msg += ": Unknown plugin connection for attribute \"";
		     msg += connAttrs[i].info();
		     msg += "\" to \"";
		     msg += plugs[0].info();
		     msg += "\"";
		     LOG_ERROR(msg);
		     continue;
		  }
		  break;
	       }
	 }
      }
      else if ( newObj.hasFn( MFn::kShadingEngine ) )
      {
	 // Handle materials connected as mental ray parameters
	 MFnDependencyNode fn( newObj );
	 mrShadingGroup* g = mrShadingGroup::factory(fn);
	 g->write(f);
	 connNames.insert( std::make_pair( key, g->name ) );
	 continue;
      }
      else if ( newObj.hasFn( MFn::kSet ) )
      {
	 // Handle light/object sets as mental ray instance groups
	 MFnSet fn( newObj );

	 mrGroupInstance* s = mrGroupInstance::factory( fn );
	 MString name = fn.name() + ":inst";
	 mrInstanceGroup* g = mrInstanceGroup::factory( name, s );
	 g->write(f);

	 connNames.insert( std::make_pair( key, name ) );
	 continue;
      }
#if MAYA_API_VERSION >= 700
      else if ( options->motionBlur == mrOptions::kMotionBlurExact && 
		options->scanline != mrOptions::kRapid &&
		newObj.hasFn( MFn::kAnimCurveTimeToUnitless ) )
      {
	 container = NULL;
      }
      else if ( newObj.hasFn( MFn::kUnitConversion ) ||
		newObj.hasFn( MFn::kAnimCurveUnitlessToUnitless ) 
		 )
      { 
         MFnDependencyNode fn( newObj );
	 MPlug p = fn.findPlug("input");
	 if ( !p.isConnected() ) continue;
	 container = NULL;
      }
#endif
      else if ( newObj.hasFn( MFn::kAnimCurve ) ||
		newObj.hasFn( MFn::kExpression ) || 
		newObj.hasFn( MFn::kTime ) )
      {
	 isAnimated = true;
	 continue;
      }
      else if ( newObj.hasFn( MFn::kPlace2dTexture ) )
      {
	 // We assume this node is the leaf of a shading network, so
	 // no container needed.
	 container = NULL;
      }
      else if ( newObj.hasFn( MFn::kPlace3dTexture ) )
      {
	 // We assume this node is the leaf of a shading network, so
	 // no container needed.
	 container = NULL;
      }
      else if ( newObj.hasFn( MFn::kDisplayLayer ) )
	{
	  // ignore display layer connection ( for IBL or lights mainly )
	  continue;
	}
      else if (
	       newObj.hasFn( MFn::kDependencyNode )
	       )
      {
	 MFnDependencyNode fn( newObj );
	 const MString& klass = fn.classification( fn.typeName() );
	 if ( klass.length() < 1 ) {
	    if ( dynamic_cast< mrShaderMaya* >(this) != NULL )
	    {
	       mrShaderMaya* dummy = (mrShaderMaya*) this;
	       if ( dummy->id == 0 )
		  continue;
	    }
	    invalid_connection( connAttrs[i], plugs[0] );
	    continue;  // not a shader, skip it
	 }
      }
      else
	{
	  invalid_connection( connAttrs[i], plugs[0] );
	  continue;
	}


      mrShader* s = mrShader::factory( newObj, container );
      if ( !s || s->written == kInProgress ) continue;  // cycle check

      DBG(name << ": +++ NEW CHILD SHADER " << s->name);
      s->write(f);
      DBG2(name << ": +++ WRITTEN CHILD SHADER " << s->name);

      // OKAY, are we are dealing with a child connection of
      // a compound attribute?  If so, we need to create a shader bridge
      if ( isComponentPlug( connAttrs[i] ) )
      {
	 DBG(connAttrs[i].info() << " is a child attribute");

	 // Change the connection map key to be the parent (main) plug
	 MPlug p = connAttrs[i].parent();

	 AuxShaders::iterator ie = auxShaders.end();
	 if ( std::find( auxShaders.begin(), ie, p ) != ie )
	    continue;  // we already did this shader with another plug

	 auxShaders.push_back( p );
	 auxiliaryShaders.push_back( s );
	 continue;
      }
      else  // No, we are connecting directly into the plug
      {

// 	// Is this an output plug?  If not, no connection (we use
// 	// value directly)
//      // this is wrong.  it needs to check if it is a mentalray output,
//      // not a maya output
// 	MFnAttribute fnAttr( plugs[0].attribute() );
// 	if ( fnAttr.isWritable() )
// 	  {
// 	    continue;
// 	  }

	 MString destName;
	 if ( isComponentPlug( plugs[0] ) )
	 {
	    MString outShader = newOutShader( f, plugs[0], container );
	    char* destComp = getLowercaseComponent( plugs[0] );
	    destName = outShader + "." + destComp;
	 }
	 else
	 {
	    DBG(s->name << "  # outputs: " << s->numOutAttrs);
	    // Are we dealing with a shader with multiple out attributes,
	    // like maya's shaders?  If so, it should have an "out"
	    // attribute and we need to use the full name of the plug.
	    if ( s->numOutAttrs > 1 )
	    {
	       if ( !container )
	       {
		  destName = plugs[0].info();
		  DBG("destName !container: " << destName);
	       }
	       else
	       {
		  MString plugName = plugs[0].info();
		  int idx = plugName.rindex('.');
		  destName = plugName.substring(0, idx-1);
		  destName += "-";
		  destName += container;
		  destName += plugName.substring(idx, plugName.length()-1);
	       }
	    }
	    else
	    {
	      // single output result, we can just hook up the
	      // name of the shader directly, without the plug name
	      MFnDependencyNode d( newObj );
	      destName = d.name();
	      if ( container )
		{
		  destName += "-";
		  destName += container;
		}
	    }
	 }

	 DBG("destName: " << destName);
	 connNames.insert( std::make_pair( key, destName ) );
      }
      
   } // for (i... numConn)
   
   AuxiliaryShaders::iterator si = auxiliaryShaders.begin();
   AuxiliaryShaders::iterator se = auxiliaryShaders.end();

   AuxShaders::iterator it = auxShaders.begin();

   for ( ; si != se; ++it, ++si )
   {
      MPlug p = *it;
      mrShader* s = *si;

      MString newShader = dep.name();
      if ( container ) {
	 newShader += "-"; newShader += container;
      }
      newShader += "<";

      MString attrName = getStringForPlug( p, "<" );
      newShader += attrName;

      std::string key = p.name().asChar();
      connNames.insert( std::make_pair( key, newShader ) );

      DBG2("new < shader " << newShader);

      ////////////////////////////////////////////////////////////////
      // Handle single channel connections using a dummy shader bridge
      ////////////////////////////////////////////////////////////////

      // Before we create this new shader, we need to verify the child
      // plugs to make sure we don't need to ALSO create a dummy
      // shader in the outgoing direction.
      // For example, if we have:
      // ramp1
      // lambert1
      //
      // and we connect:
      //
      // lambert1.colorR = ramp1.outColorR
      //
      // we need to create BOTH (notice the arrows):
      //      ramp1>outColor
      //      lambert1<color
      //
      /// Here we check to see if we create > out bridges
      ///
      unsigned numChildren = p.numChildren();
      for ( unsigned c = 0; c < numChildren; ++c )
      {
	 MPlug cp = p.child(c);
	 if ( ! cp.isConnected() ) continue;
	 cp.connectedTo( plugs, true, false );
	 if ( plugs.length() == 0 ) continue;
	 if ( !isComponentPlug( plugs[0] ) ) continue;

	 // ok, we DO need to create a new outgoing bridge, too
	 newOutShader( f, plugs[0], container );
      }

      // Now, let's go ahead and create the new < bridge shader

      // Find the type of auxiliary shader we need to create
      // from the last letter of the connection.
      BridgeType type = getBridgeType( p );
	    
      if ( written == kIncremental ) MRL_PUTS("incremental ");
      MRL_FPRINTF(f, "shader \"%s\"\n", newShader.asChar() );
	      
      float x;
      MPlug cp;
      TAB(1);
      switch(type)
      {
	 case kRGB:
	    MRL_PUTS("\"maya_rgb_to_color\" (\n");
	    GET_COMP( r, 0 ); MRL_PUTC(','); MRL_PUTC('\n');
	    GET_COMP( g, 1 ); MRL_PUTC(','); MRL_PUTC('\n');
	    GET_COMP( b, 2 ); MRL_PUTC('\n');
	    TAB(1); MRL_PUTS(")\n");
	    break;
	 case kHSV:
	    MRL_PUTS("\"maya_hsv_to_color\" (\n");
	    GET_COMP( h, 0 ); MRL_PUTC(','); MRL_PUTC('\n');
	    GET_COMP( s, 1 ); MRL_PUTC(','); MRL_PUTC('\n');
	    GET_COMP( v, 2 ); MRL_PUTC('\n');
	    TAB(1); MRL_PUTS(")\n");
	    break;
	 case kXYZ:
	    MRL_PUTS("\"maya_xyz_to_vector\" (\n");
	    GET_COMP( x, 0 ); MRL_PUTC(','); MRL_PUTC('\n');
	    GET_COMP( y, 1 ); MRL_PUTC(','); MRL_PUTC('\n');
	    GET_COMP( z, 2 ); MRL_PUTC('\n');
	    TAB(1); MRL_PUTS(")\n");
	    break;
	 case kUVW:
	    MRL_PUTS("\"maya_uvw_to_vector\" (\n");
	    GET_COMP( u, 0 ); MRL_PUTC(','); MRL_PUTC('\n');
	    GET_COMP( v, 1 ); MRL_PUTC(','); MRL_PUTC('\n');
	    GET_COMP( w, 2 ); MRL_PUTC('\n');
	    TAB(1); MRL_PUTS(")\n");
	    break;
      }

      NEWLINE();
   }

   auxiliaryShaders.clear();
   auxShaders.clear();
   /////////////// end of getting connections
}


void mrShader::write( MRL_FILE* f )
{

   DBG( name << ": mrShader::write()" );

   if ( options->exportFilter & mrOptions::kShaderDef )
      return;

#ifndef MR_NO_CUSTOM_TEXT
   if ( miText && miText->mode == mrCustomText::kReplace )
   {
      miText->write(f); return;
   }
   
   if ( miText && miText->mode == mrCustomText::kPreface )
   {
      miText->write(f);
   }
#endif

   // Multisample shaders will get re-spitted on incremental updates..
   if ( written == kWritten && options->progressive && 
	progressiveLast >= 0.0f &&
	options->progressivePercent != progressiveLast )
   {
      written = kIncremental;
   }

   MFnDependencyNode dep( nodeRef() );
   
   if ( written == kWritten )
   {
     written = kInProgress;
     updateChildShaders( f, dep ); 
     written = kWritten;
     return;
   }


   char oldWritten = written;
   written = kInProgress;
   ConnectionMap connNames;
   getConnectionNames( f, connNames, dep );
   DBG2(name << ": Done with getConnection names");


   
   if ( oldWritten == kIncremental )
   {
      MRL_PUTS("incremental ");
   }
#ifdef MR_OPTIMIZE_SHADERS
   else
   {
      write_link_line( f );
   }
#endif

   MRL_FPRINTF(f, "shader \"%s\"\n", name.asChar() );

   TAB(1);
   MRL_FPRINTF(f, "\"%s\" (\n", shaderName.asChar() );

   write_shader_parameters( f, dep, connNames );
   MRL_PUTC('\n');
   TAB(1); MRL_PUTS(")\n");
   
   if ( miText && miText->mode == mrCustomText::kAppend )
   {
      miText->write(f);
   }
   
#ifdef _DEBUG
   // Reset parameter checking
   lastParam = 999999999;
#endif
   
   NEWLINE();
   written = kWritten;

}


// GOTCHA... if the user changes shader values thru MEL's renderPass
//           overrides, incremental saving will not re-spit the material, as
//           it should.  Not sure how to solve this other than caching and
//           checking ALL parameters of the shader, which is not a good idea.
void mrShader::newRenderPass()
{
   char oldWritten = written;
   written = kInProgress;
   setIncrementalChildShaders(true);
   written = oldWritten;
}


void mrShader::forceIncremental()
{
   if ( written == kWritten ) written = kIncremental;
}

void mrShader::setWritten( const WriteMode val )
{
   if ( written == kWritten ) written = val;
   // And update all child shaders, too
   char oldWritten = written;
   written = kInProgress;
   setWrittenChildShaders( val );
   written = oldWritten;
}

void mrShader::setIncremental(bool sameFrame)
{
   if ( isAnimated && written == kWritten ) written = kIncremental;
   // And update all child shaders, too
   char oldWritten = written;
   written = kInProgress;
   setIncrementalChildShaders( sameFrame );
   written = oldWritten;
}


void mrShader::write_link_line( MRL_FILE* f )
{
#ifdef MR_OPTIMIZE_SHADERS
   if ( !(options->exportFilter & mrOptions::kLink) )
   {
      DBG(name << "  find dso for shader: " << shaderName );
      const MString& tmp = mrShaderFactory::findDSO( shaderName );
      DBG(name << "  shader: " << shaderName << "   in dso file: " << tmp);
      if ( tmp.length() && 
	   options->DSOs.find( tmp ) == options->DSOs.end() )
      {
	 MRL_FPRINTF(f, "link \"%s\"\n", tmp.asChar() );
	 options->DSOs.insert( tmp );
      }
   }

   if ( !(options->exportFilter & mrOptions::kInclude) )
   {
      DBG(name << "  find mi for shader: " << shaderName );
      const MString& tmp = mrShaderFactory::findMI( shaderName );
      DBG(name << "  shader: " << shaderName << "   in mi file: " << tmp);
      if ( tmp.length() && 
	   options->miFiles.find( tmp ) == options->miFiles.end() )
      {
	 const char* mifile = tmp.asChar();
	 MRL_FPRINTF(f, "$ifndef \"%s\"\n", mifile);
	 MRL_FPRINTF(f, "$include \"%s\"\n", mifile );
	 MRL_FPRINTF(f, "set \"%s\" \"t\"\n", mifile );
	 MRL_PUTS("$endif\n");
	 options->miFiles.insert( tmp );
	 NEWLINE();
      }
   }
#endif
}


void mrShader::write_link_lines( MRL_FILE* f )
{
   char oldWritten = written;
   written = kInProgress;
   write_link_line(f);

   MFnDependencyNode dep( nodeRef() );

   MPlugArray connAttrs;
   dep.getConnections( connAttrs );

   int cont = name.rindex('-');
   const char* container = NULL;
   if ( cont >= 0 ) container = name.asChar() + cont + 1;

   
   unsigned numConn = connAttrs.length();
   MPlugArray plugs;
   for ( unsigned i = 0; i < numConn; ++i )
   {
      connAttrs[i].connectedTo( plugs, true, false );
      if ( plugs.length() == 0 ) continue;

      const MObject& newObj = plugs[0].node();

      if ( newObj.hasFn( MFn::kDagNode ) )
      {
	 switch( newObj.apiType() )
	 {
	    case MFn::kFluidTexture2D:
	    case MFn::kFluidTexture3D:
	       {
		  container = NULL;
		  break;
	       }
	    case MFn::kCamera:
	    case MFn::kLightSource:
	    case MFn::kTransform:
	    case MFn::kMesh:
	    case MFn::kSubdiv:
	    case MFn::kNurbsSurface:
	    case MFn::kNurbsCurve:
	    default:
	       {
		  continue;
	       }
	 }
      }
      else if ( newObj.hasFn( MFn::kPluginDependNode ) )
      {
	 MFnDependencyNode fn( newObj );

	 unsigned id = fn.typeId().id();
	 switch( id )
	 {
	    case kMentalrayLightProfile:
	    case kMentalrayUserData:
	    case kMentalrayVertexColors:
	       {
		  continue;
	       }
	    default:
	       {
		  MStatus status;
		  MPlug tp = fn.findPlug("miFactoryNode", false, &status);
		  if ( status != MS::kSuccess )
		     continue;
		  break;
	       }
	 }
      }
      else if ( options->motionBlur == mrOptions::kMotionBlurExact && 
		options->scanline != mrOptions::kRapid &&
		newObj.hasFn( MFn::kAnimCurveTimeToUnitless ) )
      {
	 container = NULL;
      }
      else if ( newObj.hasFn( MFn::kAnimCurve ) ||
		newObj.hasFn( MFn::kExpression ) ||
		newObj.hasFn( MFn::kTime ) )
      {
	 continue;
      }
      else if ( newObj.hasFn( MFn::kPlace2dTexture ) )
      {
	 // We assume this node is the leaf of a shading network, so
	 // no container needed.
	 container = NULL;
      }
      else if ( newObj.hasFn( MFn::kPlace3dTexture ) )
      {
	 // We assume this node is the leaf of a shading network, so
	 // no container needed.
	 container = NULL;
      }
#if MAYA_API_VERSION >= 700
      else if ( newObj.hasFn( MFn::kUnitConversion ) )
      { // empty on purpose
         MFnDependencyNode fn( newObj );
	 MPlug p = fn.findPlug("input");
	 if ( !p.isConnected() ) continue;
      }
#endif
#if MAYA_API_VERSION >= 600
      else if ( newObj.hasFn( MFn::kMentalRayTexture ) )
      {
	 MFnDependencyNode fn( newObj );
	 mrTextureNode* s = mrTextureNode::factory( fn );
	 s->write(f);
	 continue;
      }
#endif
      else if ( newObj.hasFn( MFn::kDisplayLayer ) )
	{
	  // ignore display layer connection ( for IBL or lights mainly )
	  continue;
	}
      else if (
	       newObj.hasFn( MFn::kDependencyNode )
	       )
      {
	 MFnDependencyNode fn( newObj );
	 const MString& klass = fn.classification( fn.typeName() );
	 if ( klass.length() < 1 ) continue;  // not a shader, skip it
      }
      else
      {
	 // invalid connection
	 continue;
      }

      mrShader* s = mrShader::factory( newObj, container );
      if ( !s || s->written == kInProgress ) continue;
      s->write_link_lines(f);

      // If a mentalray texture or a maya file texture shader, write line too
      mrShaderMayaFile* file = dynamic_cast< mrShaderMayaFile* >( s );
      if ( file ) 
      {
	 file->write_texture(f);
      }
   } // for (i... numConn)


   written = oldWritten;
}

#ifdef GEOSHADER_H
#include "raylib/mrShader.inl"
#endif

