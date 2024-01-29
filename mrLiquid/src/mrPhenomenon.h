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



#include "mrShader.h"

//! Class used to spit out phenomena trees to disk
class mrPhenomenon : public mrShader
{
     friend class mrShader;
     
   public:
     virtual ~mrPhenomenon();
     virtual void write( MRL_FILE* f );
     
     //! Construct a new phenomenon from the node connected to a plug.
     //! Container is an optional string to append to the name of the
     //! shading network to make it unique, even if the shading network
     //! is shared in the maya scene.
     static mrPhenomenon* factory( const MPlug& p,
				   const char* container = NULL );
   protected:
     mrPhenomenon( const MFnDependencyNode& p );

     void write_type( MRL_FILE* f, const MPlug& p );

     bool write_output_material( MRL_FILE* f, const MPlug& cp );
     void write_output( MRL_FILE* f, const MFnDependencyNode& dep );

     void write_default_value( MRL_FILE* f, const MPlug& p );
     void write_interface_parameter( MRL_FILE* f, const MPlug& p, bool nodeName );
     void write_interface( MRL_FILE* f, const MFnDependencyNode& dep );

     void write_shader_network( MRL_FILE* f, const MPlug& p );
     void write_shaders( MRL_FILE* f, const MFnDependencyNode& dep );

     void write_one_root( MRL_FILE* f, const char* name, const MPlug& p );
     void write_roots( MRL_FILE* f, const MFnDependencyNode& dep );
     
     void write_header( MRL_FILE* f );
     void write_footer( MRL_FILE* f );

     void write_link_lines( MRL_FILE* f, const MPlug& p );
     void write_links( MRL_FILE* f, const MFnDependencyNode& dep );

     void write_ae_header( MRL_FILE* f );
     void write_ae_parameter( MRL_FILE* f, const MPlug& p );
     void write_ae_body( MRL_FILE* f );
     void write_ae_footer( MRL_FILE* f );
     void write_ae_template();
     
#ifdef GEOSHADER_H
   public:
     virtual void write();

   protected:
     miParameter* write_output_material( const MPlug& p );
     miParameter* write_output( const MFnDependencyNode& dep );

     void write_default_value( const MPlug& p );
     miParameter* write_interface_parameter( const MPlug& p,
					     miParam_type& type, 
					     miParameter* last );
     miParameter* write_interface( const MFnDependencyNode& dep );

     void write_one_root( miTag* tag,
			  miFunction_decl* func,  
			  const MPlug& p );
     void write_roots( miFunction_decl* func, const MFnDependencyNode& dep );

   protected:
	miTag		root;			/* root attachment point */
	miTag		lens;			/* optional lens shaders */
	miTag		output; 		/* optional output shaders */
	miTag		volume; 		/* optional volume shaders */
	miTag		environment;		/* optional environm. shaders*/
	miTag		geometry;		/* optional geometry shaders */
	miTag		contour_store;		/* opt'l contour store func */
	miTag		contour_contrast;	/* opt'l contour contrast f. */
#endif
};
