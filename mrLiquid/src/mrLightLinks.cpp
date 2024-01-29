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


#include <set>

#include "maya/MPlug.h"
#include "maya/MPlugArray.h"
#include "maya/MString.h"
#include "maya/MStatus.h"
#include "maya/MFnDependencyNode.h"
#include "maya/MFnSet.h"
#include "maya/MItSelectionList.h"
#include "maya/MItDag.h"
#include "maya/MItDependencyNodes.h"

#include "mrIO.h"
#include "mrLightLinks.h"
#include "mrShadingGroup.h"
#include "mrGroupInstance.h"
#include "mrLight.h"
#include "mrShader.h"
#include "mrInstance.h"
#include "mrHelpers.h"

// #undef DBG
// #define DBG(x) cerr << x << endl

MObjectArray mrLightLinks::lightLinks;
MLightLinks  mrLightLinks::links;

mrLightLinks::mrLinks  mrLightLinks::linkedSets;
mrLightLinks::mrLinks  mrLightLinks::linkedObjects;

mrLightLinks::mrLinks  mrLightLinks::ignoredSets;
mrLightLinks::mrLinks  mrLightLinks::ignoredObjects;

mrLightLinks::mrLinks  mrLightLinks::shadowIgnoredSets;
mrLightLinks::mrLinks  mrLightLinks::shadowIgnoredObjects;

void mrLightLinks::reset()
{
   mrLinks::iterator i = linkedSets.begin();
   mrLinks::iterator e = linkedSets.end();
   for ( ; i != e; ++i )
      delete i->second;
   i = linkedObjects.begin();
   e = linkedObjects.end();
   for ( ; i != e; ++i )
      delete i->second;

   i = ignoredSets.begin();
   e = ignoredSets.end();
   for ( ; i != e; ++i )
      delete i->second;
   i = ignoredObjects.begin();
   e = ignoredObjects.end();
   for ( ; i != e; ++i )
      delete i->second;

   i = shadowIgnoredSets.begin();
   e = shadowIgnoredSets.end();
   for ( ; i != e; ++i )
      delete i->second;
   i = shadowIgnoredObjects.begin();
   e = shadowIgnoredObjects.end();
   for ( ; i != e; ++i )
      delete i->second;


   linkedSets.clear();
   linkedObjects.clear();
   ignoredSets.clear();
   ignoredObjects.clear();
   shadowIgnoredSets.clear();
   shadowIgnoredObjects.clear();
}


mrLightLinks::mrLightLinks()
{
   reset();
}


mrLightLinks::~mrLightLinks()
{
   reset();
}

void mrLightLinks::write_ignored_links( MRL_FILE* f, const mrLinks& ignored,
					const char* suffix )
{
  MRL_FPRINTF( f, "incremental shader \"lightLinker1%s\"\n", suffix );
  TAB(1); MRL_PUTS( "\"maya_lightlink\" (\n" );

  mrLinks::const_iterator i = ignored.begin();
  mrLinks::const_iterator e = ignored.end();
  if ( i != e )
    {
      TAB(2); MRL_PUTS( "\"ignoreLinks\" [\n");

      //
      // Then, spit out light->object ignore connections
      //
      bool comma = false;
      for ( ; i != e; ++i )
	{
	  MString lightName = i->first;
	  MItSelectionList it( *(i->second) );
	  if ( comma ) MRL_PUTS( ",\n" );
	  TAB(3);
	  MRL_FPRINTF(f, "\"%s\",\n", lightName.asChar() );
	  comma = true;
	  for ( ; !it.isDone(); it.next() )
	    {
	      MDagPath path;
	      it.getDagPath( path );
	      MString objName = getMrayName( path );
	      TAB(3);
	      MRL_FPRINTF(f, "\"%s\",\n", objName.asChar());
	    }
	  TAB(3);
	  MRL_PUTS( "\"\"" );
	}

      MRL_PUTC( '\n' );
      TAB(2);
      MRL_PUTS("]\n");
    }
  TAB(1);
  MRL_PUTS( ")\n");
}

