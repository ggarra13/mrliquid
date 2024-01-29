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


#include <vector>
#include <algorithm>


#include "maya/MFnFluid.h"
#include "maya/MFnDagNode.h"
#include "maya/MBoundingBox.h"



#include "mrHelpers.h"
#include "mrIO.h"
#include "mrByteSwap.h"
#include "mrFluid.h"
#include "mrAttrAux.h"

extern mrOptions* options;
extern MString    tempDir;
extern MString  sceneName;
extern int          frame;



void mrFluid::getData()
{
   MStatus status; MPlug p;
   MFnDagNode fn( path );

   miFluidFile = "";
   GET_OPTIONAL( miFluidFile );
   if ( miFluidFile != "" )
   {
      int last = (int) miFluidFile.length() - 1;
      // If user placed a .pdc file, remove that extension
      if ( last > 3 && miFluidFile.substring( last-3 , last ) == ".fld" )
	 miFluidFile = miFluidFile.substring( 0, last-4 );
      // If user placed a xxx.0250.pdc file, remove the numbers, too.
      int period = miFluidFile.rindex('.');
      if ( period != -1 )
      {
	 const char* c = miFluidFile.asChar();
	 while (  c[last] >= '0' && c[last] <= '9' && last != period )
	    --last;
	 if ( last == period )
	    miFluidFile = miFluidFile.substring( 0, period-1 );
      }
      const char* froot = miFluidFile.asChar();
      if ( froot[0] != '/' && froot[1] != ':' )
	 miFluidFile = tempDir + sceneName + "/" + miFluidFile;
      return;
   }

      
   miFluidFile  = tempDir + sceneName + "/";
   checkOutputDirectory( miFluidFile, true );

   miFluidFile += dagPathToFilename( name );
}


mrFluid::mrFluid( const MDagPath& shape ) :
mrObject( getMrayName( shape ) )
#ifdef GEOSHADER_H
, function( miNULLTAG )
#endif
{
   shapeAnimated = true;
   path = shape;
   getData();
}



mrFluid* mrFluid::factory( const MDagPath& shape )
{
   mrFluid* s;
   char written = mrNode::kNotWritten;
   mrShape* base = mrShape::find( shape );
   if ( base )
   {
      s = dynamic_cast< mrFluid* >( base );
      if ( s != NULL ) return s;
      DBG("Warning:: redefined object type " << base->name);
      written = base->written;
      nodeList.remove( nodeList.find( base->name ) );
   }

   s = new mrFluid( shape );
   s->written = written;
   nodeList.insert( s );
   return s;
}


void mrFluid::forceIncremental()
{
   maxDisplace = -1;
   mrObject::forceIncremental();
   getData();
}


void mrFluid::setIncremental( bool sameFrame )
{
   written = kIncremental;
   maxDisplace = -1;
   mrObject::setIncremental( sameFrame );
}


void mrFluid::getMotionBlur( char step )
{
}


void mrFluid::write_group( MRL_FILE* f )
{
   MFnDagNode fn( path );
   MBoundingBox b = fn.boundingBox();

   MVector bmin = b.min();
   MVector bmax = b.max();
   if ( bmin.z == bmax.z )
   {
      bmin.z -= 0.125;
      bmax.z += 0.125;
   }

   MVectorArray v;
   v.append( MVector( bmax.x, bmax.y, bmax.z ) );
   v.append( MVector( bmin.x, bmax.y, bmax.z ) );
   v.append( MVector( bmax.x, bmin.y, bmax.z ) );
   v.append( MVector( bmin.x, bmin.y, bmax.z ) );
   v.append( MVector( bmax.x, bmax.y, bmin.z ) );
   v.append( MVector( bmin.x, bmax.y, bmin.z ) );
   v.append( MVector( bmax.x, bmin.y, bmin.z ) );
   v.append( MVector( bmin.x, bmin.y, bmin.z ) );

   TAB(2); COMMENT("# point vectors\n");
   write_vectors( f, v );

   TAB(2); COMMENT("# vertices\n");
   for ( short i = 0; i < 8; ++i )
   {
      TAB(2);
      MRL_FPRINTF(f, "v %d\n", i );
   }
   TAB(2); COMMENT("# polygons\n");

   TAB(2); MRL_PUTS("c 0 0 1 3 2\n");
   TAB(2); MRL_PUTS("c 0 0 1 5 4\n");
   TAB(2); MRL_PUTS("c 0 0 4 6 2\n");
   TAB(2); MRL_PUTS("c 0 1 5 7 3\n");
   TAB(2); MRL_PUTS("c 0 2 3 7 6\n");
   TAB(2); MRL_PUTS("c 0 5 4 6 7\n");
}






MString mrFluid::getFluidFile() const
{
   long fldframe = (long) frame;
   char frameStr[15];
   sprintf( frameStr, "%ld", fldframe);
   
   MString fldFile = miFluidFile;
   fldFile += ".";
   fldFile += frameStr;
   fldFile += ".fld";
   return fldFile;
}

