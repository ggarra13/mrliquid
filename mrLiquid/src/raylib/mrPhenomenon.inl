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

#include <cassert>


void mrPhenomenon::write_default_value( const MPlug& p )
{
   if ( p.isArray() ) return;

   const MString& attrName = p.partialName( false, false, false,
					    false, false, true );

   miScalar valF = 1.0f;
   miInteger valI;

   if ( p.isCompound() )
   {
      unsigned numChildren = p.numChildren();
      if ( numChildren < 4 )
      {

	if ( attrName == "normalCamera" ||
	     attrName == "miNormalCamera" )
	  {
	    mi_api_parameter_default( miTYPE_SCALAR, &valF );
	    mi_api_parameter_default( miTYPE_SCALAR, &valF );
	    mi_api_parameter_default( miTYPE_SCALAR, &valF );
	    return;
	  }

	for ( unsigned i = 0; i < numChildren; ++i )
	  {
	    MPlug cp = p.child(i);
	    double val;
	    cp.getValue(val);

	    valF = val;
	    mi_api_parameter_default( miTYPE_SCALAR, &valF );
	 }

	 if ( numChildren == 2 )
	   {
	     valF = 0.0f;
	     mi_api_parameter_default( miTYPE_SCALAR, &valF );
	   }
      }
   }
   else
   {
      MObject attrObj = p.attribute();

      if ( attrObj.hasFn( MFn::kEnumAttribute ) )
      {
	 short val;
	 p.getValue(val);
	 valI = val;
	 mi_api_parameter_default( miTYPE_INTEGER, &valI );
      }
      else if ( attrObj.hasFn( MFn::kNumericAttribute ) )
      {
	 double val;
	 p.getValue(val);

	 valF = val;
	 mi_api_parameter_default( miTYPE_SCALAR, &valF );
      }
      else if ( attrObj.apiType() == MFn::kDoubleLinearAttribute ||
		attrObj.apiType() == MFn::kFloatLinearAttribute )
      {
	 MDistance dst;
	 p.getValue(dst);
	 valF = dst.as( MDistance::uiUnit() );
	 mi_api_parameter_default( miTYPE_SCALAR, &valF );
      }
      else if (	attrObj.apiType() == MFn::kDoubleAngleAttribute ||
		attrObj.apiType() == MFn::kFloatAngleAttribute )
      {
	 MAngle ang;
	 p.getValue(ang);
	 valF = ang.as( MAngle::uiUnit() );
	 mi_api_parameter_default( miTYPE_SCALAR, &valF );
      }
      else if (	attrObj.apiType() == MFn::kTimeAttribute )
      {
	 MTime time;
	 p.getValue(time);
	 valF = time.as( MTime::uiUnit() );
	 mi_api_parameter_default( miTYPE_SCALAR, &valF );
      }
   }
}



miParameter* mrPhenomenon::write_output_material( const MPlug& p )
{
   MPlugArray plugs;
   p.connectedTo( plugs, true, false );
   if ( plugs.length() == 0 )
   {
      MString err = name;
      err += ": root material node is not connected.";
      LOG_ERROR(err);
      return false;
   }

   MObject sgObj = plugs[0].node();
   if ( !sgObj.hasFn( MFn::kShadingEngine ) )
   {
      MString err = name;
      err += ": rootMaterial plug not connected to a material.";
      LOG_ERROR( err );
      return false;
   }

   return mi_api_parameter_decl( miTYPE_MATERIAL,
				 MRL_MEM_STRDUP("outValue"), 0 );
}


miParameter* mrPhenomenon::write_output( const MFnDependencyNode& dep )
{
   MPlug p = dep.findPlug( "rootMaterial", true );
   miParameter* param = write_output_material( p );
   if ( param ) return param;

   p = dep.findPlug( "root", true );
   unsigned num = p.numConnectedElements();
   if ( num == 0 )
   {
      MString err = name;
      err += ": no root node found.";
      LOG_ERROR(err);
      return NULL;
   }

   MPlug cp = p.connectionByPhysicalIndex( 0 );
   MPlugArray plugs;
   cp.connectedTo( plugs, true, false );
   if ( plugs.length() == 0 )
   {
      MString err = name;
      err += ": root node is not connected.";
      LOG_ERROR(err);
      return NULL;
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
      MObject attrObj = fn.attribute(i);
      MPlug ap( shaderObj, attrObj );

      inc = 1;
      if ( ap.isCompound() ) inc += ap.numChildren();

      
      MObject attr = ap.attribute();
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
      return NULL;
   }

   if ( numIndices == 1 )
   {
      MObject attrObj = fn.attribute( indices[0] );
      MPlug p( shaderObj, attrObj );
      return mi_api_parameter_decl( miTYPE_COLOR,
				    MRL_MEM_STRDUP("outValue"), 0 );
   }
   else
   {
      for ( i = 0; i < numIndices; ++i )
      {
	 MObject attrObj = fn.attribute( indices[i] );
	 MPlug p( shaderObj, attrObj );
	 miParam_type type;
	 param = write_interface_parameter( p, type, param );
      }
      return param;
   }
}


