/**
 * @file   geoshader.i
 * @author gga
 * @date   Tue Dec 26 05:24:51 2006
 * 
 * @brief  Swig wrapper for geoshader.h
 * 
 * 
 */

#ifdef __cplusplus
extern "C" {
#endif




#define miNAMESIZE	80	
#define miPATHSIZE	1024	





struct miInstance;			
struct miMaterial;			
typedef int (*miInh_func)(void **, void *, int, void *, int,
			struct miInstance *, struct miInstance *, miMatrix,
			struct miMaterial *);


/*
 * Vertices are stored as lists of miGeoIndex values. The first always
 * references the point-in-space vector. The meaning of the others is
 * determined by this structure. Offsets are relative to the first index
 * for this vertex and can never be zero (because index 0 is the point
 * in space). Zero is therefore used to indicate "none" or "missing".
 * no_derivs is implicitly either 0 or 2; no_derivs2 either 0 or 3.
 */

typedef struct miVertex_content {
	miUchar		sizeof_vertex;	
	miUchar		normal_offset;	
	miUchar		derivs_offset;	
	miUchar		derivs2_offset;	
	miUchar		motion_offset;	
	miUchar		no_motions;	
	miUchar		texture_offset;	
	miUchar		no_textures;	
	miUchar		bump_offset;	
	miUchar		no_bumps;	
	miUchar		user_offset;	
	miUchar		no_users;	
} miVertex_content;


/*
 * Hair has both per-hair data and per-vertex data, and needs a different
 * structure than miVertex_content because of this. Since hair uses only two
 * structures per hair-object, we relax the space constraints and use shorts.
 * For all fields, examine the counter to see if there are data present, not
 * the offset field. offsets are in miScalar amounts, ie. typically 4 bytes.
 */

typedef struct miHair_content {
	miUshort  sizeof_vertex;  
	miUshort  normal_offset;
	miUshort  no_normals;     
	miUshort  motion_offset;
	miUshort  no_motions;     
	miUshort  texture_offset;
	miUshort  no_textures;    
	miUshort  radius_offset;
	miUshort  no_radii;       
	miUshort  user_offset;
	miUshort  no_users;       
} miHair_content;


/*
 * Vertices contain references to vectors, which are stored in another
 * list. The list is divided into seven "sections" that permit simple
 * transformation of the box without traversing the vertices: points,
 * normals, surface derivatives, motion, texture, bump basis, user-
 * defined. A section is empty if no vertex needs it. This structure
 * defines the layout of the sections in the vector list.
 * We use miGeoIndex to reference them.
 * (miVector_list is no longer used, the boxes use miGeoVector_list.)
 */

typedef struct miGeoVector_list {
	miGeoIndex	no_vectors;	
	miGeoIndex	no_points;	
	miGeoIndex	no_normals;	
	miGeoIndex	no_derivs;	
	miGeoIndex	no_motions;	
	miGeoIndex	no_textures;	
	miGeoIndex	no_bumps;	
	miGeoIndex	no_users;	
} miGeoVector_list;


void *mi_scene_edit	(const miTag	tag);
void mi_scene_edit_end	(const miTag	tag);


struct miState;
miBoolean mi_geoshader_tesselate
			(struct miState	*state,
			 miTag		*leaves,
			 miTag		root);
miBoolean mi_geoshader_tesselate_end
			(miTag		leaves);




						
/*****************************************************************************
 **				tree nodes				    **
 *****************************************************************************
 *
 * Element type:    miSCENE_INSTANCE
 * Data type:	    miInstance
 * Sizes:	    int param_size
 * Defaults:	    all nulls, two identity matrices, parameter
 *		    size as given by size argument
 */

enum miGenMotionType {
	miGM_INHERIT = 0,
	miGM_TRANSFORM,
	miGM_OFF
};

typedef struct miTransform {
	miTag		function;		
	float		time;			
	miMatrix	global_to_local;	
	miMatrix	local_to_global;	
	miCBoolean	identity;		
	miCBoolean	spare[3];		
	int		id;			
} miTransform;


typedef struct miInstance {
	miTransform	tf;			
	miMatrix	motion_transform;	
	miTag		item;			
	miTag		material;		
	miTag		parent; 		
	miTag		boxes;			
	miTag		next;			
	miTag		prev;			
	int		mtl_array_size; 	
	miTag		light_obj_ring; 	
	miTag		userdata;		
	miUint		label;			
	miTag		geogroup;		
	miCBoolean	off;			
	miUint1 	gen_motion;		
	miUint1 	visible;		
	miUint1 	shadow; 		
	miUint1 	reflection;		
	miUint1 	refraction;		
	miUint1		transparency;		
	miUint1 	caustic;		
	miUint1 	globillum;		
	miUint1		finalgather;		
	char		face;			
	miCBoolean	temp_material;		
	miUint1 	select; 		
	miCBoolean	mtl_override;		
	miUshort	history_size;		
	miTag		approx_list;		
						
	miCBoolean	approx_override;	
	miUint1 	hardware;		
	miUint1 	shadowmap; 		
	char		spare;			
	miTag		param_decl;		
	int		param_size;		
	char		param[8];		
} miInstance;



#define miLEAFHISTORY(i) ((miTag *)(&(i)->param[(i)->param_size +3&~3]))


/*-----------------------------------------------------------------------------
 * Element type:    miSCENE_GROUP
 * Data type:	    miGroup
 * Sizes:	    int max_kids, int max_connections
 * Defaults:	    all nulls, max_* as given by size arguments.
 */

typedef struct miGroup {
	miGeoScalar	merge;		
	miBoolean	placeholder;	
	int		spare0; 	
	miBoolean	merge_group;	
	int		max_kids;	
	int		no_kids;	
	int		max_connections;
	int		no_connections; 
	miTag		userdata;	
	miUint		label;		
	int		spare1[3];	
					
	miTag		kids[2];	
					
	miTag		groupjob;	
	miTag		filename;	
	miScalar	maxdisplace;	
	int		spare2; 	
	miVector	bbox_min;	
	miVector	bbox_max;	
	miVector	bbox_min_m;	
	miVector	bbox_max_m;	
} miGroup;
					
					

#define miMERGE_MIN	0.001		

typedef struct miConnection {
	miTag		instance[2];	
	miGeoIndex	face[2];	
	miGeoIndex	curve[2];	
	miGeoRange	range[2];
} miConnection;
					
					
#define miGROUP_GET_CONNECTIONS(g) \
	((miConnection *)&(g)->kids[(g)->max_kids + 1 & ~1])
					
						


/*****************************************************************************
 **				lights					    **
 *****************************************************************************
 *
 * Element type:    miSCENE_LIGHT
 * Data type:	    miLight
 * Sizes:	    ---
 * Defaults:	    see documentation
 */

						
						

struct miLight_rectangle {
	miVector	edge_u;
	miVector	edge_v;
};

struct miLight_disc {
	miVector	normal;
	miScalar	radius;
};

struct miLight_sphere {
	miScalar	radius;
};

struct miLight_cylinder {
	miVector	axis;
	float		radius;
};

struct miLight_object {
	miTag		object;
};

struct miLight_shmap {
	miTag		camera;		
	miScalar	accuracy;	
	miScalar	filter_u;	
	miScalar	filter_v;	
	miSint2		samples;	
	miUchar		filter;		
	miUchar		type;		
};


typedef struct miLight {
	enum miLight_type	type;		
	enum miLight_area	area;		
	miScalar		exponent;	
	unsigned int		caustic_store_photons;
	unsigned int		global_store_photons; 
	miColor 		energy; 	
	miTag			shader; 	
	miTag			emitter;	
	miVector		origin; 	
	miVector		direction;	
	float			spread; 	
	union {
		struct miLight_rectangle    rectangle;
		struct miLight_disc	    disc;
		struct miLight_sphere	    sphere;
		struct miLight_cylinder     cylinder;
		struct miLight_object	    object;
	}			primitive;	
	short			samples_u;	
	short			samples_v;	
	short			low_samples_u;	
	short			low_samples_v;	
	short			low_level;	
	miUint1			shadowmap_flags;
	miUint1			dirlight_has_org;

	miBoolean		use_shadow_maps;
	miTag			shadowmap_file; 
	int			shadowmap_resolution;  
	float			shadowmap_softness;    
	int			shadowmap_samples;     
	miBoolean		visible;	
	miUint			label;		
	miTag			userdata;	
	unsigned int		caustic_emit_photons; 
	unsigned int		global_emit_photons;  
	miTag			hardware;	
	short			shmap_h_min;	
	short			shmap_h_max;	
	short			shmap_v_min;	
	short			shmap_v_max;	
	float			transparent;	
	miScalar		shadowmap_bias;	
	struct miLight_shmap	shmap;		
	int			spare2[2];	
} miLight;

						
						

/*****************************************************************************
 **				materials				    **
 *****************************************************************************
 *
 * Element type:    miSCENE_MATERIAL
 * Data type:	    miMaterial
 * Sizes:	    ---
 * Defaults:	    all nulls
 */

typedef struct miMaterial {
	miBoolean	opaque; 		
	miTag		shader; 		
	miTag		displace;		
	miTag		shadow; 		
	miTag		volume; 		
	miTag		environment;		
	miTag		contour;		
	miTag		photon; 		
	miTag		photonvol;		
	miTag		lightmap;		
	miTag		hardware;		
} miMaterial;
#define miSCENE_MTL_NSHADERS 8


/*****************************************************************************
 **				functions				    **
 *****************************************************************************
 *
 * Element type:    miSCENE_FUNCTION
 * Data type:	    miFunction
 * Sizes:	    int param_size
 * Defaults:	    all nulls
 *
 * The miDecl_fileparm structure is stored as the parameter structure of
 * miFunctions of type miFUNCTION_OUTFILE (i.e. a file output statement).
 * File outputs, like output shaders, are stored in miFunctions because
 * they are easy to manage and easy to chain and attach to a camera.
 */
						
						

typedef struct {			
	miImg_format	format; 	
	miImg_type	type;		
	miUint		one;		
	miTag		colorprofile;	
	char		filename[4];	
					
					
					
} miDecl_fileparm;
						
						


/*-----------------------------------------------------------------------------
 * Element type:    miSCENE_FUNCTION_DECL
 * Data type:	    miFunction_decl
 * Sizes:	    int decl_size
 * Defaults:	    all nulls, type miFUNCTION_C
 *
 * When adding a new miTYPE_*, it's a very good idea to not use chars and
 * shorts, because we could run into structure padding problems. Keep in
 * mind that pointers may be 64 bits. miTYPE_TEX is a catchall for map values;
 * when the parser finds a map value it doesn't know yet which type of map.
 */

typedef enum {
	miTYPE_BOOLEAN = 0,			
	miTYPE_INTEGER, 			
	miTYPE_SCALAR,
	miTYPE_STRING,
	miTYPE_COLOR,
	miTYPE_VECTOR,
	miTYPE_TRANSFORM,
	miTYPE_SHADER,				
	miTYPE_SCALAR_TEX,			
	miTYPE_COLOR_TEX,
	miTYPE_VECTOR_TEX,
	miTYPE_LIGHT,
	miTYPE_STRUCT,
	miTYPE_ARRAY,
	miTYPE_TEX,
	miTYPE_MATERIAL,			
	miTYPE_GEOMETRY,
	miTYPE_LIGHTPROFILE,			
	miTYPE_DATA,				
	miTYPE_SPECTRUM,			
	miNTYPES
} miParam_type;

						
#define miAPPLY_LENS		(1 << miSHADER_LENS)
#define miAPPLY_MATERIAL	(1 << miSHADER_MATERIAL)
#define miAPPLY_LIGHT		(1 << miSHADER_LIGHT)
#define miAPPLY_SHADOW		(1 << miSHADER_SHADOW)
#define miAPPLY_ENVIRONMENT	(1 << miSHADER_ENVIRONMENT)
#define miAPPLY_VOLUME		(1 << miSHADER_VOLUME)
#define miAPPLY_TEXTURE 	(1 << miSHADER_TEXTURE)
#define miAPPLY_PHOTON		(1 << miSHADER_PHOTON)
#define miAPPLY_GEOMETRY	(1 << miSHADER_GEOMETRY)
#define miAPPLY_DISPLACE	(1 << miSHADER_DISPLACE)
#define miAPPLY_PHOTON_EMITTER	(1 << miSHADER_PHOTON_EMITTER)
#define miAPPLY_OUTPUT		(1 << miSHADER_OUTPUT)
#define miAPPLY_LIGHTMAP	(1 << miSHADER_LIGHTMAP)
#define miAPPLY_PHOTONVOL	(1 << miSHADER_PHOTONVOL)
#define miAPPLY_STATE		(1 << miSHADER_STATE)
#define miAPPLY_CONTOUR		(1 << miSHADER_CONTOUR)
#define miAPPLY_OTHER		(1 << miSHADER_OTHER)

typedef struct miPhen_decl {
	int		n_subtags;		
	miTag		root;			
	miTag		lens;			
	miTag		output; 		
	miTag		volume; 		
	miTag		environment;		
	miTag		geometry;		
	miTag		contour_store;		
	miTag		contour_contrast;	
	int		lens_seqnr;		
	int		output_seqnr;		
	int		volume_seqnr;		
	int		environment_seqnr;	
	
	miCBoolean	scanline;		
	miCBoolean	trace;			
	
	miCBoolean	deriv1; 		
	miCBoolean	deriv2; 		
	miUchar 	mintextures;		
	miUchar 	minbumps;		
	miUchar 	volume_level;		
	miUchar 	parallel;		
	char		shadow; 		
	char		face;			
	char		render_space;		
	miCBoolean	cloned; 		
} miPhen_decl;

typedef struct miFunction_decl {
	miPointer	sparep[2];
	enum miFunction_type type;		
	miParam_type	ret_type;		
	int		declaration_size;	
	int		result_size;		
	int		version;		
	miUint		apply;			
	miPhen_decl	phen;			
	miTag		hardware;		
	int		spare[1];		
	miTag		defaults;		
	char		name[miNAMESIZE];	
	char		declaration[4]; 	
} miFunction_decl;
						
						
#define miDECL_SUBTAG(d,i) ((miTag *)&(d)->declaration\
					[(d)->declaration_size + 3&~3])[i]


