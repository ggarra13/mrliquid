

#include <maya/MSwatchRenderBase.h>

#include "mrStandalone.h"
#include "mrOptions.h"

class mrInstanceSwatch;
class mrInstanceLight;
class mrInstanceCamera;
class mrShadingGroup;
class mrCamera;


class mrSwatchRender : public MSwatchRenderBase
{
   public:
     static MSwatchRenderBase* factory( MObject obj, MObject renderObj,
					int res );


   public:
     mrSwatchRender(MObject obj, MObject renderObj, int res);
     virtual ~mrSwatchRender();

     virtual bool doIteration();

     static void stop();


     bool doRender( mrCamera* camera );

   protected:
     void write_header();

     bool   written;

     static mrStandalone           ray;

     static MRL_FILE*                    f;
     static mrOptions*            opts;
     static mrInstanceLight* lightInst;
     static mrShadingGroup*         sg;
     static mrInstanceSwatch*  objInst;
     static mrInstanceCamera*  camInst;
};
