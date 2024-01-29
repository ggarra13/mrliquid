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
#include "maya/MFnAttribute.h"

#include "mrShaderMayaUVChooser.h"
#include "mrAttrAux.h"


mrShaderMayaUVChooser::mrShaderMayaUVChooser( const MFnDependencyNode& p ) :
mrShaderMaya( p, 0 )
{
}


void 
mrShaderMayaUVChooser::write_shader_parameters( MRL_FILE* f,
						MFnDependencyNode& dep,
						const ConnectionMap& conn )
{
   MStatus status;
   MPlug p = dep.findPlug("uvSets", true, &status);
   if ( status != MS::kSuccess )
      return;
   MRL_PUTS("\"uvSets\" [");

   unsigned numConnected = p.numConnectedElements();
   for (unsigned i = 0; i < numConnected; ++i)
   {
      MPlug ep = p.connectionByPhysicalIndex( i );
      MPlugArray plugs;
      if (! ep.connectedTo( plugs, true, false ) )
	 continue;

      MPlug op = plugs[0];
      if ( !op.isChild() ) continue;

      const MObject& node = op.node();
      if ( !node.hasFn( MFn::kDagNode ) ) continue;

      MPlug pp = op.parent();
      if ( !pp.isElement() ) continue;

      MDagPath dag;
      MDagPath::getAPathTo( node, dag );
      dag.pop(); // go to instance

      MString objname = getMrayName( dag );

      if ( i > 0 ) MRL_PUTS(",\n");
      MRL_PUTS("{\n");
      MRL_FPRINTF(f, "\"object\" \"%s\",\n", objname.asChar());
      unsigned idx = pp.logicalIndex();
      MRL_FPRINTF(f, "\"index\" %d\n", idx );
      MRL_PUTS("}");
   }

   MRL_PUTC(']');
}

#ifdef GEOSHADER_H
#include "raylib/mrShaderMayaUVChooser.inl"
#endif
