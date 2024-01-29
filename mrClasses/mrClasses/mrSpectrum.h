//
//  Copyright (c) 2006, Gonzalo Garramuno
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


#ifndef mrSpectrum_h
#define mrSpectrum_h


namespace mr
{

class spectrum
{
protected:
  miSpectrum* s;

public:
  /////////////////////////
  // Blueprint spectrum
  /////////////////////////
  static miSpectrum* blueprint()
  {
    return mi_spectrum_blueprint();
  }


  /////////////////////////
  // Constructor
  /////////////////////////
  inline spectrum() :
    s( mi_spectrum_create(NULL) )
  {
  }

  inline spectrum( miSpectrum* b ) :
    s( b )
  {
  }

  /////////////////////////
  // Copy Constructor
  /////////////////////////
  inline spectrum( const spectrum& b )
  {
    mi_spectrum_copy( s, b.s );
  }

  /////////////////////////
  // Destructor
  /////////////////////////
  inline ~spectrum()
  {
    if ( s ) mi_spectrum_delete( s );
  }


  /////////////////////////
  // Clearing
  /////////////////////////
  inline void clear()
  {
    mi_spectrum_clear( s );
  }

  inline void reset()
  {
    mi_spectrum_reset( s );
  }


  /////////////////////////
  // Copies
  /////////////////////////

  inline miSpectrum* clone() const
  {
    return mi_spectrum_clone( s );
  }

  inline spectrum& copy( const spectrum& b )
  {
    mi_spectrum_copy( s, b.s );
    return *this;
  }

  inline spectrum& copy( const miSpectrum* b )
  {
    mi_spectrum_copy( s, b );
    return *this;
  }

  /////////////////////////
  // Assignments
  /////////////////////////
  inline spectrum& operator=( const miSpectrum* b )
  {
    mi_spectrum_assign( s, b );
    return *this;
  }

  inline spectrum& operator=( const miSpectrum_data* b )
  {
    mi_spectrum_assign_data( s, b );
    return *this;
  }

  inline spectrum& operator=( const spectrum& b )
  {
    mi_spectrum_assign( s, b.s );
    return *this;
  }

  /////////////////////////
  // Operators
  /////////////////////////
  inline spectrum& operator+=( const miSpectrum* b )
  {
    mi_spectrum_add( s, b );
    return *this;
  }

  inline spectrum& operator+=( const spectrum& b )
  {
    mi_spectrum_add( s, b.s );
    return *this;
  }

  inline spectrum& operator+=( const mr::color& b )
  {
    mi_spectrum_color_add( s, &b );
    return *this;
  }

  inline spectrum& operator*=( const miSpectrum* b )
  {
    mi_spectrum_mul( s, b );
    return *this;
  }

  inline spectrum& operator*=( const spectrum& b )
  {
    mi_spectrum_mul( s, b.s );
    return *this;
  }

  inline spectrum& operator*=( const miScalar& b )
  {
    mi_spectrum_scale( s, b );
    return *this;
  }

  /////////////////////////
  // Special Functions
  /////////////////////////
  inline miScalar linewidth( miScalar w )
  {
    return mi_spectrum_linewidth( s, w );
  }

  inline unsigned size() const { return mi_spectrum_size(s); }

  inline void range( miScalar& lo, miScalar& hi )
  {
    mi_spectrum_range_get( s, &lo, &hi );
  }

  static void visible_range( miScalar& lo, miScalar& hi )
  {
    mi_spectrum_visible_range( &lo, &hi );
  }

  inline void blackbody( const miScalar temperature, const miScalar intensity )
  {
    mi_spectrum_blackbody( s, temperature, intensity );
  }

  inline void cie( const miScalar intensity, const char* illuminant = "D65" )
  {
    mi_spectrum_cie( s, intensity, illuminant );
  }

  inline void sky( const miScalar temperature, const miScalar intensity )
  {
    mi_spectrum_cie_sky( s, temperature, intensity );
  }

  /////////////////////////
  // Conversion Functions
  /////////////////////////
  inline miColor to_rgb() 
  {
    miColor c;
    mi_spectrum_rgb( &c, s );
    return c;
  }

  inline miColor ciexyz()
  {
    miColor c;
    mi_spectrum_ciexyz( &c, s );
    return c;
  }


  /////////////////////////
  // Components
  /////////////////////////

  inline miScalar operator[]( const miScalar w )
  {
    return mi_spectrum_value_get( s, w );
  }

  inline miScalar get( const miScalar w )
  {
    return mi_spectrum_value_get( s, w );
  }

  inline miScalar set( const miScalar w, const miScalar v )
  {
    return mi_spectrum_value_set( s, w, v );
  }

  inline miScalar add( const miScalar w, const miScalar v )
  {
    return mi_spectrum_value_add( s, w, v );
  }

  /////////////////////////
  // Auxiliary Functions
  /////////////////////////
  inline void lerp( const spectrum& b, const miScalar v )
  {
    mi_spectrum_lerp( s, b.s, v );
  }

  inline void lerp( const miSpectrum* b, const miScalar v )
  {
    mi_spectrum_lerp( s, b, v );
  }

};

}  // namespace mr

#endif  // mrSpectrum

