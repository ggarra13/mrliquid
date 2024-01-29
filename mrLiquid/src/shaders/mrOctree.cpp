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

#if defined(WIN32) || defined(WIN64)
#include <float.h>

#define finite(x) _finite(x)
#endif

#include <limits>
#include <algorithm>

#include "mrOctree.h"
#include "mrGenerics.h"

unsigned mrOctree::maxElements = 20;
unsigned mrOctree::maxDepth    = 8;  // max. nodes = 8 ^ 8 = 16,772,216

#ifdef MR_OCTREE_STATS
unsigned mrOctree::numLevels   = 0;  // max. nodes = 8 ^ 8 = 16,772,216
#endif

//! Functor used to sort HitList
struct HitListSorter
{
     bool operator()( const ParticleIntersection* a,
		      const ParticleIntersection* b )
     {
	return a->t < b->t;
     }
};


void mrOctree::getChildCenter( miVector& childCenter, const childIds id ) const
{
   miVector size4 = {
   size.x * 0.25f,
   size.y * 0.25f,
   size.z * 0.25f
   };
   
   switch(id)		
   {
      case kTOP_LEFT_FRONT:
	 childCenter.x = center.x - size4.x;
	 childCenter.y = center.y + size4.y;
	 childCenter.z = center.z + size4.z;
	 break;
      case kTOP_LEFT_BACK:
	 childCenter.x = center.x - size4.x;
	 childCenter.y = center.y + size4.y;
	 childCenter.z = center.z - size4.z;
	 break;
      case kTOP_RIGHT_BACK:
	 childCenter.x = center.x + size4.x;
	 childCenter.y = center.y + size4.y;
	 childCenter.z = center.z - size4.z;
	 break;
      case kTOP_RIGHT_FRONT:
	 childCenter.x = center.x + size4.x;
	 childCenter.y = center.y + size4.y;
	 childCenter.z = center.z + size4.z;
	 break;
      case kBOTTOM_LEFT_FRONT:
	 childCenter.x = center.x - size4.x;
	 childCenter.y = center.y - size4.y;
	 childCenter.z = center.z + size4.z;
	 break;
      case kBOTTOM_LEFT_BACK:
	 childCenter.x = center.x - size4.x;
	 childCenter.y = center.y - size4.y;
	 childCenter.z = center.z - size4.z;
	 break;
      case kBOTTOM_RIGHT_BACK:
	 childCenter.x = center.x + size4.x;
	 childCenter.y = center.y - size4.y;
	 childCenter.z = center.z - size4.z;
	 break;
      case kBOTTOM_RIGHT_FRONT:
	 childCenter.x = center.x + size4.x;
	 childCenter.y = center.y - size4.y;
	 childCenter.z = center.z + size4.z;
	 break;
   }
}
     

bool mrOctree::hasPoint(
			const mrBox< miVector >& box,
			const miVector& p
			)
{
   const miVector& bMin = box.min();
   const miVector& bMax = box.max();
   if ( p.x < bMin.x || p.x > bMax.x ||
	p.y < bMin.y || p.y > bMax.y ||
	p.z < bMin.z || p.z > bMax.z  )
      return false;
   return true;
}


mrOctreeParticles::mrOctreeParticles(
				     const miVectorList& pts,
				     const RadiiList& radius,
				     const miVectorList& vel
				     ) :
