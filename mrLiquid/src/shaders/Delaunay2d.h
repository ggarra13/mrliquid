// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WML) source code is supplied under the terms of
// the license agreement http://www.magic-software.com/License/WildMagic.pdf
// and may not be copied or disclosed except in accordance with the terms of
// that agreement.
// -------------------------------------------------------------------------
// This code was slightly modified by Gonzalo Garramuno to use mrClasses
// 

#ifndef WMLDELAUNAY2A_H
#define WMLDELAUNAY2A_H


#include <map>
#include <set>
#include <vector>

#include "mrVector.h"
#include "mrHairInfo.h"


#if defined(WIN32) || defined(WIN64)
#pragma auto_inline( off )
#endif

class Delaunay2d
{
   public:
     // The number of triangles in the Delaunay triangulation is returned in
     // riTQuantity.  The array raiTVertex stores riTQuantity triples of
     // indices into the vertex array akVertex.  The i-th triangle has
     // vertices akVertex[raiTVertex[3*i]], akVertex[raiTVertex[3*i+1]], and
     // akVertex[raiTVertex[3*i+2]].  
     //
     // The caller is responsible for deleting the input and output arrays.

     Delaunay2d(const hairList& akVertex,
		int& riTQuantity, int*& raiTVertex);

     virtual ~Delaunay2d();

   protected:
     // for sorting to remove duplicate input points
     class SortedVertex
     {
	public:
	  SortedVertex() { /**/ }

	  SortedVertex(const mr::vector2d& rkV, int iIndex) :
	  m_kV(rkV), m_iIndex( iIndex )
	  {
	  }

	  bool operator==(const SortedVertex& rkSV) const
	  {
	     return m_kV == rkSV.m_kV;
	  }

	  bool operator!=(const SortedVertex& rkSV) const
	  {
	     return !(m_kV == rkSV.m_kV);
	  }

	  bool operator<(const SortedVertex& rkSV) const
	  {
	     if ( m_kV.u < rkSV.m_kV.u ) return true;
	     if ( m_kV.u > rkSV.m_kV.u ) return false;
	     return m_kV.v < rkSV.m_kV.v;
	  }

	  mr::vector2d m_kV;
	  int m_iIndex;
     };
     typedef std::vector<SortedVertex> SVArray;

     // triangles
     class Triangle
     {
	public:
	  Triangle()
	  {
	     for (int i = 0; i < 3; i++)
	     {
                m_aiV[i] = -1;
                m_apkAdj[i] = NULL;
	     }
	  }

	  Triangle(int iV0, int iV1, int iV2, Triangle* pkA0, Triangle* pkA1,
		   Triangle* pkA2)
	  {
	     m_aiV[0] = iV0;
	     m_aiV[1] = iV1;
	     m_aiV[2] = iV2;
	     m_apkAdj[0] = pkA0;
	     m_apkAdj[1] = pkA1;
	     m_apkAdj[2] = pkA2;
	  }

	  bool PointInCircle(const mr::vector2d& rkP,
			     const SVArray& rkVertex) const
	  {
	     // assert: <V0,V1,V2> is counterclockwise ordered
	     const mr::vector2d& rkV0 = rkVertex[m_aiV[0]].m_kV;
	     const mr::vector2d& rkV1 = rkVertex[m_aiV[1]].m_kV;
	     const mr::vector2d& rkV2 = rkVertex[m_aiV[2]].m_kV;

	     double dV0x = (double) rkV0.u;
	     double dV0y = (double) rkV0.v;
	     double dV1x = (double) rkV1.u;
	     double dV1y = (double) rkV1.v;
	     double dV2x = (double) rkV2.u;
	     double dV2y = (double) rkV2.v;
	     double dV3x = (double) rkP.u;
	     double dV3y = (double) rkP.v;

	     double dR0Sqr = dV0x*dV0x + dV0y*dV0y;
	     double dR1Sqr = dV1x*dV1x + dV1y*dV1y;
	     double dR2Sqr = dV2x*dV2x + dV2y*dV2y;
	     double dR3Sqr = dV3x*dV3x + dV3y*dV3y;

	     double dDiff1x = dV1x - dV0x;
	     double dDiff1y = dV1y - dV0y;
	     double dRDiff1 = dR1Sqr - dR0Sqr;
	     double dDiff2x = dV2x - dV0x;
	     double dDiff2y = dV2y - dV0y;
	     double dRDiff2 = dR2Sqr - dR0Sqr;
	     double dDiff3x = dV3x - dV0x;
	     double dDiff3y = dV3y - dV0y;
	     double dRDiff3 = dR3Sqr - dR0Sqr;

	     double dDet =
	     dDiff1x*(dDiff2y*dRDiff3 - dRDiff2*dDiff3y) -
	     dDiff1y*(dDiff2x*dRDiff3 - dRDiff2*dDiff3x) +
	     dRDiff1*(dDiff2x*dDiff3y - dDiff2y*dDiff3x);

	     return dDet <= 0.0;
	  }

