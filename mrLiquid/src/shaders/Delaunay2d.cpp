// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WML) source code is supplied under the terms of
// the license agreement http://www.magic-software.com/License/WildMagic.pdf
// and may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "Delaunay2d.h"

#include <algorithm>
#include <set>
using namespace std;

#if defined(WIN32) || defined(WIN64)
#pragma auto_inline( off )
#endif

//----------------------------------------------------------------------------

Delaunay2d::Delaunay2d(const hairList& akVertex,
		       int& riTQuantity, int*& raiTVertex) :
m_kVertex(akVertex.size())
{
   //// output values
   riTQuantity  = 0;
   raiTVertex   = NULL;

    // transform the points to [-1,1]^2 for numerical preconditioning
   miScalar fMin = akVertex[0].u, fMax = fMin;
   std::vector< hairInfo >::const_iterator ch = akVertex.begin();
   std::vector< hairInfo >::const_iterator ce = akVertex.end();
   for (; ch != ce; ++ch)
   {
      if ( ch->u < fMin )      fMin = ch->u;
      else if ( ch->u > fMax ) fMax = ch->u;

      if ( ch->v < fMin )      fMin = ch->v;
      else if ( ch->v > fMax ) fMax = ch->v;
   }
   miScalar fHalfRange    = ((miScalar)0.5)*(fMax - fMin);
   miScalar fInvHalfRange = ((miScalar)1.0)/fHalfRange;

   // sort by x-component to prepare to remove duplicate vertices
   int i = 0;
   for (ch = akVertex.begin(); ch != ce; ++ch, ++i)
   {
      m_kVertex[i].m_kV.u = -(miScalar)1.0 + fInvHalfRange*(ch->u - fMin);
      m_kVertex[i].m_kV.v = -(miScalar)1.0 + fInvHalfRange*(ch->v - fMin);
      m_kVertex[i].m_iIndex = i;
   }
   sort(m_kVertex.begin(),m_kVertex.end());

   // remove duplicate points
   SVArray::iterator pkEnd = unique(m_kVertex.begin(),m_kVertex.end());
   m_kVertex.erase(pkEnd,m_kVertex.end());

   // construct supertriangle and add to triangle mesh
   int iVQuantity = (int)m_kVertex.size();
   m_aiSuperV[0] = iVQuantity;
   m_aiSuperV[1] = iVQuantity+1;
   m_aiSuperV[2] = iVQuantity+2;
   m_kVertex.push_back(SortedVertex(mr::vector2d(-2.0f,-2.0f), m_aiSuperV[0]));
   m_kVertex.push_back(SortedVertex(mr::vector2d( 5.0f,-2.0f), m_aiSuperV[1]));
   m_kVertex.push_back(SortedVertex(mr::vector2d(-2.0f, 5.0f), m_aiSuperV[2]));

   Triangle* pkTri = new Triangle(m_aiSuperV[0],m_aiSuperV[1],
				  m_aiSuperV[2],NULL,NULL,NULL);
   m_kTriangle.insert(pkTri);
   m_apkSuperT[0] = pkTri;
   m_apkSuperT[1] = pkTri;
   m_apkSuperT[2] = pkTri;

   // incrementally update the triangulation
   for (i = 0; i < iVQuantity; ++i)
   {
      // construct the insertion polygon
      TSet kPolyTri;
      GetInsertionPolygon(m_kVertex[i].m_kV,kPolyTri);

      EArray kPoly;
      GetInsertionPolygonEdges(kPolyTri,kPoly);

      // add new triangles formed by the point and insertion polygon edges
      AddTriangles(i,kPoly);

      // remove insertion polygon
      RemoveInsertionPolygon(kPolyTri);
   }

   // remove triangles sharing a vertex of the supertriangle
   RemoveTriangles();

   // assign integer values to the triangles for use by the caller
   TSetIterator pkTIter = m_kTriangle.begin();
   for (i = 0; pkTIter != m_kTriangle.end(); ++pkTIter)
   {
      pkTri = *pkTIter;
   }

    // put Delaunay triangles into an array (vertices and adjacency info)
   riTQuantity = (int)m_kTriangle.size();
   if ( riTQuantity > 0 )
   {
      raiTVertex = new int[3*riTQuantity];
      i = 0;
      pkTIter = m_kTriangle.begin();
      for (/**/; pkTIter != m_kTriangle.end(); pkTIter++)
      {
	 pkTri = *pkTIter;
	 raiTVertex[i++] = m_kVertex[pkTri->m_aiV[0]].m_iIndex;
	 raiTVertex[i++] = m_kVertex[pkTri->m_aiV[1]].m_iIndex;
	 raiTVertex[i++] = m_kVertex[pkTri->m_aiV[2]].m_iIndex;
      }
      mrASSERT( i == 3*riTQuantity );
   }
}
//----------------------------------------------------------------------------

