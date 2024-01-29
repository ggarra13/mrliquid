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

#include "maya/MVectorArray.h"
#include "maya/MPointArray.h"
#include "maya/MPoint.h"
#include "maya/MQuaternion.h"
#include "maya/MImage.h"
#include "maya/MTransformationMatrix.h"

#include "mrImagePlane.h"
#include "mrAttrAux.h"
#include "mrCamera.h"



void mrImagePlane::getData()
{
   MFnDependencyNode fn( nodeRef() );
   
   MStatus status; MPlug p;
   GET( rotate  );

   uMin = vMin = 0.0;
   uMax = vMax = 1.0;

   GET( lockedToCamera );

   int coverageX, coverageY;
   GET( coverageX );
   GET( coverageY );

   int coverageOriginX, coverageOriginY;
   GET( coverageOriginX );
   GET( coverageOriginY );

   int type;
   GET( type );
   unsigned int imageX = 1, imageY = 1;
   if ( type == 0 )
   {
      MString imageName;
      GET(imageName);
      MImage im;
      im.readFromFile( imageName );
      im.getSize( imageX, imageY );
      im.release();
   }


   double imageAspect = (double) imageX / (double) imageY;
   if ( imageAspect == 0 ) imageAspect = 1.0;


   double squeezeCorrection;
   GET( fit );
   if ( lockedToCamera )
   {
      GET_ATTR( x, sizeX );
      GET_ATTR( y, sizeY );
      GET_ATTR( z, depth );
      GET( offsetX );
      GET( offsetY );

      if ( fit == 1 )
      {
	 if ( imageAspect < 1 ) fit = 3;
	 else                   fit = 2;
      }

      switch( fit )
      {
	 case 0: // Fill
	    // what changes is the uv parameters
	    if ( x < ( y * imageAspect ) )
	    {
	       // what changes is the V parameter
	       double size  = y * imageAspect;
	       double udiff = 1.0 - ( x / size );
	       udiff *= 0.5;
	       uMin += udiff; 
	       uMax -= udiff; 
	    }
	    if ( y < (x / imageAspect) )
	    {
	       // what changes is the U parameter
	       double size  = x / imageAspect;
	       double vdiff = 1.0 - (y / size);
	       vdiff *= 0.5;
	       vMin += vdiff; 
	       vMax -= vdiff;
	    }
	    break;
	 case 2: // Horizontal
	    if ( y >= (x / imageAspect) )
	    {
	       y = x / imageAspect;
	    }
	    else
	    {
	       // what changes is the V parameter
	       double size  = x / imageAspect;
	       double vdiff = 1.0 - (y / size);
	       vdiff *= 0.5;
	       vMin += vdiff;
	       vMax -= vdiff;
	    }
	    break;
	 case 3: // Vertical
	    if ( x >= ( y * imageAspect ) )
	    {
	       x = y * imageAspect;
	    }
	    else
	    {
	       // what changes is the U parameter
	       double size  = y * imageAspect;
	       double udiff = 1.0 - ( x / size );
	       udiff *= 0.5;
	       uMin += udiff; 
	       uMax -= udiff; 
	    }
	    break;
	 case 4: // To User
	    GET( squeezeCorrection );
	    if ( squeezeCorrection > 0 )
	       x /= squeezeCorrection;
	    break;
	 default:
	    mrTHROW("Unknown Image Fit");
      }
      double ratio = z / c->focal;
      x *= ratio;
      y *= ratio;
      z = -z;
   }
   else
   {
      GET_ATTR( offsetX, centerX );
      GET_ATTR( offsetY, centerY );
      GET_ATTR( z,       centerZ );

      GET_ATTR( x, width );
      GET_ATTR( y, height );
      if ( y * imageAspect > x )
      {
	 y = x / imageAspect;
      }
      else
      {
	 x = y * imageAspect;
      }

      // Take the point from world space to camera space
      MPoint p( offsetX, offsetY, z );
      MMatrix m = c->path.inclusiveMatrixInverse();
      p *= m;
      offsetX = p.x;
      offsetY = p.y;
      z = p.z;
   }
   x *= 0.5f;
   y *= 0.5f;

   if ( type == 0 )
   { 
      if ( (unsigned)coverageX < imageX )
      {
	 double diff = 1.0 - (double) coverageX / (double)imageX;
	 uMax += diff;
      }
      if ( (unsigned)coverageY < imageY )
      {
	 double diff = 1.0 - (double) coverageX / (double)imageX;
	 vMax += diff;
      }

      if ( coverageOriginX != 0 )
      {
	 double diff = (double) coverageOriginX / (double)imageX;
	 if (coverageOriginX > 0)
	 {
	    uMin += diff;
	    if (uMin > uMax) uMin = uMax;
	 }
	 else
	 {
	    uMax += diff;
	    if (uMax < uMin) uMax = uMin;
	 }
      }
      if ( coverageOriginY != 0 )
      {
	 double diff = (double) coverageOriginY / (double)imageY;
	 if (coverageOriginY > 0)
	 {
	    vMin += diff;
	    if (vMin > vMax) vMin = vMax;
	 }
	 else
	 {
	    vMax += diff;
	    if (vMax < vMin) vMax = vMin;
	 }
      }
   }
   DBG("uMin: " << uMin << "  uMax: " << uMax);
   DBG("vMin: " << uMin << "  vMax: " << vMax);

   shadow = 0;
   shadowmap = 0;

   maxDisplace = 0;
}


