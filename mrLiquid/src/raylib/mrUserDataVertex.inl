/**
 * @file   mrUserDataVertex.inl
 * @author gga
 * @date   Fri Aug  3 06:38:55 2007
 * 
 * @brief  
 * 
 * 
 */


#undef WRITE_FLOAT

#define WRITE_FLOAT(tmp)    { scalars.push_back( tmp ); }



void mrUserDataVertex::write_trilist( std::vector< miScalar >& scalars, 
				      const unsigned idx )
{   
  MStatus status;
  mrUserVectors::iterator i = userAttrs.begin();
  mrUserVectors::iterator e = userAttrs.end();
  MFnDependencyNode dep( path.node() );

  float tmp;
  for ( ; i != e; ++i )
    {
      const mrVertexData& v = *i;
      
      MString attrName = "miCustomTex" + v.attr;
      MPlug p = dep.findPlug( attrName, true, &status );
      
      MObject data;
      p.getValue(data);
      if ( data.hasFn( MFn::kIntArrayData ) )
	{
	  MFnIntArrayData fn( data );
	  tmp = (float) fn.array()[idx];
	  WRITE_FLOAT(tmp);
	}
      else if ( data.hasFn( MFn::kDoubleArrayData ) )
	{
	  MFnDoubleArrayData fn( data );
	  tmp = (float) fn.array()[idx];
	  WRITE_FLOAT(tmp);
	}
      else if ( data.hasFn( MFn::kVectorArrayData ) )
	{
	  MFnVectorArrayData fn( data );
	  const MVector& v = fn.array()[idx];
	  tmp = (float) v[0];
	  WRITE_FLOAT(tmp);
	  tmp = (float) v[1];
	  WRITE_FLOAT(tmp);
	  tmp = (float) v[2];
	  WRITE_FLOAT(tmp);
	}
      else if ( data.hasFn( MFn::kPointArrayData ) )
	{
	  MFnPointArrayData fn( data );
	  const MPoint& v = fn.array()[idx];
	  tmp = (float) v[0];
	  WRITE_FLOAT(tmp);
	  tmp = (float) v[1];
	  WRITE_FLOAT(tmp);
	  tmp = (float) v[2];
	  WRITE_FLOAT(tmp);
	  tmp = (float) v[3];
	  WRITE_FLOAT(tmp);
	}
    }
  
}



#undef WRITE_VECTOR
#define WRITE_VECTOR( tmp ) { vectors.push_back( tmp ); pos = 0; }


void mrUserDataVertex::write_trilist( std::vector< miVector >& vectors, 
				      const unsigned idx )
{
   MStatus status;
   mrUserVectors::iterator i = userAttrs.begin();
   mrUserVectors::iterator e = userAttrs.end();
   MFnDependencyNode dep( path.node() );

   miVector tmp;
   float*   t = (float*) &tmp;
   unsigned pos = 0;
   for ( ; i != e; ++i )
     {
       const mrVertexData& v = *i;
       
       MString attrName = "miCustomTex" + v.attr;
       MPlug p = dep.findPlug( attrName, true, &status );
       
       MObject data;
       p.getValue(data);
       if ( data.hasFn( MFn::kIntArrayData ) )
	 {
	   MFnIntArrayData fn( data );
	   t[pos++] = (float) fn.array()[idx];
	   if ( pos > 2 ) WRITE_VECTOR( tmp );
	 }
       else if ( data.hasFn( MFn::kDoubleArrayData ) )
	 {
	   MFnDoubleArrayData fn( data );
	   t[pos++] = (float) fn.array()[idx];
	   if ( pos > 2 ) WRITE_VECTOR( tmp );
	 }
       else if ( data.hasFn( MFn::kVectorArrayData ) )
	 {
	   if ( pos ) WRITE_VECTOR( tmp );

	   MFnVectorArrayData fn( data );
	   const MVector& v = fn.array()[idx];
	   tmp.x = (float) v[0];
	   tmp.y = (float) v[1];
	   tmp.z = (float) v[2];
	   WRITE_VECTOR( tmp );
	 }
       else if ( data.hasFn( MFn::kPointArrayData ) )
	 {
	   if ( pos ) WRITE_VECTOR( tmp );

	   MFnPointArrayData fn( data );
	   const MPoint& v = fn.array()[idx];
	   tmp.x = (float) v[0];
	   tmp.y = (float) v[1];
	   tmp.z = (float) v[2];
	   WRITE_VECTOR( tmp );
	   
	   t[pos++] = (float) v[3];
	 }
     }

   if ( pos ) WRITE_VECTOR( tmp );
}