Delaunay2d::~Delaunay2d ()
{
   TSetIterator pkTIter    = m_kTriangle.begin();
   TSetIterator pkTIterEnd = m_kTriangle.end();
   for (/**/; pkTIter != pkTIterEnd; ++pkTIter)
      delete *pkTIter;
}
//----------------------------------------------------------------------------

Delaunay2d::Triangle*
Delaunay2d::GetContaining (const mr::vector2d& rkP) const
{
   // start with supertriangle <S0,S1,V>
   Triangle* pkTri = m_apkSuperT[0];
   mrASSERT( pkTri->m_aiV[0] == m_aiSuperV[0] &&
	     pkTri->m_aiV[1] == m_aiSuperV[1] );

   const mr::vector2d& rkS1 = m_kVertex[m_aiSuperV[1]].m_kV;
   mr::vector2d kDiff = rkP - rkS1;
   int iSIndex = 1;
   int iVIndex = 2;  // local index following that of S1

   // The search should never iterate over more than all the triangles.  But
   // just to be safe...
   for (int i = 0; i < (int)m_kTriangle.size(); ++i)
   {
      // test if P is inside the triangle
      mr::vector2d kVmS1 = m_kVertex[pkTri->m_aiV[iVIndex]].m_kV - rkS1;
      miScalar fKross = kVmS1.u*kDiff.v - kVmS1.v*kDiff.u;
      if ( fKross >= (miScalar)0.0 )
	 return pkTri;

      pkTri = pkTri->m_apkAdj[iSIndex];
      mrASSERT( pkTri );
      if ( pkTri->m_aiV[0] == m_aiSuperV[1] )
      {
	 iSIndex = 0;
	 iVIndex = 1;
      }
      else if ( pkTri->m_aiV[1] == m_aiSuperV[1] )
      {
	 iSIndex = 1;
	 iVIndex = 2;
      }
      else if ( pkTri->m_aiV[2] == m_aiSuperV[1] )
      {
	 iSIndex = 2;
	 iVIndex = 0;
      }
      else
      {
	 mrASSERT( false );
      }
   }

   // by construction, point must be in some triangle in the mesh
   mrASSERT( false );
   return pkTri;
}
//----------------------------------------------------------------------------

bool Delaunay2d::IsInsertionComponent (const mr::vector2d& rkV,
						 Triangle* pkTri) const
{
   // determine the number of vertices in common with the supertriangle
   int iCommon = 0, j = -1;
   for (int i = 0; i < 3; ++i)
   {
      int iV = pkTri->m_aiV[i];
      if ( iV == m_aiSuperV[0] )
      {
	 iCommon++;
	 j = i;
      }
      if ( iV == m_aiSuperV[1] )
      {
	 iCommon++;
	 j = i;
      }
      if ( iV == m_aiSuperV[2] )
      {
	 iCommon++;
	 j = i;
      }
   }

   if ( iCommon == 0 )
      return pkTri->PointInCircle(rkV,m_kVertex);

   if ( iCommon == 1 )
      return pkTri->PointLeftOfEdge(rkV,m_kVertex,(j+1)%3,(j+2)%3);

   return pkTri->PointInTriangle(rkV,m_kVertex);
}
//----------------------------------------------------------------------------