mrOctree()
{
   assert( pts.size() == radius.size() );
   if ( pts.empty() )
   {
      center.x = center.y = center.z =
      size.x = size.y = size.z = 0.0f;
      return;
   }

   miVector bMin = { pts[0].x - radius[0], pts[0].y - radius[0],
		     pts[0].z - radius[0] };
   miVector bMax = { pts[0].x + radius[0], pts[0].y + radius[0],
		     pts[0].z + radius[0] };
   if ( !vel.empty() )
   {
      bMax.x += vel[0].x;
      bMax.y += vel[0].y;
      bMax.z += vel[0].z;

      if ( bMax.x < bMin.x ) {
	 std::swap( bMin.x, bMax.x );
	 bMin.x -= 2 * radius[0];
      }
      if ( bMax.y < bMin.y ) {
	 std::swap( bMin.y, bMax.y );
	 bMin.y -= 2 * radius[0];
      }
      if ( bMax.z < bMin.z ) {
	 std::swap( bMin.z, bMax.z );
	 bMin.z -= 2 * radius[0];
      }
   }

   // Calculate center and size of bbounding box of particles
   size_t numPts = pts.size();
   for ( size_t j = 1; j < numPts; ++j )
   {
      miVector pMin = { pts[j].x - radius[j], pts[j].y - radius[j],
			pts[j].z - radius[j] };
      miVector pMax = { pts[j].x + radius[j], pts[j].y + radius[j],
			pts[j].z + radius[j] };
      if ( !vel.empty() )
      {
	 pMax.x += vel[j].x;
	 pMax.y += vel[j].y;
	 pMax.z += vel[j].z;
	 
	 if ( pMax.x < pMin.x ) {
	    std::swap( pMin.x, pMax.x );
	    pMin.x -= 2 * radius[j];
	 }
	 if ( pMax.y < pMin.y ) {
	    std::swap( pMin.y, pMax.y );
	    pMin.y -= 2 * radius[j];
	 }
	 if ( pMax.z < pMin.z ) {
	    std::swap( pMin.z, pMax.z );
	    pMin.z -= 2 * radius[j];
	 }
      }

      if ( pMax.x > bMax.x ) bMax.x = pMax.x;
      if ( pMin.x < bMin.x ) bMin.x = pMin.x;
      
      if ( pMax.y > bMax.y ) bMax.y = pMax.y;
      if ( pMin.y < bMin.y ) bMin.y = pMin.y;
      
      if ( pMax.z > bMax.z ) bMax.z = pMax.z;
      if ( pMin.z < bMin.z ) bMin.z = pMin.z;
   }

   size.x = ( bMax.x - bMin.x );
   size.y = ( bMax.y - bMin.y );
   size.z = ( bMax.z - bMin.z );

   center.x = bMin.x + size.x * 0.5f; 
   center.y = bMin.y + size.y * 0.5f; 
   center.z = bMin.z + size.z * 0.5f; 

   bool hasBig[8];
   ParticleIndices childParticleIndices[8];
   for ( int i = 0; i < 8; ++i )
   {
      hasBig[i] = false;
      childParticleIndices[i].reserve( numPts );
   }
   subdivide( childParticleIndices, pts, radius, vel );
   createChildren( 0, childParticleIndices, hasBig, pts, radius, vel );

#ifdef MR_OCTREE_STATS
   mi_progress("Octree numLevels: %d", numLevels);
#endif
}


mrOctreeParticles::mrOctreeParticles(
				     const unsigned level,
				     const ParticleIndices& idx,
				     const miVectorList& pts,
				     const RadiiList& radius,
				     const miVectorList& vel,
				     const miVector& ctr,
				     const miVector& sz
				     ) :
mrOctree(ctr, size)
{
   center = ctr;
   size = sz;
   
   assert( pts.size() == radius.size() );
   ParticleIndices childParticleIndices[8];
   size_t numPts = idx.size();
   bool hasBig[8];
   for ( int i = 0; i < 8; ++i )
   {
      hasBig[i] = false;
      childParticleIndices[i].reserve( numPts );
   }
   subdivide( childParticleIndices, hasBig, idx, pts, radius, vel );
   createChildren( level, childParticleIndices, hasBig, pts, radius, vel );
}


void mrOctreeParticles::getChildBBox(
				     mrBox< miVector >& box,
				     const miVector& w,
				     const int childId
				     ) const
{
   miVector c;
   getChildCenter( c, (mrOctree::childIds) childId );

   box.center = c;
   box.size   = w;
}




bool mrOctreeParticles::hasParticle(
				    const mrBox< miVector >& box,
				    const miVector& part,
				    const float r,
				    const miVector& vel
				    )
{
   // @todo:  make proper capsule/AABB intersection here, instead of this crap
   miVector partCtr = part;
   partCtr.x += vel.x * 0.5f; 
   partCtr.y += vel.y * 0.5f; 
   partCtr.z += vel.z * 0.5f;
   
   float r2 = r + 0.5f * mi_vector_norm(&vel);
   return hasParticle( box, part, r2 );
}



bool mrOctreeParticles::hasParticle(
				    const mrBox< miVector >& box,
				    const miVector& part,
				    const float r
				    )
{
   miVector bMin = box.min();
   miVector bMax = box.max();
   
   const miScalar* Bmin = (miScalar*) &bMin;
   const miScalar* Bmax = (miScalar*) &bMax;
   const miScalar* C    = (miScalar*) &part;

   float r2   = r * r;
   float dmin = 0;
   for( int i = 0; i < 3; ++i )
   {
      if( C[i] < Bmin[i] )
      {
	 float t = C[i] - Bmin[i];
	 dmin += t * t;
      }
      else if( C[i] > Bmax[i] )
      {
	 float t = C[i] - Bmax[i];
	 dmin += t * t;
      }
   }
   if( dmin <= r2 ) return true;
   return false;
}