void mrLightLinks::write( MRL_FILE* f )
{
   bool comma = false;

   if ( options->lightLinkerType == mrOptions::kLightLinkerShaderIgnore )
     {
       write_ignored_links( f, ignoredObjects, "" );
       if ( options->shadowLinking == mrOptions::kShadowsObeyShadowLinking )
	 {
	   write_ignored_links( f, shadowIgnoredObjects, ":shadow" );
	 }
     }
   else if ( options->lightLinkerType == mrOptions::kLightLinkerShaderSets )
     {
       mrLinks::iterator i = linkedSets.begin();
       mrLinks::iterator e = linkedSets.end();
       for ( ; i != e; ++i )
	 {
	   MItSelectionList it( *(i->second) );
	   for ( ; !it.isDone(); it.next() )
	     {
	       MObject node;
	       it.getDependNode( node );
	       MFnSet fn( node );
	       mrGroupInstance* g = mrGroupInstance::factory( fn );
	       if ( g->written == mrNode::kWritten ) g->forceIncremental();
	       g->write_group(f);
	     }
	 }

       i = ignoredSets.begin();
       e = ignoredSets.end();
       for ( ; i != e; ++i )
	 {
	   MItSelectionList it( *(i->second) );
	   for ( ; !it.isDone(); it.next() )
	     {
	       MObject node;
	       it.getDependNode( node );
	       MFnSet fn( node );
	       mrGroupInstance* g = mrGroupInstance::factory( fn );
	       if ( g->written == mrNode::kWritten ) g->forceIncremental();
	       g->write_group(f);
	     }
	 }

       MRL_PUTS( "incremental shader \"lightLinker1\"\n" );
       TAB(1); MRL_PUTS( "\"maya_lightlink\" (\n" );
       TAB(2); MRL_PUTS( "\"link\" [");

       bool expandGroups = false;

       //
       // First, spit out light->set link connections
       //
       i = linkedSets.begin();
       e = linkedSets.end();
       for ( ; i != e; ++i )
	 {
	   MString lightName = i->first;
	   MItSelectionList it( *(i->second) );
	   for ( ; !it.isDone(); it.next() )
	     {
	       MObject node;
	       it.getDependNode( node );
	       MFnSet fn( node );
	       mrGroupInstance* g = mrGroupInstance::factory( fn );
	       if ( comma ) MRL_PUTC(',');
	       MRL_PUTC('\n');
	       TAB(3);
	       MRL_FPRINTF(f, "{\"light\" \"%s\", \"object\" \"%s\"}",
		       lightName.asChar(), g->name.asChar());
	       comma = true;
	       expandGroups = true;
	     }
	 }

       //
       // Then spit out light->object link connections
       //
       i = linkedObjects.begin();
       e = linkedObjects.end();
       for ( ; i != e; ++i )
	 {
	   MString lightName = i->first;
	   MItSelectionList it( *(i->second) );
	   for ( ; !it.isDone(); it.next() )
	     {
	       MDagPath path;
	       it.getDagPath( path );
	       MString objName = getMrayName( path );
	       if ( comma ) MRL_PUTC(',');
	       MRL_PUTC('\n');
	       TAB(3);
	       MRL_FPRINTF(f, "{\"light\" \"%s\", \"object\" \"%s\"}",
		       lightName.asChar(), objName.asChar());
	       comma = true;
	     }
	 }
       MRL_PUTC('\n');
       TAB(2);
       MRL_PUTS("],\n");

       //
       // Then, spit out light->set ignore connections
       //
       TAB(2); MRL_PUTS( "\"ignore\" [");
       i = ignoredSets.begin();
       e = ignoredSets.end();
       comma = false;
       for ( ; i != e; ++i )
	 {
	   MString lightName = i->first;
	   MItSelectionList it( *(i->second) );
	   for ( ; !it.isDone(); it.next() )
	     {
	       MObject node;
	       it.getDependNode( node );
	       MFnSet fn( node );
	       mrGroupInstance* g = mrGroupInstance::factory( fn );
	       if ( comma ) MRL_PUTC(',');
	       MRL_PUTC('\n');
	       TAB(3);
	       MRL_FPRINTF(f, "{\"lightIgnored\" \"%s\", \"objectIgnored\" \"%s\"}",
		       lightName.asChar(), g->name.asChar());
	       comma = true;
	       expandGroups = true;
	     }
	 }

       //
       // Then, spit out light->object ignore connections
       //
       i = ignoredObjects.begin();
       e = ignoredObjects.end();
       for ( ; i != e; ++i )
	 {
	   MString lightName = i->first;
	   MItSelectionList it( *(i->second) );
	   for ( ; !it.isDone(); it.next() )
	     {
	       MDagPath path;
	       it.getDagPath( path );
	       MString objName = getMrayName( path );
	       if ( comma ) MRL_PUTC(',');
	       MRL_PUTC('\n');
	       TAB(3);
	       MRL_FPRINTF(f, "{\"lightIgnored\" \"%s\", \"objectIgnored\" \"%s\"}",
		       lightName.asChar(), objName.asChar());
	       comma = true;
	     }
	 }

       MRL_PUTC('\n');
       TAB(2);
       MRL_PUTS("],\n");
       TAB(2); MRL_FPRINTF( f, "\"expandGroups\" %s\n", expandGroups? "on" : "off");
       TAB(1);
       MRL_PUTS( ")\n");
     }

}