/** 
 * Write user data for object.
 * 
 * @param f 
 */
void mrFluid::write_fluid_data( MRL_FILE* f )
{
   if ( written == kWritten || 
	(options->exportFilter & mrOptions::kUserData) ) return;

   mrObject::write_user_data( f );
   
   if ( written == kIncremental ) MRL_PUTS("incremental ");


   MStatus status;
   MPlug p;
   MFnFluid fn( path );

   unsigned xres, yres, zres;
   fn.getResolution( xres, yres, zres );

   unsigned gridsize = fn.gridSize();

   bool density = false;
   MFnFluid::FluidGradient densityGradient;
   MFnFluid::FluidMethod   densityMode;
   fn.getDensityMode( densityMode, densityGradient );
   if ( densityMode == MFnFluid::kStaticGrid ||
	densityMode == MFnFluid::kDynamicGrid )
      density = true;

   MFnFluid::FluidGradient temperatureGradient;
   MFnFluid::FluidMethod   temperatureMode;
   fn.getTemperatureMode( temperatureMode, temperatureGradient );
   bool temperature = false;
   if ( temperatureMode == MFnFluid::kStaticGrid ||
	temperatureMode == MFnFluid::kDynamicGrid )
      temperature = true;


   MFnFluid::FluidGradient fuelGradient;
   MFnFluid::FluidMethod fuelMode;
   fn.getFuelMode( fuelMode, fuelGradient );
   bool fuel = false;
   if ( fuelMode == MFnFluid::kStaticGrid ||
	fuelMode == MFnFluid::kDynamicGrid )
      fuel = true;

   MFnFluid::ColorMethod colorMode;
   fn.getColorMode ( colorMode );
   bool color = false;
   if ( colorMode != MFnFluid::kUseShadingColor ) color = true;


   char header[220];
   for ( int i = 0; i < 220; ++i ) header[i] = 0;
   int* idummy   = (int*) header;
   float* fdummy = (float*) header;

   // Magic number == 'Maya'
#ifdef MR_BIG_ENDIAN
   header[0] = 'M';
   header[1] = 'a';
   header[2] = 'y';
   header[3] = 'a';
#else
   header[0] = 'a';
   header[1] = 'y';
   header[2] = 'a';
   header[3] = 'M';
#endif

   // 
   idummy[1] = 1;
   idummy[2] = 1;
   idummy[3] = 0;  // 0 = 3d,  2d = 1  ???

   //
   idummy[4] = 0;
   idummy[5] = 40;

   //////////// Handle density
   if ( densityMode != MFnFluid::kZero )
   {
      GET_ATTR( fdummy[6], densityScale );
   }
   else
   {
      fdummy[6] = 0.0f;
   }

   // density gradient or 8 if static/dynamic
   switch ( densityMode )
   {
      case MFnFluid::kGradient:
	 idummy[7] = densityGradient; break;
      case MFnFluid::kZero:
	 idummy[7] = 0; break;
      default:
	 idummy[7] = 8; break;
   }


   idummy[8] = 40;

   /////// Handle velocity
   MFnFluid::FluidGradient velocityGradient;
   MFnFluid::FluidMethod   velocityMode;
   fn.getVelocityMode( velocityMode, velocityGradient );
   if ( velocityMode != MFnFluid::kZero )
   {
      GET_ATTR( fdummy[9],  velocityScaleX );
      GET_ATTR( fdummy[10], velocityScaleY );
      GET_ATTR( fdummy[11], velocityScaleZ );
   }
   else
   {
      fdummy[9] = fdummy[10] = fdummy[11] = 0.0f;
   }

   switch ( velocityMode )
   {
      case MFnFluid::kGradient:
	 idummy[12] = velocityGradient; break;
      case MFnFluid::kZero:
	 idummy[12] = 0; break;
      default:
	 idummy[12] = 8; break;
   }

   // some grid data size
   unsigned    num = 15 + xres * yres * zres;
   unsigned offset = 40 + num * density;
   idummy[13] = offset;
   idummy[14] = offset;
   idummy[15] = offset;

   idummy[16] = 0;

   fdummy[17] = 1.0f;
   fdummy[18] = 1.0f;
   fdummy[19] = 1.0f;
   fdummy[20] = 1.0f;


   // density gradient or 8 if static/dynamic
   if ( colorMode == MFnFluid::kUseShadingColor )
      idummy[21] = 0;
   else
      idummy[21] = 8;

   idummy[22] = offset;


   // color offsets
   offset += num * color;
   idummy[23] = offset;
   offset += num * color;
   idummy[24] = offset;
   offset += num * color;

   if ( temperature )
   {
      GET_ATTR( fdummy[25], temperatureScale );
   }
   else
   {
      fdummy[25] = 0.0f;
   }

   switch ( temperatureMode )
   {
      case MFnFluid::kGradient:
	 idummy[26] = temperatureGradient; break;
      case MFnFluid::kZero:
	 idummy[26] = 0; break;
      default:
	 idummy[26] = 8; break;
   }

   idummy[27] = offset;
   offset += num * temperature;

   idummy[28] = 0;

   fdummy[29] = 1.0f;
   fdummy[30] = 0.5f; // --- todo: but not quite "cdcc4c3d" (NOT RES DEPENDANT)

   if ( fuel ) 
   {
      GET_ATTR( fdummy[31], fuelScale );
   }
   else
   {
      fdummy[31] = 0.0f;
   }
   switch ( fuelMode )
   {
      case MFnFluid::kGradient:
	 idummy[32] = fuelGradient; break;
      case MFnFluid::kZero:
	 idummy[32] = 0; break;
      default:
	 idummy[32] = 8; break;
   }

   idummy[33] = offset;
   idummy[34] = 0;

   idummy[35] = offset;
   idummy[36] = offset;
   idummy[37] = offset;
   idummy[38] = 0;

   idummy[39] = offset;

   /////////////////////////////////// this repeats on each dump
   idummy[40] = 0;
   idummy[41] = 0;
   idummy[42] = 0;

   // Write out resolution
   idummy[43] = xres;
   idummy[44] = yres;
   idummy[45] = zres;

   // Dimensions (Size in attribute editor)
   double dim[3];
   fn.getDimensions( dim[0], dim[1], dim[2] );
   fdummy[46] = (float) dim[0];
   fdummy[47] = (float) dim[1];
   fdummy[48] = (float) dim[2];

   // 
   fdummy[49] = (float) (dim[0] / xres);
   fdummy[50] = (float) (dim[1] / yres);
   fdummy[51] = (float) (dim[2] / zres);

   // boundaries
   GET_ATTR(idummy[52], boundaryX);
   GET_ATTR(idummy[53], boundaryY);
   GET_ATTR(idummy[54], boundaryZ);

   unsigned size = ( 4 * gridsize * ( density + temperature +
				      3 * color
				      ) + 160 +
		     60 * ( density + temperature + 3 * color )
		     );

   MRL_FILE* oldF = f;
   bool  oldBinary = options->exportBinary;
   if ( options->exportFluidFiles )
   {
      MString file = getFluidFile();
      MRL_FPRINTF(f, "data \"%s:fldata\" \"%s\"\n", name.asChar(), file.asChar() );

      options->exportBinary = true;
      f = MRL_FOPEN(file.asChar(), "wb");
   }
   else
   {
      options->exportBinary = false;
      MRL_FPRINTF(f, "data \"%s:fldata\"\n", name.asChar() );
      if ( size < 220 )       MRL_PUTS( "220 [\n" );
      else              MRL_FPRINTF( f, "%d  [\n", size );
      TAB(1);
   }


   // write out the header
   write_hex_floats( f, fdummy, 40 );
   if ( size < 220 ) write_hex_floats( f, &fdummy[40], 15 );


   // ------------------------- Spit data
   if ( density )
   {
      float* data = fn.density();
      write_hex_floats( f, &fdummy[40], 15 );
      write_hex_floats( f, data, gridsize );
   }

   if ( temperature )
   {
      float* data = fn.temperature();
      write_hex_floats( f, &fdummy[40], 15 );
      write_hex_floats( f, data, gridsize );
   }

   if ( colorMode != MFnFluid::kUseShadingColor )
   {
      float* r, *g, *b;
      fn.getColors( r, g, b );
      write_hex_floats( f, &fdummy[40], 15 );
      write_hex_floats( f, r, gridsize );
      write_hex_floats( f, &fdummy[40], 15 );
      write_hex_floats( f, g, gridsize );
      write_hex_floats( f, &fdummy[40], 15 );
      write_hex_floats( f, b, gridsize );
   }

   if ( options->exportFluidFiles )
   {
      MRL_FCLOSE(f);  f = oldF;
   }
   else
   {
      TAB(1); MRL_PUTS( "]\n");
      NEWLINE();
   }

   options->exportBinary = oldBinary;

   MRL_FPRINTF(f, "data \"%s:shape:data\"\n", name.asChar() );
   TAB(1); MRL_PUTS( "\"maya_objectdata\" (\n" );
   TAB(2); MRL_PUTS( "\"magic\" 1298233697,\n" );
   TAB(2); MRL_PUTS( "\"uvSpace\" -1,\n" );
   TAB(2); MRL_PUTS( "\"renderableObjectType\" 0,\n" );
   TAB(2); MRL_PUTS( "\"volumeSamples\" 1,\n" );
   TAB(2); MRL_PUTS( "\"depthJitter\" off\n" );
   TAB(1); MRL_PUTS( ")\n");
   NEWLINE();
}

/** 
 * Write object properties.
 * 
 * @param f 
 */
void mrFluid::write_properties( MRL_FILE* f )
{
   mrObject::write_properties(f);
   TAB(1); MRL_FPRINTF( f, "data \"%s:shape:data\"\n", name.asChar() );
}



#ifdef GEOSHADER_H
#include "raylib/mrFluid.inl"
#endif
