/******************************************************************************
 * Copyright 1986-2000 by mental images GmbH & Co.KG, Fasanenstr. 81, D-10623
 * Berlin, Germany. All rights reserved.
 ******************************************************************************
 *
 * Appendix A    : Scene File Grammar
 * Description   : Formal syntax of the mental images .mi scene 
 *                 description language in yacc format
 *                 
 *****************************************************************************/

%{
   typedef bool miBoolean;
   typedef double[16] miMatrix;
   typedef struct miVector { float x, y, z };
   typedef struct miGeoVector { double x,y,z };
   typedef miMatrix  miTransform;
%} 

%start start

%union {
        bool            boolean;
        char            *symbol;
        char            *string;
        struct          {
                int      len;
                miUchar *bytes;
        }                byte_string;
        int              integer;
        double           floating;
        float            floatoctet[8];
        miMatrix         matrix;
        miVector         vector;
        miGeoVector      geovector;
}

%token <symbol>         T_SYMBOL
%token <integer>        T_INTEGER
%token <floating>       T_FLOAT
%token <string>         T_STRING
%token <byte_string>    T_BYTE_STRING
%token <vector>         T_VECTOR

%token ACCELERATION ACCURACY ADAPTIVE ALL ALPHA ANGLE ANY APERTURE APPLY
%token APPROXIMATE ARRAY ASPECT
%token BACK BASIS BEZIER BLUE BOOLEAN_ BOTH BOX BSP BSPLINE BUFFER BUMP
%token CALL CAMERA CARDINAL CAUSTIC CLASSIFICATION CLIP CODE COLOR COLORCLIP
%token CONE CONIC CONNECT CONSTANT CONTOUR CONTRAST CONTROL CORNER CP
%token CREASE CURVATURE CURVE CUSP CYLINDER
%token D DART DATA DEBUG_ DECLARE DELAUNAY DELETE_ DENSITY DEPTH DERIVATIVE
%token DESATURATE DIAGNOSTIC DIRECTION DISC DISPLACE DISTANCE DITHER
%token ECHO EMITTER END ENVIRONMENT EVEN ENERGY EXPONENT
%token FACE FALSE_ FAN FIELD FILE_ FILTER FINALGATHER FOCAL FRAME FRONT
%token GAMMA GAUSS GEOMETRY GLOBILLUM GRADING GREEN GRID GROUP GUI
%token HIDE HOLE
%token IMP IMPLICIT INCREMENTAL INFINITY_ INHERITANCE INSTANCE INSTGROUP
%token INTEGER INTERFACE_ IRRADIANCE
%token JITTER
%token LANCZOS LENGTH LENS LIGHT LINK LOCAL
%token M MAPSTO MATERIAL MATRIX MAX_ MEMORY MERGE MI MIN_ MITCHELL MIXED MOTION
%token N NOCONTOUR NULL_
%token OBJECT ODD OFF OFFSET ON OPAQUE_ OPENGL OPTIONS ORIGIN OUTPUT
%token P PARALLEL_ PARAMETRIC PHENOMENON PHOTON PHOTONMAP PHOTONS PHOTONVOL
%token PREMULTIPLY PRIORITY
%token Q QUALITY
%token RATIONAL RAY RAYCL RAW REBUILD RECTANGLE RECURSIVE RED REGISTRY REGULAR
%token RENDER RESOLUTION RGB_ ROOT
%token SAMPLELOCK SAMPLES SCALAR SCANLINE SEGMENTS SET SHADOW SHADOWMAP SHUTTER
%token SIZE SOFTNESS SORT SPACE SPATIAL SPDL SPECIAL SPHERE SPREAD STORE STRING
%token STRIP STRUCT SUBDIVISION SHADER SMART SURFACE SYSTEM
%token T TAG TAGGED TASK TAYLOR TEXTURE TIME TRACE TRANSFORM TREE TRIANGLE TRIM
%token TRUE_
%token U V VALUE VECTOR VERBOSE VERSION VIEW VISIBLE VOLUME
%token W WHITE WIDTH WINDOW WORLD

%type <floating>                floating
%type <boolean>                 boolean
%type <string>                  symbol
%type <string>                  opt_symbol
%type <matrix>                  transform
%type <integer>                 tex_flags
%type <integer>                 tex_flag
%type <integer>                 tex_type
%type <string>                  inst_item
%type <integer>                 filter_type
%type <integer>                 c_filter_type
%type <string>                  mtl_or_label
%type <boolean>                 rational
%type <floating>                merge_option
%type <vector>                  vector
%type <geovector>               geovector
%type <boolean>                 opt_volume_flag
%type <boolean>                 opt_vector_flag
%type <integer>                 apply
%type <integer>                 apply_list
%type <floatoctet>              out_parms
%type <integer>                 colorclip_mode

