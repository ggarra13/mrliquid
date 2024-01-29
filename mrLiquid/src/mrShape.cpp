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

#include "maya/MFnDagNode.h"
#include "maya/MPlug.h"

#include "mrShape.h"
#include "mrAttrAux.h"


extern MDagPath currentObjPath;


mrNodeList::iterator mrShape::find_iterator( const MDagPath& shape )
{
   if ( shape.isInstanced() )
   {
      MDagPath firstShape;
      MDagPath::getAPathTo( shape.node(), firstShape );
      const MString& name = getMrayName( firstShape );
      return nodeList.find( name );
   }

   const MString& name = getMrayName( shape );
   return nodeList.find( name );
}

mrShape* mrShape::find( const MDagPath& shape )
{
   mrNodeList::iterator i = find_iterator( shape );
   if ( i != nodeList.end() )
   {
#ifdef DEBUG
      return dynamic_cast< mrShape* >( i->second );
#else
      return static_cast< mrShape* >( i->second );
#endif
   }
   return NULL;
}

mrShape* mrShape::find( const MString& name )
{
   mrNodeList::iterator i = nodeList.find( name );
   if ( i != nodeList.end() )
   {
#ifdef DEBUG
      return dynamic_cast< mrShape* >( i->second );
#else
      return static_cast< mrShape* >( i->second );
#endif
   }
   return NULL;
}


mrShape::mrShape( const MString& s ) :
mrNode( s ),
shapeAnimated( false )
{
}

mrShape::mrShape( const MDagPath& p ) :
mrNode( p ),
path( p ),
shapeAnimated( false )
{
   DBG("Creating shape " << name);
}


void mrShape::debug( int tabs ) const throw()
{
   for (int i = 0; i < tabs; ++i)
      cerr << "\t";
   cerr << "+ SHAPE " << this << "  name: " << name << endl;
}

void mrShape::forceIncremental()
{
   DBG(name << " mrShape::forceIncremental  written= " << written);
   currentObjPath = path;  // for $INST expressions
   if ( written == kWritten ) written = kIncremental;
   DBG(name << " mrShape::forceIncremental END written= " << written);
   // no need for mrNode::forceIncremental() -- 
   // let's save a function call.
}


void mrShape::setIncremental( bool sameFrame )
{
   currentObjPath = path;  // for $INST expressions
   if ( shapeAnimated && written == kWritten ) 
      written = kIncremental;
   DBG(name << "  mrShape::setIncremental  animated? " << shapeAnimated);
   DBG(name << "  mrShape::setIncremental  written = " << written);
}

