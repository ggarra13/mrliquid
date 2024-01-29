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


#include <cassert>
#include <cstring>

#include "maya/MPlug.h"
#include "maya/MFnDagNode.h"

#ifndef mrLightMapOptions_h
#include "mrLightMapOptions.h"
#endif

#ifndef mrOptions_h
#include "mrOptions.h"
#endif

#ifndef mrShaderMaya_h
#include "mrShaderMaya.h"
#endif

#ifndef mrShaderMayaShadow_h
#include "mrShaderMayaShadow.h"
#endif

#ifndef mrBakeSet_h
#include "mrBakeSet.h"
#endif

#ifndef mrParticles_h
#include "mrParticles.h"
#endif

#ifndef mrShaderMayaContour_h
#include "mrShaderMayaContour.h"
#endif


#ifndef mrShadingGroup_h
#include "mrShadingGroup.h"
#endif

#ifndef mrShadingGroupParticles_h
#include "mrShadingGroupParticles.h"
#endif

#ifndef mrPhenomenon_h
#include "mrPhenomenon.h"
#endif

extern MDagPath      currentObjPath;
extern MDagPath     currentInstPath;  // current instance being translated
extern mrLightMapOptions* lightMapOptions;
extern mrShadingGroup*    currentSG;   // current shading group


static const unsigned kDisplacementShaderId = 1380209480;
static const unsigned kOceanShaderId        = 1380929619;


mrShadingGroup::~mrShadingGroup()
{
   free(container);
   // No need to delete any of the shader pointers.  mrHash will clean up.
}

bool mrShadingGroup::newDisplacement() const
{
  return (displace && displace->written == kIncremental); 
}

void mrShadingGroup::getData()
{
   currentSG = this;
   MStatus status; MPlug p;
   MFnDependencyNode fn( nodeRef() );

   surface = displace = volume = shadow = environment = 
   photon  = photonvol = lightmap = contour = NULL;
   miExportShadowShader = false;
   miContourEnable = false;

#if MAYA_API_VERSION >= 850

   p = fn.findPlug("miMaterial", &status);
   if ( status == MS::kSuccess && p.isConnected() )
     {
       material = mrPhenomenon::factory( p, container );
     }


   miCutAwayOpacity = 0.0f;
   GET_OPTIONAL( miCutAwayOpacity );
#endif

   p = fn.findPlug( "miMaterialShader", true, &status );
   if ( status == MS::kSuccess && p.isConnected() )
   {
      DBG("created mray surface");
      surface = mrShader::factory( p, container );
      assert( surface != NULL );
      GET( miExportShadingEngine );
      GET_OPTIONAL( miExportShadowShader );
   }
   else
   {
      p = fn.findPlug( "surfaceShader", true, &status );
      if ( status == MS::kSuccess && p.isConnected() )
      {
	 miExportShadingEngine = true;
	 surface = mrShader::factory( p, container );
      }
   }
   
   p = fn.findPlug( "miDisplacementShader", true, &status );
   if ( status == MS::kSuccess && p.isConnected() )
   {
      displace = mrShader::factory( p, container );
   }
   else
   {
      p = fn.findPlug( "displacementShader", true, &status );
      if ( status == MS::kSuccess && p.isConnected() )
      {
	 maya_displace = true;
	 displace = mrShader::factory( p, container );
      }
   }

   p = fn.findPlug( "miVolumeShader", true, &status );
   if ( status == MS::kSuccess && p.isConnected() )
   {
      volume = mrShader::factory( p, container );
      if ( volume->numOutAttrs > 1 )
	 GET(miExportVolumeSampler);
   }
   else
   {
      p = fn.findPlug( "volumeShader", true, &status );
      if ( status == MS::kSuccess && p.isConnected() )
      {
	 miExportVolumeSampler = true;
	 volume = mrShader::factory( p, container );
      }
   }

   p = fn.findPlug( "miShadowShader", true, &status );
   if ( status == MS::kSuccess && p.isConnected() )
      shadow = mrShader::factory( p, container );
   
   p = fn.findPlug( "miPhotonShader", true, &status );
   if ( status == MS::kSuccess && p.isConnected() )
      photon = mrShader::factory( p, container );
   
   p = fn.findPlug( "miPhotonVolumeShader", true, &status );
   if ( status == MS::kSuccess && p.isConnected() )
      photonvol = mrShader::factory( p, container );
   
   p = fn.findPlug( "miEnvironmentShader", true, &status );
   if ( status == MS::kSuccess && p.isConnected() )
      environment = mrShader::factory( p, container );

   if ( options->lightMap )
   {
      MString lightmapName = name;
      lightmapName += "-";

      MString object = currentInstPath.partialPathName();
      lightmapName += object;
      lightmapName += ":lightmap";
      DBG("object: " << object);

      mrObjectToBakeSet::iterator bset;
      bset = lightMapOptions->obj2bakeset.find( object );
      
      if ( bset == lightMapOptions->obj2bakeset.end() )
      {
	 DBG("no bake set - using defaults");
	 lightmap = mrBakeSet::toDefaultLightMap( lightmapName );
      }
      else
      {
	 DBG("bake set");
	 mrBakeSet* b = bset->second;
	 lightmapName += ":lightmap";
	 lightmap = b->toLightMap( lightmapName );
      }
   }
   else
   {
      p = fn.findPlug( "miLightMapShader", true, &status );
      if ( status == MS::kSuccess && p.isConnected() )
	 lightmap = mrShader::factory( p, container );
   }
   
#if MAYA_API_VERSION >= 650
   if ( options->contourEnable )
   {
      bool miContourEnable = false;
      GET_OPTIONAL( miContourEnable );
      if ( miContourEnable )
      {
	 contour = mrShaderMayaContour::factory( this, container );
      }
   }
   else
   {
      p = fn.findPlug( "miContourShader", true, &status );
      if ( status == MS::kSuccess && p.isConnected() )
	 contour = mrShader::factory( p, container );
   }
#endif

   if ( !shadow && miExportShadingEngine && surface &&
	surface->numOutAttrs > 1 )
   {
      shadow = mrShaderMayaShadow::factory( surface, container );
   }

   currentSG = NULL;
   GET_OPTIONAL( miOpaque );
}