void mrLightLinks::addSet( mrLinks& lgtset,
			   const MString& lightName, const MObject& node )
{
#ifdef DEBUG
   MFnDependencyNode fn( node );
   DBG("\t\tAdding set: " << fn.name() );
#endif

   mrLinks::iterator it = lgtset.find( lightName );
   if ( it != lgtset.end() )
   {
      MSelectionList* sel = it->second;
      sel->add( node, true );
   }
   else
   {
      MSelectionList* sel = new MSelectionList;
      sel->add( node, false );
      lgtset.insert( std::make_pair( lightName, sel ) );
   }
}


void mrLightLinks::addObject( mrLinks& lgtobj,
                              const MString& lightName, 
			      MDagPath& instPath, const MDagPath& shapePath )
{
//    mrInstance* inst = mrInstance::find( instPath, shapePath );
//    if ( !inst ) return;

   DBG("\tAdding object: " << instPath.fullPathName() );
   mrLinks::iterator it = lgtobj.find( lightName );
   if ( it != lgtobj.end() )
   {
      MSelectionList* sel = it->second;
      sel->add( instPath, MObject::kNullObj, true );
   }
   else
   {
      MSelectionList* sel = new MSelectionList;
      sel->add( instPath );
      lgtobj.insert( std::make_pair( lightName, sel ) );
   }
}


void mrLightLinks::addToLists( mrLinks& lgtset, mrLinks& lgtobj,
			       MString& lightName, const MObject& node )
{
   if ( node.hasFn( MFn::kSet ) )
   {
      // Check if a material... if a material, we expand it
      // in place...
      MFnSet fn( node );
      if ( fn.hasRestrictions() && 
           fn.restriction() == MFnSet::kRenderableOnly )
      {
	 DBG("\t\tFound material set: " << fn.name() );

	 mrShadingGroupList::iterator i = sgList.find( fn.name() );
	 if ( i == sgList.end() )
	 {
	    DBG("\t\tMaterial not spit out in this pass");
	    return;
	 }

#ifdef FLATTEN_MATERIALS
	 MSelectionList sel;
	 fn.getMembers( sel, true );
	 MItSelectionList it( sel );
	 for ( ; !it.isDone(); it.next() )
	 {
	    MDagPath path;
	    it.getDagPath( path );
	    MDagPath inst = path;
	    inst.pop();
	    addObject( lgtobj, lightName, inst, path );
	 }
#else
	 mrGroupInstance* g = mrGroupInstance::factory( fn );
	 if ( g->size() == 0 )
	 {
	    DBG("\t\t\t...but object set is empty.  Ignoring.");
	    nodeList.remove( nodeList.find( g->name ) );
	    return;
	 }
	 addSet( lgtset, lightName, node );
#endif

      }
      else
      {
	 DBG("\t\tFound object set: " << fn.name() );
	 mrGroupInstance* g = mrGroupInstance::factory( fn );
	 if ( g->size() == 0 )
	 {
	    DBG("\t\t\t...but object set is empty.  Ignoring.");
	    nodeList.remove( nodeList.find( g->name ) );
	    return;
	 }

	 addSet( lgtset, lightName, node );
      }
   }
   else if ( node.hasFn( MFn::kDagNode ) )
   { 
      // empty
      MFnDependencyNode fn( node );
      DBG("\t\tFound object: " << fn.name() );
      MDagPath path;
      MDagPath::getAPathTo( node, path );
      if ( path.hasFn( MFn::kTransform ) )
      {
	 MItDag itd;
	 MDagPath curr;
	 for ( itd.reset( path ); !itd.isDone(); itd.next() )
	 {
	    itd.getPath( curr );

	    unsigned numShapes;
	    MRL_NUMBER_OF_SHAPES( curr, numShapes );
	    for ( unsigned c = 0; c < numShapes; ++c )
	    {
	       MDagPath shape = curr;
	       MRL_EXTEND_TO_SHAPE_DIRECTLY_BELOW( shape, c, numShapes );

	       addObject( lgtobj, lightName, curr, shape );
	    }
	 }
      }
      else
      {
	 MDagPath inst = path;
	 inst.pop();
	 addObject( lgtobj, lightName, inst, path );
      }
   }
   else
   {
      LOG_ERROR("Unknown object connection");
      return;
   }
}


