



#ifdef __cplusplus
extern "C" {
#endif





typedef enum {miFALSE=0, miTRUE=1}	miBoolean;
typedef unsigned char			miCBoolean;	
typedef int				miInteger;	
typedef signed char			miSint1;
typedef unsigned char			miUint1;
typedef signed short			miSint2;
typedef unsigned short			miUint2;
typedef signed int			miSint4;
typedef unsigned int			miUint4;
typedef unsigned int			miUint;
typedef unsigned char			miUchar;
typedef unsigned short			miUshort;

#if defined(WIN_NT) && defined(BIT64)
typedef unsigned __int64		miUlong;
typedef signed   __int64		miSlong;
#else
typedef unsigned long			miUlong;
typedef signed   long			miSlong;
#endif

typedef signed char			miSchar;
typedef float				miScalar;
typedef double				miGeoScalar;
typedef struct {miScalar u, v;}		miVector2d;
typedef struct {miScalar x, y, z;}	miVector;
typedef struct {miGeoScalar u, v;}	miGeoVector2d;
typedef struct {miGeoScalar x, y, z;}	miGeoVector;
typedef struct {miScalar x, y, z, w;}	miQuaternion;
typedef miScalar			miMatrix[16];
typedef struct {float r, g, b, a;}	miColor;
typedef struct {miScalar min, max;}	miRange;
typedef struct {miGeoScalar min, max;}	miGeoRange;
typedef unsigned short			miIndex;
typedef unsigned int			miGeoIndex;
typedef miBoolean			(*miFunction_ptr)(void);
typedef union {void *p; double d;}	miPointer;	
typedef miUint 				miTag;
typedef miSint4				miThreadID;
typedef miSint4				miHostID;
typedef miSint4				miSessionID;
typedef miSint4				miTransactionID;
typedef miSint4				miWorldID;

// struct mi_lock;
typedef union {
// 	struct	mi_lock *milockptr;
	int	padding[2];
} miLock;


#if defined(WIN_NT) && defined(BIT64)			
typedef unsigned __int64		miIntptr;	
#else							
typedef unsigned long			miIntptr;	
#endif							

#define miSCALAR_EPSILON		(1.0e-5f)
#define miGEO_SCALAR_EPSILON		(1e-9)
#define miHUGE_SCALAR			(1e36f)	
#define miHUGE_INT			(2147483647)
#define miNULL_INDEX			(0xffff) 
#define miMAX_INDEX			(0xfffe) 
#define miNULL_GEOINDEX			(0xffffffff) 
#define miMAX_GEOINDEX			(0xfffffffe) 

typedef enum {
	miM_NULL = 0,		
	miM_MSG,		
	miM_PAR,		
	miM_MEM,		
	miM_DB,			
	miM_PHEN,		
	miM_RCRM,		
	miM_SCENE,		
	miM_RC,			
	miM_GAPMI,		
	miM_IMG,		
	miM_ECHO,		
	miM_MI,			
	miM_RCLM,		
	miM_SBS,		
	miM_RCH,		
	miM_IMATTER,		
	miM_GEOSI,		
	miM_TEST,		
	miM_LINK,		
	miM_P2SD,		
	miM_DISP,		
	miM_LPROF,              
	miM_JOB,		
	miM_MAIN,		
	miM_GEOMI,		
	miM_EXR,		
	miM_TRANS,		
	miM_INVENTOR,		
	miM_LIB,		
	miM_API,		
	miM_SPDL,		
	miM_PHCR,		
	miM_RCB,		
	miM_RCC,		
	miM_RCI,		
	miM_RCGI,		
	miM_RCHW,	        
	miM_SIMP,		
	miM_SURF,		
	miM_CAPI,		
	miM_GAPAD,		
	miM_GAPCU,		
	miM_GAPDI,		
	miM_GAPIS,		
	miM_GAPPO,		
	miM_GAPFA,		
	miM_GAPTE,		
	miM_GEOCO,		
	miM_GEOEV,		
	miM_GEOMO,		
	miM_GEOST,		
	miM_GEOTE,		
	miM_JPG,		
	miM_RCFG,		
	miNMODULES		
} miModule;


#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif
#ifndef M_PI_2
#define M_PI_2		1.57079632679489661923
#endif
#ifndef M_SQRT1_2
#define M_SQRT1_2	0.70710678118654752440
#endif


#define RAY2			
#define RAY3			
#define RAY31                   
#define RAY32                   
#define RAY33                   
#define RAY34                   
#ifdef _WIN32
#define DLLIMPORT __declspec(dllexport)
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLIMPORT
#define DLLEXPORT
#endif





/*
 * vector operations. MI_SQRT works for ANSI C compilers. If it causes trouble,
 * #define MI_SQRT sqrt before including this file. IBM and HP mess up as usual.
 */

#ifndef MI_SQRT
#ifdef IRIX
#define MI_SQRT sqrtf
#else
#define MI_SQRT sqrt
#endif
#endif

#define mi_vector_neg(r)	((r)->x = -(r)->x,\
				 (r)->y = -(r)->y,\
				 (r)->z = -(r)->z)

#define mi_vector_add(r,a,b)	((r)->x = (a)->x + (b)->x,\
				 (r)->y = (a)->y + (b)->y,\
				 (r)->z = (a)->z + (b)->z)

#define mi_vector_sub(r,a,b)	((r)->x = (a)->x - (b)->x,\
				 (r)->y = (a)->y - (b)->y,\
				 (r)->z = (a)->z - (b)->z)

#define mi_vector_mul(r,f)	((r)->x *= (f),\
				 (r)->y *= (f),\
				 (r)->z *= (f))

#define mi_vector_div(r,d)	do {if ((d) != 0.0f)\
					{register miScalar _i=1.0f/(d);\
						 (r)->x *= _i;\
						 (r)->y *= _i;\
						 (r)->z *= _i; }} while (0)

#define mi_vector_div_d(r,d)	do {if ((d) != 0.0)\
					{register double _i = 1.0/(d);\
						 (r)->x *= _i;\
						 (r)->y *= _i;\
						 (r)->z *= _i; }} while (0)

#define mi_vector_prod(r,a,b)	((r)->x = (a)->y * (b)->z - (a)->z * (b)->y,\
				 (r)->y = (a)->z * (b)->x - (a)->x * (b)->z,\
				 (r)->z = (a)->x * (b)->y - (a)->y * (b)->x)

#define mi_vector_dot(a,b)	((a)->x * (b)->x +\
				 (a)->y * (b)->y +\
				 (a)->z * (b)->z)

#define mi_vector_dot_d(a,b)	((double)(a)->x * (double)(b)->x +\
				 (double)(a)->y * (double)(b)->y +\
				 (double)(a)->z * (double)(b)->z)

#define mi_vector_norm(r)	(miScalar)MI_SQRT((r)->x * (r)->x +\
					(r)->y * (r)->y +\
					(r)->z * (r)->z)

#define mi_vector_norm_d(r)	sqrt((double)(r)->x * (double)(r)->x +\
				     (double)(r)->y * (double)(r)->y +\
				     (double)(r)->z * (double)(r)->z)

#define mi_vector_normalize(r)	do { register miScalar _n = mi_vector_norm(r);\
				     mi_vector_div(r, _n); } while (0)

#define mi_vector_normalize_d(r) do { register double _n= mi_vector_norm_d(r);\
				     mi_vector_div_d(r, _n); } while (0)

#define mi_vector_min(r,a,b)	((a)->x < (b)->x ? ((r)->x = (a)->x) \
						 : ((r)->x = (b)->x),\
				 (a)->y < (b)->y ? ((r)->y = (a)->y) \
						 : ((r)->y = (b)->y),\
				 (a)->z < (b)->z ? ((r)->z = (a)->z) \
						 : ((r)->z = (b)->z))

#define mi_vector_max(r,a,b)	((a)->x > (b)->x ? ((r)->x = (a)->x) \
						 : ((r)->x = (b)->x),\
				 (a)->y > (b)->y ? ((r)->y = (a)->y) \
						 : ((r)->y = (b)->y),\
				 (a)->z > (b)->z ? ((r)->z = (a)->z) \
						 : ((r)->z = (b)->z))

#define mi_vector_det(a,b,c)	((a)->x * ((b)->y * (c)->z - (c)->y * (b)->z)+\
				 (b)->x * ((c)->y * (a)->z - (a)->y * (c)->z)+\
				 (c)->x * ((a)->y * (b)->z - (b)->y * (a)->z))

#define mi_vector_dist(a,b)	MI_SQRT(((a)->x - (b)->x) * ((a)->x - (b)->x)+\
					((a)->y - (b)->y) * ((a)->y - (b)->y)+\
					((a)->z - (b)->z) * ((a)->z - (b)->z))

/*
 * matrix operations (libmatrix.c)
 */

#define	  mi_matrix_copy(t,s)		(void)memcpy((t),(s),sizeof(miMatrix))
#define	  mi_matrix_null(m)		(void)memset((m), 0, sizeof(miMatrix))

void		mi_matrix_ident		(miMatrix);
void		mi_matrix_prod		(miMatrix,
					 miMatrix const,
					 miMatrix const);
miBoolean	mi_matrix_isident	(miMatrix const);
miBoolean	mi_matrix_isnull	(miMatrix const);
float		mi_matrix_rot_det	(miMatrix const);
miBoolean	mi_matrix_invert	(miMatrix, miMatrix const);
void		mi_point_transform	(miVector * const,
					 miVector const * const,
					 miMatrix const);
void		mi_vector_transform	(miVector * const,
					 miVector const * const,
					 miMatrix const);
void		mi_vector_transform_T	(miVector * const,
					 miVector const * const,
					 miMatrix const);
void		mi_matrix_rotate	(miMatrix, const miScalar,
					 const miScalar, const miScalar);
void		mi_matrix_rotate_axis	(miMatrix, miVector const * const,
					 miScalar const);
miBoolean	mi_matrix_solve		(miScalar *, miMatrix const,
					 miScalar const *, int);



miScalar	mi_spline		(miScalar, const int, miScalar *const);
miScalar	mi_random		(void);
miScalar	mi_erandom		(unsigned short seed[3]);
void		mi_srandom		(long newseed);
miScalar	mi_noise_1d		(const miScalar);
miScalar	mi_unoise_1d		(const miScalar);
miScalar	mi_noise_2d		(const miScalar, const miScalar);
miScalar	mi_unoise_2d		(const miScalar, const miScalar);
miScalar	mi_noise_3d		(miVector * const);
miScalar	mi_unoise_3d		(miVector * const);
miScalar	mi_noise_1d_grad	(const miScalar, miScalar * const);
miScalar	mi_unoise_1d_grad	(const miScalar, miScalar * const);
miScalar	mi_noise_2d_grad	(const miScalar, const miScalar,
					 miScalar * const, miScalar * const);
miScalar	mi_unoise_2d_grad	(const miScalar, const miScalar,
					 miScalar * const, miScalar * const);
miScalar	mi_noise_3d_grad	(miVector * const, miVector * const);
miScalar	mi_unoise_3d_grad	(miVector * const, miVector * const);


/*
 * error handler: severity levels for mi_set_verbosity (miERR_*), and
 * sub-levels to turn on individual message types per module (miMSG_*).
 */



void		mi_fatal		(const char * const, ...);
void		mi_error		(const char * const, ...);
void		mi_warning		(const char * const, ...);
void		mi_info			(const char * const, ...);
void		mi_progress		(const char * const, ...);
void		mi_debug		(const char * const, ...);
void		mi_vdebug		(const char * const, ...);
void		mi_nfatal		(const int, const char * const, ...);
void		mi_nerror		(const int, const char * const, ...);
void		mi_nwarning		(const int, const char * const, ...);
char		*mi_strerror		(void);



char *mi_string_substitute(	
	char	*new_name,		
	char	*name,			
	long	max_new_name_len);	


struct miState;
miScalar mi_luminance(struct miState *state, miColor *c);