mrShadingGroup::mrShadingGroup( const MString& name, const char* contain ) :
mrInstanceBase( name ),
container( NULL ),
surface(NULL),
displace(NULL),
shadow(NULL),
environment(NULL),
photon(NULL),
volume(NULL),
lightmap(NULL),
contour(NULL),
photonvol(NULL),
miOpaque(false),
miExportShadingEngine(false),
miExportVolumeSampler(false),
maya_particle_sprite(false),
maya_particle_hair(false),
maya_particle(false),
maya_displace(false)
{
   // Store container for possible IPR changes
   if (contain) container = STRDUP(contain);
}


mrShadingGroup::mrShadingGroup( const MFnDependencyNode& fn,
				const char* contain ) :
mrInstanceBase(fn),
nodeHandle( fn.object() ),
container( NULL ),
material(NULL),
surface(NULL),
displace(NULL),
shadow(NULL),
environment(NULL),
photon(NULL),
volume(NULL),
lightmap(NULL),
contour(NULL),
photonvol(NULL),
miAlphaMode(0),
miCutAwayOpacity(0),
miOpaque(false),
miExportShadingEngine(false),
miExportVolumeSampler(false),
maya_particle_sprite(false),
maya_particle_hair(false),
maya_particle(false),
maya_displace(false)
{
   // Store container for possible IPR changes
   if (contain) container = STRDUP(contain);
   getData();
}


mrShadingGroup* mrShadingGroup::factory(const MString& name,
					const char* contain )
{
   mrShadingGroupList::iterator i = sgList.find( name ); 
   mrShadingGroup* s;
   if ( i != sgList.end() )
   {
      s = dynamic_cast<mrShadingGroup*>( i->second );      
   }
   else
   {
      s = new mrShadingGroup( name, contain );
      s->name = name;
      sgList.insert( s );
   }
   return s;
}


