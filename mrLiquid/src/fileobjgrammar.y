/******************************************************************************
  Yacc .mi file grammar for parsing and searching for file object declarations,
  extended for maya.

  Note:
  The %token section has to match between fileobjgrammar.y, grammar.y, etc.
  as it allows the same lexer to be used for all parsing.
  
  Note2:
  Sadly, the .mi grammar provided by mental images uses tokens which are not
  prefixed by anything.  These tokens are usually spit as #define macros/enums
  with no prefixes which can sometimes conflict with C/C++ macros in std libs.
  So watch out for that when porting to other OSes. mental images usually gets
  away with that as their parsing and api is all done in C.
 *****************************************************************************/

%{

#include <cstdio>
#include <cstring>
#include <cassert>

#include "mentalFileObjectShape.h"
#include "mrIO.h"
#include "mrDebug.h"
   

#if defined(WIN32) || defined(WIN64)
#define strdup(x) _strdup(x)
#endif


   struct mrShaderParameter;
   
   mentalFileObjectShape* fileobj = NULL;
   
   static const int kMAX_NAMESPACES = 12;
   int   currNamespace = 0;
   char* namespaces[kMAX_NAMESPACES];

   extern "C" {
   // We will use the same lexer for all mi files, so undefine the macros
#define MRL_PREFIX mrl_yy
#undef yylex
#undef yyerror
#undef yyin
#define yylex   mrl_yylex
#define yyerror mrl_yyerror
#define yyin    mrl_yyin
/* #define yylex   MRL_PREFIX ## lex */
/* #define yyerror MRL_PREFIX ## error */
/* #define yylval  MRL_PREFIX ## lval */
/* #define yyin    MRL_PREFIX ## in */

   extern int   mi_open_file(const char *str);
   extern int   yylex();
   extern FILE* yyin;
   extern void  yyerror(const char* e);
   }

   //! Flag to determine if object command has incremental set to it or not
   bool incremental;

   
extern "C" {

%} 

%start start
 
%union {
        char            boolean;
        char            *symbol;
        char            *string;
        struct          {
                int      len;
                unsigned char *bytes;
        }                byte_string;
        int              integer;
        double           floating;
        float            vector[3];
        unsigned         nodeid;
        struct mrShaderParameter* para_type;
}

%token <symbol>         T_SYMBOL
%token <integer>        T_INTEGER
%token <floating>       T_FLOAT
%token <string>         T_STRING
%token <byte_string>    T_BYTE_STRING
%token <vector>         T_VECTOR
%token <nodeid>         T_NODEID

%token ACCELERATION ACCURACY ADAPTIVE ALL ALPHA ANGLE ANY APERTURE APPLY
%token APPROXIMATE ARRAY ASPECT AUTOVOLUME
%token B BACK BASIS BEZIER BIAS BLUE BOOLEAN_ BORDER BOTH BOX BSDF BSP BSP2
%token BSPLINE BUFFER BUMP
%token CALL CAMERA CARDINAL CAUSTIC CCMESH CG CHILD
%token CLASSIFICATION CLIP CODE COLOR COLORCLIP COLORPROFILE
%token COMPRESS CONE
%token CONIC CONNECT CONSTANT CONTOUR CONTRAST CONTROL CORNER CP CREASE
%token CURVATURE CURVE CUSP CYLINDER
%token D D2 DART DATA DEBUG_ DECLARE DEFAULT DEGREE DELAUNAY DELETE_ DENSITY
%token DEPTH DERIVATIVE DESATURATE DETAIL DIAGNOSTIC DIRECTION DISC DISPLACE
%token DISTANCE DITHER DOD DOF DPI
%token ECHO_ EMITTER END ENUM ENVIRONMENT EVEN ENERGY EXPONENT
%token FACE FALSE_ FAN FIELD FINE FILE_ FILTER FINALGATHER FOCAL FRAME FRONT
%token GAMMA GAUSS GEOMETRY GLOBILLUM GRADING GREEN GRID GROUP_ GUI
%token HAIR HARDWARE HERMITE HIDE HIDDEN HOLE
%token IES IMP IMPLICIT INCREMENTAL INFINITY_ INHERITANCE INSTANCE INSTGROUP
%token INTEGER INTERFACE_ INTERSECTION IRRADIANCE
%token JITTER
%token LANCZOS LARGE LENGTH LENS LEVEL LIGHT LIGHTMAP LIGHTPROFILE LINK LOCAL
%token LUMINANCE
%token M MAPSTO MATERIAL MATRIX MAX_ MEMORY MERGE MI MIN_ MITCHELL MIXED MOTION
%token N_ NAMESPACE NOCONTOUR NOSMOOTHING NULL_ NTSC
%token OBJECT ODD OFF OFFSET OFFSCREEN ON ONLY OPAQUE_ OPENGL OPTIONS ORIGIN
%token OUTPUT OVERRIDE
%token P PARALLEL_ PARAMETRIC PASS PHENOMENON PHOTON PHOTONMAP PHOTONS
%token PHOTONVOL POLYGON POSITION PREMULTIPLY PREP PRESAMPLE PRIORITY PRIVATE
%token PROTOCOL
%token QUALITY
%token PROPERTY
%token RADIUS RAPID RATIONAL RAY_ RAYCL RAW READ REBUILD RECTANGLE RECURSIVE
%token RED REFLECTION REFRACTION REGISTRY REGULAR
%token RENDER RESOLUTION RGB_ ROOT
%token SAMPLELOCK SAMPLES SCALE SCALAR SCANLINE SECONDARY SEGMENTS SELECT SESSION
%token SET SHADER SHADING SHADOW SHADOWMAP SHARP SHUTTER SIZE_ SOFTNESS SORT SPACE
%token SPATIAL SPDL SPECIAL SPECTRUM SPHERE SPREAD STATE STEPS STORE STRING
%token STRIP STRUCT 
%token SUBDIVISION SURFACE SYSTEM
%token T_ TAG TAGGED TASK TAYLOR TEXTURE TIME TRACE TRANSFORM TRAVERSAL
%token TOPOLOGY TOUCH
%token TRANSPARENCY TREE TRIANGLE TRILIST TRIM TRUE_
%token U UNIFORM USER
%token V VALUE VECTOR VENDOR VERBOSE VERSION VERTEX VIEW VISIBLE VOLUME
%token W WEIGHT WHITE WIDTH WINDOW WORLD WRITABLE WRITE


/* maya tokens */
%token T_MAYA_OPTION
%token SHORTNAME
%token NODEID_
%token SOFTMIN SOFTMAX
%token ICON
%token HELP
%token END_STR


%type <floating>                floating
%type <boolean>                 boolean
%type <string>                  symbol
%type <string>                  opt_symbol
%type <integer>                 tex_flags
%type <integer>                 tex_flag
%type <integer>                 tex_type
%type <string>                  inst_item
%type <integer>                 filter_type
%type <integer>                 c_filter_type
%type <string>                  mtl_or_label
%type <boolean>                 rational
%type <floating>                merge_option
%type <boolean>                 opt_volume_flag
%type <boolean>                 opt_vector_flag

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
                        { $$ = true; }
                | OFF
                        { $$ = false; }
                | TRUE_
                        { $$ = true; }
                | FALSE_
                        { $$ = false; }
                ;

floating        : T_FLOAT       { $$ = $1; }
                | T_INTEGER     { $$ = $1; }
                ;

vector          : floating floating floating
                | T_VECTOR
                ;

geovector       : floating floating floating
                | T_VECTOR
                ;

transform       : TRANSFORM     floating floating floating floating
                                floating floating floating floating
                                floating floating floating floating
                                floating floating floating floating
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
                |  ALPHA
                |  RAW
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
                | { incremental = false; }
                  incr_command
		| namespace
                | INCREMENTAL
                  { incremental = true; }
                  incr_command
                | DELETE_ symbol
                | RENDER symbol symbol symbol
                | VERBOSE boolean
                | VERBOSE T_INTEGER
		| ECHO_ T_STRING
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
                | ECHO_ symbol
                | SYSTEM symbol
                | symbol symbol
                ;

incr_command    : options
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
                | AUTOVOLUME boolean 
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
                | TASK SIZE_ T_INTEGER
                | RAYCL SUBDIVISION T_INTEGER T_INTEGER
                | RAYCL MEMORY T_INTEGER
                | BSP SIZE_ T_INTEGER
                | BSP DEPTH T_INTEGER
                | BSP MEMORY T_INTEGER
                | GRID SIZE_ floating
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
                | STATE symbol parameter_list
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
                ;

frame_number    : FRAME T_INTEGER
                | FRAME T_INTEGER floating
                ;


/*-----------------------------------------------------------------------------
 * instance
 *---------------------------------------------------------------------------*/

instance        : 
                  INSTANCE symbol
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
                | LIGHT incl_excl light_list
                | LIGHT SHADOW incl_excl light_list
                | MATERIAL
                  inst_mtl
                | TAG T_INTEGER
                | data
                | OVERRIDE
                | APPROXIMATE
                | APPROXIMATE
                  '[' inst_approx_arr ']'
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
                | '[' inst_mtl_array ']'
                ;

inst_mtl_array  : symbol inst_mtl_next
                ;

inst_mtl_next   :
                | ','
                | ',' inst_mtl_array
                ;

inst_approx_arr : approx_flags APPROXIMATE s_approx_tech ALL
                  inst_approx_nxt
                | approx_flags APPROXIMATE DISPLACE s_approx_tech ALL
                  inst_approx_nxt
                ;

inst_approx_nxt :
                | ','
                | ',' inst_approx_arr
                ;

incl_excl       :
                | T_STRING
                ; 
                
light_list      : '[' light_list_element ']'
                | '[' ']'
                ;
                
light_list_element : symbol
                   | symbol light_list_element
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
                | simple_type maya_gui_opt
                | STRUCT '{' shret_decl_seq '}'
                ;

shret_decl_seq  : shret_decl_seq ',' maya_gui_opt shret_decl maya_gui_opt
                | shret_decl maya_gui_opt
                ;

shret_decl      : simple_type symbol
                | SHADER symbol
                | DATA symbol
                ;

parm_decl_list  : '(' parm_decl_seq ')'
                | '(' parm_decl_seq ',' maya_gui_opt ')'
                | '(' ')'
                ;

parm_decl_seq   : parm_decl_seq ',' maya_gui_opt parm_decl maya_gui_opt
                | parm_decl maya_gui_opt
                ;

parm_decl       : simple_type symbol parm_defaults
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

apply_list      : apply                 
                | apply ',' apply_list 
                ;

apply           : LENS                  
                | MATERIAL              
                | LIGHT                 
                | SHADOW                
                | ENVIRONMENT           
                | VOLUME                
                | TEXTURE               
                | PHOTON                
                | GEOMETRY              
                | DISPLACE              
                | EMITTER               
                | OUTPUT                
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
                | WRITABLE 
                        { $$ = 1; }
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
 * namespace
 *---------------------------------------------------------------------------*/
namespace       : NAMESPACE symbol
                  {
		     assert( currNamespace < kMAX_NAMESPACES-1 );
		     if ( currNamespace > 0 )
		     {
			char fullpath[256];
			sprintf(fullpath, "%s::%s", 
				namespaces[currNamespace - 1], $2 );
			namespaces[ currNamespace++ ] = strdup( fullpath );
		     }
		     else
		     {
			namespaces[ currNamespace++ ] = strdup( $2 );
		     }
                  }
                  command_list
                  END NAMESPACE
                  {
		     free( namespaces[ --currNamespace ] );
                  }
                ;

/*-----------------------------------------------------------------------------
 * object
 *---------------------------------------------------------------------------*/

object          : OBJECT opt_symbol
                  {
		     if ( $2 != NULL )
		     {
			if ( strcmp( $2, fileobj->getName() ) == 0 )
			{
			   fileobj->setFound( incremental );
			}
			else if ( currNamespace )
			{
			   char fullname[1024];
			   sprintf( fullname, "%s:%s",
				    namespaces[currNamespace-1], $2 );
			   if ( strcmp( fullname, fileobj->getName() ) == 0 )
			   {
			      fileobj->setFound( incremental );
			   }
			}
		     }
                  }
                  obj_flags
		  object_body
                  END OBJECT
                  {
                  }
                ;

obj_flags       :
                | obj_flag obj_flags
                ;

obj_flag        : VISIBLE
                | VISIBLE boolean
                | SHADOW
                | SHADOW T_INTEGER
                | SHADOW boolean
                | TRANSPARENCY boolean
                | TRANSPARENCY T_INTEGER
                | REFLECTION   boolean
                | REFLECTION   T_INTEGER
                | REFRACTION   boolean
                | REFRACTION   T_INTEGER
                | FINALGATHER  boolean
                | FINALGATHER  T_INTEGER
                | SHADOWMAP    boolean
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
                | FACE FRONT
                | FACE BACK
                | FACE BOTH
                | TAG T_INTEGER
                | SAMPLES T_INTEGER T_INTEGER
                | SAMPLES T_INTEGER
                | RAY_ OFFSET floating
                | data
                | transform
                | MAX_ DISPLACE floating
                {
		   fileobj->setMaxDisplace( $3 );
                }
                | BOX  floating floating floating floating floating floating
                {
		   fileobj->setBBox( $2, $3, $4, $5, $6, $7 );
                }
                | MOTION BOX  floating floating floating floating floating floating
                {
		   fileobj->setMotionBox( $3, $4, $5, $6, $7, $8 );
                }
                ;

object_body     : bases_and_groups
                | FILE_ T_STRING
                | hair_object
                | trilist_object
                | isect_object
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
                  { }
                | RATIONAL
		  { $$ = 1; }
                ;

basis_matrix    : 
                | basis_matrix floating
                ;

group           : GROUP_ opt_symbol merge_option
		{
		     if ( fileobj->wasFound() )
		     {
		       fclose( yyin );
		       YYACCEPT;
		     }
		}
                  vector_list
                  vertex_list
                  geometry_list
                  END GROUP_
                ;

merge_option    :
                  { }
                | MERGE floating
		  { }
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
                | N_ T_INTEGER
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
                | t_vector_list T_ T_INTEGER
                | t_vector_list T_ T_INTEGER T_INTEGER T_INTEGER
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
                | ccmesh
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
                        { $$ = false; }
                | VOLUME
                        { $$ = true; }
                ;

opt_vector_flag :
                        { $$ = false; }
                | VECTOR
                        { $$ = true; }
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

subdivsurf      : SUBDIVISION SURFACE
                  sds_surf
                  sds_base_faces
                  derivatives
                  END SUBDIVISION SURFACE
                ;

sds_surf        : symbol sds_specs

sds_specs       :
                | sds_specs sds_spec
                ;

sds_spec        : POLYGON T_INTEGER T_INTEGER T_INTEGER T_INTEGER
                | TEXTURE SPACE
                  '[' texspace ']'
                ;

texspace        : FACE texspace_nxt
                | SUBDIVISION texspace_nxt
                ;

texspace_nxt    :
                | ','
                | ',' texspace
                ;

derivatives     :
                | derivatives derivative
                ;

derivative      : DERIVATIVE T_INTEGER
                | DERIVATIVE T_INTEGER SPACE T_INTEGER
                ;

sds_base_faces  :
                | sds_base_faces sds_base_face
                ;

sds_base_face   : P opt_symbol sds_indices
                  base_data
                ;

base_data       :
                | base_spec base_data
                ;

base_spec       : '{'
                  hira_data '}'
                | CREASE T_INTEGER sds_creaseflags
                | TRIM T_INTEGER
                ;

hira_data       :
                | hira_spec hira_data
                ;

hira_spec       : MATERIAL T_INTEGER symbol
                | MATERIAL T_INTEGER T_INTEGER
                | DETAIL T_INTEGER sds_indices
                | CREASE T_INTEGER T_INTEGER sds_creaseflags
                | TRIM T_INTEGER T_INTEGER
                | CHILD T_INTEGER '{'
                  hira_data '}'
                ;

sds_indices     :
                | T_INTEGER
                  sds_indices
                ;

sds_creaseflags :
                | floating
                  sds_creaseflags
                ;

/* ccmesh surfaces */

ccmesh          : CCMESH symbol
                  POLYGON T_INTEGER VERTEX T_INTEGER
                  ccmesh_polys
                  ccmesh_derivs
                  END CCMESH
                  ;

ccmesh_polys    : P symbol ccmesh_vertices
                  ccmesh_crease
                  ccmesh_sym
                | P
                  ccmesh_vertices
                  ccmesh_crease
                  ccmesh_nosym
                ;

ccmesh_sym      :
                | ccmesh_sym P opt_symbol ccmesh_vertices
                  ccmesh_crease
                ;

ccmesh_nosym    :
                | ccmesh_nosym P
                  ccmesh_vertices
                  ccmesh_crease
                ;

ccmesh_vertices :
                | ccmesh_vertices T_INTEGER
                ;

ccmesh_crease   :
                | CREASE ccmesh_cvalues
                ;

ccmesh_cvalues  :
                | ccmesh_cvalues floating
                ;

ccmesh_derivs   :
                | ccmesh_derivs ccmesh_deriv
                ;

ccmesh_deriv    : DERIVATIVE T_INTEGER
                | DERIVATIVE T_INTEGER SPACE T_INTEGER
                ;


/* hairs */

hair_object     : HAIR
                  hair_options
                  SCALAR '[' T_INTEGER ']'
                  hair_scalars
                  HAIR '[' T_INTEGER ']'
                  hair_hairs
                  END HAIR

hair_options    : hair_option hair_options
                |
                ;

hair_option     : MATERIAL symbol
                | RADIUS floating
                | APPROXIMATE T_INTEGER
                | DEGREE T_INTEGER
                | MAX_ SIZE_ T_INTEGER
                | MAX_ DEPTH T_INTEGER
                | HAIR N_
                | HAIR M T_INTEGER
                | HAIR T_ T_INTEGER
                | HAIR RADIUS
                | HAIR U T_INTEGER
                | VERTEX N_
                | VERTEX M T_INTEGER
                | VERTEX T_ T_INTEGER
                | VERTEX RADIUS
                | VERTEX U T_INTEGER
                ;

hair_scalars    :
                | hair_scalars floating
                ;

hair_hairs      :
                | hair_hairs T_INTEGER
                ;
 
/* trilists */

trilist_object  : TRILIST
                  VECTOR '[' T_INTEGER ']'
                  VERTEX '[' T_INTEGER ']' P
                        tl_specs
                  TRIANGLE '[' T_INTEGER ']'
                  '[' tl_vectors   ']'
                  '[' tl_vertices  ']'
                  '[' tl_triangles ']'
                  END TRILIST
                  tl_approx
                | TRILIST VERTEX T_INTEGER P            /* new in 3.4 */
                  pl_specs
                  TRIANGLE T_INTEGER pl_border pl_priminfo
                  pl_lines
                  pl_prims
                  pl_borderprims
                  pl_primdata
                  pl_topology
                  END TRILIST
                  tl_approx
                ;

/* new trilists 3.4 */

pl_topology     :
                | TOPOLOGY
                  pl_top
                ;

pl_top          : '[' pl_top_ascii ']'
                | INTEGER
                  T_VECTOR
                ;

pl_top_ascii    :
                | pl_top_ascii T_INTEGER
                ;

pl_border       :
                | BORDER T_INTEGER
                ;

pl_priminfo     :
                | DATA T_INTEGER T_INTEGER
                ;

pl_primdata     :
                | TRIANGLE DATA '[' pl_priminfodata_a ']'
                                '[' pl_primdata_a ']'
                | TRIANGLE DATA INTEGER
                    T_VECTOR INTEGER
                    T_VECTOR
                ;

pl_priminfodata_a:
                | pl_priminfodata_a T_INTEGER
                ;

pl_primdata_a   :
                | pl_primdata_a T_INTEGER
                ;

pl_lines        : SCALAR
                  T_VECTOR
                | '[' pl_ascii_lines ']'
                ;

pl_ascii_lines  :
                | pl_ascii_lines floating
                ;

pl_borderprims  :
                | pl_prims
                ;

pl_prims        : '[' symbol T_INTEGER T_INTEGER T_INTEGER
                  pl_tri_mtl
                  ']'
                | pl_tri_index
                ;

pl_tri_mtl      :
                | pl_tri_mtl opt_symbol T_INTEGER T_INTEGER T_INTEGER
                ;

pl_tri_index    : INTEGER
                  T_VECTOR
                  INTEGER
                  T_VECTOR
                | '[' pl_tri_ascindex ']'
                ;

pl_tri_ascindex :
                | pl_tri_ascindex T_INTEGER
                ;

pl_specs        :
                | pl_spec pl_specs
                ;

tex_dims        :
                | tex_dims T_INTEGER
                ;

us_dims         :
                | us_dims T_INTEGER
                ;

pl_spec         : N_
                | D
                | D2
                | M opt_size
                | B opt_size
                | T_ tex_dims
                | U
                  us_dims
                ;

/* trilists 3.3 */

tl_vectors      :
                | tl_vectors vector
                ;

tl_vertices     :
                | tl_vertices T_INTEGER
                ;

tl_triangles    : symbol T_INTEGER T_INTEGER T_INTEGER
                  tl_tri_mtl
                | tl_tri_index
                ;

tl_tri_mtl      :
                | tl_tri_mtl opt_symbol T_INTEGER T_INTEGER T_INTEGER
                ;

tl_tri_index    :
                | tl_tri_index T_INTEGER
                ;

tl_specs        :
                | tl_spec tl_specs
                ;

tl_spec         : N_
                | D
                | D2
                | M opt_size
                | T_ opt_size
                | B opt_size
                | U opt_size
                ;

opt_size        :
                | T_INTEGER
                ;

tl_approx       :
                | approximation
                ;

isect_object   : INTERSECTION function
               ;

/*-----------------------------------------------------------------------------
 * approximations (in objects and options)
 *---------------------------------------------------------------------------*/

approximation   : approx_flags approx_body
                ;

approx_flags    :
                | approx_flag approx_flags
                ;

approx_flag     : VISIBLE
                | TRACE
                | SHADOW
                | CAUSTIC
                | GLOBILLUM
                ;

approx_body     : APPROXIMATE SURFACE  s_approx_tech s_approx_names
                | APPROXIMATE SUBDIVISION SURFACE
                                       c_approx_tech sds_approx_names
                | APPROXIMATE CCMESH
                                       c_approx_tech ccm_approx_names
                | APPROXIMATE DISPLACE s_approx_tech d_approx_names
                | APPROXIMATE CURVE    c_approx_tech c_approx_names
                | APPROXIMATE SPACE CURVE
                                       c_approx_tech spc_approx_names
                | APPROXIMATE TRIM     c_approx_tech t_approx_names
                | APPROXIMATE          s_approx_tech
                | APPROXIMATE TRILIST s_approx_tech
                ;

s_approx_tech   : s_approx_params
                | s_approx_params T_INTEGER T_INTEGER
                | s_approx_params MAX_ T_INTEGER
                | s_approx_params T_INTEGER T_INTEGER MAX_ T_INTEGER
                | s_approx_params SAMPLES T_INTEGER
                | s_approx_params T_INTEGER T_INTEGER SAMPLES T_INTEGER
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
                | REGULAR PARAMETRIC floating '%' floating '%'
                | IMPLICIT
                ;

c_approx_param  : x_approx_param
                | PARAMETRIC floating
                | REGULAR PARAMETRIC floating
                | REGULAR PARAMETRIC floating '%'
                ;

x_approx_param  : VIEW
                | OFFSCREEN
                | ANY
                | TREE
                | GRID
                | DELAUNAY
                | FINE
                | FINE NOSMOOTHING
                | SHARP floating
                | LENGTH floating
                | DISTANCE floating
                | ANGLE floating
                | SPATIAL approx_view floating
                | CURVATURE approx_view floating floating
                | GRADING floating
                ;

approx_view     :
                | VIEW
                | OFFSCREEN
                ;

s_approx_names  : symbol
                | s_approx_names symbol
                ;

sds_approx_names: symbol
                | sds_approx_names symbol
                ;

ccm_approx_names: symbol
                | ccm_approx_names symbol
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

/*-----------------------------------------------------------------------------
 * maya interface commands
 *---------------------------------------------------------------------------*/

maya_gui_opt   :
               | maya_gui maya_gui_opt
               ;

parm_defaults :
                 { }
               | DEFAULT boolean
               | DEFAULT floating
               | DEFAULT floating floating floating
               | DEFAULT floating floating floating floating
               ;

maya_gui       : T_MAYA_OPTION maya_cmd_list
               ;

maya_cmd_list  :
               | maya_cmd maya_cmd_list
               ;

maya_cmd       : SHORTNAME T_STRING
               | HELP maya_help_seq END_STR
               | MIN_ floating
               | MIN_ floating floating floating
               | MAX_ floating
               | MAX_ floating floating floating
               | SOFTMIN floating
               | SOFTMIN floating floating floating
               | SOFTMAX floating
               | SOFTMAX floating floating floating
               | DEFAULT floating
               | DEFAULT floating floating floating
               | DEFAULT floating floating floating floating
               | ICON symbol
               | CLASSIFICATION symbol
               | NODEID_ T_NODEID
               ;
 
maya_help_seq  : maya_help maya_help_seq
               | maya_help 
               ;
 
maya_help      : T_STRING
               ;
%%

 } // extern "C"


bool findFileObjectInMiFile( mentalFileObjectShape* node,
			     const char* const mifile )
{
   fileobj = node;
   currNamespace = 0;

   try
   {
      if( mi_open_file( mifile ) )
      {
	 yyparse();
      }
      else
      {
	 MString err( node->getName() );
	 err += ": Could not open mi file \"";
	 err += mifile;
	 err += "\".";
	 LOG_ERROR(err);
	 return false;
      }

      while ( currNamespace )
      {
	 free( namespaces[--currNamespace] );
      }
   }
   catch(...)
   {
      LOG_ERROR("file object mi parser failed for unknown reason");
   }
   return true;
}