typedef enum {
	miQ_NONE			= 0,
	miQ_VERSION			= 1,
	miQ_DATE			= 2,
	miQ_INST_FUNCTION		= 3,
	miQ_INST_GLOBAL_TO_LOCAL	= 4,
	miQ_INST_LOCAL_TO_GLOBAL	= 5,
	miQ_INST_IDENTITY		= 6,
	miQ_INST_ITEM			= 7,
	miQ_INST_PLACEHOLDER_ITEM	= 153,
	miQ_INST_PARENT			= 8,
	miQ_INST_HIDE			= 9,
	miQ_INST_VISIBLE		= 90,
	miQ_INST_SHADOW			= 91,
	miQ_INST_TRACE			= 92,
	miQ_INST_CAUSTIC		= 93,
	miQ_INST_GLOBILLUM		= 111,
	miQ_INST_FINALGATHER		= 151,
	miQ_INST_DECL			= 10,
	miQ_INST_PARAM_SIZE		= 11,
	miQ_INST_PARAM			= 12,
	miQ_INST_MATERIAL		= 83,
	miQ_INST_LABEL			= 101,
	miQ_INST_DATA			= 102,
        miQ_INST_AREA			= 125, 
	miQ_GROUP_MERGE_GROUP		= 13,
	miQ_GROUP_NKIDS			= 14,
	miQ_GROUP_KID			= 15,
	miQ_GROUP_LABEL			= 103,
	miQ_GROUP_DATA			= 104,
	miQ_OBJ_TYPE			= 16,
	miQ_OBJ_VISIBLE			= 17,
	miQ_OBJ_TRACE			= 18,
	miQ_OBJ_SHADOW			= 19,
	miQ_OBJ_VIEW_DEPENDENT		= 20,
	miQ_OBJ_CAUSTIC			= 21,
	miQ_OBJ_GLOBILLUM		= 112,
	miQ_OBJ_FINALGATHER		= 150,
	miQ_OBJ_LABEL			= 22,
	miQ_OBJ_DATA			= 100,
	miQ_LIGHT_TYPE			= 23,
	miQ_LIGHT_AREA			= 24,
	miQ_LIGHT_EXPONENT		= 25,
	miQ_LIGHT_CAUSTIC_PHOTONS	= 26,
	miQ_LIGHT_CAUSTIC_PHOTONS_EMIT	= 135,
	miQ_LIGHT_CAUSTIC_PHOTONS_EMITTED	= 144,
	miQ_LIGHT_GLOBAL_PHOTONS	= 27,
	miQ_LIGHT_GLOBAL_PHOTONS_EMIT	= 136,
	miQ_LIGHT_GLOBAL_PHOTONS_EMITTED	= 145,
	miQ_LIGHT_ENERGY		= 28,
	miQ_LIGHT_SHADER		= 29,
	miQ_LIGHT_EMITTER		= 30,
	miQ_LIGHT_ORIGIN		= 31,
	miQ_LIGHT_DIRECTION		= 32,
	miQ_LIGHT_AREA_R_EDGE_U		= 33,
	miQ_LIGHT_AREA_R_EDGE_V		= 34,
	miQ_LIGHT_AREA_D_NORMAL		= 35,
	miQ_LIGHT_AREA_D_RADIUS		= 36,
	miQ_LIGHT_AREA_S_RADIUS		= 37,
	miQ_LIGHT_AREA_C_RADIUS		= 96,
	miQ_LIGHT_AREA_C_AXIS		= 97,
	miQ_LIGHT_AREA_SAMPLES_U	= 38,
	miQ_LIGHT_AREA_SAMPLES_V	= 39,
	miQ_LIGHT_SPREAD		= 40,
	miQ_LIGHT_USE_SHADOWMAP		= 41,
	miQ_LIGHT_LABEL			= 98,
	miQ_LIGHT_DATA			= 99,
	miQ_LIGHT_SAMPLE_COORD		= 147,
	miQ_MTL_OPAQUE			= 42,
	miQ_MTL_SHADER			= 43,
	miQ_MTL_DISPLACE		= 44,
	miQ_MTL_SHADOW			= 45,
	miQ_MTL_VOLUME			= 46,
	miQ_MTL_ENVIRONMENT		= 47,
	miQ_MTL_CONTOUR			= 48,
	miQ_MTL_PHOTON			= 49,
	miQ_MTL_PHOTONVOL		= 50,
	miQ_MTL_LIGHTMAP		= 146,
 	miQ_FUNC_USERPTR		= 51,
	miQ_FUNC_LOCK			= 52,
	miQ_FUNC_TYPE			= 53,
	miQ_FUNC_DECL			= 54,
	miQ_FUNC_NEXT			= 55,
	miQ_FUNC_INDIRECT		= 56,
	miQ_FUNC_PARAM_SIZE		= 57,
	miQ_FUNC_RESULT_SIZE		= 58,
	miQ_FUNC_PARAM			= 59,
	miQ_FUNC_TLS_GET		= 124,
	miQ_FUNC_TLS_SET		= 126,
	miQ_FUNC_TLS_GETALL		= 127,
	miQ_FUNC_CALLTYPE		= 140,
	miQ_FUNC_TAG			= 141,
	miQ_FUNC_LENS_RECALL		= 149,
	miQ_DECL_LOCK			= 60,
	miQ_DECL_TYPE			= 61,
	miQ_DECL_RESULT_SIZE		= 62,
	miQ_DECL_NAME			= 63,
	miQ_DECL_PARAM			= 64,
	miQ_DECL_VERSION		= 89,
	miQ_IMAGE_WIDTH			= 65,
	miQ_IMAGE_HEIGHT		= 66,
	miQ_IMAGE_BITS			= 67,
	miQ_IMAGE_COMP			= 68,
	miQ_IMAGE_FILTER		= 69,
	miQ_IMAGE_DESATURATE		= 85,
	miQ_IMAGE_DITHER		= 86,
	miQ_IMAGE_NOPREMULT		= 87,
	miQ_IMAGE_GAMMA			= 88,
	miQ_IMAGE_COLORCLIP		= 115,
	miQ_NUM_GLOBAL_LIGHTS		= 70,
	miQ_GLOBAL_LIGHTS		= 71,
	miQ_TRANS_INTERNAL_TO_WORLD	= 72,
	miQ_TRANS_INTERNAL_TO_CAMERA	= 73,
	miQ_TRANS_INTERNAL_TO_OBJECT	= 74,
	miQ_TRANS_WORLD_TO_INTERNAL	= 75,
	miQ_TRANS_CAMERA_TO_INTERNAL	= 76,
	miQ_TRANS_OBJECT_TO_INTERNAL	= 77,
	miQ_TRANS_WORLD_TO_CAMERA	= 78,
	miQ_TRANS_WORLD_TO_OBJECT	= 79,
	miQ_TRANS_CAMERA_TO_WORLD	= 80,
	miQ_TRANS_OBJECT_TO_WORLD	= 81,
	miQ_NUM_TEXTURES		= 82,
	miQ_NUM_BUMPS			= 137,
	miQ_GEO_LABEL			= 84,
	miQ_GEO_DATA			= 114,
	miQ_GEO_HAS_DERIVS		= 138,
	miQ_GEO_HAS_DERIVS2		= 139,
	miQ_PRI_BBOX_MIN		= 94,
	miQ_PRI_BBOX_MAX		= 95,
	miQ_DATA_PARAM			= 105,
	miQ_DATA_DECL			= 106,
	miQ_DATA_NEXT			= 107,
	miQ_DATA_LABEL			= 108,
	miQ_DATA_PARAM_SIZE		= 109,
	miQ_DATA_NEEDSWAP		= 110,
	miQ_TILE_PIXELS			= 116,
	miQ_TILE_SAMPLES		= 117,
	miQ_FINALGATHER_STATE		= 142,
	miQ_SCENE_BBOX_ALL		= 118,
	miQ_SCENE_BBOX_SHADOW		= 119,
	miQ_SCENE_BBOX_GLOBILLUM_G	= 120,
	miQ_SCENE_BBOX_GLOBILLUM_R	= 121,
	miQ_SCENE_BBOX_CAUSTIC_G	= 122,
	miQ_SCENE_BBOX_CAUSTIC_R	= 123,
        miQ_LIGHTPROFILE_PHI_MIN        = 128,
        miQ_LIGHTPROFILE_PHI_MAX        = 129,
        miQ_LIGHTPROFILE_COSTHETA_MIN   = 130,
        miQ_LIGHTPROFILE_COSTHETA_MAX   = 131,
        miQ_LIGHTPROFILE_INTENSITY_MAX  = 132,
        miQ_LIGHTPROFILE_PHI_RES        = 133,
        miQ_LIGHTPROFILE_THETA_RES      = 134,
	miQ_PIXEL_SAMPLE		= 143,
	miQ_SCENE_ROOT_GROUP		= 148,
	miQ_TEXTURE_DIM			= 152,
	miNQ				= 154
} miQ_type;

struct miState;
miBoolean	mi_query(const miQ_type, struct miState * const,
		 miUint, void * const, ...);

					
#define miSS_LIFETIME_EYERAY	1	
#define miSS_LIFETIME_RECT	2	
					
miBoolean mi_shaderstate_set(
	struct miState	*state,		
	const char	*key,		
	void		*value,		
	int		valsize,	
	int		lifetime);	

void *mi_shaderstate_get(
	struct miState	*state,		
	const char	*key,		
	int		*valsize);	

void mi_shaderstate_enumerate(
	struct miState	*state,		
	miBoolean	(*cb)(void *arg, char *key, void *val, int vsz, int l),
	void		*arg);		
					
					
typedef enum miKdtree_type {
	miKDTREE_UNKNOWN		= 0,
	miKDTREE_3D,
	miKDTREE_NTYPES 		= miKDTREE_3D
} miKdtree_type;


struct miKdtree;

// typedef miBoolean ((miKdtree_insert_callback)
// 	(void 		*callback_data, 
// 	 void		*data,		
// 	 miScalar	point[],	
// 	 const void 	*shared_data)); 

// typedef miBoolean ((miKdtree_lookup_callback)
// 	(void 		*callback_data, 
// 	 const void	*data,		
// 	 const miScalar	point[]));
  

miTag mi_kdtree_create(
	miKdtree_type	type,		
	miUint		datasize);	

struct miKdtree *mi_kdtree_edit(
	miTag		tag,			
	const void	*shared_data,		
	miUint		shared_data_size);	

miBoolean mi_kdtree_add(
	struct miKdtree	*tree,		
	const void	*data,		
	const miScalar	point[]);	

miBoolean mi_kdtree_unpin(
	struct miKdtree	*tree);		

struct miKdtree *mi_kdtree_access(
	miTag			 tag,
	miKdtree_insert_callback *callback, 
	void			 *callback_data); 

miUint mi_kdtree_lookup(
	struct miKdtree	*tree,	     
	const miScalar	point[],     
	miUint		max_n,	     
	miScalar	max_dist,    
	miScalar	*max_dist_used,	   
	miKdtree_lookup_callback *callback,
	void		*callback_data);   

miBoolean mi_kdtree_iterate(
	struct miKdtree	*tree,		   
	miKdtree_lookup_callback *callback,
	void		*callback_data);   

miBoolean mi_kdtree_delete(
	miTag		tag);	
				




struct miMemblk;
typedef struct miMemblk miMemblk;


#define mi_mem_blkcreate(s)	mi_mem_int_blkcreate	(__FILE__,__LINE__,s)
#define mi_mem_strdup(s)	mi_mem_int_strdup	(__FILE__,__LINE__,s)
#define mi_mem_release(m)	mi_mem_int_release	(__FILE__,__LINE__,m)
#define mi_mem_allocate(s)	mi_mem_int_allocate	(__FILE__,__LINE__,s)
#define mi_mem_test_allocate(s)	mi_mem_int_test_allocate(__FILE__,__LINE__,s)
#define mi_mem_reallocate(m,s)	mi_mem_int_reallocate	(__FILE__,__LINE__,m,s)
#define mi_mem_test_reallocate(m,s)	mi_mem_int_test_reallocate\
							(__FILE__,__LINE__,m,s)


void		*mi_mem_int_allocate	   (const char * const, const int,
					    const int);
