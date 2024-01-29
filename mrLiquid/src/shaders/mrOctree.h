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

#include <cassert>

#include <vector>


#include "mrBoundingBox.h"


#define MR_OCTREE_STATS

struct ParticleIntersection
{
     char     type;  // 0 front, 1 back
     unsigned id;
     miScalar t;
     miVector particleCenter;
     miScalar r2;    // particle radii squared

     bool operator<( const ParticleIntersection& b ) const
     {
	return t < b.t;
     }
};

struct HitList : public std::vector< ParticleIntersection* > 
{
     typedef std::vector< ParticleIntersection* > IntList;

     inline HitList() : std::vector< ParticleIntersection* >() {};

     inline ~HitList() 
     { 
	HitList::iterator i = begin();
	HitList::iterator e = end();
	for ( ; i != e; ++i )
	   delete *i;
     }

};


typedef std::vector< unsigned > ParticleIndices;

class mrOctree
{
   public:
     typedef std::vector< miVector > miVectorList;
     
   public:
     virtual bool isLeaf() { return false; };

     inline void             setSize( const miVector& w )  { size = w; };
     inline miVector         getSize()  const              { return size; };
     
     inline void             setCenter( const miVector& c ) { center = c; };
     inline const miVector&  getCenter() const              { return center; };

     static void setMaxElements( const unsigned d )   { maxElements = d; };
     static void setMaxDepth( const unsigned d )      { maxDepth = d; };


   protected:


     mrOctree()
     {
	center.x = center.y = center.z =
	size.x = size.y = size.z = 0;
#ifdef MR_OCTREE_STATS
      numLevels = 0;
#endif
	
	for (int i = 0; i < 8; ++i )
	   child[i] = NULL;
     }
     mrOctree(
	      const miVector& ctr,
	      const miVector& sz
	      ) :
     center( ctr ),
     size( sz )
     {
	for (int i = 0; i < 8; ++i )
	   child[i] = NULL;
     }
     
     ~mrOctree()
     {
	for (int i = 0; i < 8; ++i )
	   delete child[i];
     }

   public:
     virtual 
     float process_subtree( HitList& hits,
			    const miVector& org,
			    miState* const state,
			    const float tx0, const float ty0,
			    const float tz0,
			    const float tx1, const float ty1,
			    const float tz1, int a ) const = 0;

     virtual float process_subtree( miState* const hit,
				    ParticleIndices::value_type& partIdx, 
				    const miVector& org,
				    miState* const state,
				    const float tx0, const float ty0,
				    const float tz0,
				    const float tx1, const float ty1,
				    const float tz1, int a ) const = 0;

     static bool    hasPoint(
			     const mrBox< miVector >& box1,
			     const miVector& part
			     );
   protected:
     //    Octree node distribution for algorithm:
  
     //           *---*---*
     //  	 / 3 / 7 /|
     //         *---*---*7*   (hidden octant is 1)
     //        / 2 / 6 /|/|
     //       *---*---*6*5*      +Y
     //       | 2 | 6 |/|/       *  * +Z
     //       *---*---*4*        | /
     //       | 0 | 4 |/         |/
     //       *---*---*          *-----* +X
     enum childIds {
     kBOTTOM_LEFT_BACK = 0,
     kBOTTOM_LEFT_FRONT = 1,
     kTOP_LEFT_BACK = 2,
     kTOP_LEFT_FRONT  = 3,
     kBOTTOM_RIGHT_BACK = 4,
     kBOTTOM_RIGHT_FRONT = 5,
     kTOP_RIGHT_BACK = 6,
     kTOP_RIGHT_FRONT  = 7
     };
     

     void getChildCenter( miVector& childCenter, const childIds id ) const;
     
     mrOctree* child[8];

     miVector  center;
     miVector  size;

     static unsigned maxElements;
     static unsigned maxDepth;

#ifdef MR_OCTREE_STATS
     static unsigned numLevels;
#endif
};