void mrLightLinks::addLight( mrLinks& lgtset, mrLinks& lgtobj,
			     const MDagPath& shape, const MObject& node )
{
   MDagPath inst = shape;
   if ( shape.hasFn( MFn::kTransform ) )
   {
      unsigned numShapes;
      MRL_NUMBER_OF_SHAPES( inst, numShapes );
      for ( unsigned c = 0; c < numShapes; ++c )
      {
	 MDagPath shapePath = inst;
	 MRL_EXTEND_TO_SHAPE_DIRECTLY_BELOW( shapePath, c, numShapes );

	 mrInstance* ptr = mrInstance::find( inst, shapePath, numShapes );
	 if ( ptr == NULL ) {
	    DBG("\tDid not find light from transform: " << 
		inst.fullPathName());
	    continue;
	 }

	 mrInstanceLight* lgt = dynamic_cast< mrInstanceLight* >( ptr );
	 if (!lgt) continue;

	 MString& lightName = ptr->name;
	 DBG("\tFound light from transform: " << lightName );
	 addToLists( lgtset, lgtobj, lightName, node );
      }
   }
   else if ( shape.hasFn( MFn::kLight ) )
   {
      inst.pop();
      mrInstance* ptr = mrInstance::find( inst, shape );
      if ( ptr == NULL ) {
	 DBG("\tDid not find light from shape: " << 
	     shape.fullPathName());
	 return;
      }
      mrInstanceLight* lgt = dynamic_cast< mrInstanceLight* >( ptr );
      if (!lgt) return;

      MString& lightName = ptr->name;
      DBG("\tFound light: " << lightName );
      addToLists( lgtset, lgtobj, lightName, node );
   }
   else
   {
      MString msg = "\"";
      msg += shape.fullPathName();
      msg += "\" is not a light.  Cannot light link to it.";
      LOG_ERROR(msg);
   }
}