void		*mi_mem_int_test_allocate  (const char * const, const int,
					    const int);
void		*mi_mem_int_reallocate	   (const char * const, const int,
					    void *, const int);
void		*mi_mem_int_test_reallocate(const char * const, const int,
					    void *, const int);
void		 mi_mem_int_release	   (const char * const, const int,
					    void *);

							
void		 mi_mem_check		(void);
miUlong		 mi_mem_getsize		(void);
void		 mi_mem_summary		(void);
void		 mi_mem_dump		(const miModule);

char		*mi_mem_int_strdup	(const char * const, const int,
					 const char *);

/*
 * block allocator. note that the current internal block size is 8Kb, and
 * we assume that at least two items fit on a page. hence the largest block
 * you should use the block allocator for is around 4000 bytes. even in this
 * range the efficiency of using a block allocator is noticeably lower than
 * for smaller blocks. it is recommended to use the block allocator only up
 * to a block size of about 1Kb.
 */

miMemblk	*mi_mem_int_blkcreate	(const char * const, const int, int);
void		 mi_mem_blkdelete	(miMemblk *);
void		*mi_mem_blkallocate	(miMemblk *);
void		*mi_mem_blkfallocate	(miMemblk *);
void		 mi_mem_blkrelease	(miMemblk *, void *);
void		 mi_mem_blkfrelease	(miMemblk *, void *);
void		 mi_mem_blkclear	(miMemblk *);
void		 mi_mem_blkenumerate	(miMemblk *, void (*)(void *));
							



					 

#define miNULLTAG 0
					
					
void  *mi_db_access(
			const miTag);	
int    mi_db_type(
			const miTag);	
int    mi_db_size(
			const miTag);	
miBoolean mi_db_isjob(
			const miTag);	
void  *mi_db_access_type(
			int * const,	
			const miTag);	
void   mi_db_unpin(
			const miTag);	
void  *mi_db_create(
			miTag * const,	
			const int,	
			const int);	
void  *mi_db_realloc(
			const miTag,	
			const int);		
void   mi_db_delete(
			const miTag);	
miTag  mi_db_copy(
			const miTag);	
void   mi_db_dump(
			const miModule);
					




/*
 * supported internal scanline types. These types are used when a file is
 * opened and created; they specify the internal data structure but not the
 * type of the file to be created. If you change this list, also change
 * compatible_type[] in imgcall.c. The use as array indexes is the reason
 * why every enumerator is assigned explicitly, to show that the numbers
 * are important. No real type may exceed 31!
 * miIMG_TYPE_ANY is a wildcard that disables conversion. The USER types
 * are assigned real types in "frame buffer" statements. Defining them here
 * allows storing them in the out_type and out_interp bitmaps in miFunction.
 * No type should have a code >31 because output statements use type bitmaps.
 */

struct miColor_profile;

typedef enum {
	miIMG_TYPE_RGBA		= 0,	
	miIMG_TYPE_RGBA_16	= 1,	
	miIMG_TYPE_RGB		= 2,	
	miIMG_TYPE_RGB_16	= 3,	
	miIMG_TYPE_A		= 4,	
	miIMG_TYPE_A_16		= 5,	
	miIMG_TYPE_S		= 6,	
	miIMG_TYPE_S_16		= 7,	
	miIMG_TYPE_VTA		= 8,	
	miIMG_TYPE_VTS		= 9,	
	miIMG_TYPE_Z		= 10,	
	miIMG_TYPE_N		= 11,	
	miIMG_TYPE_M		= 14,	
	miIMG_TYPE_TAG		= 12,	
	miIMG_TYPE_BIT		= 15,	
	miIMG_TYPE_RGBA_FP	= 13,	
	miIMG_TYPE_RGB_FP	= 16,	
	miIMG_TYPE_COVERAGE	= 17,	
	miIMG_TYPE_RGBE		= 18,	
	miIMG_TYPE_A_FP		= 19,	
	miIMG_TYPE_S_FP		= 20,	
	miIMG_TYPE_RGBA_H	= 21,	
	miIMG_TYPE_RGB_H	= 22,	
	miIMG_NTYPES		= 23,
					
	miIMG_TYPE_CONT		= 29,	
	miIMG_TYPE_ANY		= 30,	
	miIMG_TYPE_USER_FIRST	= 31	
} miImg_type;

#define miIMG_TYPE_ERROR miIMG_NTYPES





/*
 * supported file formats. These are used for one purpose only: when creating
 * an image file, they specify the output format. When adding or inserting
 * an entry, add external declarations and entries to all arrays defined as
 * "something[miIMG_NFORMATS]" at the beginning of imgdispatch.c. Make sure
 * the order is correct!
 * Generally, common formats should precede uncommon, and formats difficult
 * to recognize by their headers should come last (qnt, zpic). QNT_PAL must
 * be QNT_NTSC-1, and there may be only two QNT formats (see mi_img_open).
 */

typedef enum {
	miIMG_FORMAT_RLA	= 0,	
	miIMG_FORMAT_RLB	= 1,	
	miIMG_FORMAT_PIC	= 2,	
	miIMG_FORMAT_ALIAS	= 3,	
	miIMG_FORMAT_RGB	= 4,	
	miIMG_FORMAT_CT		= 5,	
	miIMG_FORMAT_CT_16	= 6,	
	miIMG_FORMAT_ST		= 7,	
	miIMG_FORMAT_ST_16	= 8,	
	miIMG_FORMAT_VT		= 9,	
	miIMG_FORMAT_WT		= 10,	
	miIMG_FORMAT_ZT		= 11,	
	miIMG_FORMAT_NT		= 12,	
	miIMG_FORMAT_MT		= 28,	
	miIMG_FORMAT_TT		= 13,	
	miIMG_FORMAT_BIT	= 29,	
	miIMG_FORMAT_TIFF	= 14,	
	miIMG_FORMAT_TIFF_U	= 15,	
	miIMG_FORMAT_TIFF_3	= 16,	
	miIMG_FORMAT_TIFF_U3	= 17,	
	miIMG_FORMAT_TIFF_16	= 30,	
	miIMG_FORMAT_TIFF_16_U	= 31,	
	miIMG_FORMAT_TIFF_16_3	= 32,	
	miIMG_FORMAT_TIFF_16_U3	= 33,	
	miIMG_FORMAT_CATIA	= 18,	
	miIMG_FORMAT_PPM	= 19,	
	miIMG_FORMAT_CT_FP	= 20,	
	miIMG_FORMAT_MAP	= 21,	
	miIMG_FORMAT_TARGA	= 22,	
	miIMG_FORMAT_BMP	= 23,	
	miIMG_FORMAT_QNT_PAL	= 24,	
	miIMG_FORMAT_QNT_NTSC	= 25,	
	miIMG_FORMAT_ZPIC	= 26,	
	miIMG_FORMAT_NULL	= 27,	
	miIMG_FORMAT_JPEG	= 34,	
	miIMG_FORMAT_LWI	= 35,	
	miIMG_FORMAT_PS		= 36,	
	miIMG_FORMAT_EPS	= 37,	
	miIMG_FORMAT_CT_H	= 38,	
	miIMG_FORMAT_HDR	= 39,	
	miIMG_FORMAT_ST_FP	= 40,	
	miIMG_FORMAT_IFF	= 41,	
	miIMG_FORMAT_CUSTOM_0	= 42,	
	miIMG_FORMAT_CUSTOM_1	= 43,	
	miIMG_FORMAT_CUSTOM_2	= 44,	
	miIMG_FORMAT_CUSTOM_3	= 45,	
	miIMG_FORMAT_CUSTOM_4	= 46,	
	miIMG_FORMAT_CUSTOM_5	= 47,	
	miIMG_FORMAT_CUSTOM_6	= 48,	
	miIMG_FORMAT_CUSTOM_7	= 49,	
	miIMG_FORMAT_CUSTOM_8	= 50,	
	miIMG_FORMAT_CUSTOM_9	= 51,	
	miIMG_FORMAT_CUSTOM_10	= 52,	
	miIMG_FORMAT_CUSTOM_11	= 53,	
	miIMG_FORMAT_CUSTOM_12	= 54,	
	miIMG_FORMAT_CUSTOM_13	= 55,	
	miIMG_FORMAT_CUSTOM_14	= 56,	
	miIMG_FORMAT_CUSTOM_15	= 57,	
	miIMG_FORMAT_CUSTOM_LAST= 57,	
	miIMG_FORMAT_PNG	= 58,	
	miIMG_FORMAT_SHMAP	= 59,   
	miIMG_FORMAT_EXR	= 60,   
	miIMG_NFORMATS		= 61
} miImg_format;

#define miIMG_FORMAT_ERROR miIMG_NFORMATS


/*
 * error codes. If you add one, also add a message to errmsg[] in libimg.c.
 */

typedef enum {
	miIMG_ERR_NONE		= 0,	
	miIMG_ERR_READ		= 1,	
	miIMG_ERR_WRITE		= 2,	
	miIMG_ERR_DECODE	= 3,	
	miIMG_ERR_ENCODE	= 4,	
	miIMG_ERR_TYPE		= 5,	
	miIMG_ERR_SUBTYPE	= 6,	
	miIMG_ERR_CREATE	= 7,	
	miIMG_ERR_OPEN		= 8,	
	miIMG_ERR_CLOSE		= 9,	
	miIMG_ERR_ARGS		= 10,	
	miIMG_ERR_NOTOPEN	= 11,	
	miIMG_ERR_NOTCLOSED	= 12,	
	miIMG_ERR_SIZE		= 13,	
	miIMG_NERRS
} miImg_error;


/*
 * file descriptor. The file name is stored to be able to print meaningful
 * error messages. It is an array, not a strdup'ed pointer, because it is
 * not clear who would de-allocate the name otherwise. If the file name is
 * longer than the array, it is truncated. The real file name is stored
 * too, but it can't be relied upon to exist. Gamma is not used and may go
 * away in future versions.
 * Other modules may only use the public fields. The others are reserved
 * by raylib (and in fact not documented in the external manual). The fp
 * field is actually a FILE*, but it's void* here because otherwise every
 * file including img.h would have to include stdio.h as well. It's always
 * cast to FILE* in img*.c.
 */

typedef struct {
	int		c[4];		
} miImg_line;				

typedef struct miImg_file {
	int		null;		
					
	int		width, height;	
	int		bits;		
	int		comp;		
	miBoolean	filter;		
	miBoolean	topdown;	
	int		filesize;	
	float		gamma;		
	float		aspect;		
	float		parms[8];	
					/* [0] is .jpg quality: 0..100
					 *     or .exr compression type:
					 *     0=default, 1=none, 2=piz,
					 *     3=zip, 4=rle, 5=pxr24 */
					
	                      		
					
	                                
	                                
	                                
	miImg_type	type;		
	miImg_format	format;		
	miImg_error	error;		
	int		os_error;	
					
	int		magic;		
	int		lineno;		
	miBoolean	writing;	
	miCBoolean	swap_map;	
	miUint1		ftype;		
					
					
					
	miCBoolean	writable;	
	miUint1		pyr_level;	
	miPointer	fp;		
	miPointer	real_name;	
	miPointer	convline;	
	miPointer	data;		
					
	char		name[64];	
	miTag		colorprofile;	
	miCBoolean	win_error;	
	miCBoolean	spare2[3];
	int		spare[12];	
} miImg_file;


/*
 * generic scanline and image struct. They are based on index values that
 * define the beginning of component scanlines relative to the beginning of
 * the struct (including header and all). Both miImg_line and miImg_image
 * are actually allocated larger than their size; the component scanlines
 * directly follow the typedef'ed header. One mi_mem_release suffices for
 * the whole thing including data.
 * <comp> in the access macro stands for one of the miIMG_* constants below.
 * Do not use miIMG_LINEACCESS before using miIMG_ACCESS to load local texs!
 */

