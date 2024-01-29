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

// Auxiliary macro to obtain and write child channel value or connection
#undef GET_COMP
#define GET_COMP( NAME, IDX ) \
        MRL_FUNCTION( #NAME ); \
	cp = p.child( IDX ); \
	if ( cp.isConnected() ) \
	{ \
	   cp.connectedTo( plugs, true, false ); \
	   if ( plugs.length() == 0 ) \
	   { \
	      cp.getValue( x ); \
              MRL_SCALAR_VALUE( &x ); \
	   } \
	   else \
	   { \
               MString destName; \
               MFnDependencyNode d( plugs[0].node() ); \
               if ( isComponentPlug( plugs[0] ) ) \
               { \
                  MPlug cp = plugs[0].parent(); \
		  attrName = getStringForChildPlug( cp, ">" ); \
                  char* destChannel = getLowercaseComponent( plugs[0] ); \
		  destName = d.name() + ">" + attrName + "." + destChannel; \
	       } \
	       else \
	       { \
                  if ( s->numOutAttrs > 1 ) \
		  { \
	            if ( !container ) \
	            { \
		       destName = plugs[0].info(); \
	            } \
	            else \
	            { \
		       MString plugName = plugs[0].info(); \
		       int idx = plugName.rindex('.'); \
		       destName = plugName.substring(0, idx-1); \
		       destName += "-"; \
		       destName += container; \
		       destName += plugName.substring(idx,  \
						      plugName.length()-1); \
	            } \
		  } \
		  else { \
		     destName = d.name(); \
		   } \
	       } \
	       MRL_SHADER( destName.asChar() ); \
	    } \
	 } \
	 else \
	 { \
	    cp.getValue( x ); \
            MRL_SCALAR_VALUE( &x ); \
	 }


   
//
// As mray uses colors with alpha, we need to check if this
// compound was a color.  If it was, we need to pass alpha
// as 1, which is what maya2mr does.
//
// Also, miVector2d is not exposed in .mi scene format, so
// all vectors are passed as triplets (WASTE OF MEMORY!!!!)
// in maya shaders.
// For example, place2dTexture uses SEVEN 3D vectors.
//
void mrShader::dealWithComponent( const ConnectionMap& connNames,
				  const unsigned numChildren,
				  const MPlug& ap )
{
   // Verify no child is connected individually
   bool connected = false;
   MPlug cp;
   unsigned j;
   for ( j = 0; j < numChildren; ++j )
   {
      cp = ap.child( j );
      if ( cp.isConnected() )
      {
	 connected = true; break;
      }
   }
   if ( connected )
   {
      std::string key( cp.name().asChar() );
      ConnectionMap::const_iterator it = connNames.find( key );
      if ( it != connNames.end() )
      {
	 const MString& conn = it->second;
	 MRL_SHADER( conn.asChar() );
	 return;
      }
   }

   if ( numChildren == 2 )
   {
      // dealing with a uv vector
      miVector valV;
      cp = ap.child(0);
      cp.getValue( valV.x );
      cp = ap.child(1);
      cp.getValue( valV.y );
      valV.z = 0.0f;
      MRL_VECTOR_VALUE( &valV );
   }
   else if ( numChildren == 3 )
   {
      miColor valC;
      cp = ap.child(0);
      cp.getValue( valC.r );
      cp = ap.child(1);
      cp.getValue( valC.g );
      cp = ap.child(2);
      cp.getValue( valC.b );
      valC.a = 0.0f;

      // If we are dealing with a color parameter in
      // a non-maya custom shader, pass alpha as 1
      if (
	  dynamic_cast< mrShaderMaya* >( this ) == NULL &&
	  dynamic_cast< mrShaderLight* >( this ) == NULL &&
	  dynamic_cast< mrShaderLightSpot* >( this ) == NULL 
	  )
      {
	 MString attrName = ap.child(2).partialName( false, true, true,
						     false, true, true );
	 if ( attrName.asChar()[ attrName.length() - 1 ] == 'B' )
	 {
	    valC.a = 1.0f;
	    MFnDependencyNode fn( ap.node() );
	    attrName = attrName.substring(0, attrName.length() - 1 ) + "A";
	    MStatus status;
	    MPlug p = fn.findPlug(attrName, &status);
	    if ( status == MS::kSuccess )
	    {
	       p.getValue( valC.a );
	    }
	 }
      }
      MRL_COLOR_VALUE( &valC );
   }
}


void mrShader::handleParameter( unsigned& i, const MPlug& ap,
				const ConnectionMap& connNames,
				const bool skip )
{
   // This was a simple check to catch parser's bug if handleParameter
   // is called recursively in an infinite way.
#ifdef _DEBUG
   assert( i != lastParam );
   lastParam = i;
#endif
   
   if ( ap.isConnected() ||
	( ap.isCompound() && ap.numConnectedChildren() > 0) )
   {
      std::string key( ap.name().asChar() );
      ConnectionMap::const_iterator it = connNames.find( key );
      if ( it != connNames.end() )
      {
	 const MString& conn = it->second;
	 if ( ap.isElement() || 
	      ap.attribute().hasFn( MFn::kMessageAttribute ) )
	 {
	    //! @todo: verify this one
	    MRL_STRING_VALUE( conn.asChar() );
	 }
	 else
	 {
	    MRL_SHADER( conn.asChar() );
	 }
	 
	 ++i;
	 if ( ap.isCompound() )
	    i += ap.numChildren();
	 return;
      }
      else
      {
	 MPlugArray plugs;
	 ap.connectedTo( plugs, true, false );
	 if ( plugs.length() > 0 )
	 {
	    MObject node = plugs[0].node();
	    if ( node.hasFn( MFn::kPluginDependNode ) )
	    {
	       MFnDependencyNode fn( node );
	       if ( fn.typeId().id() == kMentalrayPhenomenon )
	       {
		  MString attrName = ap.partialName( false, false, false,
						     false, false, true );
		  char* dummy = strdup( attrName.asChar() );
		  char* s = dummy;
		  for ( ; *s != 0; ++s )
		  {
		     if ( *s == '[' || *s == ']' || *s == '.' ) *s = '_';
		  }
		  MRL_INTERFACE( dummy );
		  free(dummy);

		  ++i;
		  if ( ap.isCompound() )
		     i += ap.numChildren();
		  return;
	       }
	    }
	    
	    MObject attrObj = ap.attribute();
	    if ( !skip && options->exportVerbosity > 5 &&
		 (! ( node.hasFn( MFn::kAnimCurve ) ||
		      node.hasFn( MFn::kExpression ) ||
		      node.hasFn( MFn::kTime ) ||
		      attrObj.hasFn( MFn::kMatrixAttribute ) ||
		      attrObj.hasFn( MFn::kFloatMatrixAttribute ) )
		  ) )
	    {
	       MString err( ap.info() );
	       err += " was connected but connection was not matched.  "
	       "Current value used.";
	       LOG_ERROR( err );
	    }
	 }
      }
   }
   
   MObject attrObj = ap.attribute();
   
   if ( ap.isArray() )
   {
      bool compound = ap.isCompound();
      if (!skip) MRL_CHECK(mi_api_parameter_push( miTRUE ));
      unsigned numElements = ap.numElements();
      unsigned numConnectedElements = ap.numConnectedElements();

      unsigned ij = i + 1;  // needed for numElements == 0
      
      if ( numConnectedElements == 0 )
      {
	 if ( attrObj.hasFn( MFn::kMessageAttribute ) )
	 {
	    MString attrName = ap.partialName( false, true, true,
					       false, true, true );
	    if ( attrName == "lights" || attrName == "light" )
	    {
	       if (!skip) {
#ifdef MR_LIGHT_GROUPS
		  MRL_STRING_VALUE( "!AllLights::inst" );
#else
		  mrGroupInstance* allLights = mrGroupInstance::factory( "!AllLights" );
		  mrGroupInstance::iterator i = allLights.begin();
		  mrGroupInstance::iterator e = allLights.end();
		  for ( ; i != e; ++i )
		  {
		     MRL_CHECK(mi_api_new_array_element());
		     MRL_STRING_VALUE( (*i)->name );
		  }
#endif
	       }
	    }
	 }
      }
      
      if ( numElements == 0 && compound )
      {
	 // We need to still skip parameters as if there
	 // was an element in the array as
	 // maya leaves a [-1] index as stub.
	 unsigned numChildren = ap.numChildren();
	 for ( unsigned c = 0; c < numChildren; ++c )
	 {
	    MPlug cp = ap.child( c );
	    handleParameter( ij, cp, connNames, true );
	 }
      }

      if ( attrObj.hasFn( MFn::kMessageAttribute ) )
      {
	 for (unsigned j = 0; j < numElements; ++j )
	 {
	    MPlug pp = ap.elementByPhysicalIndex( j );
	    if ( ! pp.isConnected() ) continue;
	    
	    ij = i + 1;  // For array attributes, we reset on each iteration
	    if (!skip) MRL_CHECK(mi_api_new_array_element());
	    handleParameter( ij, pp, connNames );
	 }
	 if (!skip) MRL_CHECK(mi_api_parameter_pop());
	 i = ij;
	 return;
      }
      
      for (unsigned j = 0; j < numElements; ++j )
      {
	 MPlug pp = ap.elementByPhysicalIndex( j );
	 if ( pp.isNull() ) continue;

	 bool skipchild = skip;
	 unsigned idx = pp.logicalIndex();
	 if ( idx == 0xffffffff ) skipchild = true;
	    
	 if (!skipchild) MRL_CHECK(mi_api_new_array_element());
	 ij = i + 1;  // For array attributes, we reset this on each iteration
	 if ( compound )
	 {
	    unsigned numChildren = ap.numChildren();
	    
	    MPlug cp = pp.child( 0 );
	    if ( isComponentPlug( cp ) )
	    {
	       if (!skipchild)
		  dealWithComponent( connNames, numChildren, pp );
	       ij += numChildren;
	    }
	    else
	    {
	       if ( !skipchild )
		  MRL_CHECK(mi_api_parameter_push( miFALSE ));
	       for ( unsigned c = 0; c < numChildren; ++c )
	       {
		  MPlug cp = pp.child( c );
		  if(!skipchild)
		  {
		     MString attrName = cp.partialName( false, false, false,
							false, false, true );
		     MStringArray split;
		     attrName.split( '_', split);
		     if ( split.length() < 2 )
			attrName.split( '.', split);
		     if ( split.length() > 1 )
			attrName = split[ split.length() - 1 ];
		     attrName = ( attrName.substring(0,0).toLowerCase() +
				  attrName.substring(1, attrName.length()-1) );
		     MRL_PARAMETER( attrName.asChar() );
		  }
		  handleParameter( ij, cp, connNames, skipchild );
	       }
	       if ( !skipchild )
		  MRL_CHECK(mi_api_parameter_pop());
	    }
	 }
	 else
	 {
	    handleParameter( ij, pp, connNames );
	 }
      }
      i = ij;
      
      if (!skip) MRL_CHECK(mi_api_parameter_pop());
      return;
   }

      
   if ( ap.isCompound() )
   {
      i += 1;
      unsigned numChildren = ap.numChildren();

      MPlug cp = ap.child( 0 );
      if ( isComponentPlug( cp ) )
      {
	 if ( !skip ) dealWithComponent( connNames, numChildren, ap );
	 i += numChildren;
      }
      else
      {
	 if ( !skip )
	    MRL_CHECK(mi_api_parameter_push( miFALSE ));
	 for ( unsigned c = 0; c < numChildren; ++c )
	 {
	    MPlug cp = ap.child( c );
	    if(!skip)
	    {
	       MString attrName = cp.partialName( false, false, false,
						  false, false, true );
	       MStringArray split;
	       attrName.split( '_', split);
	       if ( split.length() < 2 )
		  attrName.split( '.', split);
	       if ( split.length() > 1 )
		  attrName = split[ split.length() - 1 ];
	       attrName = ( attrName.substring(0,0).toLowerCase() +
			    attrName.substring(1, attrName.length()-1) );
	       MRL_PARAMETER( attrName.asChar() );
	    }
	    handleParameter( i, cp, connNames, skip );
	 }
	 if ( !skip )
	    MRL_CHECK(mi_api_parameter_pop());
      }

      return;
   }

   if ( skip )
   {
      i++;
      return;
   }

   if ( attrObj.hasFn( MFn::kEnumAttribute ) )
   {
      miInteger valS;
      ap.getValue( valS );
      MRL_INT_VALUE( &valS );
   }
   else if ( attrObj.hasFn( MFn::kNumericAttribute ) )
   {
      MFnNumericAttribute n( attrObj );
	    
      switch( n.unitType() )
      {
	 case MFnNumericData::kBoolean:
	    {
	       bool  valB;
	       ap.getValue( valB );
	       miBoolean b = valB? miTRUE : miFALSE;
	       MRL_BOOL_VALUE( &b );
	       break;
	    }
	 case MFnNumericData::kShort:
	 case MFnNumericData::kInt:
	    {
	       miInteger valI;
	       ap.getValue( valI );
               if ( options->progressive && valI > 1 )
	       {
		  MString attrName = ap.partialName( false, true, true,
						     false, true, true );
		  int len = attrName.length();
		  if ( len >= 4 )
		  {
		     const char* s = attrName.asChar() + len - 4;
		     if ( strcmp( s, "Rays" ) == 0 )
		     {
			valI = (short) (valI * options->progressivePercent);
			if ( valI < 1 ) valI = 1;
			progressiveLast = options->progressivePercent;
		     }
		     else if ( len >= 7 )
		     {
			s -= 3;
			if ( (strcmp( s, "Samples" ) == 0) ||
			     (strcmp( s, "samples") == 0) )
			{
			   valI = (short) (valI * options->progressivePercent);
			   if ( valI < 1 ) valI = 1;
			   progressiveLast = options->progressivePercent;
			}
		     }
		  }
	       }
	       MRL_INT_VALUE( &valI );
	       break;
	    }
	 case MFnNumericData::kFloat:
	 case MFnNumericData::kDouble:
	    {
	       miScalar valF;
	       ap.getValue( valF );
	       MRL_SCALAR_VALUE( &valF );
	       break;
	    }
	 default:
	    mrTHROW("Invalid numeric type");
      }
   }
   else if ( attrObj.hasFn( MFn::kTypedAttribute ) )
   {
      MString valString;
      ap.getValue( valString );

      currentNode = nodeHandle.objectRef();
      MRL_STRING_VALUE( parseString(valString).asChar() );
   }
   else if ( attrObj.hasFn( MFn::kMessageAttribute ) )
   {
      MRL_STRING_VALUE( "" );
   }
   // Note hasFn( MFn::FloatAngleAttribute ) does not work, we check for
   // equality.
   else if ( attrObj.apiType() == MFn::kDoubleAngleAttribute ||
	     attrObj.apiType() == MFn::kFloatAngleAttribute )
   {
      MAngle t;
      ap.getValue( t );
      miScalar valF = (miScalar) t.asRadians();
      MRL_SCALAR_VALUE( &valF );
   }
   // Note hasFn( MFn::kTimeAttribute ) does not work, we check for
   // equality.
   else if ( attrObj.apiType() == MFn::kTimeAttribute )
   {
      MTime t;
      ap.getValue( t );
      miScalar valF = (miScalar) t.as( MTime::uiUnit() );
      MRL_SCALAR_VALUE( &valF );
   }
   // Note hasFn( MFn::kDoubleLinearAttribute ) does not work, we check for
   // equality.
   else if ( attrObj.apiType() == MFn::kDoubleLinearAttribute ||
	     attrObj.apiType() == MFn::kFloatLinearAttribute )
   {
      MDistance t;
      ap.getValue( t );
      miScalar valF = (miScalar) t.as( MDistance::uiUnit() );
      MRL_SCALAR_VALUE( &valF );
   }
   else if ( attrObj.hasFn( MFn::kMatrixAttribute ) ||
	     attrObj.hasFn( MFn::kFloatMatrixAttribute ) )
   {
      MObject t;
      ap.getValue( t );
      MFnMatrixData fn( t );
      const MMatrix& m = fn.matrix();
      miMatrix mF;
      mrl_maya_matrix( mF, m );
      MRL_MATRIX_VALUE( &mF );
   }
   else if ( attrObj.hasFn( MFn::kGenericAttribute ) )
   {
      MObject tmp;
      ap.getValue( tmp );
      MFnDependencyNode d( tmp );
      MRL_STRING_VALUE( d.name().asChar() );
   }
   else
   {
      MString err( "Unknown attribute type in shader: " );
      err += ap.name() + " " + attrObj.apiType() + " " + attrObj.apiTypeStr();
      mrTHROW( err );
   }
   
   ++i;
}


void mrShader::write_shader_parameters( MFnDependencyNode& dep,
					const ConnectionMap& connNames )
{
   // Find parameters...
   unsigned numAttrs = dep.attributeCount();
   unsigned inc;

   numOutAttrs = 0;
   for ( unsigned i = START_ATTR; i < numAttrs; i += inc )
   {
      MObject attrObj = dep.attribute(i);
      MPlug ap( nodeHandle.objectRef(), attrObj );

      inc = 1;
      if ( ap.isCompound() ) inc += ap.numChildren();
      if ( ap.isProcedural() ) continue;

      MFnAttribute fnAttr(attrObj);
      bool isReadable = fnAttr.isReadable();
      bool isWritable = fnAttr.isWritable();
      bool isHidden   = fnAttr.isHidden();
      if ( !isReadable || !isWritable || isHidden ) continue;

      const MString& attrName = ap.partialName( false, true, true,
						false, true, true );

      const char* attrAsChar = attrName.asChar();
      // Disregard any parameter that begins with "out", like
      // outValue, outColor, outUV, etc.
      if ( strncmp( attrAsChar, "out", 3 ) == 0 ) {
	 continue;
      }

      // These two seem to be present in any custom shader node.
      // What for?  Who knows?
      if ( attrName == "miFactoryNode" ||
	   attrName == "miForwardDefinition" ) continue;
      
      
      inc = 0;
      MRL_PARAMETER( attrAsChar );
      handleParameter( i, ap, connNames );
   }

}


MString mrShader::newOutShader( const MPlug& p,
				const char* container )
{
   // Then, we get the parent (main) plug of the compount
   MPlug pp = p.parent();
   
   MString attrName = getStringForPlug( pp, ">" );
   MFnDependencyNode d( p.node() );
   
   MString newOutShader = d.name();
   if ( container )
   {
      newOutShader += "-";
      newOutShader += container;
   }
   newOutShader += ">";
   newOutShader += attrName;
   
   AuxShaders::iterator ie = auxShaders.end();
   if ( std::find( auxShaders.begin(), ie, pp ) != ie )
      return newOutShader;  // we already did this shader with another plug
   auxShaders.push_back( pp );
   
   MString newOutConn = d.name() + "." + attrName;

   miTag function = miNULLTAG;
   if ( written == kIncremental )
   {
      function = MRL_ASSIGN( newOutShader.asChar() );
   }

   // Now, we get the last letter of compound to help us
   // identify its type.
   BridgeType type = getBridgeType( pp );

   switch( type )
   {
      case kRGB:
	 MRL_FUNCTION( "maya_color_to_rgb" );
	 MRL_PARAMETER( "color" );
	 break;
      case kHSV:
	 MRL_FUNCTION( "maya_color_to_hsv" );
	 MRL_PARAMETER( "color" );
	 break;
      case kXYZ:
	 MRL_FUNCTION( "maya_vector_to_xyz" );
	 MRL_PARAMETER( "vector" );
	 break;
      case kUVW:
	 MRL_FUNCTION( "maya_vector_to_uvw" );
	 MRL_PARAMETER( "vector" );
	 break;
   }

   MRL_SHADER( newOutConn.asChar() );
   
   function = mi_api_function_call_end( function );
   MRL_CHECK(mi_api_shader_add( MRL_MEM_STRDUP( newOutShader.asChar() ),
				function ));
   return newOutShader;
}


void mrShader::getConnectionNames( ConnectionMap& connNames,
				   const MFnDependencyNode& dep )
{
   MPlugArray connAttrs;
   dep.getConnections( connAttrs );

   const char* container = NULL;
   {
      int cont = name.rindex('-');
      if ( cont >= 0 ) container = name.asChar() + cont + 1;
   }
   
   ////////////////////////////////////////////////////////////////////////
   // OK, first we iterate thru all the connections to this node
   // and we store a mapping of them.  We need to do this as not all
   // connections result in spawning new shaders (for example, connections
   // may be message connections to lights or objects).
   //
   // Also, in the case of mental ray, mray's phenomena trees are not
   // as flexible as maya and as such, only connections of identical type
   // are allowed.  So, for a color component, you can only connect another
   // color.  In the maya hypergraph, however, the user can connect each r,g,b
   // channel of the color individually.  To solve the issue, maya2mr relies
   // on auxiliary shader nodes that will turn a color into a three struct
   // component which can be connected individually.
   // This is, overall, a PAIN in the butt and makes the code below 10 times
   // more complex than it should be.
   //
   // Note that mray ONLY allows connections with OUT parameters of shaders,
   // like outColor, outValue, etc.  This is a another annoying limitation
   // of phenomena, too, albeit more from the user's pov.
   //
   ////////////////////////////////////////////////////////////////////////
   unsigned numConn = connAttrs.length();
   MPlugArray plugs;
   for ( unsigned i = 0; i < numConn; ++i )
   {
      if ( connAttrs[i].isProcedural() ) continue;

      connAttrs[i].connectedTo( plugs, true, false );
      if ( plugs.length() == 0 )
	 continue;

      MObject newObj = plugs[0].node();
      std::string key( connAttrs[i].name().asChar() );

      
      if ( newObj.hasFn( MFn::kDagNode ) )
      {
	 switch( newObj.apiType() )
	 {
	    case MFn::kCamera:
	       {
		  MDagPath camPath;
		  MDagPath::getAPathTo( newObj, camPath );
		  mrCamera* cam = mrCamera::factory( NULL,
						     camPath, 
						     MObject::kNullObj,
						     MObject::kNullObj );
		  mrInstanceCamera* inst = mrInstanceCamera::factory( camPath, 
								      cam );
		  inst->write();
		  connNames.insert( std::make_pair( key, inst->name ) );
		  continue;
	       }
	    case MFn::kLightSource:
	       {
		  MDagPath inst;
		  MDagPath::getAPathTo( plugs[0].node(), inst );
		  inst.pop();
		  connNames.insert( std::make_pair( key,
						    getMrayName( inst ) ) );
		  continue;
	       }
	    case MFn::kMesh:
	    case MFn::kSubdiv:
	    case MFn::kNurbsSurface:
	    case MFn::kNurbsCurve:
	       {
		  if ( dynamic_cast< mrShaderMaya* >(this) == NULL )
		  {
		     MDagPath objPath;
		     MDagPath::getAPathTo( plugs[0].node(), objPath );
		     mrObject* obj = mrObjectEmpty::factory( objPath );
		     assert( obj != NULL );
		     if (obj->written == kNotWritten) obj->write();
		     connNames.insert( std::make_pair( key, obj->name ) );
		     continue;
		  }
		  // else... single/dual/tripleswitch shader... 
		  // deal as instance in MFn::kDagNode next... no break here
	       }
	    case MFn::kTransform:
	       {
		  MDagPath instPath;
		  MDagPath::getAPathTo( newObj, instPath );

		  unsigned numShapes;
		  MRL_NUMBER_OF_SHAPES( instPath, numShapes );
		  if ( numShapes == 0 )
		  {
		     MString err = name;
		     err += ": attached to empty transform \"";
		     err += instPath.fullPathName();
		     err += "\"";
		     LOG_ERROR(err);
		     continue;
		  }

		  if ( numShapes > 1 )
		  {
		     MString err = name;
		     err += ": attached to transform with multiple shapes \"";
		     err += instPath.fullPathName();
		     err += "\"";
		     LOG_ERROR(err);
		     continue;
		  }

		  MDagPath shape = instPath;
		  MRL_EXTEND_TO_SHAPE_DIRECTLY_BELOW( shape, 0, numShapes );

		  mrInstance* inst = mrInstance::factory( shape, NULL );
		  assert( inst != NULL );
		  if (inst->written == kNotWritten)
		  {
		     char* dummy = MRL_MEM_STRDUP( inst->name.asChar() );
		     mi_api_instance_begin( dummy );
		     mi_api_instance_end( NULL, miNULLTAG, miNULLTAG);
		     inst->written = mrNode::kIncremental;
		  }

		  connNames.insert( std::make_pair( key, inst->name ) );
		  continue;
	       }
	    case MFn::kFluidTexture2D:
	    case MFn::kFluidTexture3D:
	       {
		  break;
	       }
	    default:
	       {
		  continue;
	       }
	 }
      }
      else if ( newObj.hasFn( MFn::kPluginDependNode ) )
      {
	 MFnDependencyNode fn( newObj );

	 unsigned id = fn.typeId().id();
	 switch( id )
	 {
	    case kMentalrayLightProfile:
	       {
		  mrLightProfile* s = mrLightProfile::factory( fn );
		  s->write();
		  connNames.insert( std::make_pair( key, s->name ) );
		  continue;
	       }
	    case kMentalrayVertexColors:
	       {
		  break;
	       }
	    case kMentalrayUserData:
	       {
		  mrUserData* s = mrUserData::factory( fn );
		  s->write();
		  connNames.insert( std::make_pair( key, s->name ) );
		  break;
	       }
	    case kMentalrayPhenomenon:
	       {
		  // we don't add phenomenon to connection names
		  continue;
	       }
	    default:
	       {
		  MStatus status;
		  MPlug tp = fn.findPlug("miFactoryNode", false, &status);
		  if ( status != MS::kSuccess )
		  {
		     MString msg = name;
		     msg += ": Unknown plugin connection for attribute \"";
		     msg += connAttrs[i].info();
		     msg += "\" to \"";
		     msg += plugs[0].info();
		     msg += "\"";
		     LOG_ERROR(msg);
		     continue;
		  }
		  break;
	       }
	 }
      }
#if MAYA_API_VERSION >= 600
      else if ( newObj.hasFn( MFn::kMentalRayTexture ) )
      {
	 MFnDependencyNode fn( newObj );
	 mrTextureNode* s = mrTextureNode::factory( fn );
	 s->write();
	 MString destName = fn.name();
	 connNames.insert( std::make_pair( key, destName ) );
	 continue;
      }
#endif
      else if ( newObj.hasFn( MFn::kShadingEngine ) )
      {
	 // Handle materials connected as mental ray parameters
	 MFnDependencyNode fn( newObj );
	 mrShadingGroup* g = mrShadingGroup::factory(fn);
	 g->write();
	 connNames.insert( std::make_pair( key, g->name ) );
	 continue;
      }
      else if ( newObj.hasFn( MFn::kSet ) )
      {
	 // Handle light/object sets as mental ray instance groups
	 MFnSet fn( newObj );
	 
	 MString name = fn.name() + ":inst";
	 mrGroupInstance* s = mrGroupInstance::factory( fn );
	 mrInstanceGroup* g = mrInstanceGroup::factory( name, s );
	 g->write();
	 
	 connNames.insert( std::make_pair( key, name ) );
	 continue;
      }
      else if ( newObj.hasFn( MFn::kAnimCurve ) )
      {
	 isAnimated = true;
	 continue;
      }
      else if ( newObj.hasFn( MFn::kExpression ) )
      {
	 isAnimated = true;
	 continue;
      }
      else if ( newObj.hasFn( MFn::kTime ) )
      {
	 isAnimated = true;
	 continue;
      }
      else if ( newObj.hasFn( MFn::kPlace2dTexture ) )
      {
	 // We assume this node is the leaf of a shading network, so
	 // no container needed.
	 container = NULL;
      }
#if MAYA_API_VERSION >= 700
      else if ( newObj.hasFn( MFn::kUnitConversion ) )
      { // empty on purpose
      }
#endif
      else if (
	       newObj.hasFn( MFn::kDependencyNode )
	       )
      {
	 MFnDependencyNode fn( newObj );
	 const MString& klass = fn.classification( fn.typeName() );
	 if ( klass == "" ) {
	    if ( dynamic_cast< mrShaderMaya* >(this) != NULL )
	    {
	       mrShaderMaya* dummy = (mrShaderMaya*) this;
	       if ( dummy->id == 0 )
		  continue;
	    }
	    invalid_connection( connAttrs[i], plugs[0] );
	    continue;  // not a shader, skip it
	 }
      }
      else
      {
	 invalid_connection( connAttrs[i], plugs[0] );
	 continue;
      }

      
      mrShader* s = mrShader::factory( newObj, container );
      if ( s == this ) continue;
      if ( s->written == kInProgress ) continue;

      s->write();

      // OKAY, are we are dealing with a child connection of
      // a compound attribute?  If so, we need to create a shader bridge
      if ( connAttrs[i].isChild() )
      {
	 // Change the connection map key to be the parent (main) plug
	 MPlug p = connAttrs[i].parent();

	 AuxShaders::iterator ie = auxShaders.end();
	 if ( std::find( auxShaders.begin(), ie, p ) != ie )
	    continue;  // we already did this shader with another plug

	 auxShaders.push_back( p );
	 auxiliaryShaders.push_back( s );
	 continue;
      }
      else  // No, we are connecting directly into the plug
      {
	 MString destName;
	 if ( isComponentPlug( plugs[0] ) )
	 {
	    MString outShader = newOutShader( plugs[0], container );
	       
	    MString attrName = plugs[0].partialName( false, false, false,
						     false, false, true );
	       
	    int last = attrName.length() - 1;
	    MString destComp = attrName.substring( last, last );
	    destComp.toLowerCase();
	       
	    attrName = attrName.substring(0, last - 1);
	    MFnDependencyNode d( plugs[0].node() ); 
	    destName = d.name() + ">" + attrName + "." + destComp;
	 }
	 else
	 {
	    MFnDependencyNode d( newObj );
	    // Are we dealing with a shader with multiple out attributes,
	    // like maya's shaders?  If so, it should have an "out"
	    // attribute and we need to use the full name of the plug.
	    if ( s->numOutAttrs > 1 )
	    {
	       if ( !container )
	       {
		  destName = plugs[0].info();
	       }
	       else
	       {
		  MString plugName = plugs[0].info();
		  int idx = plugName.rindex('.');
		  destName = plugName.substring(0, idx-1);
		  destName += "-";
		  destName += container;
		  destName += plugName.substring(idx, plugName.length()-1);
	       }
	    }
	    else
	    {  // single output result, we can just hook up the
	       // name of the shader directly, without the plug name
	       destName = d.name();
	       if ( container )
	       {
		  destName += "-";
		  destName += container;
	       }
	    }
	 }
	 connNames.insert( std::make_pair( key, destName ) );
      }
      
   }


   AuxiliaryShaders::iterator si = auxiliaryShaders.begin();
   AuxiliaryShaders::iterator se = auxiliaryShaders.end();

   AuxShaders::iterator it = auxShaders.begin();

   for ( ; si != se; ++it, ++si )
   {
      MPlug p = *it;
      mrShader* s = *si;
	    
      MString newShader = dep.name();
      if ( container ) newShader += container;
      newShader += "<";
      MString attrName = getStringForPlug( p, "<" );
      newShader += attrName;

      ////////////////////////////////////////////////////////////////
      // Handle single channel connections using a dummy shader bridge
      ////////////////////////////////////////////////////////////////

      // Before we create this new shader, we need to verify the child
      // plugs to make sure we don't need to ALSO create a dummy
      // shader in the outgoing direction.
      // For example, if we have:
      // ramp1
      // lambert1
      //
      // and we connect:
      //
      // lambert1.colorR = ramp1.outColorR
      //
      // we need to create BOTH (notice the arrows):
      //      ramp1>outColor
      //      lambert1<color
      //
      /// Here we check to see if we create > out bridges
      ///
      unsigned numChildren = p.numChildren();
      for ( unsigned c = 0; c < numChildren; ++c )
      {
	 MPlug cp = p.child(c);
	 if ( ! cp.isConnected() ) continue;
	 cp.connectedTo( plugs, true, false );
	 if ( plugs.length() == 0 ) continue;
	 if ( !isComponentPlug( plugs[0] ) ) continue;

	 // ok, we DO need to create a new outgoing bridge, too
	 newOutShader( plugs[0], container );
      }

      // Now, let's go ahead and create the new < bridge shader

      // Find the type of auxiliary shader we need to create
      // from the last letter of the connection.
      BridgeType type = getBridgeType( p );
	    

      miTag function = miNULLTAG;
      if ( written == kIncremental )
      {
	 function = MRL_ASSIGN( newShader.asChar() );
      }
	    
      float x;
      MPlug cp;
      switch(type)
      {
	 case kRGB:
	    MRL_FUNCTION( "maya_rgb_to_color" );
	    GET_COMP( r, 0 ); GET_COMP( g, 1 ); GET_COMP( b, 2 );
	    break;
	 case kHSV:
	    MRL_FUNCTION( "maya_hsv_to_color" );
	    GET_COMP( h, 0 ); GET_COMP( s, 1 ); GET_COMP( v, 2 );
	    break;
	 case kXYZ:
	    MRL_FUNCTION( "maya_xyz_to_vector" );
	    GET_COMP( x, 0 ); GET_COMP( y, 1 ); GET_COMP( z, 2 );
	    break;
	 case kUVW:
	    MRL_FUNCTION( "maya_uvw_to_vector" );
	    GET_COMP( u, 0 ); GET_COMP( v, 1 ); GET_COMP( w, 2 );
	    break;
      }

      function = mi_api_function_call_end( function );

      //! @todo:  Do incremental updates need shader_add?
      MRL_CHECK(mi_api_shader_add( MRL_MEM_STRDUP( newShader.asChar() ),
				   function ));
   }

   auxiliaryShaders.clear();
   auxShaders.clear();
   /////////////// end of getting connections
}



void mrShader::updateChildShaders( const MFnDependencyNode& dep )
{

  NodeSet nodes;

  getChildShaders( nodes, dep );

  NodeSet::iterator i = nodes.begin();
  NodeSet::iterator e = nodes.end();
  for ( ; i != e; ++i )
    {
      (*i)->write();
    }
}


void mrShader::write()
{
   // Multisample shaders will get re-spitted on incremental updates..
   if ( written == kWritten && options->progressive && 
	progressiveLast >= 0.0f &&
	options->progressivePercent != progressiveLast )
   {
      written = kIncremental;
   }

   MFnDependencyNode dep( nodeHandle.objectRef() );
   if ( written == kWritten )
   {
      written = kInProgress;
      updateChildShaders( dep );
      written = kWritten;
      return;
   }
   else
   {
#ifdef MR_RELAY
      if (!written) write_link_line();
#endif
   }

   written = kInProgress;

   ConnectionMap connNames;
   getConnectionNames( connNames, dep );
   
   if ( written == kIncremental )
      mi_api_incremental( miTRUE );
   else
      mi_api_incremental( miFALSE );
   MRL_FUNCTION( shaderName.asChar() );
   write_shader_parameters( dep, connNames );
   tag = mi_api_function_call_end( tag );
   mrl_shader_add( name, tag );
   
#ifdef _DEBUG
   // Reset parameter checking
   lastParam = 999999999;
#endif
   
   written = kWritten;
}


void mrShader::write_link_line()
{
#ifdef MR_RELAY
#ifdef MR_OPTIMIZE_SHADERS
   MString& tmp = mrShaderFactory::findDSO( shaderName );
   if ( tmp != "" && options->DSOs.find( tmp ) == options.DSOs.end() )
   {
      mi_link_file_add(tmp.asChar(), miFALSE, miFALSE, miFALSE);
      options->DSOs.insert( tmp );
   }

   
   tmp = mrShaderFactory::findMI( shaderName );
   if ( tmp != "" &&
	options->miFiles.find( tmp ) == options.miFiles.end() )
   {
      mi_mi_parse(tmp.asChar(), miFALSE, 0, 0, 0, getc, miFALSE, 0);
      options->miFiles.insert( tmp );
   }
#endif
#endif
}