/*****************************************************************************
 **				geometry: geoboxes			    **
 *****************************************************************************
 *
 * Element type:    miSCENE_BOX
 * Data type:	    miGeoBox
 * Sizes:	    int nvec, int nvert, int npri, miBox_type type,
 *		    miBoolean moving
 * Defaults:	    all nulls except sizes
 */

enum miBox_type { miBOX_TRIANGLES, miBOX_ALGEBRAICS };
enum miTriangle_projection {
	TRI_XY_P = 0, TRI_YZ_P, TRI_ZX_P,
	TRI_XY_N = 4, TRI_YZ_N, TRI_ZX_N,
	TRI_DEG
};

#define miISOLATED_TRI	0x1		
#define miIS_STRIP	0x2
#define miSTRIP_BEGIN	0x4
#define miSTRIP_RIGHT	0x8

#define miIS_QUAD	0x10

typedef struct miTriangle {
	miUchar 	proj;		
	miUchar 	gap_flags;	
	miUchar 	spare[2];	
	miGeoIndex	a;		
	miGeoIndex	b;		
	miGeoIndex	c;		
	miTag		material;	
	miVector	normal; 	
	miScalar	d;		
} miTriangle;

typedef struct miGeoBox {
	miUint		label;		
	miTag		next_box;	
	enum miBox_type type;		
	miBoolean	spare[3];	
	miBoolean	mtl_is_label;	
	miUint1		spare2[2];	
	miUint1 	select; 	
	miUint1 	sharp;		
	miCBoolean	spare3[4];	
	miTag		userdata;	
	miTag		facedata;	
	miBoolean	hardware;	
	miTag		intersect;	
	miVertex_content vert_info;	
	miGeoVector_list vect_info;	
	miGeoIndex	no_vertices;	
	miGeoIndex	no_primitives;	
	miVector	bbox_min;	
	miVector	bbox_max;	
	miVector	vectors[1];	
} miGeoBox;
					
					

#define miBOX_GET_VERTICES(box) 					\
	((miGeoIndex *)((miVector *)((box) + 1) +			\
		 (box)->vect_info.no_vectors-1))

#define miBOX_GET_PRIMITIVES(box)					\
	(void *)(miBOX_GET_VERTICES(box) +				\
		((box)->no_vertices * (box)->vert_info.sizeof_vertex))

#define miBOX_GET_CONTENTS(box,vertices)		\
	((void *)((miGeoIndex *)((vertices) = miBOX_GET_VERTICES(box)) + \
		((box)->no_vertices * (box)->vert_info.sizeof_vertex)))

#define miBOX_PRIMITIVE_SIZE(box)					\
	((box)->type == miBOX_TRIANGLES ?				\
		sizeof(miTriangle) : sizeof(miAlgeb_pri))




/*****************************************************************************
 **                             light spectra                               **
 *****************************************************************************
 * Element type:   miSCENE_SPECTRUM
 * Data type:      miSpectrum_data
 * Defaults:       31 equidistant coeffs between 400 and 700 nm, 
 *		   all coeffs set to 0, repres = miSPECTRUM_GRID 
 *		   tristimulus color 0
 */

typedef enum miSpectrum_representation {
	miSPECTRUM_SAMPLES	= 1,	
	miSPECTRUM_LINE		= 2,	
	miSPECTRUM_FOURIER	= 3,	

	miSPECTRUM_TYPE		= 0x000000ff, 
	miSPECTRUM_FORCE	= 0x00000100, 
	miSPECTRUM_READ		= 0x00000200, 
	miSPECTRUM_GRID		= 0x00000400, 
	miSPECTRUM_FLAGS	= 0x0000ff00,   
	miSPECTRUM_MAGIC	= 0x5eaf0000, 
	miSPECTRUM_MAGIC_MASK   = 0x7fff0000  
} miSpectrum_representation;

typedef struct miSpectrum_data {
	miSpectrum_representation	representation; 
	int				n_coeff;
	int				n_comp;
	miScalar			coeff[1];
} miSpectrum_data;


/*****************************************************************************
 **				color profiles				    **
 *****************************************************************************
 *
 * Element type:    miSCENE_COLORPROFILE
 * Data type:	    miColor_profile
 * DefaultsL:	    all nulls
 */


#define miCPROF_DEFAULT		0x00
#define miCPROF_RGB		0x01	
#define miCPROF_NTSC		0x02		
#define miCPROF_HDTV		0x03	
#define miCPROF_SHARP		0x04	
#define miCPROF_CIEXYZ		0x05	
#define miCPROF_CIEXYY		0x06	
#define miCPROF_CIELUV		0x07	
#define miCPROF_CIELAB		0x08	
#define miCPROF_SPECTRUM	0x09	
#define miCPROF_CUSTOM		0x0a	
#define miCPROF_LINERGB		0x0b	
#define miCPROF_BOXRGB		0x0c	
#define miCPROF_SIMPLECMY	0x0d	
#define miCPROF_SIMPLECMYK	0x0e	
#define miCPROF_SPACEMASK	0x3f
#define miCPROF_WHITEPOINT	0x40	
#define miCPROF_CID_NOT_ENOUGH	0x80	
					
					
					