/** 
 * 
 * 
 * 
 * @return 
 */
unsigned mrUserDataVertex::write_user()
{
   unsigned num = 0;
   MStatus status;
   mrUserVectors::iterator i = userAttrs.begin();
   mrUserVectors::iterator e = userAttrs.end();
   MFnDependencyNode dep( path.node() );
   for ( ; i != e; ++i )
   {
      const mrVertexData& v = *i;

      MString attrName = "miCustomTex" + v.attr;
      MPlug p = dep.findPlug( attrName, &status );
      
      MObject data;
      p.getValue(data);
      if ( data.hasFn( MFn::kIntArrayData ) )
      {
	 MFnIntArrayData fn( data );
	 mrObject::write_user_vectors( fn.array() );
	 ++num;
      }
      else if ( data.hasFn( MFn::kDoubleArrayData ) )
      {
	 MFnDoubleArrayData fn( data );
	 mrObject::write_user_vectors( fn.array() );
	 ++num;
      }
      else if ( data.hasFn( MFn::kVectorArrayData ) )
      {
	 MFnVectorArrayData fn( data );
	 mrObject::write_user_vectors( fn.array() );
	 ++num;
      }
      else if ( data.hasFn( MFn::kPointArrayData ) )
      {
	 MFnPointArrayData fn( data );
	 mrObject::write_user_vectors( fn.array() );
	 num += 2;
      }
   }
   return num;
}


void mrUserDataVertex::write()
{
  if ( written == kWritten ) return;
  else if ( written == kIncremental )
    mi_api_incremental( miTRUE );
  else
    mi_api_incremental( miFALSE );

  MRL_FUNCTION( "maya_vertexdata" );
      
  MRL_PARAMETER( "magic" );
  int intValue = 1298749048;
  MRL_INT_VALUE( &intValue );

  MRL_PARAMETER( "data" );
  MRL_CHECK(mi_api_parameter_push( miTRUE )); // it is an array

  mrUserVectors::iterator i = userAttrs.begin();
  mrUserVectors::iterator e = userAttrs.end();
  for ( ; i != e; ++i )
    {
      MRL_CHECK(mi_api_new_array_element());
      MRL_CHECK(mi_api_parameter_push( miFALSE )); // it is a struct
      MRL_PARAMETER( "name" );
      MRL_STRING_VALUE( i->attr.asChar() );

      MRL_PARAMETER( "type" );
      int intValue = i->type;
      MRL_INT_VALUE( &intValue );
	 
      MRL_PARAMETER( "size" );
      intValue = i->size;
      MRL_INT_VALUE( &intValue );
	 
      MRL_PARAMETER( "offset" );
      intValue = i->offset;
      MRL_INT_VALUE( &intValue );
	 
      MRL_CHECK(mi_api_parameter_pop()); // back from struct
    }
  MRL_CHECK(mi_api_parameter_pop()); // back from array

  function =  mi_api_function_call_end( function );
  assert( function != miNULLTAG );
      
  mi_api_data_begin( MRL_MEM_STRDUP( name.asChar() ), 2, (void*)function );
  tag = mi_api_data_end();
  assert( tag != miNULLTAG );
}
