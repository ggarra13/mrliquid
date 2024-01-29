#ifndef mrUserDataObject_h
#define mrUserDataObject_h

#include "mrUserData.h"


class mrUserDataObject : public mrUserData
{
public:
  static mrUserDataObject* factory( const MDagPath& path );


  virtual void write( MRL_FILE* f );
  virtual void setIncremental( bool sameFrame );
  virtual void forceIncremental();


#ifdef GEOSHADER_H
  virtual void write();
#endif

protected:
  void getData(bool sameFrame);
  mrUserDataObject( const MDagPath& path );

  virtual MObject node() { return path.node(); };

protected:
  MDagPath path;

#if MAYA_API_VERSION >= 650
  float  finalGatherMinRadius, finalGatherMaxRadius;
  float  causticRadius;
  float  globillumRadius;
#endif

#if MAYA_API_VERSION >= 650
  short  finalGatherRays;
  short  finalGatherFilter;
  short  globillumAccuracy;
  short  causticAccuracy;
#endif

  bool overrideFG, finalGatherView;
};


#endif // mrUserDataObject_h

