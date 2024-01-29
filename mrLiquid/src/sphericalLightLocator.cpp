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
#include "sphericalLightLocator.h"



MTypeId sphericalLightLocator::id( kSphericalLightLocator );

unsigned      sphericalLightLocator::num = 0;
GLUnurbsObj*  sphericalLightLocator::glu = NULL;

// Knot list
static const GLfloat knots[] = {
0,1,2,3,4,5,6,7,8,9,10,11,12,13
};

// CV list
static const GLfloat CVs[] = {
1.039f, -0.6f, 0,
0, -1.2f, 0,
-1.039f, -0.6f, 0,
-1.039f,  0.6f, 0,
0, 1.2f, 0,
1.039f, 0.6f, 0,
1.039f, -0.6f, 0,
0, -1.2f, 0,
-1.039f, -0.6f, 0,
};



sphericalLightLocator::sphericalLightLocator()
{
   ++num;
   if ( glu == NULL )
   {
      glu = gluNewNurbsRenderer();

      gluNurbsProperty( glu, GLU_DISPLAY_MODE, GLU_OUTLINE_PATCH );
      gluNurbsProperty( glu, GLU_SAMPLING_METHOD, GLU_PATH_LENGTH );
      gluNurbsProperty( glu, GLU_SAMPLING_TOLERANCE, 50.0 );
   }
}


sphericalLightLocator::~sphericalLightLocator()
{
   --num;
   if ( num == 0 )
   {
      gluDeleteNurbsRenderer(glu); glu = NULL;
   }
}



void sphericalLightLocator::draw( M3dView & view, const MDagPath & path, 
			     M3dView::DisplayStyle style,
			     M3dView::DisplayStatus status )
{
   view.beginGL();

   if ( status == M3dView::kDormant )
   {
      view.setDrawColor( 11 );
   }
   
   gluBeginCurve( glu );
   gluNurbsCurve( glu, 13, (GLfloat*)knots,
		  3, (GLfloat*)CVs, 4, GL_MAP1_VERTEX_3 );
   gluEndCurve( glu );

   glRotatef( 45.0f, 0.0f, 1.0f, 0.0f );
   gluBeginCurve( glu );
   gluNurbsCurve( glu, 13, (GLfloat*)knots,
		  3, (GLfloat*)CVs, 4, GL_MAP1_VERTEX_3 );
   gluEndCurve( glu );
   
   glRotatef( 45.0f, 0.0f, 1.0f, 0.0f );
   gluBeginCurve( glu );
   gluNurbsCurve( glu, 13, (GLfloat*)knots,
		  3, (GLfloat*)CVs, 4, GL_MAP1_VERTEX_3 );
   gluEndCurve( glu );

   
   glRotatef( 45.0f, 0.0f, 1.0f, 0.0f );
   gluBeginCurve( glu );
   gluNurbsCurve( glu, 13, (GLfloat*)knots,
		  3, (GLfloat*)CVs, 4, GL_MAP1_VERTEX_3 );
   gluEndCurve( glu );
   
   
   glRotatef( 45.0f, 0.0f, 1.0f, 0.0f );
   glRotatef( 90.0f, 1.0f, 0.0f, 0.0f );
   gluBeginCurve( glu );
   gluNurbsCurve( glu, 13, (GLfloat*)knots,
		  3, (GLfloat*)CVs, 4, GL_MAP1_VERTEX_3 );
   gluEndCurve( glu );

   glScalef( 0.707f, 0.707f, 0.707f );
   glTranslatef( 0.0, 0.0, 1.0f );
   gluBeginCurve( glu );
   gluNurbsCurve( glu, 13, (GLfloat*)knots,
		  3, (GLfloat*)CVs, 4, GL_MAP1_VERTEX_3 );
   gluEndCurve( glu );

   glTranslatef( 0.0, 0.0, -2.0f );
   gluBeginCurve( glu );
   gluNurbsCurve( glu, 13, (GLfloat*)knots,
		  3, (GLfloat*)CVs, 4, GL_MAP1_VERTEX_3 );
   gluEndCurve( glu );
   
   view.endGL();
}




bool            sphericalLightLocator::isBounded() const
{
   return true;
}


MBoundingBox    sphericalLightLocator::boundingBox() const
{
   return MBoundingBox( MPoint(-1,-1,-1), MPoint(1, 1, 1));
}

  
void *          sphericalLightLocator::creator()
{
   return new sphericalLightLocator;
}


MStatus         sphericalLightLocator::initialize()
{
   return MS::kSuccess;
}