void mrOctreeParticles::subdivide( 
				  ParticleIndices index[8],
				  bool  hasBig[8],
				  const ParticleIndices& idx,
				  const miVectorList& pts,
				  const RadiiList& radius,
				  const miVectorList& vel
				  )
{   
   size_t numIds = idx.size();

   miVector childSize = {
   size.x * 0.5f,
   size.y * 0.5f,
   size.z * 0.5f
   };
   
   mrBox< miVector > box;

   if ( vel.empty() )
   {
      for (int i = 0; i < 8; ++i )
      {
	 getChildBBox( box, childSize, i );
	 
	 for ( size_t j = 0; j < numIds; ++j )
	 {
	    unsigned id = idx[j];
	    if ( ! hasParticle( box, pts[id], radius[id] ) ) continue;
	    // hasBig is a simple heuristic to avoid trying to subdivide a node
	    // that fully contains a big particle.
	    if ( ! hasPoint( box, pts[id] ) )
	    {
	       if ( radius[id] > childSize.x && radius[id] > childSize.y &&
		    radius[id] > childSize.z )
		  hasBig[i] = true;
	    }
	    index[i].push_back( id );
	 }
      }
   }
   else
   {
      for (int i = 0; i < 8; ++i )
      {
	 getChildBBox( box, childSize, i );
	 
	 for ( size_t j = 0; j < numIds; ++j )
	 {
	    unsigned id = idx[j];
	    if ( ! hasParticle( box, pts[id], radius[id], vel[id] ) ) continue;
	    // hasBig is a simple heuristic to avoid trying to subdivide a node
	    // that fully contains a big particle.
	    if ( ! hasPoint( box, pts[id] ) )
	    {
	       if ( radius[id] > childSize.x && radius[id] > childSize.y &&
		    radius[id] > childSize.z )
		  hasBig[i] = true;
	    }
	    index[i].push_back( id );
	 }
      }
   }
}


void mrOctreeParticles::subdivide( 
				  ParticleIndices index[8],
				  const miVectorList& pts,
				  const RadiiList& radius,
				  const miVectorList& vel
				  )
{
   size_t numPts = pts.size();

   miVector childSize = {
   size.x * 0.5f,
   size.y * 0.5f,
   size.z * 0.5f
   };
   
   mrBox< miVector > box;

   if ( vel.empty() )
   {
      for (int i = 0; i < 8; ++i )
      {
	 getChildBBox( box, childSize, i );
	 
	 for ( size_t j = 0; j < numPts; ++j )
	 {
	    if ( ! hasParticle( box, pts[j], radius[j] ) ) continue;
	    index[i].push_back( j );
	 }
      }
   }
   else
   {
      for (int i = 0; i < 8; ++i )
      {
	 getChildBBox( box, childSize, i );
	 
	 for ( size_t j = 0; j < numPts; ++j )
	 {
	    if ( ! hasParticle( box, pts[j], radius[j], vel[j] ) ) continue;
	    index[i].push_back( j );
	 }
      }
   }
}


   
void
mrOctreeParticles::createChildren( 
				  const unsigned level,
				  const ParticleIndices index[8],
				  const bool           hasBig[8],  
				  const miVectorList& pts,
				  const RadiiList& radius,
				  const miVectorList& vel
				  )
{
   miVector childSize = {
   size.x * 0.5f,
   size.y * 0.5f,
   size.z * 0.5f
   };
   
#ifdef MR_OCTREE_STATS
   if ( level > numLevels ) numLevels = level;
#endif

   for (int i = 0; i < 8; ++i )
   {
      size_t numParts = index[i].size();
      if ( numParts == 0 ) continue;

      miVector childCenter;
      getChildCenter( childCenter, (mrOctree::childIds) i );

       if ( hasBig[i] || numParts <= maxElements || level >= maxDepth )
	  //      if ( numParts <= maxElements || level >= maxDepth )
      {
	 child[i] = new mrOctreeParticlesLeaf( index[i], pts, radius, vel,
					       childCenter, childSize );
      }
      else
      {
	 child[i] = new mrOctreeParticles( level + 1, index[i], pts, radius, 
					   vel, childCenter, childSize );
      }

   }
}



