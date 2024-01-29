//
//
//


#include "mrColor.h"

struct maya_material
{
  mr::color outColor;
  mr::color outGlowColor;
  mr::color outMatteOpacity;
  mr::color outTransparency;

  maya_material( const mr::kNoConstruct t ) :
    outColor(t),
    outGlowColor(t),
    outMatteOpacity(t),
    outTransparency(t)
  {};

   maya_material() :
     outColor( 0.0f ),
     outGlowColor( 0.0f ),
     outMatteOpacity( 1.0f ),
     outTransparency( 0.0f )
  {};

  ~maya_material() {};

};

