/*
  GNU's Flex scanner for a mi file shader declarations
  
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
//   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

 */

%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

struct mrShaderParameter;
#include "fileobjgrammar.hpp"

#define MRL_PREFIX mrl_yy

#define yylval  mrl_yylval
#define yyfile  mrl_yyfile
#define yyline  mrl_yyline
#define yyerror mrl_yyerror

/* #define yylval  MRL_PREFIX ## lval */
/* #define yyfile  MRL_PREFIX ## file */
/* #define yyline  MRL_PREFIX ## line */
/* #define yyerror MRL_PREFIX ## error */
/* #define MRL_PREFIX_new_file  MRL_PREFIX ## new_file */
/* #define MRL_PREFIX_next_file MRL_PREFIX ## next_file */

extern YYSTYPE yylval;


#if defined(WIN32) || defined(WIN64)
#define strdup(x) _strdup(x)
#endif


struct INCLUDE_STACK
{
     YY_BUFFER_STATE buf;
     char* file;
     unsigned line;
};


#define MAX_INCLUDE_DEPTH 11
 struct INCLUDE_STACK include_stack[MAX_INCLUDE_DEPTH];
 static int include_stack_ptr = -1;
 static int string_caller;

 char*    yyfile = NULL;
 unsigned yyline = 1;
 
#define YYERROR_VERBOSE 1
#ifdef DEBUG
#  define YY_DEBUG 0
#else
#  define YY_DEBUG 0
#endif


 unsigned reserve_strings = 1024;
 unsigned max_strings = 0;
 unsigned num_strings = 0;
 char** string_elem = NULL;
 
 
 int mrl_yy_new_file( const char* text, int len );
 int mrl_yy_next_file();
 extern void yyerror(const char *str);


#if YY_DEBUG
#  define DBG(fmt,var) fprintf(stderr, fmt, var);
#  if YY_DEBUG > 1
#    define DBG_TOK() \
 fprintf(stderr,"grammar.lex: Matched rule: %d, line %d\n", __LINE__, yyline);
#    define DBG_TOKEN(str) \
  fprintf(stderr,"grammar.lex: Matched rule: %s %d, line %d\n", str, __LINE__, yyline);
#else
#  define DBG_TOK()
#  define DBG_TOKEN(str)
#endif
#else
#  define DBG(fmt,var)
#  define DBG_TOK()
#  define DBG_TOKEN(str)
#endif
 
 void free_all_strings();
 char* new_string( const char* s );
 
int yywrap()
{
   return 1;
} 
#define YY_SKIP_YYWRAP

%}

DIGIT      [0-9]                 
WORD       [A-Za-z0-9_]+
string     \"[^\n\"]+\"

%s NODEID INCLUDE LOOKUP 
%s STR MULTILINESTR BINVEC BYTESTRING MAYA HELPMSG

%option never-interactive

%%


<INITIAL>^\$ifdef[ \t]+.*\n { DBG_TOKEN("$ifdef"); ++yyline; }
<INITIAL>^\$ifndef[ \t]+.*\n { DBG_TOKEN("$ifndef"); ++yyline; }
<INITIAL>^\$endif[ \t]*.* { DBG_TOKEN("$endif"); ++yyline; }

