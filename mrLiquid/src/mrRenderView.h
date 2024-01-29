

class mrTranslator;

//
// Routines to deal with maya's Render View
//
namespace mrRenderView
{

#ifdef MI_RELAY
//! Callback for raylib displaying
void raylib_cb(void* arg, miTag image, int xl, int yl, int xh, int yh);
#endif

//! Feed render view from a file descriptor to ray's stand-alone
//! output.
bool imgpipe_reader(mrTranslator* t);


struct stubData
{
     float gamma;
     float exposure;
};

//! Feed render view from an .iff stub or .iff file.
bool iffstub_reader( stubData* stub );


};


namespace mr3dView
{
//! Feed current 3d view from a file descriptor to ray's stand-alone
//! output.
bool imgpipe_reader(mrTranslator* t);
};
