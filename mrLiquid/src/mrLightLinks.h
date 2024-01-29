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


#ifndef mrLightLinks_h
#define mrLightLinks_h

#include <map>

#include "maya/MSelectionList.h"
#include "maya/MDagPath.h"
#include "maya/MLightLinks.h"

#include "mrHash.h"  // for MString < MString
#include "mrHelpers.h"

class mrGroupInstance;


class mrLightLinks
{
protected:
  typedef std::map< MString, MSelectionList* > mrLinks;
public:
  mrLightLinks();
  ~mrLightLinks();

  void getLightLinks();

  void parse( const mrGroupInstance* allLights );

  void reset();

  void isIgnored( MStringArray& ignoredLights, const MDagPath& object );
  void isShadowIgnored( MStringArray& ignoredLights, const MDagPath& object );


  void write( MRL_FILE* f );
  void write_ignored_links( MRL_FILE* f, const mrLinks& ignored,
			    const char* suffix );

#ifdef GEOSHADER_H
  void write();
  void write_ignored_links( const mrLinks& ignored );

protected:
  miTag tag, shadowTag;
#endif

protected:
  void addSet( mrLinks& lgtset,
	       const MString& lightName, const MObject& node );
  void addObject( mrLinks& lgtobj,
		  const MString& lightName, 
		  MDagPath& inst, const MDagPath& shape );
  void addToLists( mrLinks& lgtset, mrLinks& lgtobj,
		   MString& name, const MObject& node );
  void addLight( mrLinks& lgtset, mrLinks& lgtobj,
		 const MDagPath& shape, const MObject& node );


  MStatus parseLinks( const MObject& lightLinkObj );
  MStatus parseLinks( const MObject& lightLinkObj, 
		      const mrGroupInstance* allLights );
     
  //! List of lightLink nodes in scene
  static MObjectArray lightLinks;

  //! List of MLightLinks operators
  static MLightLinks  links;

  //! Parsed list of linked sets and objects ( < maya 8.0 only )
  static mrLinks      linkedSets;
  static mrLinks      linkedObjects;

  //! Parsed list of ignored sets and objects
  static mrLinks      ignoredSets;
  static mrLinks      ignoredObjects;

  //! Parsed list of ignored shadow sets and objects (maya 8.5+)
  static mrLinks      shadowIgnoredSets;
  static mrLinks      shadowIgnoredObjects;
};


#endif // mrLightLinks_h