<INITIAL>"transparency"	{ DBG_TOKEN("transparency"); return TRANSPARENCY; }
<INITIAL>"lightprofile"	{ DBG_TOKEN("lightprofile"); return LIGHTPROFILE; }
<INITIAL>"intersection" { DBG_TOKEN("intersection"); return INTERSECTION; }
<INITIAL>"colorprofile" { DBG_TOKEN("colorprofile"); return COLORPROFILE; }
<INITIAL>"acceleration"	{ DBG_TOKEN("acceleration"); return ACCELERATION; }
<INITIAL>"subdivision"	{ DBG_TOKEN("subdivision"); return SUBDIVISION; }
<INITIAL>"premultiply"	{ DBG_TOKEN("premultiply"); return PREMULTIPLY; }
<INITIAL>"nosmoothing"	{ DBG_TOKEN("nosmoothing"); return NOSMOOTHING; }
<INITIAL>"inheritance"	{ DBG_TOKEN("inheritance"); return INHERITANCE; }
<INITIAL>"incremental"	{ DBG_TOKEN("incremental"); return INCREMENTAL; }
<INITIAL>"finalgather"	{ DBG_TOKEN("finalgather"); return FINALGATHER; }
<INITIAL>"environment"	{ DBG_TOKEN("environment"); return ENVIRONMENT; }
<INITIAL>"approximate"	{ DBG_TOKEN("approximate"); return APPROXIMATE; }
<INITIAL>"samplelock"	{ DBG_TOKEN("samplelock"); return SAMPLELOCK; }
<INITIAL>"resolution"	{ DBG_TOKEN("resolution"); return RESOLUTION; }
<INITIAL>"reflection"	{ DBG_TOKEN("reflection"); return REFLECTION; }
<INITIAL>"refraction"	{ DBG_TOKEN("refraction"); return REFRACTION; }
<INITIAL>"phenomenon"	{ DBG_TOKEN("phenomenon"); return PHENOMENON; }
<INITIAL>"parametric"	{ DBG_TOKEN("parametric"); return PARAMETRIC; }
<INITIAL>"irradiance"	{ DBG_TOKEN("irradiance"); return IRRADIANCE; }
<INITIAL>"diagnostic"	{ DBG_TOKEN("diagnostic"); return DIAGNOSTIC; }
<INITIAL>"desaturate"	{ DBG_TOKEN("desaturate"); return DESATURATE; }
<INITIAL>"derivative"	{ DBG_TOKEN("derivative"); return DERIVATIVE; }
<INITIAL>"autovolume"   { DBG_TOKEN("autovolume"); return AUTOVOLUME; } 
<INITIAL>"transform"	{ DBG_TOKEN("transform"); return TRANSFORM; }
<INITIAL>"shadowmap"	{ DBG_TOKEN("shadowmap"); return SHADOWMAP; }
<INITIAL>"recursive"	{ DBG_TOKEN("recursive"); return RECURSIVE; }
<INITIAL>"rectangle"	{ DBG_TOKEN("rectangle"); return RECTANGLE; }
<INITIAL>"photonvol"	{ DBG_TOKEN("photonvol"); return PHOTONVOL; }
<INITIAL>"photonmap"	{ DBG_TOKEN("photonmap"); return PHOTONMAP; }
<INITIAL>"nocontour"	{ DBG_TOKEN("nocontour"); return NOCONTOUR; }
<INITIAL>"namespace"	{ DBG_TOKEN("namespace"); return NAMESPACE; }
<INITIAL>"interface"	{ DBG_TOKEN("interface"); return INTERFACE_; }
<INITIAL>"instgroup"	{ DBG_TOKEN("instgroup"); return INSTGROUP; }
<INITIAL>"globillum"	{ DBG_TOKEN("globillum"); return GLOBILLUM; }
<INITIAL>"direction"	{ DBG_TOKEN("direction"); return DIRECTION; }
<INITIAL>"curvature"	{ DBG_TOKEN("curvature"); return CURVATURE; }
<INITIAL>"colorclip"	{ DBG_TOKEN("colorclip"); return COLORCLIP; }
<INITIAL>"writable"	{ DBG_TOKEN("writable"); return WRITABLE; }
<INITIAL>"triangle"	{ DBG_TOKEN("triangle"); return TRIANGLE; }
<INITIAL>"topology"	{ DBG_TOKEN("topology"); return TOPOLOGY; }
<INITIAL>"spectrum"	{ DBG_TOKEN("spectrum"); return SPECTRUM; }
<INITIAL>"softness"	{ DBG_TOKEN("softness"); return SOFTNESS; }
<INITIAL>"segments"	{ DBG_TOKEN("segments"); return SEGMENTS; }
<INITIAL>"scanline"	{ DBG_TOKEN("scanline"); return SCANLINE; }
<INITIAL>"registry"	{ DBG_TOKEN("registry"); return REGISTRY; }
<INITIAL>"rational"	{ DBG_TOKEN("rational"); return RATIONAL; }
<INITIAL>"protocol"	{ DBG_TOKEN("protocol"); return PROTOCOL; }
<INITIAL>"priority"	{ DBG_TOKEN("priority"); return PRIORITY; }
<INITIAL>"position"	{ DBG_TOKEN("position"); return POSITION; }
<INITIAL>"parallel"	{ DBG_TOKEN("parallel"); return PARALLEL_; }
<INITIAL>"override"	{ DBG_TOKEN("override"); return OVERRIDE; }
<INITIAL>"mitchell"	{ DBG_TOKEN("mitchell"); return MITCHELL; }
<INITIAL>"material"	{ DBG_TOKEN("material"); return MATERIAL; }
<INITIAL>"lightmap"	{ DBG_TOKEN("lightmap"); return LIGHTMAP; }
<INITIAL>"instance"	{ DBG_TOKEN("instance"); return INSTANCE; }
<INITIAL>"infinity"	{ DBG_TOKEN("infinity"); return INFINITY_; }
<INITIAL>"implicit"	{ DBG_TOKEN("implicit"); return IMPLICIT; }
<INITIAL>"geometry"	{ DBG_TOKEN("geometry"); return GEOMETRY; }
<INITIAL>"exponent"	{ DBG_TOKEN("exponent"); return EXPONENT; }
<INITIAL>"distance"	{ DBG_TOKEN("distance"); return DISTANCE; }
<INITIAL>"displace"	{ DBG_TOKEN("displace"); return DISPLACE; }
<INITIAL>"delaunay"	{ DBG_TOKEN("delaunay"); return DELAUNAY; }
<INITIAL>"cylinder"	{ DBG_TOKEN("cylinder"); return CYLINDER; }
<INITIAL>"contrast"	{ DBG_TOKEN("contrast"); return CONTRAST; }
<INITIAL>"constant"	{ DBG_TOKEN("constant"); return CONSTANT; }
<INITIAL>"cardinal"	{ DBG_TOKEN("cardinal"); return CARDINAL; }
<INITIAL>"aperture"	{ DBG_TOKEN("aperture"); return APERTURE; }
<INITIAL>"adaptive"	{ DBG_TOKEN("adaptive"); return ADAPTIVE; }
<INITIAL>"accuracy"	{ DBG_TOKEN("accuracy"); return ACCURACY; }
<INITIAL>"visible"	{ DBG_TOKEN("visible"); return VISIBLE; }
<INITIAL>"version"	{ DBG_TOKEN("version"); return VERSION; }
<INITIAL>"verbose"	{ DBG_TOKEN("verbose"); return VERBOSE; }
<INITIAL>"uniform"	{ DBG_TOKEN("uniform"); return UNIFORM; }
<INITIAL>"trilist"	{ DBG_TOKEN("trilist"); return TRILIST; }
<INITIAL>"texture"	{ DBG_TOKEN("texture"); return TEXTURE; }
<INITIAL>"surface"	{ DBG_TOKEN("surface"); return SURFACE; }
<INITIAL>"special"	{ DBG_TOKEN("special"); return SPECIAL; }
<INITIAL>"spatial"	{ DBG_TOKEN("spatial"); return SPATIAL; }
<INITIAL>"shutter"	{ DBG_TOKEN("shutter"); return SHUTTER; }
<INITIAL>"shading"	{ DBG_TOKEN("shading"); return SHADING; }
<INITIAL>"session"	{ DBG_TOKEN("session"); return SESSION; }
<INITIAL>"samples"	{ DBG_TOKEN("samples"); return SAMPLES; }
<INITIAL>"regular"	{ DBG_TOKEN("regular"); return REGULAR; }
<INITIAL>"rebuild"	{ DBG_TOKEN("rebuild"); return REBUILD; }
<INITIAL>"quality"	{ DBG_TOKEN("quality"); return QUALITY; }
<INITIAL>"polygon"	{ DBG_TOKEN("polygon"); return POLYGON; }
<INITIAL>"photons"	{ DBG_TOKEN("photons"); return PHOTONS; }
<INITIAL>"options"	{ DBG_TOKEN("options"); return OPTIONS; }
<INITIAL>"lanczos"	{ DBG_TOKEN("lanczos"); return LANCZOS; }
<INITIAL>"integer"	{ DBG_TOKEN("integer"); return INTEGER; }
<INITIAL>"hermite"	{ DBG_TOKEN("hermite"); return HERMITE; }
<INITIAL>"grading"	{ DBG_TOKEN("grading"); return GRADING; }
<INITIAL>"emitter"	{ DBG_TOKEN("emitter"); return EMITTER; }
<INITIAL>"density"	{ DBG_TOKEN("density"); return DENSITY; }
<INITIAL,MAYA>"default"	{ DBG_TOKEN("default"); return DEFAULT; }
<INITIAL>"declare"	{ DBG_TOKEN("declare"); return DECLARE; }
<INITIAL>"control"	{ DBG_TOKEN("control"); return CONTROL; }
<INITIAL>"contour"	{ DBG_TOKEN("contour"); return CONTOUR; }
<INITIAL>"connect"	{ DBG_TOKEN("connect"); return CONNECT; }
<INITIAL>"caustic"	{ DBG_TOKEN("caustic"); return CAUSTIC; }
<INITIAL>"bspline"	{ DBG_TOKEN("bspline"); return BSPLINE; }
<INITIAL>"boolean"	{ DBG_TOKEN("boolean"); return BOOLEAN_; }
<INITIAL>"window"	{ DBG_TOKEN("window"); return WINDOW; }
<INITIAL>"volume"	{ DBG_TOKEN("volume"); return VOLUME; }
<INITIAL>"vertex"       { DBG_TOKEN("vertex"); return VERTEX; }
<INITIAL>"vendor"	{ DBG_TOKEN("vendor"); return VENDOR; }
<INITIAL>"vector"	{ DBG_TOKEN("vector"); return VECTOR; }
<INITIAL>"taylor"	{ DBG_TOKEN("taylor"); return TAYLOR; }
<INITIAL>"tagged"	{ DBG_TOKEN("tagged"); return TAGGED; }
<INITIAL>"system"	{ DBG_TOKEN("system"); return SYSTEM; }
<INITIAL>"struct"	{ DBG_TOKEN("struct"); return STRUCT; }
<INITIAL>"string"	{ DBG_TOKEN("string"); return STRING; }
<INITIAL>"spread"	{ DBG_TOKEN("spread"); return SPREAD; }
<INITIAL>"sphere"	{ DBG_TOKEN("sphere"); return SPHERE; }
<INITIAL>"shadow"	{ DBG_TOKEN("shadow"); return SHADOW; }
<INITIAL>"shader"	{ DBG_TOKEN("shader"); return SHADER; }
<INITIAL>"select"	{ DBG_TOKEN("select"); return SELECT; }
<INITIAL>"scalar"	{ DBG_TOKEN("scalar"); return SCALAR; }
<INITIAL>"render"	{ DBG_TOKEN("render"); return RENDER; }
<INITIAL>"radius"	{ DBG_TOKEN("radius"); return RADIUS; }
<INITIAL>"photon"	{ DBG_TOKEN("photon"); return PHOTON; }
<INITIAL>"output"	{ DBG_TOKEN("output"); return OUTPUT; }
<INITIAL>"origin"	{ DBG_TOKEN("origin"); return ORIGIN; }
<INITIAL>"opengl"	{ DBG_TOKEN("opengl"); return OPENGL; }
<INITIAL>"opaque"	{ DBG_TOKEN("opaque"); return OPAQUE_; }
<INITIAL>"offset"	{ DBG_TOKEN("offset"); return OFFSET; }
<INITIAL>"object"	{ DBG_TOKEN("object"); return OBJECT; }
<INITIAL>"motion"	{ DBG_TOKEN("motion"); return MOTION; }
<INITIAL>"memory"	{ DBG_TOKEN("memory"); return MEMORY; }
<INITIAL>"matrix"	{ DBG_TOKEN("matrix"); return MATRIX; }
<INITIAL>"mapsto"	{ DBG_TOKEN("mapsto"); return MAPSTO; }
<INITIAL>"length"	{ DBG_TOKEN("length"); return LENGTH; }
<INITIAL>"jitter"	{ DBG_TOKEN("jitter"); return JITTER; }
<INITIAL>"filter"	{ DBG_TOKEN("filter"); return FILTER; }
<INITIAL>"energy"	{ DBG_TOKEN("energy"); return ENERGY; }
<INITIAL>"dither"	{ DBG_TOKEN("dither"); return DITHER; }
<INITIAL>"detail"	{ DBG_TOKEN("detail"); return DETAIL; }
<INITIAL>"delete"	{ DBG_TOKEN("delete"); return DELETE_; }
<INITIAL>"degree"	{ DBG_TOKEN("degree"); return DEGREE; }
<INITIAL>"crease"	{ DBG_TOKEN("crease"); return CREASE; }
<INITIAL>"corner"	{ DBG_TOKEN("corner"); return CORNER; }
<INITIAL>"ccmesh"	{ DBG_TOKEN("ccmesh"); return CCMESH; }
<INITIAL>"camera"	{ DBG_TOKEN("camera"); return CAMERA; }
<INITIAL>"buffer"	{ DBG_TOKEN("buffer"); return BUFFER; }
<INITIAL>"border"	{ DBG_TOKEN("border"); return BORDER; }
<INITIAL>"bezier"	{ DBG_TOKEN("bezier"); return BEZIER; }
<INITIAL>"aspect"	{ DBG_TOKEN("aspect"); return ASPECT; }
<INITIAL>"write"	{ DBG_TOKEN("write"); return WRITE; }
<INITIAL>"world"	{ DBG_TOKEN("world"); return WORLD; }
<INITIAL>"width"	{ DBG_TOKEN("width"); return WIDTH; }
<INITIAL>"white"	{ DBG_TOKEN("white"); return WHITE; }
<INITIAL>"value"	{ DBG_TOKEN("value"); return VALUE; }
<INITIAL>"trace"	{ DBG_TOKEN("trace"); return TRACE; }
<INITIAL>"touch"	{ DBG_TOKEN("touch"); return TOUCH; }
<INITIAL>"strip"	{ DBG_TOKEN("strip"); return STRIP; }
<INITIAL>"store"	{ DBG_TOKEN("store"); return STORE; }
<INITIAL>"state"	{ DBG_TOKEN("state"); return STATE; }
<INITIAL>"space"	{ DBG_TOKEN("space"); return SPACE; }
<INITIAL>"sharp"	{ DBG_TOKEN("sharp"); return SHARP; }
<INITIAL>"scale"	{ DBG_TOKEN("scale"); return SCALE; }
<INITIAL>"raycl"	{ DBG_TOKEN("raycl"); return RAYCL; }
<INITIAL>"mixed"	{ DBG_TOKEN("mixed"); return MIXED; }
<INITIAL>"merge"	{ DBG_TOKEN("merge"); return MERGE; }
<INITIAL>"local"	{ DBG_TOKEN("local"); return LOCAL; }
<INITIAL>"light"	{ DBG_TOKEN("light"); return LIGHT; }
<INITIAL>"level"	{ DBG_TOKEN("level"); return LEVEL; }
<INITIAL>"large"	{ DBG_TOKEN("large"); return LARGE; }
<INITIAL>"group"	{ DBG_TOKEN("group"); return GROUP_; }
<INITIAL>"green"	{ DBG_TOKEN("green"); return GREEN; }
<INITIAL>"gauss"	{ DBG_TOKEN("gauss"); return GAUSS; }
<INITIAL>"gamma"	{ DBG_TOKEN("gamma"); return GAMMA; }
<INITIAL>"front"	{ DBG_TOKEN("front"); return FRONT; }
<INITIAL>"frame"	{ DBG_TOKEN("frame"); return FRAME; }
<INITIAL>"focal"	{ DBG_TOKEN("focal"); return FOCAL; }
<INITIAL>"fine"	        { DBG_TOKEN("fine"); return FINE;  }
<INITIAL>"file"	        { DBG_TOKEN("file"); return FILE_; }
<INITIAL>"field"	{ DBG_TOKEN("field"); return FIELD; }
<INITIAL,MAYA>"false"	{ DBG_TOKEN("false"); return FALSE_; }
<INITIAL>"depth"	{ DBG_TOKEN("depth"); return DEPTH; }
<INITIAL>"debug"	{ DBG_TOKEN("debug"); return DEBUG_; }
<INITIAL>"curve"	{ DBG_TOKEN("curve"); return CURVE; }
<INITIAL>"conic"	{ DBG_TOKEN("conic"); return CONIC; }
<INITIAL>"color"	{ DBG_TOKEN("color"); return COLOR; }
<INITIAL>"child"	{ DBG_TOKEN("child"); return CHILD; }
<INITIAL>"basis"	{ DBG_TOKEN("basis"); return BASIS; }
<INITIAL>"array"	{ DBG_TOKEN("array"); return ARRAY; }
<INITIAL>"apply"	{ DBG_TOKEN("apply"); return APPLY; }
<INITIAL>"angle"	{ DBG_TOKEN("angle"); return ANGLE; }
<INITIAL>"alpha"	{ DBG_TOKEN("alpha"); return ALPHA; }
<INITIAL>"view"	{ DBG_TOKEN("view"); return VIEW; }
<INITIAL>"user"	{ DBG_TOKEN("user"); return USER; }
<INITIAL,MAYA>"true" 	{ DBG_TOKEN("true"); return TRUE_; }
<INITIAL>"trim"	{ DBG_TOKEN("trim"); return TRIM; }
<INITIAL>"tree"	{ DBG_TOKEN("tree"); return TREE; }
<INITIAL>"time"	{ DBG_TOKEN("time"); return TIME; }
<INITIAL>"task"	{ DBG_TOKEN("task"); return TASK; }
<INITIAL>"spdl"	{ DBG_TOKEN("spdl"); return SPDL; }
<INITIAL>"sort"	{ DBG_TOKEN("sort"); return SORT; }
<INITIAL>"size"	{ DBG_TOKEN("size"); return SIZE_; }
<INITIAL>"root"	{ DBG_TOKEN("root"); return ROOT; }
<INITIAL>"read"	{ DBG_TOKEN("read"); return READ; }
<INITIAL>"prep" { DBG_TOKEN("prep"); return PREP; }
<INITIAL>"pass" { DBG_TOKEN("pass"); return PASS; }
<INITIAL>"only" { DBG_TOKEN("only"); return ONLY; }
<INITIAL>"null" { DBG_TOKEN("null"); return NULL_; }
<INITIAL>"ntsc" { DBG_TOKEN("ntsc"); return NTSC; }
<INITIAL>"link"	{ DBG_TOKEN("link"); return LINK; }
<INITIAL>"lens"	{ DBG_TOKEN("lens"); return LENS; }
<INITIAL>"hole"	{ DBG_TOKEN("hole"); return HOLE; }
<INITIAL>"hide"	{ DBG_TOKEN("hide"); return HIDE; }
<INITIAL>"hair"	{ DBG_TOKEN("hair"); return HAIR; }
<INITIAL>"grid"	{ DBG_TOKEN("grid"); return GRID; }
<INITIAL>"face"	{ DBG_TOKEN("face"); return FACE; }
<INITIAL>"even"	{ DBG_TOKEN("even"); return EVEN; }
<INITIAL>"enum"	{ DBG_TOKEN("enum"); return ENUM; }
<INITIAL>"echo"	{ DBG_TOKEN("echo"); return ECHO_; }
<INITIAL>"disc"	{ DBG_TOKEN("disc"); return DISC; }
<INITIAL>"data"	{ DBG_TOKEN("data"); return DATA; }
<INITIAL>"dart"	{ DBG_TOKEN("dart"); return DART; }
<INITIAL>"cusp"	{ DBG_TOKEN("cusp"); return CUSP; }
<INITIAL>"cone"	{ DBG_TOKEN("cone"); return CONE; }
<INITIAL>"code"	{ DBG_TOKEN("code"); return CODE; }
<INITIAL>"clip"	{ DBG_TOKEN("clip"); return CLIP; }
<INITIAL>"call"	{ DBG_TOKEN("call"); return CALL; }
<INITIAL>"bump"	{ DBG_TOKEN("bump"); return BUMP; }
<INITIAL>"both"	{ DBG_TOKEN("both"); return BOTH; }
<INITIAL>"blue"	{ DBG_TOKEN("blue"); return BLUE; }
<INITIAL>"bias"	{ DBG_TOKEN("bias"); return BIAS; }
<INITIAL>"back"	{ DBG_TOKEN("back"); return BACK; }
<INITIAL>"tag"	{ DBG_TOKEN("tag"); return TAG; }
<INITIAL>"set"	{ DBG_TOKEN("set"); return SET; }
<INITIAL>"rgb"	{ DBG_TOKEN("rgb"); return RGB_; }
<INITIAL>"red"	{ DBG_TOKEN("red"); return RED; }
<INITIAL>"ray"	{ DBG_TOKEN("ray"); return RAY_; }
<INITIAL>"raw"	{ DBG_TOKEN("raw"); return RAW; }
<INITIAL,MAYA>"off"	{ DBG_TOKEN("off"); return OFF; }
<INITIAL>"odd"	{ DBG_TOKEN("odd"); return ODD; }
<INITIAL,MAYA>"min"	{ DBG_TOKEN("min"); return MIN_; }
<INITIAL,MAYA>"max"	{ DBG_TOKEN("max"); return MAX_; }
<INITIAL>"imp"	{ DBG_TOKEN("imp"); return IMP; }
<INITIAL>"ies"  { DBG_TOKEN("ies"); return IES; }
<INITIAL>"gui"	{ DBG_TOKEN("gui"); return GUI; }
<INITIAL>"fan"	{ DBG_TOKEN("fan"); return FAN; }
<INITIAL>"end"	{ DBG_TOKEN("end"); return END; }
<INITIAL>"dpi"  { DBG_TOKEN("dpi"); return DPI; }
<INITIAL>"dof"  { DBG_TOKEN("dof"); return DOF; }
<INITIAL>"dod"  { DBG_TOKEN("dod"); return DOD; }
<INITIAL>"bsp"	{ DBG_TOKEN("bsp"); return BSP; }
<INITIAL>"box"	{ DBG_TOKEN("box"); return BOX; }
<INITIAL>"any"	{ DBG_TOKEN("any"); return ANY; }
<INITIAL>"all"	{ DBG_TOKEN("all"); return ALL; }
<INITIAL,MAYA>"on"	{ DBG_TOKEN("on"); return ON; }
<INITIAL>"n"	{ DBG_TOKEN("n"); return N_; }
<INITIAL>"mi"	{ DBG_TOKEN("mi"); return MI; }
<INITIAL>"d2"   { DBG_TOKEN("d2"); return D2; }
<INITIAL>"cg"	{ DBG_TOKEN("cg"); return CG; }
<INITIAL>"w"	{ DBG_TOKEN("w"); return W; }
<INITIAL>"v"	{ DBG_TOKEN("v"); return V; }
<INITIAL>"u"	{ DBG_TOKEN("u"); return U; }
<INITIAL>"t"	{ DBG_TOKEN("t"); return T_; }
<INITIAL>"p"	{ DBG_TOKEN("p"); return P; }
<INITIAL>"m"	{ DBG_TOKEN("m"); return M; }
<INITIAL>"d"	{ DBG_TOKEN("d"); return D; }
<INITIAL>"c"	{ DBG_TOKEN("c"); return CP; }
<INITIAL>"b"	{ DBG_TOKEN("b"); return B; }


