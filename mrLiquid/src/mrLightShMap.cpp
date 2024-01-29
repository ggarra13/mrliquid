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

#if defined(WIN32) || defined(WIN64)
#else
#include <glob.h>     // glob(), globfree()
#endif

#include "maya/MPlug.h"
#include "maya/MFnNonExtendedLight.h"

#include "mrLightShMap.h"
#include "mrLightPoint.h"
#include "mrHelpers.h"
#include "mrCamera.h"
#include "mrOptions.h"

#include "mrAttrAux.h"

//
// Currently, turning on "shadowmap file" will work fine, and on incremental
// updates, it will not be re-read once taken off.
// However, it seems it will still re-generate a shadow map
// file each time once it was "activated" in some other pass.
//

// #define MR_SHADOWMAP_BUG


extern int frame;
extern MString shmapDir;
extern MString tempDir;
extern MString sceneName;
extern MString currentInstPath;


mrLightShMap::~mrLightShMap()
{
   camera = NULL;
}


void mrLightShMap::getData()
{
   DBG(name << ": mrLightShMap::getData");
   MStatus status;
   MFnNonExtendedLight fn( path );
   
   if ( raytraced )
   {
      shadowMapResolution = 0;
      shadowMapBias = 0;
      shadowCrops = false;
      updateShadowMap = false;
   }
   else
   {
     bool smapLightName;
     bool smapSceneName;
     bool smapFrameExt;

      short tmpS;
      float tmpF;
      bool  tmpB = useDepthMapShadows;
      MPlug p;
      GET_OPTIONAL_ATTR( useDepthMapShadows, shadowMap );
      if ( useDepthMapShadows )
      {
	 if ( tmpB != useDepthMapShadows ) updateShadowMap = true;

	 tmpS = shadowMapResolution;
	 GET_OPTIONAL_ATTR( shadowMapResolution, smapResolution );
	 if ( tmpS != shadowMapResolution ) updateShadowMap = true;


	 MString oldName = shadowMapName;
	 GET_OPTIONAL_ATTR( shadowMapName, smapFilename );
	 shadowMapName = parseString( shadowMapName );

#if MAYA_API_VERSION > 650
	 GET_OPTIONAL( smapLightName );
	 if ( smapLightName )
	   {
	     shadowMapName += "_";
	     shadowMapName += dagPathToFilename( name );
	   }
	 GET_OPTIONAL( smapSceneName );
	 if ( smapSceneName )
	   {
	     shadowMapName += "_";
	     shadowMapName += sceneName; 
	   }
	 GET_OPTIONAL( smapFrameExt  );
	 if ( smapFrameExt )
	   {
	     shadowMapName += frame;
	   }
#endif

	 if ( oldName != shadowMapName ) updateShadowMap = true;
	 
	 GET_OPTIONAL_ATTR( shadowMapSamples, smapSamples );
	 GET_OPTIONAL_ATTR( shadowMapSoftness, smapSoftness );
	 shadowMapBias = 0;
	 GET_OPTIONAL_ATTR( shadowMapBias, smapBias );

	 tmpB = shadowCrops;
	 GET_OPTIONAL_ATTR( shadowCrops, smapCrop );
	 if ( tmpB != shadowCrops ) updateShadowMap = true;

	 
	 tmpB = detailShadows;
	 GET_OPTIONAL_ATTR( detailShadows, smapDetail );
	 if ( tmpB != detailShadows ) updateShadowMap = true;

	 if ( detailShadows )
	 {
	    tmpB = detailAlpha;
	    GET_OPTIONAL_ATTR( detailAlpha, smapDetailAlpha );
	    if ( tmpB != detailAlpha ) updateShadowMap = true;

	    tmpS = detailSamples;
	    GET_OPTIONAL_ATTR( detailSamples, smapDetailSamples );
	    if ( tmpS != detailSamples ) updateShadowMap = true;

	    tmpF = detailShadowAccuracy;
	    GET_OPTIONAL_ATTR( detailShadowAccuracy,
			       smapDetailAccuracy );
	    if ( tmpF != detailShadowAccuracy ) updateShadowMap = true;
	 }
	 return;
      }

      useDepthMapShadows = fn.useDepthMapShadows();
      if ( useDepthMapShadows )
      {
	 if ( tmpB != useDepthMapShadows ) updateShadowMap = true;

	 tmpS = shadowMapResolution;
	 shadowMapResolution = fn.depthMapResolution( &status );
	 if ( tmpS != shadowMapResolution ) updateShadowMap = true;

	 shadowMapBias = 0; // fn.depthMapBias(&status );

	 short filterSize    = fn.depthMapFilterSize( &status );

	 shadowMapSamples = filterSize * 10;
 	 shadowMapSoftness = (float)filterSize / 40.f;

#if MAYA_API_VERSION > 650
	 GET_OPTIONAL_ATTR( smapLightName, dmapLightName );
	 if ( smapLightName )
	   {
	     shadowMapName += "_";
	     shadowMapName += dagPathToFilename( name );
	   }
	 GET_OPTIONAL_ATTR( smapSceneName, dmapSceneName );
	 if ( smapSceneName )
	   {
	     shadowMapName += "_";
	     shadowMapName += sceneName;
	   }
	 GET_OPTIONAL_ATTR( smapFrameExt, dmapFrameExt );
	 if ( smapFrameExt )
	   {
	     shadowMapName += frame;
	   }
#endif
      }
   }
}