void Delaunay2d::GetInsertionPolygon(const mr::vector2d& rkV,
				     TSet& rkPolyTri) const
{
   // locate triangle containing input point, add to insertion polygon
   Triangle* pkTri = GetContaining(rkV);
   //mrASSERT( IsInsertionComponent(rkV,pkTri) );
   rkPolyTri.insert(pkTri);

    // breadth-first search for other triangles in the insertion polygon
   TSet kInterior, kBoundary;
   kInterior.insert(pkTri);
   Triangle* pkAdj;
   int i;
   for (i = 0; i < 3; ++i)
   {
      pkAdj = pkTri->m_apkAdj[i];
      if ( pkAdj )
	 kBoundary.insert(pkAdj);
   }

   while ( kBoundary.size() > 0 )
   {
      TSet kExterior;

      // process boundary triangles
      TSetIterator pkIter = kBoundary.begin();
      TSetIterator pkEnd = kBoundary.end();
      for ( ; pkIter != pkEnd; ++pkIter)
      {
	 // current boundary triangle to process
	 pkTri = *pkIter;
	 if ( IsInsertionComponent(rkV,pkTri) )
	 {
	    // triangle is an insertion component
	    rkPolyTri.insert(pkTri);

	    // locate adjacent, exterior triangles for later processing
	    for (i = 0; i < 3; ++i)
	    {
	       pkAdj = pkTri->m_apkAdj[i];
	       if ( pkAdj
                    &&   kInterior.find(pkAdj) == kInterior.end()
                    &&   kBoundary.find(pkAdj) == pkEnd )
	       {
		  kExterior.insert(pkAdj);
	       }
	    }
	 }
      }

      // boundary triangles processed, move them to interior
      for (pkIter = kBoundary.begin(); pkIter != pkEnd; ++pkIter)
	 kInterior.insert(*pkIter);

      // exterior triangles are next in line to be processed
      kBoundary = kExterior;
   }

   // If the input point is outside the current convex hull, triangles
   // sharing a supertriangle edge have to be added to the insertion polygon
   // if the non-supertriangle vertex is shared by two edges that are visible
   // to the input point.
   for (i = 0; i < 3; ++i)
   {
      pkTri = m_apkSuperT[i];
      TSetIterator pkTIterEnd = rkPolyTri.end();
      if ( rkPolyTri.find(pkTri->m_apkAdj[1]) != pkTIterEnd
	   &&   rkPolyTri.find(pkTri->m_apkAdj[2]) != pkTIterEnd )
      {
	 rkPolyTri.insert(pkTri);
      }
   }
}
//----------------------------------------------------------------------------

void Delaunay2d::GetInsertionPolygonEdges(TSet& rkPolyTri,
					  EArray& rkPoly) const
{
   // locate edges of the insertion polygon
   EMap kIPolygon;
   int iV0, iV1;
   Triangle* pkTri;
   Triangle* pkAdj;
   TSetIterator pkTIter;
   TSetIterator pkTIterEnd = rkPolyTri.end();
   for (pkTIter = rkPolyTri.begin(); pkTIter != pkTIterEnd; ++pkTIter)
   {
      // get an insertion polygon triangle
      pkTri = *pkTIter;

      // If an adjacent triangle is not an insertion polygon triangle, then
      // the shared edge is a boundary edge of the insertion polygon.  Use
      // this edge to create a new Delaunay triangle from the input vertex
      // and the shared edge.
      for (int j = 0; j < 3; ++j)
      {
	 pkAdj = pkTri->m_apkAdj[j];
	 if ( pkAdj )
	 {
	    if ( rkPolyTri.find(pkAdj) == pkTIterEnd )
	    {
	       // adjacent triangle is not part of insertion polygon
	       iV0 = pkTri->m_aiV[j];
	       iV1 = pkTri->m_aiV[(j+1)%3];
	       kIPolygon[iV0] = Edge(iV0,iV1,pkTri,pkAdj);
	    }
	 }
	 else
	 {
	    // no adjacent triangle, edge is part of insertion polygon
	    iV0 = pkTri->m_aiV[j];
	    iV1 = pkTri->m_aiV[(j+1)%3];
	    kIPolygon[iV0] = Edge(iV0,iV1,pkTri,pkAdj);
	 }
      }
   }

   // insertion polygon should be at least a triangle
   int iSize = (int)kIPolygon.size();
   mrASSERT( iSize >= 3 );

    // sort the edges
   EMap::iterator pkE = kIPolygon.begin();
   iV0 = pkE->second.m_iV0;
   for (int i = 0; i < iSize; ++i)
   {
      iV1 = pkE->second.m_iV1;
      rkPoly.push_back(pkE->second);
      pkE = kIPolygon.find(iV1);
      mrASSERT( pkE != kIPolygon.end() );
   }
   mrASSERT( iV1 == iV0 );
}
//----------------------------------------------------------------------------