mrImagePlane::mrImagePlane( const mrCamera* cam,
			    const MFnDependencyNode& fn ) :
mrObject( fn.name() + ":shape" ),
c( cam ),
nodeHandle( fn.object() )
{
   getData();
}


mrImagePlane* mrImagePlane::factory( const mrCamera* cam,
				     const MFnDependencyNode& fn )
{
   const MString& name = fn.name() + ":shape";
   mrNodeList::iterator i = nodeList.find( name );
   if ( i != nodeList.end() )
      return dynamic_cast<mrImagePlane*>( i->second );
   mrImagePlane* p = new mrImagePlane( cam, fn );
   nodeList.insert( p );
   return p;
}


void mrImagePlane::forceIncremental()
{
   getData();
   written = kIncremental;
}

void mrImagePlane::setIncremental(bool sameFrame)
{
   getData();
   if ( written == kWritten ) written = kIncremental;
}

void mrImagePlane::write_properties( MRL_FILE* f )
{
   TAB(1); MRL_PUTS("visible on\n");
   TAB(1); MRL_PUTS("trace on\n");
   TAB(1); MRL_PUTS("tagged\n");
}

void mrImagePlane::write_group( MRL_FILE* f )
{

   MPointArray pts(4);
   pts[0].x = offsetX - x; pts[0].y = offsetY - y;
   pts[1].x = offsetX + x; pts[1].y = offsetY - y;
   pts[2].x = offsetX + x; pts[2].y = offsetY + y;
   pts[3].x = offsetX - x; pts[3].y = offsetY + y;
   pts[0].z = pts[1].z = pts[2].z = pts[3].z = z;

   int i;
   if ( rotate != 0 )
   {
      MQuaternion q;
      q.setToZAxis( -rotate );
      for ( i = 0; i < 4; ++i)
	 pts[i] *= q;
   }

   MMatrix m = c->path.inclusiveMatrix();
   for (i = 0; i < 4; ++i)
      pts[i] *= m;


   write_vectors(f, pts);

   MVectorArray uvs(4);
   uvs[0].x = uvs[3].x = uMin;
   uvs[1].x = uvs[2].x = uMax;
   uvs[0].y = uvs[1].y = vMin;
   uvs[2].y = uvs[3].y = vMax;
   write_vectors(f, uvs);

   TAB(2); MRL_PUTS("v 0 t 4\n");
   TAB(2); MRL_PUTS("v 1 t 5\n");
   TAB(2); MRL_PUTS("v 2 t 6\n");
   TAB(2); MRL_PUTS("v 3 t 7\n");

   TAB(2); MRL_PUTS("c 0 0 1 2 3\n");
}




#ifdef GEOSHADER_H
#include "raylib/mrImagePlane.inl"
#endif

