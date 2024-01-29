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

#include "mrIds.h"
#include "cylindricalLightLocator.h"


MTypeId cylindricalLightLocator::id( kCylindricalLightLocator );

unsigned       cylindricalLightLocator::num = 0;
GLUquadricObj* cylindricalLightLocator::glu = NULL;



cylindricalLightLocator::cylindricalLightLocator()
{
   ++num;
   if ( glu == NULL )
   {
      glu = gluNewQuadric();
      gluQuadricDrawStyle( glu, GLU_SILHOUETTE );
   }
}


cylindricalLightLocator::~cylindricalLightLocator()
{
   --num;
   if ( num == 0 )
   {
      gluDeleteQuadric(glu); glu = NULL;
   }
}



void cylindricalLightLocator::draw( M3dView & view, const MDagPath & path, 
			     M3dView::DisplayStyle style,
			     M3dView::DisplayStatus status )
{
   view.beginGL();

   if ( status == M3dView::kDormant )
   {
      view.setDrawColor( 11 );
   }

   glTranslatef( -1, 0, 0);
   glRotatef( 90.0f, 0, 1, 0);
   gluCylinder( glu, 1, 1, 2, 32, 1 );
   
   view.endGL();
}




bool            cylindricalLightLocator::isBounded() const
{
   return true;
}


MBoundingBox    cylindricalLightLocator::boundingBox() const
{
   return MBoundingBox( MPoint(-1,-1,-1), MPoint(1, 1, 1));
}

  
void *          cylindricalLightLocator::creator()
{
   return new cylindricalLightLocator;
}


MStatus         cylindricalLightLocator::initialize()
{
   return MS::kSuccess;
}

