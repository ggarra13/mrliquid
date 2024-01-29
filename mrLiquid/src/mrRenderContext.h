/**
 * @file   mrRenderContext.h
 * @author gga
 * @date   Tue Jun 27 00:26:39 2006
 * 
 * @brief  
 * 
 * 
 */

/**
 * Render Context class.   Manages rendering different portions
 * of the scene.
 * 
 */
class mrRenderContext
{
   public:
     mrRenderContext();
     ~mrRenderContext();

     bool open( const char* filename );

     void insert( mrInstanceBase* inst );

     void write();

     void clear_cameras();
     void insert_camera( mrInstanceCamera* inst );

     void render();

     void close();

     void destroy();

   protected:
     // whether this context is used in IPR or not
     // Only one context is allowed to be used for ipr at a time.
     bool ipr;

     mrGroupInstance* world;
     mrOptions*        opts;

     typedef std::vector< mrCamera* > mrCameraList;
     mrCameraList   cameras;

     static MRL_FILE* f;
};


void mrRenderContext::destroy()
{
//    if ( f )
//    {
//       opts->destroy(f);
//       camera->destroy(f);
//       world->destroy(f);
//    }
//    else
//    {
//       opts->destroy();
//       camera->destroy();
//       world->destroy();
//    }
}


void mrRenderContext::mrRenderContext( const char* worldName )
{
   world = mrGroupInstance::factory( worldName );
}

void mrRenderContext::~mrRenderContext()
{
   destroy();
}

void mrRenderContext::clear_cameras()
{
   cameras->clear();
}

void mrRenderContext::insert_camera( mrInstanceCamera* inst )
{
   cameras->push_back( inst );
}

void mrRenderContext::insert( mrInstanceBase* inst )
{
   world->insert( inst );
}

void mrRenderContext::write()
{
   if ( f )
   {
      opts->write(f);
      world->write(f);
      camera->write(f);
   }
   else
   {
#ifdef GEOSHADER_H
      opts->write();
      world->write();
      camera->write();
#endif
   }
}


void mrRenderContext::open( const char* filename )
{
}

void mrRenderContext::close()
{
   if ( f ) fclose(f);
}