mrLightShMap::mrLightShMap( const MDagPath& light ) :
mrLight( light ),
camera(NULL),
shadowMapBias(0.0),
#if MAYA_API_VERSION > 650
shadowMapFilter(0),
#endif
shadowMapSamples( 1 ),
shadowMapResolution( 0 ),
updateShadowMap( true ),
useDepthMapShadows( false ),
detailShadows( false ),
detailAlpha( false ),
shadowCrops( false )
{
   mrLightShMap::getData();
}

void mrLightShMap::write_camera( MRL_FILE* f )
{
   if ( camera ) camera->write_light(f);
}

void mrLightShMap::forceIncremental()
{
   DBG(name << " mrLightShMap::forceIncremental");
   mrLight::forceIncremental();
   mrLightShMap::getData();
   if ( camera ) camera->forceIncremental();
}

void mrLightShMap::setIncremental( bool sameFrame )
{
   DBG(name << " mrLightShMap::setIncremental");
   mrLight::setIncremental( sameFrame );
   mrLightShMap::getData();
   if ( camera ) camera->setIncremental( sameFrame );
}


void mrLightShMap::write( MRL_FILE* f )
{
   mrLight::write(f);
   if ( options->progressive && useDepthMapShadows && 
        options->progressivePercent < 1.0f )
   {
      written = kIncremental;
   }
}

MString mrLightShMap::getShadowName( bool forRemoval )
{
   MString shadowName;
   if ( shadowMapName.length() > 0 )
   {
      char path = options->exportPathNames[mrOptions::kShadowMapPath];
      if ( path == 'a' ) shadowName = shmapDir;
      shadowName += shadowMapName;
   }
   else if ( options->IPR || 
	     options->rebuildShadowMaps != mrOptions::kRebuildOff )
   {
      shadowName  = tempDir;
      shadowName += dagPathToFilename( path.partialPathName() );
      shadowName += ".";
      // @todo: find out hash function used by mental ray, so we can remove
      //        the appropiate instance. mrInstanceLight would then pass its
      //        hash value over to mrLight if updateShadowMap is on.
      if ( forRemoval ) 
      {
	 shadowName += "*"; // -- mray uses a hash per instance...remove all
      }
      else
      {
	 shadowName += "#"; // -- mray hash per instance... hmmm---
      }
      shadowName += ".";
      shadowName += frame;
      shadowName += ".zt";
   }
   return shadowName;
}

void mrLightShMap::deleteOneShadowMap( const MString& shadowGlobName )
{
  int idx = shadowGlobName.rindex('/');
  MString dir = shadowGlobName.substring(0, idx);

#if defined(WIN32) || defined(WIN64)
  WIN32_FIND_DATA data;
  HANDLE glob = FindFirstFile( shadowGlobName.asChar(), 
			       &data );
  if (glob != INVALID_HANDLE_VALUE) 
    {
      if ( (data.dwFileAttributes & 
	    FILE_ATTRIBUTE_DIRECTORY) == 0 )
	{
	  MString fullname = dir;
	  fullname += data.cFileName;
	  DBG("unlink " << fullname );
	  UNLINK( fullname.asChar() );
	}

      while (FindNextFile(glob, &data) != 0) 
	{
	  MString fullname = dir;
	  fullname += data.cFileName;
	  DBG("unlink " << fullname );
	  UNLINK( fullname.asChar() );
	}
      FindClose(glob);
    }
#else
  glob_t gl;
  if(0 == glob( shadowGlobName.asChar(), GLOB_NOSORT, 
		NULL, &gl))
    {
      char** begin = &gl.gl_pathv[0];
      char** end = &gl.gl_pathv[gl.gl_pathc];
      for( ; begin != end; ++begin)
	{
	  UNLINK( *begin );
	}
      globfree(&gl);
    }
#endif
}

