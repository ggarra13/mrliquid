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


#include "maya/MPlug.h"
#include "maya/MVector.h"
#include "maya/MAnimUtil.h"
#include "maya/MFnAttribute.h"
#include "maya/MTransformationMatrix.h"

#include "mrOptions.h"
#include "mrShaderIBLLight.h"

#include "mrAttrAux.h"

static const unsigned START_ATTR = 62;

// This list of attributes works without wildcards.
static const char* kAcceptList[] = {
"primaryVisibility",
"visibleInReflections",
"visibleInRefractions",
"visibleInEnvironment",
"visibleInFinalGather",
"alphaIsLuminance",
"mapping",
"texture",
"invert",
"color",
"envColorGain",
"envColorOffset",
"fgInvert",
"fgColorGain",
"fgColorOffset",
"fgFilterSize",
"infinite",
NULL
};


void mrShaderIBLLight::getPlacementMatrix()
{
   MTransformationMatrix t( shape.inclusiveMatrixInverse() );

   MStatus status;
   MFnDagNode fn( shape );
   bool infinite = true;
   MPlug p;

   GET( infinite );
   
   if ( infinite )
   {
      double v[3];
      MTransformationMatrix::RotationOrder order;
      // API bug:  this returns incorrect values if there is a 
      //           shear applied.
      t.getRotation( v, order, MSpace::kObject );
      t = MTransformationMatrix::identity;
      t.setRotation( v, order, MSpace::kObject );
   }

   placement = t.asMatrix();

   overrideFgColorFx = overrideEnvColorFx = true;
   GET_OPTIONAL( overrideFgColorFx );
   GET_OPTIONAL( overrideEnvColorFx );

   colorOffset = MColor(0,0,0,0);
   colorGain   = MColor(1,1,1,1);
   GET_RGB( colorOffset );
   GET_RGB( colorGain );
}


mrShaderIBLLight::mrShaderIBLLight( const MString& shader,
				    const MDagPath& lgt ) :
mrShader(shader, lgt),
shape( lgt )
{
   MFnDependencyNode fn( nodeHandle.objectRef() );
   name += ":shader";

   if ( !isAnimated )
   {
      MDagPath inst( lgt ); inst.pop();
      isAnimated = MAnimUtil::isAnimated( inst );
   }
}


void mrShaderIBLLight::forceIncremental()
{
   mrShader::forceIncremental();
}

void mrShaderIBLLight::setIncremental( bool sameFrame )
{
   if ( !isAnimated ) return;
   mrShader::setIncremental(sameFrame);
}

mrShaderIBLLight* mrShaderIBLLight::factory( const MString& shader,
					     const MDagPath& lgt )
{
   MString name = getMrayName( lgt );
   name += ":shader";
   mrNodeList::iterator i = nodeList.find( name );
   if ( i != nodeList.end() )
      return dynamic_cast<mrShaderIBLLight*>( i->second );

//    MStatus status;  MPlug p;
//    MFnDagNode fn( lgt );
//    int type;
//    GET( type );

//    if ( type == 0 ) // Image File
//    {
//       MString texture;
//       GET( texture );
      
//       texture = parseString(texture);
//       if ( texture == "" )
//       {
// 	 LOG_ERROR("IBL filename is empty.  Ignored.");
// 	 return NULL;
//       }
//    }

   mrShaderIBLLight* l = new mrShaderIBLLight( shader, lgt );
   nodeList.insert( l );
   return l;
}


void mrShaderIBLLight::write( MRL_FILE* f )
{
   MPlug p;  MStatus status;
   MFnDependencyNode fn( nodeHandle.objectRef() );
   GET( type );

   if ( type == 0 ) // Image File
   {
      MString texture;
      GET( texture );
      
      texture = parseString(texture);

      if ( textureName != texture || written != kWritten )
      {
	 textureName = texture;

	 if (options->makeMipMaps) 
	   {
#ifdef USE_OPENEXR
	     Extrapolation wrapU = PERIODIC, wrapV = PERIODIC;
	     makeExr( texture, wrapU, wrapV );
#else
	     makeMipMap( texture );
#endif
	   }
	 
	 switch( written )
	 {
	    case kWritten:
	       return; break;
	    case kIncremental:
	       MRL_PUTS( "incremental ");
	 }
	 MRL_FPRINTF(f, "local color texture \"%s:tex\" \"%s\"\n",
		 name.asChar(), texture.asChar());
      }
   }
   
   mrShader::write(f);
}