<INITIAL>\=		{ DBG_TOKEN("="); return '='; }
<INITIAL>\,		{ DBG_TOKEN(","); return ','; }
<INITIAL>\(		{ DBG_TOKEN("("); return '('; }
<INITIAL>\)		{ DBG_TOKEN(")"); return ')'; }
<INITIAL>\{		{ DBG_TOKEN("{"); return '{'; }
<INITIAL>\}		{ DBG_TOKEN("}"); return '}'; }
<INITIAL>\[		{ DBG_TOKEN("["); return '['; }
<INITIAL>\]		{ DBG_TOKEN("]"); return ']'; }

<INITIAL>"#":             { BEGIN(MAYA); DBG_TOKEN("#:"); 
                            return T_MAYA_OPTION; }
<INITIAL>"#"\n            { DBG_TOKEN("#"); ++yyline; }
<INITIAL>"#"[^:\n].*\n    { DBG_TOKEN("#"); ++yyline; }


<INITIAL,MAYA>[+-]?{DIGIT}*"."{DIGIT}*  {
   /* This matches floating point numbers like: 1. and 1.32 */
   DBG_TOKEN(yytext);
  yylval.floating = atof( yytext );
  return T_FLOAT;
}

<INITIAL,MAYA>[+-]?{DIGIT}+[dDeE][+-]{DIGIT}+   {
  /* This matches floating point numbers like: 1e+42 */
   DBG_TOKEN(yytext);
  yylval.floating = atof( yytext );  return T_FLOAT;
}