%%

start           : command_list
                |
                        {
			}
                ;


/*-----------------------------------------------------------------------------
 * primitive types
 *---------------------------------------------------------------------------*/

boolean         : ON
                        { $$ = miTRUE; }
                | OFF
                        { $$ = miFALSE; }
                | TRUE_
                        { $$ = miTRUE; }
                | FALSE_
                        { $$ = miFALSE; }
                ;

floating        : T_FLOAT       { $$ = $1; }
                | T_INTEGER     { $$ = $1; }
                ;

vector          : floating floating floating
                        { $$.x = $1; $$.y = $2; $$.z = $3; }
                | T_VECTOR
                        { $$ = $1; }
                ;

geovector       : floating floating floating
                        { $$.x = $1; $$.y = $2; $$.z = $3; }
                | T_VECTOR
                        { $$.z = $1.z; $$.y = $1.y; $$.x = $1.x; }
                ;

transform       : TRANSFORM     floating floating floating floating
                                floating floating floating floating
                                floating floating floating floating
                                floating floating floating floating
                        { $$[0] = $2;  $$[1] = $3;  $$[2] = $4;  $$[3] = $5;
                          $$[4] = $6;  $$[5] = $7;  $$[6] = $8;  $$[7] = $9;
                          $$[8] = $10; $$[9] = $11; $$[10]= $12; $$[11]= $13;
                          $$[12]= $14; $$[13]= $15; $$[14]= $16; $$[15]= $17; }
                ;

symbol          : T_SYMBOL
                        { $$ = $1; }
                | T_STRING
                        { $$ = $1; }
                ;

opt_symbol      :
                        { $$ = 0; }
                | symbol
                        { $$ = $1; }
                ;

colorclip_mode  :  RGB_
                        { $$ = miIMG_COLORCLIP_RGB; }
                |  ALPHA
                        { $$ = miIMG_COLORCLIP_ALPHA; }
                |  RAW
                        { $$ = miIMG_COLORCLIP_RAW; }
                ;


/*-----------------------------------------------------------------------------
 * top-level commands
 *---------------------------------------------------------------------------*/

command_list    : command
                | command_list
                  command
                ;

command         : debug
                | set
                | call
                | version
                | incr_command
                | INCREMENTAL
                  incr_command
                | DELETE_ symbol
                | RENDER symbol symbol symbol
                | VERBOSE boolean
                | VERBOSE T_INTEGER
                | ECHO T_STRING
                | SYSTEM T_STRING
                | MEMORY T_INTEGER
                | CODE T_STRING
                | CODE
                  code_bytes_list
                | LINK T_STRING
                | DECLARE function_decl
                | DECLARE phenomenon_decl
                | DECLARE data_decl
                | REGISTRY symbol
                  reg_body END REGISTRY
                ;

reg_body        :
                | reg_item reg_body
                ;

reg_item        : VALUE symbol
                | LINK symbol
                | CODE symbol
                | MI symbol
                | SPDL symbol
                | ECHO symbol
                | SYSTEM symbol
                | symbol symbol
                ;

incr_command    : namespace
                | options
                | camera
                | texture
                | material
                | light
                | instance
                | instgroup
                | object
                | userdata
                | gui
                | SHADER symbol function_list
                ;

code_bytes_list : T_BYTE_STRING
                | code_bytes_list T_BYTE_STRING
                ;

set             : SET symbol
                | SET symbol symbol
                ;

call            : CALL function_list
                | CALL function_list ',' symbol symbol
                ;

debug           : DEBUG_ symbol opt_symbol
                ;

version         : VERSION T_STRING
                | MIN_ VERSION T_STRING
                | MAX_ VERSION T_STRING
                ;



/*****************************************************************************
 *************************    new mi2 features    ****************************
 *****************************************************************************/

/*-----------------------------------------------------------------------------
 * namespace
 *---------------------------------------------------------------------------*/
namespace       : NAMESPACE symbol
                  command
                  END NAMESPACE
                ;

/*-----------------------------------------------------------------------------
 * options
 *---------------------------------------------------------------------------*/

options         : OPTIONS symbol
                  option_list END OPTIONS
                ;

option_list     :
                | option_list option_item
                ;

