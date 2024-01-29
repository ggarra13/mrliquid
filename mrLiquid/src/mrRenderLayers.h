
#ifdef MAYA_API_VERSION >= 650
#include "maya/MObjectHandle.h"
#else
#include "maya/MObject.h"
#endif


/**
 * Class used to deal with render layers (passes).
 * This encapsulates code previously in mrTranslator.
 * 
 */
class mrRenderLayers
{
   public:
#if MAYA_API_VERSION >= 650
     typedef std::map< short, MObject >       mrRenderPassList;
#else
     typedef std::map< short, MObjectHandle > mrRenderPassList;
#endif

   public:
     mrRenderLayers();
     mrRenderLayers( const MStringArray& validPassList );
     ~mrRenderLayers();

     void getRenderLayers();
     void count() { return renderPasses.size(); };

     mrRenderPassList::iterator begin();
     mrRenderPassList::iterator end();


   protected:
     MStringArray      validPasses;
     mrRenderPassList  renderPasses;
};
