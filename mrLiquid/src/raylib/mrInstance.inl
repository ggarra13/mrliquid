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



void mrInstanceGroup::write()
{
   if ( written == kWritten ) return;

   group->write();

   if ( written == kIncremental )
      mi_api_incremental( miTRUE );
   else
      mi_api_incremental( miFALSE );

   mi_api_instance_begin( MRL_MEM_STRDUP( name.asChar() ) );
   tag = mi_api_instance_end( MRL_MEM_STRDUP( group->name.asChar() ),
			      miNULLTAG, miNULLTAG);
   assert( tag != miNULLTAG );
   
   written = kWritten;
}


void mrInstance::write_matrices( miInstance* i )
{
   mrl_maya_matrix( i->tf.global_to_local, m );
   mi_matrix_invert(i->tf.local_to_global,
		    i->tf.global_to_local);
   if ( mt != m )
      mrl_maya_matrix( i->motion_transform, mt );
}


void mrInstance::write_properties( miInstance* i )
{
   if ( hide )
   {
      i->off = old_hide = true;
   }
   else
   {
      if ( hide != old_hide ) i->off = old_hide = false;
      
      // instance flags are +1 (see mray manual for how instance flags
      // are merged)
#ifdef RAY34
      i->visible = i->shadow = i->reflection = i->refraction =
      i->transparency = i->finalgather = visible + 1;
      i->face = 'a';
#else
      i->visible = i->shadow = i->trace = visible + 1;
#endif
   }
}


void mrInstance::write_instance()
{
   if      ( written == kWritten ) return;
   else if ( written == kIncremental )
      mi_api_incremental( miTRUE );
   else
      mi_api_incremental( miFALSE );

   miInstance* i = mi_api_instance_begin( MRL_MEM_STRDUP( name.asChar() ) );
   
   write_properties( i );
   if(!hide) write_matrices( i );

   tag = mi_api_instance_end( MRL_MEM_STRDUP( shape->name.asChar() ),
			      miNULLTAG, miNULLTAG);
   assert( tag != miNULLTAG );
}


void mrInstance::write()
{
   assert( shape != NULL );
   shape->write();
   if ( shape->written == kWritten )
     {
       write_instance();   
       written = kWritten;
     }
}


void mrInstanceLight::write()
{
   assert( shape != NULL );
   shape->write();
   if ( shape->written == kWritten && 
	! (options->exportFilter & mrOptions::kLightInstances) ) 
     {
      write_instance();
      written = kWritten;
     }
}

void mrInstanceCamera::write()
{
   assert( shape != NULL );
   shape->write();
   if ( shape->written == kWritten && 
	! (options->exportFilter & mrOptions::kCameraInstances) )
     {
       write_instance();
       written = kWritten;
     }
}
