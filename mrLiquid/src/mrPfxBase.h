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
#ifndef mrPfxBase_h
#define mrPfxBase_h

#include "mrObject.h"

class MRenderLineArray;

class mrPfxBase : public mrObject
{
protected:
  typedef std::vector< unsigned > HairOffsets;

  enum SpitFlags
    {
      kTwist         = 1 << 0,
      kFlatness      = 1 << 1,
      kIncandescence = 1 << 2,
      kTransparency  = 1 << 3,
      kColor         = 1 << 4,
      kMoblur        = 1 << 5,
      kParameter     = 1 << 6,
    };

  int spit;
  int type_;
  int degree_;
  int approx_;
  int hairsPerClump_;

public:
  inline int      type()     { return type_; };
  inline int      degree()   { return degree_; };
  inline int      approx()   { return approx_; };
  inline int hairsPerClump() { return hairsPerClump_; };

  static void write_mi_scalars( MRL_FILE* f, const MDoubleArray& d );

protected:
  virtual void write_approximation(MRL_FILE* ) {};

  //! Constructor
  mrPfxBase( const MDagPath& shape );

  virtual void write_object_definition( MRL_FILE* f );

  //////////////////////////////////////////////////////////
  /// PFX functions using MRenderLines
  //////////////////////////////////////////////////////////
  static void write_scalars( MRL_FILE* f, const MDoubleArray& d );
  void write_lines( MRL_FILE* f, const MRenderLineArray& lines );

  void add_vertices( const MRenderLineArray& lines );
  void add_vertices( const MRenderLineArray& lines,
		     char step );
  void count_vertices( const MRenderLineArray& lines );
  void count_scalars( unsigned& numScalars, HairOffsets& offsets,
		      const MRenderLineArray& lines );

  /** 
   * Writes a single .hr file for the lines.
   */
  void write_hr_file( MRL_FILE* f, 
		      HairOffsets& offsets,
		      const MRenderLineArray& lines );
  void getOffsets( HairOffsets& offsets, 
		   const MRenderLineArray& lines );

#ifdef GEOSHADER_H
public:
  static void write_scalars( miScalar* store, const MDoubleArray& d );
protected:
  virtual void write_approximation() {};

  void write_lines(  miScalar* store, const MRenderLineArray& lines );
#endif

  unsigned mbIdx;
  unsigned totalVerts;
};


#endif // mrPfxBase_h
