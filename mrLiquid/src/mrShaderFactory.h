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

#ifndef mrShaderFactory_h
#define mrShaderFactory_h

#include <vector>
#include <map>
#include <limits>
#include <cassert>

#include "maya/MPxNode.h"
#include "maya/MColor.h"
#include "maya/MVector.h"
#include "maya/MString.h"
#include "maya/MStringArray.h"

#include "mrDebug.h"

#ifndef mrFactoryCmd_h
#include "mrFactoryCmd.h"
#endif


#undef min
#undef max

class MFnCompoundAttribute;

//! Hidden (never created) node from which all shader nodes are derived from 
class mentalrayShaderNode : public MPxNode
{
public:
  mentalrayShaderNode();
  virtual           ~mentalrayShaderNode();

  virtual void      postConstructor();
     
  static void *     creator();
  static MStatus    initialize();

  static MObject miFactoryNode;
  static MObject miForwardDefinition;
};


//! Base class for all shader parameters found in .mi file, used to
//! create a corresponding maya attribute in a node.
struct mrShaderParameter
{
  void setOutput();
  void setName( const char* s ) { name = s; };
  void setHelp( MString s )     { s += "\n"; help += s; }
  void setHidden( bool t )      { hidden = t; }

  virtual void setShortName();
  virtual void setShortName( const char* s ) { shortname = s; };


  virtual mrFactoryShaderParameter::Type getType() const
  {
    return mrFactoryShaderParameter::kUnknown;
  }
     
  virtual const MObject createAttribute( bool output, bool array ) const 
  {
    return MObject::kNullObj;
  };

  virtual const MObject addAttribute( bool output, bool array ) const 
  {
    return MObject::kNullObj;
  };

  virtual void setDefault( double a, double b, double c, double d,
			   double e, double f, double g, double h,
			   double i, double j, double k, double l,
			   double m, double n, double o, double p ) {};
     
#define MRL_SET(T) \
     virtual void set##T( int x ) {}; \
     virtual void set##T( double x ) {}; \
     virtual void set##T( double x, double y, double z ) {}; \
     virtual void set##T( double r, double g, double b, double a ) {};

  MRL_SET(SoftMax);
  MRL_SET(SoftMin);
  MRL_SET(Max);
  MRL_SET(Min);
  MRL_SET(Default);
#undef MRL_SET

  virtual void setEnum( const char* ) {};


  mrShaderParameter() : next(NULL), name("value"), hidden(false)
  {
  };
  mrShaderParameter( const MString& n ) : next(NULL), name(n), hidden(false)
  {
  };
  virtual ~mrShaderParameter()
  {
    delete next;
  }

  mrShaderParameter* next;
  MString name;
  MString shortname;
  MString help;
  bool    hidden;
};


//! A boolean attribute obtained from the .mi file
//! create a corresponding maya attribute in a node.
struct mrShaderBooleanParameter : public mrShaderParameter
{
#define MRL_SET(T) \
     bool T; \
     virtual void set##T( int x ) { T = ( x != 0 ); }; \
     virtual void set##T( double x ) { T = ( x != 0 ); }; \
     virtual void set##T( double x, double y, double z ) {}; \
     virtual void set##T( double r, double g, double b, double a ) {};

  MRL_SET(Default);
#undef MRL_SET
     
  virtual mrFactoryShaderParameter::Type getType() const
  {
    return mrFactoryShaderParameter::kBoolean;
  }
  virtual const MObject createAttribute( bool output, bool array ) const;
  virtual const MObject addAttribute( bool output, bool array ) const;
     
  mrShaderBooleanParameter() :
    mrShaderParameter(), Default(false)
  {};
};


//! An integer parameter obtained from the .mi file
//! create a corresponding maya attribute in a node.
struct mrShaderIntegerParameter : public mrShaderParameter
{
#define MRL_SET(T) \
     int T; \
     virtual void set##T( int x ) { T = x; }; \
     virtual void set##T( double x ) { T = (int)x; }; \
     virtual void set##T( double x, double y, double z ) {}; \
     virtual void set##T( double r, double g, double b, double a ) {};

  MRL_SET(Default);
  MRL_SET(SoftMin);
  MRL_SET(SoftMax);
  MRL_SET(Min);
  MRL_SET(Max);
#undef MRL_SET

  MString Enum;
  virtual void setEnum( const char* x ) { Enum = x; };

  virtual const MObject createAttribute( bool output, bool array ) const;
  virtual const MObject addAttribute( bool output, bool array ) const;
     
  virtual mrFactoryShaderParameter::Type getType() const
  {
    return mrFactoryShaderParameter::kInteger;
  }
     
