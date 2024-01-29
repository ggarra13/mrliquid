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
#ifndef mrParticles_h
#define mrParticles_h

#include "maya/MFnParticleSystem.h"
#include "maya/MTime.h"

class MStringArray;
class MIntArray;


#include "mrObject.h"

//! Abstract class used to obtain all information about a particle shape.
//! This class has methods also to save out a pdc file for rendering.
class mrParticles : public mrObject
{
     friend class mrShadingGroupParticles;

#if MAYA_API_VERSION < 700
     enum RenderType
     {
     kMultiPoint,  //DONE
     kMultiStreak, //DONE
     kNumeric,
     kPoints,      //DONE
     kSpheres,
     kSprites,     //DONE
     kStreak,      //DONE
     kBlobby,      //DONE
     kCloud,      //DONE
     kTube
     };
#else
     enum RenderType
     {
     kMultiPoint = MFnParticleSystem::kMultiPoint,
     kMultiStreak = MFnParticleSystem::kMultiStreak,
     kNumeric = MFnParticleSystem::kNumeric,
     kPoints = MFnParticleSystem::kPoints,
     kSpheres = MFnParticleSystem::kSpheres,
     kSprites = MFnParticleSystem::kSprites,
     kStreak = MFnParticleSystem::kStreak,
     kBlobby = MFnParticleSystem::kBlobby,
     kCloud = MFnParticleSystem::kCloud,
     kTube = MFnParticleSystem::kTube,
     };
#endif

   public:
     static mrParticles* factory( const MDagPath& shape );

     virtual void getMotionBlur( const char ) {};
     virtual void write_approximation(MRL_FILE* ) {};

#ifdef GEOSHADER_H
     virtual void write_approximation()       {};
#endif

     bool isVolumetric() const { return volumetric; }
     bool isSprite() const { return ((short)renderType == (short)kSprites); };
     
     //! Return the frame rate for a particular time unit
     static int  getFrameRate( const MTime::Unit unit );

     virtual void forceIncremental();
     virtual void setIncremental( bool sameFrame );

     virtual void write_user_data( MRL_FILE* f );
     virtual void write_properties( MRL_FILE* f );
     virtual void write_group( MRL_FILE* f );
     virtual void write_object_definition( MRL_FILE* f );

     /// HAIR STUFF
     void write_hair_object_definition( MRL_FILE* f );
     void write_hair_group( MRL_FILE* f );

     /// VOLUMETRIC STUFF
     void write_volumetric_user_data( MRL_FILE* f );
     void write_volumetric_properties( MRL_FILE* f );
     void write_volumetric_group( MRL_FILE* f );


#ifdef GEOSHADER_H
     virtual void write_user_data();
     virtual void write_group();
     virtual void write_object_definition();

     /// HAIR STUFF
     void write_hair_object_definition();
     void write_hair_group( miHair_list* h );

     /// VOLUMETRIC STUFF
     void write_volumetric_properties();
     void write_volumetric_user_data();
     void write_volumetric_group();
#endif

     //! Return the full name of the pdc file for this frame.
     MString getPDCFile() const;

   protected:
     char pdcWritten;

     //! Write the PDC file to disk
     void writePDCFile() const;

     //! Write particle data file to disk (in maya2mr's format)
     void writeParticleFile( MRL_FILE* f ) const;

     //! Get data of particle node
     void getData();

     //! Constructor
     mrParticles( const MDagPath& shape );

     //! is it a volumetric particle type?
     bool volumetric;

     //! PDC file (root) where particle data will be saved to
     //! (or has already been saved to)
     MString miPDCFile;

     //! Number of particles in system
     unsigned          count;

     //! List of attributes to spit out
     MStringArray attributes;

     //! Type of attribute (PDC integer value)
     MIntArray    attributeTypes;

     //! Type of particle
     MFnParticleSystem::RenderType  renderType;
};


#endif // mrParticles_h

