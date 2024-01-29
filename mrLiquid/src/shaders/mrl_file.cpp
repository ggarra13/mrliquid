
#include "mrGenerics.h"
#include "mrRayDifferentials.h"
#include "mrRman.h"

using namespace rsl;
using namespace mr;

struct mrl_file_result_t
{
     color    outColor;
     miScalar outAlpha;
     vector   outSize;
     miBoolean fileHasAlpha;
     color     outTransparency;
};

struct mrl_file_t
{
     miTag fileTextureName;
     miInteger filterType;
     miScalar  filterWidth;
     miBoolean useMaximumRes;
     // inherited from texture2d
     miVector uvCoord;
     miVector uvFilterSize;
     miScalar filter;
     miScalar filterOffset;
     miBoolean invert;
     miBoolean alphaIsLuminance;
     miColor colorGain;
     miColor colorOffset;
     miScalar alphaGain;
     miScalar alphaOffset;
     miColor defaultColor;
     // new in maya4.5
     miBoolean fileHasAlpha;
     // new in maya6.5
     miBoolean disableFileLoad;
     miVector  coverage;
     miVector  translateFrame;
     miScalar  rotateFrame;
     miBoolean doTransform;
     miBoolean mirrorU;
     miBoolean mirrorV;
     miBoolean stagger;
     miBoolean wrapU;
     miBoolean wrapV;
     miVector  repeatUV;
     miVector  offset;
     miScalar  rotateUV;
     miVector  noiseUV;
     miBoolean blurPixelation;
     miVector  vertexUvOne;
     miVector  vertexUvTwo;
     miVector  vertexUvThree;
};