class mrOctreeParticles : public mrOctree
{
   public:
     typedef std::vector< float >    RadiiList;
   public:
     mrOctreeParticles( const miVectorList& pts,
			const RadiiList& radius,
			const miVectorList& vel );
     mrOctreeParticles(
		       const unsigned level,
		       const ParticleIndices& idx,
		       const miVectorList& pts,
		       const RadiiList& radius,
		       const miVectorList& vel,
		       const miVector& ctr,
		       const miVector& sz
		       );

     /** 
      * Intersect particles against ray in miState* state.
      * Return all intersections along ray, sorted from closest to
      * farthest away.
      * 
      * @param hit         hit state, containing point of intersect and 
      *                    new normal (returned).
      * @param ids         list of particle indices intersected (returned).
      * @param state       original state containing ray.
      * 
      * @return true if intersection, false if not.
      */
     virtual bool intersect(
			    HitList& hits,
			    miState* const state
			    ) const;

     virtual 
     float process_subtree( HitList& hits,
			    const miVector& org,
			    miState* const state,
			    const float tx0, const float ty0,
			    const float tz0,
			    const float tx1, const float ty1,
			    const float tz1, int a ) const;

     /** 
      * Intersect particles against ray in miState* state.
      * 
      * @param hit         new state, containing point of intersect 
      *                    and new normal (returned).
      * @param partIdx     index of closest particle (returned).
      * @param state       original state containing ray.
      * 
      * @return true if intersection, false if not.
      */
     virtual bool intersect(
			    miState* const hit,
			    ParticleIndices::value_type& partIdx,
			    miState* const state
			    ) const;
     
     virtual float process_subtree( miState* const hit,
				    ParticleIndices::value_type& partIdx,
				    const miVector& org,
				    miState* const state,
				    const float tx0, const float ty0,
				    const float tz0,
				    const float tx1, const float ty1,
				    const float tz1, int a ) const;
     
   protected:
     void getChildBBox(
		       mrBox< miVector >& box,
		       const miVector& w,
		       const int childId
		       ) const;
     
     void createChildren( 
			 const unsigned level,
			 const ParticleIndices particleIndices[8],
			 const bool           hasBig[8],  
			 const miVectorList& pts,
			 const RadiiList& radius,
			 const miVectorList& vel
			 );
     
     static
     bool hasParticle(
		      const mrBox< miVector >& box1,
		      const miVector& part,
		      const float r,
		      const miVector& vel
		      );
     static
     bool hasParticle(
		      const mrBox< miVector >& box1,
		      const miVector& part,
		      const float r
		      );
     
     void subdivide( 
		    ParticleIndices index[8],
		    bool  hasBig[8],
		    const ParticleIndices& idx,
		    const miVectorList& pts,
		    const RadiiList& radius,
		    const miVectorList& vel
		    );
     
     void subdivide( 
		    ParticleIndices index[8],
		    const miVectorList& pts,
		    const RadiiList& radius,
		    const miVectorList& vel
		    );
};


class mrOctreeParticlesLeaf : public mrOctree
{
   public:
     typedef std::vector< float >    RadiiList;
   public:
     mrOctreeParticlesLeaf( const ParticleIndices& idx,
			    const miVectorList& pts,
			    const RadiiList& radius,
			    const miVectorList& vel,
			    const miVector& ctr,
			    const miVector& sz ) :
     mrOctree( ctr, sz ),
     indices( idx ),
     p( pts ),
     r( radius ),
     v( vel )
     {
	assert( p.size() == r.size() );
	assert( v.empty() || p.size() == v.size() );
     };
     
     virtual bool isLeaf() { return true; };
     
     virtual float 
     process_subtree( HitList& hits,
		      const miVector& org,
		      miState* const state,
		      const float tx0, const float ty0,
		      const float tz0,
		      const float tx1, const float ty1,
		      const float tz1, int a ) const;
     
     virtual float process_subtree( miState* const hit,
				    ParticleIndices::value_type& partIdx,
				    const miVector& org,
				    miState* const state,
				    const float tx0, const float ty0,
				    const float tz0,
				    const float tx1, const float ty1,
				    const float tz1, int a ) const;
     
   protected:
     // This holds the particle indices for this leaf
     const ParticleIndices indices;
     const miVectorList& p;
     const RadiiList&    r;
     const miVectorList& v;
};