#define miIMG_R		0		
#define miIMG_G		1		
#define miIMG_B		2		
#define miIMG_A		3		
#define miIMG_E		3		
#define miIMG_S		miIMG_A		
#define miIMG_U		0		
#define miIMG_V		1		
#define miIMG_Z		0		
#define miIMG_COVERAGE	0		
#define miIMG_T		0		
#define miIMG_NX	0		
#define miIMG_NY	1		
#define miIMG_NZ	2		


#define miIMG_ACCESS(_i, _y,_c)\
        ((_i)->cacheable \
         ? ((miUchar *) mi_img_cache_get((_i), (_i)->c[4*(_y)+(_c)]))\
	 : ((miUchar *)(_i) + (_i)->c[4*(_y)+(_c)]))


#define miIMG_LINEACCESS(_l,_c)	((miUchar *)(_l)+(_l)->c[(_c)])


#define miIMG_DIRSIZE	20		

typedef struct miImg_image {
	miScalar	filter;		
	int		dirsize;	
	int		dir[miIMG_DIRSIZE]; 
	int		width, height;	
	int		bits;		
	int		comp;		
	miCBoolean	local;		
	miCBoolean	writable;	
        miCBoolean	cacheable;	
	miCBoolean	remap;		
	int		type;		
	miTag		real_name;	
	miTag		colorprofile;	
	int		c[4];		
					
					
} miImg_image;
							
							

typedef struct miImg_tonemap {
	miScalar	ref;		
	miScalar	tolerance;	
	miScalar	low_out;	
	miScalar	hi_out;		
	miScalar	low_in;		
	miScalar	hi_in;		
	miColor		lum_weights;	
	int		n_bins;		
	int		x_res;		
	int		y_res;		
	int		x_lo;		
	int		y_lo;		
	int		x_hi;		
	int		y_hi;
	miBoolean	linear;	  	
	miBoolean	desaturate;	
	miTag		map;		
					
} miImg_tonemap;
							
							
miBoolean    	mi_img_create(
				miImg_file *const,	
				char * const,		
				const miImg_type,	
				const miImg_format,	
				const int,		
				const int);		
miBoolean    	mi_img_open(
				miImg_file *const,	
				char * const,		
				const miImg_type);	
miBoolean    	mi_img_close(
				miImg_file *const);	
miBoolean	mi_img_line_read(
				miImg_file * const,	
				miImg_line * const,	
				struct miColor_profile *,
				struct miColor_profile *,
				struct miColor_profile *,
				miBoolean,		
				miColor *);		

miBoolean    	mi_img_line_write(
				miImg_file *const,	
				miImg_line *const);	

miBoolean    	mi_img_image_read(
				miImg_file *const,	
				miImg_image*const);	
miBoolean    	mi_img_image_write(
				miImg_file *const,	
				miImg_image*const);	
miImg_image 	*mi_img_mmap_address(
				miImg_file *);		
miBoolean	mi_img_tonemap(
				miImg_image*,		
				miImg_image*,		
				miImg_tonemap*);	

							
							
miImg_image 	*mi_img_image_dbcreate(
				miImg_file *const,	
				miTag * const);		
miBoolean      	mi_img_placeholder_dbcreate(
				miImg_file * const,	
				miTag * const,		
				miBoolean);		
miBoolean mi_img_image_shared(
			        miTag,			
				miTag);			
                                                        
miImg_image 	*mi_img_mmap_address(
				miImg_file *ifp);	
void		mi_img_image_release(
				miImg_image *img);	

miUchar     	*mi_img_access(
				miImg_image	*,	
				const int,		
				const int);		
miImg_image	*mi_img_image_alloc(
				miImg_file *const);	
miImg_type	mi_img_type_identify(
				char * const);		
miImg_format	mi_img_format_identify(
				char * const);		
miImg_type	mi_img_best_type(
				const miImg_format);	
char		*mi_img_type_name(
				const miImg_type);	
char		*mi_img_format_name(
				const miImg_format);	
void		mi_img_type_query(
				const miImg_type,	
				int *,			
				int *,			
				miCBoolean *);		/* ret. comp mask.
							 * set to NULL if not
							 * wanted */
							
							
typedef enum {						
	miIMG_COLORCLIP_RGB,				
	miIMG_COLORCLIP_ALPHA,				
	miIMG_COLORCLIP_RAW				
} miImg_colorclip;
							
							
void		mi_img_clip_color(
				miImg_image *const,	
				miColor *const);	
void		mi_img_put_color(
				miImg_image *const,	
				miColor * const,	
				const int,		
				const int);		
void		mi_img_put_scalar(
				miImg_image *const,	
				const float,		
				const int,		
				const int);		
void		mi_img_put_vector(
				miImg_image *const,	
				miVector * const,	
				const int,		
				const int);		
void		mi_img_put_depth(
				miImg_image *const,	
				const float,		
				const int,		
				const int);		
void		mi_img_put_coverage(
				miImg_image *const,	
				const float,		
				const int,		
				const int);		
void		mi_img_put_normal(
				miImg_image *const,	
				miVector * const,	
				const int,		
				const int);		
void		mi_img_put_label(
				miImg_image *const,	
				const miUint,		
				const int,		
				const int);		
void		mi_img_get_color(
				miImg_image *const,	
				miColor * const,	
				const int,		
				const int);		
void		mi_img_get_scalar(
				miImg_image *const,	
				float * const,		
				const int,		
				const int);		
void		mi_img_get_vector(
				miImg_image *const,	
				miVector * const,	
				const int,		
				const int);		
void		mi_img_get_depth(
				miImg_image *const,	
				float * const,		
				const int,		
				const int);		
void		mi_img_get_coverage(
				miImg_image *const,	
				float * const,		
				const int,		
				const int);		
void		mi_img_get_normal(
				miImg_image *const,	
				miVector * const,	
				const int,		
				const int);		
void		mi_img_get_label(
				miImg_image *const,	
				miUint * const,		
				const int,		
				const int);		
miImg_image	*mi_img_pyramid_get_level(
				miImg_image *,		
				int);			
							

miImg_image	*mi_lightmap_edit(			
				void **handle,		
				miTag tag);		
void		mi_lightmap_edit_end(			
				void *handle);		

							
void *mi_img_cache_get(
		const miImg_image	*image,		
		miUint			offset);	



struct miColor_profile;		

miUint1 mi_colorprofile_renderspace_id(void);
miUint1 mi_colorprofile_internalspace_id(void);
miUint1 mi_colorprofile_ciexyz_color_id(void);

miBoolean mi_colorprofile_ciexyz_to_internal(
		miColor			*c);		

miBoolean mi_colorprofile_internal_to_ciexyz(
		miColor			*c);		

miBoolean mi_colorprofile_internal_to_render(
		miColor			*c);		

miBoolean mi_colorprofile_render_to_internal(
		miColor			*c);		

miBoolean mi_colorprofile_ciexyz_to_render(
		miColor			*c);		

miBoolean mi_colorprofile_render_to_ciexyz(
		miColor			*c);		

							



						

#if defined(LOCK_DEBUG)
  miBoolean mi_init_lock_dbg(miLock *, char *, int);
# define mi_init_lock(p) mi_init_lock_dbg(p, __FILE__, __LINE__)
#else
  miBoolean mi_init_lock(miLock *);
#endif



void mi_lock_thread_suspend(miLock);

  void mi_delete_lock(miLock *); 
  void mi_lock(miLock);          
  void mi_unlock(miLock);        
  miBoolean mi_trylock(miLock);

					
void mi_abort(void);

int mi_par_aborted(void);
					



struct miState;
void *mi_renderpass_access(		
	struct miState	*state,		
	int		pass,		
	int		fb);		

miBoolean mi_renderpass_resolution(
	int		*xl,		
	int		*yl,		
	int		*xs,		
	int		*ys,		
	int		*xres,		
	int		*yres,		
	struct miState	*state,		
	int		x,		
	int		y);		

miBoolean mi_renderpass_sample_get(
	void		*result,	
	int		resultsize,	
	struct miState	*state,		
	int		fb,		
	int		x,		
	int		y);		

miBoolean mi_renderpass_sample_put(
	void		*result,	
	int		resultsize,	
	struct miState	*state,		
	int		fb,		
	int		x,		
	int		y);		

miBoolean mi_renderpass_samplerect_black(
	miBoolean	*result,	
	struct miState	*state,		
	int		x,		
	int		y);		

miBoolean mi_renderpass_samplerect_flush(
	struct miState	*state,		
	int		pass,		
	int		x,		
	int		y);		
/*
 * Set the falloff start/stop. If the intersecton point is closer than
 * start, result of the intersection is taken. If it lies between start
 * ebd stop, the result is linearly interpoletad from surface shader and
 * environment shader. If intersection point is behind stop, the environment 
 * shader result is taken. Obviously, intersections behind stop may be ignored
 * for optimization.
 */
miBoolean mi_ray_falloff(
	const struct miState	*state,		
	double			*start,		
	double			*stop);		

/*
 * Set the distance to the normal plane under which the intersections
 * would be ignored. The main purpose is to allow rendering of objects
 * with different approximation levels.
 */
miBoolean mi_ray_offset(
	const struct miState	*state,		
        double			*offset);	




					
miScalar     mi_lightprofile_value	(const void *, miScalar, miScalar,
					 const miVector *, miBoolean);
struct miState;
miScalar     mi_lightprofile_sample	(struct miState *, miTag, miBoolean);
					




/*
 * Every DB element managed by SCENE has a unique type and is known by SCENE.
 * mi_db_create should not be used by modules other than SCENE because that
 * makes it hard to properly swap bytes between sane parts of the
 * network and Windows NT.
 * When adding types, change the arrays in scncall1.c!
 */

typedef enum {
	miSCENE_NONE = 0,	
	miSCENE_FUNCTION_DECL,	
	miSCENE_FUNCTION,	
	miSCENE_MATERIAL,	
	miSCENE_LIGHT,		
	miSCENE_CAMERA,		
	miSCENE_BASIS_LIST,	
	miSCENE_OBJECT,		
	miSCENE_BOX,		
	miSCENE_LINEBOX,	
	miSCENE_INSTANCE,	
	miSCENE_GROUP,		
	miSCENE_OPTIONS,	
	miSCENE_IMAGE,		
	miSCENE_POLYGON,	
	miSCENE_INDEX,		
	miSCENE_GEOINDEX,	
	miSCENE_VERTEX,		
	miSCENE_GEOVERTEX,	
	miSCENE_VECTOR,		
	miSCENE_GEOVECTOR,	
	miSCENE_FACE,		
	miSCENE_SURFACE,	
	miSCENE_CURVE,		
	miSCENE_CURVPNT,	
	miSCENE_SCALAR,		
	miSCENE_GEOSCALAR,	
	miSCENE_ALGEBRAIC,	
	miSCENE_TAG,		
	miSCENE_STRING,		
	miSCENE_SPACECURVE,	
	miSCENE_BOOK,		
	miSCENE_PAGE,		
	miSCENE_SUBDIVSURF,	
	miSCENE_USERDATA,	
	miSCENE_WORLD,		
	miSCENE_SESSION,	
	miSCENE_TRANSACTION,	
	miSCENE_SPLIT,		
	miSCENE_IMATTER,	
	miSCENE_NBOX,		
	miSCENE_LOCAL,		
	miSCENE_SPLITOBJECT,    
	miSCENE_INDEXRANGE, 	
        miSCENE_LIGHTPROFILE,   
        miSCENE_HARDWARE,       
	miSCENE_LIGHTPROFILE_STD,	
        miSCENE_APPROX_LIST,    
        miSCENE_VENDOR,         
	miSCENE_FBINFO,		
	miSCENE_COLORPROFILE,	
	miSCENE_SPECTRUM,	
	miSCENE_BSDF,		
	miSCENE_FLIST,		
	
	miSCENE_NTYPES = 64	
} miScene_types;




					
enum miApprox_method {
	miAPPROX_PARAMETRIC,
	miAPPROX_REGULAR,
	miAPPROX_SPATIAL,
	miAPPROX_CURVATURE,
	miAPPROX_LDA,
	miAPPROX_ADJACENCY,		
	miAPPROX_ALGEBRAIC,		
	miAPPROX_DEFTRIM,		
	miAPPROX_REGULAR_PERCENT,	
	miAPPROX_INDIRECT,		
					
					
					