<INITIAL,MAYA>[+-]?{DIGIT}+"."{DIGIT}*[dDeE][+-]{DIGIT}+  {
   DBG_TOKEN(yytext);
  /* This matches floating point numbers like: 1.32e+42 */
  yylval.floating = atof( yytext ); return T_FLOAT;
}

<INITIAL,MAYA>[+-]?{DIGIT}+             {
   DBG_TOKEN(yytext);
  yylval.integer = atoi( yytext ); return T_INTEGER;
}


<INITIAL>^$lookup     { BEGIN(LOOKUP); }
<LOOKUP>[ \t]+      /* eat the whitespace */
<LOOKUP>{string}   { /* got the include file name */
   BEGIN(INITIAL);
}

<INITIAL>^$include     { BEGIN(INCLUDE); }
<INCLUDE>[ \t]+      /* eat the whitespace */
<INCLUDE>{string}   { /* got the include file name */
   mrl_yy_new_file( yytext+1, yyleng-2 );
   BEGIN(INITIAL);
}
<<EOF>> {
   if ( mrl_yy_next_file() == 0 ) {
      yyterminate();
   }
}



<MAYA>"help"      { DBG_TOKEN("help"); BEGIN(HELPMSG); return HELP; }
<MAYA>"shortname" { DBG_TOKEN("shortname"); return SHORTNAME; }
<MAYA>"softmin"   { DBG_TOKEN("softmin"); return SOFTMIN; }
<MAYA>"softmax"   { DBG_TOKEN("softmax"); return SOFTMAX; }
<MAYA>"icon"      { DBG_TOKEN("icon"); return ICON; }
<MAYA>"enum"      { DBG_TOKEN("enum"); return ENUM; }
<MAYA>"classification" { DBG_TOKEN("classification"); return CLASSIFICATION; }
<MAYA>"property"  { DBG_TOKEN("property"); return PROPERTY; }
<MAYA>"nodeid"    { BEGIN(NODEID); DBG_TOKEN("nodeid"); return NODEID_; }
<MAYA>"#".*\n     { DBG_TOK(); BEGIN(INITIAL); ++yyline; }
<MAYA>"hidden"    { DBG_TOKEN("hidden"); return HIDDEN; }
<MAYA>[\t \r]+    { DBG_TOKEN(" "); }
<MAYA>\n          { DBG_TOKEN("\\n");BEGIN(INITIAL); ++yyline; }