mrShadingGroup* mrShadingGroup::factory(const MFnDependencyNode& fn, 
					const char* contain,
					const mrShape* shape )
{
   MString name = fn.name();
   
   bool isParticle = ( dynamic_cast< const mrParticles* >( shape ) != NULL );
   if ( isParticle )
   {
      const mrParticles* part = (const mrParticles*) shape;
      if ( part->isVolumetric() )
      {
	 name += ":";
	 name += shape->path.partialPathName();
      }
   }
   else if ( options->lightMap )
   {
      name += "-";
      name += currentInstPath.partialPathName();
   }
   else if ( contain )
   {
      name += "-";
      name += contain;
   }
   
   mrShadingGroupList::iterator i = sgList.find( name ); 
   mrShadingGroup* s;
   if ( i != sgList.end() )
   {
      s = dynamic_cast<mrShadingGroup*>( i->second );      
   }
   else
   {
      if ( isParticle )
      {
	 s = new mrShadingGroupParticles( fn, contain );
      }
      else
      {
	 s = new mrShadingGroup( fn, contain );
      }
      s->name = name;
      sgList.insert( s );
   }


   return s;
}



void mrShadingGroup::forceIncremental()
{
   DBG( name << "  mrShadingGroup::forceIncremental");
   getData();

#define incShader(x) if ( x ) x->forceIncremental();

   incShader( material );
   incShader( surface );
   
   if ( options->displacementShaders )
      incShader( displace );
   if ( options->volumeShaders )
      incShader( volume );
   if ( options->shadow != mrOptions::kShadowOff )
      incShader( shadow );
   incShader( environment );
   if ( options->caustics || options->globalIllum )
   {
      incShader( photon );
      incShader( photonvol );
   }
   incShader( lightmap );
   incShader( contour );
   mrNode::forceIncremental();
#undef incShader
}



void mrShadingGroup::setIncremental( bool sameFrame )
{
   DBG( name << " mrShadingGroup::setIncremental");
   
#define incShader(x) if ( x ) x->setIncremental( sameFrame );

   incShader( material );
   incShader( surface );
   
   if ( options->displacementShaders )
      incShader( displace );
   if ( options->volumeShaders )
      incShader( volume );
   if ( options->shadow != mrOptions::kShadowOff )
      incShader( shadow );
   incShader( environment );
   if ( options->caustics || options->globalIllum )
   {
      incShader( photon );
      incShader( photonvol );
   }
   incShader( lightmap );
   incShader( contour );
#undef incShader
   // Note: we do not set this node to incremental, as shading groups
   //       do not change between frames.
}


