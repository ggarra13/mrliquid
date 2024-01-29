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

#include "maya/MPlug.h"
#include "maya/MPlugArray.h"
#include "maya/MColor.h"
#include "maya/MVector.h"
#include "maya/MFnAttribute.h"

#include "mrShaderMayaShadow.h"
#include "mrShaderMayaPfxHair.h"
#include "mrAttrAux.h"
#include "mrOptions.h"



mrShaderMayaShadow::mrShaderMayaShadow( const MFnDependencyNode& fn,
					const mrShader* surf ) :
mrShaderMaya(fn, 0),
surface( surf )
{
   shaderName = "maya_shadow";
}


mrShaderMayaShadow* 
mrShaderMayaShadow::factory( const mrShader* surf, const char* container )
{
   
   MString name = surf->name;
   if ( container != NULL )
   {
      int idx = name.rindex('-');
      if ( idx > 0 )
      {
	 name = name.substring(0, idx-1);
	 name += ":shadow-";
	 name += container;
      }
   }
   else
   {
      name += ":shadow";
   }

   mrNodeList::iterator i = nodeList.find( name );

   if ( i != nodeList.end() )
   {
      return dynamic_cast<mrShaderMayaShadow*>( i->second );
   }

   MFnDependencyNode fn( surf->nodeRef() );
   mrShaderMayaShadow* s = new mrShaderMayaShadow( fn, surf );
   s->name = name;

   nodeList.insert( s );
   return s;
}

#define MAYA2MR_HAIR_SHADOW_BUG

void 
mrShaderMayaShadow::write_shader_parameters( MRL_FILE* f,
					     MFnDependencyNode& fn,
					     const ConnectionMap& connNames )
{

#ifdef  MAYA2MR_HAIR_SHADOW_BUG
  const mrShaderMayaPfxHair* hair = dynamic_cast< const mrShaderMayaPfxHair* >( surface );
  if ( (!hair) || (hair->miHairType != 3) )
    {
#endif

      TAB(2); 
      if ( surface->numOutAttrs > 1 )
	{
	  MRL_FPRINTF( f, "\"transparency\" = \"%s.outTransparency\",\n", 
		       surface->name.asChar() );
	}
      else
	{
	  MRL_FPRINTF( f, "\"transparency\" = \"%s\",\n", 
		       surface->name.asChar() );
	}

#ifdef  MAYA2MR_HAIR_SHADOW_BUG
    }
#endif

   bool connected = false;
   float tmpF = 0.5f;

   MStatus status;
   MPlug p = fn.findPlug("shadowAttenuation", true, &status);

   TAB(2); MRL_PUTS("\"shadowAttenuation\" ");
   if ( status == MS::kSuccess && p.isConnected() )
      connected = true;

   if (connected)
   {
      std::string key( p.name().asChar() );
      ConnectionMap::const_iterator it = connNames.find( key );
      if ( it != connNames.end() )
      {
	 const MString& conn = it->second;
	 MRL_FPRINTF(f, " = \"%s\",\n", conn.asChar());
      }
      else
      {
	 connected = false;
      }
   }
   //... not else...

   if ( !connected )
   {
      p.getValue(tmpF);
      MRL_FPRINTF( f, "%g,\n", tmpF );
   }

   //   tmpF = 0.5f;
   tmpF = 1.0f;
   p = fn.findPlug("translucenceDepth", true, &status);
   TAB(2); MRL_PUTS("\"translucenceDepth\" ");

   connected = false;
   if ( status == MS::kSuccess && p.isConnected() )
      connected = true;

   if (connected)
   {
      std::string key( p.name().asChar() );
      ConnectionMap::const_iterator it = connNames.find( key );
      if ( it != connNames.end() )
      {
	 const MString& conn = it->second;
	 MRL_FPRINTF(f, " = \"%s\"", conn.asChar());
      }
      else
      {
	 connected = false;
      }
   }
   if ( !connected )
   {
      p.getValue(tmpF);
      MRL_FPRINTF( f, "%g", tmpF );
   }


#if MAYA_API_VERSION < 900
   write_light_mode(f);
#endif

}




#ifdef GEOSHADER_H
#include "raylib/mrShaderMayaShadow.inl"
#endif