void mrLightShMap::deleteShadowMap()
{
  // There was a change in shadow map settings or 
  // light location.  
  // Make sure to remove any saved file for shadow map.
  MString shadowName = getShadowName(true);
  // Point lights actually create 6 shadow maps that we need to
  // remove.  All named  "rootname.[1-6]"
  if ( dynamic_cast< mrLightPoint* >( this ) != NULL )
    {
      for ( int i = 1; i < 7; ++i )
	{
	  MString sideShadowName = shadowName;
	  sideShadowName += ".";
	  sideShadowName += i;
	  DBG("+++++++ unlink p shadow map: " << sideShadowName);

	  deleteOneShadowMap( sideShadowName );
	}
    }
  else
    {
      deleteOneShadowMap( shadowName );
    }
}

void mrLightShMap::changeShadowMap() 
{ 
   updateShadowMap = true;
   if ( written != kNotWritten ) written = kIncremental;
}

void mrLightShMap::write_properties( MRL_FILE* f )
{
   if ( useDepthMapShadows )
   {
      MRL_PUTS( "shadowmap");
      if ( detailShadows )
	 MRL_PUTS(" detail\n");
      else
	 MRL_PUTC('\n');
      if ( updateShadowMap && options->progressive && shadowMapName == "" )
      {
	 if ( updateShadowMap )
	 {
	    int u = (int) (shadowMapResolution * options->progressivePercent);
	    if ( u < 64 ) u = 64;
	    DBG(name << ": shadowmap resolution " << u);
	    MRL_FPRINTF(f, "shadowmap resolution %d\n", u);
	 }
	 else
	 {
	    DBG(name << ": NOT UPDATE SHADOWMAP ");
	    MRL_FPRINTF(f, "shadowmap resolution %d\n", shadowMapResolution);
	 }
	 if ( options->progressive )
	 {
	    int u = (int) (shadowMapSamples * options->progressivePercent);
	    if ( u < 1 ) u = 1;
	    DBG(name << ": shadowmap samples " << u);
	    MRL_FPRINTF(f, "shadowmap samples %d\n", u);
	 }
	 else
	 {
	    MRL_FPRINTF(f, "shadowmap samples %d\n", shadowMapSamples);
	 }
      }
      else
      {
	 MRL_FPRINTF(f, "shadowmap resolution %d\n", shadowMapResolution);
	 MRL_FPRINTF(f, "shadowmap samples %d\n", shadowMapSamples);
      }

      MRL_FPRINTF(f, "shadowmap softness %g\n", shadowMapSoftness);
      if ( shadowMapName.length() > 0 )
      {
	 MString shadowName = getShadowName();
	 MRL_FPRINTF(f, "shadowmap file \"%s\"\n", shadowName.asChar());
	 updateShadowMap = false;
      }
      else if ( options->IPR ||	options->rebuildShadowMaps != mrOptions::kRebuildOff )
      {
#ifndef MR_SHADOWMAP_BUG
	 if ( (!options->progressive) || 
	      (options->progressivePercent > 0.995f) ||
	      (!updateShadowMap) )
	 {
	    if ( updateShadowMap )
	    {
	      deleteShadowMap();
	    }
	    MString shadowName = getShadowName();
	    DBG("shadowName: " << shadowName << 
		" update: " << (int) updateShadowMap);
	    MRL_FPRINTF(f, "shadowmap file \"%s\"\n", shadowName.asChar());
	    updateShadowMap = false;
	 }
#endif
      }

      MRL_FPRINTF(f, "shadowmap bias %g\n", shadowMapBias);
      if ( detailShadows )
      {
	 if ( detailShadowAccuracy > 0.0f )
	    MRL_FPRINTF(f, "shadowmap accuracy %g\n", detailShadowAccuracy);
	 if ( detailSamples > 0 )
	 {
	    if ( options->progressive )
	    {
	       int u = (int) (detailSamples * options->progressivePercent);
	       if ( u < 1 ) u = 1;
	       MRL_FPRINTF(f, "shadowmap detail samples %d\n", u);
	    }
	    else
	    {
	       MRL_FPRINTF(f, "shadowmap detail samples %d\n", detailSamples);
	    }
	 }
	 if ( detailAlpha )
	    MRL_FPRINTF(f, "shadowmap alpha\n");
	 else
	    MRL_FPRINTF(f, "shadowmap color\n");
      }
      if ( camera )
	 MRL_FPRINTF( f, "shadowmap camera \"%s\"\n", camera->name.asChar() );
   }
   else
   {
      if ( written == kIncremental )
	 MRL_PUTS( "shadowmap off\n");
   }

   mrLight::write_properties(f);
}



#ifdef GEOSHADER_H
#include "raylib/mrLightShMap.inl"
#endif

