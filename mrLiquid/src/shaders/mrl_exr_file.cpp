/**
 * @file   mrl_exr_file.cpp
 * @author 
 * @date   Mon Sep 04 12:26:40 2006
 * 
 * @brief  Shader to read ripmap exr files and mimic maya_file's functionality.
 * 
 * 
 */

#include <sys/stat.h>

#include <algorithm>

#include "mrGenerics.h"
#include "mrRayDifferentials.h"
#include "mrRman.h"
#include "mrFilters.h"

#ifndef mrTextureStats_h
#  include "mrTextureStats.h"
#endif

#if defined(WIN32) || defined(WIN64)
# define strdup _strdup
# pragma warning( disable: 4231 )
#endif


#include "ImfRgba.h"
#include "ImfArray.h"
#include "ImfTiledRgbaFile.h"
#include "ImfFrameBuffer.h"
#include "ImfStringAttribute.h"

// #define MR_ELLIPTIC_FILTER

using namespace rsl;
using namespace mr;


#if !defined(WIN32) && !defined(WIN64)
#undef  DLLEXPORT
#define DLLEXPORT __attribute__((visibility("default"))) 
#endif



struct mrl_exr_file_result_t
{
     color     outColor;
     miScalar  outAlpha;
     vector    outSize;
     miBoolean fileHasAlpha;
     color     outTransparency;
     vector    outNormal;
};



struct mrl_exr_file_t
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
  // maya 8.0
  miInteger normalEncoding;
  miInteger normalSpace;
  // maya 8.5
  miBoolean missingAlphaOpaque;
};




struct mrlTextureBlock;

namespace
{
  miLock mMemFlushLock;
  const unsigned maxFileDescriptors = 512;
  miUint accessId = 0;
  miUint memoryUsed = 0;
  miUint memoryMax  = 0;
  miUint numUsedBlocks = 0;
  mrlTextureBlock*  usedBlocks = NULL;
  mrlTextureBlock*  freeBlocks = NULL;
}


/**
 * This struct holds a texture block.
 * 
 */
struct mrlTextureBlock
{
  mrlTextureBlock() :
    access( accessId ),
    next( NULL ),
    size( 0 ),
    data( NULL )
  {
  }

  miUint access;           //<- last access 'time' of texture block
  mrlTextureBlock* next;   //<- next block in list
  unsigned size;           //<- size of the data
  void*    data;           //<- actual texture data
};



/**
 * Functor used to sort texture blocks by last reference number.
 * 
 */
struct latestBlock
{
  inline
  bool operator()( const mrlTextureBlock* a, const mrlTextureBlock* b ) const
  {
    return ( a->access < b->access );
  }
};


/** 
 * Delete a texture block's data
 * 
 * @param block Texture block to delete.
 */
static void textureDeleteBlock(mrlTextureBlock* block)
{
  mrlTextureBlock* p = NULL;
  mrlTextureBlock* c = usedBlocks;
  for ( ; c != NULL; p = c, c = c->next )
    {
      if ( c != block ) continue;

      if ( p )
	{
	  p->next = c->next;
	}
      else
	{
	  usedBlocks = c->next;
	}

      if ( c->data )
	{
	  memoryUsed -= c->size;
	  mi_mem_release( c->data );
	  c->data = NULL;
	}

      --numUsedBlocks;
      c->next = freeBlocks;
      freeBlocks = c;

      return;
    }
}


/** 
 * Flush texture blocks from memory to leave room for new block.
 * 
 * @param entry new block to allocate
 * 
 * @return true if memory was flushed, false if not.
 */
