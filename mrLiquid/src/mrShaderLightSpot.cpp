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
#include "maya/MFnAttribute.h"

#include "mrShaderLightSpot.h"
#include "mrShaderLightAttrs.h"


mrShaderLightSpot::mrShaderLightSpot( const MString& shader,
				      const MDagPath& lgt ) :
mrShader(shader, lgt)
{
   name += ":shader";
   
#ifdef SPIT_ATTR
   const MObject& shaderObj = nodeRef();
   MFnDependencyNode fn( shaderObj );
   unsigned id = fn.typeId().id();
   cerr << "SHADER: " << shaderName << " id: " << id << endl;
   unsigned numAttrs = fn.attributeCount();
   int inc;
   for ( unsigned i = START_ATTR; i < numAttrs; i += inc )
   {
      const MObject& attrObj = fn.attribute(i);
      MPlug ap( shaderObj, attrObj );

      inc = 1;
      if ( ap.isCompound() ) inc += ap.numChildren();
      
      MString& attrName = ap.partialName( false, true, true,
					  false, true, true );
      
      cerr << i << ",  // " << attrName << endl;
   }
#endif
}



mrShaderLightSpot* mrShaderLightSpot::factory( const MString& shader,
					       const MDagPath& lgt )
{
   MString name = getMrayName( lgt );
   name += ":shader";
   mrNodeList::iterator i = nodeList.find( name );
   if ( i != nodeList.end() )
      return dynamic_cast<mrShaderLightSpot*>( i->second );
   mrShaderLightSpot* l = new mrShaderLightSpot( shader, lgt );
   nodeList.insert( l );
   return l;
}



void 
mrShaderLightSpot::write_shader_parameters( MRL_FILE* f,
					    MFnDependencyNode& fn,
					    const ConnectionMap& connNames )
{
   bool barnDoors = false, useDecayRegions = false;
   
   bool comma = false;  // Whether to print a comma before attribute
   
   const MObject& shaderObj = nodeRef();
   const int* i = kSpotAttrs;
   for ( ; *i != 0; ++i )
   {
      const MObject& attrObj = fn.attribute(*i);
      MPlug ap( shaderObj, attrObj );

      const MString& attrName = ap.partialName( false, true, true,
						false, true, true );
      
      if ( ! barnDoors )
      {
	 if ( *i == kBarnDoorAttr )
	 {
	    ap.getValue( barnDoors );
	 }
	 else
	 {
	    if ( *i > kBarnDoorAttr && *i < kBarnDoorLast )
	       continue;  // ignore barndoors as barndoor is off
	 }
      }
      
      if ( ! useDecayRegions )
      {
	 if ( *i == kUseDecayRegions )
	 {
	    ap.getValue( useDecayRegions );
	 }
	 else
	 {
	    if ( *i > kUseDecayRegions && *i < kUseDecayRegionsLast )
	       continue;  // ignore decay regions as useDecayRegion is off
	 }
      }
      
      if ( comma ) MRL_PUTS( ",\n");
      comma = true;
      

      TAB(2);
      MRL_FPRINTF(f, "\"%s\"", attrName.asChar() );

      // Shadow maps are special, as they can be either useDepthShadowMaps or
      // mray's shadowMap attribute, so we check if mray's attribute is on.
      if ( *i == kShadowMap )
      {
	 MStatus status;
	 MPlug sp = fn.findPlug("shadowMap", &status);
	 if ( status == MS::kSuccess )
	 {
	    bool shadow;
	    sp.getValue(shadow);
	    if (shadow)
	    {
	       MRL_PUTS(" on");
	       continue;
	    }
	 }
      }

      unsigned idx = *i;
      handleParameter( f, idx, ap, connNames );
   }
   
}




#ifdef GEOSHADER_H
#include "raylib/mrShaderLightSpot.inl"
#endif
