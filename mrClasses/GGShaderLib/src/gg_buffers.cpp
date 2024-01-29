//
//  Copyright (c) 2004, Gonzalo Garramuno
//
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are
//  met:
//  *       Redistributions of source code must retain the above copyright
//  notice, this list of conditions and the following disclaimer.
//  *       Redistributions in binary form must reproduce the above
//  copyright notice, this list of conditions and the following disclaimer
//  in the documentation and/or other materials provided with the
//  distribution.
//  *       Neither the name of Gonzalo Garramuno nor the names of
//  its other contributors may be used to endorse or promote products derived
//  from this software without specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
/******************************************************************************
 * Created:	02.02.04
 * Module:	gg_buffers
 *
 * Exports:
 *      gg_buffers(), gg_buffers_version()
 *
 * Requires:
 *
 * History:
 *      --- Made unneeded in maya7 --
 *      07.05.03: initial version
 *
 * Description:
 *      This output shader saves all mray channels (buffers) into several
 *      .iff images.
 *
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "mrGenerics.h"
using namespace mr;


////////////////// Output shader
struct gg_buffers_t
{
     miTag      filename;
     miBoolean normalize;
};

EXTERN_C DLLEXPORT int gg_buffers_version(void) {return(1);}

const char* EXT = ".iff";
#define FMT miIMG_FORMAT_IFF
#define MAX_USER_BUFFERS 8

EXTERN_C DLLEXPORT miBoolean 
gg_buffers(
	   miColor* const result,
	   miState* const state,
	   struct gg_buffers_t* p
	   )
{

#if 0
   // Extract  filename from color image output name...
   
   //  If you look at camera->output, you'll find a list of miFunction tags
   //  chained with miFunction::next_function. Look for functions where
   //  miFunction::type == miFUNCTION_OUTFILE, and get a pointer

   //      miDecl_fileparm *p = (miDecl_fileparm *)func->parameters;

   //  You'll find miDecl_fileparm in geoshader.h. It contains a file name.

   // Ok, that is how it is supposed to be, but the code below crashes,
   // as next_function returns crap.

   char* n = NULL;
   miTag current = state->camera->output;
   do {
      miFunction* f = (miFunction*) mi_db_access( current );
      if ( f->type == miFUNCTION_OUTFILE )
      {
	 miDecl_fileparm *p = (miDecl_fileparm *)f->parameters;
	 n = p->filename;
	 mi_db_unpin( current );
	 break;
      }
      mi_db_unpin( current );

      current = f->next_function;
   } while ( current != miNULLTAG );

   if ( n == NULL )
   {
      mi_error("Could not determine output file name");
      n = "dummy";
   }
   std::string file( n );
   
#else
   miTag filenameTag = *mi_eval_tag( &p->filename );
   const char* n = (char*) mi_db_access( filenameTag );
   if ( n == NULL )
   {
      mi_db_unpin( filenameTag );
      mi_error("Please set the name of the filename for "
	       "gg_buffers output shader");
      return miFALSE;
   }
   std::string file( n ); 
   mi_db_unpin( filenameTag );
#endif
   
   int xres = state->camera->x_resolution;
   int yres = state->camera->y_resolution;
   
   // get subwindow
   int xl = state->camera->window.xl;
   int yl = state->camera->window.yl;
   int xh = state->camera->window.xh;
   int yh = state->camera->window.yh;
   if ( xh > xres ) xh = xres;
   if ( yh > yres ) yh = yres;
   
   int bxres = xh - xl;
   int byres = yh - yl;
   if ( bxres < xres || byres < yres )
   {
      mi_info("Just a preview, I assume.  Not saving buffers.");
      return miTRUE;
   }

   char tmp[10];
#if 0
   itoa(state->camera->frame,tmp,10);
#else
   sprintf( tmp, "%d", state->camera->frame);
#endif

   std::string frame( tmp );
   if ( state->camera->frame_field )
   {
#if 0
     itoa(state->camera->frame_field,tmp,10);
#else
     sprintf( tmp, "%d", state->camera->frame_field);
#endif

     frame += ".";
     frame += tmp;
   }
   
   const miOptions* o = state->options;
   
   miImg_image* buf;
#ifdef RAY34
   buf = mi_output_image_open( state, miRC_IMAGE_Z );
#else
   buf = static_cast< miImg_image* >( o->image[miRC_IMAGE_Z].p );
#endif
   if ( buf )
   {
     std::string name( file );
     name += ".Z.";
     name += frame;
     name += EXT;
     mi_info("Saving out depth channel image as \"%s\"", name.c_str());

     miImg_file fd;
     fd.width = bxres;
     fd.height = byres;
     fd.bits = buf->bits;
     fd.comp = buf->comp;
     fd.filter = miFALSE;
     fd.topdown = miFALSE;
     fd.gamma = 1;
     fd.aspect = state->camera->aspect/xres*yres;
     fd.type = miIMG_TYPE_Z;
     fd.format = FMT;
     fd.error = miIMG_ERR_NONE;
     fd.os_error = 0;
     
     if ( mi_img_create( &fd, mi_mem_strdup( name.c_str() ),
			 fd.type, FMT, bxres, byres ) )
     {
	if (! mi_img_image_write( &fd, buf ) )
	   mi_error("Could not write depth channel image.");
	mi_img_close( &fd );
     }
     else
     {
	mi_error("Could not open depth channel image.");
     }
  }
  else
  {
     mi_info("Ignoring depth channel");
  }
#ifdef RAY34
   mi_output_image_close( state, miRC_IMAGE_Z );
#endif
  
#ifdef RAY34
  buf = mi_output_image_open( state, miRC_IMAGE_N );
#else
  buf = static_cast< miImg_image* >( o->image[miRC_IMAGE_N].p );
#endif
  if ( buf )
  {
     std::string name( file );
     name += ".N.";
     name += frame;
     name += EXT;
     mi_info("Saving out normal channel image as \"%s\"", name.c_str());

     miImg_file fd;
     fd.width = bxres;
     fd.height = byres;
     fd.bits = buf->bits;
     fd.comp = buf->comp;
     fd.filter = miFALSE;
     fd.topdown = miFALSE;
     fd.gamma = 1;
     fd.aspect = state->camera->aspect/xres*yres;
     fd.type = miIMG_TYPE_N;
     fd.format = FMT;
     fd.error = miIMG_ERR_NONE;
     fd.os_error = 0;
     
     if ( mi_img_create( &fd, mi_mem_strdup( name.c_str() ),
			 fd.type, FMT, bxres, byres ) )
     {
	if (! mi_img_image_write( &fd, buf ) )
	   mi_error("Could not write normal channel image.");
	mi_img_close( &fd );
     }
     else
     {
	mi_error("Could not open normal channel image.");
     }
  }
  else
  {
     mi_info("Ignoring normal channel");
  }

#ifdef RAY34
   mi_output_image_close( state, miRC_IMAGE_N );
#endif


#ifdef RAY34
  buf = mi_output_image_open( state, miRC_IMAGE_M );
#else
  buf = static_cast< miImg_image* >( o->image[miRC_IMAGE_M].p );
#endif

  if ( buf )
  {
     std::string name( file );
     name += ".M.";
     name += frame;
     name += EXT;
     mi_info("Saving out motion channel image as \"%s\"", name.c_str());

     miImg_file fd;
     fd.width = bxres;
     fd.height = byres;
     fd.bits = buf->bits;
     fd.comp = buf->comp;
     fd.filter = miFALSE;
     fd.topdown = miFALSE;
     fd.gamma = 1;
     fd.aspect = state->camera->aspect/xres*yres;
     fd.type = miIMG_TYPE_M;
     fd.format = FMT;
     fd.error = miIMG_ERR_NONE;
     fd.os_error = 0;
     
     if ( mi_img_create( &fd, mi_mem_strdup( name.c_str() ),
			 fd.type, FMT, bxres, byres ) )
     {
	if (! mi_img_image_write( &fd, buf ) )
	   mi_error("Could not write motion channel image.");
	mi_img_close( &fd );
     }
     else
     {
	mi_error("Could not open motion channel image.");
     }
  }
  else
  {
     mi_info("Ignoring motion channel");
  }

#ifdef RAY34
   mi_output_image_close( state, miRC_IMAGE_M );
#endif



#ifdef RAY34
  buf = mi_output_image_open( state, miRC_IMAGE_TAG );
#else
  buf = static_cast< miImg_image* >( o->image[miRC_IMAGE_TAG].p );
#endif

  if ( buf )
  {
     std::string name( file );
     name += ".TAG.";
     name += frame;
     name += EXT;
     mi_info("Saving out tag channel image as \"%s\"", name.c_str());

     miImg_file fd;
     fd.width = bxres;
     fd.height = byres;
     fd.bits = buf->bits;
     fd.comp = buf->comp;
     fd.filter = miFALSE;
     fd.topdown = miFALSE;
     fd.gamma = 1;
     fd.aspect = state->camera->aspect/xres*yres;
     fd.type = miIMG_TYPE_TAG;
     fd.format = FMT;
     fd.error = miIMG_ERR_NONE;
     fd.os_error = 0;
     
     if ( mi_img_create( &fd, mi_mem_strdup( name.c_str() ),
			 fd.type, FMT, bxres, byres ) )
     {
	if (! mi_img_image_write( &fd, buf ) )
	   mi_error("Could not write tag channel image.");
	mi_img_close( &fd );
     }
     else
     {
	mi_error("Could not open tag channel image.");
     }
  }
  else
  {
     mi_info("Ignoring tag channel");
  }


#ifdef RAY34
   mi_output_image_close( state, miRC_IMAGE_TAG );
#endif





#ifdef RAY34
  buf = mi_output_image_open( state, miRC_IMAGE_COVERAGE );
#else
  buf = static_cast< miImg_image* >( o->image[miRC_IMAGE_COVERAGE].p );
#endif

  if ( buf )
  {
     std::string name( file );
     name += ".COV.";
     name += frame;
     name += EXT;
     mi_info("Saving out coverage channel image as \"%s\"", name.c_str());

     miImg_file fd;
     fd.width = bxres;
     fd.height = byres;
     fd.bits = buf->bits;
     fd.comp = buf->comp;
     fd.filter = miFALSE;
     fd.topdown = miFALSE;
     fd.gamma = 1;
     fd.aspect = state->camera->aspect/xres*yres;
     fd.type = miIMG_TYPE_COVERAGE;
     fd.format = FMT;
     fd.error = miIMG_ERR_NONE;
     fd.os_error = 0;
     
     if ( mi_img_create( &fd, mi_mem_strdup( name.c_str() ),
			 fd.type, FMT, bxres, byres ) )
     {
	if (! mi_img_image_write( &fd, buf ) )
	   mi_error("Could not write coverage channel image.");
	mi_img_close( &fd );
     }
     else
     {
	mi_error("Could not open coverage channel image.");
     }
  }
  else
  {
     mi_info("Ignoring coverage channel");
  }

#ifdef RAY34
   mi_output_image_close( state, miRC_IMAGE_COVERAGE );
#endif


  miBoolean normalize = *mi_eval_boolean( &p->normalize );
  for (int i = 0; i < MAX_USER_BUFFERS; ++i )
  {
#ifdef RAY34
     buf = mi_output_image_open( state, miRC_IMAGE_USER + i );
#else
     buf = static_cast< miImg_image* >( o->image[miRC_IMAGE_USER+i].p );
#endif
     if ( buf )
     {
	std::string name( file );
	char c[3]; 
#if 0
	itoa(i, c, 10);
#else
	sprintf( c, "%d", i);
#endif
	name += ".USER";
	name += c;
	name += ".";
	name += frame;
	name += EXT;
	
	if ( normalize )
	{
	   mi_info("Normalizing user buffer #%d", i);

	   color col;
	   miColor maxColor = {0,0,0};
	   int y,x;
	   for (y=0; y < state->camera->y_resolution; ++y)
	   {
	      for (x=0; x<state->camera->x_resolution; ++x)
	      {
		 mi_img_get_color(buf, &col, x, y);

		 if ( col.r > maxColor.r ) maxColor.r = col.r;
		 if ( col.g > maxColor.g ) maxColor.g = col.g;
		 if ( col.b > maxColor.b ) maxColor.b = col.b;
		 if ( col.a > maxColor.a ) maxColor.a = col.a;
	      }
	   }
	   // Make sure to avoid / by 0
	   if ( maxColor.r == 0.0f ) maxColor.r = 1.0f;
	   if ( maxColor.g == 0.0f ) maxColor.g = 1.0f;
	   if ( maxColor.b == 0.0f ) maxColor.b = 1.0f;
	   if ( maxColor.a == 0.0f ) maxColor.a = 1.0f;
	   for (y=0; y < state->camera->y_resolution; y++)
	   {
	      for (x=0; x<state->camera->x_resolution; x++)
	      {
		 mi_img_get_color(buf, &col, x, y);
		 col   /= maxColor;
		 col.a /= maxColor.a;
		 mi_img_put_color(buf, &col, x, y);
	      }
	   }
	}

	mi_info("Saving out user buffer #%d as \"%s\"", i, name.c_str());
	
	miImg_file fd;
	fd.width = bxres;
	fd.height = byres;
	fd.bits = buf->bits;
	fd.comp = buf->comp;
	fd.filter = miFALSE;
	fd.topdown = miFALSE;
	fd.gamma = 1;
	fd.aspect = state->camera->aspect/xres*yres;
	fd.type = (miImg_type) buf->type;
	fd.format = FMT;
	fd.error = miIMG_ERR_NONE;
	fd.os_error = 0;

	
	if ( mi_img_create( &fd, mi_mem_strdup( name.c_str() ),
			    fd.type, FMT, bxres, byres ) )
	{
	   if (! mi_img_image_write( &fd, buf ) )
	      mi_error("Could not write user buffer #%d.",i);
	   mi_img_close( &fd );
	}
	else
	{
	   mi_error("Could not open image for user buffer #%d.",i);
	}
	
#ifdef RAY34
	mi_output_image_close( state, miRC_IMAGE_USER + i );
#endif
     }
     else
     {
	mi_info("Ignoring user buffer #%d channel", i);
     }
  }

  
  return(miTRUE);
}