static bool textureMemFlush(mrlTextureBlock* current) 
{
  bool ret = false;
  mrlTextureBlock** blockPtr;
  mrlTextureBlock** blockList;
  mrlTextureBlock* c = usedBlocks;

  ++gStats->textureFlushes;

  mi_lock( mMemFlushLock );

//   // Count the number of used blocks
//   unsigned num = 0;
//   for ( ; c != NULL; c = c->next )
//     {
//       if ( c != current ) ++num;
//     }

  // Allocate a new temporary block list
  blockList = (mrlTextureBlock**) mi_mem_allocate( numUsedBlocks * 
						   sizeof( mrlTextureBlock* ) );
  if ( blockList == NULL ) {
    mi_warning("mrl_exr_file: could not allocate temporary block buffer for texture flush");

    // Flush all the used blocks in panic mode
    while(usedBlocks != NULL)
      textureDeleteBlock(usedBlocks);
    return true;
  }

  // Copy all blocks but current block to it
  mrlTextureBlock** lastBlock = blockList;
  for ( c = usedBlocks; c != NULL; c = c->next )
    {
      *lastBlock = c; ++lastBlock;
    }

  if ( numUsedBlocks > 1 )
    {
      // Sort on temporary block list based on access times
      std::sort( blockList, blockList + numUsedBlocks, latestBlock() );
    }

  // Flush the used blocks until we have enough memory
  bool flushed = false;
  for ( blockPtr = blockList; blockPtr < lastBlock; ++blockPtr )
    {
      if ( *blockPtr == current ) continue;

      flushed = true;
      c = *blockPtr;
      mi_mem_release( c->data );
      c->data = NULL;
      memoryUsed -= c->size;
      
      if ( memoryUsed <= (memoryMax / 2) ) break;
    }
   
  // Release the temporary block list we used for sorting by access times
  mi_mem_release( blockList );

  mi_unlock( mMemFlushLock );

  return flushed;
}


/** 
 * Allocate a new texture block
 * 
 * @param entry 
 */
static void	textureAllocateBlock(mrlTextureBlock* c)
{
  int s = c->size;
  memoryUsed += s;
  gStats->transferredTextureData += s;

  if ( memoryUsed > memoryMax )
    {
      if ( ! textureMemFlush( c ) )
	{
	  mi_warning("mrl_exr_file: Could not flush texture blocks");
	}
    }

  c->data = mi_mem_allocate( s * sizeof( unsigned char ) );
  if ( c->data == NULL )
    {
      mi_error("mrl_exr_file: Could not allocate memory for pixels");
    }

  if ( memoryUsed > gStats->peakTextureMemory )
    gStats->peakTextureMemory = memoryUsed;
}



/** 
 * Load a new EXR texture block
 * 
 * @param entry mrlTextureBlock* to place data into
 * @param name  EXR filename to load block from
 * @param x     tileX number
 * @param y     tileY number
 * @param lx    LOD level X
 * @param ly    LOD level Y
 */
static void	textureLoadBlock(
				 mrlTextureBlock *entry,
				 Imf::TiledRgbaInputFile& in,
				 const int x, const int y,
				 const int lx, const int ly
				 )
{
  // Update the stats
  ++gStats->numTextureMisses;
  

  try 
    {
      mrASSERT( lx < in.numXLevels() );
      mrASSERT( ly < in.numYLevels() );
      
      Imath::Box2i dataWindow = in.dataWindowForLevel(lx, ly);
      
      int dw = dataWindow.max.x - dataWindow.min.x + 1;
      int dh = dataWindow.max.y - dataWindow.min.y + 1;
      int dx = dataWindow.min.x;
      int dy = dataWindow.min.y;
      
      unsigned tileXSize = in.tileXSize();
      unsigned tileYSize = in.tileYSize();


      entry->size = tileXSize * tileYSize * sizeof( Imf::Rgba );

      textureAllocateBlock( entry );

      Imf::Rgba* pixels = static_cast< Imf::Rgba* >( entry->data );
      if ( pixels != NULL ) 
	{
	  unsigned offset = dx + (x + y * tileYSize + dy) * tileXSize;
	  in.setFrameBuffer( pixels - offset, 1, tileXSize );
	  in.readTile( x, y, lx, ly );
	}
    } 
  catch( const std::exception& e )
    {
      mi_error("mrl_exr_file: %s", e.what() );
    }

   entry->access = accessId;
}


/** 
 * Create a new texture block.
 * 
 * @return new mrlTextureBlock*
 */
static mrlTextureBlock*	textureNewBlock() 
{
   mrlTextureBlock* block;
   if (freeBlocks != NULL)
     {
       block = freeBlocks;
       freeBlocks = block->next;
     }
   else
     {
       block = new mrlTextureBlock;
     }

   block->next = usedBlocks;
   usedBlocks  = block;
   ++numUsedBlocks;

   return block;
}


class exrTexture;

typedef std::map< const exrTexture*, Imf::TiledRgbaInputFile* > FileDescriptors;
static FileDescriptors fileDescriptors;


static void textureDeleteFileDescriptor(const exrTexture* exr )
{
  FileDescriptors::iterator i = fileDescriptors.find( exr );
  FileDescriptors::iterator e = fileDescriptors.end();
  if ( i != e )
    {
      delete i->second;
      fileDescriptors.erase( i );
    }
}