option_item     : optview_item
                | data
                | ACCELERATION RAYCL
                | ACCELERATION BSP
                | ACCELERATION GRID
                | DIAGNOSTIC SAMPLES boolean
                | DIAGNOSTIC PHOTON OFF
                | DIAGNOSTIC PHOTON DENSITY floating
                | DIAGNOSTIC PHOTON IRRADIANCE floating
                | DIAGNOSTIC GRID OFF
                | DIAGNOSTIC GRID OBJECT floating
                | DIAGNOSTIC GRID WORLD floating
                | DIAGNOSTIC GRID CAMERA floating
                | SAMPLES T_INTEGER
                | SAMPLES T_INTEGER T_INTEGER
                | SHADOW SORT
                | SHADOW SEGMENTS
                ;

optview_item    : SHADOW OFF
                | SHADOW ON
                | TRACE boolean
                | SCANLINE boolean
                | SCANLINE OPENGL
                | LENS boolean
                | VOLUME boolean
                | GEOMETRY boolean
                | DISPLACE boolean
                | OUTPUT boolean
                | MERGE boolean
                | FILTER filter_type
                | FILTER filter_type floating
                | FILTER filter_type floating floating
                | FACE FRONT
                | FACE BACK
                | FACE BOTH
                | FIELD OFF
                | FIELD EVEN
                | FIELD ODD
                | SAMPLELOCK boolean
                | PHOTON TRACE DEPTH T_INTEGER
                | PHOTON TRACE DEPTH T_INTEGER T_INTEGER
                | PHOTON TRACE DEPTH T_INTEGER T_INTEGER T_INTEGER
                | TRACE DEPTH T_INTEGER
                | TRACE DEPTH T_INTEGER T_INTEGER
                | TRACE DEPTH T_INTEGER T_INTEGER T_INTEGER
                | CONTRAST floating floating floating
                | CONTRAST floating floating floating floating
                | TIME CONTRAST floating floating floating
                | TIME CONTRAST floating floating floating floating
                | CONTOUR STORE function
                | CONTOUR CONTRAST function
                | JITTER floating
                | SHUTTER floating
                | TASK SIZE T_INTEGER
                | RAYCL SUBDIVISION T_INTEGER T_INTEGER
                | RAYCL MEMORY T_INTEGER
                | BSP SIZE T_INTEGER
                | BSP DEPTH T_INTEGER
                | BSP MEMORY T_INTEGER
                | GRID SIZE floating
                | DESATURATE boolean
                | DITHER boolean
                | PREMULTIPLY boolean
                | COLORCLIP colorclip_mode
                | GAMMA floating
                | OBJECT SPACE
                | CAMERA SPACE
                | MIXED SPACE
                | WORLD SPACE
                | INHERITANCE symbol
                | SHADOWMAP MOTION boolean
                | SHADOWMAP REBUILD boolean
                | SHADOWMAP boolean
                | SHADOWMAP OPENGL
                | CAUSTIC boolean
                | CAUSTIC T_INTEGER
                | CAUSTIC ACCURACY T_INTEGER
                | CAUSTIC ACCURACY T_INTEGER floating
                | CAUSTIC FILTER c_filter_type
                | CAUSTIC FILTER c_filter_type floating
                | GLOBILLUM boolean
                | GLOBILLUM T_INTEGER
                | GLOBILLUM ACCURACY T_INTEGER
                | GLOBILLUM ACCURACY T_INTEGER floating
                | FINALGATHER boolean
                | FINALGATHER ACCURACY T_INTEGER
                | FINALGATHER ACCURACY T_INTEGER floating
                | FINALGATHER ACCURACY T_INTEGER floating floating
                | PHOTONVOL ACCURACY T_INTEGER
                | PHOTONVOL ACCURACY T_INTEGER floating
                | PHOTONMAP FILE_ T_STRING
                | PHOTONMAP REBUILD boolean
                | APPROXIMATE opt_displace
                | FRAME BUFFER T_INTEGER opt_symbol
                ;

filter_type     : BOX
                        { $$ = 'b'; }
                | TRIANGLE
                        { $$ = 't'; }
                | GAUSS
                        { $$ = 'g'; }
                | MITCHELL
                        { $$ = 'm'; }
                | LANCZOS
                        { $$ = 'l'; }
                ;

c_filter_type   : BOX
                        { $$ = 'b'; }
                | CONE
                        { $$ = 'c'; }
                | GAUSS
                        { $$ = 'g'; }
                ;

opt_displace    : s_approx_tech ALL
                | DISPLACE s_approx_tech ALL
                ;


/*-----------------------------------------------------------------------------
 * camera
 *---------------------------------------------------------------------------*/

camera          : CAMERA symbol
                  camera_list END CAMERA
                ;

camera_list     :
                | camera_list camera_item
                ;

