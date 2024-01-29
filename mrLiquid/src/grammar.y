/*
  Yacc .mi file grammar for parsing shader declarations, extended for
  maya.
  Largely based on Appendix A of mental ray's manual.

  Note:
  The %token section has to match between fileobjgrammar.y, grammar.y, etc.
  as it allows the same lexer to be used for all parsing.
  
  Note2:
  Sadly, the .mi grammar provided by mental images uses tokens which are not
  prefixed by anything.  These tokens are usually spit as #define macros/enums
  with no prefixes which can sometimes conflict with C++ macros in std libs.
  So watch out for that when porting to other OSes.  mental images usually gets
  away with that as their parsing and api is all done in C.
 */
%{

#include <cstdio>
#include <cstring>
#include <cassert>

#include "mrShaderFactory.h"
#include "mrIO.h"
#include "mrDebug.h"

#if defined(WIN32) || defined(WIN64)
#define strdup(x) _strdup(x)
#endif
   
extern "C" {

#define MRL_PREFIX mrl_yy
#define yyfile mrl_yyfile
#define yytext mrl_yytext
#define yyline mrl_yyline
#define yyin   mrl_yyin

  extern char*    yyfile;
  extern unsigned yyline;
  extern char*    yytext;
  extern FILE*    yyin;

  extern int   yylex();
  extern int   mi_open_file(const char *str);
  extern int   mrl_yy_next_file();
   
   void yyerror(const char *str)
   {
      char err[1024];
      sprintf(err,"%s, for token \"%s\"", str, yytext );
      fprintf(stderr, "%s", err);
      LOG_ERROR(err);
      sprintf(err,"in file \"%s\", line %u\n", yyfile, yyline );
      fprintf(stderr, "%s", err);
      LOG_ERROR(err);
      mrl_yy_next_file();
   }
}

 
   
mrShaderParameter*          param  = NULL;
mrShaderDeclaration*        shader = NULL;

mrShaderParameter* linkParameter( mrShaderParameter* first,
				  mrShaderParameter* const second )
{
    mrShaderParameter* c = first;
    while ( c->next != NULL )
       c = c->next;
    assert( c != second );
    c->next = second;
    return c;
}
 
extern "C" {


%}

%start start


%union {
     char             boolean;
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

 
%type <floating>                boolean
%type <floating>                floating
%type <string>                  symbol
%type <string>                  opt_symbol
%type <integer>                 tex_flags
%type <integer>                 tex_flag
%type <integer>                 tex_type
%type <string>                  inst_item

%type <string>                  apply
%type <string>                  apply_list

%type <para_type>               simple_type
%type <para_type>               shret_type
%type <para_type>               shret_type_nosh
%type <para_type>               shret_decl_seq
%type <para_type>               shret_decl
%type <para_type>               parm_decl_list
%type <para_type>               parm_decl_seq
%type <para_type>               parm_decl


%%

start           :
                  command_list
                |
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


/*-----------------------------------------------------------------------------
 * top-level commands
 *---------------------------------------------------------------------------*/

command_list    : command
                | command_list
                  command
                ;

command         : version
                | set 
                | incr_command
		| namespace
                | INCREMENTAL
                  incr_command
                | VERBOSE boolean
                | VERBOSE T_INTEGER
                | ECHO_ T_STRING
                | LINK T_STRING
                | DECLARE function_decl
                | DECLARE phenomenon_decl
                | DECLARE data_decl
                | REGISTRY symbol
                  reg_body END REGISTRY
                ;

set             : SET symbol
                | SET symbol symbol
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

incr_command    : instance
                | userdata
                | texture
                | gui
                | SHADER symbol function_list
                ;

version         : VERSION T_STRING
                | MIN_ VERSION T_STRING
                | MAX_ VERSION T_STRING
                ;



/*-----------------------------------------------------------------------------
 * namespace
 *---------------------------------------------------------------------------*/
namespace       : NAMESPACE symbol
                  start
                  END NAMESPACE
                ;

/*****************************************************************************
 *************************    new mi2 features    ****************************
 *****************************************************************************/


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
                | '['
                  inst_mtl_array ']'
                ;

inst_mtl_array  : symbol
                  inst_mtl_next
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

/*****************************************************************************
 *************************    either    **************************************
 *****************************************************************************/

/*-----------------------------------------------------------------------------
 * function declaration
 *---------------------------------------------------------------------------*/

function_decl   : shret_type_nosh T_STRING parm_decl_list
                       {
			  assert( $1 != NULL );
			  assert( $2 != NULL );
			  assert( $1 != $3 );
			  shader = new mrShaderDeclaration($1, $2, $3);
			  param  = NULL;
			  assert( shader != NULL );
			  mrShaderFactory::registerShader( shader );
			  delete shader; shader = NULL;
		       }
                | SHADER shret_type T_STRING parm_decl_list
                       {
			  assert( $2 != NULL );
			  assert( $3 != NULL );
			  assert( $2 != $4 );
			  shader = new mrShaderDeclaration($2, $3, $4);
			  param  = NULL;
			  assert( shader != NULL );
		       }
                  declare_req_seq END DECLARE
                      {
			 mrShaderFactory::registerShader( shader );
			 delete shader; shader = NULL;
		      }
                ;

shret_type      : shret_type_nosh
                        {
			   $$ = $1;
			}
                | SHADER
                        {
			   param = new mrShaderShaderParameter("outShader");
			   param->setOutput();
			} 
		  maya_gui_opt
		        {
			   $$ = param;
			} 
                ;

shret_type_nosh :
                        {
			   param = new mrShaderColorParameter();
			   param->setOutput();
			   $$ = param;
			}
                | simple_type
		        {
			  param = $1; param->setOutput();
			}
                  maya_gui_opt
                | STRUCT '{' shret_decl_seq '}'
		  {
		     mrShaderStructParameter* s =
		            new mrShaderStructParameter("outStruct", $3);
		     param = s;
		     $$ = param;
		  }
                ;

shret_decl_seq  : shret_decl_seq ',' maya_gui_opt shret_decl
                        {
			  linkParameter( $1, $4 );
			}
                  maya_gui_opt 
                | shret_decl 
                  maya_gui_opt
                ;

shret_decl      : simple_type symbol
                  {
		     $$ = $1; $1->setName( $2 ); $1->setOutput();
		  } 
                | SHADER symbol
                  { $$ = param = new mrShaderShaderParameter($2);
		    param->setOutput();
		  } 
                | DATA symbol
                  { $$ = param = new mrShaderDataParameter($2);
		    param->setOutput();
		  } 
                ;

parm_decl_list  : '(' parm_decl_seq ')'
                        { $$ = $2; }
                | '(' parm_decl_seq ',' maya_gui_opt ')'
                        { $$ = $2; }
                | '(' ')'
		  { $$ = 0; }
                ;

parm_decl_seq   : parm_decl_seq ',' maya_gui_opt parm_decl 
                        {
			   linkParameter( $1, $4 );
			}
                  maya_gui_opt 
                | parm_decl maya_gui_opt 
                        {
			   $$ = $1;
			}
                ;

parm_decl       : simple_type symbol
                        {
			   param->setName( $2 ); 
			}
                  parm_defaults
                | SHADER symbol
		  {
		     $$ = param = new mrShaderShaderParameter($2);
		  }
                | DATA symbol 
		  { $$ = param = new mrShaderDataParameter($2); }
                | STRUCT symbol '{' parm_decl_seq '}'
		  {
		     mrShaderStructParameter* s =
		            new mrShaderStructParameter($2, $4);
		     $$ = param = s;
		  }
                | ARRAY parm_decl
		  {
		     mrShaderArrayParameter* a =
		                     new mrShaderArrayParameter($2);
		     $$ = param = a;
		  }
                ;

simple_type     : BOOLEAN_
                        { $$ = param = new mrShaderBooleanParameter(); }
                | INTEGER
                        { $$ = param = new mrShaderIntegerParameter(); }
                | SCALAR
		        { $$ = param = new mrShaderScalarParameter(); }
                | STRING
		        { $$ = param = new mrShaderStringParameter(); }
                | COLOR
        		{ $$ = param = new mrShaderColorParameter(); }
                | VECTOR
		        { $$ = param = new mrShaderVectorParameter(); }
                | TRANSFORM
                        { $$ = param = new mrShaderTransformParameter(); }
                | SCALAR TEXTURE
		        { $$ = param = new mrShaderScalarTextureParameter(); }
                | VECTOR TEXTURE
                        { $$ = param = new mrShaderVectorTextureParameter(); }
                | COLOR TEXTURE
                        { $$ = param = new mrShaderColorTextureParameter(); }
                | LIGHT
                  	{ $$ = param = new mrShaderLightParameter(); }
                | MATERIAL
                        { $$ = param = new mrShaderMaterialParameter(); }
                | GEOMETRY
                        { $$ = param = new mrShaderGeometryParameter(); }
                | LIGHTPROFILE
                        { $$ = param = new mrShaderLightProfileParameter(); }
                ;

declare_req_seq :
                | declare_req declare_req_seq
                ;

declare_req     : gui
                | maya_gui
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
                      { shader->type = $2; }
                ;

apply_list      : apply                 { $$ = $1; }
                | apply ',' apply_list  { $$ = $1; }
                ;

apply           : LENS                  { $$ = "lens";            }
                | MATERIAL              { $$ = "material";         }
                | LIGHT                 { $$ = "light";           }
                | SHADOW                { $$ = "shadow";          }
                | ENVIRONMENT           { $$ = "environment";     }
                | VOLUME                { $$ = "volume";          }
                | TEXTURE               { $$ = "texture";         }
                | PHOTON                { $$ = "photon";          }
                | GEOMETRY              { $$ = "geometry";        }
                | DISPLACE              { $$ = "displace";        }
                | EMITTER               { $$ = "photonemitter";   }
                | OUTPUT                { $$ = "output";          }
                | STATE                 { $$ = "state";           }
                | PHOTONVOL             { $$ = "photonvol";       }
                | LIGHTMAP              { $$ = "lightmap";        }
		| CONTOUR               { $$ = "contour";         }
                ;


/*-----------------------------------------------------------------------------
 * function instance (we need to keep for phenomena parsing)
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

data_decl_req   : 
		| gui
                | VERSION T_INTEGER
                | APPLY apply_list
                ;

data            : DATA symbol
                | DATA NULL_
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
                | LIGHTMAP
                | LIGHTMAP function_list
                ;


/*-----------------------------------------------------------------------------
 * phenomenon declaration
 *---------------------------------------------------------------------------*/

phenomenon_decl : PHENOMENON shret_type T_STRING parm_decl_list
                  {
		     assert( $2 != $4 );
		     shader = new mrShaderDeclaration($2, $3, $4, true);
		     assert( shader != NULL );
		     param = NULL;
		  }
		  phen_body_list
                  END DECLARE
		  {
		     mrShaderFactory::registerShader( shader );
		     delete shader; shader = NULL;
		  }
                ;

phen_body_list  :
                | phen_body phen_body_list
                ;

phen_body       : SHADER symbol function_list
                | instance
                | declare_req
                | ROOT phen_root
                | material
                | OUTPUT T_STRING T_STRING
                | OUTPUT T_STRING T_STRING T_STRING
                | OUTPUT T_STRING function
                | OUTPUT function_list
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
 * approximations (in objects and options)
 *---------------------------------------------------------------------------*/

approx_flags    :
                | approx_flag approx_flags
                ;

approx_flag     : VISIBLE
                | TRACE
                | SHADOW
                | CAUSTIC
                | GLOBILLUM
                ;

s_approx_tech   : s_approx_params
                | s_approx_params T_INTEGER T_INTEGER
                | s_approx_params MAX_ T_INTEGER
                | s_approx_params T_INTEGER T_INTEGER MAX_ T_INTEGER
                | s_approx_params SAMPLES T_INTEGER
                | s_approx_params T_INTEGER T_INTEGER SAMPLES T_INTEGER
                ;

s_approx_params : s_approx_param
                | s_approx_param s_approx_params
                ;

s_approx_param  : x_approx_param
                | PARAMETRIC floating floating
                | REGULAR PARAMETRIC floating floating
                | REGULAR PARAMETRIC floating '%' floating '%'
                | IMPLICIT
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

/*-----------------------------------------------------------------------------
 * user-interface commands (3dmax)
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
                 {
		    param->setDefault($2);
		 } 
               | DEFAULT floating
                 {
		    param->setDefault($2);
		 }
               | DEFAULT floating floating floating
                 {
		    param->setDefault($2,$3,$4);
		 }
               | DEFAULT floating floating floating floating
                 {
		    param->setDefault($2,$3,$4,$5);
		 }
               | DEFAULT floating floating floating floating 
		         floating floating floating floating
		         floating floating floating floating
                         floating floating floating floating
                 {
		    param->setDefault(  $2,  $3, $4, $5,
				        $6,  $7, $8, $9, 
				       $10, $11, $12, $13,
				       $14, $15, $16, $17 );
		 }
               ;

maya_gui       : T_MAYA_OPTION maya_cmd_list
               ;

maya_cmd_list  :
               | maya_cmd maya_cmd_list
               ;

maya_cmd       : SHORTNAME symbol
		 {  param->setShortName($2); }
               | HELP maya_help_seq END_STR
               | MIN_ floating
                  { param->setMin($2); }
               | MIN_ floating floating floating
                  { param->setMin($2,$3,$4); }
               | MAX_ floating
                  { param->setMax($2); }
               | MAX_ floating floating floating
                  { param->setMax($2,$3,$4); }
               | SOFTMIN floating
                  { param->setSoftMin($2); }
               | SOFTMIN floating floating floating
                  { param->setSoftMin($2,$3,$4); }
               | SOFTMAX floating
                  { param->setSoftMax($2); }
               | SOFTMAX floating floating floating
                  { param->setSoftMax($2,$3,$4); }
               | DEFAULT boolean
                  { param->setDefault($2); }
               | DEFAULT floating
                  { param->setDefault($2); }
               | DEFAULT floating floating floating
                  { param->setDefault($2,$3,$4); }
               | DEFAULT floating floating floating floating
                  { param->setDefault($2,$3,$4,$5); }
               | DEFAULT floating floating floating floating 
		         floating floating floating floating
		         floating floating floating floating
                         floating floating floating floating
                 {
		    param->setDefault(  $2,  $3, $4, $5,
				        $6,  $7, $8, $9, 
				       $10, $11, $12, $13,
				       $14, $15, $16, $17 );
		 }
               | ICON symbol
	          { shader->setIcon($2); }
               | CLASSIFICATION symbol
	          { shader->setClassification($2); }
               | ENUM T_STRING
	          { param->setEnum($2); }
               | NODEID_ T_NODEID
                  { shader->nodeid = $2; }
               | HIDDEN boolean
		 { param->setHidden( bool($2) ); } 
               | HIDDEN
		 { param->setHidden(true); } 
	       | PROPERTY symbol
		 { }
               ;

maya_help_seq  : maya_help maya_help_seq
               | maya_help 
               ;
 
maya_help      : T_STRING
	          {
		     if (param == NULL)
			shader->setHelp($1);
		     else
			param->setHelp($1);
	          }
               ;
 
%%
		 } // extern "C"


   void parseMiGrammar( const char* mifile )
   {
      MString msg("Parsing mi file: \"");
      msg += mifile;
      msg += "\"";
      LOG_MESSAGE( msg );


      try
      {
	 if( mi_open_file( mifile ) )
	    yyparse();
      }
      catch(...)
      {
	 LOG_ERROR("mi file parser failed for unknown reason");
	 delete shader; delete param;
      }

      shader = NULL; param = NULL;
   }