  mrShaderIntegerParameter() :
    mrShaderParameter(),
    Default(0),
    SoftMin( std::numeric_limits<int>::min() ),
    SoftMax( std::numeric_limits<int>::max() ),
    Min( std::numeric_limits<int>::min() ),
    Max( std::numeric_limits<int>::max() )
  {};
};


//! A scalar (float) parameter obtained from the .mi file
//! create a corresponding maya attribute in a node.
struct mrShaderScalarParameter : public mrShaderParameter
{
#define MRL_SET(T) \
     double T; \
     virtual void set##T( int x ) { T = (double)x; }; \
     virtual void set##T( double x ) { T = x; }; \
     virtual void set##T( double x, double y, double z ) {}; \
     virtual void set##T( double r, double g, double b, double a ) {};
     
  MRL_SET(Default);
  MRL_SET(SoftMin);
  MRL_SET(SoftMax);
  MRL_SET(Min);
  MRL_SET(Max);
#undef MRL_SET
  virtual mrFactoryShaderParameter::Type getType() const
  {
    return mrFactoryShaderParameter::kScalar;
  }
  virtual const MObject createAttribute( bool output, bool array ) const;
  virtual const MObject addAttribute( bool output, bool array ) const;
     
  mrShaderScalarParameter() :
    mrShaderParameter(),
    Default(0),
    SoftMin( std::numeric_limits<double>::min() ),
    SoftMax( std::numeric_limits<double>::max() ),
    Min( std::numeric_limits<double>::min() ),
    Max( std::numeric_limits<double>::max() )
  {};
};


//! A vector parameter obtained from the .mi file
//! create a corresponding maya attribute in a node.
struct mrShaderVectorParameter : public mrShaderParameter
{
     
#define MRL_SET(T) \
     MVector T; \
     virtual void set##T( int x ) { T.x = T.y = T.z = (double) x; }; \
     virtual void set##T( double x ) { T.x = T.y = T.z = x; }; \
     virtual void set##T( double x, double y, double z ) { \
	T.x = x; T.y = y; T.z = z; \
     }; \
     virtual void set##T( double r, double g, double b, double a ) { \
	T.x = r; T.y = g; T.z = b; \
     };
     
  MRL_SET(Default);
  MRL_SET(Min);
  MRL_SET(Max);
#undef MRL_SET
  virtual mrFactoryShaderParameter::Type getType() const
  {
    return mrFactoryShaderParameter::kVector;
  }
  virtual const MObject createAttribute( bool output, bool array ) const;
  virtual const MObject addAttribute( bool output, bool array ) const;
     
  mrShaderVectorParameter() :
    mrShaderParameter("vector"),
    Default( MVector(0,0,0 ) ),
    Min( MVector(
		 std::numeric_limits<double>::min(),
		 std::numeric_limits<double>::min(),
		 std::numeric_limits<double>::min()
		 )
	 ),
    Max( MVector(
		 std::numeric_limits<double>::max(),
		 std::numeric_limits<double>::max(),
		 std::numeric_limits<double>::max()
		 )
	 )
  {};
};


//! A color parameter obtained from the .mi file
//! create a corresponding maya attribute in a node.
struct mrShaderColorParameter : public mrShaderParameter
{
     
#define MRL_SET(T) \
     MColor T; \
     virtual void set##T( int x ) { T.r = T.g = T.b = (float) x; }; \
     virtual void set##T( double x ) { T.r = T.g = T.b = (float)x; }; \
     virtual void set##T( double x, double y, double z ) { \
	T.r = (float)x; T.g = (float)y; T.b = (float)z; T.a = 1; \
     }; \
     virtual void set##T( double r, double g, double b, double a ) { \
	T.r = (float)r; T.g = (float)g; T.b = (float)b; \
     };
     
  MRL_SET(Default);
  MRL_SET(SoftMin);
  MRL_SET(SoftMax);
  MRL_SET(Min);
  MRL_SET(Max);
#undef MRL_SET
     
  virtual mrFactoryShaderParameter::Type getType() const
  {
    return mrFactoryShaderParameter::kColor;
  }
  const MObject createAlphaAttribute( bool output, bool array ) const;
  virtual const MObject createAttribute( bool output, bool array ) const;
  virtual const MObject addAttribute( bool output, bool array ) const;
     
  mrShaderColorParameter() : 
    mrShaderParameter("color"),
    Default( MColor(0,0,0,0) ),
    SoftMin( MColor(0,0,0,0) ),
    SoftMax( MColor(1,1,1,1) ),
    Min( MColor(
		std::numeric_limits<float>::min(),
		std::numeric_limits<float>::min(),
		std::numeric_limits<float>::min(),
		std::numeric_limits<float>::min()
		)
	 ),
    Max( MColor(
		std::numeric_limits<float>::max(),
		std::numeric_limits<float>::max(),
		std::numeric_limits<float>::max(),
		std::numeric_limits<float>::max()
		)
	 )
  {};
};