camera_item     : camview_item
                | frame_number
                | FIELD T_INTEGER
                ;

camview_item    : OUTPUT
                | OUTPUT T_STRING T_STRING
                | OUTPUT T_STRING out_parms T_STRING
                | OUTPUT T_STRING T_STRING T_STRING
                | OUTPUT T_STRING T_STRING out_parms T_STRING
                | OUTPUT T_STRING function
                | VOLUME
                | VOLUME function_list
                | ENVIRONMENT
                | ENVIRONMENT function_list
                | LENS
                | LENS function_list
                | FOCAL floating
                | FOCAL INFINITY_
                | APERTURE floating
                | ASPECT floating
                | RESOLUTION T_INTEGER T_INTEGER
                | OFFSET floating floating
                | WINDOW T_INTEGER T_INTEGER T_INTEGER T_INTEGER
                | CLIP floating floating
                | data
                ;

out_parms       : QUALITY T_INTEGER
                        { $$[0] = (float) $2; }
                ;

frame_number    : FRAME T_INTEGER
                | FRAME T_INTEGER floating
                ;


/*-----------------------------------------------------------------------------
 * instance
 *---------------------------------------------------------------------------*/

instance        : INSTANCE symbol
                  inst_item inst_func
                  inst_flags
                  inst_params
                  END INSTANCE
                ;

inst_item       :
                        { $$ = 0; }
                | symbol
                        { $$ = $1; }
                ;

inst_func       :
                | GEOMETRY function_list
                ;

inst_flags      :
                | inst_flag inst_flags
                ;

inst_flag       : VISIBLE boolean
                | SHADOW boolean
                | TRACE boolean
                | CAUSTIC
                | CAUSTIC boolean
                | CAUSTIC T_INTEGER
                | GLOBILLUM
                | GLOBILLUM boolean
                | GLOBILLUM T_INTEGER
                | HIDE boolean
                | TRANSFORM
                | TRANSFORM function
                | transform
                | MOTION OFF
                | MOTION TRANSFORM
                | MOTION transform
                | MATERIAL
                  inst_mtl
                | TAG T_INTEGER
                | data
                ;

inst_params     :
                | '(' ')'
                | '('
                  parameter_seq comma_rparen
                ;

comma_rparen    : ')'
                | ',' ')'
                ;

inst_mtl        :
                | symbol
                | '['
                  inst_mtl_array ']'
                ;

inst_mtl_array  : symbol inst_mtl_next
                ;

inst_mtl_next   :
                | ','
                | ',' inst_mtl_array
                ;


/*-----------------------------------------------------------------------------
 * instgroup
 *---------------------------------------------------------------------------*/

instgroup       : INSTGROUP symbol
                  group_flags
                  group_list END INSTGROUP
                ;

group_flags     :
                | group_flag group_flags
                ;

group_flag      : MERGE floating
                | TAG T_INTEGER
                | data
                ;

group_list      :
                | symbol
                  group_list
                ;


/*****************************************************************************
 *************************    either    **************************************
 *****************************************************************************/

/*-----------------------------------------------------------------------------
 * function declaration
 *---------------------------------------------------------------------------*/

function_decl   : shret_type_nosh T_STRING parm_decl_list
                | SHADER shret_type T_STRING parm_decl_list
                  declare_req_seq END DECLARE
                ;

shret_type      : shret_type_nosh
                | SHADER
                ;

shret_type_nosh :
                | simple_type
                | STRUCT '{' shret_decl_seq '}'
                ;

shret_decl_seq  : shret_decl_seq ',' shret_decl
                | shret_decl
                ;

shret_decl      : simple_type symbol
                | SHADER symbol
                | DATA symbol
                ;

parm_decl_list  : '(' parm_decl_seq ')'
                | '(' parm_decl_seq ',' ')'
                | '(' ')'
                ;

parm_decl_seq   : parm_decl_seq ',' parm_decl
                | parm_decl
                ;

parm_decl       : simple_type symbol
                | SHADER symbol
                | DATA symbol
                | STRUCT symbol '{' parm_decl_seq '}'
                | ARRAY parm_decl
                ;

simple_type     : BOOLEAN_
                | INTEGER
                | SCALAR
                | STRING
                | COLOR
                | VECTOR
                | TRANSFORM
                | SCALAR TEXTURE
                | VECTOR TEXTURE
                | COLOR TEXTURE
                | LIGHT
                | MATERIAL
                | GEOMETRY
                ;

declare_req_seq :
                | declare_req declare_req_seq
                ;