static void textureDeleteFileDescriptors()
{
  FileDescriptors::iterator i = fileDescriptors.begin();
  FileDescriptors::iterator e = fileDescriptors.end();
  for ( ; i != e; ++i )
    {
      delete i->second;
    }
  fileDescriptors.clear();
}

static Imf::TiledRgbaInputFile* 
textureGetFileDescriptor( const exrTexture* txt );



class exrTexture
{
protected:
  typedef std::map< unsigned long long, mrlTextureBlock* > DataBlocks;

  enum WrapMode
    {
      kBlack,
      kClamp,
      kPeriodic,
      kMirror
    };

public:
  exrTexture( const char* filename ) :
    _name( strdup( filename ) ),
    _wrapX( kClamp ),
    _wrapY( kClamp )
  {
    gStats->numTextures++;
    if (gStats->numTextures > gStats->numPeakTextures)
      gStats->numPeakTextures = gStats->numTextures;

    Imf::TiledRgbaInputFile& in = *(textureGetFileDescriptor( this ));

    const Imf::Header& h = in.header();
    Imf::Header::ConstIterator i = h.find("wrapmodes");
    if ( i != h.end() )
      {
	const Imf::StringAttribute* a = 
	  dynamic_cast<const Imf::StringAttribute*>( &i.attribute() );

	const char* value = a->value().c_str();
	_wrapX = _wrapY = wrapmode( value );
	const char* s = value + 5;

	for ( ; *s != ','; ++s ) ;

	if ( *s == ',' )
	  {
	    for ( ++s; *s == ' '; ++s ) ;
	    _wrapY = wrapmode( s );
	  }
      }

    _width  = in.levelWidth( 0 );
    _height = in.levelHeight( 0 );
    _tileXSize = in.tileXSize();
    _tileYSize = in.tileYSize();
    _numXLevels = in.numXLevels();
    _numYLevels = in.numYLevels();
  }

  ~exrTexture() 
  {
    DataBlocks::iterator i = _datablocks.begin();
    DataBlocks::iterator e = _datablocks.end();
    for ( ; i != e; ++i )
      {
	textureDeleteBlock( i->second );
      }

    free( _name );
  }

  WrapMode wrapmode( const char* s )
  {
    if ( strncmp( s, "black", 5 ) == 0 )
      return kBlack;
    else if (  strncmp( s, "clamp", 5 ) == 0 )
      return kClamp;
    else if (  strncmp( s, "periodic", 8 ) == 0 )
      return kPeriodic;
    else if (  strncmp( s, "mirror", 6 ) == 0 )
      return kMirror;
    else
      return kClamp;
  }

  void  blur( color& c,
	      const miScalar xf, const miScalar yf,
	      const int lx, const int ly )
  {
    // Calculate pixel coordinate for level
    int dw = _width  / (1 << lx);
    int dh = _height / (1 << ly);
    miScalar ds = xf * dw - 0.5f;
    miScalar dt = yf * dh - 0.5f;

    int  x = mr::fastmath<miScalar>::floor(ds);
    int  y = mr::fastmath<miScalar>::floor(dt);

    ds -= x;
    dt -= y;

    color tmp[4];
    texel( tmp[0], x,   y, lx, ly );
    texel( tmp[1], x,   y+1, lx, ly );
    texel( tmp[2], x+1, y, lx, ly );
    texel( tmp[3], x+1, y+1, lx, ly );

    c = mix( mix( tmp[0], tmp[1], dt ),
	     mix( tmp[2], tmp[3], dt ), ds );
    c.a = mix( mix( tmp[0].a, tmp[1].a, dt ),
	       mix( tmp[2].a, tmp[3].a, dt ), ds );
  }

  void  lookup( color& c, 
		const miScalar xf, const miScalar yf,
		const int lx, const int ly)
  {
    // Calculate pixel coordinate for level
    int dw = _width  / (1 << lx);
    int dh = _height / (1 << ly);
    miScalar ds = xf * dw;  // no -0.5 here
    miScalar dt = yf * dh;
    int  x = mr::fastmath<float>::floor(ds);
    int  y = mr::fastmath<float>::floor(dt);

    texel( c, x, y, lx, ly );
  }