	miAPPROX_NMETHODS
};

enum miApprox_style {
	miAPPROX_STYLE_NONE = 0,	
	miAPPROX_STYLE_GRID,		
	miAPPROX_STYLE_TREE,
	miAPPROX_STYLE_DELAUNAY,
	miAPPROX_STYLE_FINE,		
        miAPPROX_STYLE_FINE_NO_SMOOTHING,/* fine poly displacement without
                                            Hermite interpolation */
	miAPPROX_NSTYLES
};

#define miCNST_UPARAM	  0		
#define miCNST_VPARAM	  1
#define miCNST_LENGTH	  0		
#define miCNST_DISTANCE   1
#define miCNST_ANGLE	  2

enum miCnst_type {
	miCNST_VISIBLE	= 0,		
	miCNST_TRACE,
	miCNST_SHADOW,
	miCNST_CAUSTIC,
	miCNST_GLOBILLUM,
	miCNST_NTYPES			
};

enum miApprox_flag {
	miAPPROX_FLAG_ANY	= 0,			
	miAPPROX_FLAG_VISIBLE	= 1 << miCNST_VISIBLE,	
	miAPPROX_FLAG_TRACE	= 1 << miCNST_TRACE,	
	miAPPROX_FLAG_SHADOW	= 1 << miCNST_SHADOW,	
	miAPPROX_FLAG_CAUSTIC	= 1 << miCNST_CAUSTIC,	
	miAPPROX_FLAG_GLOBILLUM = 1 << miCNST_GLOBILLUM 
};

typedef struct miApprox {
	miScalar	     cnst[6];	
	miUint1 	     sharp;	
	miUint1 	     flag;	
	miCBoolean	     any;	
	miCBoolean	     view_dep;	
	enum miApprox_method method;
	enum miApprox_style  style;
	miUshort	     subdiv[2]; 
	miGeoIndex	     max;	
					/* in case of style contour:
					   resolution of sampling grid	     */
	miScalar	     grading;	
} miApprox;


					
#define miAPPROX_MAX_SUBDIV 7
#define miAPPROX_NONE(A) memset(&(A), 0, sizeof(miApprox))

#define miAPPROX_DEFAULT(A) do {		 \
	(A).style	  = miAPPROX_STYLE_TREE; \
	(A).method	  = miAPPROX_PARAMETRIC; \
	(A).sharp	  = 0;			 \
	(A).flag	  = miAPPROX_FLAG_ANY;	 \
	(A).any 	  = miFALSE;		 \
	(A).view_dep	  = miFALSE;		 \
	(A).cnst[0]	  = 0.0;		 \
	(A).cnst[1]	  = 0.0;		 \
	(A).cnst[2]	  = 0.0;		 \
	(A).cnst[3]	  = 0.0;		 \
	(A).cnst[4]	  = 0.0;		 \
	(A).cnst[5]	  = 0.0;		 \
	(A).subdiv[miMIN] = 0;			 \
	(A).subdiv[miMAX] = 5;			 \
	(A).max 	  = miHUGE_INT; 	 \
	(A).grading	  = 0.0; } while (0)
#define miAPPROX_FINE_DEFAULT(A) do {		 \
	(A).style	  = miAPPROX_STYLE_FINE; \
	(A).method	  = miAPPROX_LDA;	 \
	(A).sharp	  = 0;			 \
	(A).flag	  = (1<<miCNST_NTYPES)-1;\
	(A).any 	  = miFALSE;		 \
	(A).view_dep	  = miTRUE;		 \
	(A).cnst[0]	  = 0.25;		 \
	(A).cnst[1]	  = 0.0;		 \
	(A).cnst[2]	  = 0.0;		 \
	(A).cnst[3]	  = 0.0;		 \
	(A).cnst[4]	  = 0.0;		 \
	(A).cnst[5]	  = 0.0;		 \
	(A).subdiv[miMIN] = 0;			 \
	(A).subdiv[miMAX] = miAPPROX_MAX_SUBDIV; \
	(A).max 	  = miHUGE_INT; 	 \
	(A).grading	  = 0.0; } while (0)
					
						
#define miRC_IMAGE_RGBA 	0		
#define miRC_IMAGE_Z		1		
#define miRC_IMAGE_N		2		
#define miRC_IMAGE_M		3		
#define miRC_IMAGE_TAG		4		
#define miRC_IMAGE_COVERAGE	5		
#define miRC_IMAGE_PPREV	7		
						
#define miRC_IMAGE_USER		12		
#define miRC_CLIP_MIN		0.001		

#define miRc_options		miOptions	


#define miSCENE_DIAG_SAMPLES	1		
#define miSCENE_DIAG_PHOTON	6		
#define miSCENE_DIAG_PHOTON_D	2		
#define miSCENE_DIAG_PHOTON_I	4		
#define miSCENE_DIAG_GRID	24		
#define miSCENE_DIAG_GRID_O	8		
#define miSCENE_DIAG_GRID_W	16		
#define miSCENE_DIAG_GRID_C	24		
#define miSCENE_DIAG_BSP_D	32		
#define miSCENE_DIAG_BSP_L	64		
#define miSCENE_DIAG_BSP	96		
#define miSCENE_DIAG_FG 	128		


#define miSCENE_HWDIAG_WIRE	1		
#define miSCENE_HWDIAG_SOLID	2		
#define miSCENE_HWDIAG_MESH	3		
#define miSCENE_HWDIAG_LIGHTS	4		
#define miSCENE_HWDIAG_WINDOW	8		
#define miSCENE_HWDIAG_DEBUG	16		
						


#define miSCENE_MOTION_ON	1		
#define miSCENE_MOTION_C_INT	2		


#define miSHADOWMAP_MERGE	0x01
#define miSHADOWMAP_TRACE	0x02
#define miSHADOWMAP_CROP	0x04
#define miSHADOWMAP_ONLY	0x08
#define miSHADOWMAP_CAMERA	0x10
#define miSHADOWMAP_DETAIL	0x20
#define miSHADOWMAP_TILE	0x40


#define miLIGHTMAP_OFF		0
#define miLIGHTMAP_ON		1
#define miLIGHTMAP_ONLY		2
						
						


#define miHW_OFF                0x0             
#define miHW_ON                 0x1             
#define miHW_ALL                0x2             


#define miHW_DEFAULT            0x0             
#define miHW_FORCE              0x1             
#define miHW_CG                 0x2             
#define miHW_NATIVE             0x4             
#define miHW_FAST               0x8             

/*
 * Frame buffer information entry. options define an array of these
 * entries (images_info).
 * swapped as integer vector.
 */

typedef struct miFb_info {
	miImg_type	image_type;		
						
	miUint4		write_image;		
						
						
	miBoolean	interp_image;		
} miFb_info;

typedef struct miOptions {
	miBoolean	trace;			
	int		scanline;		/* 0=off, 1=on, 'o'=OpenGL, 
						 * 'r'=rapid */
	miUint		motion; 		
	miBoolean	shadow_sort;		
	miBoolean	preview_mode;		
	int		reflection_depth;	
	int		refraction_depth;	
	int		trace_depth;		
	int		min_samples;		
	int		max_samples;		
	miColor 	contrast;		
	miColor 	time_contrast;		
	miTag		contour_contrast;	
	miTag		contour_store;		
	miBoolean	caustic;		
	miBoolean	globillum;		
	int		caustic_accuracy;	
	float		caustic_radius; 	
	int		globillum_accuracy;	
	float		globillum_radius;	
	float		caustic_filter_const;	
	float		filter_size_x;		
	float		filter_size_y;		
	float		jitter; 		
	float		shutter;		
	short		grid_res[3];		
	short		grid_max_size;		
	short		grid_max_depth; 	
	miSint1 	def_min_samples;	
	miSint1 	def_max_samples;	
	int		space_max_size; 	
	int		space_max_depth;	
	float		shutter_delay;		
	int		no_images;		
	miUint		spare1[47];		
	miTag		images_info;		

	miTag		finalgather_file;	
	miCBoolean	space_shadow_separate;	
	miUchar 	finalgather_filter;	
	char		rapid_collect_rate;	
						
						
	char		rapid_motion_resample;	
						
	char		use_shadow_maps;	
						
	miCBoolean	rendering_shadow_maps;	
	char		recompute_shadow_maps;	
	char		shadow; 		
						
						
	char		caustic_filter; 	
						
						
	miCBoolean	finalgather_rebuild;	
	char		filter; 		
						
						
	char		acceleration;		
						
						
						
	char		face;			
						
						
	char		field;			
						
	char		smethod;		
	char		render_space;		
						
						
						
	miCBoolean	pixel_preview;		
	miCBoolean	task_preview;		
	miCBoolean	visible_lights; 	
	miCBoolean	shadow_map_motion;	
	int		task_size;		
	miBoolean	strips; 		/* GAP returns triangles in
						 * strips */
	miTag		photonmap_file; 	
	miBoolean	photonmap_rebuild;	
	int		photon_reflection_depth;
	int		photon_refraction_depth;
	int		photon_trace_depth;	
	int		space_max_mem;		
	miColor		caustic_scale;		
	miColor		globillum_scale;	
	miColor		finalgather_scale;	
	miUint		spare2[15];		
	miTag		spectrum_blueprint;	
	float		lum_efficacy;		
	miTag		white_cprof;		
	miTag		render_cprof;		
	miTag		fb_dir;			
	miCBoolean	no_lens;		
	miCBoolean	no_volume;		
	miCBoolean	no_geometry;		
	miCBoolean	no_displace;		
	miUint1 	no_output;		
	miCBoolean	no_merge;		
	miUint1 	caustic_flag;		
	miUint1 	diagnostic_mode;	
	int		photonvol_accuracy;	
	float		photonvol_radius;	
	miUint1 	globillum_flag; 	
	miCBoolean	samplelock;		
	miCBoolean	autovolume;		
	miCBoolean	finalgather_view;	
	miCBoolean	no_hair;		
	miUint1 	n_motion_vectors;	
	miCBoolean	no_pass;		
	miCBoolean	photon_autovolume;	
	miCBoolean	no_predisplace; 	
	miUint1 	fg_reflection_depth;	
	miUint1 	fg_refraction_depth;	
	miUint1 	fg_trace_depth; 	
	float		fg_falloff_start;	
	float		fg_falloff_stop;	
	miTag		userdata;		
	miApprox	approx; 		
	miApprox	approx_displace;	
	int		finalgather;		
	int		finalgather_rays;	
	float		finalgather_maxradius;	
	float		finalgather_minradius;	
	float		diag_photon_density;	
	float		diag_grid_size; 	
	miBoolean	desaturate;		
	miBoolean	dither; 		
	miBoolean	nopremult;		
	int		colorclip;		
	float		gamma;			
	miTag		inh_funcdecl;		
	miColor 	luminance_weight;	
	int		split_obj_no_triangs;	/* max no_triangs for split of
						 * miOBJECT_POLYGONS */
	int		split_obj_no_faces;	/* max no_faces for split of
						 * miOBJECT_FACES */
	miCBoolean	inh_is_traversal;	
						
	miUint1 	hardware;		
	miUint1 	hardware_diagnostic;	
	miUint1 	shadowmap_flags;	
	float		maxdisplace;		
	miScalar	shadowmap_bias;		
	miUint1		hwshader;		
                                                
	miCBoolean	photonmap_only;		
	miUint1		lightmap;		
	                                        
	miUint1		fg_diffuse_depth;	
	miTag		state_func;		
	float		fg_presamp_density;	/* samples density for precomp.
						 * finalgather points */
	float		rapid_shading_samples;	
						
	miCBoolean	fb_virtual;		
	miCBoolean	spare3[3];		
	int		spare[10];		
} miOptions;
						
						
typedef struct miCamera {
	miBoolean	orthographic;		
	float		focal;			
	float		aperture;		
	float		aspect; 		
	miRange 	clip;			
	int		x_resolution;		
	int		y_resolution;		
	struct {int xl, yl, xh, yh;}
			window; 		
	miTag		volume; 		
	miTag		environment;		
	miTag		lens;			
	miTag		output; 		
	int		frame;			
	float		frame_time;		
	int		frame_field;		
	float		x_offset;		
	float		y_offset;		
	miTag		userdata;		
	miTag		pass;			
	float		focus;			
	float		radius;			
	miBoolean	pass_mask;		
	int		spare[11];		
} miCamera;
						
						
enum miLight_type {
	miLIGHT_ORIGIN,
	miLIGHT_DIRECTION,
	miLIGHT_SPOT
};

