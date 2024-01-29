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
#include "maya/MFnLight.h"

#include "mrLightPhotons.h"
#include "mrInstance.h"
#include "mrAttrAux.h"

#include "mrInstanceObject.h"
#include "mrOptions.h"


mrLightPhotons::~mrLightPhotons()
{}

void mrLightPhotons::getData()
{ 
   DBG(name << ": mrLightPhotons::getData");
   MStatus status;
   MPlug p;

   // photons
   MFnLight fn( path );
   bool emitPhotons = false;
   GET_OPTIONAL( emitPhotons );

   causticPhotons = globIllPhotons = 0;
   if ( emitPhotons )
   {
      GET_RGB( energy );
      GET( exponent );
      GET( causticPhotons );
      GET( globIllPhotons );
      GET_OPTIONAL( causticPhotonsEmit );
      GET_OPTIONAL( globIllPhotonsEmit );
   }

#if MAYA_API_VERSION < 850
   bool areaLight = false;
   GET_OPTIONAL( areaLight );
   areaType = kNone;
   if ( areaLight )
   {
      GET( areaType );
      areaType += 1;
      GET_OPTIONAL( areaSamplingU );
      GET_OPTIONAL( areaSamplingV );
      areaLowLevel = 0;
      GET_OPTIONAL( areaLowLevel );
      if ( areaLowLevel > 0 )
      {
	 GET(areaLowSamplingU);
	 if (areaLowSamplingU > areaSamplingU )
	    areaLowSamplingU = areaSamplingU-1;
	 GET(areaLowSamplingV);
	 if (areaLowSamplingV > areaSamplingV )
	    areaLowSamplingV = areaSamplingV-1;
      }
      GET( areaVisible );
   }
#endif

}



mrLightPhotons::mrLightPhotons( const MDagPath& light ) :
mrLightShMap( light ),
areaObject(NULL),
causticPhotons( 0 ),
globIllPhotons( 0 ),
causticPhotonsEmit( 0 ),
globIllPhotonsEmit( 0 )
#if MAYA_API_VERSION < 850
,areaType( kNone )
#endif
{
   mrLightPhotons::getData();
}


void mrLightPhotons::forceIncremental()
{
   DBG(name << " mrLightPhotons::forceIncremental");
   mrLightPhotons::getData();
   mrLightShMap::forceIncremental();
}

void mrLightPhotons::setIncremental( bool sameFrame )
{
   DBG(name << " mrLightPhotons::setIncremental");
   mrLightPhotons::getData();
   mrLightShMap::setIncremental(sameFrame);
}

void mrLightPhotons::write( MRL_FILE* f )
{
   mrLightShMap::write(f);
#if MAYA_API_VERSION < 850
   if ( options->progressive && areaType != kNone && 
        options->progressivePercent < 1.0f )
   {
      written = kIncremental;
   }
#endif
}

void mrLightPhotons::write_properties( MRL_FILE* f )
{
   if ( causticPhotons > 0 || globIllPhotons > 0 )
   {
      MRL_FPRINTF(f,"energy %g %g %g\n",
	      energy[0], energy[1], energy[2]);
      if ( causticPhotons != 10000 || causticPhotonsEmit != 0 )
      {
	 MRL_FPRINTF(f, "caustic photons %d", causticPhotons);
	 if ( causticPhotonsEmit > 0 ) MRL_FPRINTF(f, " %d", causticPhotonsEmit);
	 MRL_PUTC('\n');
      }
      if ( globIllPhotons != 100000 || globIllPhotonsEmit != 0 )
      {
	 MRL_FPRINTF(f,"globillum photons %d", globIllPhotons);
	 if ( globIllPhotonsEmit > 0 ) MRL_FPRINTF(f, " %d", globIllPhotonsEmit);
	 MRL_PUTC('\n');
      }
      MRL_FPRINTF(f,"exponent %g\n", exponent);
   }

#if MAYA_API_VERSION < 850   
   if ( areaType != kNone )
   {
      //@todo: there's an area edge and area normal
      //       in light's attributes.  What for?

      switch( areaType )
      {
	 case kRectangle:
	    MRL_PUTS("rectangle 0 2 0 2 0 0"); 
	    break;
	 case kDisc:
	    MRL_PUTS("disc 0 0 -1 1"); 
	    break;
	 case kSphere:
	    MRL_PUTS("sphere 1"); 
	    break;
	 case kCylinder:
	    MRL_PUTS("cylinder 1 0 0 1" ); 
	    break;
	 case kUser:
	    MRL_PUTS("user");
	    break;
	 case kObject:
	    assert( areaObject != NULL );
	    MRL_FPRINTF(f,"object \"%s\"", areaObject->name.asChar());
	    break;
	 default:
	    throw( "Unknown area light type" );
      }

      if ( options->progressive )
      {
	 int u = (int) (areaSamplingU * options->progressivePercent);
	 int v = (int) (areaSamplingV * options->progressivePercent);
	 if ( u < 1 ) u = 1;
	 if ( v < 1 ) v = 1;
	 MRL_FPRINTF(f, "\n%d %d", u, v );
	 if ( areaLowLevel > 0 )
	 {
	    u = (int) (areaLowSamplingU * options->progressivePercent);
	    v = (int) (areaLowSamplingV * options->progressivePercent);
	    if ( u < 1 ) u = 1;
	    if ( v < 1 ) v = 1;
	    MRL_FPRINTF(f, " %d %d %d", areaLowLevel, u, v );
	 }
      }
      else
      {
	 MRL_FPRINTF(f, "\n%d %d", areaSamplingU, areaSamplingV );
	 if ( areaLowLevel > 0 )
	 {
	    MRL_FPRINTF(f, " %d %d %d", areaLowLevel, 
		    areaLowSamplingU, areaLowSamplingV );
	 }
      }
      MRL_FPRINTF(f, "\n" );
      if ( areaVisible ) MRL_PUTS( "visible\n");
   }
   else
   {
      if ( written == kIncremental )
	 MRL_PUTS("disc\n");  // this reverts the light to non-area light
   }
#endif

   mrLightShMap::write_properties(f);
}




#ifdef GEOSHADER_H
#include "raylib/mrLightPhotons.inl"
#endif