typedef struct miColor_profile {
	miUint1		space;		
	miCBoolean	white_adapt;	
	miUint2		flags;	
	miTag		cprof_tag;	
	miTag		filename;	
	miScalar	gamma;		
	miScalar	gamma_offset;	
	miScalar	gamma_scale;     
	miScalar	gamma_slope;	
	miColor		white;		
	miScalar	trafo[9];		
	miScalar	inv_trafo[9];	
	miUint		spare[3];
} miColor_profile;


/*****************************************************************************
 **				light profiles				    **
 *****************************************************************************
 *
 * Element type:    miSCENE_LIGHTPROFILE
 * Data type:	    miLight_profile
 * DefaultsL:	    all nulls
 */

typedef enum miLight_profile_basis {
	miLP_BASIS_NONE    = 0,
	miLP_BASIS_HERMITE = 1
} miLight_profile_basis;


/*-----------------------------------------------------------------------------
 * Element type:    miSCENE_LIGHTPROFILE
 * Data type:	    miLight_profile
 * DefaultsL:	    all nulls
 */

typedef enum miLight_profile_std {
	miLP_STD_NONE	   = 0,
	miLP_STD_IES	   = 1,
	miLP_STD_EULUMDAT  = 2
} miLight_profile_std;

typedef struct miLight_profile {
	miTag		profile;	/* in placeholder, points to
					 * real profile job, 0 else */
	miLight_profile_std   format;	
	miLight_profile_basis base;	
	int		quality;	
	miTag		filename;	
	int		n_vert_angles;	
	int		n_horz_angles;	
	int		flags;		
	miTag		std_data;	
	miScalar	val_max;	
	miScalar	phi0;		
	miScalar	theta0; 	
	miScalar	dphi;		
	miScalar	dtheta; 	
	miScalar	table[1];
} miLight_profile;



/*-----------------------------------------------------------------------------
 * Element type:    miSCENE_LIGHTPROFILE_STD
 * Data type:	    miIes_light or miEulumdat_light
 * Defaults:	    all nulls (invalid)
 */

/*
 * Version of Ies light profile format
 */

typedef enum miIes_version {
	miIESNA_LM_63_1986 = 0,
	miIESNA_LM_63_1991,
	miIESNA_LM_63_1995	
} miIes_version;


/*
 * Enumeration of IES standard labels
 */

typedef enum miIes_label {
	
	miIES_LABEL_TEST = 0,	
	miIES_LABEL_MANUFAC,	
	miIES_LABEL_LUMCAT,	
	miIES_LABEL_LUMINAIRE,	
	miIES_LABEL_LAMPCAT,	
	miIES_LABEL_LAMP,	

	
	miIES_LABEL_DATE,	
	miIES_LABEL_NEARFIELD,	
	miIES_LABEL_BALLAST,	
	miIES_LABEL_BALLASTCAT, 
	miIES_LABEL_MAINTCAT,	
	miIES_LABEL_FLUSHAREA,	
	miIES_LABEL_COLORCONSTANT,	
	miIES_LABEL_OTHER,	
	miIES_LABEL_SEARCH,	

	
	miIES_LABEL_FIELD_NUMBER	= miIES_LABEL_SEARCH,
	miIES_LABEL_FIELD_DB_NUMBER	= 32	
} miIes_label;

/*
 * Supported photometric types
 */
typedef enum miIes_photometric_type {
	miIES_PHOTOMETRIC_TYPE_C = 1,
	miIES_PHOTOMETRIC_TYPE_B = 2,
	miIES_PHOTOMETRIC_TYPE_A = 3
} miIes_photometric_type;


/*
 * Unit types
 */
typedef enum miIes_unit_type {
	miIES_UNIT_TYPE_FEET	= 1,
	miIES_UNIT_TYPE_METER	= 2
} miIes_unit_type;

/*
 * Tilt types
 */
typedef enum miIes_tilt_type {
	miIES_LAMP_SYMM 	= 0,	
	miIES_LAMP_VERT 	= 1,
	miIES_LAMP_HORIZ	= 2,
	miIES_LAMP_TILT 	= 3
} miIes_tilt_type;

/*
 * Tilt structure
 */

typedef struct miIes_tilt {
	miIes_tilt_type type;		
	miUint		number; 	
	struct {
		miScalar angle; 	
		miScalar factor;		
	} pairs[1];
} miIes_tilt;




/*
 * Original light profile information as provided in IES file.
 */
typedef struct miIes_light {
	miLight_profile_std std;	
	miIes_version	version;	
	miTag		filename;	
	miTag		labels[miIES_LABEL_FIELD_DB_NUMBER];
					
	miTag		tilt_filename;	
	miUint		n_lamps;	
	miScalar	lamp_lumen;	
	miScalar	multiplier;
	miUint		n_vert_angles;
	miUint		n_horz_angles;
	miIes_photometric_type	photometric_type;	
	miIes_unit_type unit_type;	
	miScalar	sizes[3];	
	miScalar	ballast_factor;
	miScalar	ballast_lamp;	
	miScalar	spare;		
	miScalar	input_watt;
	miUint		mr_spare[16];	
	miIes_tilt	tilt;		
} miIes_light;



/*
 * Photometric types
 */

typedef enum miEulumdat_type {
	miEULUMDAT_POINT_VERTAXISSYMM	= 1,
	miEULUMDAT_LINEAR		= 2,
	miEULUMDAT_POINT		= 3
} miEulumdat_type;


/*
 * Symmetry types
 */

typedef enum miEulumdat_symm {
	miEULUMDAT_SYMM_NONE		= 0,
	miEULUMDAT_SYMM_VERTAXIS	= 1,
	miEULUMDAT_SYMM_0_180		= 2,
	miEULUMDAT_SYMM_90_270		= 3,
	miEULUMDAT_SYMM_QUAD		= 4
} miEulumdat_symm;


/*
 * Lamp set
 */

typedef struct miEulumdat_lampset {
	miUint		n_lamps;
	miTag		lamps_type;
	miScalar	lamps_flux;		
	miTag		temperature;
	miTag		rendering_index;
	miScalar	watt;			
	miUint		spare[4];		
} miEulumdat_lampset;



/*
 * Original light profile information as provided in EULUMDAT file. Note
 * that some data fields are not parsed and passed as miSCENE_STRING tags.
 */

typedef struct miEulumdat_light {
	miLight_profile_std	std;	
	miTag		filename;
	miTag		id;		
	miEulumdat_type type;
	miEulumdat_symm symm;
	miUint		n_horz_angles;
	miScalar	horz_step;	
	miUint		n_vert_angles;
	miScalar	vert_step;	
	miTag		report_number;
	miTag		luminaire;	
	miTag		luminaire_number;
	miTag		filename_orig;
	miTag		date;
	miScalar	luminaire_sizes[3];
	miScalar	lum_area_sizes[6]; /* length/diam,width,
					    * height0,90,180,270 */
	miScalar	dff;	
	miScalar	lorl;	
	miScalar	factor; 
	miScalar	tilt;
	miScalar	direct_ratios[10];

	miUint		mr_spare[16];	

	miUint			n_lampsets;
	miEulumdat_lampset	lampsets[1];
} miEulumdat_light;




/*****************************************************************************
 **				geometry: line boxes			    **
 *****************************************************************************
 *
 * Element type:    miSCENE_LINEBOX
 * Data type:	    miLinebox
 * Sizes:	    int nvec, int npnt, int nlin
 * Defaults:	    all nulls except sizes
 */

typedef struct miLinebox_info {
	miGeoIndex	no_vectors;	
	miGeoIndex	vector_offset;	
	miGeoIndex	no_stripes;	
	miGeoIndex	stripe_offset;	
	miGeoIndex	no_u_lines;	
	miGeoIndex	u_line_offset;	
	miGeoIndex	no_v_lines;	
	miGeoIndex	v_line_offset;	
	miGeoIndex	no_trims;	
	miGeoIndex	trim_offset;	
	miGeoIndex	no_specials;	
	miGeoIndex	special_offset; 
} miLinebox_info;

/*:: Lineboxes do not contain anything else than ints.
     Swapping of lineboxes relies on this fact. ::*/

typedef struct miLinebox {
	miUint		label;		
	miTag		next_linebox;	
	miTag		userdata;	
	int		spare;		
	miLinebox_info	info;		
	miVector	vectors[1];	
} miLinebox;

typedef struct miLine {
	miGeoIndex	no_stripes;
	miGeoIndex	total_no_points;
	miGeoIndex	line_idx;
} miLine;

typedef struct miLine_stripe {
	miGeoIndex	no_points;
	miGeoIndex	stripe[1];
} miLine_stripe;


/*****************************************************************************
 **				geometry: objects			    **
 *****************************************************************************
 *
 * Element type:    miSCENE_OBJECT
 * Data type:	    miObject
 * Sizes:	    ---
 * Defaults:	    see documentation
 */

enum miObject_type {
	miOBJECT_POLYGONS,
	miOBJECT_FACES,
	miOBJECT_BOXES,
	miOBJECT_SPACECURVES,
	miOBJECT_SUBDIVSURFS,
	miOBJECT_ALGEBRAICS,			
	miOBJECT_PLACEHOLDER,			
	miOBJECT_HAIR
};

typedef struct miPolygon_list {
	miGeoIndex	no_polygons;
	miGeoIndex	no_indices;
	miGeoIndex	no_vertices;
	miGeoVector_list vect_info;		
	miVertex_content vert_info;		
	miTag		polygons;		
	miTag		indices;		
	miTag		vertices;		
	miTag		vectors;		
	miApprox	approx; 		
} miPolygon_list;

typedef struct miFace_list {
	miGeoIndex	no_faces;
	miGeoIndex	no_surfaces;
	miGeoIndex	no_curves;
	miGeoIndex	no_specpnts;
	miGeoIndex	no_surf_scalars;
	miGeoIndex	no_curve_scalars;
	miTag		faces;			
	miTag		surfaces;		
	miTag		curves; 		
	miTag		specpnts;		
	miTag		surf_scalars;		
	miTag		curve_scalars;		
	miTag		basis_list;		
} miFace_list;