MStatus mrLightLinks::parseLinks( const MObject& lightLinkObj,
				  const mrGroupInstance* allLights )
{
  links.parseLinks( lightLinkObj );

  if (!allLights) return MS::kFailure;

  // Then, get all the objs for each light
  mrGroupInstance::const_iterator k  = allLights->begin();
  mrGroupInstance::const_iterator ke = allLights->end();
  for ( ; k != ke; ++k )
    {
      mrInstance* inst = dynamic_cast<mrInstance*>( *k );
      mrLight*     lgt = dynamic_cast<mrLight*>( inst->shape );

      std::set< MString > objectList, shadowList;
      MSelectionList objs;
      MDagPath   objPath;


#if MAYA_API_VERSION >= 800

      links.getIgnoredObjects( lgt->path, objs );
      unsigned numObjs = objs.length();

      for ( unsigned j = 0; j < numObjs; ++j )
	{
	  objs.getDagPath( j, objPath );
	  objPath.pop(1);
	  const MString& objName = getMrayName( objPath );
	  if ( objectList.find( objName ) !=
	       objectList.end() ) continue;
	  
	  objectList.insert( objName );
	  
	  addLight( ignoredSets, ignoredObjects, 
		    lgt->path, objPath.node() );
	}

      links.getShadowIgnoredObjects( lgt->path, objs );
      numObjs = objs.length();
      for ( unsigned j = 0; j < numObjs; ++j )
	{
	  objs.getDagPath( j, objPath );
	  objPath.pop(1);
	  const MString& objName = getMrayName( objPath );

	  if ( shadowList.find( objName ) !=
	       shadowList.end() ) continue;

	  shadowList.insert( objName );
	  
	  
	  addLight( shadowIgnoredSets, shadowIgnoredObjects, 
		    lgt->path, objPath.node() );
	}
#else
      links.getLinkedObjects( lgt->path, objs );
      unsigned numObjs = objs.length();
      for ( unsigned j = 0; j < numObjs; ++j )
	{
	  objs.getDagPath( j, objPath );
	  objPath.pop(1);
	  const MString& objName = getMrayName( objPath );
	  if ( objectList.find( objName ) !=
	       objectList.end() ) continue;
	  
	  objectList.insert( objName );
	  
	  addLight( linkedSets, linkedObjects, 
		    lgt->path, objPath.node() );
	}
#endif
    }

  return MS::kSuccess;
}


MStatus mrLightLinks::parseLinks( const MObject& lightLinkObj )
{

   MStatus status;
   MFnDependencyNode fn( lightLinkObj );
   MString linkerName = fn.name();
   DBG("LIGHTLINKER: " << linkerName);

   MPlug p = fn.findPlug( "link", &status );
   unsigned numElements = p.evaluateNumElements();
   DBG("\tLINK Elements " << numElements);
   if ( numElements )
   {
      MPlugArray plugs;
      for ( unsigned i = 0; i < numElements; ++i)
      {
	 MPlug ep = p.elementByPhysicalIndex(i);
	 if (ep.numConnectedChildren() != 2) continue;

	 MPlug lp = ep.child(0);
	 lp.connectedTo( plugs, true, false );
	 if ( plugs.length() != 1 ) {
	    DBG("\t\t" << lp.info() << " != 1 element");
	    continue;
	 }
	 const MObject& lgtNode = plugs[0].node();
	 

	 ///////////// ....Get object info...
	 MPlug op = ep.child(1);
	 op.connectedTo( plugs, true, false );
	 if ( plugs.length() != 1 ) {
	    DBG("\t\t" << p.info() << " != 1 element");
	    continue;
	 }
	 const MObject& objNode = plugs[0].node();


	 if ( lgtNode.hasFn( MFn::kSet ) )
	 {
	    MFnSet fnset( lgtNode );
	    MSelectionList sel;
	    fnset.getMembers( sel, true );

	    DBG("\tFound light set: " << fnset.name() );

	    MDagPath path;
	    MItSelectionList it( sel );
	    for ( ; !it.isDone(); it.next() )
	    {
	       it.getDagPath( path );
	       DBG("\t" << path.fullPathName() << " in set");
	       addLight( linkedSets, linkedObjects, path, objNode );
	    }
	 }
	 else if ( lgtNode.hasFn( MFn::kLight ) )
	 {
	    MDagPath shape;
	    MDagPath::getAPathTo( lgtNode, shape );
	    addLight( linkedSets, linkedObjects, shape, objNode );
	 }
	 else
	 {
	    LOG_ERROR("Unknown light connection");
	    continue;
	 }
      }
   }
      
   p = fn.findPlug( "ignore", &status );
   numElements = p.evaluateNumElements();
   DBG("\tIGNORE Elements " << numElements);
   if ( numElements )
   {
      MPlugArray plugs;
      for ( unsigned i = 0; i < numElements; ++i)
      {
	 MPlug ep = p.elementByPhysicalIndex(i);
	 if (ep.numConnectedChildren() != 2) continue;

	 MPlug lp = ep.child(0);
	 lp.connectedTo( plugs, true, false );
	 if ( plugs.length() != 1 ) {
	    DBG("\t\t" << lp.info() << " != 1 element");
	    continue;
	 }
	 const MObject& lgtNode = plugs[0].node();
	 

	 ///////////// ....Get object info...
	 MPlug op = ep.child(1);
	 op.connectedTo( plugs, true, false );
	 if ( plugs.length() != 1 ) {
	    DBG("\t\t" << p.info() << " != 1 element");
	    continue;
	 }
	 const MObject& objNode = plugs[0].node();


	 if ( lgtNode.hasFn( MFn::kSet ) )
	 {
	    MFnSet fnset( lgtNode );
	    MSelectionList sel;
	    fnset.getMembers( sel, true );
	    DBG("\tFound light set: " << fnset.name() );
	    MDagPath path;
	    MItSelectionList it( sel );
	    for ( ; !it.isDone(); it.next() )
	    {
	       it.getDagPath( path );
	       addLight( ignoredSets, ignoredObjects, path, objNode );
	    }
	 }
	 else if ( lgtNode.hasFn( MFn::kLight ) )
	 {
	    MDagPath path;
	    MDagPath::getAPathTo( lgtNode, path );
	    addLight( ignoredSets, ignoredObjects, path, objNode );
	 }
	 else
	 {
	    LOG_ERROR("Unknown light connection");
	    continue;
	 }
      }
   }


   return status;
}


