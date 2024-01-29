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
#include "maya/MColor.h"

#include "mrImagePlaneSG.h"
#include "mrShader.h"
#include "mrAttrAux.h"


mrImagePlaneSG::mrImagePlaneSG( const MFnDependencyNode& fn ) :
mrNode( fn ),
nodeHandle( fn.object() ),
texture( NULL )
{
   name += ":material";
   getData();
}



mrImagePlaneSG* mrImagePlaneSG::factory( const MFnDependencyNode& fn )
{
   const MString& name = fn.name() + ":material";
   mrNodeList::iterator i = nodeList.find( name );
   if ( i != nodeList.end() )
      return dynamic_cast<mrImagePlaneSG*>( i->second );
   mrImagePlaneSG* p = new mrImagePlaneSG( fn );
   nodeList.insert( p );
   return p;
}



void mrImagePlaneSG::getData()
{
   MFnDependencyNode fn( nodeRef() );

   MStatus status; MPlug p;
   short type;
   GET( type );
   if ( type == 1 )
   {
      p = fn.findPlug("sourceTexture", &status);
      if ( status == MS::kSuccess && p.isConnected() )
      {
	 texture = mrShader::factory( p, NULL );
      }
      else
      {
	 LOG_WARNING( name + ": no texture connected");
	 texture = NULL;
      }
   }
   else
   {
      texture = NULL;
   }
}


void mrImagePlaneSG::setIncremental( bool sameFrame )
{
   getData();
   if ( texture ) texture->setIncremental( sameFrame );
   if ( written == kWritten ) written = kIncremental;
}


void mrImagePlaneSG::write( MRL_FILE* f )
{
   if ( texture ) texture->write(f);

   if ( written == kWritten )
      return;

   MFnDependencyNode fn( nodeRef() );

   MStatus status; MPlug p;
   short type;
   GET( type );
   if ( type == 0 )
   {
      MString imageName;
      GET( imageName );
      
      if ( written == kIncremental )
	 MRL_PUTS("incremental ");
      MRL_FPRINTF(f, "local color texture \"%s:tex\" \"%s\"\n",
	      name.asChar(), imageName.asChar() );
      NEWLINE();
   }
   MColor colorGain, colorOffset;
   short displayMode;
   GET(displayMode);
   GET_RGB( colorGain );
   GET_RGB( colorOffset );
   float alphaGain;
   GET( alphaGain );
   bool visibleInReflections, visibleInRefractions;
   GET( visibleInReflections );
   GET( visibleInRefractions );
   
   if ( written == kIncremental )
      MRL_PUTS("incremental ");
   MRL_FPRINTF(f, "material \"%s\"\n", name.asChar() );
   TAB(1); MRL_PUTS("\"maya_imageplane\" (\n");
   TAB(2); MRL_FPRINTF(f, "\"type\" %d,\n", type);
   TAB(2);
   if ( type == 0 )
      MRL_FPRINTF(f, "\"imageName\" \"%s:tex\",\n", name.asChar());
   else if ( texture )
   {
      MString attr = texture->name;
      if ( texture->numOutAttrs > 1 )
	 attr += ".outColor";
      MRL_FPRINTF(f, "\"sourceTexture\" = \"%s\",\n", attr.asChar());
   }

   TAB(2);
   MRL_FPRINTF(f, "\"displayMode\" %d,\n", displayMode);

   TAB(2);
   MRL_FPRINTF(f, "\"colorGain\" %g %g %g,\n",
	   colorGain.r, colorGain.g, colorGain.b );
   TAB(2);
   MRL_FPRINTF(f, "\"colorOffset\" %g %g %g,\n",
	   colorOffset.r, colorOffset.g, colorOffset.b );
   TAB(2);
   MRL_FPRINTF(f, "\"alphaGain\" %g,\n", alphaGain );
   TAB(2);
   MRL_FPRINTF(f, "\"visibleInReflections\" %s,\n",
	   visibleInReflections? "on" : "off" );
   TAB(2);
   MRL_FPRINTF(f, "\"visibleInRefractions\" %s\n", 
	   visibleInRefractions? "on" : "off" );
   TAB(1);
   MRL_PUTS(")\n");
   MRL_PUTS("end material\n");
   NEWLINE();

   written = kWritten;
}



#ifdef GEOSHADER_H
#include "raylib/mrImagePlaneSG.inl"
#endif