typedef struct miSpacecurve_list {
	miGeoIndex	no_spacecurves;
	miGeoIndex	no_curves;
	miGeoIndex	no_specpnts;
	miGeoIndex	no_curve_scalars;
	miTag		spacecurves;		
	miTag		curves; 		
	miTag		specpnts;		
	miTag		curve_scalars;		
	miTag		basis_list;		
	miBoolean	pad;			
} miSpacecurve_list;

typedef struct miSubdivsurf_list {
	miGeoIndex	no_subdivsurfs;
	miTag		subdivsurfs;
} miSubdivsurf_list;

typedef struct miAlgebraic_list {
	miGeoIndex	not_yet_supported;
} miAlgebraic_list;

typedef struct miPlaceholder_list {
	miTag			object; 	
	miTag			filename;	
	enum miObject_type	type;		
} miPlaceholder_list;


typedef struct miHair_list {
	miGeoIndex	no_hairs;		
	miGeoIndex	no_scalars;		
	miHair_content	hair_info;		
	miHair_content	vert_info;		
	miTag		hairs;			
	miTag		scalars;		
	miInteger	approx; 		
	miTag		material;		
	miScalar	radius; 		
	int		degree; 		
	int		space_max_size; 	
	int		space_max_depth;	
} miHair_list;



typedef struct miBox_list {
	miTag		box;			
	miApprox	approx;			
} miBox_list;



typedef struct miObject {
	int			spare_0[1];	
	miUint			volume_id;	
	miTag			finalgather_file; 
	enum miObject_type	type;		
	miUint1			visible;	
	miUint1			shadow; 	
	miUint1			reflection;	
	miUint1			refraction;	
	miUint1			transparency;	
	miUint1 		caustic;	
	miUint1 		globillum;	
	miUint1			finalgather;	
	miUint1 		select; 	
	char			face;		
	miUint1			spare_1[5];	
	miCBoolean		fine;		
	miBoolean		view_dependent; 
	miBoolean		mtl_is_label;	
	miScalar		maxdisplace;	
	miBoolean		hardware;	
	miTag			userdata;	
	miUint			label;		
	miVector		bbox_min;	
	miVector		bbox_max;	
	miVector		bbox_min_m;	
	miVector		bbox_max_m;	
	miTag			functions;	
	unsigned int		n_functions;	
	miCBoolean		mbox_explicit;	
	miSint1 		min_samples;	
	miSint1 		max_samples;	
	miUint1 		shadowmap; 	
	miUchar 		spare_3[4];	
	miScalar		ray_offset;	
	miTag			approx_list;	
	
	union {
	    miPolygon_list	polygon_list;
	    miFace_list 	face_list;
	    miBox_list		box_list;
	    miSpacecurve_list	spacecurve_list;
	    miSubdivsurf_list	subdivsurf_list;
	    miPlaceholder_list	placeholder_list;
	    miHair_list 	hair_list;
	}			geo;		
} miObject;



#define miU		0
#define miV		1
#define miUV_DIM	2
#define miMIN		0
#define miMAX		1
#define miMINMAX_DIM	2


/*****************************************************************************
 **				geometry: object splitting
 *****************************************************************************
 *
 * Element type:    ---
 * Data type:	    miSub_object
 * Sizes:	    int n_sub_objects
 * Defaults:	    all nulls
 */

typedef struct miSub_object {
	miVector	bbox_min;	
	miVector	bbox_max;	
	miVector	bbox_min_m;	
	miVector	bbox_max_m;	
	miTag		box;		
	miUint1		spare1;
	miUint1 	approx_flag;	
	miUint1 	spare2[2];
} miSub_object;



/*-----------------------------------------------------------------------------
 * Element type:    miSCENE_SPLITOBJECT
 * Data type:	    miSplit_object
 * Sizes:	    ---
 * Defaults:	    all nulls
 */

typedef struct miSplit_object {
	miGeoIndex	n_sub_objects;	
	miSub_object	sub_objects[1]; 
} miSplit_object;


/*****************************************************************************
 **				geometry: poly/hair/surf		    **
 *****************************************************************************
 *
 * Element type:    miSCENE_POLYGON
 * Data type:	    miPolygon[]
 * Sizes:	    int no_polys
 * Defaults:	    all nulls
 */

typedef struct miPolygon {
	miGeoIndex	no_loops;	
	miGeoIndex	no_vertices;	
	miGeoIndex	vertex_idx;	
	miTag		material;	
	miCBoolean	convex; 	
	miCBoolean	spare[3];	
} miPolygon;


/*-----------------------------------------------------------------------------
 * Element type:    miSCENE_FACE
 * Data type:	    miFace[]
 * Sizes:	    int no_faces
 * Defaults:	    all nulls except where otherwise noted
 */

typedef struct miFace {
	miApprox	def_trim_approx;  
	miGeoRange	range[miUV_DIM];  
	miGeoIndex	no_curves;	  
	miGeoIndex	no_surfaces;	  
	miGeoIndex	surface_idx;	  
	miGeoIndex	curve_idx;	  
	miTag		material;	  
	miVertex_content gap_vert_info;   
} miFace;


/*-----------------------------------------------------------------------------
 * Element type:    miSCENE_SURFACE
 * Data type:	    miSurface[]
 * Sizes:	    int no_surfaces
 * Defaults:	    all nulls, except where otherwise noted
 */

enum miSurface_type {
	miSURFACE_GEOMETRIC,
	miSURFACE_GEOMOTION,		
	miSURFACE_TEXTURE_2D,		
	miSURFACE_TEXTURE_3D,
	miSURFACE_BUMP, 		
	miSURFACE_TEXBUMP_2D,		
	miSURFACE_TEXBUMP_3D,
	miSURFACE_NTYPES
};
#define miSURFACE_TEXBUMP miSURFACE_TEXBUMP_2D	

typedef struct miSurface {
	miApprox	approx; 		
	miApprox	disp_approx;		
	miGeoIndex	no_parms[miUV_DIM];	
	miGeoIndex	no_ctls;		
	miGeoIndex	no_specpnts;		
	miGeoIndex	scalar_idx;		
	miGeoIndex	specpnt_idx;		
	miGeoIndex	basis_idx[miUV_DIM];	
	enum miSurface_type type;
	miUshort	degree[miUV_DIM];	
	miUshort	ctl_dim;		
	miUshort	spare[3];
} miSurface;

#define miSURFACE_CTLS(S)					\
	((S)->scalar_idx + (S)->no_parms[miU] + (S)->no_parms[miV])


/*-----------------------------------------------------------------------------
 * Element type:    miSCENE_CURVE
 * Data type:	    miCurve
 * Sizes:	    int no_curves
 * Defaults:	    see documentation
 */

enum miCurve_type {
	miCURVE_TRIM,
	miCURVE_HOLE,
	miCURVE_SPECIAL,
	miCURVE_SPACE,
	miCURVE_NTYPES
};

typedef struct miCurve {
	miApprox	approx; 	
	miBoolean	new_loop;	
	miUshort	ctl_dim;	
					
					
	miUshort	degree; 	
	miGeoRange	range;		
	miGeoIndex	no_parms;	
	miGeoIndex	no_ctls;	
	miGeoIndex	no_specpnts;	
	miGeoIndex	scalar_idx;	
	miGeoIndex	specpnt_idx;	
	miGeoIndex	basis_idx;	
	enum miCurve_type type;
	int		pad;		
} miCurve;

#define miCURVE_CTLS(C) 	((C)->scalar_idx + (C)->no_parms)


/*-----------------------------------------------------------------------------
 * Element type:    miSCENE_SPACECURVE
 * Data type:	    miSpacecurve[]
 * Sizes:	    int no_curves
 * Defaults:	    all nulls except where otherwise noted
 */

typedef struct miSpacecurve {
	miApprox	def_approx;	
	miGeoIndex	no_curves;	
	miGeoIndex	curve_idx;	
	miBoolean	pad;		
	miVertex_content gap_vert_info; 
} miSpacecurve;


/*-----------------------------------------------------------------------------
 * Element type:    miSCENE_CURVPNT
 * Data type:	    miCurve_point[]
 * Sizes:	    int no_points
 * Defaults:	    all nulls
 */

typedef struct miCurve_point {
	miGeoVector	v;		
	miGeoVector2d	uv;		
	miGeoScalar	t;		
	miUshort	flags;		
	miUshort	spare_flags;	
	miCBoolean	is_t;		
	miCBoolean	is_uv;		
	miCBoolean	is_v;		
	miCBoolean	spare;		
} miCurve_point;



/*-----------------------------------------------------------------------------
 * Element type:    miSCENE_BASIS_LIST
 * Data type:	    miBasis_list[]
 * Sizes:	    int no_bases, int no_scalars
 * Defaults:	    all nulls
 */

enum miBasis_type {
	miBASIS_NONE = 0,		
	miBASIS_BEZIER,
	miBASIS_BSPLINE,
	miBASIS_CARDINAL,
	miBASIS_MATRIX,
	miBASIS_TAYLOR,
	miNBASISTYPES
};

typedef struct miBasis {
	enum miBasis_type type;
	miGeoIndex	degree; 	
	miGeoIndex	stepsize;	
	miGeoIndex	value_idx;	
} miBasis;

typedef struct miBasis_list {
	miGeoIndex	no_bases;
	miGeoIndex	no_scalars;
	miBasis 	bases[1];
} miBasis_list;



#define miMAX_BASIS_DEGREE 21
#define miBASIS_LIST_SCALARS(bl) ((miGeoScalar *)&(bl)->bases[(bl)->no_bases])