	  bool PointLeftOfEdge(const mr::vector2d& rkP,
			       const SVArray& rkVertex, int i0, int i1) const
	  {
	     const mr::vector2d& rkV0 = rkVertex[m_aiV[i0]].m_kV;
	     const mr::vector2d& rkV1 = rkVertex[m_aiV[i1]].m_kV;

	     double dV0x = (double) rkV0.u;
	     double dV0y = (double) rkV0.v;
	     double dV1x = (double) rkV1.u;
	     double dV1y = (double) rkV1.v;
	     double dV2x = (double) rkP.u;
	     double dV2y = (double) rkP.v;

	     double dEdgex = dV1x - dV0x;
	     double dEdgey = dV1y - dV0y;
	     double dDiffx = dV2x - dV0x;
	     double dDiffy = dV2y - dV0y;

	     double dKross = dEdgex*dDiffy - dEdgey*dDiffx;
	     return dKross >= 0.0;
	  }

	  bool PointInTriangle(const mr::vector2d& rkP,
			       const SVArray& rkVertex) const
	  {
	     // assert: <V0,V1,V2> is counterclockwise ordered
	     const mr::vector2d& rkV0 = rkVertex[m_aiV[0]].m_kV;
	     const mr::vector2d& rkV1 = rkVertex[m_aiV[1]].m_kV;
	     const mr::vector2d& rkV2 = rkVertex[m_aiV[2]].m_kV;

	     double dV0x = (double) rkV0.u;
	     double dV0y = (double) rkV0.v;
	     double dV1x = (double) rkV1.u;
	     double dV1y = (double) rkV1.v;
	     double dV2x = (double) rkV2.u;
	     double dV2y = (double) rkV2.v;
	     double dV3x = (double) rkP.u;
	     double dV3y = (double) rkP.v;

	     double dEdgex = dV1x - dV0x;
	     double dEdgey = dV1y - dV0y;
	     double dDiffx = dV3x - dV0x;
	     double dDiffy = dV3y - dV0y;

	     double dKross = dEdgex*dDiffy - dEdgey*dDiffx;
	     if ( dKross < 0.0 )
	     {
                // P right of edge <V0,V1>, so outside the triangle
                return false;
	     }

	     dEdgex = dV2x - dV1x;
	     dEdgey = dV2y - dV1y;
	     dDiffx = dV3x - dV1x;
	     dDiffy = dV3y - dV1y;
	     dKross = dEdgex*dDiffy - dEdgey*dDiffx;
	     if ( dKross < 0.0 )
	     {
                // P right of edge <V1,V2>, so outside the triangle
                return false;
	     }

	     dEdgex = dV0x - dV2x;
	     dEdgey = dV0y - dV2y;
	     dDiffx = dV3x - dV2x;
	     dDiffy = dV3y - dV2y;
	     dKross = dEdgex*dDiffy - dEdgey*dDiffx;
	     if ( dKross < 0.0 )
	     {
                // P right of edge <V2,V0>, so outside the triangle
                return false;
	     }

	     // P left of all edges, so inside the triangle
	     return true;
	  }


	  // vertices, listed in counterclockwise order
	  int m_aiV[3];

	  // adjacent triangles,
	  //   a[0] points to triangle sharing edge (v[0],v[1])
	  //   a[1] points to triangle sharing edge (v[1],v[2])
	  //   a[2] points to triangle sharing edge (v[2],v[0])
	  Triangle* m_apkAdj[3];
     };

     typedef std::set<Triangle*> TSet;
     typedef TSet::iterator TSetIterator;
     typedef std::vector<Triangle*> TArray;

     // edges (to support constructing the insertion polygon)
     class Edge
     {
	public:
	  Edge(int iV0 = -1, int iV1 = -1, Triangle* pkT = NULL,
	       Triangle* pkA = NULL)
	  {
	     m_iV0 = iV0;
	     m_iV1 = iV1;
	     m_pkT = pkT;
	     m_pkA = pkA;
	  }


	  int m_iV0, m_iV1;  // ordered vertices
	  Triangle* m_pkT;   // insertion polygon triangle
	  Triangle* m_pkA;   // triangle adjacent to insertion polygon
     };

     typedef std::map<int,Edge> EMap;  // <V0,(V0,V1,T,A)>
     typedef std::vector<Edge> EArray;  // (V0,V1,T,A)

     Triangle* GetContaining(const mr::vector2d& rkP) const;
     bool IsInsertionComponent(const mr::vector2d& rkV, Triangle* pkTri) const;
     void GetInsertionPolygon(const mr::vector2d& rkV, TSet& rkPolyTri) const;
     void GetInsertionPolygonEdges(TSet& rkPolyTri, EArray& rkPoly) const;
     void AddTriangles(int iV2, const EArray& rkPoly);
     void RemoveInsertionPolygon(TSet& rkPolyTri);
     void RemoveTriangles();

     // sorted input vertices for processing
     SVArray m_kVertex;

     // indices for the supertriangle vertices
     int m_aiSuperV[3];

     // triangles that contain a supertriangle edge
     Triangle* m_apkSuperT[3];

     // the current triangulation
     TSet m_kTriangle;
};

#if defined(WIN32) || defined(WIN64)
#pragma auto_inline( on )
#endif

#endif