/*
  
  This Octree Traversal code is taken from
  "An Efficient Parametric Algorithm for Octree Traversal"
  by J. Revelles, C. Urena, M. Lastra

  Octree node distribution for algorithm:
  
         *---*---*
	/ 3 / 7 /|
       *---*---*7*   (hidden octant is 1)
      / 2 / 6 /|/|
     *---*---*6*5*      +Y
     | 2 | 6 |/|/       ^  * +Z
     *---*---*4*        | /
     | 0 | 4 |/         |/
     *---*---*          *-----> +X

  Errors in paper:

     Pseudo-code for flipping of rays is wrong in paper.
     oct->sizeX - r.ox, instead of oct->centerX * 2 - r.ox.
     
*/

int next_node(double t1, int a, double t2, int b, double t3, int c)
{
  return( (t1<t2 && t1<t3) ? a : (t2 < t3 ? b : c) );
}

int first_node(const miVector& t0, const miVector& tm)
{
    int b = 0;
  
    // Select plane of cube we intersected
    if (t0.x > t0.y && t0.x > t0.z)
    { 
       // max(t0.x, t0.y, t0.z) is t0.x. Entry plane is YZ.
       if (tm.y < t0.x) b |= 2;
       if (tm.z < t0.x) b |= 1;
    }
    else if (t0.y > t0.z)
    {
       // max(t0.x, t0.y, t0.z) is t0.y. Entry plane is XZ.
       if (tm.x < t0.y) b |= 4;
       if (tm.z < t0.y) b |= 1;
    }
    else
    {
       // max(t0.x, t0.y, t0.z) is t0.z. Entry plane is XY.   
       if (tm.x < t0.z) b |= 4;
       if (tm.y < t0.z) b |= 2;
    }
    return b;
}


bool 
mrOctreeParticles::intersect(
			     HitList& hits,
			     miState* const state
			     ) const
{  
   using namespace mr;

   miVector dir = state->dir;
   miVector org = state->org;

   miVector halfsize = {
   size.x * 0.5f,
   size.y * 0.5f,
   size.z * 0.5f
   };
   
   miVector bottom = { center.x - halfsize.x,
		       center.y - halfsize.y,
		       center.z - halfsize.z
   };
   miVector top    = { center.x + halfsize.x,
		       center.y + halfsize.y,
		       center.z + halfsize.z
   };

   
   // Store bit-mask to flip ray if needed
   int a = 0;
   if (dir.x < 0.0)
   {
      a |= 4;
      org.x = 2.0f * center.x - org.x;
      dir.x = -dir.x;
   }
   if (dir.y < 0.0)
   {
      a |= 2;
      org.y = 2.0f * center.y - org.y;
      dir.y = -dir.y;
   }
   
   if (dir.z < 0.0)
   {
      a |= 1;
      org.z = 2.0f * center.z - org.z;
      dir.z = -dir.z;
   }

   miVector t0, t1;
   t0.x = (bottom.x - org.x) / dir.x;
   t0.y = (bottom.y - org.y) / dir.y;
   t0.z = (bottom.z - org.z) / dir.z;
   
   t1.x = (top.x - org.x) / dir.x;
   t1.y = (top.y - org.y) / dir.y;
   t1.z = (top.z - org.z) / dir.z;

#define MAX3(a,b,c) (a > b ? (a > c ? a : c) : (b > c ? b : c) ) 
#define MIN3(a,b,c) (a < b ? (a < c ? a : c) : (b < c ? b : c) )
   
   float tmin=MAX3(t0.x, t0.y, t0.z);
   float tmax=MIN3(t1.x, t1.y, t1.z);
   
   float t = -1.0f;
   if ( state->dist == 0.0 )
      state->dist = std::numeric_limits<double>::max();
   if(tmin < tmax)
   {
      process_subtree(hits, org, state,
		      t0.x, t0.y, t0.z, t1.x, t1.y, t1.z, a );
   }
   if ( hits.empty() ) return false;

   std::sort( hits.begin(), hits.end(), HitListSorter() );

   return true;
}