  unsigned numXLevels()  const { return _numXLevels; }
  unsigned numYLevels()  const { return _numYLevels; }

  const char* filename() const { return _name; }

  int width()  const { return _width; }
  int height() const { return _height; }

  void mirrorU() { _wrapX = kMirror; }
  void mirrorV() { _wrapY = kMirror; }

  void wrapU() { _wrapX = kPeriodic; }
  void wrapV() { _wrapY = kPeriodic; }


protected:
  void texel( color& c, int x, int y,
	      const int lx, const int ly )
  {   
    ++accessId;
    ++(gStats->numTextureAccesses);

    // Handle wrapping modes
    int dw = _width  / (1 << lx);
    int dh = _height / (1 << ly);

    switch( _wrapX )
      {
      case kBlack:
	if ( x < 0 || x >= dw ) 
	  {
	    c = 0; return;
	  }
	break;
      case kClamp:
	if ( x < 0 ) x = 0;
	else if ( x >= dw ) x = dw - 1;
	break;
      case kPeriodic:
	x = mr::mod( x, dw );
	break;
      case kMirror:
	if ( x < 0 )        x = abs(x);
	else if ( x >= dw ) x = dw - (x - dw) - 1;
	break;
      default:
	mi_fatal("Unknown wrap mode for U");
      }

    switch( _wrapY )
      {
      case kBlack:
	if ( y < 0 || y >= dh ) 
	  {
	    c = 0; return;
	  }
	break;
      case kClamp:
	if ( y < 0 ) y = 0;
	else if ( y >= dh ) y = dh - 1;
	break;
      case kPeriodic:
	y = mr::mod( y, dh );
	break;
      case kMirror:
	if ( y < 0 )        y = abs(y);
	else if ( y >= dh ) y = dh - (y - dh) - 1;
	break;
      default:
	mi_fatal("Unknown wrap mode for V");
      }


    // Calculate tileX and tileY value
    int tileX = x / _tileXSize;
    int tileY = y / _tileYSize;

    unsigned long long key = hash_key( tileX, tileY, lx, ly );
    DataBlocks::iterator e = _datablocks.end();
    DataBlocks::iterator i = _datablocks.find( key );

    mrlTextureBlock* block;
    if ( i == e )
      {
	// block not created
	block = textureNewBlock();
	_datablocks.insert( std::make_pair( key, block ) );
      }
    else
      {
	block = i->second;
      }

    if ( block->data == NULL )
      {
	// block not loaded
	Imf::TiledRgbaInputFile* in = textureGetFileDescriptor( this );
	if ( in == NULL ) return;

	textureLoadBlock( block, *in, tileX, tileY, lx, ly );
      }

    const Imf::Rgba* pixels = (Imf::Rgba*) block->data;
    if ( pixels == NULL ) 
      {
	c.r = c.g = c.b = c.a = 0.0f;
	return;
      }

    x -= tileX * _tileXSize;
    y -= tileY * _tileYSize;
    const Imf::Rgba& rgba = pixels[ y * _tileXSize + x ];
    c.r = rgba.r;
    c.g = rgba.g;
    c.b = rgba.b;
    c.a = rgba.a;
  }

  unsigned long long hash_key( const int x, const int y, 
			       const int lx, const int ly )
  {
    // Max. resolution accepted is 32768 x 32768 and 16 levels in X/Y.
    mrASSERT( lx < 16 );
    mrASSERT( ly < 16 );
    mrASSERT( x < 32768 );
    mrASSERT( y < 32768 );
    unsigned long long key = (lx + (ly << 4) + (x << 8) + (y << 23));
    return key;
  }

  DataBlocks _datablocks;
  char* _name;
  int _width, _height;
  unsigned _tileXSize, _tileYSize;
  unsigned _numXLevels, _numYLevels;
  WrapMode _wrapX, _wrapY;
};


static
Imf::TiledRgbaInputFile* textureGetFileDescriptor( const exrTexture* txt )
{
  FileDescriptors::iterator i = fileDescriptors.find( txt );
  if ( i == fileDescriptors.end() )
    {
      if ( fileDescriptors.size() == maxFileDescriptors )
	{
	  i = fileDescriptors.begin();
	  delete i->second;
	  fileDescriptors.erase(i);
	}

      Imf::TiledRgbaInputFile* ret;
      ret = new Imf::TiledRgbaInputFile( txt->filename() );
      fileDescriptors.insert( std::make_pair( txt, ret ) );
      return ret;
    }
  else
    {
      return i->second;
    }
}