void Delaunay2d::AddTriangles (int iV2, const EArray& rkPoly)
{
   // create new triangles
   int iSize = (int)rkPoly.size();
   TArray kTriangle(iSize);
   Triangle* pkTri;
   Triangle* pkAdj;
   int i, j;
   for (i = 0; i < iSize; ++i)
   {
      const Edge& rkE = rkPoly[i];

      // construct new triangle
      pkTri = new Triangle(rkE.m_iV0,rkE.m_iV1,iV2,rkE.m_pkA,NULL,NULL);
      kTriangle[i] = pkTri;
      if ( rkE.m_iV0 == m_aiSuperV[0] && rkE.m_iV1 == m_aiSuperV[1] )
	 m_apkSuperT[0] = pkTri;
      else if ( rkE.m_iV0 == m_aiSuperV[1] && rkE.m_iV1 == m_aiSuperV[2] )
	 m_apkSuperT[1] = pkTri;
      else if ( rkE.m_iV0 == m_aiSuperV[2] && rkE.m_iV1 == m_aiSuperV[0] )
	 m_apkSuperT[2] = pkTri;

      // update information of triangle adjacent to insertion polygon
      pkAdj = rkE.m_pkA;
      if ( pkAdj )
      {
	 for (j = 0; j < 3; ++j)
	 {
	    if ( pkAdj->m_apkAdj[j] == rkE.m_pkT )
	    {
	       pkAdj->m_apkAdj[j] = pkTri;
	       break;
	    }
	 }
      }
   }

   // Insertion polygon is a star polygon with center vertex V2.  Finalize
   // the triangles by setting the adjacency information.
   for (i = 0; i < iSize; ++i)
   {
      // current triangle
      pkTri = kTriangle[i];

      // connect to next new triangle
      int iNext = i+1;
      if ( iNext == iSize )
	 iNext = 0;
      pkTri->m_apkAdj[1] = kTriangle[iNext];

      // connect to previous new triangle
      int iPrev = i-1;
      if ( iPrev == -1 )
	 iPrev = iSize-1;
      pkTri->m_apkAdj[2] = kTriangle[iPrev];
   }

   // add the triangles to the mesh
   for (i = 0; i < iSize; ++i)
      m_kTriangle.insert(kTriangle[i]);
}
//----------------------------------------------------------------------------

void Delaunay2d::RemoveInsertionPolygon (TSet& rkPolyTri)
{
   TSetIterator pkTIter    = rkPolyTri.begin();
   TSetIterator pkTIterEnd = rkPolyTri.end();
   for (/**/; pkTIter != pkTIterEnd; ++pkTIter)
   {
      Triangle* pkTri = *pkTIter;
      m_kTriangle.erase(pkTri);
      delete pkTri;
   }
}
//----------------------------------------------------------------------------

void Delaunay2d::RemoveTriangles ()
{
   // identify those triangles sharing a vertex of the supertriangle
   TSet kRemoveTri;
   Triangle* pkTri;
   TSetIterator pkTIter    = m_kTriangle.begin();
   TSetIterator pkTIterEnd = m_kTriangle.end();
   for (/**/; pkTIter != pkTIterEnd; ++pkTIter)
   {
      pkTri = *pkTIter;
      for (int j = 0; j < 3; ++j)
      {
	 int iV = pkTri->m_aiV[j];
	 if ( iV == m_aiSuperV[0] ||
	      iV == m_aiSuperV[1] ||
	      iV == m_aiSuperV[2] )
	 {
	    kRemoveTri.insert(pkTri);
	    break;
	 }
      }
   }

   // remove the triangles from the mesh
   pkTIter    = kRemoveTri.begin();
   pkTIterEnd = kRemoveTri.end();
   for (/**/; pkTIter != pkTIterEnd; ++pkTIter)
   {
      pkTri = *pkTIter;
      for (int j = 0; j < 3; ++j)
      {
	 // break the links with adjacent triangles
	 Triangle* pkAdj = pkTri->m_apkAdj[j];
	 if ( pkAdj )
	 {
	    for (int k = 0; k < 3; ++k)
	    {
	       if ( pkAdj->m_apkAdj[k] == pkTri )
	       {
		  pkAdj->m_apkAdj[k] = NULL;
		  break;
	       }
	    }
	 }
      }
      m_kTriangle.erase(pkTri);
      delete pkTri;
   }
}
//----------------------------------------------------------------------------