enum miLight_area {
	miLIGHT_NONE = 0,
	miLIGHT_RECTANGLE,
	miLIGHT_DISC,
	miLIGHT_SPHERE,
	miLIGHT_CYLINDER,
	miLIGHT_OBJECT,
	miLIGHT_USER
};
						
						
enum miFunction_type {
	miFUNCTION_C,				
	miFUNCTION_PHEN,			
	miFUNCTION_C_REQ,			
	miFUNCTION_OUTFILE,			
	miFUNCTION_DATA,			
	miFUNCTION_PASS_SAVE,			
	miFUNCTION_PASS_PREP,			
	miFUNCTION_PASS_MERGE,			
	miFUNCTION_PASS_DELETE, 		
	miFUNCTION_HARDWARE			
};

typedef struct miFunction {
	miPointer	sparep1[7];
	enum miFunction_type type;		
	miUint		out_typemap;		
	miUint		out_interpmap;		
	miTag		function_decl;		
	miTag		next_function;		
	miTag		parameter_indirect;	
	miTag		interfacephen;		
	miBoolean	spare0; 		
	int		parameter_size; 	
	int		result_size;		
	int		ghost_offs;		
	miTag		pass_read;		
	miTag		pass_write;		
	miSint1 	pass_maxsamples;	
	miCBoolean	spare2[3];		
	miCBoolean	no_expl_params; 	
	miCBoolean	cloned; 		
	miCBoolean	spare3[5];		
	miUchar 	label;			
	char		parameters[8];		
						
} miFunction;
						
						
typedef struct miUserdata {
	miTag		data_decl;		
	miTag		next_data;		
	miUint		label;			
	int		parameter_size; 	
	short		one;			
	short		spare1; 		
	int		spare2; 		
	char		parameters[8];		
} miUserdata;
						




/*
 * Enumeration of all types of rays to be traced
 * DO NOT renumber the flags, or precompiled shaders may stop working!
 * volume shaders can find out from state->type whether this ray came
 * from an object or from a light source. Note that in 3.0, mi_trace_probe
 * used miRAY_NONE and hence ignored non-trace objects. Changed in 3.1.2.
 */


typedef enum miRay_type {
	miRAY_EYE,			
	miRAY_TRANSPARENT,		
	miRAY_REFLECT,			
	miRAY_REFRACT,			
	miRAY_LIGHT,			
	miRAY_SHADOW,			
	miRAY_ENVIRONMENT,		
	miRAY_NONE,			
	miPHOTON_ABSORB,		
	miPHOTON_LIGHT, 		
	miPHOTON_REFLECT_SPECULAR,	
	miPHOTON_REFLECT_GLOSSY,	
	miPHOTON_REFLECT_DIFFUSE,	
	miPHOTON_TRANSMIT_SPECULAR,	
	miPHOTON_TRANSMIT_GLOSSY,	
	miPHOTON_TRANSMIT_DIFFUSE,	
	miRAY_DISPLACE, 		
	miRAY_OUTPUT,			
	miPHOTON_SCATTER_VOLUME,	
	miPHOTON_TRANSPARENT,		
	miRAY_FINALGATHER,		
	miRAY_LM_VERTEX,		
	miRAY_LM_MESH,			
	miPHOTON_EMIT_GLOBILLUM,	
	miPHOTON_EMIT_CAUSTIC,		
	miRAY_PROBE,			
	miRAY_HULL,			
	miPHOTON_HULL,			
	miRAY_NO_TYPES
} miRay_type;



/*
 * Determine if a ray given by miRay_type is an eye or first generation
 * transparency ray (primary), or a reflection or refraction (secondary) ray.
 */

#define miRAY_PRIMARY(r)	((r) == miRAY_EYE)
#define miRAY_SECONDARY(r)	((r) >	miRAY_EYE &&\
				 (r) <	miPHOTON_ABSORB ||\
				 (r) == miRAY_FINALGATHER ||\
				 (r) == miRAY_HULL)

/*
 * Determine if ray is a photon
 */

#define miRAY_PHOTON(r) 	(((r) >= miPHOTON_ABSORB &&\
				 (r) <= miPHOTON_TRANSMIT_DIFFUSE) ||\
				 ((r) >= miPHOTON_SCATTER_VOLUME &&\
				  (r) <= miPHOTON_TRANSPARENT) ||\
				 ((r) >= miPHOTON_EMIT_GLOBILLUM &&\
				  (r) <= miPHOTON_EMIT_CAUSTIC) ||\
				  (r) == miPHOTON_HULL)

#define MAX_TEX 	64	
#define NO_DERIVS	5	

/*
 * Virtual frame buffers numbers.  Used exclusively by pass filters.
 * to access sample information that is not written to files.
 */

#define miVFB_FIRST		8	
#define miVFB_SAMPLE_TIME	8	
#define miVFB_SAMPLE_LIMITS	9	
#define miVFB_TOTAL		9

/*
 * Types of shaders
 */

typedef enum {
	miSHADER_LENS,		
	miSHADER_MATERIAL,	
	miSHADER_LIGHT, 	
	miSHADER_SHADOW,	
	miSHADER_ENVIRONMENT,	
	miSHADER_VOLUME,	
	miSHADER_TEXTURE,	
	miSHADER_PHOTON,	
	miSHADER_GEOMETRY,	
	miSHADER_DISPLACE,	
	miSHADER_PHOTON_EMITTER,
	miSHADER_OUTPUT,	
	miSHADER_OTHER, 	
	miSHADER_LIGHTMAP,	
	miSHADER_PHOTONVOL,	
	miSHADER_STATE,    	
        miSHADER_CONTOUR,       
	miSHADER_NO_TYPES
} miShader_type;



#if defined(__GNUC__) && (defined(__APPLE_CPP__)     ||  \
                          defined(__APPLE_CC__)      ||  \
                          defined(__MACOS_CLASSIC__))
#define MC_PACK __attribute__((packed))
#else
#define MC_PACK 
#endif

/*
 * State structure
 */

#define MI_STATE_VERSION 2

typedef struct miState {
					
	int		version;		/* version of state struct,
						 * and shader interface
						 * currently 2 */
	miTag		camera_inst;		
	struct miCamera *camera;		
	struct miOptions *options;		
	miLock		global_lock;		
	miUint		qmc_instance;		/* instance of low discrepacny
						 * vector associated to current
						 * ray tree */
#ifdef BIT64
	miUint		pad;
#endif
	struct miTs	*ts;			
						
	float		raster_x;		
	float		raster_y;		
					
	struct miState	*parent;		/* state of parent ray
						 * first eye ray/lens shader:
						 *   NULL
						 * subsequent eye rays/lens
						 * shaders: previous one
						 * reflection, refraction,
						 * dissolve rays: parent ray
						 * light rays: parent ray
						 * shadow rays: light ray */
	miRay_type	type;			
	miUchar 	qmc_component;		/* next component of current
						 * instance of low discrepancy
						 * vector to be used */
	miUint1 	scanline;		
	miCBoolean	inv_normal;		/* normals are inverted because
						 * ray hit the back side */
	char		face;			/* f)ront, b)ack, a)ll, from
						 * state->options for subrays*/
	void		*spare0;		
	void		*cache; 		/* cache holding shadow rays
						 * it's a shadow cache
						 * internal to RC */
	int		reflection_level;	/* reflection depth of the ray
						 * SHOULD BE SHORT
						 */
	int		refraction_level;	/* refraction depth of the ray
						 * SHOULD BE SHORT
						 */
	miVector	org;			/* ray origin
						 * for light, shadow rays the
						 * light position */
	miVector	dir;			/* ray direction
						 * for light, shadow ray
						 * it points
						 * towards the intersection */
	double		dist MC_PACK;		
	float		time;			
	miScalar	ior;			
	miScalar	ior_in; 		
	miTag		material;		/* material of the primitive.
						 * Set to miNULL_TAG if no hit.
						 */
	miTag		volume; 		/* volume shader to be applied
						 */
	miTag		environment;		/* environment shader to be
						 * applied if no hit */
					
	miTag		refraction_volume;	/* volume shader to apply
						 * to refraction ray */
	miUint		label;			
	miTag		instance;		
	miTag		light_instance; 	

	void		*pri;			/* box_info of hit box, shaders
						 * can check if  pri== NULL
						 */
	int		pri_idx;		/* index of hit primitive
						 * in box */
	float		bary[4];		/* barycentric coordinates
						 * of hit primitive
						 * triangles: 0 - 2,
						 * imp. patches: 0 - 3 */
	miVector	point;			
	miVector	normal; 		/* interpolated normal
						 * pointing to the side of the
						 * ray */
	miVector	normal_geom;		/* geometric normal of pri.
						 * pointing to the side of the
						 * ray */
	miScalar	dot_nd; 		/* dot of normal, dir
						 * for light rays,
						 * it's the dot_nd of the
						 * parent (intersection) normal
						 * and the light ray dir */
	double		shadow_tol MC_PACK;	/* the minimum distance to a
						 * plane perpendicular to the
						 * shading normal for which the
						 * triangle is completely on
						 * the negative side.  */
	miVector	*tex_list;		/* list of texture coordinates
						 */
	miVector	*bump_x_list;		/* list of bump map basis x
						 * (perturbation) vectors */
	miVector	*bump_y_list;		/* list of bump map basis y
						 * (perturbation) vectors */
	miVector	motion; 		
	miVector	*derivs;		/* list of surface derivative
						 * vectors containing dx/du,
						 * dx/dv, d^2x/du^2, d^2x/dv^2,
						 * d^2x/dudv or (0,0,0) */
					
						/* instance is the
						 * light instance, see above */
					
	miVector	tex;			/* texture coordinates for
						 * lookup */
					
	const struct miFunction *shader;	
					
	struct miState	*child; 		
	short		thread; 		
	miUshort	count;			
					
	void		*user;			
	int		user_size;		
} miState;



/*
 * state structure passed to old-style output shaders. It is used for
 * output shaders whose declaration does not specify a version number.
 * Do not use this. Output shaders with nonzero version numbers get
 * a miState, not a miOutstate, and work with versioning and init/exit.
 */

#define MI_OUTSTATE_VERSION 2

typedef struct {				
	int		version;		
	miTag		camera_inst;		
	struct miCamera *camera;		
	struct miOptions *options;		
	int		xres, yres;		
	miImg_image	*frame_rgba;		
	miImg_image	*frame_z;		
	miImg_image	*frame_n;		
	miImg_image	*frame_label;		
	miMatrix	camera_to_world;	
	miMatrix	world_to_camera;	
	miImg_image	*frame_m;		
} miOutstate;


/*
 * shader type
 */

typedef miBoolean (*miShader)(
		miColor 	*result,	
		miState 	*state, 	
		void		*args); 	

typedef miBoolean (*miShaderx)(
		miColor 	*result,	
		miState 	*state, 	
		void		*args,		
		void		*arg);		

typedef miBoolean (*miShader_init)(
		miState 	*state, 	
		void		*args,		
		miBoolean	*init_req);	

typedef miBoolean (*miShader_exit)(
		miState 	*state, 	
		void		*args); 	

typedef int (*miShader_version)(void);


typedef miBoolean (*miShader_state)(
		miState		*state,
		void		*args);

typedef void (*miContour_Store_Function)(
		void		*info_void,
		int		*info_size,
		miState		*state,
		miColor		*color);

typedef miBoolean (*miContour_Contrast_Function)(
		void		*info1,
		void		*info2,
		int		level,
		miState		*state,
		void		*paras);

struct miContour_endpoint;			