static void textureShutdown()
{
  accessId = 0;

  while(usedBlocks != NULL)
    textureDeleteBlock(usedBlocks);

  mrlTextureBlock* c, *n;
  for (c = freeBlocks; c != NULL; c = n) 
    {
      n = c->next;
      delete c;
    }

  textureDeleteFileDescriptors();

  freeBlocks = usedBlocks = NULL;
  mi_delete_lock( &mMemFlushLock );
}


static void textureInit( const unsigned maxMemory )
{
  if ( memoryUsed == 0 )
    {
      accessId = 0;
      if ( usedBlocks != NULL || freeBlocks != NULL )
	{
	  textureShutdown();
	}
      memoryUsed = 0;
      memoryMax = maxMemory;
    }

  mi_init_lock( &mMemFlushLock );
}


#define WEIGHT_LUT_SIZE 1024
static miScalar* weightLut = NULL;

static miScalar getWeight( miScalar r2 )
{
  int offset = (int) (r2 * (WEIGHT_LUT_SIZE - 1));
  mrASSERT(offset >= 0 && offset < WEIGHT_LUT_SIZE);
  return weightLut[offset];
}

static void weightLutInit()
{
  if ( weightLut ) return;

  weightLut = new miScalar[WEIGHT_LUT_SIZE];

  for (int i = 0; i < WEIGHT_LUT_SIZE; ++i) {
    miScalar alpha = 2;
    miScalar r2 = miScalar(i) / miScalar(WEIGHT_LUT_SIZE - 1);
    miScalar weight = exp(-alpha * r2);
    weightLut[i] = weight;
  }
}

void weightLutRelease()
{
  delete [] weightLut;
  weightLut = NULL;
}


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

static float invLog2 = (float) (1. / log(2.));
#define LOG2(x) (log(x)*invLog2)


//!
//! Check to see if a file exists on disk.
//!
static bool fileExists(const char* filename)
{
   struct stat sbuf;
   int result = stat(filename, &sbuf);
   return (result == 0);
}



DLLEXPORT int mrl_exr_file_version() { return 6; }

DLLEXPORT void mrl_exr_file_init( miState* const state,
				  const mrl_exr_file_t* p,
				  miBoolean* req_inst )
{
  if ( !p )
    {
      weightLutInit();

      unsigned maxNumPages = 16384;

      const char* m = getenv( "MRL_TEXTURE_MEMORY" );
      if ( m != NULL )
	{
	  maxNumPages = (unsigned) atoi( m );
	  if ( maxNumPages < 1 ) maxNumPages = 16384;
	}
      mi_info("mrl_exr_file: MRL_TEXTURE_MEMORY=%u", maxNumPages);
      textureInit( maxNumPages * 1024 );
      gStats->init();
      *req_inst = miTRUE;
      return;
    }

   void **user;
   mi_query(miQ_FUNC_USERPTR, state, 0, &user);
   *user = NULL;


   miTag fileNameTag = *mi_eval_tag( &p->fileTextureName );
   if ( fileNameTag == miNULLTAG ) 
     {
       const char* name = mi_api_tag_lookup( state->instance );
       mi_info("mrl_exr_file (%s): No filename.", name);
       return;
     }

   const char* name = (const char*) mi_db_access( fileNameTag );

   if ( !fileExists( name ) )
     {
       mi_error("mrl_exr_file: File \"%s\" does not exist.", name);
       return;
     }


   try 
     {
       exrTexture* cache = new exrTexture( name );

       mi_info("mrl_exr_file: Opened file \"%s\", file descriptor %d.", name,
		fileDescriptors.size() );

       miBoolean mirrorU = *mi_eval_boolean( &p->mirrorU );
       if ( mirrorU ) cache->mirrorU();
       else
	 {
	   miBoolean wrapU = *mi_eval_boolean( &p->wrapU );
	   if ( wrapU ) cache->wrapU();
	 }
       
       miBoolean mirrorV = *mi_eval_boolean( &p->mirrorV );
       if ( mirrorV ) cache->mirrorV();
       else
	 {
	   miBoolean wrapV = *mi_eval_boolean( &p->wrapV );
	   if ( wrapV ) cache->wrapV();
	 }


       *user = cache;
     }
   catch( std::exception& e )
     {
       mi_error("mrl_exr_file: File \"%s\" %s", name, e.what() );
     }
   catch( ... )
     {
       mi_error("mrl_exr_file: File \"%s\" Unknown exception thrown", name );
     }

   mi_db_unpin( fileNameTag );
}


