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


void
mrShaderLightCustom::getConnectionNames(
					ConnectionMap& connNames,
					const MFnDependencyNode& dep
					)
{
   if ( light )
   {
      // If we are inheriting parameters, we also have to add possible
      // shading connections in light shape, so we can inherit those if
      // needed.
      MObject realObject = node();
      char realWritten   = written;
      
      // As we query light, getConnectionNames will find the connection to
      // ourselves (ie. the custom shader) and try to recreate it.
      // To avoid this recursion, we set written to already written
      // (not very efficient, thou)
      written    = kWritten; 
      nodeHandle = light->path.node();
      assert( node() != MObject::kNullObj );

      MStatus status;
      MFnDependencyNode fn( nodeRef(), &status );
      assert( status == MS::kSuccess );
      
      mrShader::getConnectionNames(connNames, fn);
      
      nodeHandle = realObject;
      written    = realWritten;
      assert( node() != MObject::kNullObj );
   }
   mrShader::getConnectionNames(connNames, dep);
}


void
mrShaderLightCustom::write_shader_parameters(
					     MFnDependencyNode& dep,
					     const ConnectionMap& connNames
					     )
{
   
   // Find parameters...
   unsigned numAttrs = dep.attributeCount();
   unsigned inc;

   const MObject& shaderObj = nodeRef();

   for ( unsigned i = START_ATTR; i < numAttrs; i += inc )
   {
      MObject attrObj = dep.attribute(i);
      MPlug ap( shaderObj, attrObj );
      
      inc = 1;
      if ( ap.isCompound() ) inc += ap.numChildren();

      MFnAttribute fnAttr(attrObj);
      bool isReadable = fnAttr.isReadable();
      bool isWritable = fnAttr.isWritable();
      if ( !isReadable || !isWritable ) continue;

      const MString& attrName = ap.partialName( false, true, true,
						false, true, true );
      if ( attrName == "miInherit" ) continue;
      
      inc = 0;
      MRL_PARAMETER( attrName.asChar() );

      if ( light )
      {
	 // If attribute is in light, inherit
         MStatus status;
	 MFnDagNode fn( light->path );
	 MPlug p = fn.findPlug( attrName, &status );
	 if ( status == MS::kSuccess )
	 {
	    unsigned idx = 0;
	    unsigned num = fn.attributeCount();
	    for ( ; idx < num ; ++idx )
	    {
	       if ( p == fn.attribute(idx) ) break;
	    }
	    unsigned origidx = idx;
	    handleParameter( idx, p, connNames );
	    i += idx-origidx;
	    continue;
	 }
      }
      handleParameter( i, ap, connNames );
   }
}