declare_req     : gui
                | SCANLINE OFF
                | SCANLINE ON
                | TRACE OFF
                | TRACE ON
                | SHADOW OFF
                | SHADOW ON
                | SHADOW SORT
                | SHADOW SEGMENTS
                | FACE FRONT
                | FACE BACK
                | FACE BOTH
                | TEXTURE T_INTEGER
                | BUMP T_INTEGER
                | DERIVATIVE
                | DERIVATIVE T_INTEGER
                | DERIVATIVE T_INTEGER T_INTEGER
                | OBJECT SPACE
                | CAMERA SPACE
                | MIXED SPACE
                | WORLD SPACE
                | PARALLEL_
                | VERSION T_INTEGER
                | APPLY apply_list
                ;

apply_list      : apply                 { $$ = $1; }
                | apply ',' apply_list  { $$ = $1 | $3; }
                ;

apply           : LENS                  { $$ = miAPPLY_LENS;            }
                | MATERIAL              { $$ = miAPPLY_MATERIAL;        }
                | LIGHT                 { $$ = miAPPLY_LIGHT;           }
                | SHADOW                { $$ = miAPPLY_SHADOW;          }
                | ENVIRONMENT           { $$ = miAPPLY_ENVIRONMENT;     }
                | VOLUME                { $$ = miAPPLY_VOLUME;          }
                | TEXTURE               { $$ = miAPPLY_TEXTURE;         }
                | PHOTON                { $$ = miAPPLY_PHOTON;          }
                | GEOMETRY              { $$ = miAPPLY_GEOMETRY;        }
                | DISPLACE              { $$ = miAPPLY_DISPLACE;        }
                | EMITTER               { $$ = miAPPLY_PHOTON_EMITTER;  }
                | OUTPUT                { $$ = miAPPLY_OUTPUT;          }
                ;


/*-----------------------------------------------------------------------------
 * function instance
 *---------------------------------------------------------------------------*/

function_list   :
                  func_list
                ;

func_list       : function
                | func_list function
                ;

function        : T_STRING
                  parameter_list
                | '=' symbol
                | '=' opt_incremental SHADER symbol function
                ;

opt_incremental :
                  { }
                | INCREMENTAL
                ;

parameter_list  : '(' ')'
                | '(' parameter_seq ')'
                | '(' parameter_seq ',' ')'
                ;

parameter_seq   : parameter
                | parameter_seq ',' parameter
                ;

parameter       : symbol
                  value_list
                ;

value_list      : value
                | value_list value
                ;

value           : NULL_
                | boolean
                | T_INTEGER
                | T_FLOAT
                | symbol
                | '=' symbol
                | '=' INTERFACE_ symbol
                | '{'
                  parameter_seq '}'
                | '['
                  array_value_seq ']'
                | '[' ']'
                ;

array_value_seq : value_list
                  array_value_cont
                ;

array_value_cont:
                | ','
                  value_list array_value_cont
                ;


/*-----------------------------------------------------------------------------
 * user data
 *---------------------------------------------------------------------------*/

userdata        : DATA symbol data_label T_INTEGER
                  '[' data_bytes_list ']'
                | DATA symbol data_label symbol
                | DATA symbol data_label symbol '('
                  parameter_seq comma_rparen
                ;

data_label      :
                | TAG T_INTEGER

data_bytes_list :
                | tex_bytes_list T_BYTE_STRING
                ;

data_decl       : DATA T_STRING parm_decl_list
                  data_decl_req END DECLARE
                ;

data_decl_req   : gui
                | VERSION T_INTEGER
                | APPLY apply_list
                ;

data            : DATA symbol
                | DATA NULL_
                ;


/*-----------------------------------------------------------------------------
 * phenomenon declaration
 *---------------------------------------------------------------------------*/

phenomenon_decl : PHENOMENON shret_type T_STRING parm_decl_list
                  phen_body_list
                  END DECLARE
                ;

phen_body_list  :
                | phen_body phen_body_list
                ;

phen_body       : SHADER symbol function_list
                | material
                | light
                | instance
                | declare_req
                | ROOT phen_root
                | OUTPUT T_STRING T_STRING
                | OUTPUT T_STRING T_STRING T_STRING
                | OUTPUT T_STRING function
                | LENS function_list
                | VOLUME function_list
                | ENVIRONMENT function_list
                | GEOMETRY function_list
                | CONTOUR STORE function
                | CONTOUR CONTRAST function
                | OUTPUT PRIORITY T_INTEGER
                | LENS PRIORITY T_INTEGER
                | VOLUME PRIORITY T_INTEGER
                ;

phen_root       : MATERIAL symbol
                | LIGHT symbol
                | function_list
                ;