void mrLightLinks::getLightLinks()
{
  lightLinks.clear();

  MStatus status;
  MItDependencyNodes dgIterator( MFn::kLightLink );
  for (; !dgIterator.isDone(); dgIterator.next())
    {
#if MAYA_API_VERSION >= 700
      const MObject& obj = dgIterator.thisNode();
#else
      const MObject& obj = dgIterator.item();
#endif
      lightLinks.append( obj );
    }




#if MAYA_API_VERSION < 900
   bool oldExportLightLinkerNow = options->exportLightLinkerNow;

   unsigned numLightLinks = lightLinks.length();
   for (unsigned lk = 0; lk < numLightLinks; ++lk )
   {
      MFnDependencyNode dep( lightLinks[lk] );
      DBG("checking " << dep.name());
      
      MPlug p = dep.findPlug( "link", true, &status );
      unsigned numElements = p.evaluateNumElements();
      DBG("\tlink elements " << numElements);
      if ( numElements )
      {
	 MPlugArray plugs;
	 for ( unsigned i = 0; i < numElements; ++i)
	 {
	    MPlug ep = p.elementByPhysicalIndex(i);
	    if (ep.numConnectedChildren() != 2) continue;

	    MPlug lp = ep.child(0);
	    lp.connectedTo( plugs, true, false );
	    if ( plugs.length() != 1 ) {
	       DBG("\t\t" << lp.info() << " != 1 element");
               continue;
	    }
	    if ( plugs[0].info() == "defaultLightSet.message" )
	    {
	       DBG("\t\t" << plugs[0].info() << " is " << 
		   plugs[0].node().apiTypeStr());
	       continue;
	    }

#if 0
	    if ( !plugs[0].node().hasFn( MFn::kLight ) ) {
	      DBG("\t\t" << plugs[0].info() << " != light");
	      DBG("\t\t" << plugs[0].info() << " is " << plugs[0].node().apiTypeStr());
              continue;
	    }
	    
	    lp = ep.child(1);
	    lp.connectedTo( plugs, true, false );
	    if ( plugs.length() != 1 ) {
	       DBG("\t\t" << lp.info() << " != 1 element");
               continue;
	    }
	    if ( !plugs[0].node().hasFn( MFn::kDagNode ) ) {
	       DBG("\t\t" << plugs[0].info() << " != dagnode");
	       DBG("\t\t" << plugs[0].info() << " is " <<
		   plugs[0].node().apiTypeStr());
	       continue;
	    }
#endif

	    /// YIPEEE  User is indeed using light linking
	    options->exportLightLinkerNow = true;
	    break;
	 }
	 if ( options->exportLightLinkerNow ) break;
      }
      
      p = dep.findPlug( "ignore", true, &status );
      numElements = p.evaluateNumElements();
      DBG("\tignore elements " << numElements);
      if ( numElements )
      {
	 MPlugArray plugs;
	 for ( unsigned i = 0; i < numElements; ++i)
	 {
	    MPlug ep = p.elementByPhysicalIndex(i);
	    if (ep.numConnectedChildren() != 2) continue;
	    
	    MPlug lp = ep.child(0);
	    lp.connectedTo( plugs, true, false );
	    if ( plugs.length() != 1 ) {
	       DBG("\t\t" << lp.info() << " != 1 element");
               continue;
	    }

#if 0
	    if ( (!plugs[0].node().hasFn( MFn::kLight )) &&
		 (!plugs[0].node().hasFn( MFn::kSet ) ) ) {
	       DBG("\t\t" << plugs[0].info() << " != light");
	       DBG("\t\t" << plugs[0].info() << " is " << 
		   plugs[0].node().apiTypeStr());
	       continue;
	    }
	    
	    lp = ep.child(1);
	    lp.connectedTo( plugs, true, false );
	    if ( plugs.length() != 1 ) {
	       DBG("\t\t" << lp.info() << " != 1 element");
               continue;
	    }
	    if ( (!plugs[0].node().hasFn( MFn::kDagNode )) &&
		 (!plugs[0].node().hasFn( MFn::kSet )) ) {
	       DBG("\t\t" << plugs[0].info() << " != dagnode");
	       DBG("\t\t" << plugs[0].info() << " is " << 
		   plugs[0].node().apiTypeStr());
	       continue;
	    }
#endif
	    
	    /// YIPEEE  User is indeed using light linking
	    options->exportLightLinkerNow = true;
	    break;
	 }
	 if ( options->exportLightLinkerNow ) break;
      }


#if MAYA_API_VERSION >= 800
      p = dep.findPlug( "shadowLink", true, &status );
      numElements = p.evaluateNumElements();
      DBG("\tlink elements " << numElements);
      if ( numElements )
      {
	 MPlugArray plugs;
	 for ( unsigned i = 0; i < numElements; ++i)
	 {
	    MPlug ep = p.elementByPhysicalIndex(i);
	    if (ep.numConnectedChildren() != 2) continue;

	    MPlug lp = ep.child(0);
	    lp.connectedTo( plugs, true, false );
	    if ( plugs.length() != 1 ) {
	       DBG("\t\t" << lp.info() << " != 1 element");
               continue;
	    }
	    if ( plugs[0].info() == "defaultLightSet.message" )
	    {
	       DBG("\t\t" << plugs[0].info() << " is " << 
		   plugs[0].node().apiTypeStr());
	       continue;
	    }

#if 0
	    if ( !plugs[0].node().hasFn( MFn::kLight ) ) {
	      DBG("\t\t" << plugs[0].info() << " != light");
	      DBG("\t\t" << plugs[0].info() << " is " << plugs[0].node().apiTypeStr());
              continue;
	    }
	    
	    lp = ep.child(1);
	    lp.connectedTo( plugs, true, false );
	    if ( plugs.length() != 1 ) {
	       DBG("\t\t" << lp.info() << " != 1 element");
               continue;
	    }
	    if ( !plugs[0].node().hasFn( MFn::kDagNode ) ) {
	       DBG("\t\t" << plugs[0].info() << " != dagnode");
	       DBG("\t\t" << plugs[0].info() << " is " <<
		   plugs[0].node().apiTypeStr());
	       continue;
	    }
#endif

	    /// YIPEEE  User is indeed using light linking
	    options->exportLightLinkerNow = true;
	    break;
	 }
	 if ( options->exportLightLinkerNow ) break;
      }
      
      p = dep.findPlug( "shadowIgnore", true, &status );
      numElements = p.evaluateNumElements();
      DBG("\tignore elements " << numElements);
      if ( numElements )
      {
	 MPlugArray plugs;
	 for ( unsigned i = 0; i < numElements; ++i)
	 {
	    MPlug ep = p.elementByPhysicalIndex(i);
	    if (ep.numConnectedChildren() != 2) continue;
	    
	    MPlug lp = ep.child(0);
	    lp.connectedTo( plugs, true, false );
	    if ( plugs.length() != 1 ) {
	       DBG("\t\t" << lp.info() << " != 1 element");
               continue;
	    }

#if 0
	    if ( (!plugs[0].node().hasFn( MFn::kLight )) &&
		 (!plugs[0].node().hasFn( MFn::kSet ) ) ) {
	       DBG("\t\t" << plugs[0].info() << " != light");
	       DBG("\t\t" << plugs[0].info() << " is " << 
		   plugs[0].node().apiTypeStr());
	       continue;
	    }
	    
	    lp = ep.child(1);
	    lp.connectedTo( plugs, true, false );
	    if ( plugs.length() != 1 ) {
	       DBG("\t\t" << lp.info() << " != 1 element");
               continue;
	    }
	    if ( (!plugs[0].node().hasFn( MFn::kDagNode )) &&
		 (!plugs[0].node().hasFn( MFn::kSet )) ) {
	       DBG("\t\t" << plugs[0].info() << " != dagnode");
	       DBG("\t\t" << plugs[0].info() << " is " << 
		   plugs[0].node().apiTypeStr());
	       continue;
	    }
#endif
	    
	    /// YIPEEE  User is indeed using light linking
	    options->exportLightLinkerNow = true;
	    break;
	 } // for-loop

	 if ( options->exportLightLinkerNow ) break;

      } // if ( numElements )

#endif  // MAYA_API_VERSION >= 800

   } // for-loop numLightLinks

   DBG( "exportLightLinkerNow: " << (int) options->exportLightLinkerNow );

   // If light linker state changes, re-spit all materials...
   if ( oldExportLightLinkerNow != options->exportLightLinkerNow )
   {
      DBG("light linker state changed, re-spit surface/volume/shadow shaders");
      mrShadingGroupList::iterator i = sgList.begin();
      mrShadingGroupList::iterator e = sgList.end();
      for ( ; i != e; ++i )
      {
	 mrShadingGroup* sg = i->second;
	 mrShader* shader = sg->surface;
	 if ( shader ) {
	    DBG("force incremental: " << shader->name);
	    shader->forceIncremental();
	 }
         shader = sg->volume;
	 if ( shader ) {
	    DBG("force incremental: " << shader->name);
	    shader->forceIncremental();
	 }
         shader = sg->shadow;
	 if ( shader ) {
	    DBG("force incremental: " << shader->name);
	    shader->forceIncremental();
	 }
      }
   }

#endif  // MAYA_API_VERSION < 900

}