void
mrShaderIBLLight::write_shader_parameters(
					  MRL_FILE* f,
					  MFnDependencyNode& fn,
					  const ConnectionMap& connNames
					  )
{

   getPlacementMatrix();


   // Find parameters...
   unsigned numAttrs = fn.attributeCount();
   bool comma = false;  // Whether to print a comma before attribute
   unsigned inc;

   //  Hmmm.... maya2mr uses a "placement" attribute to pass the
   //  matrix of the instance of the IBL light.  However, maya2mr does
   //  only passes the rotation of such a matrix, not the scale or translation
   //  as IBL light is assumed to be an infinite dome.
  
   
   const MObject& shaderObj = nodeHandle.objectRef();
   for ( unsigned i = START_ATTR; i < numAttrs; i += inc )
   {
      const MObject& attrObj = fn.attribute(i);
      MPlug ap( shaderObj, attrObj );

      inc = 1;
      if ( ap.isCompound() ) inc += ap.numChildren();
      
      if ( ap.isProcedural() ) continue;


      MFnAttribute fnAttr(attrObj);
      bool isReadable = fnAttr.isReadable();
      bool isWritable = fnAttr.isWritable();
      if ( !isReadable || !isWritable ) continue;
      
      
      const MString& attrName = ap.partialName( false, true, true,
						false, true, true );
      
      // Test if this is an attribute to ignore
      const char* attrAsChar = attrName.asChar();
      const char**  testAttr = kAcceptList;
      bool ignore = true;
      for ( ; *testAttr != NULL ; ++testAttr )
      {
	 if ( strcmp( attrAsChar, *testAttr ) == 0 ) {
	    ignore = false;
	    break;
	 }
      }
      if ( ignore ) continue;

      
      if ( type == 0 )
      {
	 if ( strcmp( attrAsChar, "color" ) == 0 )
	    continue;
      }
      else
      {
	 if ( strcmp( attrAsChar, "texture" ) == 0 )
	    continue;
      }
      
      
      if ( comma != 0 ) MRL_PUTS( ",\n");
      comma = true;
      
      if ( strcmp( attrAsChar, "texture" ) == 0 )
      {
	 MRL_FPRINTF(f, "\"%s\" \"%s:tex\"", attrAsChar, name.asChar() );
	 continue;
      }

      if ( strcmp( attrAsChar, "primaryVisibility" ) == 0 )
      {
	 attrAsChar = "visible";
      }

      if ( strcmp( attrAsChar, "envColorGain" ) == 0 )
	{
	  attrAsChar = "colorGain";
	  if ( !overrideEnvColorFx )
	    {
	      MRL_FPRINTF( f, "\"%s\" %f %f %f", attrAsChar,
			   colorGain.r, colorGain.g, colorGain.b );
	      continue; 
	    }
	}

      if ( strcmp( attrAsChar, "envColorOffset" ) == 0 )
	{
	  attrAsChar = "colorOffset";
	  if ( !overrideEnvColorFx )
	    {
	      MRL_FPRINTF( f, "\"%s\" %f %f %f", attrAsChar,
			   colorOffset.r, colorOffset.g, colorOffset.b ); 
	      continue; 
	    }
	}

      if ( strcmp( attrAsChar, "fgColorGain" ) == 0 )
	{
	  if ( !overrideFgColorFx )
	    {
	      MRL_FPRINTF( f, "\"%s\" %f %f %f", attrAsChar,
			   colorGain.r, colorGain.g, colorGain.b );
	      continue; 
	    }
	}

      if ( strcmp( attrAsChar, "fgColorOffset" ) == 0 )
	{
	  if ( !overrideFgColorFx )
	    {
	      MRL_FPRINTF( f, "\"%s\" %f %f %f", attrAsChar,
			   colorOffset.r, colorOffset.g, colorOffset.b ); 
	      continue; 
	    }
	}

      inc = 0;
      MRL_FPRINTF(f, "\"%s\"", attrAsChar );
      handleParameter( f, i, ap, connNames );
   }

   MRL_PUTS( ",\n\"placement\" " );
   write_matrix_parameter( f, placement );
}




#ifdef GEOSHADER_H
#include "raylib/mrShaderIBLLight.inl"
#endif
