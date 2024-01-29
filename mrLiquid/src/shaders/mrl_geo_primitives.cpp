/****************************************************************************
 * Created:	03.03.04
 * Module:	gg_geo_primitives
 *
 * Exports:
 *      mrl_geo_primitives_init()
 *      mrl_geo_primitives_exit()
 *      mrl_geo_primitives()
 *
 * Requires:
 *      mrClasses, a pdc file (Maya particle format)
 *
 * History:
 *      10.06.06: initial version
 *
 * Description:
 *      Create mray's polygonal cards from a pdc file.
 *
 ***************************************************************************/


#include <vector>
#include <limits>


#include "mrGenerics.h"
using namespace mr;


const int SHADER_VERSION = 1;


//!
//! Geometry Shader parameters
//!
struct mrl_geo_primitives_t
{
     miInteger type;
};




#ifdef _DEBUG
#define MRL_CHECK( x ) \
      do { \
        if ( x != miTRUE ) \
           mi_error( #x " failed at %s, %d", __FILE__, __LINE__); \
       } while (0) 

#else
#define MRL_CHECK( miFnc ) miFnc
#endif


#define MRL_MEM_STRDUP(x) mi_mem_strdup( x )



enum SwatchType
{
kBall,
kCube,
kPlane,
kCylinder,
kCone,
kTorus,
};





static void add_vector(miScalar	a, miScalar b, miScalar c)
{
   miVector v;
   v.x=a; v.y=b; v.z=c;
   mi_api_vector_xyz_add(&v);
}

static void add_triangle(int a, int b, int c)
{
   mi_api_poly_begin_tag(1, 0);
   mi_api_poly_index_add(a);
   mi_api_poly_index_add(b);
   mi_api_poly_index_add(c);
   mi_api_poly_end();
}



miDlist* create_knots( const unsigned numKnots, double* knots )
{
   miDlist* params = mi_api_dlist_create(miDLIST_GEOSCALAR);
   for(unsigned i = 0; i < numKnots; ++i)
      MRL_CHECK(mi_api_dlist_add(params, &knots[i]));
   return params;
}


/*
 * nurbs torus geometry shader
 * creates a torus with inner radius and thickness.
 */


int mrl_geo_torus_version(void) {return(1);}

static miBoolean mrl_geo_torus(miTag*  result,
			miState* state)
{
   return miTRUE;
}



/** 
 * Create a nurbs sphere
 * 
 * @param result 
 * @param state 
 * 
 */
static void mrl_geo_ball( miTag* result, miState* state)
{
   miInteger	i;

   MRL_CHECK(mi_api_basis_add(MRL_MEM_STRDUP( "B1" ),
			      miTRUE, miBASIS_BSPLINE, 1, 0, NULL));
   MRL_CHECK(mi_api_basis_add(MRL_MEM_STRDUP( "B3" ),
			      miTRUE, miBASIS_BSPLINE, 3, 0, NULL));

   mi_api_object_group_begin(0.0);

   miGeoVector	v[] = {
   { 9.59616e-017, -1, -2.53153e-016 },
   { 0.199917, -1, -0.199917 },
   { 0.61643, -0.783612, -0.61643 },
   { 0.867202, 6.53499e-017, -0.867202 },
   { 0.61643, 0.783612, -0.61643 },
   { 0.199917, 1, -0.199917 },
   { 1.73997e-016, 1, -1.68048e-017 },
   { 9.59616e-017, -1, -2.53153e-016 },
   { 0.282725, -1, -6.22066e-017 },
   { 0.871764, -0.783612, 1.05063e-016 },
   { 1.22641, 1.22508e-017, 2.90108e-016 },
   { 0.871764, 0.783612, 3.07369e-016 },
   { 0.282725, 1, 1.95964e-016 },
   { 1.73997e-016, 1, -1.68048e-017 },
   { 9.59616e-017, -1, -2.53153e-016 },
   { 0.199917, -1, 0.199917 },
   { 0.61643, -0.783612, 0.61643 },
   { 0.867202, -4.08482e-017, 0.867202 },
   { 0.61643, 0.783612, 0.61643 },
   { 0.199917, 1, 0.199917 },
   { 1.73997e-016, 1, -1.68048e-017 },
   { 9.59616e-017, -1, -2.53153e-016 },
   { 1.21354e-016, -1, 0.282725 },
   { 2.18134e-016, -0.783612, 0.871764 },
   { 2.3207e-016, -6.28426e-017, 1.22641 },
   { 1.1179e-016, 0.783612, 0.871764 },
   { -1.43558e-017, 1, 0.282725 },
   { 1.73997e-016, 1, -1.68048e-017 },
   { 9.59616e-017, -1, -2.53153e-016 },
   { -0.199917, -1, 0.199917 },
   { -0.61643, -0.783612, 0.61643 },
   { -0.867202, -4.08482e-017, 0.867202 },
   { -0.61643, 0.783612, 0.61643 },
   { -0.199917, 1, 0.199917 },
   { 1.73997e-016, 1, -1.68048e-017 },
   { 9.59616e-017, -1, -2.53153e-016 },
   { -0.282725, -1, 6.72051e-017 },
   { -0.871764, -0.783612, 1.91987e-016 },
   { -1.22641, 1.22508e-017, 2.62787e-016 },
   { -0.871764, 0.783612, 1.81604e-016 },
   { -0.282725, 1, 5.39556e-017 },
   { 1.73997e-016, 1, -1.68048e-017 },
   { 9.59616e-017, -1, -2.53153e-016 },
   { -0.199917, -1, -0.199917 },
   { -0.61643, -0.783612, -0.61643 },
   { -0.867202, 6.53499e-017, -0.867202 },
   { -0.61643, 0.783612, -0.61643 },
   { -0.199917, 1, -0.199917 },
   { 1.73997e-016, 1, -1.68048e-017 },
   { 9.59616e-017, -1, -2.53153e-016 },
   { -1.85084e-016, -1, -0.282725 },
   { -4.14639e-016, -0.783612, -0.871764 },
   { -5.08518e-016, 8.73443e-017, -1.22641 },
   { -3.08295e-016, 0.783612, -0.871764 },
   { -4.93738e-017, 1, -0.282725 },
   { 1.73997e-016, 1, -1.68048e-017 },
   { 9.59616e-017, -1, -2.53153e-016 },
   { 0.199917, -1, -0.199917 },
   { 0.61643, -0.783612, -0.61643 },
   { 0.867202, 6.53499e-017, -0.867202 },
   { 0.61643, 0.783612, -0.61643 },
   { 0.199917, 1, -0.199917 },
   { 1.73997e-016, 1, -1.68048e-017 },
   { 9.59616e-017, -1, -2.53153e-016 },
   { 0.282725, -1, -6.22066e-017 },
   { 0.871764, -0.783612, 1.05063e-016 },
   { 1.22641, 1.22508e-017, 2.90108e-016 },
   { 0.871764, 0.783612, 3.07369e-016 },
   { 0.282725, 1, 1.95964e-016 },
   { 1.73997e-016, 1, -1.68048e-017 },
   { 9.59616e-017, -1, -2.53153e-016 },
   { 0.199917, -1, 0.199917 },
   { 0.61643, -0.783612, 0.61643 },
   { 0.867202, -4.08482e-017, 0.867202 },
   { 0.61643, 0.783612, 0.61643 },
   { 0.199917, 1, 0.199917 },
   { 1.73997e-016, 1, -1.68048e-017 },
   // texture vertices
   { 0, 0, 0 },
   { 1, 0, 0 },
   { 0, 1, 0 },
   { 1, 1, 0 }
   };

   // Add vertices
   for (i=0; i < sizeof(v)/sizeof(miVector); ++i)
      mi_api_geovector_xyz_add(&v[i]);

   for (i=0; i <= 80; ++i)
   {
      mi_api_vertex_add(i);
   }

   double uKnots[] = {
   0., 0., 0., 0., 1., 2., 3., 4., 4., 4., 4.
   };
   int numU = sizeof(uKnots) / sizeof(double);
   double uMin = 0.0;
   double uMax = 4.0;

   double vKnots[] = {
   -2., -2., -1., 0., 1., 2., 3., 4., 5., 6., 7., 8., 9., 10., 10.
   };
   int numV = sizeof(vKnots) / sizeof(double);
   double vMin = 0.0;
   double vMax = 8.0;


   MRL_CHECK( mi_api_surface_begin( MRL_MEM_STRDUP( "S" ), NULL ) );
   miDlist* uparams = create_knots( numU, uKnots );
   MRL_CHECK( mi_api_surface_params(miU,
				    MRL_MEM_STRDUP( "B3" ),
				    uMin,
				    uMax,
				    uparams,
				    miFALSE
				    ) );
   miDlist* vparams = create_knots( numV, vKnots );
   MRL_CHECK( mi_api_surface_params(miV,
				    MRL_MEM_STRDUP( "B3" ),
				    vMin,
				    vMax,
				    vparams,
				    miFALSE
				    ) );
   for ( i = 0; i <= 76; ++i )
      MRL_CHECK(mi_api_vertex_ref_add( i, 1.0 ));

   ////////// Create default texture
   uparams = mi_api_dlist_create( miDLIST_GEOSCALAR );
   MRL_CHECK(mi_api_dlist_add( uparams, &uMin ));
   MRL_CHECK(mi_api_dlist_add( uparams, &uMin ));
   MRL_CHECK(mi_api_dlist_add( uparams, &uMax ));
   MRL_CHECK(mi_api_dlist_add( uparams, &uMax ));
   
   vparams = mi_api_dlist_create( miDLIST_GEOSCALAR );
   MRL_CHECK(mi_api_dlist_add( vparams, &vMin ));
   MRL_CHECK(mi_api_dlist_add( vparams, &vMin ));
   MRL_CHECK(mi_api_dlist_add( vparams, &vMax ));
   MRL_CHECK(mi_api_dlist_add( vparams, &vMax ));
   MRL_CHECK(
	     mi_api_surface_texture_begin(
					  miTRUE,  // volume texture? yes
					  miFALSE, // is bump? no
					  MRL_MEM_STRDUP( "B1" ),
					  uparams,
					  miFALSE, // is u rational?
					  MRL_MEM_STRDUP( "B1" ),
					  vparams,
					  miFALSE  // is v rational?
					  )
	     );

   for ( i = 77; i <= 80; ++i )
      MRL_CHECK(mi_api_vertex_ref_add( i, 1.0 ));

   miApprox approx;
   miAPPROX_DEFAULT( approx );
   approx.method = miAPPROX_PARAMETRIC;
   approx.cnst[miCNST_UPARAM] = 4.5;
   approx.cnst[miCNST_VPARAM] = 4.5;

   MRL_CHECK( mi_api_surface_end() );

   MRL_CHECK(mi_api_surface_approx( MRL_MEM_STRDUP( "S" ),
				    &approx ));
}


/*
 * nurbs cone geometry shader
 */


static void mrl_geo_cone(
		       miTag		*result,
		       miState		*state
		       )
{
}


/*
 * polygonal cylinder geometry shader
 * creates origin centered polygonal cylinder around z-axis,
 * bottom in z=-1, top in z=0, radius=1. u_subdiv subdivisions
 * in the xy plane, v_subdiv in the z axis direction.
 */



static miBoolean mrl_geo_cylinder(
			   miTag		*result,
			   miState		*state
			   )
{
   mi_api_object_group_end();
   return(mi_geoshader_add_result(result, mi_api_object_end()));
}


/*
 * polygonal cube geometry shader
 * creates origin centered unit cube, volume 1
 */

static void mrl_geo_cube(
		  miTag		*result,
		  miState		*state
		  )
{
   int		i, k;
   miVector	v[] = {{-.5,-.5,-.5},{-.5, .5,-.5},
		       { .5, .5,-.5},{ .5,-.5,-.5}};
   int		poly[][4] = {{0,3,7,4},{3,2,6,7},{2,1,5,6},
			     {1,0,4,5},{0,1,2,3},{4,7,6,5}};

   mi_api_object_group_begin(0.0);

   for (i=0; i < 4; i++)
      add_vector(v[i].x, v[i].y, v[i].z);
   for (i=0; i < 4; i++)
      add_vector(v[i].x, v[i].y, -v[i].z);
   for (i=0; i < 8; i++)
      mi_api_vertex_add(i);
   for (i=0; i < 6; i++) {
      mi_api_poly_begin_tag(1, 0);
      for (k=0; k < 4; k++)
	 mi_api_poly_index_add(poly[i][k]);
      mi_api_poly_end();
   }
}


/*
 * polygonal square geometry shader
 * creates origin centered rectangle with area 1
 */

static void mrl_geo_square( miTag*   result, miState* state)
{
   int		i;
   miVector	v[] = {
   // Vertices
   {-.5,-.5, 0},{ .5,-.5, 0},
   { .5, .5, 0},{-.5, .5, 0},
   // Texture Coordinates
   {1.0,  0, 0},{1.0,1.0, 0},
   {  0,1.0, 0},{  0,  0, 0},
   // U & V vectors
   {  0,1.0, 0},{1.0,  0, 0},
   };

   mi_api_object_group_begin(0.0);

   for (i=0; i < 10; ++i)
      mi_api_vector_xyz_add(&v[i]);

   for (i=0; i < 4; i++)
   {
      mi_api_vertex_add(i);
      mi_api_vertex_tex_add(i+4, 8, 9);
   }

   mi_api_poly_begin_tag(1, 0);
   for (i=0; i < 4; i++)
      mi_api_poly_index_add(i);
   mi_api_poly_end();
}

//!
//! MAIN ENTRY FUNCTIONS FOR SHADER.
//! 
EXTERN_C 
DLLEXPORT int mrl_geo_primitives_version(void) {return(SHADER_VERSION);};



EXTERN_C 
DLLEXPORT miBoolean mrl_geo_primitives(
					 miTag* const result,
					 miState* const state,
					 mrl_geo_primitives_t* const p
					 )
{
   SwatchType type = (SwatchType) *mi_eval_integer( &p->type );
   if ( type < kBall || type > kTorus )
   {
      mi_error("mrl_geo_primitives: Unknown primitive type");
      return miFALSE;
   }

   miObject* obj = mi_api_object_begin(NULL);
   obj->visible = miTRUE;
   obj->shadow  = miTRUE;
   obj->reflection = obj->refraction = obj->finalgather = miTRUE;
   obj->mtl_is_label = miTRUE;  // tagged flag

   mi_api_basis_list_clear();

   switch( type )
   {
      case kBall:
	 mrl_geo_ball( result, state ); break;
      case kCube:
	 mrl_geo_cube( result, state ); break;
      case kPlane:
	 mrl_geo_square( result, state ); break;
      case kCylinder:
	 mrl_geo_cylinder( result, state ); break;
      case kCone:
	 mrl_geo_cone( result, state ); break;
      case kTorus:
	 mrl_geo_torus( result, state ); break;
   }

   mi_api_object_group_end();
   return(mi_geoshader_add_result(result, mi_api_object_end()));
}