miParameter* mrPhenomenon::write_interface_parameter( const MPlug& p,
						      miParam_type& type,
						      miParameter* lastParam )
{
   const MString& attrName = p.partialName( false, false, false,
					    false, false, true );
   char* attr = MRL_MEM_STRDUP( attrName.asChar() );

   miParameter* parent = NULL;
   if ( p.isArray() )
   {
      parent = mi_api_parameter_decl( miTYPE_ARRAY, attr, 0 );
   }

   miParameter* newparam;
   if ( p.isCompound() )
   {
      MPlug cp  = p.child(0);
      char comp = getComponent( cp );
      if ( comp == 'R' || comp == 'G' || comp == 'B' )
      {
	 type = miTYPE_COLOR;
	 newparam = mi_api_parameter_decl( miTYPE_COLOR, attr, 0 );
      }
      else
      {
	 type = miTYPE_VECTOR;
	 newparam = mi_api_parameter_decl( miTYPE_VECTOR, attr, 0 );
      }
   }
   else
   {
      MObject attrObj = p.attribute();
      if ( attrObj.hasFn( MFn::kMessageAttribute ) )
      {
	 if ( attrName == "lights" || attrName == "light" )
	 {
	    type = miTYPE_LIGHT;
	    newparam = mi_api_parameter_decl( miTYPE_LIGHT, attr, 0 );
	 }
	 else if ( attrName == "geometry" || attrName == "object" ||
		   attrName == "objects"  || attrName == "geometries" )
	 {
	    type = miTYPE_GEOMETRY;
	    newparam = mi_api_parameter_decl( miTYPE_GEOMETRY, attr, 0 );
	 }
	 else if ( attrName == "shader" || attrName == "shaders" )
	 {
	    type = miTYPE_SHADER;
	    newparam = mi_api_parameter_decl( miTYPE_SHADER, attr, 0 );
	 }
	 else
	 {
	    type = miTYPE_COLOR_TEX;
	    newparam = mi_api_parameter_decl( miTYPE_COLOR_TEX, attr, 0 );
	 }
      }
      else if ( attrObj.hasFn( MFn::kEnumAttribute ) )
      {
	 type = miTYPE_INTEGER;
	 newparam = mi_api_parameter_decl( miTYPE_INTEGER, attr, 0 );
      }
      else if ( attrObj.hasFn( MFn::kNumericAttribute ) )
      {
	 MFnNumericAttribute n( attrObj );
	    
	 switch( n.unitType() )
	 {
	    case MFnNumericData::kBoolean:
	       {
		  type = miTYPE_BOOLEAN;
		  newparam = mi_api_parameter_decl( miTYPE_BOOLEAN, attr, 0 );
		  break;
	       }
	    case MFnNumericData::kShort:
	    case MFnNumericData::kInt:
	       {
		  type = miTYPE_INTEGER;
		  newparam = mi_api_parameter_decl( miTYPE_INTEGER, attr, 0 );
		  break;
	       }
	    case MFnNumericData::kFloat:
	    case MFnNumericData::kDouble:
	       {
		  type = miTYPE_SCALAR;
		  newparam = mi_api_parameter_decl( miTYPE_SCALAR, attr, 0 );
		  break;
	       }
	    default:
	       mrTHROW("Unhandled numeric type");
	 }
      }
      else if ( attrObj.hasFn( MFn::kTypedAttribute ) )
      {
	 type     = miTYPE_STRING;
	 newparam = mi_api_parameter_decl( miTYPE_STRING, attr, 256 );
      }
      else if ( attrObj.apiType() == MFn::kDoubleLinearAttribute ||
		attrObj.apiType() == MFn::kFloatLinearAttribute  ||
		attrObj.apiType() == MFn::kDoubleAngleAttribute ||
		attrObj.apiType() == MFn::kFloatAngleAttribute ||
		attrObj.apiType() == MFn::kTimeAttribute )
      {
	 type = miTYPE_SCALAR;
	 newparam = mi_api_parameter_decl( miTYPE_SCALAR, attr, 0 );
      }
      else if ( attrObj.hasFn( MFn::kMessageAttribute ) )
      {
	 type = miTYPE_MATERIAL;
	 newparam = mi_api_parameter_decl( miTYPE_MATERIAL, attr, 0 );
      }
      else if ( attrObj.hasFn( MFn::kGenericAttribute ) )
      {
	 type = miTYPE_STRING;
	 newparam = mi_api_parameter_decl( miTYPE_STRING, attr, 256 );
      }
      else if ( attrObj.hasFn( MFn::kMentalRayTexture ) )
      {
	 type = miTYPE_COLOR_TEX;
	 newparam = mi_api_parameter_decl( miTYPE_COLOR_TEX, attr, 0 );
      }
      else
      {
	 MString err = "Unknown parameter type ";
	 err += attrName;
	 mrTHROW(err);
      }
   }
   
   if ( p.isArray() )
   {
      assert( parent != NULL );
      assert( newparam != NULL );
      mi_api_parameter_child( parent, newparam );
   }

   if ( lastParam )
   {
      assert( newparam != NULL );
      newparam = mi_api_parameter_append( lastParam, newparam );
   }
   
   assert( newparam != NULL );
   return newparam;
}