typedef miBoolean (*miContour_Shader)(
		struct miContour_endpoint *epoint,
		void		*info_near,
		void		*info_far,
		miState		*state,
		void		*paras);



typedef struct miRclm_triangle {
		miInteger	a;
		miInteger	b;
		miInteger	c;
		miGeoIndex	pri_idx;
} miRclm_triangle;

typedef struct miRclm_mesh_render {
		void			*pri;
		int			no_triangles;
		miRclm_triangle const	*triangles;
		void const		*vertex_data;
} miRclm_mesh_render;


/*
 * functions
 */

#define mi_eval( s, p ) (!(s)->shader->ghost_offs ? (void*)(p) : \
  (!*((miTag*)((char*)(p)+(s)->shader->ghost_offs)) ? (void*)(p) : \
  mi_phen_eval(s,(void*)(p))))

#define mi_eval_boolean(p)	((miBoolean *)mi_eval(state, (void *)(p)))
#define mi_eval_integer(p)	((miInteger *)mi_eval(state, (void *)(p)))
#define mi_eval_scalar(p)	((miScalar  *)mi_eval(state, (void *)(p)))
#define mi_eval_vector(p)	((miVector  *)mi_eval(state, (void *)(p)))
#define mi_eval_transform(p)	((miScalar  *)mi_eval(state, (void *)(p)))
#define mi_eval_color(p)	((miColor   *)mi_eval(state, (void *)(p)))
#define mi_eval_tag(p)		((miTag     *)mi_eval(state, (void *)(p)))

miBoolean mi_call_shader(
		miColor 	*result,	
		miShader_type	type,		
		miState 	*state, 	
		miTag		shader);	
miBoolean mi_call_shader_x(
		miColor 	*result,	
		miShader_type	type,		
		miState 	*state, 	
		miTag		shader, 	
		void		*arg);		
void mi_flush_cache(
		miState *state );		
miBoolean mi_call_material(			
		miColor 	*result,	
		miState 	*state);	
miBoolean mi_call_photon_material(		
		miColor 	*energy,	
		miState 	*state);	
miBoolean mi_call_shadow_material(		
		miColor 	*result,	
		miState 	*state);	
miBoolean mi_call_environment_material(		
		miColor 	*result,	
		miState 	*state);	
miBoolean mi_call_volume_material(		
		miColor 	*result,	
		miState 	*state);	
miBoolean mi_call_photonvol_material(		
		miColor 	*result,	
		miState 	*state);	
miBoolean mi_call_contour_material(		
		struct miContour_endpoint* result,  
		void		*info_near,     
		void            *info_far,      
		miState         *state);        
void *mi_phen_eval(				
		miState		*state, 	
		void		*parm );	
miBoolean mi_lookup_color_texture(
		miColor * const	color,		
		miState * const	state,
		const miTag	tex,		
		miVector *const	coord);		
miBoolean mi_lookup_scalar_texture(
		miScalar *const	scalar,		
		miState * const	state,
		const miTag	tex,		
		miVector *const	coord);		
miBoolean mi_lookup_vector_texture(
		miVector *const	rvector,	
		miState * const	state,
		const miTag	tex,		
		miVector *const	coord);		


/*
 * texture filtering
 */

typedef struct {
		miScalar	eccmax; 	
		miScalar	max_minor;	
		miScalar	circle_radius;	
		miBoolean	bilinear;	
		miScalar	spare[10];	
} miTexfilter;

miBoolean mi_lookup_filter_color_texture(
		miColor * const color,		
		miState * const state,
		const miTag	tex,		
		const miTexfilter *paras,	
		miMatrix	ST);		

miBoolean mi_texture_filter_project(
		miVector	p[3],		
		miVector	t[3],		
		miState *const	state,
		miScalar	disc_r, 	
		miUint		space); 	

miBoolean mi_texture_filter_transform(
		miMatrix	ST,		
		miVector	p[3],		
		miVector	t[3]);		


void	  mi_texture_info(
		const miTag	tag,		
		int * const	xresp,		
		int * const	yresp,		
		void ** const	paraspp);	
void	  mi_light_info(
		const miTag	tag,		
		miVector *const orgp,		
		miVector *const dirp,		
		void ** const	paraspp);	
int	  mi_global_lights_info(
		miTag		**tag); 	
void	**mi_shader_info(
		miState * const state); 	
int	  mi_instance_info(
		const miTag	inst_tag,	
		void ** const	paraspp,	
		void ** const	spare1, 	
		void ** const	spare2, 	
		void ** const	spare3);	

miBoolean mi_inclusive_lightlist(
		int		*n_lights,	
		miTag		**lights,	
		miState 	*state);	
miBoolean mi_exclusive_lightlist(
		int		*n_lights,	
		miTag		**lights,	
		miState 	*state);	

miBoolean mi_tri_vectors(
		miState 	*state,
		int		which,		
		int		ntex,		
		miVector	**a,		
		miVector	**b,
		miVector	**c);

miBoolean mi_texture_interpolate(
		miState		*state,		
		miUint		space,		
		miScalar	*result);	

miBoolean mi_raster_unit(
		miState 	*state, 	
		miVector	*x,		
		miVector	*y);		

miImg_image * mi_output_image_open(
		miState 	*state, 	
		miUint		idx);		

void mi_output_image_close(
		miState 	*state, 	
		miUint		idx);		


miInteger mi_volume_num_shaders(
		miState 	*state);	
miInteger mi_volume_cur_shader(
		miState 	*state);	
miColor *mi_volume_user_color(
		miState 	*state);	
miTag const *mi_volume_tags(
		miState 	*state);	
miTag const *mi_volume_instances(
		miState 	*state);	


miBoolean mi_trace_eye(
		miColor 	*result,	
		miState 	*state, 	
		miVector	*origin,	
		miVector	*direction);	
miBoolean mi_trace_transparent(
		miColor 	*result,	
		miState 	*state);	
miBoolean mi_trace_reflection(
		miColor 	*result,	
		miState 	*state, 	
		miVector	*direction);	
miBoolean mi_trace_refraction(
		miColor 	*result,	
		miState 	*state, 	
		miVector	*direction);	
miBoolean mi_trace_environment(
		miColor 	*result,	
		miState 	*state, 	
		miVector	*direction);	
miBoolean mi_trace_probe(
		miState 	*state, 	
		miVector const	*direction,	
		miVector const	*origin);	
miBoolean mi_trace_continue(
		miColor 	*result,	
		miState 	*state);	
miBoolean mi_sample_light(
		miColor * const result, 	
		miVector *const dir,		
		float	* const dot_nl, 	
		miState * const state,		
		const miTag	light_inst,	
		int * const	samples);	
miBoolean mi_trace_light(
		miColor 	*result,	
		miVector	*dir,		
		float		*dot_nl,	
		miState 	*state, 	
		miTag		light_inst);	
miBoolean mi_trace_shadow(
		miColor * const result, 	
		miState * const state); 	
miBoolean mi_trace_shadow_seg(
		miColor * const result, 	
		miState * const state); 	
miBoolean mi_continue_shadow_seg(
		miColor * const result, 	
		miState * const state); 	

/* set opacity color. mental ray will use the last value set. note
   that if opacity is not set, alpha is assumed to hold the opacity. */
void mi_opacity_set(
		miState 	*state, 	
		miColor const * const opacity); 

/* get opacity colour. return miBoolean is miTRUE
   if it was previously set, otherwise miFALSE. */
miBoolean mi_opacity_get(
		miState 	*state, 	
		miColor 	*opacity);	


void mi_reflection_dir(
		miVector	*dir,		
		miState 	*state);	
miBoolean mi_refraction_dir(
		miVector	*dir,		
		miState 	*state, 	
		miScalar	ior_in, 	
		miScalar	ior_out);	
void mi_reflection_dir_specular(	
		miVector	*dir,		
		miState 	*state);	
void mi_reflection_dir_glossy(
		miVector	*dir,		
		miState 	*state, 	
		miScalar	shiny); 	
void mi_reflection_dir_glossy_x(
		miVector	*dir,		
		miState 	*state, 	
		miScalar	shiny, 		
                const double	sample[2]);	
void mi_reflection_dir_anisglossy(
		miVector	*dir,		
		miState 	*state, 	
		miVector	*u,		
		miVector	*v,		
		miScalar	shiny_u,	
		miScalar	shiny_v);	
void mi_reflection_dir_anisglossy_x(
		miVector	*dir,		
		miState 	*state, 	
		const miVector	*u,		
		const miVector	*v,		
		miScalar	shiny_u,	
		miScalar	shiny_v,	
                const  double	sample[2]);	
void mi_reflection_dir_diffuse(
		miVector	*dir,		
		miState 	*state);	
void mi_reflection_dir_diffuse_x(
		miVector	*dir,		
		miState 	*state,		
                const  double	sample[2]);	
miBoolean mi_transmission_dir_specular( 
		miVector	*dir,		
		miState 	*state, 	
		miScalar	ior_in, 	
		miScalar	ior_out);	
miBoolean mi_transmission_dir_glossy(
		miVector	*dir,		
		miState 	*state, 	
		miScalar	ior_in, 	
		miScalar	ior_out,	
		miScalar	shiny); 	
miBoolean mi_transmission_dir_glossy_x(
		miVector	*dir,		
		miState 	*state, 	
		miScalar	ior_in, 	
		miScalar	ior_out,	
		miScalar	shiny, 		
                const  double	sample[2]);	
miBoolean mi_transmission_dir_anisglossy(
		miVector	*dir,		
		miState 	*state, 	
		miScalar	ior_in, 	
		miScalar	ior_out,	
		miVector	*u,		
		miVector	*v,		
		miScalar	shiny_u,	
		miScalar	shiny_v);	
miBoolean mi_transmission_dir_anisglossy_x(
		miVector	*dir,		
		miState 	*state, 	
		miScalar	ior_in, 	
		miScalar	ior_out,	
		const miVector	*u,		
		const miVector	*v,		
		miScalar	shiny_u,	
		miScalar	shiny_v,	
		const  double	sample[2]);	
void mi_transmission_dir_diffuse(
		miVector	*dir,		
		miState 	*state);	
void mi_transmission_dir_diffuse_x(
		miVector	*dir,		
		miState 	*state,		
		const  double	sample[2]);	
void mi_scattering_dir_diffuse(
		miVector	*dir,		
		miState 	*state);
void mi_scattering_dir_directional(
		miVector	*dir,		
		miState 	*state,
		miScalar	directionality);
miScalar mi_scattering_pathlength(
		miState		*state,
		miScalar	k);


miRay_type mi_choose_scatter_type(
		miState 	*state,
		miScalar	transp, 	
		miColor 	*diffuse,
		miColor 	*glossy,
		miColor 	*specular);
miRay_type mi_choose_simple_scatter_type(
		miState 	*state,
		miColor 	*rdiffuse,
		miColor 	*rspecular,
		miColor 	*tdiffuse,
		miColor 	*tspecular);
int mi_choose_lobe(
		miState 	*state,
		miScalar	r);		


miScalar mi_phong_specular(
		miScalar	spec_exp,	
		miState 	*state, 	
		miVector	*dir);		
miScalar mi_blong_specular(
		miScalar	spec_exp,	
		miState 	*state, 	
		miVector	*dir);		
miScalar mi_blinn_specular(
		miVector	*di,		
		miVector	*dr,		
		miVector	*n,		
		miScalar	roughness,	
		miScalar	ior);		
miBoolean mi_cooktorr_specular(
		miColor 	*result,	
		miVector	*di,		
		miVector	*dr,		
		miVector	*n,		
		miScalar	roughness,	
		miColor 	*ior);		
void mi_fresnel_specular(
		miScalar	*ns,
		miScalar	*ks,
		miScalar	spec_exp,	
		miState 	*state, 	
		miVector	*dir,		
		miScalar	ior_in, 	
		miScalar	ior_out);	
miScalar mi_ward_glossy(
		miVector	*di,		
		miVector	*dr,		
		miVector	*n,		
		miScalar	shiny); 	