/*****************************************************************************
 **				books					    **
 *****************************************************************************
 *
 * Element type:    miSCENE_BOOK
 * Data type:	    miBook
 * Initialize:	    linesize, page_size, swap
 * Defaults:	    all nulls except fields to be initialized
 */

#define miSCENE_BOOK_MAXSWAP	16

typedef struct miBook {
	miUint		label;		
	miGeoIndex	no_pages;	
	miGeoIndex	first_page_size;
	miGeoIndex	line_size;	
	miGeoIndex	next_free_line; 
	miGeoIndex	last_used_line; 
	char		swap[miSCENE_BOOK_MAXSWAP];
					
	miTag		next_book;	
	miTag		pages[1];	
} miBook;


void	  *mi_scene_book_get_line	(miBook *, miGeoIndex);
void	  *mi_scene_book_allocate_line	(miBook *, miGeoIndex *);
void	   mi_scene_book_release_line	(miBook *, miGeoIndex);
void	   mi_scene_book_enumerate	(miBook *, void (*)(void*,miGeoIndex,
								void*), void*);
miGeoIndex mi_scene_book_free_blk_start (miBook *);
miGeoIndex mi_scene_book_no_used_lines	(miBook *);
miGeoIndex mi_scene_book_max_lines	(miBook *);
miBoolean  mi_scene_book_line_valid	(miBook *, miGeoIndex);
void	   mi_scene_book_sort_freelist	(miBook *);
miTag	   mi_scene_book_attach 	(miTag, miTag, miGeoIndex);
miTag	   mi_scene_book_detach 	(miTag, miGeoIndex);
void	   mi_scene_book_compactify	(miBook *, miGeoIndex **);

typedef struct miPage {
	char		swap[miSCENE_BOOK_MAXSWAP];
					
	miGeoIndex	page_size;	
	miGeoIndex	line_size;	
} miPage;

					
						

/*****************************************************************************
 **				geometry: new boxes			    **
 *****************************************************************************/

/*
 * miVertex_info defines a primitive vertex.
 * The length of a vertex line in number of scalars is given by line_size.
 * The element offsets in number of scalars from the beginning of the line
 * is given by the various offset members.
 *
 * The scalar offsets within a vertex line for all texture and user spaces
 * are stored in the box in an array (dim_offset gives the address of this
 * array), allowing fast access to a certain texture or user data space.
 * The dimension of a certain texture space n can be obtained by subtracting
 * the array entry for space n+1 from the entry for space n.
 * The array first defines offsets for the textures, followed by entries
 * for user data. The macros miBOX_TEX_OFFSET and miBOX_USER_OFFSET are
 * provided for access to the array elements in the box. A certain texture
 * or user data space within a line can be accessed using miVL_TEXTURE_D
 * or miVL_USER_D.
 *
 * The dim_offset array has at least one entry for texture spaces and one
 * for user data spaces, which are identical to no_textures or no_users.
 * If no_textures or no_users > 0, additional entries are present with
 * values >= 3 (defined after position in the vertex line).
 * 
 * no_textures/no_users are the number of texture/user spaces (not the
 * number of scalars). The total number of scalars for all texture spaces
 * can be obtained by dim_offset[n] - dim_offset[0] where n is either
 * no_textures or no_user.
 *
 * The scalar offset for the first user data space begin at entry
 * dim_offset[no_textures+1] (macro miBOX_USER_OFFSET). The array has
 * no_textures+no_users+2 entries.
 *
 * A 3d position is always present (line_size >= 3).
 *
 * Vertex line data is stored in an interleaved array of lines of scalars
 * in the box. There are miBOX_NO_VTXLINES lines in the box.
 * Example:
 * px py pz nx ny nz t1u t1v t2u t2v <-- line 0 vertex scalars
 * px py pz nx ny nz t1u t1v t2u t2v <-- line 1 vertex scalars
 * px py pz nx ny nz t1u t1v t2u t2v
 * 0  1  2  3  4  5  6   7   8   9       scalar offsets within line
 *
 * line_size = 10, no_textures = 2,
 * dim_offset = [6, 8, 10, 0].
 * normal_offset = 3, texture_offset = 6.
 * dimensions for texture spaces are 2, 2.
 * miBOX_TEX_OFFSET(box) points to 6, miBOX_USER_OFFSET(box) to 0.
 * 
 * In case a vertex has different texture/normal/etc vectors at a shared
 * 3d position, multiple lines are generated, i.e. 3d positions and other
 * vectors are duplicated.
 */

typedef struct miVertex_info {
	miUint2		line_size;	
	miUchar		normal_offset;	
	miUchar		derivs_offset;	
	miUchar		derivs2_offset;	
	miUchar		bump_offset;	
	miUchar		no_bumps;	
	miUchar		motion_offset;	
	miUchar		no_motions;	
	miUchar		texture_offset;	
	miUchar		no_textures;	
	miUchar		user_offset;	
	miUchar		no_users;	
	miUchar		spare[3];	
} miVertex_info;


/*
 * prim_offset is an array of integers defining multiple primitive
 * lists. Each primitive list begins with a primitive type combined
 * with a count into a single 32-bit value: type is stored in bits
 * 28-31, count in bits 0-27. The primitive vertex integers follow,
 * referring to the vertex lines.
 * count specifies the number of vertex indices defined by the
 * primitive list.
 * -> Note that a material cannot change within a strip
 *
 * miBox::no_primlists is the total number of primitive lists
 * contained in the box. each primitive list consists of prims.
 * miBox::no_prims is the total number of primitives (triangles,
 * strip faces, quads etc) contained in all primitive lists.
 *
 * The box may contain user data (pdata_offset), swapped as integers,
 * interpretation is up to the application.
 * primdata_size specifies the number of 32-bit integers which are
 * assigned to each primitive from the pdata_offset array. For
 * example primdata_size=1 means that for subsequent primitives one
 * integer from pdata_offset can be used as user data.
 * pdata_size specifies the size of the whole pdata section. there
 * may be extra space after primlist data, for unique needs.
 *
 * mtl_offset is an array of integers defining materials.
 *
 * Current restrictions:
 * - Only two primlists may be defined, one for region triangles,
 *   a second for border triangles (used for split regions).
 * - Only miSCENE_PRIM_TRI primitives are supported.
 * - There must be one material defined for each triangle.
 *
 * Example: content of a primitive list defining a single
 * triangle. BOX_PRIMS will returns a pointer to the code integer.
 * miBox::no_prims=1.
 * 1 0 1 2
 * ^ ^   ^
 * | |   +- third triangle vertex: line 2
 * | +----- first triangle vertex: referring to line 0
 * +------- miSCENE_PRIMLIST_MAKE_CODE(miSCENE_PRIM_TRI, 3)
 *
 * Regions:
 * no_primlists is the number of primitive lists defining the
 * region (without border). no_border_primlists is the number
 * of primitive lists defining the border. Border primitive
 * lists are defined (in memory layout) behind all region
 * primitive lists.
 * no_prims is the total number of primitives in all region
 * primitive lists. no_border_prims is the total number of
 * primitives in all border primitive lists.
 * Border primitives also have corresponding materials.
 *
 */


typedef enum {
	miSCENE_PRIM_TRI,
	miSCENE_PRIM_TRI_STRIP,
	miSCENE_PRIM_TRI_FAN,
	miSCENE_PRIM_QUAD,
	miSCENE_PRIM_QUAD_STRIP,
	miSCENE_PRIM_LINE_STRIP
} miScene_primtype;

#define miSCENE_PRIMLIST_TYPE_MASK	0x0fffffff


#define miSCENE_PRIMLIST_GET_TYPE(code)		\
	(miScene_primtype)((code) >> 28)


#define miSCENE_PRIMLIST_GET_COUNT(code)		\
	((code) & miSCENE_PRIMLIST_TYPE_MASK)

/* construct a primitive list id from primitive type
   and number of indices contained in the list */
#define miSCENE_PRIMLIST_MAKE_CODE(type, count)	\
	((type) << 28 | (count))

typedef struct miBox {
	miCBoolean	mtl_is_label;
	miUchar		sharp;
	miUchar		spare[2];
	miUint		label;		

	miVector	bbox_min;	
	miVector	bbox_max;	

	miUint		no_primlists;	/* number of lists of prims,
					   excluding border primlists */
	miUint		no_prims;	/* total number of primitives
					   contained in all primitive lists,
					   excluding border prims */
	miUint		no_border_primlists; 
	miUint		no_border_prims;
					

	miVertex_info	vertex_info;	
	miUint		primdata_size;	
	miUint		pdata_size;	/* prim data: total size (there could
					   be extra data after primlist data */
	miTag		topology;	/* adjacency information for all
					   primitives (region+border) */

	miUint		prim_offset;	
	miUint		mtl_offset;	
	miUint		dim_offset;	
	miUint		pdata_offset;	
	miScalar	data[1];	
} miBox;






#define miBOX_VERTEX_LINES(box)	\
	(box)->data


#define miBOX_VERTEX_LINE(box, idx)	\
	((box)->data + (idx) * (box)->vertex_info.line_size)


#define miBOX_PRIMS(box)		\
	(miUint*)(((char*)(box))+(box)->prim_offset)


#define miBOX_MATERIALS(box)	\
	(miTag*)(((char*)(box))+(box)->mtl_offset)


#define miBOX_TEX_OFFSET(box)	\
	(miUint*)(((char*)(box))+(box)->dim_offset)


#define miBOX_USER_OFFSET(box)	\
	(miBOX_TEX_OFFSET(box) + ((box)->vertex_info.no_textures+1))


#define miBOX_PRIMDATA(box)	\
	(miUint*)(((char*)(box))+(box)->pdata_offset)




#define miVL_POS(box, line)	\
		(miVector*)(line)