float
mrOctreeParticlesLeaf::process_subtree( HitList& hits,
					const miVector& unused,
					miState* const state,
					const float tx0, const float ty0,
					const float tz0,
					const float tx1, const float ty1,
					const float tz1, int a ) const
{
   miScalar t = std::numeric_limits< miScalar >::max();

   assert( state->time >= 0 && state->time <= 1.0f );

   // We do all calculations in object space, state is a fake state already
   // set to object space
   miVector dir = state->dir;
   miVector org = state->org;
   
   ParticleIndices::const_iterator i = indices.begin();
   ParticleIndices::const_iterator e = indices.end();
   
   for ( ; i != e; ++i )
   {
      ParticleIndices::value_type idx = *i;
      
      float r2 = r[idx] * r[idx];
      
      // check for ray-sphere intersection
      // Set up quadratic A * t^2 + 2*B*t + c = 0
      //
      // where A = 1 (as ray's dir normalized to 1)
      //       diff = O - C  (ray origin - particle center)
      //       b = DOT( diff, raydir ) 
      //       B = 2 * b
      //       C = diff.lenSq() - r2
      //
      // Solve using quadratic formula:
      //           -B +/- sqrt( B*B - 4 AC )
      //       t = --------------------------
      //                     2A
      //
      //      t = -B +/-  sqrt( B*B - 4C )
      //          ------------------------
      //                    2
      //
      //   and as B = 2 * b, we can just do:
      //      t = -b +/-  sqrt( b*b - c )

      // Calculate particle center (in case of motion blur, we move the
      // particle to the position for this state->time [0,1])
      miVector Pc = p[idx];

      if ( !v.empty() )
      {
	 Pc.x += v[idx].x * state->time;
	 Pc.y += v[idx].y * state->time;
	 Pc.z += v[idx].z * state->time;
      }

      miVector diff = org;
      diff.x -= Pc.x;
      diff.y -= Pc.y;
      diff.z -= Pc.z;

      
      miScalar b = mi_vector_dot( &diff, &dir );

      miScalar lenSq = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
      miScalar C = lenSq - r2;
      
      miScalar discr = b * b - C;
      if ( discr < 0.0f ) continue; // no intersection
      miScalar root = sqrtf(discr);
      miScalar T = -b - root;

      if ( T >= 0.0f )
      {
	 if ( T > state->dist ) continue;
	 HitList::iterator it = hits.begin();
	 HitList::iterator ie = hits.end();
	 bool found = false;
	 for ( ; it != ie; ++it )
	 {
	    if ( (*it)->t == T ) { found = true; break; }
	 }
	 if (found) continue;
	 ParticleIntersection* hit = new ParticleIntersection;
	 hit->type = 0;
	 hit->id   = idx;
	 hit->t    = t = T;
	 hit->r2   = r2;
	 hit->particleCenter = Pc;
	 hits.push_back( hit );
      }

      T = -b + root;
      if ( T >= 0.0f )
      {
	 if ( T > state->dist ) continue;
	 HitList::iterator it = hits.begin();
	 HitList::iterator ie = hits.end();
	 bool found = false;
	 for ( ; it != ie; ++it )
	 {
	    if ( (*it)->t == T ) { found = true; break; }
	 }
	 if (found) continue;
	 ParticleIntersection* hit = new ParticleIntersection;
	 hit->type = 1;
	 hit->id   = idx;
	 hit->t    = t = T;
	 hit->r2   = r2;
	 hit->particleCenter = Pc;
	 hits.push_back( hit );
      }
   }
//    if ( t > state->dist ) return -1.0f;
   return -1;
}


 
float
mrOctreeParticles::process_subtree(HitList& hits,
				   const miVector& org,
				   miState* const state,
				   const float tx0, const float ty0,
				   const float tz0,
				   const float tx1, const float ty1,
				   const float tz1, int a) const
{

#if 0
   // Traverse the octree by brute force (visiting all nodes of octree)
   // REALLY inefficient.  Use this just to verify other octree traversal
   // algorithms are working properly.
   for ( int i = 0; i < 8; ++i )
   {
      if ( child[i] == NULL ) continue;
      
      child[i]->process_subtree(hits, org, state,
				tx0, ty0, tz0, tx1, ty1, tz1, a);
   }
   return 0;

#else
   
    if (tx1 < 0.0 || ty1 < 0.0 || tz1 < 0.0)
       return -1.0f;
    
    miVector tm = { tx1, ty1, tz1 };
    if (finite(tx1))
    {
	tm.x = 0.5f * (tx0 + tx1);
    }
    else if (org.x > center.x)
    {
	tm.x = -tm.x;
    }

    if (finite(ty1))
    {
	tm.y = 0.5f * (ty0 + ty1);
    }
    else if (org.y > center.y)
    {
	tm.y = -tm.y;
    }

    if (finite(tz1))
    {
	tm.z = 0.5f * (tz0 + tz1);
    }
    else if (org.z > center.z)
    {
	tm.z = -tm.z;
    }

   
   float t;
   miVector t0 = { tx0, ty0, tz0 };
   int node = first_node(t0, tm);

   do {
      switch (node)
      {
	 case 0:
	    if(child[a] &&
	       0.0 <= (t=child[a]->process_subtree(hits, org, state,
						   tx0, ty0, tz0,
						   tm.x, tm.y, tm.z, a)))
	       return t;
	    node = next_node(tm.x, 4, tm.y, 2, tm.z, 1);
	    break;
	 case 1:
	    if(child[1^a] &&
	       0.0 <= (t=child[1^a]->process_subtree(hits, org, state,
						     tx0, ty0, tm.z,
						     tm.x, tm.y, tz1, a)))
	       return t;
	    node = next_node(tm.x, 5, tm.y, 3, tz1, 8);
	    break;
	 case 2:
	    if(child[2^a] &&
	       0.0 <= (t=child[2^a]->process_subtree(hits, org, state,
						     tx0, tm.y, tz0,
						     tm.x, ty1, tm.z, a)))
	       return t;
	    node = next_node(tm.x, 6, ty1, 8, tm.z, 3);
	    break;
	 case 3:
	    if(child[3^a] &&
	       0.0 <= (t=child[3^a]->process_subtree(hits, org, state,
						     tx0, tm.y, tm.z,
						     tm.x, ty1, tz1, a)))
	       return t;
	    node = next_node(tm.x, 7, ty1, 8, tz1, 8);
	    break;
	 case 4:
	    if(child[4^a] &&
	       0.0 <= (t=child[4^a]->process_subtree(hits, org, state,
						     tm.x, ty0, tz0,
						     tx1, tm.y, tm.z, a)))
	       return t;
	    node = next_node(tx1, 8, tm.y, 6, tm.z, 5);
	    break;
	 case 5:
	    if(child[5^a] &&
	       0.0 <= (t=child[5^a]->process_subtree(hits, org, state,
						     tm.x, ty0, tm.z,
						     tx1, tm.y, tz1, a)))
	       return t;
	    node = next_node(tx1, 8, tm.y, 7, tz1, 8);
	    break;
	 case 6:
	    if(child[6^a] &&
	       0.0 <= (t=child[6^a]->process_subtree(hits, org, state,
						     tm.x, tm.y, tz0,
						     tx1, ty1, tm.z, a)))
	       return t;
	    node = next_node(tx1, 8, ty1, 8, tm.z, 7);
	    break;
	 case 7:
	    if(child[7^a] &&
	       0.0 <= (t=child[7^a]->process_subtree(hits, org, state,
						     tm.x, tm.y, tm.z,
						     tx1, ty1, tz1, a)))
	       return t;
	    node = 8;
	    break;
      }
   } while(node < 8);

  return -1.0;      
#endif
}



