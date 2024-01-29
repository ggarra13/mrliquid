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

void mrLightPhotons::write_properties( miLight* l )
{
   if ( causticPhotons > 0 || globIllPhotons > 0 )
   {
      l->energy.r = energy[0];
      l->energy.g = energy[1];
      l->energy.b = energy[2];
      l->caustic_store_photons = causticPhotons;
      l->caustic_emit_photons  = causticPhotonsEmit;
      l->global_store_photons  = globIllPhotons;
      l->global_emit_photons   = globIllPhotonsEmit;
      l->exponent = exponent;
   }
      
#if MAYA_API_VERSION < 850
   if ( areaType != kNone )
   {
      switch( areaType )
      {
	 case kRectangle:
	    l->area = miLIGHT_RECTANGLE;
	    l->primitive.rectangle.edge_u.x =
	    l->primitive.rectangle.edge_u.z =
	    l->primitive.rectangle.edge_v.y =
	    l->primitive.rectangle.edge_v.z = 0.0f;
	    l->primitive.rectangle.edge_u.y =
	    l->primitive.rectangle.edge_v.x = 2.0f;
	    break;
	 case kDisc:
	    l->area = miLIGHT_DISC;
	    l->primitive.disc.normal.x = l->primitive.disc.normal.y = 0.0f;
	    l->primitive.disc.normal.z = -1;
	    l->primitive.disc.radius = 1;
	    break;
	 case kSphere:
	    l->area = miLIGHT_SPHERE;
	    l->primitive.sphere.radius = 1;
	    break;
	 case kCylinder:
	    l->area = miLIGHT_CYLINDER;
	    l->primitive.cylinder.axis.x = 1;
	    l->primitive.cylinder.axis.y = l->primitive.cylinder.axis.z = 0.0f;
	    l->primitive.cylinder.radius = 1;
	    break;
	 case kUser:
	    mrTHROW("user area light not implemented yet");
	    break;
	 case kObject:
	    assert( areaObject != NULL );
	    l->area = miLIGHT_OBJECT;
	    l->primitive.object.object = areaObject->tag;
	    break;
	 default:
	    throw( "Unknown area light type" );
      }

      l->samples_u = areaSamplingU;
      l->samples_v = areaSamplingV;
      if ( areaLowLevel > 0 )
      {
	 l->low_level = areaLowLevel;
	 l->low_samples_u = areaLowSamplingU;
	 l->low_samples_v = areaLowSamplingV;
      }
      l->visible = (miBoolean) areaVisible;
   }
#endif

   mrLightShMap::write_properties( l );
}

void mrLightPhotons::write()
{
   mrLightShMap::write();
#if MAYA_API_VERSION < 850
   if ( options->progressive && areaType != kNone && 
        options->progressivePercent < 1.0f )
   {
      written = kIncremental;
   }
#endif
}
