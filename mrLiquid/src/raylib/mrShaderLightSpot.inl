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
mrShaderLightSpot::write_shader_parameters(
					   MFnDependencyNode& fn,
					   const ConnectionMap& connNames
					   )
{
   bool barnDoors = false, useDecayRegions = false;

   const int* i = kSpotAttrs;
   const MObject& shaderObj = nodeRef();

   for ( ; *i != 0; ++i )
   {
      MObject attrObj = fn.attribute(*i);
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
      
      
      unsigned idx = *i;
      MRL_PARAMETER( attrName.asChar() );
      handleParameter( idx, ap, connNames );
   }

}