bool mrOctreeParticles::intersect(
				  miState* const hit,
				  ParticleIndices::value_type& hitIdx,
				  miState* const state
				  ) const
{  
   using namespace mr;
   
   miVector dir = state->dir;
   miVector org = state->org;
   

   miVector halfsize = {
   size.x * 0.5f,
   size.y * 0.5f,
   size.z * 0.5f
   };
   
   miVector bottom = { center.x - halfsize.x,
		       center.y - halfsize.y,
		       center.z - halfsize.z
   };
   miVector top    = { center.x + halfsize.x,
		       center.y + halfsize.y,
		       center.z + halfsize.z
   };

   
   // Store bit-mask to flip ray if needed
   int a = 0;
   if (dir.x < 0.0)
   {
      a |= 4;
      org.x = 2.0f * center.x - org.x;
      dir.x = -dir.x;
   }
   if (dir.y < 0.0)
   {
      a |= 2;
      org.y = 2.0f * center.y - org.y;
      dir.y = -dir.y;
   }
   
   if (dir.z < 0.0)
   {
      a |= 1;
      org.z = 2.0f * center.z - org.z;
      dir.z = -dir.z;
   }

   miVector t0, t1;
   t0.x = (bottom.x - org.x) / dir.x;
   t0.y = (bottom.y - org.y) / dir.y;
   t0.z = (bottom.z - org.z) / dir.z;
   
   t1.x = (top.x - org.x) / dir.x;
   t1.y = (top.y - org.y) / dir.y;
   t1.z = (top.z - org.z) / dir.z;

#define MAX3(a,b,c) (a > b ? (a > c ? a : c) : (b > c ? b : c) ) 
#define MIN3(a,b,c) (a < b ? (a < c ? a : c) : (b < c ? b : c) )
   
   float tmin=MAX3(t0.x, t0.y, t0.z);
   float tmax=MIN3(t1.x, t1.y, t1.z);
   
   if ( state->dist == 0.0 )
      state->dist = std::numeric_limits<double>::max();

   float t = -1.0f;
   if(tmin < tmax)
   {
      t = process_subtree(hit, hitIdx, org, state,
			  t0.x, t0.y, t0.z, t1.x, t1.y, t1.z , a );
   }
   if ( t < 0.0f || t > state->dist ) return false;

   hit->normal = hit->normal_geom;
   
   vector dist = hit->point - hit->org;
   hit->dist   = dist.length();

   hit->dot_nd = mi_vector_dot( &hit->normal_geom, &state->dir );
   hit->inv_normal = miFALSE;
   if ( hit->dot_nd > 0 )
   {
      hit->dot_nd = -hit->dot_nd;
      hit->inv_normal = miTRUE;
   }
   return true;
}