#define miVL_NORMAL(box, line)	\
		(miVector*)((line)+(box)->vertex_info.normal_offset)


#define miVL_DERIVS1(box, line)	\
		(miVector*)((line)+(box)->vertex_info.derivs_offset)


#define miVL_DERIVS2(box, line)	\
		(miVector*)((line)+(box)->vertex_info.derivs2_offset)


#define miVL_BUMP(box, line)	\
	(miVector*)((line)+(box)->vertex_info.bump_offset)


#define miVL_MOTION(box, line)	\
		(miVector*)((line)+(box)->vertex_info.motion_offset)


#define miVL_TEXTURE(box, line)	\
		(miScalar*)((line)+(box)->vertex_info.texture_offset)

/* Return address of a certain texture space. offs is an entry from
   the dim_offset array, i.e a scalar offset within the line */
#define miVL_TEXTURE_D(box, line, offs)	\
		(miScalar*)((line)+offs)


#define miVL_USER(box, line)	\
		(miScalar*)((line)+(box)->vertex_info.user_offset)

/* Return address of a certain user data space. offs is an entry from
   the dim_offset array, i.e a scalar offset within the line */
#define miVL_USER_D(box, line, offs)	\
		(miScalar*)((line)+offs)




#define miBOX_NO_VTXLINES(box)	\
	((box)->vertex_info.line_size == 0 ? 0 :    \
        ((miScalar*)miBOX_PRIMS(box)-(box)->data) / \
	(box)->vertex_info.line_size)


#define miBOX_NO_MATERIALS(box)	\
	(((box)->dim_offset-(box)->mtl_offset)/sizeof(miUint))



/* Return address of primitive data for a certain primitive with
 * global index "idx" */
#define miBOX_PD(box, idx)	\
	miBOX_PRIMDATA(box)+(idx)*(box)->primdata_size




#define miBOX_POS(box, idx)		\
	miVL_POS(box, miBOX_VERTEX_LINE(box, idx))


#define miBOX_MOTION(box, idx, which)	\
	(miVL_MOTION(box, miBOX_VERTEX_LINE(box, idx))+which)

/* Return vertex triple for a certain triangle. Assumes
   that the box defines one sequential list of triangles.
   Skip the primtype/index count entry (+1) */
#define miBOX_TRI(box, idx)		\
	(miBOX_PRIMS(box)+1+(idx)*3)

/* Return material for a certain triangle, same assumptions as
   for miBOX_TRI */
#define miBOX_MATERIAL(box, idx)	\
	(*(miBOX_MATERIALS(box)+(idx)))


#define miBOX_SIZE(box)			\
	(miUint)((box)->pdata_offset+(box)->pdata_size)



/* Return address of first border prim.
   assumes there is only one region primlist */
#define miBOX_BORDER_PRIMS(box)		\
	(miBOX_PRIMS(box) + 1 +		\
	 miSCENE_PRIMLIST_GET_COUNT(*(miBOX_PRIMS(box))))

/* Return address of first border prim material.
   assumes there is a material for every prim */
#define miBOX_BORDER_MATERIALS(box)	\
	(miBOX_MATERIALS(box) + (box)->no_prims)

/* Return material for a border triangle, same assumptions as
   for miBOX_TRI */
#define miBOX_BORDER_MATERIAL(box, idx)		\
	(*(miBOX_BORDER_MATERIALS(box)+(idx)))
						



/*
 * public API interface. Most of these are used by geometry shaders; some
 * are here for completeness only. This is not the geometry shader interface
 * specification; use only those calls documented in the manual.
 */


/*
 * dynamic ordered lists. Lists can be created, deleted, and appended to.
 */

#define miDLIST_SCALAR		1
#define miDLIST_GEOSCALAR	2
#define miDLIST_DOUBLE		3
#define miDLIST_INTEGER		4
#define miDLIST_POINTER		5
#define miDLIST_VREF		6
#define miDLIST_TAG		7

typedef struct miDlist {
	int		type;		
	int		nb;		
	int		nb_allocd;	
	void		*dlist;		
} miDlist;

typedef struct miVref {			
	int		ref;		
	miGeoScalar	w;		
} miVref;


/*
 * vertex flags for subdivision surfaces
 */

typedef enum {
	miAPI_V_SMOOTH=0,
	miAPI_V_CORNER,
	miAPI_V_CONIC,
	miAPI_V_CUSP,
	miAPI_V_DART
} miApi_vertexflags;

/*
 * Performance hints for subdivision surfaces
 */

typedef struct miApi_subdivsurf_options {
	miUint	no_basetris;		
	miUint	no_hiratris;		
	miUint	no_basequads;		
	miUint	no_hiraquads;		
	miUint	no_vertices;		
} miApi_subdivsurf_options;

/*
 * Subdivision surface Texture space options
 */

typedef struct miApi_texspace_options {
	miBoolean face;			
} miApi_texspace_options;


/*
 * API object and instgroup callbacks
 */

typedef miBoolean (*miApi_object_callback)   (miTag, void *);
typedef miBoolean (*miApi_instgroup_callback)(miTag, void *);

typedef miBoolean (*miApi_object_placeholder_callback) 
                  (miTag, miObject *, void *);

					
					

						
miDlist  *mi_api_dlist_create		(int type);
miBoolean mi_api_dlist_add		(miDlist *dlp, void *elem);
miBoolean mi_api_dlist_delete		(miDlist *dlp);

						
miBoolean mi_api_delete			(char *);
miBoolean mi_api_delete_tree		(char *);
					
					
miBoolean mi_api_variable_set		(char *, char *);
const char *mi_api_variable_lookup	(char *);
const char *mi_api_tag_lookup		(miTag);
miTag     mi_api_name_lookup		(char *);
miTag     mi_api_decl_lookup		(char *);
miBoolean mi_api_registry_begin		(char *);
miBoolean mi_api_registry_add		(char *, char *);
miBoolean mi_api_registry_end		(void);
const char *mi_api_registry_eval	(char *);
const char *mi_api_registry_lookup	(char *);
miBoolean mi_api_debug			(char *, char *);
miBoolean mi_api_touch			(char *);

						
miBoolean mi_api_incremental		(miBoolean);
miBoolean mi_api_private		(int);
miBoolean mi_api_version_check		(char *, int);
					
					
miRc_options *mi_api_options_begin	(char *);
miTag     mi_api_options_end		(void);
miCamera *mi_api_camera_begin		(char *);
miTag     mi_api_camera_end		(void);
miBoolean mi_api_render			(char *, char *, char *, char *);
miBoolean mi_api_render_params		(miTag*, miTag*, miTag*, miTag*,
						miInh_func*);
void	     mi_api_render_release	(void);
miBoolean mi_api_taglist_reset		(miTag*, miDlist*);

						
miLight  *mi_api_light_begin		(char *);
miTag     mi_api_light_end		(void);
miTag     mi_api_light_lookup		(char *);
miBoolean mi_api_light_shmap_camera	(char *);

						
miMaterial *mi_api_material_begin	(char *);
miTag     mi_api_material_end		(void);
miTag     mi_api_material_lookup	(char *);
miTag     mi_api_taglist		(miDlist *);

						
typedef miBoolean (*miApi_texture_callback)   
     (miTag, struct miImg_image *, void *);
miBoolean mi_api_texpath_config		(char *, char *, char *);
miTag     mi_api_texture_begin		(char *, int, int);
void	  mi_api_texture_end		(void);
void      mi_api_texture_set_filter	(miScalar);
miBoolean mi_api_texture_array_def_begin(int, int, int);
miBoolean mi_api_texture_byte_copy	(int, miUchar *);
miTag     mi_api_texture_array_def_end	(void);
miBoolean mi_api_texture_file_size	(int, int, int, int);
miTag     mi_api_texture_file_def	(char *);
miTag     mi_api_texture_function_def	(miTag);
miTag 	  mi_api_texture_callback_def(miApi_texture_callback, 
				      miApi_texture_callback, void *);
miTag     mi_api_texture_lookup		(char *, int);
void	  mi_api_texture_set_colorprofile(char *);
int	  mi_api_texture_type_identify(char *);

						
miInstance *mi_api_instance_begin	(char *);
miTag     mi_api_instance_end		(char *, miTag, miTag);
miBoolean mi_api_instance_approx	(miApprox *, miBoolean);
miBoolean mi_api_instance_register	(miTag, char*);

						
miBoolean mi_api_instgroup_file		(char *);
miBoolean mi_api_instgroup_callback	(miApi_instgroup_callback, void *);
miGroup  *mi_api_instgroup_begin	(char *);
miTag     mi_api_instgroup_end		(void);
miBoolean mi_api_instgroup_clear	(void);
miBoolean mi_api_instgroup_additem	(char *);
miBoolean mi_api_instgroup_delitem	(char *);
miBoolean mi_api_instgroup_register	(miTag, char*);

						
miBoolean mi_api_output_file_override	(char *, char *);
miTag     mi_api_output_file_def	(miUint, miUint, char *, char *);
miBoolean mi_api_output_file_parameter	(miTag, miInteger, void *);
miTag     mi_api_output_function_def	(miUint, miUint, miTag);
miBoolean mi_api_output_type_identify	(miUint *, miUint *, char *);
miBoolean mi_api_output_imfdisp_handle	(miTag, miTag);
miBoolean mi_api_output_list		(miTag, miTag);
miBoolean mi_api_output_colorprofile	(char *);
miBoolean mi_api_framebuffer		(miOptions *, int, char *);

						
miBoolean mi_api_basis_list_clear	(void);
miBoolean mi_api_basis_add		(char *, miBoolean, enum miBasis_type,
					 miUshort, miUshort, struct miDlist *);
