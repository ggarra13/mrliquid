//
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
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#ifndef mrStream_inl
#define mrStream_inl


BEGIN_NAMESPACE( mr )

inline int buffer_base::sync()
{  
  // mostly borrowed from msvc's stl...
  if ( ! pbase() ) return 0;

  // make sure to null terminate the string
  sputc('\0');

  // freeze and call the virtual print method
  print( str().c_str() );

  // reset iterator to first position & unfreeze
  seekoff( 0, std::ios::beg );
  return 0;
}

END_NAMESPACE( mr )


// Constants to determine how much precision to use in printing
static const std::streamsize kCP  = 3;  // for miColor
static const std::streamsize kGVP = 7;  // for miGeoVector
static const std::streamsize kVP  = 6;  // for miVector
static const std::streamsize kV2P = 3;  // for miVector2d
static const std::streamsize kMP  = 5;  // for miMatrix


inline
std::ostream& operator<<( std::ostream& o, const miShader_state_op t )
{
  using namespace std;
  return o << ( 
	       t == miSHADERSTATE_STATE_INIT  ? "STATE Init" :
	       t == miSHADERSTATE_STATE_EXIT  ? "STATE Exit" :
	       t == miSHADERSTATE_SAMPLE_INIT ? "SAMPLE Init" :
	       t == miSHADERSTATE_SAMPLE_EXIT ? "SAMPLE Exit" :
	       "Unknown" 
	       );
}


inline
std::ostream& operator<<( std::ostream& o, const miRay_type t )
{
  using namespace std;
  return o << ( 
	       t == miRAY_EYE                  ? "Eye" :
	       t == miRAY_TRANSPARENT          ? "Transparent" :
	       t == miRAY_REFLECT              ? "Reflect" :
	       t == miRAY_REFRACT              ? "Refract" :
	       t == miRAY_LIGHT                ? "Light" :
	       t == miRAY_SHADOW               ? "Shadow" :
	       t == miRAY_ENVIRONMENT          ? "Environment" :
	       t == miRAY_NONE                 ? "None" :
	       t == miPHOTON_ABSORB            ? "Photon Absorb" :
	       t == miPHOTON_LIGHT             ? "Photon Light" :
	       t == miPHOTON_REFLECT_SPECULAR  ? "Photon Reflect Specular" :
	       t == miPHOTON_REFLECT_GLOSSY    ? "Photon Reflect Glossy" :
	       t == miPHOTON_REFLECT_DIFFUSE   ? "Photon Reflect Diffuse" :
	       t == miPHOTON_TRANSMIT_SPECULAR ? "Photon Transmit Specular" :
	       t == miPHOTON_TRANSMIT_GLOSSY   ? "Photon Transmit Glossy" :
	       t == miPHOTON_TRANSMIT_DIFFUSE  ? "Photon Transmit Diffuse" :
	       t == miRAY_DISPLACE             ? "Displace" :
	       t == miRAY_OUTPUT               ? "Output" :
	       t == miPHOTON_SCATTER_VOLUME    ? "Photon Volume Scatter" :
	       t == miPHOTON_TRANSPARENT       ? "Photon Transparent" :
	       t == miRAY_FINALGATHER          ? "Final Gather" :
	       t == miRAY_LM_VERTEX            ? "LightMap Vertex" :
	       t == miRAY_LM_MESH              ? "LightMap Mesh" :
	       t == miPHOTON_EMIT_GLOBILLUM    ? "Photon Emit Glob.Illum" :
	       t == miPHOTON_EMIT_CAUSTIC      ? "Photon Emit Caustic" :
	       t == miRAY_PROBE                ? "Probe" :
	       t == miRAY_HULL                 ? "Hull" :
	       t == miPHOTON_HULL              ? "Photon Hull" :
	       "Unknown" 
	       );
}


//! IOStream Manipulator to turn an miTag into a string
//! Usage:
//! \code
//!    mr_info( tag2name(state->intance) );
//! \endcode
//!
//! Note that some .mi converters save time and memory by not
//! sending the name of the object in previews ( maya2mr ).
//! Also, only the server machine has the scene name database.
struct tag2name
{
   public:
     explicit tag2name( const miTag t ) : tag(t) {};
     
   private:
     const miTag tag;
     
     inline friend
     std::ostream&  operator<<( std::ostream& o, const tag2name& b )
     {
	if ( b.tag == miNULLTAG ) return o << "(miNULLTAG)";
	
	o << "\"(" << (int) b.tag << ") ";
	
	mi_db_access( b.tag );
	const char* obj_inst_name = mi_api_tag_lookup( b.tag );
	if ( obj_inst_name ) o << obj_inst_name << '"';
	else o << "(unnamed)\"";
	mi_db_unpin( b.tag );
	return o;
     };
};



