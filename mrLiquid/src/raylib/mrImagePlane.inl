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



void mrImagePlane::write_properties( miObject* o )
{
   o->shadow = miFALSE;
   o->shadowmap = miFALSE;
   mrObject::write_properties( o );
}



void mrImagePlane::write_group()
{   
   MVectorArray pts(4);
   pts[0].x = offsetX - x; pts[0].y = offsetY - y; 
   pts[1].x = offsetX + x; pts[1].y = offsetY - y; 
   pts[2].x = offsetX + x; pts[2].y = offsetY + y; 
   pts[3].x = offsetX - x; pts[3].y = offsetY + y; 
   pts[0].z = pts[1].z = pts[2].z = pts[3].z = z;

   int i;
   if ( rotate != 0 )
   {
      for ( i = 0; i < 4; ++i)
      {
	 pts[i] = pts[i].rotateBy( MVector::kZaxis, -rotate );
      }
   }

   MMatrix m = c->path.inclusiveMatrix();
   for (i = 0; i < 4; ++i)
      pts[i] = pts[i] * m;

   write_vectors(pts);

   MVectorArray uvs(4);
   uvs[0].x = uvs[3].x = uMin;
   uvs[1].x = uvs[2].x = uMax;
   uvs[0].y = uvs[1].y = vMin;
   uvs[2].y = uvs[3].y = vMax;
   write_vectors(uvs);

   MRL_CHECK(mi_api_vertex_add(0));
   MRL_CHECK(mi_api_vertex_tex_add(4, -1, -1));
   MRL_CHECK(mi_api_vertex_add(1));
   MRL_CHECK(mi_api_vertex_tex_add(5, -1, -1));
   MRL_CHECK(mi_api_vertex_add(2));
   MRL_CHECK(mi_api_vertex_tex_add(6, -1, -1));
   MRL_CHECK(mi_api_vertex_add(3));
   MRL_CHECK(mi_api_vertex_tex_add(7, -1, -1));

   MRL_CHECK(mi_api_poly_begin( 1, NULL ));  // convex polygon
   MRL_CHECK(mi_api_poly_index_add( 0 ));    // material id
   MRL_CHECK(mi_api_poly_index_add( 0 ));
   MRL_CHECK(mi_api_poly_index_add( 1 )); 
   MRL_CHECK(mi_api_poly_index_add( 2 ));
   MRL_CHECK(mi_api_poly_index_add( 3 )); 
   MRL_CHECK(mi_api_poly_end());
}
