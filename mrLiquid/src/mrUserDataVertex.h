#ifndef mrUserDataVertex_h
#define mrUserDataVertex_h

#include <vector>
#include "mrUserData.h"


struct mrVertexData
{
  enum AttrType
    {
      kIntArray = 1,
      kDoubleArray,
      kVectorArray,
      kPointArray
    };

  mrVertexData() {};
     
  mrVertexData( const mrVertexData& b ) :
    attr( b.attr ),
    type( b.type ),
    size( b.size ),
    offset( b.offset )
  {
  }
  
  MString attr;
  char    type;
  char    size;
  unsigned short offset;
};

typedef std::vector< mrVertexData > mrUserVectors;


class mrUserDataVertex : public mrUserData
{
public:
  mrUserVectors userAttrs;

  static mrUserDataVertex* factory( const MDagPath& path );

  void write_trilist( MRL_FILE* f, unsigned idx );
  unsigned write_user( MRL_FILE* f );

  virtual void write( MRL_FILE* f );
  virtual void setIncremental( bool sameFrame );
  virtual void forceIncremental();

  void clear() { offset = 1; userAttrs.clear(); valid = false; }
  void add( const MString& attrName, const MObject& attrObj, 
	    const unsigned numVerts );


#ifdef GEOSHADER_H

  void write_trilist( std::vector< miScalar >& scalars, 
		      const unsigned idx );
  void     write_trilist( std::vector< miVector >& vectors, 
			  const unsigned idx );
  unsigned write_user();
  virtual void write();
#endif

protected:
  void getData(bool sameFrame);
  mrUserDataVertex( const MDagPath& path );

  virtual MObject node() { return path.node(); };

protected:
  MDagPath path;
  unsigned offset;
};


#endif // mrUserDataVertex_h