miScalar mi_ward_anisglossy(
		miVector	*di,		
		miVector	*dr,		
		miVector	*n,		
		miVector	*u,		
		miVector	*v,		
		miScalar	shiny_u,	
		miScalar	shiny_v);	
miScalar mi_fresnel(
		miScalar	n1,
		miScalar	n2,
		miScalar	t1,
		miScalar	t2);
miScalar mi_fresnel_reflection(
		miState 	*state,
		miScalar	ior_in,
		miScalar	ior_out);
miScalar mi_schlick_scatter(
		miVector	*di,		
		miVector	*dr,		
		miScalar	directionality);


miBoolean mi_photon_reflection_specular(
		miColor 	*energy,	
		miState 	*state,
		miVector	*dir);
miBoolean mi_photon_reflection_glossy(
		miColor 	*energy,	
		miState 	*state,
		miVector	*dir);
miBoolean mi_photon_reflection_diffuse(
		miColor 	*energy,	
		miState 	*state,
		miVector	*dir);
miBoolean mi_photon_transmission_specular(
		miColor 	*energy,	
		miState 	*state,
		miVector	*dir);
miBoolean mi_photon_transmission_glossy(
		miColor 	*energy,	
		miState 	*state,
		miVector	*dir);
miBoolean mi_photon_transmission_diffuse(
		miColor 	*energy,	
		miState 	*state,
		miVector	*dir);
miBoolean mi_photon_transparent(
		miColor 	*energy,	
		miState 	*parent);
miBoolean mi_photon_light(
		miColor 	*energy,
		miState 	*state );
miBoolean mi_photon_volume_scattering(
		miColor 	*energy,	
		miState 	*state,
		miVector	*dir);

/*
 * user frame buffer access
 */

miBoolean mi_fb_put(
		miState 	*state, 	
		int		fb,		
		void		*data); 	
miBoolean mi_fb_get(
		miState 	*state, 	
		int		fb,		
		void		*data); 	

/*
 * contour output shader interface
 */

typedef struct miContour_endpoint {
		miVector	point;		
						
		miColor 	color;
		float		width;
		miVector	motion;
		miVector	normal;
		miTag		material;
		int		label;
} miContour_endpoint;


miBoolean mi_get_contour_line(
		miContour_endpoint *p1,
		miContour_endpoint *p2);

void mi_add_contour_lines(
		miContour_endpoint p1[],	
		miContour_endpoint p2[],	
		int		n);		


/*
 * parallel safe random number generator
 */

double mi_par_random(
		miState		*state);

/*
 * more functions, defined by LIB. They are here because they need miState.
 * All take the state, a result vector pointer, and an input vector pointer.
 */

void mi_point_to_world(
		miState *const, miVector *const, miVector *const);
void mi_point_to_camera(
		miState *const, miVector *const, miVector *const);
void mi_point_to_object(
		miState *const, miVector *const, miVector *const);
void mi_point_to_raster(
		miState *const, miVector *const, miVector *const);
void mi_point_to_light(
		miState *const, miVector *const, miVector *const);
void mi_point_from_world(
		miState *const, miVector *const, miVector *const);
void mi_point_from_camera(
		miState *const, miVector *const, miVector *const);
void mi_point_from_object(
		miState *const, miVector *const, miVector *const);
void mi_point_from_light(
		miState *const, miVector *const, miVector *const);
void mi_vector_to_world(
		miState *const, miVector *const, miVector *const);
void mi_vector_to_camera(
		miState *const, miVector *const, miVector *const);
void mi_vector_to_object(
		miState *const, miVector *const, miVector *const);
void mi_vector_to_light(
		miState *const, miVector *const, miVector *const);
void mi_vector_from_world(
		miState *const, miVector *const, miVector *const);
void mi_vector_from_camera(
		miState *const, miVector *const, miVector *const);
void mi_vector_from_object(
		miState *const, miVector *const, miVector *const);
void mi_vector_from_light(
		miState *const, miVector *const, miVector *const);
void mi_normal_to_world(
		miState *const, miVector *const, miVector *const);
void mi_normal_to_camera(
		miState *const, miVector *const, miVector *const);
void mi_normal_to_object(
		miState *const, miVector *const, miVector *const);
void mi_normal_to_light(
		miState *const, miVector *const, miVector *const);
void mi_normal_from_world(
		miState *const, miVector *const, miVector *const);
void mi_normal_from_camera(
		miState *const, miVector *const, miVector *const);
void mi_normal_from_object(
		miState *const, miVector *const, miVector *const);
void mi_normal_from_light(
		miState *const, miVector *const, miVector *const);

/*
 * geometry shaders
 */

miBoolean mi_geoshader_add_result(
		miTag		*result,	
		const miTag	item);		

typedef struct {				
		double		dist;		
		miVector	point;		
		miVector	normal_geom;	
		float		bary[4];	
		void		*pri;		
		int		pri_idx;	
		miTag		instance;	
} miIntersect;





miBoolean mi_store_volume_photon(
  miColor* energy, 
  miState* state);

miBoolean mi_store_photon(
  miColor* energy, 
  miState* state);

miBoolean mi_compute_irradiance(
  miColor* result, 
  miState* state);

miBoolean mi_compute_irradiance_backside(
  miColor* result, 
  miState* state);

miBoolean mi_compute_directional_irradiance(
  miColor* result, 
  miState* state,
  const miScalar r,
  const miScalar g1,
  const miScalar g2);

miBoolean mi_compute_volume_irradiance(
  miColor* result, 
  miState* state);

typedef struct miIrrad_options {
  int		size;			
  
  int		finalgather_rays;	
  miScalar	finalgather_maxradius;	
  miScalar	finalgather_minradius;	
  miCBoolean	finalgather_view;	
  miUchar	finalgather_filter;	
  miUchar	padding1[2];		

  
  int		globillum_accuracy;	
  miScalar	globillum_radius; 	

  
  
  int		caustic_accuracy;	
  miScalar	caustic_radius;		

  
} miIrrad_options;

/* 
 * Initialize values from the default options. Call this macro, when overwrite
 * defaults where necessary.
 */
#define miIRRAD_DEFAULT(irrad, state) {                                       \
  (irrad)->size                  = sizeof(miIrrad_options);                 \
  (irrad)->finalgather_rays      = (state)->options->finalgather_rays;      \
  (irrad)->finalgather_maxradius = (state)->options->finalgather_maxradius; \
  (irrad)->finalgather_minradius = (state)->options->finalgather_minradius; \
  (irrad)->finalgather_view      = (state)->options->finalgather_view;      \
  (irrad)->finalgather_filter    = (state)->options->finalgather_filter;    \
  (irrad)->padding1[0]           = 0;                                       \
  (irrad)->padding1[1]           = 0;                                       \
  (irrad)->globillum_accuracy    = (state)->options->globillum_accuracy;    \
  (irrad)->globillum_radius      = (state)->options->globillum_radius;      \
  (irrad)->caustic_accuracy      = (state)->options->caustic_accuracy;      \
  (irrad)->caustic_radius        = (state)->options->caustic_radius;        \
}

/*****************************************************************************
 * This function is a replacement for mi_compute_irradiance and 
 * mi_compute_irradiance_backside. If irrad_options is NULL, the result is 
 * 1/M_PI of mi_compute_irradiance(_backside), side depending on the face 
 * parameter. If it is not NULL, fields in the miIrrad_options overwrite the 
 * global values from miOptions structure.
 */
miBoolean mi_compute_avg_radiance(
  miColor			*result,
  miState			*state,
  miUchar			face,	
  struct miIrrad_options	*irrad_options);



typedef enum miFinalgather_store_mode {
  miFG_STORE_COMPUTE		= 1,	
  miFG_STORE_SET		= 2	
} miFinalgather_store_mode;

miBoolean mi_finalgather_store(
  miColor	*result,
  miState	*state,
  int 		mode);



						
miTag		mi_phen_clone			(struct miState *, miTag);
void		mi_phen_unclone			(miTag);
						
					

typedef enum miShader_state_op {
        miSHADERSTATE_STATE_INIT,	
        miSHADERSTATE_STATE_EXIT,	
        miSHADERSTATE_SAMPLE_INIT,	
	miSHADERSTATE_SAMPLE_EXIT	 
} miShader_state_op;

typedef struct miShader_state_arg {
	miShader_state_op	op;	
	miBoolean		shader_return;	
} miShader_state_arg;
					



miBoolean mi_sample(
	double 		*sample, 
	int 		*instance,
	struct miState 	*state, 
	const miUshort 	dimension, 
	miUint 		*n);



miScalar mi_lightprofile_value(
        const void*     vlprof,         
        miScalar        phi,            
        miScalar        costheta,       
        const miVector* pos,            
        miBoolean       rel);           

miScalar mi_lightprofile_sample(
        miState*        state,
        miTag           light_profile,
        miBoolean       rel);



						
struct miSpectrum;
typedef struct miSpectrum miSpectrum;
struct miSpectrum_iterator;
typedef struct miSpectrum_iterator miSpectrum_iterator;
struct miSpectrum_data;
						
						

const char* 		mi_spectrum_enabled	(void);

struct miSpectrum*	mi_spectrum_blueprint	(void);
struct miSpectrum*	mi_spectrum_create	(const struct 
							miSpectrum_data *sd);
struct miSpectrum*	mi_spectrum_create_inheap (const struct
							miSpectrum_data *sd);
struct miSpectrum*	mi_spectrum_clone	(const struct miSpectrum *s);

void			mi_spectrum_delete	(struct miSpectrum *s);
void			mi_spectrum_delete_fromheap (struct miSpectrum *s);
void			mi_spectrum_clear	(struct miSpectrum *s);

unsigned int 		mi_spectrum_size	(const struct miSpectrum *s);
void			mi_spectrum_assign	(struct miSpectrum *s,
						 const struct miSpectrum* in);
void			mi_spectrum_assign_data (struct miSpectrum *s,
						 const struct miSpectrum_data* sd);
void			mi_spectrum_copy	(struct miSpectrum *s,
						 const struct miSpectrum* in);
void			mi_spectrum_mul		(struct miSpectrum *s,
						 const struct miSpectrum* in);
void			mi_spectrum_add		(struct miSpectrum *s,
						 const struct miSpectrum* in);
void			mi_spectrum_scale	(struct miSpectrum *s,
						 miScalar scale);
void			mi_spectrum_scale_add	(struct miSpectrum  *s,
						 const struct miSpectrum* in,
						 miScalar   scale);
void			mi_spectrum_lerp	(struct miSpectrum *s,
						 const struct miSpectrum *in,
						 miScalar lerp);
void			mi_spectrum_color_add	(struct miSpectrum *s,
						 const miColor	   *c);	
miScalar    		mi_spectrum_ciexyz	(miColor	   *c,
						 const struct miSpectrum *s);

struct miSpectrum_iterator* mi_spectrum_iterator_create(
						struct miSpectrum* s,
						miScalar quality);
void			mi_spectrum_iterator_destroy(
					    struct miSpectrum_iterator* it);

struct miSpectrum*	mi_spectrum_iterator_begin(
					struct miSpectrum_iterator* it);
struct miSpectrum*	mi_spectrum_iterator_next(
					struct miSpectrum_iterator* it);
struct miSpectrum*	mi_spectrum_iterator_end(
					struct miSpectrum_iterator* it);

unsigned int		mi_spectrum_iterator_size(void);

miScalar		mi_spectrum_eval	(const struct miSpectrum* s,
						 miScalar   wavelen);
miScalar		mi_spectrum_value_set	(struct miSpectrum* s,
						 miScalar   wavelen,
						 miScalar   value);
miScalar		mi_spectrum_value_add	(struct miSpectrum* s,
						 miScalar   wavelen,
						 miScalar   value);
void			mi_spectrum_range_get	(const struct miSpectrum* s,
						 miScalar*  wavelen_min,
						 miScalar*  wavelen_max);
miScalar		mi_spectrum_linewidth	(const struct miSpectrum* s,
						 miScalar   wavelen);
miBoolean		mi_spectrum_check	(const struct miSpectrum* s);

						

#ifdef __cplusplus
}
#endif