#define MR_CREATE_PARAMETER( x ) \
struct mrShader ## x ## Parameter : public mrShaderParameter \
{ \
     virtual mrFactoryShaderParameter::Type getType() const \
     { \
	return mrFactoryShaderParameter::k ## x; \
     } \
     virtual const MObject createAttribute( bool output, bool array ) const; \
     virtual const MObject addAttribute( bool output, bool array ) const; \
     mrShader## x ##Parameter() : mrShaderParameter() {}; \
     mrShader## x ##Parameter( const MString& n ) : mrShaderParameter(n) \
     {}; \
};

//! A string parameter obtained from the .mi file
//! create a corresponding maya attribute in a node.
MR_CREATE_PARAMETER( String );

//! A transform (matrix) parameter obtained from the .mi file
//! create a corresponding maya attribute in a node.
struct mrShaderTransformParameter : public mrShaderParameter
{
  MMatrix Default;
  virtual void setDefault( double a, double b, double c, double d,
			   double e, double f, double g, double h,
			   double i, double j, double k, double l,
			   double m, double n, double o, double p )
  {
    Default[0][0] = a; Default[0][1] = b; Default[0][2] = c; Default[0][3] = d;
    Default[1][0] = e; Default[1][1] = f; Default[1][2] = g; Default[1][3] = h;
    Default[2][0] = i; Default[2][1] = j; Default[2][2] = k; Default[2][3] = l;
    Default[3][0] = m; Default[3][1] = n; Default[3][2] = o; Default[3][3] = p;
  }
  virtual mrFactoryShaderParameter::Type getType() const
  {
    return mrFactoryShaderParameter::kTransform;
  }
  virtual const MObject createAttribute( bool output, bool array ) const;
  virtual const MObject addAttribute( bool output, bool array ) const;
  mrShaderTransformParameter() : 
    mrShaderParameter(), 
    Default( MMatrix::identity )
  {
  };
  mrShaderTransformParameter( const MString& n ) : 
    mrShaderParameter(n), 
    Default( MMatrix::identity )
  {
  };
};

//! A base class for all message parameters.
struct mrShaderMessageParameter : public mrShaderParameter
{
  virtual const MObject createAttribute( bool output, bool array ) const;
  virtual const MObject addAttribute( bool output, bool array ) const;
  mrShaderMessageParameter() : mrShaderParameter() {};
  mrShaderMessageParameter( const MString& n ) : mrShaderParameter(n)
  {};
};

//! A class for all data parameters (turned into strings).
struct mrShaderDataParameter : public mrShaderStringParameter
{
  virtual mrFactoryShaderParameter::Type getType() const
  {
    return mrFactoryShaderParameter::kData;
  }
  mrShaderDataParameter( const char* n ) : mrShaderStringParameter(n) {}
};

//! A base class for all texture parameters (turned into msg parameter).
struct mrShaderTextureParameter : public mrShaderMessageParameter
{
  virtual void setShortName();
};

//! A class for all color texture parameters
struct mrShaderColorTextureParameter : public mrShaderTextureParameter
{
  virtual mrFactoryShaderParameter::Type getType() const
  {
    return mrFactoryShaderParameter::kColorTexture;
  }
};

//! A class for all scalar texture parameters (turned into strings).
struct mrShaderScalarTextureParameter : public mrShaderTextureParameter
{
  virtual mrFactoryShaderParameter::Type getType() const
  {
    return mrFactoryShaderParameter::kScalarTexture;
  }
};

//! A class for all vector texture parameters (turned into strings).
struct mrShaderVectorTextureParameter : public mrShaderTextureParameter
{
  virtual mrFactoryShaderParameter::Type getType() const
  {
    return mrFactoryShaderParameter::kVectorTexture;
  }
};

//! A class for all lightprofile parameters (turned into msg. params).
struct mrShaderLightProfileParameter : public mrShaderMessageParameter
{
  virtual mrFactoryShaderParameter::Type getType() const
  {
    return mrFactoryShaderParameter::kLightProfile;
  }
};

//! A class for all material parameters (turned into msg. params).
//! Note: maya does not use material parameters (only soft3d?)
struct mrShaderMaterialParameter : public mrShaderMessageParameter
{
  mrShaderMaterialParameter() : mrShaderMessageParameter("material") {}
  virtual mrFactoryShaderParameter::Type getType() const
  {
    return mrFactoryShaderParameter::kMaterial;
  }
};

//! A class for all shader parameters (turned into msg. params).
struct mrShaderShaderParameter : public mrShaderMessageParameter
{
  mrShaderShaderParameter( const char* n ) : mrShaderMessageParameter(n) {}
  virtual mrFactoryShaderParameter::Type getType() const
  {
    return mrFactoryShaderParameter::kShader;
  }
};