/*-----------------------------------------------------------------------------
 * texture
 *---------------------------------------------------------------------------*/

texture         : tex_flags tex_type TEXTURE symbol
                  tex_data
                ;

tex_flags       :
                        { $$ = 0; }
                | tex_flag tex_flags
                        { $$ = $1 | $2; }
                ;

tex_flag        : LOCAL
                        { $$ = 1; }
                | FILTER
                        { $$ = 2; }
                | FILTER floating
                        { $$ = 2; }
                ;

tex_type        : COLOR
                        { $$ = 0; }
                | SCALAR
                        { $$ = 1; }
                | VECTOR
                        { $$ = 2; }
                ;

tex_data        : '[' T_INTEGER T_INTEGER ']'
                  tex_bytes_list
                | '[' T_INTEGER T_INTEGER T_INTEGER ']'
                  tex_bytes_list
                | tex_func_list
                | T_STRING
                ;

tex_func_list   : function
                | function tex_func_list
                ;

tex_bytes_list  :
                | tex_bytes_list T_BYTE_STRING
                ;


/*-----------------------------------------------------------------------------
 * light
 *---------------------------------------------------------------------------*/

light           : LIGHT symbol
                  light_ops
                  END LIGHT
                ;

light_ops       :
                | light_op light_ops
                ;

light_op        : function
                | EMITTER function
                | SHADOWMAP
                | SHADOWMAP boolean
                | ORIGIN vector
                | DIRECTION vector
                | ENERGY floating floating floating
                | EXPONENT floating
                | CAUSTIC PHOTONS T_INTEGER
                | CAUSTIC PHOTONS T_INTEGER T_INTEGER
                | GLOBILLUM PHOTONS T_INTEGER
                | GLOBILLUM PHOTONS T_INTEGER T_INTEGER
                | RECTANGLE vector vector light_samples
                | DISC vector floating light_samples
                | SPHERE floating light_samples
                | CYLINDER vector floating light_samples
                | RECTANGLE
                | DISC
                | SPHERE
                | CYLINDER
                | SPREAD floating
                | SHADOWMAP RESOLUTION T_INTEGER
                | SHADOWMAP SOFTNESS floating
                | SHADOWMAP SAMPLES T_INTEGER
                | SHADOWMAP FILE_ T_STRING
                | VISIBLE
                | VISIBLE boolean
                | TAG T_INTEGER
                | data
                ;

light_samples   :
                | T_INTEGER T_INTEGER
                | T_INTEGER T_INTEGER T_INTEGER
                | T_INTEGER T_INTEGER T_INTEGER T_INTEGER T_INTEGER
                ;


/*-----------------------------------------------------------------------------
 * material
 *---------------------------------------------------------------------------*/

material        : MATERIAL symbol
                  mtl_flags
                  function_list
                  mtl_args
                  END MATERIAL
                ;

mtl_flags       :
                | mtl_flag mtl_flags
                ;

mtl_flag        : NOCONTOUR
                | OPAQUE_
                ;

mtl_args        :
                | mtl_arg mtl_args
                ;

mtl_arg         : DISPLACE
                | DISPLACE function_list
                | SHADOW
                | SHADOW function_list
                | VOLUME
                | VOLUME function_list
                | ENVIRONMENT
                | ENVIRONMENT function_list
                | CONTOUR
                | CONTOUR function_list
                | PHOTON
                | PHOTON function_list
                | PHOTONVOL
                | PHOTONVOL function_list
                ;


/*-----------------------------------------------------------------------------
 * object
 *---------------------------------------------------------------------------*/

object          : OBJECT opt_symbol
                  obj_flags
                  bases_and_groups
                  END OBJECT
                ;

obj_flags       :
                | obj_flag obj_flags
                ;

obj_flag        : VISIBLE
                | VISIBLE boolean
                | SHADOW
                | SHADOW boolean
                | TRACE
                | TRACE boolean
                | TAGGED
                | TAGGED boolean
                | CAUSTIC
                | CAUSTIC boolean
                | CAUSTIC T_INTEGER
                | GLOBILLUM
                | GLOBILLUM boolean
                | GLOBILLUM T_INTEGER
                | TAG T_INTEGER
                | data
                | transform
                | MAX_ DISPLACE floating
                ;

bases_and_groups:
                | basis bases_and_groups
                | group bases_and_groups
                ;

basis           : BASIS symbol rational MATRIX T_INTEGER T_INTEGER basis_matrix
                | BASIS symbol rational BEZIER T_INTEGER
                | BASIS symbol rational TAYLOR T_INTEGER
                | BASIS symbol rational BSPLINE T_INTEGER
                | BASIS symbol rational CARDINAL
                ;

