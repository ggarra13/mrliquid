#ifndef mrUserData_h
#define mrUserData_h

#include <vector>

#include <maya/MObjectHandle.h>

#include "mrNode.h"
#include "mrAttrAux.h"



class mrUserData : public mrNode
{
public:
  static mrUserData* factory( const MFnDependencyNode& fn );

  virtual void write( MRL_FILE* f );

  
protected:
  mrUserData( const MFnDependencyNode& fn );
  mrUserData( const MDagPath& path ) {};

  bool valid_hex( char c );
  char hex(char c);

  template< typename T >
  void get_attr( MStatus& status,
		 MFnDagNode& fn, MPlug& p,
		 T& var, MString attr );
  
protected:
#if MAYA_VERSION >= 600
  virtual MObject node() { return handle.object(); };
  MObjectHandle handle;
#else
  virtual MObject node() { return handle; };
  MObject handle;
#endif

public:
  bool valid;


#ifdef GEOSHADER_H
public:
  virtual void write();

  miTag tag;

protected:
  miTag function;
#endif
};


template< typename T >
void mrUserData::get_attr( MStatus& status,
			   MFnDagNode& fn, MPlug& p,
			   T& var, MString attr ) 
{ 
  T old = var;
  _GET( var, attr );
  if ( written == kWritten && old != var ) written = kIncremental;
}


typedef std::vector< mrUserData* > mrUserDataList;

#endif // mrUserData_h