float
mrOctreeParticlesLeaf::process_subtree( miState* const hit,
					ParticleIndices::value_type& hitIdx,
					const miVector& unused,
					miState* const state,
					const float tx0, const float ty0,
					const float tz0,
					const float tx1, const float ty1,
					const float tz1, int a ) const
{
   miScalar t = std::numeric_limits< miScalar >::max();

   assert( state->time >= 0 && state->time <= 1.0f );

   // We do all calculations in object space, state is a fake state already
   // set to object space
   miVector dir = state->dir;
   miVector org = state->org;
   
   ParticleIndices::const_iterator i = indices.begin();
   ParticleIndices::const_iterator e = indices.end();
   
   
   miVector particleCenter;
   for ( ; i != e; ++i )
   {
      ParticleIndices::value_type idx = *i;
      
      float r2 = r[idx] * r[idx];
      
      // check for ray-sphere intersection
      // Set up quadratic A * t^2 + 2*B*t + c = 0
      //
      // where A = 1 (as ray's dir normalized to 1)
      //       diff = O - C  (ray origin - particle center)
      //       b = DOT( diff, raydir ) 
      //       B = 2 * b
      //       C = diff.lenSq() - r2
      //
      // Solve using quadratic formula:
      //           -B +/- sqrt( B*B - 4 AC )
      //       t = --------------------------
      //                     2A
      //
      //      t = -B +/-  sqrt( B*B - 4C )
      //          ------------------------
      //                    2
      //
      //   and as B = 2 * b, we can just do:
      //      t = -b +/-  sqrt( b*b - c )

      // Calculate particle center (in case of motion blur, we move the
      // particle to the position for this state->time [0,1])
      miVector Pc = p[idx];
      if ( !v.empty() )
      {
	 Pc.x += v[idx].x * state->time;
	 Pc.y += v[idx].y * state->time;
	 Pc.z += v[idx].z * state->time;
      }

      miVector diff = org;
      diff.x -= Pc.x;
      diff.y -= Pc.y;
      diff.z -= Pc.z;

      
      miScalar b = mi_vector_dot( &diff, &dir );

      miScalar lenSq = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
      miScalar C = lenSq - r2;
      
      miScalar discr = b * b - C;
      if ( discr < 0.0f )  continue; // no intersection
      miScalar root = sqrtf(discr);
      miScalar T = -b - root;

      if ( T >= 0.0f )
      {
	 if ( T >= t ) continue;
	 t = T;
	 hitIdx = idx;
	 particleCenter = Pc;
      }
      else
      {
	 T = -b + root;
	 if ( T >= 0.0f && T < t )
	 {
	    t = T;
	    hitIdx = idx;
	    particleCenter = Pc;
	 }
      }
   }

   if ( t > state->dist ) return -1.0f;

   hit->point.x = org.x + dir.x * t;
   hit->point.y = org.y + dir.y * t;
   hit->point.z = org.z + dir.z * t;

   // Check if we may have intersected a particle in another voxel
   // This can happen with big particles that are only partially in this voxel.
   mrBox< miVector > box; box.center = center; box.size = size;
   if ( !hasPoint( box, hit->point ) ) return -1.0f;
   
   hit->normal_geom.x = hit->point.x - particleCenter.x;
   hit->normal_geom.y = hit->point.y - particleCenter.y;
   hit->normal_geom.z = hit->point.z - particleCenter.z;
   mi_vector_normalize( &hit->normal_geom );
   return t;
}


 
float mrOctreeParticles::process_subtree(miState* const hit,
					 ParticleIndices::value_type& hitIdx,
					 const miVector& org,
					 miState* const state,
					 const float tx0, const float ty0,
					 const float tz0,
					 const float tx1, const float ty1,
					 const float tz1, int a) const
{

#if 0
   // Traverse the octree by brute force (visiting all nodes of octree)
   // REALLY inefficient.  Use this just to verify other octree traversal
   // algorithms are working properly.
   miState hitState = *hit;
   miScalar t = std::numeric_limits< miScalar >::max();
   
   for ( int i = 0; i < 8; ++i )
   {
      if ( child[i] == NULL ) continue;
      
      miScalar r = child[i]->process_subtree(hit, hitIdx, org, state,
					     tx0, ty0, tz0, tx1, ty1, tz1, a);
      if ( r >= 0.0f && r < t )
      {
	 hitState = *hit;
	 t = r;
      }
   }
   *hit = hitState;
   if ( t == std::numeric_limits< miScalar >::max() ) return -1.0f;
   return t;

#else
   
    if (tx1 < 0.0 || ty1 < 0.0 || tz1 < 0.0)
       return -1.0f;
    
    miVector tm = { tx1, ty1, tz1 };
    if (finite(tx1))
    {
	tm.x = 0.5f * (tx0 + tx1);
    }
    else if (org.x > center.x)
    {
	tm.x = -tm.x;
    }

    if (finite(ty1))
    {
	tm.y = 0.5f * (ty0 + ty1);
    }
    else if (org.y > center.y)
    {
	tm.y = -tm.y;
    }

    if (finite(tz1))
    {
	tm.z = 0.5f * (tz0 + tz1);
    }
    else if (org.z > center.z)
    {
	tm.z = -tm.z;
    }

   
   float t;
   miVector t0 = { tx0, ty0, tz0 };
   int node = first_node(t0, tm);

   do {
      switch (node)
      {
	 case 0:
	    if(child[a] &&
	       0.0 <= (t=child[a]->process_subtree(hit, hitIdx, org, state,
						   tx0, ty0, tz0,
						   tm.x, tm.y, tm.z, a)))
	       return t;
	    node = next_node(tm.x, 4, tm.y, 2, tm.z, 1);
	    break;
	 case 1:
	    if(child[1^a] &&
	       0.0 <= (t=child[1^a]->process_subtree(hit, hitIdx, org, state,
						     tx0, ty0, tm.z,
						     tm.x, tm.y, tz1, a)))
	       return t;
	    node = next_node(tm.x, 5, tm.y, 3, tz1, 8);
	    break;
	 case 2:
	    if(child[2^a] &&
	       0.0 <= (t=child[2^a]->process_subtree(hit, hitIdx, org, state,
						     tx0, tm.y, tz0,
						     tm.x, ty1, tm.z, a)))
	       return t;
	    node = next_node(tm.x, 6, ty1, 8, tm.z, 3);
	    break;
	 case 3:
	    if(child[3^a] &&
	       0.0 <= (t=child[3^a]->process_subtree(hit, hitIdx, org, state,
						     tx0, tm.y, tm.z,
						     tm.x, ty1, tz1, a)))
	       return t;
	    node = next_node(tm.x, 7, ty1, 8, tz1, 8); 
	    break;
	 case 4:
	    if(child[4^a] &&
	       0.0 <= (t=child[4^a]->process_subtree(hit, hitIdx, org, state,
						     tm.x, ty0, tz0,
						     tx1, tm.y, tm.z, a)))
	       return t;
	    node = next_node(tx1, 8, tm.y, 6, tm.z, 5);
	    break;
	 case 5:
	    if(child[5^a] &&
	       0.0 <= (t=child[5^a]->process_subtree(hit, hitIdx, org, state,
						     tm.x, ty0, tm.z,
						     tx1, tm.y, tz1, a)))
	       return t;
	    node = next_node(tx1, 8, tm.y, 7, tz1, 8);
	    break;
	 case 6:
	    if(child[6^a] &&
	       0.0 <= (t=child[6^a]->process_subtree(hit, hitIdx, org, state,
						     tm.x, tm.y, tz0,
						     tx1, ty1, tm.z, a)))
	       return t;
	    node = next_node(tx1, 8, ty1, 8, tm.z, 7);
	    break;
	 case 7:
	    if(child[7^a] &&
	       0.0 <= (t=child[7^a]->process_subtree(hit, hitIdx, org, state,
						     tm.x, tm.y, tm.z,
						     tx1, ty1, tz1, a)))
	       return t;
	    node = 8;
	    break;
      }
   } while(node < 8);

  return -1.0;      
#endif
  
}