inline
std::ostream& operator<<( std::ostream& o, const miColor& c ) 
{
  using namespace std;
  return o << "( " << setw(kCP+2) << setprecision(kCP) << c.r 
	   << ", " << setw(kCP+2) << setprecision(kCP) << c.g 
	   << ", " << setw(kCP+2) << setprecision(kCP) << c.b
	   << "| " << setw(kCP+2) << setprecision(kCP) << c.a
	   << " )";
}


inline
std::ostream&   operator<<( std::ostream& o, const miGeoVector& v ) 
{
  using namespace std;
  return o << "[ " << setw(kGVP+5) << setprecision(kGVP) << v.x 
	   << " "  << setw(kGVP+5) << setprecision(kGVP) << v.y 
	   << " "  << setw(kGVP+5) << setprecision(kGVP) << v.z 
	   << " ]";
}


inline
std::ostream&   operator<<( std::ostream& o, const miVector& v ) 
{
  using namespace std;
  return o << "[ " << setw(kVP+5) << setprecision(kVP) << v.x 
	   << " "  << setw(kVP+5) << setprecision(kVP) << v.y 
	   << " "  << setw(kVP+5) << setprecision(kVP) << v.z 
	   << " ]";
}


inline
std::ostream&   operator<<( std::ostream& o, const miVector2d& v ) 
{
  using namespace std;
  return o << "[ " << setw(kV2P+5) << setprecision(kV2P) << v.u 
	   << " "  << setw(kV2P+5) << setprecision(kV2P) << v.v 
	   << " ]";
}


inline
std::ostream& operator<<( std::ostream& o, const miMatrix& m ) 
{
  using namespace std;
  size_t col = o.tellp();  col = col >= 0 ? col+2 : 2;
  return o << "| " 
	   << setw(kMP+5) << setprecision(kMP) << m[0] << "|"
	   << setw(kMP+5) << setprecision(kMP) << m[1] << "|"
	   << setw(kMP+5) << setprecision(kMP) << m[2] << "|"
	   << setw(kMP+5) << setprecision(kMP) << m[3] << " |" 
	   << endl << setw(col)
	   << "| " 
	   << setw(kMP+5) << setprecision(kMP) << m[4] << "|"
	   << setw(kMP+5) << setprecision(kMP) << m[5] << "|"
	   << setw(kMP+5) << setprecision(kMP) << m[6] << "|"
	   << setw(kMP+5) << setprecision(kMP) << m[7] << " |"
	   << endl << setw(col)
	   << "| " 
	   << setw(kMP+5) << setprecision(kMP) << m[8] << "|"
	   << setw(kMP+5) << setprecision(kMP) << m[9] << "|"
	   << setw(kMP+5) << setprecision(kMP) << m[10] << "|"
	   << setw(kMP+5) << setprecision(kMP) << m[11] << " |"
	   << endl << setw(col)
	   << "| " 
	   << setw(kMP+5) << setprecision(kMP) << m[12] << "|"
	   << setw(kMP+5) << setprecision(kMP) << m[13] << "|"
	   << setw(kMP+5) << setprecision(kMP) << m[14] << "|"
	   << setw(kMP+5) << setprecision(kMP) << m[15] << " |";
}


inline
std::ostream& operator<<( std::ostream& o, const miState* const state )
{
  using namespace std;
  return o << "(" 
	   << setw(5) << setprecision(2) << state->raster_x << "," 
	   << setw(5) << setprecision(2) << state->raster_y 
	   << ") [" << state->thread << "] ray=" << state->type
#ifndef RAY36 
	   << "  pri_idx=" << state->pri_idx 
#endif
	   << "  hit=" << tag2name(state->instance) 
	   << endl  
	   << "reflection level=" 
	   << state->reflection_level << "/"
	   << state->options->reflection_depth
	   << "   refraction level=" 
	   << state->refraction_level << "/"
	   << state->options->refraction_depth
	   << "   sum level=" 
	   << state->refraction_level+state->reflection_level << "/"
	   << state->options->trace_depth << endl
           << "dot_nd =" << setw(1) << setprecision(3) << state->dot_nd 
	   << "  inv_normal? " << (state->inv_normal?"Yes":"No ")
	   << " dist=" << setw(kGVP+2) << setprecision(kGVP) << state->dist 
	   << endl
	   << "org=" << state->org    << "  dir=" << state->dir << endl
	   << "  P=" << state->point  
	   << "  ior="   << setw(1) << setprecision(2) << state->ior 
	   << " ior_in=" << setw(1) << setprecision(2) << state->ior_in 
	   << endl
	   << "  N=" << state->normal << "   Ng=" << state->normal_geom 
	   << endl
	   << "mat=" << tag2name(state->material)
	   << " vol=" << tag2name(state->volume) 
	   << " ref_vol=" << tag2name(state->refraction_volume)
    ;
}


inline
std::ostream& operator<<( std::ostream& o, const miState& state ) {
  return o << &state;
}


#endif
