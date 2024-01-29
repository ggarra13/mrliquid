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

#ifndef mrInstance_h
#define mrInstance_h

#include "maya/MMatrix.h"
#include "maya/MString.h"
#include "maya/MDagPath.h"
#include "maya/MFnDagNode.h"
#include "maya/MPlug.h"
#include "maya/MPlugArray.h"


#include <cassert>

#include "mrNode.h"

class mrShape;


//! Base class used for instance and instance groups
class mrInstanceBase : public mrNode
{
   protected:
     mrInstanceBase() : mrNode() {};
     mrInstanceBase( const MFnDependencyNode& path ) : mrNode( path ) {};
     mrInstanceBase( const MDagPath& path ) : mrNode( path ) {};
     mrInstanceBase( const MString& s ) : mrNode(s) {};

     
   public:

     //! Update instance and shape's motion blur information for current step
     virtual void getMotionBlur( const char step ) {};
     
     virtual void write(MRL_FILE* f) {};
#ifdef GEOSHADER_H
     virtual void write() {};
#endif

     //! Find an instance using its .mi name as key.
     static mrInstanceBase*  find( const MString& name );
     static mrInstanceBase* factory(const MString& s);
};

class mrGroupInstance;

//! Instance that contains a group
class mrInstanceGroup : public mrInstanceBase
{
   public:
     mrGroupInstance* group;

   protected:
     mrInstanceGroup( const MString& s, mrGroupInstance* g ) : 
     mrInstanceBase(s), group(g) {};

   public:
     //! Force an incremental update/refresh of data
     virtual void forceIncremental();

     //! Update instance incrementally (if needed)
     virtual void setIncremental( bool sameFrame );

     virtual void write(MRL_FILE* f);
#ifdef GEOSHADER_H
     virtual void write();
#endif
     static mrInstanceGroup* factory(const MString& s, mrGroupInstance* g );
};


//! Base class for instances
class mrInstance : public mrInstanceBase
{
   protected:
     mrInstance( const MString&  name, mrShape* s );
     mrInstance( const MDagPath& instDagPath );
     mrInstance( const MDagPath& instDagPath, mrShape* s );

     //! Write out matrices for current instance
     void write_matrices( MRL_FILE* f );
     
     //! Check to see if instance is animated in any way
     inline void isAnimated();

     //! Get new matrix for current frame (if needed)
     virtual void getMatrix( bool sameFrame );
     
     inline char maya_vflag_to_mray( char flag ) const throw();

   public:
     virtual ~mrInstance() {};

     static mrInstance*  find( const MDagPath& inst, const MDagPath& shape,
			       const unsigned numShapes );
     static mrInstance*  find( MDagPath& inst, const MDagPath& shape );
     static mrInstance*  find( const MDagPath& shape );


     //! Create a new instance from a shape
     static mrInstance* factory( const MDagPath& shapeDagPath, mrShape* s );
     

     //! Update instance for this new render pass (if needed)
     virtual void newRenderPass();
     
     //! Update instance incrementally (if needed)
     virtual void forceIncremental();

     //! Update instance incrementally (if needed)
     virtual void setIncremental( bool sameFrame );

     //! Update instance and shape's motion blur information for current step
     virtual void getMotionBlur( const char step );
     
     //! Write out the instance
     virtual void write_instance( MRL_FILE* f );

     //! Write out the shape + instance
     virtual void write( MRL_FILE* f );
     
     //! Write out the properties of the instance
     virtual void write_properties( MRL_FILE* f );

     virtual void debug( int tabs = 0 ) const throw();

#ifdef GEOSHADER_H
   protected:
     //! Write out the properties of the instance using raylib
     void write_matrices( miInstance* i );

   public:
     //! Write out the instance using raylib
     virtual void write_instance();

     //! Write out instance using raylib
     virtual void write();

     //! Write out the properties of the instance using raylib
     virtual void write_properties( miInstance* i );
#endif
     

     enum VisibleFlags {
     kVOff = 2,
     kVOn = 1,
     kVInherit = 0,
     };

     enum MayaVisibleFlags {
     kMayaVOff = 0,
     kMayaVOn = 1,
     kMayaVInherit = 2,
     };

     mrShape* shape; //! Shape attached to instance (if any)

     MMatrix m;      //! transform matrix
     MMatrix mt;     //! motion transform matrix
     MDagPath path;  //! needed to support instancing with materials

     char visible, old_visible; 
     bool  animated, hide, old_hide;
};


typedef  mrHash< mrInstanceBase > mrInstanceList;
extern mrInstanceList instanceList;


class mrInstanceLight : public mrInstance
{
     friend class mrSwatchRender;
   protected:
     mrInstanceLight( const MString& name, mrShape* s );
     mrInstanceLight( const MDagPath&   p, mrShape* s );

   public:
     virtual void write(MRL_FILE* f );

     //! Force update of instance incrementally
     virtual void forceIncremental();

     //! Update instance incrementally (if needed)
     virtual void setIncremental( bool sameFrame );


#ifdef GEOSHADER_H
     virtual void write();
#endif

     //! Create a new instance from a shape
     static mrInstanceLight* factory( const MString& name, mrShape* s );

     //! Create a new instance from a shape
     static mrInstanceLight* factory( const MDagPath& shapeDagPath, 
                                      mrShape* s );
};


class mrInstanceCamera : public mrInstance
{
     friend class mrSwatchRender;

   protected:
     mrInstanceCamera( const MString& name, mrShape* s );
     mrInstanceCamera( const MDagPath& p, mrShape* s );

   public:
     //! Update instance for new pass (if needed)
     virtual void newRenderPass();

     //! Force update of instance incrementally
     virtual void forceIncremental();

     //! Update instance incrementally (if needed)
     virtual void setIncremental( bool sameFrame );

     //! 
     virtual void write(MRL_FILE* f );

#ifdef GEOSHADER_H
     //! 
     virtual void write();
#endif

     //! Create a new instance from a shape
     static mrInstanceCamera* factory( const MString& name, mrShape* s );

     //! Create a new instance from a shape
     static mrInstanceCamera* factory( const MDagPath& shapeDagPath, 
                                       mrShape* s );
};



char mrInstance::maya_vflag_to_mray( char flag ) const throw()
{     
   switch ( flag )
   {
      case kMayaVOff:
	 return kVOff;
      case kMayaVOn:
	 return kVOn;
      case kMayaVInherit:
	 return kVInherit; 
      default:
	 LOG_ERROR("Unknown maya vflag");
	 return kVInherit;
   }
}




#endif // mrInstance_h