//! A class for all geometry parameters (turned into msg. params).
struct mrShaderGeometryParameter : public mrShaderMessageParameter
{
  mrShaderGeometryParameter() : mrShaderMessageParameter("geometry") {}
  virtual mrFactoryShaderParameter::Type getType() const
  {
    return mrFactoryShaderParameter::kGeometry;
  }
};

//! A class for all light parameters (turned into msg. params).
struct mrShaderLightParameter : public mrShaderMessageParameter
{
  mrShaderLightParameter() : mrShaderMessageParameter("light") {}
  virtual const MObject createAttribute( bool output, bool array ) const;
  virtual const MObject addAttribute( bool output, bool array ) const;
  virtual mrFactoryShaderParameter::Type getType() const
  {
    return mrFactoryShaderParameter::kLight;
  }
};



#undef MR_CREATE_PARAMETER

typedef std::vector< mrShaderParameter* > mrShaderParameterList;


//! A base class for all parameters containing others
struct mrShaderParameterContainer : public mrShaderParameter
{
  const mrShaderParameter* child;
     
  mrShaderParameterContainer( const MString& n,
			      const mrShaderParameter* c ) :
    mrShaderParameter(n), child(c)
  {};
  virtual ~mrShaderParameterContainer()
  {
    delete child;
  }
};


//! A class for struct parameters
struct mrShaderStructParameter : public mrShaderParameterContainer
{
  mrShaderStructParameter( const MString& n,
			   const mrShaderParameter* c ) :
    mrShaderParameterContainer(n, c)
  {};

  virtual const MObject addAttribute( bool output, bool array ) const;

protected:
  MObject createAttribute( MFnCompoundAttribute& attr, bool output, bool array ) const;
};


//! A class for array parameters
struct mrShaderArrayParameter : public mrShaderParameterContainer
{
  mrShaderArrayParameter( const mrShaderParameter* c ) :
    mrShaderParameterContainer("array", c)
  {};
};


namespace mrShaderFactory
{
  extern int idx;  // used for creating shortname indices

  struct mrShaderManagerData
  {
    MString file;
    MString path;
    MString version;
    MString vendor;
     
    MStringArray            shaders;

    typedef std::vector< unsigned > NodeIds;
    NodeIds nodeids;
  };

  typedef std::vector< mrShaderManagerData* > mrShaderManagerDataList;
  extern mrShaderManagerDataList data;
}



struct mrShaderDeclaration : public mrShaderParameter
{
  bool     phenomenon;
  unsigned nodeid;
  MString  type;
  MString  icon;

  const mrShaderParameter*  output;
  const mrShaderParameter*  param;

  void setIcon( const char* c ) { icon = c; };
  void setClassification( const char* c ) { type = c; };
     
  virtual ~mrShaderDeclaration()
  {
    delete output;
    delete param;
  }
     
  mrShaderDeclaration( const mrShaderParameter* o,
		       const MString n,
		       const mrShaderParameter* p,
		       const bool ph = false ) :
    mrShaderParameter(n),
    phenomenon(ph),
    nodeid(0),
    type("misc"),
    output(o),
    param(p)
  {
     assert( o != NULL );
  };

};

namespace mrShaderFactory
{
  //! Scan all MI_* directories looking for all .mi file descriptions
  void    scanDirectories( const MObject& o );

  //! Return a string array with all the the shader libraries 
  MStringArray getShaderLibraries();

  //! Return a string array with all the shaders in the mifile 
  MStringArray getShadersInLibrary( const MString& mifile );

  //! Return the name of the vendor of the mi file
  MString  getVersionInLibrary( const MString& mifile );

  //! Return the name of the vendor of the mi file
  MString  getVendorOfLibrary( const MString& mifile );

  //! Deregister all shaders that mrLiquid created
  void deregisterAllShaders( MObject& o );  // cannot be const MObject&

  //! Register a shader based on a shader declaration gathered from an mi file
  MStatus registerShader( const mrShaderDeclaration* shader );

  //! If found, parse mi file and extract shaders
  MStatus registerShaderLibrary( const MString& mifileFullPath );

  //! Find a shader library based on its mi file.  NULL if not found
  mrShaderManagerData* findShaderLibrary( const MString& miFile );

  //! Find a shader library based on its mi file and deregister all its shaders
  MStatus deregisterShaderLibrary( const MString& miFile );

  //! Find .mi file where a particular shader is found.
  //! Returns "" if not found.
  MString findMI( const MString& shader );

  //! Find DSO/DLL file where a particular shader is found.
  //! Returns "" if not found.
  MString findDSO( const MString& shader );

  //! Make sure MI_CUSTOM_SHADER_PATH contains MAYA_LOCATION/mentalray/include
  void setCustomShaderPath();

  //! Return a new shader nodeid (autogenerated)
  unsigned newShaderId();
}


#endif