miGeoIndex mi_api_basis_lookup		(char *, miBoolean *, miUshort *);

						
miObject *mi_api_object_begin		(char *);
miTag     mi_api_object_end		(void);
miBoolean mi_api_object_file		(char *);
miBoolean mi_api_object_placeholder_callback	
                                        (miApi_object_placeholder_callback,
					 miApi_object_placeholder_callback, 
					 void *);
miBoolean mi_api_object_callback	(miApi_object_callback, void *);
miBoolean mi_api_object_matrix		(miMatrix);
miBoolean mi_api_object_group_begin	(double);
miBoolean mi_api_object_group_end	(void);
miBoolean mi_api_object_group_connection
					(char *, char *, miGeoRange *, char *,
					 char *, miGeoRange *);

						
miBoolean mi_api_vector_xyz_add		(miVector *);
miBoolean mi_api_geovector_xyz_add	(miGeoVector *);
miBoolean mi_api_vector_lookup		(miGeoVector *, int);

						
miBoolean mi_api_vertex_add		(int);
miBoolean mi_api_vertex_normal_add	(int);
miBoolean mi_api_vertex_deriv_add	(int, int);
miBoolean mi_api_vertex_deriv2_add	(int, int, int);
miBoolean mi_api_vertex_motion_add	(int);
miBoolean mi_api_vertex_tex_add		(int, int, int);
miBoolean mi_api_vertex_user_add	(int);
miBoolean mi_api_vertex_lookup		(miGeoVector *, int);
miBoolean mi_api_vertex_ref_add		(int, double);
miBoolean mi_api_vertex_flags_add	(miApi_vertexflags, int, miScalar);

						
miBoolean mi_api_poly_index_add		(int);
miBoolean mi_api_poly_begin		(int, char *);
miBoolean mi_api_poly_begin_tag		(int, miTag);
miBoolean mi_api_poly_hole_add		(void);
miBoolean mi_api_poly_approx		(miApprox *);
miBoolean mi_api_poly_end		(void);

						
miBoolean mi_api_subdivsurf_begin	(char *);
miBoolean mi_api_subdivsurf_begin_x	(char *, miApi_subdivsurf_options*);
miBoolean mi_api_subdivsurf_end		(void);
miBoolean mi_api_subdivsurf_baseface	(void);
miBoolean mi_api_subdivsurf_push	(void);
miBoolean mi_api_subdivsurf_pop		(void);
miBoolean mi_api_subdivsurf_subdivide	(int child);
miBoolean mi_api_subdivsurf_mtl		(int child, char *);
miBoolean mi_api_subdivsurf_mtl_tag	(int child, miTag);
miBoolean mi_api_subdivsurf_detail	(int mask);
miBoolean mi_api_subdivsurf_index	(int);
miBoolean mi_api_subdivsurf_crease	(int child, int mask);
miBoolean mi_api_subdivsurf_trim	(int child, int mask);
miBoolean mi_api_subdivsurf_crease_edge	(miScalar value); 
miBoolean mi_api_subdivsurf_approx	(char *, miApprox *);
miBoolean mi_api_subdivsurf_approx_displace(char *, miApprox *);
miBoolean mi_api_subdivsurf_derivative	(int d, int space);
miBoolean mi_api_subdivsurf_texspace	(miApi_texspace_options *,
					 miUint no_spaces);

						
miBoolean mi_api_surface_begin		(char *, char *);
miBoolean mi_api_surface_begin_tag	(char *, miTag);
miBoolean mi_api_surface_params		(int, char *, miGeoScalar, miGeoScalar,
					 miDlist *, miBoolean);
miBoolean mi_api_surface_curveseg	(miBoolean, enum miCurve_type, char *,
					 miGeoRange *);
miBoolean mi_api_surface_specpnt	(int, int);
miBoolean mi_api_surface_texture_begin	(miBoolean, miBoolean, char*,miDlist*,
					 miBoolean, char*, miDlist*,miBoolean);
miBoolean mi_api_surface_derivative	(int);
miBoolean mi_api_surface_end		(void);
miBoolean mi_api_surface_approx		(char *, miApprox *);
miBoolean mi_api_surface_approx_displace(char *, miApprox *);
miBoolean mi_api_surface_approx_trim	(char *, miApprox *);
miBoolean mi_api_surface_lookup		(char *, miTag *, miGeoIndex *);

						
miBoolean mi_api_curve_begin		(char *, char *, miBoolean);
miBoolean mi_api_curve_specpnt		(int, int);
miBoolean mi_api_curve_end		(miDlist *);
void	 *mi_api_curve_lookup		(char *, enum miCurve_type, miBoolean,
					 miGeoRange *, int *);
miBoolean mi_api_curve_approx		(char *, miApprox *);
miBoolean mi_api_curve_approx_store	(miDlist *, miApprox *);

						
miBoolean mi_api_spacecurve_begin	(char *);
miBoolean mi_api_spacecurve_curveseg	(miBoolean, char *, miGeoRange *);
miBoolean mi_api_spacecurve_approx	(char *, miApprox *);
miBoolean mi_api_spacecurve_end		(void);

						
					
					

typedef char *miParameter;
						
miFunction_decl *mi_api_funcdecl_begin	(miParameter *, char *, miParameter *);
miTag     mi_api_funcdecl_end		(void);
miParameter *mi_api_parameter_decl	(miParam_type, char *, int);
miParameter *mi_api_parameter_append	(miParameter *, miParameter *);
miBoolean mi_api_parameter_default	(miParam_type, void *);
miBoolean mi_api_parameter_child	(miParameter *, miParameter *);
miBoolean mi_api_parameter_lookup	(miParam_type *, miBoolean *, int *,
					 miTag, miBoolean, char *);
miBoolean mi_api_parameter_path_lookup	(miTag *, int *, int *, miParam_type *,
					 char *);
miBoolean mi_api_parameter_offset_lookup(miParam_type *, miBoolean *, char
					 **, int *, miTag, miBoolean, int);

					
					
miBoolean mi_api_function_delete	(miTag *);
miTag     mi_api_function_append	(miTag, miTag);
miTag     mi_api_function_assign	(char *);
miBoolean mi_api_function_call		(char *);
miTag     mi_api_function_call_end	(miTag);
miBoolean mi_api_parameter_name		(char *);
miBoolean mi_api_parameter_value	(miParam_type, void *, int *, int *);
miBoolean mi_api_parameter_shader	(char *);
miBoolean mi_api_parameter_interface	(char *);
miBoolean mi_api_parameter_push		(miBoolean);
miBoolean mi_api_parameter_pop		(void);
miBoolean mi_api_new_array_element	(void);
miBoolean mi_api_shader_add		(char *, miTag);
miBoolean mi_api_shader_call		(miTag, char *, char *);
miBoolean mi_api_shader_call_x		(miColor *, miTag, char *, char *,
					 void*);

						
typedef miBoolean (*miApi_data_callback)   (miTag, miUserdata *, void *);

miUserdata *mi_api_data_begin		(char *, int, void *);
miTag     mi_api_data_end		(void);
miTag     mi_api_data_append		(miTag, miTag);
miBoolean mi_api_data_byte_copy		(int, miUchar *);
miTag     mi_api_data_lookup		(char *);
miBoolean mi_api_data_callback		(miApi_data_callback, 
					 miApi_data_callback, void *);

						
miFunction_decl *mi_api_phen_begin	(miParameter *, char *, miParameter *);
miTag     mi_api_phen_end		(void);
					
					
miBoolean mi_api_scope_begin		(char *);
miBoolean mi_api_scope_end		(void);
char	 *mi_api_scope_apply		(char *);
					
					
miHair_list *mi_api_hair_begin		(void);
miBoolean    mi_api_hair_info		(int, char, int);
miScalar    *mi_api_hair_scalars_begin	(int);
miBoolean    mi_api_hair_scalars_end	(int);
miGeoIndex  *mi_api_hair_hairs_begin	(int);
miBoolean    mi_api_hair_hairs_end	(void);
miBoolean    mi_api_hair_hairs_add	(int);
void	     mi_api_hair_end		(void);
					
					
miBoolean    mi_api_lppath_config	(char *, char *, char *);
miLight_profile *mi_api_lightprofile_begin(char *);
miTag        mi_api_lightprofile_end    (void);
miTag        mi_api_lightprofile_lookup (char *);
					
					
miBoolean    mi_api_spectrum_begin	(char *name);
miTag	     mi_api_spectrum_end	(void);
miBoolean    mi_api_spectrum_pair_add	(miScalar, miScalar);
miTag	     mi_api_spectrum_lookup	(char *name);

					
					

struct miColor_profile;
struct miColor_profile *mi_api_colorprofile_begin(char *name);
miTag	     mi_api_colorprofile_end	(void);
miUint1	     mi_api_colorprofile_space	(char *cpname);
void	     mi_api_colorprofile_white	(miColor	*white,
					 int		temperature,
					 miScalar	intensity);
void	     mi_api_colorprofile_gamma  (miScalar	gamma,
					 miScalar	offset,
					 miBoolean	force);
miBoolean    mi_api_colorprofile_custom (miColor_profile *cprof,
					 miMatrix	 m);

					
					
miTag	     mi_api_pass_save_def	(miUint, miUint, int, char *);
miTag	     mi_api_pass_prep_def	(miUint, miUint, int, char *,
					 char *, miTag);
miTag	     mi_api_pass_merge_def	(miUint, miUint, int, miDlist *,
					 char *, miTag);
miTag	     mi_api_pass_delete_def	(char *);
					
					
miGeoBox     *mi_api_trilist_begin	(miVertex_content*,
					 miUint, miUint, miUint);
