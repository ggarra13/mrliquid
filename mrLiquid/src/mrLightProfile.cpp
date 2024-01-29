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


#include "mrLightProfile.h"

#include "mrHelpers.h"
#include "mrOptions.h"

#include "mrAttrAux.h"

void mrLightProfile::getData()
{
   MStatus status;
   MPlug p;
   MFnDependencyNode fn( nodeRef() );

   GET_ENUM( format, mrLightProfile::Format );
   GET_ENUM( interpolation, mrLightProfile::Interpolation );
   GET( resolutionX );
   GET( resolutionY );
   GET( iesBClockwise );
   GET( fileName );
   fileName = parseString( fileName );
}

void mrLightProfile::setPath()
{
   char path = options->exportPathNames[mrOptions::kLightProfilePath];
   if ( path == 'a' ) return;

   int idx = fileName.rindex('/');
   if ( idx < 0 ) return;

   fileName = fileName.substring( idx+1, fileName.length() - 1 );
}

mrLightProfile::~mrLightProfile()
{
}

mrLightProfile::mrLightProfile( const MFnDependencyNode& fn ) :
mrNode( fn ),
nodeHandle( fn.object() )
{
}


void mrLightProfile::forceIncremental()
{
   getData();
}

void mrLightProfile::setIncremental( bool sameFrame )
{
   getData();
}

mrLightProfile* mrLightProfile::factory( const MFnDependencyNode& fn )
{
   mrNodeList::iterator i = nodeList.find( fn.name() );
   if ( i != nodeList.end() )
      return dynamic_cast<mrLightProfile*>( i->second );

   mrLightProfile* p = new mrLightProfile( fn );
   nodeList.insert( p );
   return p;
}

void mrLightProfile::write( MRL_FILE* f )
{
   MRL_FPRINTF(f, "lightprofile \"%s\"\n", name.asChar());
   TAB(1); MRL_PUTS("format ");
   switch( format )
   {
      case kIES:
	 MRL_PUTS("ies\n");
	 break;
      case kEulumdat:
	 MRL_PUTS("eulumdat");
	 break;
   }
   TAB(1); MRL_FPRINTF( f, "file \"%s\"\n", fileName.asChar() );
   int flags = 0;
   if ( iesBClockwise ) flags |= 1;
   if ( flags )
   {
      TAB(1);
      MRL_FPRINTF(f, "flags %d\n", flags);
   }
   TAB(1); MRL_FPRINTF(f, "resolution %d %d", resolutionX, resolutionY );
   MRL_PUTS("end lightprofile");
}


#ifdef GEOSHADER_H
#include "raylib/mrLightProfile.inl"
#endif
