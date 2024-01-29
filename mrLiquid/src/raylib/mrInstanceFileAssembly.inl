

void mrInstanceFileAssembly::write()
{

#ifndef RAY36

  #warning File assemblies not available in this version of mray

#else
   mrObject::BBox box, mbox;

   mrFileAssembly* assembly = (mrFileAssembly*) shape;

   assembly->calculateBoundingBoxes( box, mbox );

   if ( box.min.x == box.max.x || box.min.y == box.max.y ||
	box.min.z == box.max.z )
     {
       MString err = "Invalid bounding box for assembly \"";
       err += name;
       err += "\"";
       LOG_WARNING(err);
       return;
     }

   MString file = assembly->file();
   if ( file.length() == 0 )
     {
       MString err = "Invalid assembly file for \"";
       err += name;
       err += "\"";
       LOG_WARNING(err);
       return;
     }


   MPoint p[] = { 
     MPoint( box.min.x, box.min.y, box.min.z ),
     MPoint( box.max.x, box.min.y, box.min.z ),
     MPoint( box.max.x, box.max.y, box.min.z ),
     MPoint( box.min.x, box.max.y, box.min.z ),
     MPoint( box.min.x, box.min.y, box.max.z ),
     MPoint( box.min.x, box.max.y, box.max.z ),
     MPoint( box.max.x, box.min.y, box.max.z ),
     MPoint( box.min.x, box.max.y, box.min.z ),
     MPoint( box.max.x, box.max.y, box.max.z )
   };

   // Transform points by matrix
   MMatrix m2 = m.inverse();

   int i;
   for ( i = 0; i < 8; ++i )
     {
       p[i] *= m2;
     }

   if ( written == kIncremental )
     mi_api_incremental( miTRUE );
   else
     mi_api_incremental( miFALSE );
   
   miAssembly* a = mi_api_assembly_begin( MRL_MEM_STRDUP( name.asChar() ) );


   // Find new min/maxs

   a->bbox_min.x = a->bbox_max.x = (miScalar) p[0].x;
   a->bbox_min.y = a->bbox_max.y = (miScalar) p[0].y;
   a->bbox_min.z = a->bbox_max.z = (miScalar) p[0].z;

   for ( i = 1; i < 8; ++i )
     {
       if ( p[i].x < a->bbox_min.x ) a->bbox_min.x = p[i].x;
       if ( p[i].y < a->bbox_min.y ) a->bbox_min.y = p[i].y;
       if ( p[i].z < a->bbox_min.z ) a->bbox_min.z = p[i].z;

       if ( p[i].x > a->bbox_max.x ) a->bbox_max.x = p[i].x;
       if ( p[i].y > a->bbox_max.y ) a->bbox_max.y = p[i].y;
       if ( p[i].z > a->bbox_max.z ) a->bbox_max.z = p[i].z;
     }

   if ( options->motionBlur != mrOptions::kMotionBlurOff )
     {
       // Transform motion by matrix?
       mbox.min *= m2; mbox.max *= m2;

       a->bbox_min_m.x = (miScalar) mbox.min.x;
       a->bbox_min_m.y = (miScalar) mbox.min.y;
       a->bbox_min_m.z = (miScalar) mbox.min.z;

       a->bbox_max_m.x = (miScalar) mbox.max.x;
       a->bbox_max_m.y = (miScalar) mbox.max.y;
       a->bbox_max_m.z = (miScalar) mbox.max.z;
     }

   mi_api_assembly_filename( MRL_MEM_STRDUP( file.asChar() ) );

   tag = mi_api_assembly_end();
#endif
   
   written = kWritten;
}