<NODEID>{DIGIT}+  { BEGIN(MAYA); yylval.nodeid = atol(yytext); 
                    DBG_TOKEN(yytext); return T_NODEID; }
<NODEID>\n          { DBG_TOK(); BEGIN(INITIAL); ++yyline; }
<NODEID>[ \t\r]+    { DBG_TOK(); }
<NODEID>.           { yyerror("missing nodeid"); yyterminate(); }




<INITIAL,MAYA>\"         { DBG_TOKEN("\""); string_caller = YY_START;
                           BEGIN(STR); }
<STR>[^\n\"]+       {
                      DBG("string:%s\n", yytext);
                      DBG_TOKEN(yytext); yylval.string = new_string(yytext);
		      return T_STRING;
                    }
<STR>[\"\n]         { DBG_TOKEN("\""); BEGIN(string_caller); } 


<HELPMSG>[\t \r\n]+   { ; }
<HELPMSG>\"            {
                        DBG_TOK(); string_caller = MAYA;
                        BEGIN(MULTILINESTR); 
                       }
<HELPMSG>.            {
                        yyerror("Missing help message");
                      }

<MULTILINESTR>[^\"\n\r]+ {
                           DBG_TOKEN(yytext); 
			   yylval.string = new_string(yytext);
		           return T_STRING;
                         }
