

/** 
 * Main virtual routine to write shader parameters
 * 
 * @param dep 
 * @param connNames (unused) in curve
 */
void 
mrShaderAnimCurveUU::write_shader_parameters( MFnDependencyNode& dep,
					      const ConnectionMap& connNames )
{
   MRL_PARAMETER( "keys" );
   MRL_CHECK(mi_api_parameter_push( miTRUE ));

   MFnAnimCurve fn( dep.object() );

   MAngle inA, outA;
   double w;
   float oneThird = 1.0f / 3.0f;
   
   unsigned numKeys = fn.numKeys();
   for ( unsigned i = 0; i < numKeys; ++i )
   {
      MRL_CHECK(mi_api_new_array_element());
      MRL_CHECK(mi_api_parameter_push( miFALSE ));

      miScalar value = (miScalar) fn.time(i).value();
      MRL_PARAMETER("time");
      MRL_SCALAR_VALUE( &value );

      value = (miScalar) fn.value( i );
      MRL_PARAMETER("value");
      MRL_SCALAR_VALUE( &value );

      fn.getTangent(i, inA, w, true);
      double r = inA.asRadians();
      miScalar x = (miScalar) (3.0 * cos( r ) * w);
      miScalar y = (miScalar) (tan( r ) * x);

      MRL_PARAMETER("tanInX");
      MRL_SCALAR_VALUE(&x);
      MRL_PARAMETER("tanInY");
      MRL_SCALAR_VALUE(&y);

      fn.getTangent( i, outA, w, false );
      r = outA.asRadians();
      x = (miScalar) (3.0 * cos( r ) * w);
      y = (miScalar) (tan( r ) * x);

      MRL_PARAMETER("tanOutX");
      MRL_SCALAR_VALUE(&x);
      MRL_PARAMETER("tanOutY");
      MRL_SCALAR_VALUE(&y);
      
      MRL_CHECK(mi_api_parameter_pop());
   }

   MRL_CHECK(mi_api_parameter_pop());

   miBoolean weighted = (miBoolean) fn.isWeighted();
   MRL_PARAMETER("isWeighted");
   MRL_BOOL_VALUE( &weighted );


   MRL_PARAMETER("preInfinity");
   int intV = fn.preInfinityType();
   MRL_INT_VALUE( &intV );

   MRL_PARAMETER("postInfinity");
   intV = fn.preInfinityType();
   MRL_INT_VALUE( &intV );

}

void mrShaderAnimCurveUU::write_curve( const ConnectionMap& connNames )
{
   MFnDependencyNode dep( nodeRef() );
   char oldWritten = written;
   written = kInProgress;
   DBG2(name << ": Done with getConnection names");

   if ( oldWritten == kIncremental )
      mi_api_incremental( miTRUE );
   else
      mi_api_incremental( miFALSE );
      
   MRL_FUNCTION( "maya_curve" );
   write_shader_parameters( dep, connNames );
   tag = mi_api_function_call_end( tag );
   char nameStr[256];
   sprintf( nameStr, "%s:curve", name.asChar() );
   MRL_CHECK(mi_api_shader_add( MRL_MEM_STRDUP( nameStr ),
				tag ));
}


void mrShaderAnimCurveUU::write()
{

   DBG( name << ": mrShaderAnimCurveUU::write()" );

   if ( options->exportFilter & mrOptions::kShaderDef )
      return;

   MFnDependencyNode dep( nodeRef() );
   
   if ( written == kWritten )
   {
      written = kInProgress;
      updateChildShaders( dep ); 
      written = kWritten;
      return;
   }


   char oldWritten = written;
   written = kInProgress;
   ConnectionMap connNames;
   getConnectionNames( connNames, dep );
   DBG2(name << ": Done with getConnection names");


   
   if ( written == kIncremental )
      mi_api_incremental( miTRUE );
   else
      mi_api_incremental( miFALSE );

   MRL_FUNCTION( shaderName.asChar() );
   write_shader_parameters( dep, connNames );
   curveTag = mi_api_function_call_end( curveTag );

   MString fullname = name + ":curve";
   MRL_CHECK(mi_api_shader_add( MRL_MEM_STRDUP( fullname.asChar() ),
				curveTag ));
   
   MRL_FUNCTION( "maya_scalar_curve" );
   MRL_PARAMETER( "input" );
   MString key = name + ".input";
   MRL_ASSIGN( connNames[key.asChar()].asChar() ); 
   MRL_PARAMETER( "curve" );
   MRL_STRING_VALUE( fullname.asChar() );
   tag = mi_api_function_call_end( tag );
   MRL_CHECK(mi_api_shader_add( MRL_MEM_STRDUP( name.asChar() ),
				tag ));


   written = kWritten;

}