miBoolean    mi_api_trilist_approx	(miApprox*);
miBoolean    mi_api_trilist_vectors	(miVector*, miUint);
miBoolean    mi_api_trilist_vertices	(miGeoIndex*, miUint);
miBoolean    mi_api_trilist_triangle_tag (miTag, miGeoIndex*);
miBoolean    mi_api_trilist_triangle	(char*, miGeoIndex*);
miBoolean    mi_api_trilist_triangles	(miGeoIndex*, miUint no_triangles);
miBoolean    mi_api_trilist_end		(void);
					
					
miBox	    *mi_api_primlist_begin	(miVertex_info*, miUint, miUint,
					 miUint, miUint, miUint, miUint);
miBoolean    mi_api_primlist_border	(miUint, miUint);
miBoolean    mi_api_primlist_topology	(miUint*);
miBoolean    mi_api_primlist_dimensions (miUint*, miUint*);
miBoolean    mi_api_primlist_approx	(miApprox*);
miBoolean    mi_api_primlist_end	(void);
					





#define miGEO_MAX_CDIM 	4	/* Max. no. of control dimensions =
                 	 	   Max. no. of spatial dimensions + 1 
				   (to account for rational surfaces). */


typedef miGeoScalar miGeoVector_3d[3];





#define array_of(t) struct t ## Ar 					      \
                    {   t 		*ar; 				      \
					   \
                        miGeoIndex 	num; 				      \
						      \
                        miGeoIndex 	size; 				      \
					      \
                        miGeoIndex 	elsize;				      \
					 \
                    }

typedef array_of(miGeoScalar)  		miGeoScalarAr;











typedef struct miGeomoCurve {
      	miGeoScalarAr		ctl_pnts[miGEO_MAX_CDIM];
	miGeoScalarAr		parm; /* parms for Bez, 
					 knots for bsplines */
       	miUshort		degree;
       	miBoolean		rat;
      	miBasis			basis;
      	miGeoScalar		nu; 
} miGeomoCurve;

typedef array_of(miGeomoCurve)  		miGeomoCurveAr;



typedef struct miGeomoSurface {
      	miGeomoCurveAr		v_curves;  /* curves whose param are
      					    * in the v-direction 
      					    */
	miGeoScalarAr		u_parm;    
      	miBasis			u_basis;
      	miBoolean		rat;       
} miGeomoSurface;


#define miGEOMO_MAX_NO_SURFACES	10

/*
 * Prototypes for geomoblend
 */

miBoolean mi_geomo_blend_surfaces
      ( miTag           	*geom_tags,
	miGeoIndex		no_surfaces,
        miGeoIndex		*face_index,
      	miGeoIndex		*trim_index,
        miBoolean   	    	*isoparm,
      	miUshort    	    	*fixed_dir,
      	miGeoScalar 	    	*fixed_parm,
        miGeoScalar 	    	*tension,
	miGeoScalar		*tangent_scale,
        miGeoScalar		tol,
	miGeoIndex		subdiv_depth,
	miGeoIndex		*no_curves,
        miGeomoSurface	    	**surfaces );

/*
 * Prototypes for geomomodel
 */

miBoolean mi_geomo_u_loft
      ( miGeomoCurve            *curves,
        miGeoIndex              no_curves,
        miGeomoSurface          *surface,
        miBoolean               clamped,
        miBoolean               curve_from_surface,
        miGeomoCurve          	*tangents,
        miBoolean		close,
      	miGeoScalar		tolerance,
        miBoolean   	    	autoalign );

void mi_geomo_free_surface
      ( miGeomoSurface            *s );

/*
 * Prototypes for geomospcurve
 */

miBoolean mi_geomo_concatenate_curves
      ( miTag			geom_tag,
        miGeomoCurve            *curve,
        miGeoIndex              scurve_index );

miBoolean mi_geomo_homogenize_curves
      ( miGeomoCurve            *curves,
        miGeoIndex              no_curves );


/*
 * Prototypes for geomoconvert
 */

miBoolean mi_geomo_extract_and_convert_curve
      ( miTag   		geom_tag,
        miGeoIndex		face_index,
      	miGeoIndex		trim_index,
        miGeoScalar		tolerance,
        miGeoScalar		tangent_scale,
      	miGeomoCurve		*c,
      	miGeomoCurve		*tangents );     

void mi_geomo_init_tangent
      ( miGeomoCurve		*tangent );       


/*
 * Prototypes for geomouvloft
 */

miBoolean mi_geomo_uv_loft
      ( miGeomoCurve 		*u_curves,
	miGeoIndex 		no_u_curves,
	miGeomoCurve 		*v_curves,
	miGeoIndex 		no_v_curves,
	miGeomoSurface 		*surface,
	miGeoScalar		tolerance );


/*
 * Prototypes for geomoutils
 */

void mi_geomo_free_curves
      ( miGeomoCurve            *c,
        miGeoIndex              no_curves );






/*
 * Prototypes for object evaluation
 * leaf_inst must be an miInstance instancing the object
 * used for evaluation.
 */

typedef struct miGeoshaderObjectApprox miGeoshaderObjectApprox;

miGeoshaderObjectApprox *mi_geoshader_approximate_object_begin
      ( miState			*state,
	miTag			leaf_inst );



miTag mi_geoshader_approximate_object
      ( miGeoshaderObjectApprox	*gm,
	miGeoIndex		elem_index);

miBoolean mi_geoshader_approximate_object_end(
	miGeoshaderObjectApprox	*gm);

/* 
 * mi_geoshader_eval_surface evaluates a surface at
 * parameter values uv. The 3D position is returned
 * in "pos", the normal vector in "normal" and
 * the derivatives in the dx fields. These values
 * are computed and stored if the pointers in
 * miGeoshaderEvalObj are nonzero.
 * The surface index must be given with "elem_index".
 */

typedef struct {
	miGeoVector	*pos;
	miGeoVector	*normal;
	miGeoVector	*dxdu;
	miGeoVector	*dxdv;
	miGeoVector	*d2xdu2;
	miGeoVector	*d2xdv2;
	miGeoVector	*d2xdudv;
} miGeoshaderEvalObj;

miBoolean mi_geoshader_eval_surface
      ( miGeoshaderEvalObj	*result,
	miGeoshaderObjectApprox	*gm,
	miGeoIndex		elem_index,
	miGeoVector2d		*uv );

/* 
 * mi_geoshader_eval_curve evaluates a curve for
 * parameter value t, returns 3D position in uv
 * if nonzero, the first derivative in dt if
 * nonzero and the second derivatives in dt2
 * if nonzero. The curve index into the list
 * of curves must be given with "curve_index".
 */

typedef struct {
	miGeoVector2d	*uv;
	miGeoVector2d	*dt;
	miGeoVector2d	*dt2;
} miGeoshaderEvalCurve;

miBoolean mi_geoshader_eval_curve
      ( miGeoshaderEvalCurve	*result,
	miGeoshaderObjectApprox *gm,
	miGeoIndex		curve_index,
	miGeoScalar		t );

/*
 * adjacency detection
 *
 * This function searches for neighboring surfaces in the
 * current object using all faces. The adjacency information
 * is returned in "connections", see the scene documentation
 * for details on this data structure.
 * "merge_epsilon" is used when merging trim curves. See
 * raylib documentation for merge group epsilon.
 * The merged curves are returned in "approx_curves". Each
 * face in the current object has a corresponding entry
 * in "approx_curves[face_index]". The number of merged
 * curves is stored in "no_approx_curves[face_index]" where
 * again each face as an integer entry.
 * See the scene documentation for details on "miCurve_point".
 * The merged 3D position is stored in miCurve_point::v, it
 * can be looked up using mi_geoshader_eval_curve and
 * mi_geoshader_eval_surface using miCurve_point::uv.
 * mi_geoshader_release_adjacency should be called to release
 * memory allocated by mi_geoshader_get_adjacency.
 */

typedef struct {
    	enum miCurve_type   type;
    	miBoolean           new_loop;
	miCurve_point	    *curve_points;
	miUint		    no_curve_points;	
} miGeoshaderApproximated;

typedef struct {
	miConnection * connections;
	miUint no_connections;
	miGeoshaderApproximated ** approx_curves;
	miUint * no_approx_curves;
} miGeoshaderObjectAdjacency;

miBoolean mi_geoshader_get_adjacency
      ( miGeoshaderObjectApprox * gm,
        miGeoScalar		merge_epsilon,
	miGeoshaderObjectAdjacency * adj );

void mi_geoshader_release_adjacency
      ( miGeoshaderObjectApprox * gm,
	miGeoshaderObjectAdjacency * adj );





miFunction_ptr mi_link_lookup(
    const char* name);		





#define miEO_INST_GROUP	   0x00000001		
#define miEO_INST_OBJECT   0x00000002		
#define miEO_INST_CAMERA   0x00000004		
#define miEO_INST_LIGHT	   0x00000008		
#define miEO_INST_FUNCTION 0x00000010		
#define miEO_GROUP	   0x00000020		
#define miEO_OBJECT	   0x00000040		
#define miEO_CAMERA	   0x00000080		
#define miEO_LIGHT	   0x00000100		
#define miEO_MATERIAL	   0x00000200		
#define miEO_IMAGE	   0x00000400		
#define miEO_OPTIONS	   0x00000800		
#define miEO_FUNCTION	   0x00001000		
#define miEO_FUNCTION_DECL 0x00002000		
#define miEO_USERDATA	   0x00004000		

typedef struct {
	miBoolean	prefer_approx_polygons;	
	miBoolean	prefer_approx_faces;	
	miBoolean	ascii_output;		
	miBoolean	verbatim_textures;	
	miBoolean	norendercommand;	
	miUint		explode_objects;	
	miTag		leaf_insts;		
	miBoolean	nolinkcommand;		
	miUint		dont_echo;		
	miUint		dont_recurse;		
	miBoolean	incremental;		
} miEchoOptions;


// miBoolean mi_geoshader_echo_tag(
// 	FILE		*fp,			
// 	miTag		tag,			
// 	miEchoOptions	*options);		



#ifdef __cplusplus
}
#endif