miParameter* mrPhenomenon::write_interface( const MFnDependencyNode& dep )
{
   MPlug p = dep.findPlug( "interface" );
   unsigned numConnectedElements = p.numConnectedElements();
   MPlugArray plugs;
   miParameter* inter = NULL;
   for ( unsigned i = 0; i < numConnectedElements; ++i )
   {
      MPlug cp = p.connectionByPhysicalIndex( i );
      cp.connectedTo( plugs, false, true );
      unsigned numDst = plugs.length();
      if ( numDst == 0 ) continue;
      for ( unsigned j = 0; j < numDst; ++j )
      {
	 miParam_type type;
	 inter = write_interface_parameter( plugs[j], type, inter );
	 write_default_value( plugs[j] );
      }
   }
   return inter;
}



void mrPhenomenon::write_one_root( miTag* tag,
				   miFunction_decl* func,
				   const MPlug& p )
{
   *tag = miNULLTAG;
   
   MPlugArray plugs;
   if ( p.isArray() )
   {
      unsigned i = p.numConnectedElements();
      do {
	 --i;
	 MPlug cp = p.connectionByPhysicalIndex( i );
	 cp.connectedTo( plugs, true, false );
	 if ( plugs.length() == 0 ) continue;
	 MFnDependencyNode fn( plugs[0].node() );
	 const char* n = fn.name().asChar();
	 if ( func->phen.root != miNULLTAG )
	 {
	    miTag newTag = MRL_ASSIGN(n);
	    *tag = mi_api_function_append( *tag, newTag );
	 }
	 else
	 {
	    *tag = MRL_ASSIGN(n);
	 }
      } while(i != 0);
   }
   else
   {
      p.connectedTo( plugs, true, false );
      unsigned num = plugs.length();
      if ( num == 0 ) return;

      for ( unsigned j = 0; j < num; ++j )
      {
	 MObject node = plugs[j].node();
	 MFnDependencyNode fn( node );
	 const char* n = fn.name().asChar();
	 if ( node.hasFn( MFn::kShadingEngine ) )
	 {
	   *tag = mi_api_name_lookup( MRL_MEM_STRDUP(n) );
	 }
	 else
	 {
	   *tag = MRL_ASSIGN( n );
	 }
      }
   }
}

void mrPhenomenon::write_roots( miFunction_decl* func, 
				const MFnDependencyNode& dep )
{
   MPlug p;
   MPlugArray plugs;

#define WRITE_ROOT( plug, root ) \
   p = dep.findPlug( #plug, true ); \
   write_one_root( &root, func, p );

//    WRITE_ROOT( rootLight, light );
   WRITE_ROOT( rootMaterial, root );
   if ( root == miNULLTAG )
   {
      WRITE_ROOT( root,        root );
   }
   WRITE_ROOT( rootLens,        lens );
   WRITE_ROOT( rootVolume,      volume );
   WRITE_ROOT( rootEnvironment, environment );
   WRITE_ROOT( rootGeometry,    geometry );
   WRITE_ROOT( rootOutput,      output );
#undef WRITE_ROOT

   // func->phen.n_subtags = 1; // what's this?
   func->phen.root        = root;
   func->phen.lens        = lens;
   func->phen.volume      = volume;
   func->phen.output      = output;
   func->phen.environment = environment;
   func->phen.geometry    = geometry;

   func->version = 1;
}


void mrPhenomenon::write()
{
   if ( options->exportFilter & mrOptions::kPhenomenizers )
      return;
   if ( written != mrNode::kNotWritten ) return;

   MFnDependencyNode dep( nodeRef() );

   miParameter* output = write_output(dep);
   miParameter* params = write_interface(dep);
   miFunction_decl* func = mi_api_phen_begin( output,
					      MRL_MEM_STRDUP(name.asChar()),
					      params
					      );
   write_roots( func, dep );
   tag = mi_api_phen_end();
   assert( tag != miNULLTAG );
   
   written = kWritten;
}
