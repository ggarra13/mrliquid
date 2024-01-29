
#define USE_SOFTWARE_OVERLAYS

#include <maya/MPxContextCommand.h>
#include <maya/MPxContext.h>
#include <maya/M3dView.h>

class mentalCropContext : public MPxContext
{
   public:
     mentalCropContext();
     virtual void	toolOnSetup( MEvent & event );
     virtual MStatus	doPress( MEvent & event );
     virtual MStatus	doDrag( MEvent & event );
     virtual MStatus	doRelease( MEvent & event );
     virtual MStatus	doEnterRegion( MEvent & event );

   protected:
     MStatus changeCameraCrop();
     MStatus changeLightCrop();

     void computeExtents( double window_aspect );

   private:
     short					start_x, start_y;
     short					last_x, last_y;

     double minX, minY, maxX, maxY;

#ifdef USE_SOFTWARE_OVERLAYS
     short					p_last_x, p_last_y;
     bool					fsDrawn;
#endif

     bool light;
     MDagPath                             lightShape;
     M3dView					view;
     static MString name;
};

//////////////////////////////////////////////
// Command to create contexts
//////////////////////////////////////////////

class mentalCropContextCmd : public MPxContextCommand
{
   public:	
     mentalCropContextCmd();
     virtual MPxContext*	makeObj();
     static	void*		creator();
};