void mrShadingGroup::write( MRL_FILE* f )
{
   if ( options->exportFilter & mrOptions::kMaterials )
      return;

   currentSG = this;

   if (! material )
     {
#define writeShader( x ) if ( x ) x->write(f);
       writeShader( surface );

       if ( miExportShadingEngine && (photon == NULL) &&
	    ( options->caustics || options->globalIllum ) )
	 {
	   mrShaderMaya* m = dynamic_cast<mrShaderMaya*>( surface );
	   if ( m ) 
	     {
	       m->written = written;
	       m->write_photon(f, container);
	     }
	 }

       if ( !maya_particle_hair )
	 {
	   writeShader( volume );
	   if ( !maya_particle )
	     writeShader( displace );
	 }

       writeShader( shadow );
       writeShader( environment );
       writeShader( photon );
       writeShader( photonvol );
       writeShader( lightmap );
       writeShader( contour );
     }
#ifdef MR_OPTIMIZE_SHADERS
   else
   {
      material->write_link_line( f );
   }
#endif

   switch( written )
   {
      case kWritten:
	 currentSG = NULL;
	 if ( material )
	   {
	     material->written = kInProgress;
	     MFnDependencyNode dep( material->nodeRef() );
	     material->updateChildShaders( f, dep ); 
	     material->written = kWritten;
	   }
	 return; break;
      case kIncremental:
	 MRL_PUTS( "incremental "); break;
   }

   if ( material )
     {
       MRL_FPRINTF( f, "shader \"%s\"\n", name.asChar() );
       TAB(1);
       MRL_FPRINTF( f, "\"%s\" (\n", material->shader().asChar() );

       MFnDependencyNode dep( material->nodeRef() );

       material->written = kInProgress;

       mrShader::ConnectionMap connNames;
       material->getConnectionNames( f, connNames, dep );
       material->write_shader_parameters( f, dep, connNames );

       MRL_PUTC('\n');
       TAB(1); MRL_PUTS(")\n");

       material->written = kWritten;
     }
   else
     {

       MRL_FPRINTF( f, "material \"%s\"\n", name.asChar() );

       if ( miOpaque )
	 {
	   TAB(1); MRL_PUTS("opaque\n");
	 }

       if ( ! maya_particle && surface )
	 {
	   if ( miExportShadingEngine )
	     {
	       TAB(1); MRL_PUTS( "\"maya_shadingengine\" (\n");	 
	       TAB(2); 
	       const char* n = surface->name.asChar();
	       if ( surface->numOutAttrs > 1 )
		 {
		   MRL_FPRINTF( f, "\"surfaceShader\" = \"%s.outColor\"", n );
		 }
	       else
		 {
		   MRL_FPRINTF( f, "\"surfaceShader\" = \"%s\"", n );
		 }
#if MAYA_API_VERSION >= 850
	       MRL_PUTS(",\n");
	       TAB(2); 
	       MRL_FPRINTF( f, "\"cutAwayOpacity\" %g,\n", miCutAwayOpacity );
	       TAB(2); 
	       MRL_FPRINTF( f, "\"alphaMode\" %d\n", miAlphaMode );
#else
	       MRL_PUTS("\n");
#endif
	       TAB(1); MRL_PUTS( ")\n");

	       if ( photon == NULL && 
		    ( options->caustics || options->globalIllum ) )
		 {
		   TAB(1);
		   MRL_FPRINTF( f, "photon = \"%s:photon\"\n", n );
		 }
	     }
	   else
	     {
	       TAB(1); MRL_FPRINTF( f, " = \"%s\"\n", surface->name.asChar() );
	     }
	 }
   
#define SGconnection( x )						\
       if ( x )								\
	 {								\
	   TAB(1); MRL_FPRINTF( f, #x " = \"%s\"\n", x->name.asChar() ); \
	 }

       if ( maya_displace && displace && !maya_particle_hair )
	 {
	   TAB(1); MRL_PUTS( "displace \"maya_material_displace\" (\n");
	   mrShaderMaya* m = dynamic_cast<mrShaderMaya*>( displace );
	   TAB(2); 
	   assert( m != NULL );
	   if ( m->id == kDisplacementShaderId ||
		m->id == kOceanShaderId )
	     MRL_FPRINTF( f, "\"displacement\" = \"%s.displacement\"\n",
			  displace->name.asChar() );
	   else
	     MRL_FPRINTF( f, "\"displacement\" = \"%s.outAlpha\"\n",
			  displace->name.asChar() );
	   TAB(1); MRL_PUTS( ")\n");
	 }
       else
	 {
	   SGconnection( displace );
	 }

       if ( !maya_particle_hair )
	 {
	   if ( miExportVolumeSampler && volume )
	     {
	       TAB(1); MRL_PUTS( "volume \"maya_volumesampler\" (\n");
	       TAB(2);
	       if ( volume->numOutAttrs > 1 )
		 MRL_FPRINTF( f, "\"volumeShader\" = \"%s.outColor\"\n",
			      volume->name.asChar() );
	       else
		 MRL_FPRINTF( f, "\"volumeShader\" = \"%s\"\n",
			      volume->name.asChar() );
	       TAB(1); MRL_PUTS( ")\n");
	     }
	   else
	     {
	       SGconnection( volume );
	     }
	 }
   
#if MAYA_API_VERSION < 700
       if ( maya_particle )
	 {
	   TAB(1); MRL_PUTS( "shadow \"maya_transparent\" ()\n");
	 }
       else
	 {
#endif
	   SGconnection( shadow );
#if MAYA_API_VERSION < 700
	 }
#endif
       SGconnection( environment );
       SGconnection( photon );
       SGconnection( photonvol );
       SGconnection( lightmap );
       SGconnection( contour );
       MRL_PUTS( "end material\n");
     }
   
   NEWLINE();
   written = kWritten;

   currentSG = NULL;
}



void mrShadingGroup::debug(int tabs) const throw()
{
   for (int i = 0; i < tabs; ++i)
      cerr << "\t";
   cerr << "* MATERIAL " << this << " name: " << name << endl;
}

#undef SGconnection
#undef writeShader

#ifdef GEOSHADER_H
#include "raylib/mrShadingGroup.inl"
#endif