rational        :
                        { $$ = miFALSE; }
                | RATIONAL
                        { $$ = miTRUE; }
                ;

basis_matrix    : 
                | basis_matrix floating
                ;

group           : GROUP opt_symbol merge_option
                  vector_list
                  vertex_list
                  geometry_list
                  END GROUP
                ;

merge_option    :
                        { $$ = 0.0; }
                | MERGE floating
                        { $$ = $2; }
                ;

vector_list     :
                | vector_list geovector
                ;

vertex_list     :
                | vertex_list vertex
                ;

vertex          : V T_INTEGER
                  n_vector
                  d_vector
                  t_vector_list
                  m_vector
                  u_vector_list
                  vertex_flag
                ;

n_vector        :
                | N T_INTEGER
                ;

d_vector        :
                | D T_INTEGER T_INTEGER
                | D T_INTEGER T_INTEGER T_INTEGER
                | D T_INTEGER T_INTEGER T_INTEGER T_INTEGER T_INTEGER
                ;

m_vector        :
                | M T_INTEGER
                ;

t_vector_list   :
                | t_vector_list T T_INTEGER
                | t_vector_list T T_INTEGER T_INTEGER T_INTEGER
                ;

u_vector_list   :
                | u_vector_list U T_INTEGER
                ;

vertex_flag     :
                | CUSP
                | CUSP floating
                | CONIC
                | CONIC floating
                | CORNER
                | DART
                ;


geometry_list   :
                | geometry_list geometry
                ;

geometry        : polygon
                | curve
                | spacecurve
                | surface
                | subdivsurf
                | connection
                | approximation
        /*      | implicit_patch        */
                ;


/* polygons */

polygon         : CP opt_symbol
                  poly_indices
                | P opt_symbol
                  poly_indices
                | STRIP opt_symbol
                  strip_indices
                | FAN opt_symbol
                  strip_indices
                ;

poly_indices    :
                | T_INTEGER
                  poly_indices
                | HOLE
                  poly_indices
                ;

strip_indices   :
                | T_INTEGER
                  strip_indices
                ;

h_vertex_ref_seq: h_vertex_ref
                | h_vertex_ref_seq h_vertex_ref
                ;

h_vertex_ref    : T_INTEGER
                | T_INTEGER T_FLOAT
                | T_INTEGER W floating
                ;


para_list       : T_FLOAT
                | para_list T_FLOAT
                ;


/* curves and space curves */

curve           : CURVE symbol rational symbol
                  para_list h_vertex_ref_seq curve_spec
                ;

curve_spec      :
                | SPECIAL curve_special_list
                ;

curve_special_list
                : curve_special
                | curve_special_list curve_special
                ;

curve_special   : T_INTEGER
                | T_INTEGER MAPSTO T_INTEGER
                ;

spacecurve      : SPACE CURVE symbol
                  spcurve_list
                ;

spcurve_list    : symbol floating floating
                | spcurve_list symbol floating floating
                ;


/* free-form surfaces */

surface         : SURFACE symbol mtl_or_label
                  rational symbol floating floating para_list
                  rational symbol floating floating para_list
                  h_vertex_ref_seq
                  tex_surf_list
                  surf_spec_list
                ;

mtl_or_label    : symbol
                        { $$ = $1; }
                | T_INTEGER
                        { $$ = (char *)(int*)$1; }
                ;

tex_surf_list   :
                | tex_surf_list tex_surf
                ;

tex_surf        : opt_volume_flag opt_vector_flag TEXTURE
                  rational symbol para_list
                  rational symbol para_list
                  h_vertex_ref_seq

                | DERIVATIVE
                | DERIVATIVE T_INTEGER
                | DERIVATIVE T_INTEGER T_INTEGER
                ;

opt_volume_flag :
                        { $$ = miFALSE; }
                | VOLUME
                        { $$ = miTRUE; }
                ;

opt_vector_flag :
                        { $$ = miFALSE; }
                | VECTOR
                        { $$ = miTRUE; }
                ;


surf_spec_list  :
                | surf_spec_list surf_spec
                ;

surf_spec       : TRIM trim_spec_list
                | HOLE hole_spec_list
                | SPECIAL
                  special_spec_list
                ;

trim_spec_list  : symbol floating floating
                | trim_spec_list symbol floating floating
                ;

hole_spec_list  : symbol floating floating
                | hole_spec_list symbol floating floating
                ;

special_spec_list
                : special_spec
                | special_spec_list special_spec
                ;