<MULTILINESTR>\n[\t #:]+ { DBG_TOK(); ++yyline; }
<MULTILINESTR>\r         { DBG_TOK(); }
<MULTILINESTR>\"         { DBG_TOK(); BEGIN(string_caller); return END_STR; }





<INITIAL>`                { BEGIN(BINVEC); }
<BINVEC>............`     {
                            yylval.vector[0] = *( (float*) yytext);
                            yylval.vector[1] = *(((float*) yytext) + 1);
                            yylval.vector[2] = *(((float*) yytext) + 2);
                            BEGIN(INITIAL);
			    return T_VECTOR;
                          }

<INITIAL>\'               { BEGIN(BYTESTRING); }

<BYTESTRING>[^\']+        {
                            yylval.byte_string.len = yyleng;
                            yylval.byte_string.bytes = (unsigned char*) yytext;
			    return T_BYTE_STRING;
                          }
<BYTESTRING>\'            { BEGIN(INITIAL); }




<INITIAL,MAYA>{WORD}     {
                            DBG("symbol:%s\n", yytext);
                            yylval.symbol = yylval.string = new_string(yytext);
                            DBG_TOKEN(yytext); return T_SYMBOL;
                          }
<INITIAL>[\ \t\r]+   { DBG_TOKEN(" "); }
<INITIAL>\n          { ++yyline; DBG_TOKEN("\\n"); }
<INITIAL>.                    {
                                DBG_TOK();
                                yyerror("illegal token");
                                yyterminate();
                              }

%%


char* new_string( const char* s )
{
   char* st;
   if ( s == NULL ) return "";

   if ( num_strings == max_strings )
   {
      char** old_string_elem = string_elem;
      string_elem = (char**) malloc( sizeof( char* ) *
				     (max_strings + reserve_strings) );
      if ( string_elem == NULL )
      {
	 fprintf(stderr,"Run out of memory to realloc strings.\n" );
	 return (char*)s;
      }
     
      if ( string_elem != old_string_elem )
      {
	 memmove( string_elem, old_string_elem, max_strings * sizeof(char*) );
	 free( old_string_elem );
      }
      max_strings += reserve_strings;
   }

   st = strdup(s);
   if ( st == NULL ) {
       fprintf(stderr,"Run out of memory to allocating string.\n");
       return "";
   }
   string_elem[num_strings] = st;
   ++num_strings;
   return st;
}



void alloc_all_strings()
{
   num_strings = 0;
   max_strings = reserve_strings;

   string_elem = (char**) malloc( max_strings * sizeof(char*) );
   memset( string_elem, 0, max_strings * sizeof(char*) );

   if ( string_elem == NULL )
     {
       fprintf(stderr,"Run out of memory to malloc strings\n");
       fflush(stderr);
     }
}



void free_all_strings()
{
   while ( num_strings > 0 )
   {
      --num_strings;
      free( string_elem[num_strings] );
   }
   free( string_elem );
   string_elem = NULL;
}



int mi_open_file( const char* mifile )
{
   yyline = 1;
   free(yyfile);
   yyfile = strdup( mifile );
   if ( !yyfile )
   {
      fprintf(stderr, "Not enough memory to alloc string" );
      return 1;
   }

   yyin = fopen(yyfile, "rb");
   if ( yyin == NULL )
   {
      free(yyfile); yyfile = NULL;
      return 0;
   }
   
   YY_NEW_FILE;
   
   alloc_all_strings();
   DBG("mi_open_file: %s\n", mifile);
   
   include_stack_ptr = 0;
   include_stack[include_stack_ptr].buf = NULL;
   include_stack[include_stack_ptr].file = yyfile;
   return 1;
}



FILE* search_for_include_file( const char* envpath, const char* incfile )
{
   char filename[256];
   char* buf;
   char* path;
   char* last;
   FILE* incyyin = NULL;

   if ( envpath == NULL ) return NULL;
   
   buf = path = last = strdup(envpath);
   for ( ; *path != 0; ++path )
      if ( *path == ';' ) *path = 0;
   
   path = buf;
   last += strlen(envpath);
   while (path < last ) { 
      sprintf( filename, "%s/%s", path, incfile );
      incyyin = fopen( filename, "rb" );
      if ( incyyin ) break;
      path += strlen( path ) + 1;
   }
   free(buf);

   fflush(stderr);
   
   return incyyin;
}


int mrl_yy_new_file( const char* text, int len )
{
   char* incfile;
   FILE* incyyin;
   if ( include_stack_ptr >= MAX_INCLUDE_DEPTH-1 )
   {
      yyerror( "Includes nested too deeply" );
      mrl_yy_next_file(); /* stop parsing rest of file */
      return 1;
   }
   
   incfile = yyfile = strdup(text);
   incfile[len] = 0;
   
   incyyin = fopen( incfile, "rb" );
   
   if ( ! incyyin )
   {
      const char* envpath = getenv("MI_CUSTOM_SHADER_PATH");
      incyyin = search_for_include_file( envpath, incfile );
      if ( !incyyin )
      {
	 char err[256];
	 sprintf( err, "Include file \"%s\" not found", incfile );
	 yyerror( err );
	 mrl_yy_next_file(); /* stop parsing rest of file */
	 return 1;
      }
   }

   include_stack[include_stack_ptr].line = yyline;
   include_stack[include_stack_ptr].buf  = YY_CURRENT_BUFFER;

   ++include_stack_ptr;
   include_stack[include_stack_ptr].file = incfile;
   
   yyline = 1;
   yy_switch_to_buffer( yy_create_buffer( incyyin, YY_BUF_SIZE ) );
   return 0;
}



int mrl_yy_next_file()
{
   if ( yyin ) fclose(yyin); 
   yyin = NULL;
   free(yyfile);
   if ( --include_stack_ptr < 0 )
   {
      yyfile = NULL;
      free_all_strings();
      return 0;
   }
   else
   {
      yyfile = include_stack[include_stack_ptr].file;
      yyline = include_stack[include_stack_ptr].line;
      yy_delete_buffer( YY_CURRENT_BUFFER );
      yy_switch_to_buffer( include_stack[include_stack_ptr].buf );
   }
   return 1;
 }