void mrLightLinks::parse(const mrGroupInstance* allLights)
{
  reset();


  unsigned numLightLinks = lightLinks.length();
  if ( options->lightLinkerType == mrOptions::kLightLinkerShaderSets )
    {
      mrTRACE("++++ Updating Light Links SETS");
      for ( unsigned i = 0; i < numLightLinks; ++i )
	parseLinks( lightLinks[i] );
    }
  else
    {
      for ( unsigned i = 0; i < numLightLinks; ++i )
	parseLinks( lightLinks[i], allLights );
    }
}

void mrLightLinks::isIgnored( MStringArray& ignoredLights, 
			      const MDagPath& object )
{
  mrLinks::iterator i = ignoredObjects.begin();
  mrLinks::iterator e = ignoredObjects.end();
  for ( ; i != e; ++i )
    {
      MSelectionList& sel = *(i->second);
      if ( !sel.hasItem( object ) ) continue;

      ignoredLights.append( i->first );
    }
}

void mrLightLinks::isShadowIgnored( MStringArray& ignoredLights, 
				    const MDagPath& object )
{
  mrLinks::iterator i = shadowIgnoredObjects.begin();
  mrLinks::iterator e = shadowIgnoredObjects.end();
  for ( ; i != e; ++i )
    {
      MSelectionList& sel = *(i->second);
      if ( !sel.hasItem( object ) ) continue;

      ignoredLights.append( i->first );
    }
}


#ifdef GEOSHADER_H
#  include "raylib/mrLightLinks.inl"
#endif
