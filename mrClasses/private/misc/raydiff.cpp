//
// raydiff.C
//
// $Id: raydiff.cpp,v 1.1.1.1 2006/05/18 05:35:39 Gonzalo Exp $
//

#include <libgeom/raydiff.h>
#include <libgeom/diffgeom.h>
#include <libgeom/packgeom.h>

void
hitRayDifferential(const Ray3d &ray, DiffGeometry *g, float hitt,
     const Point3 &v0, const Point3 &v1, const Point3 &v2,
     const Point2 tex[3], NormalPack *normals, int vi0,
     int vi1, int vi2)
{
    // first compute ray transfer
    float invDdotNg = 1. / dot(ray.d, g->Ng);
    float dtdx = -(dot(ray.dPdu + hitt * ray.dDdu, g->Ng)) * invDdotNg;
    float dtdy = -(dot(ray.dPdv + hitt * ray.dDdv, g->Ng)) * invDdotNg;

    Vector3 dPdx1 = ray.dPdu + (hitt * ray.dDdu) + dtdx * ray.d;
    Vector3 dPdy1 = ray.dPdv + (hitt * ray.dDdv) + dtdy * ray.d;

    dPdx1.x = clamp(dPdx1.x, -1e8, 1e8);
    dPdx1.y = clamp(dPdx1.y, -1e8, 1e8);
    dPdx1.z = clamp(dPdx1.z, -1e8, 1e8);
    dPdy1.x = clamp(dPdy1.x, -1e8, 1e8);
    dPdy1.y = clamp(dPdy1.y, -1e8, 1e8);
    dPdy1.z = clamp(dPdy1.z, -1e8, 1e8);

    Vector3 pac = v0 - v2;
    Vector3 pbc = v1 - v2;
    Normal3 normal = cross(pac, pbc);
    Vector3 plane = normal.hat();
    float invVol = 1. / dot(plane, normal);
     
    Vector3 alpha = invVol * cross(pbc, plane);
    Vector3 beta  = invVol * cross(plane, pac);
     
    // derivatives of barycentrics
    float db0x = dot(alpha, dPdx1);
    float db1x = dot(beta, dPdx1);
    float db2x = -db0x - db1x;

    float db0y = dot(alpha, dPdy1);
    float db1y = dot(beta, dPdy1);
    float db2y = -db0y - db1y;

    // derivative of the normal
    if (normals) {
 Vector3 dndx = db0x * (*normals)[vi0] + 
     db1x * (*normals)[vi1] + db2x * (*normals)[vi2];
 Vector3 dndy = db0y * (*normals)[vi0] + 
     db1y * (*normals)[vi1] +  db2y * (*normals)[vi2];

 float invPowFactor = 1. / pow(g->N.length_squared(), 1.5);

 g->dNdx = (dot(g->N, g->N) * dndx + dot(g->N, dndx) * g->N) *
     invPowFactor;
 g->dNdy = (dot(g->N, g->N) * dndy + dot(g->N, dndy) * g->N) *
     invPowFactor;

 assert(!isnan(g->dNdx.x + g->dNdy.x));
    }
    else
 g->dNdx = g->dNdy = Vector3(0, 0, 0);

    // and derivative of texture coordinates
    g->dudx = db0x * tex[0].u + db1x * tex[1].u + db2x * tex[2].u;
    g->dudy = db0y * tex[0].u + db1y * tex[1].u + db2y * tex[2].u;
    g->dvdx = db0x * tex[0].v + db1x * tex[1].v + db2x * tex[2].v;
    g->dvdy = db0y * tex[0].v + db1y * tex[1].v + db2y * tex[2].v;
}

void
refractedRayDifferential(const Ray3d &ray, const Point3 &Pw, 
    const Vector3 &wi, const Normal3 &Ns,
    const Normal3 &dNdx, const Normal3 &dNdy,
    float eta_i, float eta_o, Ray3d *ret)
{
    ret->o = Pw;
    ret->d = wi;

    // new ray differential...
    ret->dPdu = ray.dPdu;
    ret->dPdv = ray.dPdv;

    float eta = eta_i / eta_o;
    float mu = eta * dot(ray.d, Ns) + dot(wi, Ns);
    float dNewDotN = 1 - eta*eta*(1 - sqr(dot(ray.d, Ns))); // sqr(x) == x*x
    assert(dNewDotN >= 0.);
    dNewDotN = sqrt(dNewDotN);

    float dDdotNdx = dot(ray.dDdu, Ns) + dot(ray.d, dNdx);
    ret->dDdu = eta * ray.dDdu - mu * dNdx - 
 (eta + (eta*eta*dot(ray.d, Ns)/dNewDotN)) * dDdotNdx * Ns;

    float dDdotNdy = dot(ray.dDdv, Ns) + dot(ray.d, dNdy);
    ret->dDdv = eta * ray.dDdv - mu * dNdy - 
 (eta + (eta*eta*dot(ray.d, Ns)/dNewDotN)) * dDdotNdy * Ns;
}

void
reflectedRayDifferential(const Ray3d &ray, const Point3 &Pw, 
    const Vector3 &wi, const Normal3 &Ns,
    const Vector3 &dNdx, const Vector3 &dNdy, Ray3d *ret)
{
    ret->o = Pw;
    ret->d = wi;

    ret->dPdu = ray.dPdu;
    ret->dPdv = ray.dPdv;

    float dDdotNdx = dot(ray.dDdu, Ns) + dot(ray.d, dNdx);
    ret->dDdu = ray.dDdu - 2 * (dot(ray.d, Ns) * dNdx + dDdotNdx * Ns);

    float dDdotNdy = dot(ray.dDdv, Ns) + dot(ray.d, dNdy);
    ret->dDdv = ray.dDdv - 2 * (dot(ray.d, Ns) * dNdy + dDdotNdy * Ns);
}

