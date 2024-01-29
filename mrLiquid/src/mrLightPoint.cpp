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

#include "maya/MFnPointLight.h"
#include "maya/MPlug.h"

#ifndef   mrLightPoint_h
#include "mrLightPoint.h"
#endif

#ifndef   mrShaderLight_h
#include "mrShaderLight.h"
#endif

#ifndef   mrInstanceObject_h
#include "mrInstanceObject.h"
#endif

#ifndef   mrAttrAux_h
#include "mrAttrAux.h"
#endif

#if MAYA_API_VERSION < 850

#ifndef   mentalCmd_h
#include "mentalCmd.h"
#endif

#ifndef   mrTranslator_h
#include "mrTranslator.h"
#endif

#ifndef mrInheritableFlags_h
#include "mrInheritableFlags.h"
#endif

extern MObject     currentNode;
extern MDagPath currentObjPath;

#endif


mrLightPoint::~mrLightPoint()
{
}

void mrLightPoint::getData()
{   
   DBG(name << ": mrLightPoint::getData");
   if ( shader == NULL )
   {
      shader = mrShaderLight::factory( "maya_pointlight",
				       path );
      shader->forceIncremental();
   }


#if MAYA_API_VERSION < 850
   areaObject = NULL;

   if ( areaType == kObject || areaType == kUser )
   {
      MStatus status;  MPlug p;
      MFnDagNode fn( path );

      p = fn.findPlug("miAreaObject", true, &status);
      if ( status == MS::kSuccess )
      {
	 // Okay, we are using an area light object
	 MPlugArray plugs;
	 p.connectedTo( plugs, true, false );
	 if ( plugs.length() == 1 )
	 {
	    MObject  oldNode = currentNode;
	    MDagPath oldPath = currentObjPath;
	    
	    currentNode = plugs[0].node();
	    MDagPath::getAPathTo( currentNode, currentObjPath );

	    currentObjPath.extendToShape();
	    currentNode = currentObjPath.node();

	    mrFlagStack flags;
            mrInheritableFlags* cflags = new mrInheritableFlags;
	    flags.push( cflags );
	    cflags->update( currentObjPath );
	    // @todo: not good.  not multithread safe.
	    //        how can I make the translator accesible everywhere?
	    mrTranslator* t = mentalCmd::translator;
	    mrInstanceObjectBase* inst;
	    t->addObject( inst, flags, cflags );
	    if ( dynamic_cast< mrInstanceObject* >( inst ) != NULL )
	    {
	       if ( inst->hide )
	       {
		  // If you hide the object, the area light does
		  // not render.  So unhide it, but set all of its
		  // flags off.
		  inst->hide = false;
		  inst->visible = mrInstance::kVOff;
		  inst->trace   = mrInstance::kVOff;
		  inst->shadow  = mrInstance::kVOff;
	       }

	       areaType   = kObject;
	       areaObject = (mrInstanceObject*) inst;
	    }
	    else
	    {
	       areaType = kNone;
	       LOG_ERROR("Area object in miAreaObject is not valid.");
	    }

	    currentNode = oldNode;
	    currentObjPath = oldPath;
	 }
      }
   }
#endif

}

void mrLightPoint::forceIncremental()
{
   shader = NULL;
   mrLightPhotons::forceIncremental();
   mrLightPoint::getData();
}


void mrLightPoint::setIncremental( bool sameFrame )
{
   mrLightPhotons::setIncremental( sameFrame );
   mrLightPoint::getData();
}


mrLightPoint::mrLightPoint( const MDagPath& light ) :
mrLightPhotons( light )
{
   mrLightPoint::getData();
}


void mrLightPoint::write( MRL_FILE* f )
{
   if ( areaObject ) areaObject->write(f);
   mrLightPhotons::write(f);
}


void mrLightPoint::write_properties( MRL_FILE* f )
{
   MRL_PUTS("origin 0 0 0\n");
   mrLightPhotons::write_properties(f);
}




////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// RAYLIB
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#ifdef GEOSHADER_H


void mrLightPoint::write()
{
   if ( areaObject ) areaObject->write();
   mrLightPhotons::write();
}


void mrLightPoint::write_properties( miLight* l )
{
   l->type = miLIGHT_ORIGIN;
   l->origin.x = l->origin.y = l->origin.z = 0.0f;
   mrLightPhotons::write_properties( l );
}

#endif