special_spec    : T_INTEGER
                | T_INTEGER MAPSTO T_INTEGER
                | symbol floating floating
                ;

connection      : CONNECT
                  symbol symbol floating floating
                  symbol symbol floating floating
                ;


/* subdivision surfaces */

subdivsurf      : SUBDIVISION SURFACE symbol
                  sds_base_polys END SUBDIVISION SURFACE
                ;

sds_base_polys  :
                | sds_base_poly sds_base_polys
                ;

sds_base_poly   : P opt_symbol
                  sds_indices sds_flags sds_subdiv_base
                | Q opt_symbol
                  sds_indices sds_flags sds_subdiv_base
                ;

sds_subdiv_base :
                | '{'
                  sds_materials sds_indices sds_flags sds_subdiv_subs '}'
                ;

sds_subdiv_subs :
                | '{'
                  sds_sub_polys '}'
                ;

sds_sub_polys   : sds_sub_poly
                | sds_sub_poly ',' sds_sub_polys
                ;

sds_sub_poly    :
                | sds_materials sds_indices sds_flags sds_subdiv_subs
                ;

sds_materials   :
                | symbol
                  sds_materials
                ;

sds_indices     : T_INTEGER
                | T_INTEGER
                  sds_indices
                ;

sds_flags       :
                | CREASE floating
                  sds_creaseflags
                ;

sds_creaseflags :
                | floating
                  sds_creaseflags;
                ;


/*-----------------------------------------------------------------------------
 * approximations (in objects and options)
 *---------------------------------------------------------------------------*/

approximation   : approx_body
                ;

approx_body     : APPROXIMATE SURFACE  s_approx_tech s_approx_names
                | APPROXIMATE SUBDIVISION SURFACE
                                       c_approx_tech sds_approx_names
                | APPROXIMATE DISPLACE s_approx_tech d_approx_names
                | APPROXIMATE CURVE    c_approx_tech c_approx_names
                | APPROXIMATE SPACE CURVE
                                       c_approx_tech spc_approx_names
                | APPROXIMATE TRIM     c_approx_tech t_approx_names
                | APPROXIMATE          s_approx_tech
                ;

s_approx_tech   : s_approx_params
                | s_approx_params T_INTEGER T_INTEGER
                | s_approx_params MAX_ T_INTEGER
                | s_approx_params T_INTEGER T_INTEGER MAX_ T_INTEGER
                ;

c_approx_tech   : c_approx_params
                | c_approx_params T_INTEGER T_INTEGER
                ;

s_approx_params : s_approx_param
                | s_approx_param s_approx_params
                ;

c_approx_params : c_approx_param
                | c_approx_param c_approx_params
                ;

s_approx_param  : x_approx_param
                | PARAMETRIC floating floating
                | REGULAR PARAMETRIC floating floating
                | IMPLICIT
                ;

c_approx_param  : x_approx_param
                | PARAMETRIC floating
                | REGULAR PARAMETRIC floating
                ;

x_approx_param  : VIEW
                | ANY
                | TREE
                | GRID
                | DELAUNAY
                | LENGTH floating
                | DISTANCE floating
                | ANGLE floating
                | SPATIAL approx_view floating
                | CURVATURE approx_view floating floating
                | GRADING floating
                ;

approx_view     :
                | VIEW
                ;

s_approx_names  : symbol
                | s_approx_names symbol
                ;

sds_approx_names: symbol
                | sds_approx_names symbol
                ;

d_approx_names  : symbol
                | s_approx_names symbol
                ;

c_approx_names  : symbol
                | c_approx_names symbol
                ;

spc_approx_names: symbol
                | c_approx_names symbol
                ;

t_approx_names  : symbol
                | t_approx_names symbol
                ;


/*-----------------------------------------------------------------------------
 * user-interface commands
 *---------------------------------------------------------------------------*/

gui             : GUI opt_symbol
                  '(' gui_attr_list ')' '{' gui_controls '}'
                | GUI opt_symbol
                  '{' gui_controls '}'
                ;

gui_elems       :
                | gui_elem gui_elems
                ;

gui_elem        : '(' gui_attr_list ')'
                | '{'
                  gui_controls '}'
                ;

gui_controls    :
                | gui_control gui_controls
                ;

gui_control     : CONTROL symbol opt_symbol
                  gui_elems
                ;

gui_attr_list   :
                | ','
                | gui_attr
                | gui_attr ',' gui_attr_list
                ;

gui_attr        : symbol
                | symbol boolean
                | symbol floating
                | symbol floating floating
                | symbol floating floating floating
                | symbol floating floating floating floating
                | symbol symbol
                ;
%%