EXTERN_C DLLEXPORT void
mrl_exr_file_exit(
		  miState* const        state,
		  struct gg_exr_t* p
		  )
{
  if ( !p )
  {
    gStats->stats();
    textureShutdown();
    weightLutRelease();
    return;
  }
  
  void **user;
  mi_query(miQ_FUNC_USERPTR, state, 0, &user);
  
  exrTexture* cache = static_cast<exrTexture*>( *user );
  if (cache) 
    {
      textureDeleteFileDescriptor( cache );
      delete cache;
    }
}


DLLEXPORT miBoolean mrl_exr_file( mrl_exr_file_result_t* const m,
				  miState* const state,
				  const mrl_exr_file_t* p )
{
  m->outColor = *mi_eval_color( &p->colorOffset );
  m->outAlpha = m->outColor.a = *mi_eval_scalar( &p->alphaOffset );
  m->fileHasAlpha = *mi_eval_boolean( &p->fileHasAlpha );
  m->outTransparency = 1.0f - m->outAlpha;
  m->outNormal       = state->normal;

  void **user;
  mi_query(miQ_FUNC_USERPTR, state, 0, &user);
  
  exrTexture* cache = static_cast<exrTexture*>( *user );
  if ( cache == NULL ) return miTRUE;

  miTag fileTag = *mi_eval_tag( &p->fileTextureName );
  miVector* st = mi_eval_vector( &p->uvCoord );

  miColor*  defaultColor;
  miVector* coverage = mi_eval_vector( &p->coverage );
  if ( coverage->x < 1.0f || coverage->y < 1.0f )
    {
      defaultColor = mi_eval_color( &p->defaultColor );
    }

  miInteger filterType = *mi_eval_integer( &p->filterType );  

  //
  // Get uv coordinates of triangle
  //
  miVector txt[3];
#ifdef RAY36   
   const miVector* t[3];
#else
   miVector* t[3];
#endif
  t[0] = mi_eval_vector( &p->vertexUvOne );
  t[1] = mi_eval_vector( &p->vertexUvTwo );
  t[2] = mi_eval_vector( &p->vertexUvThree );

  miVector* repeatUV = mi_eval_vector( &p->repeatUV );
  if ( *t[0] == *t[1] && *t[1] == *t[2] )
    {
      mi_tri_vectors(const_cast< miState* >(state), 't', 0, 
		     &t[0], &t[1], &t[2]);

      txt[0] = *t[0] * (*repeatUV);
      txt[1] = *t[1] * (*repeatUV);
      txt[2] = *t[2] * (*repeatUV);

//       miScalar  rotateUV = *mi_eval_scalar( &p->rotateUV );
      
      t[0] = &txt[0];
      t[1] = &txt[1];
      t[2] = &txt[2];
    }



  //
  // Calculate dS/dx, dS/dy, dT/dx, dT/dy
  //
  vector dS(kNoInit), dT( kNoInit );
  rayDifferentials( dS, dT, state, t );

  //
  // Calculate minification level
  //
  miScalar lodX, lodY, filterWidth;

  miVector* uvFilterSize;

  if ( filterType != kNone )
    {
      // filterWidth of 0.707 works nicely here
      filterWidth  = *mi_eval_scalar( &p->filterWidth );
      filterWidth *= *mi_eval_scalar( &p->filter );
      uvFilterSize = mi_eval_vector( &p->uvFilterSize );
    }

  switch( filterType )
    {
    case kNone:
      lodX = lodY = 0;
      break;
    case kMipMap:
    case kBox:
#ifndef MR_ELLIPTIC_FILTER
    default:
#endif
      filterWidth *= 0.5f;
      lodX = filterWidth * LOG2( max( dS.lengthSquared(), 1e-6f ) );
      lodY = filterWidth * LOG2( max( dT.lengthSquared(), 1e-6f ) );
      break;
#ifdef MR_ELLIPTIC_FILTER
    default:
      if ( dT.lengthSquared() < dS.lengthSquared() )
	{
	  std::swap( dS, dT );
	}
      miScalar majorLength = dT.length();
      miScalar minorLength = dS.length();

      // if the eccentricity of the ellipse is looking to be too big, scale
      // up the shorter of the two vectors so that it's a little more reasonable.
      // This lets us avoid spending inordinate amounts of time filtering very
      // long and skinny regions (which take a lot of time), at the expense of
      // some blurring...
      const miScalar maxEccentricity = 30;
      miScalar e = majorLength / minorLength;

      if (e > maxEccentricity) 
	{
	  // blur in the interests if filtering in a reasonable amount
	  // of time
	  minorLength *= e / maxEccentricity;
	}

      // Pick a lod such that we're looking at somewhere around 3-9 texels
      // in the minor axis direction.
      lodX = lodY = -filterWidth * LOG2(3.0f / minorLength);
      lodX = clamp(lodX, 0.0f, cache->numXLevels() - 1 - 1e-7f);
      lodY = clamp(lodY, 0.0f, cache->numYLevels() - 1 - 1e-7f);
      break;
#endif
    }

  int lx = (int) lodX;
  int ly = (int) lodY;

  // Make sure s,t coordinates map onto [0,1] range
  st->x = fmod( fabs(st->x), 1.0f );
  st->y = 1.0f - fmod( fabs(st->y), 1.0f );

  color c( kNoInit );
  if ( lx <= 0 && ly <= 0 )
    {
      gStats->level( 0 );
      gStats->level( 0 );
      if ( filterType == kNone )
	cache->lookup( c, st->x, st->y, 0, 0 );
      else
	cache->blur( c, st->x, st->y, 0, 0 );
    }
  else
    {
      if ( lx < 0 ) lx = 0;
      if ( ly < 0 ) ly = 0;

      int numXLevels = cache->numXLevels();
      int numYLevels = cache->numYLevels();
      if ( lx >= numXLevels ) lx = numXLevels - 1;
      if ( ly >= numYLevels ) ly = numYLevels - 1;

      gStats->level( lx );
      gStats->level( ly );


#ifdef MR_ELLIPTIC_FILTER
      if ( filterType <= kBox )
#endif
	{
	  cache->lookup( c, st->x, st->y, lx, ly );

	  if ( filterType == kMipMap )
	    {
	      // Handle as MipMap
	      miScalar frac = lodX - lx;
	      if ( frac > 0.005f )
		{
		  lx += 1;
		  ly += 1;
		  if ( lx < numXLevels && ly < numYLevels )
		    { 
		      color c2( kNoInit );
		      cache->lookup( c2, st->x, st->y, lx, ly );
		      c = mix( c, c2, frac );
		      c2   *= frac;
		      c2.a *= frac;
		      frac  = 1.0f - frac;
		      c    *= frac;
		      c.a  *= frac;
		      c    += c2;
		      c.a  += c2.a;
		    }
		}
	    }
	  else
	    {
	      // Handle as anisotropic RipMap
	      miScalar frac = lodX - lx;
	      if ( frac > 0.005f )
		{
		  if ( (lx+1) < numXLevels )
		    { 
		      color c2( kNoInit );
		      cache->lookup( c2, st->x, st->y, lx + 1, ly );
		      c = mix( c, c2, frac );
		      c2   *= frac;
		      c2.a *= frac;
		      frac  = 1.0f - frac;
		      c    *= frac;
		      c.a  *= frac;
		      c    += c2;
		      c.a  += c2.a;
		    }
		}

	      frac = lodY - ly;
	      if ( frac > 0.005f )
		{
		  if ( (ly+1) < numYLevels )
		    { 
		      color c2( kNoInit );
		      cache->lookup( c2, st->x, st->y, lx, ly+1 );
		      c = mix( c, c2, frac );
		      c2   *= frac;
		      c2.a *= frac;
		      frac  = 1.0f - frac;
		      c    *= frac;
		      c.a  *= frac;
		      c    += c2;
		      c.a  += c2.a;
		    }
		}
	    }

	}
#ifdef MR_ELLIPTIC_FILTER
      else
	{
	  // Calculate pixel coordinate for level
	  int scaleX = cache->width()  / (1 << lx);
	  int scaleY = cache->height() / (1 << ly);
	  mi_info("scale: %d, %d", scaleX, scaleY);

	  st->x = st->x * scaleX - 0.5f;
	  st->y = st->y * scaleY - 0.5f;
	  mi_info("st: %g, %g", st->x, st->y);

	  miScalar ux = dS.x * scaleX;
	  miScalar vx = dS.y * scaleY;
	  miScalar uy = dT.x * scaleX;
	  miScalar vy = dT.y * scaleY;

	  // compute ellipse coefficients to bound the region: 
	  // A*x*x + B*x*y + C*y*y = F.
	  miScalar A = vx*vx + vy*vy + 1;
	  miScalar B = -2.0f * (ux*vx + uy*vy);
	  miScalar C = ux*ux + uy*uy + 1;
	  miScalar F = A * C - B * B * 0.25f;

	  mi_info("* A:%g B:%g C:%g F:%g", A, B, C, F);
	  float invF = 1.f / (A*C - B*B*0.25f);
	  A *= invF;
	  B *= invF;
	  C *= invF;

	  mi_info("# A:%g B:%g C:%g F:%g", A, B, C, F);

	  // it better be an ellipse!
	  mrASSERT(A*C-B*B/4. > 0.);

	  // Compute the ellipse's (u,v) bounding box in texture space
	  miScalar det = -B*B + 4.0f*A*C;
	  miScalar invDet = 1.f / det;
	  miScalar uSqrt = mr::math<miScalar>::sqrt(C * det);
	  miScalar vSqrt = mr::math<miScalar>::sqrt(A * det);
	  mi_info("uSqrt: %g", uSqrt);
	  mi_info("vSqrt: %g", vSqrt);
	  mi_info("invDet: %g", invDet);
	  int u0 = mr::fastmath<miScalar>::floor(st->x - 2.0f * invDet * uSqrt );
	  int u1 = mr::fastmath<miScalar>::ceil (st->x + 2.0f * invDet * uSqrt );
	  int v0 = mr::fastmath<miScalar>::floor(st->y - 2.0f * invDet * vSqrt );
	  int v1 = mr::fastmath<miScalar>::ceil (st->y + 2.0f * invDet * vSqrt );

	  // Heckbert MS thesis, p. 59; scan over the bounding box of the ellipse
	  // and incrementally update the value of Ax^2+Bxy*Cy^2; when this
	  // value, q, is less than F, we're inside the ellipse so we filter
	  // away..
	  c = 0;
	  miScalar den = 0.0f;
	  miScalar ddq = 2 * A;
	  miScalar U   = u0 - st->x;
	  miScalar U2  = 2 * U;
	  miScalar UU  = U * U;
	  miScalar BU  = B * U;
	  miScalar AU21 = A*(U2+1);

	  mi_info("v0: %d  v1: %d  u0: %d  u1: %d", v0, v1, u0, u1);

	  for (int v = v0; v <= v1; ++v) 
	    {
	      miScalar V  = v - st->y;
	      miScalar VV = V * V;
	      for (int u = u0; u <= u1; ++u) 
		{
		  miScalar U = u - st->x;
		  miScalar r2 = A * U * U + B * U * V + C * VV;
		  if (r2 < 1.0f)
		    {
		      miScalar weight = getWeight(r2);
		      color tmp( kNoInit );
		      miScalar x = (miScalar) mod(u, scaleX) / scaleX;
		      miScalar y = (miScalar) mod(v, scaleY) / scaleY;
		      cache->lookup(tmp, x, y, lx, ly);
 		      tmp   *= weight;
 		      tmp.a *= weight;
		      c   += tmp;
		      c.a += tmp.a;
		      den += weight;
		    }
		}
	    }


	  mrASSERT(den > 0.);
	  c   /= den;
	  c.a /= den;
	}
#endif
    }

      
  miBoolean invert = *mi_eval_boolean( &p->invert );
  if ( invert )
    {
      c   = 1.0f - c;
      c.a = 1.0f - c.a;
    }

  m->outColor   += c;

  color* colorGain = (color*) mi_eval_color( &p->colorGain );
  m->outColor   *= *colorGain;


  miBoolean alphaIsLuminance = *mi_eval_boolean( &p->alphaIsLuminance );
  if ( m->fileHasAlpha || alphaIsLuminance )
    {
      miScalar  alphaGain = *mi_eval_scalar( &p->alphaGain );
      if ( alphaIsLuminance )
	{
	  c.a = (m->outColor.r + m->outColor.g + m->outColor.b) / 3.0f;
	}

      c.a *= alphaGain;
      m->outAlpha   += c.a;
      m->outColor.a += c.a;
    }

  m->outTransparency = ( 1.0f - m->outAlpha );

  return miTRUE;
}


} // extern "C"
