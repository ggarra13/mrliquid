
void mrFluid::write_group()
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
   write_vectors( v );

   for ( short i = 0; i < 8; ++i )
   {
      MRL_CHECK(mi_api_vertex_add( i ));
   }

   static const int polyIdx[] = {
   0,1,3,2,
   0,1,5,4,
   0,4,6,2,
   1,5,7,3,
   2,3,7,6,
   5,4,6,7
   };

   const int* vidx = polyIdx;
   for ( int j = 0; j < 6; ++j )
   {
      mi_api_poly_begin_tag( 1, miNULLTAG );
      mi_api_poly_index_add( 0 );
      for ( int i = 0; i < 4; ++i, vidx++ )
	 mi_api_poly_index_add( *vidx );
      mi_api_poly_end();
   }
}







/** 
 * Write user data for object.
 * 
 * @param f 
 */
void mrFluid::write_fluid_data()
{
   if ( written == kWritten || 
	(options->exportFilter & mrOptions::kUserData) ) return;

   mrObject::write_user_data();
   
   if ( written == kIncremental )
      mi_api_incremental( miTRUE );


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

   bool  oldBinary = options->exportBinary;

   char nameStr[256];
   sprintf( nameStr, "%s:fldata", name.asChar() );

   if ( options->exportFluidFiles )
   {
      MString file = getFluidFile();
      mi_api_data_begin( MRL_MEM_STRDUP( nameStr ), 1, (void*)
			 MRL_MEM_STRDUP( file.asChar() ) );

      options->exportBinary = true;
      FILE* f = fopen(file.asChar(), "wb");

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

      fclose(f);
   }
   else
   {
      if ( size < 220 ) size = 220;
      mi_api_data_begin( MRL_MEM_STRDUP( nameStr ), 0, (void*)
			 size );

      float* fldata = new float[size / sizeof(float)];
      float*   curr = fldata;

      // write out the header
      write_hex_floats( curr, fdummy, 40 );
      if ( size < 220 ) write_hex_floats( curr, &fdummy[40], 15 );  

      // ------------------------- Spit data
      if ( density )
      {
	 float* data = fn.density();
	 write_hex_floats( curr, &fdummy[40], 15 );
	 write_hex_floats( curr, data, gridsize );
      }

      if ( temperature )
      {
	 float* data = fn.temperature();
	 write_hex_floats( curr, &fdummy[40], 15 );
	 write_hex_floats( curr, data, gridsize );
      }

      if ( colorMode != MFnFluid::kUseShadingColor )
      {
	 float* r, *g, *b;
	 fn.getColors( r, g, b );
	 write_hex_floats( curr, &fdummy[40], 15 );
	 write_hex_floats( curr, r, gridsize );
	 write_hex_floats( curr, &fdummy[40], 15 );
	 write_hex_floats( curr, g, gridsize );
	 write_hex_floats( curr, &fdummy[40], 15 );
	 write_hex_floats( curr, b, gridsize );
      }

      mi_api_data_byte_copy( size, (miUchar*) fldata);
   }

   options->exportBinary = oldBinary;

   // End of data
   miTag newData = mi_api_data_end();
   if ( userData == miNULLTAG ) userData = newData;
   else userData = mi_api_data_append( userData, newData );


   ///////////////////// Shape data

   sprintf( nameStr, "%s:shape:data", name.asChar() );
   MRL_FUNCTION("maya_objectdata");

   MRL_PARAMETER("magic");
   miInteger valI = 1298233697;
   MRL_INT_VALUE( &valI );

   valI = -1;
   MRL_PARAMETER("uvSpace");
   MRL_INT_VALUE( &valI );

   valI = 0;
   MRL_PARAMETER("renderableObjectType");
   MRL_INT_VALUE( &valI );

   valI = 1;
   MRL_PARAMETER("volumeSamples");
   MRL_INT_VALUE( &valI );

   miBoolean valB = miFALSE;
   MRL_BOOL_VALUE( &valB );

   function = mi_api_function_call_end( function );
   assert( function != miNULLTAG );

   sprintf( nameStr, "%s:data", name.asChar() );
   mi_api_data_begin( MRL_MEM_STRDUP( nameStr ), 2, (void*)function );
   newData = mi_api_data_end();
   if ( userData == miNULLTAG ) userData = newData;
   else userData = mi_api_data_append( userData, newData );
}