extern "C" {

enum FilterTypes
{
kNone,
kMipMap,
kBox,
kQuadratic,
kQuartic,
kGaussian
};

static float lodBias = 0.0f;
static float invLog2 = (float) (1. / log(2.));
#define LOG2(x) (log(x)*invLog2)

#undef miIMG_ACCESS
#define miIMG_ACCESS( img, y, comp ) \
   mi_img_access( const_cast<miImg_image*>(img), y, comp );


  void getFourTexels( color c[4],
		      const miImg_image* const img, const miScalar p[2] )
  {
    // Note that here (and elsewhere), we assume that texture maps repeat;
    // if we wanted to (say) clamp at the edges, this and other code would
    // need to be generalized.
    int u0 = (int) math<miScalar>::fmod(p[0],   (miScalar)img->width);
    int u1 = (int) math<miScalar>::fmod(p[0]+1, (miScalar)img->width);
    int v0 = (int) math<miScalar>::fmod(p[1],   (miScalar)img->height);
    int v1 = (int) math<miScalar>::fmod(p[1]+1, (miScalar)img->height);

    miUchar* r, *g, *b, *a;

    r = miIMG_ACCESS( img, v0, miIMG_R );
    g = miIMG_ACCESS( img, v0, miIMG_G );
    b = miIMG_ACCESS( img, v0, miIMG_B );
    a = miIMG_ACCESS( img, v0, miIMG_A );

    c[0].r = (r[u0] / 255.f);
    c[0].g = (g[u0] / 255.f);
    c[0].b = (b[u0] / 255.f);
    c[0].a = (a[u0] / 255.f);

    c[1].r = (r[u1] / 255.f);
    c[1].g = (g[u1] / 255.f);
    c[1].b = (b[u1] / 255.f);
    c[1].a = (a[u1] / 255.f);

    r = miIMG_ACCESS( img, v1, miIMG_R );
    g = miIMG_ACCESS( img, v1, miIMG_G );
    b = miIMG_ACCESS( img, v1, miIMG_B );
    a = miIMG_ACCESS( img, v1, miIMG_A );

    c[2].r = (r[u0] / 255.f);
    c[2].g = (g[u0] / 255.f);
    c[2].b = (b[u0] / 255.f);
    c[2].a = (a[u0] / 255.f);

    c[3].r = (r[u1] / 255.f);
    c[3].g = (g[u1] / 255.f);
    c[3].b = (b[u1] / 255.f);
    c[3].a = (a[u1] / 255.f);
  }

DLLEXPORT int mrl_file_version() { return 5; }

DLLEXPORT miBoolean mrl_file( mrl_file_result_t* const m,
			      miState* const state,
			      const mrl_file_t* p )
{
   m->outColor = *mi_eval_color( &p->colorOffset );
   m->outAlpha = m->outColor.a = *mi_eval_scalar( &p->alphaOffset );
   m->fileHasAlpha = *mi_eval_boolean( &p->fileHasAlpha );

   miTag fileTag = *mi_eval_tag( &p->fileTextureName );
   miVector* st = mi_eval_vector( &p->uvCoord );

   miColor*  defaultColor;
   miVector* coverage = mi_eval_vector( &p->coverage );
   if ( coverage->x < 1.0f || coverage->y < 1.0f )
   {
      defaultColor = mi_eval_color( &p->defaultColor );
   }


   if ( fileTag != miNULLTAG )
     {
       miImg_image* img = (miImg_image*) mi_db_access( fileTag );

//        mr_info( "levels: " << img->dirsize );
//        mr_info( "width: " << img->width );
//        mr_info( "height: " << img->height );
//        mr_info( "bits: " << img->bits );
//        mr_info( "comp: " << img->comp );
//        mr_info( "local: " << (bool)img->local );
//        mr_info( "cacheable: " << (bool)img->cacheable );
//        mr_info( "type: " << img->type );


//        miVector* repeatUV = mi_eval_vector( &p->repeatUV );
//        miVector* offset   = mi_eval_vector( &p->offset );
//        miVector* rotateUV = mi_eval_vector( &p->rotateUV );
//        miVector* noiseUV  = mi_eval_vector( &p->noiseUV );

       miVector* t[3];
       t[0] = mi_eval_vector( &p->vertexUvOne );
       t[1] = mi_eval_vector( &p->vertexUvTwo );
       t[2] = mi_eval_vector( &p->vertexUvThree );

       //
       // Calculate dS/dx, dS/dy, dT/dx, dT/dy
       //

       vector dS(kNoInit), dT( kNoInit );
       rayDifferentials( dS, dT, state, t );

       //
       // Calculate minification level
       //
       miScalar j = max(dS.lengthSquared(), dT.lengthSquared(), 1e-6f);

       //
       // Calculate lod level
       //
       miScalar lod = -0.5f * LOG2(j);

       lod -= lodBias;
       if ( lod < 0 ) 
	 {
	   mi_fatal("lod < 0");
	   lod = 0.0f;
	 }

       if ( lod >= img->dirsize )
	 {
	   lod = (float) (img->dirsize - 2);
	 }

       lod = (1.0f - img->dirsize / lod) * img->dirsize;

       color l0( kNoInit ), l1( kNoInit );

       int lod0 = (int)lod;
       int lod1 = lod0 + 1;

       // lod 0
       miImg_image* img0 = img; //mi_img_pyramid_get_level( img, (int) lod0 );

       miScalar p[2];
       p[0] = st->x * (img0->width -1);
       p[1] = st->y * (img0->height-1);

       color c[4];
       getFourTexels(c, img0, p);

       miScalar du = p[0] - (int) p[0];
       miScalar dv = p[1] - (int) p[1];

       l0 = mix( mix( c[0], c[1], du ),
		 mix( c[2], c[3], du ), dv );
       l0.a = mix( mix( c[0].a, c[1].a, du ),
		   mix( c[2].a, c[3].a, du ), dv );

       m->outColor |= c[0];

//        // lod 1
//        miImg_image* img1 = mi_img_pyramid_get_level( img, (int) lod1 );

//        p[0] = st->x * (img1->width -1);
//        p[1] = st->y * (img1->height-1);

//        getFourTexels(c, img1, p);

//        du = p[0] - (int) p[0];
//        dv = p[1] - (int) p[1];
//        l1 = mix( mix( c[0], c[1], du ),
// 		 mix( c[2], c[3], du ), dv );
//        l1.a = mix( mix( c[0].a, c[1].a, du ),
// 		   mix( c[2].a, c[3].a, du ), dv );

//        miScalar frac = lod - lod0;
//        m->outColor   = mix( l0,   l1,   frac );
//        m->outColor.a = mix( l0.a, l1.a, frac );

       mi_db_unpin( fileTag );
     }


   miBoolean alphaIsLuminance = *mi_eval_boolean( &p->alphaIsLuminance );
   if ( m->fileHasAlpha || alphaIsLuminance )
   {
      miScalar  alphaGain = *mi_eval_scalar( &p->alphaGain );
      if ( alphaIsLuminance )
      {
	 miScalar lum   = (m->outColor.r + m->outColor.g + 
			   m->outColor.b) / 3.0f;
	 lum *= alphaGain;
	 m->outAlpha   += lum;
	 m->outColor.a += lum;
      }
   }


   return miTRUE;
}


} // extern "C"

