#ifndef mrUserDataReferenceObject_h
#define mrUserDataReferenceObject_h

#include "mrUserData.h"


class mrUserDataReferenceObject : public mrUserData
{
public:
  static mrUserDataReferenceObject* factory( const MDagPath& path );


  virtual void write( MRL_FILE* f );
  virtual void setIncremental( bool sameFrame );
  virtual void forceIncremental();


#ifdef GEOSHADER_H
  virtual void write();
#endif

protected:
  void getData(bool sameFrame);
  mrUserDataReferenceObject( const MDagPath& path );

  virtual MObject node() { return path.node(); };

protected:
  MDagPath path;
  int      uv;
};


#endif // mrUserDataReferenceObject_h

