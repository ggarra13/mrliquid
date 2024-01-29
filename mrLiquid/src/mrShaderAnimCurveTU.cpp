
#include "maya/MPlug.h"
#include "maya/MFnAttribute.h"

#include "mrShaderAnimCurveTU.h"
#include "mrOptions.h"
#include "mrParticles.h"

mrShaderAnimCurveTU::mrShaderAnimCurveTU( const MFnDependencyNode& fn ) :
mrShaderAnimCurveUU( fn )
{
   shaderName = "maya_time_curve";
}


void mrShaderAnimCurveTU::write( MRL_FILE* f )
{

   DBG( name << ": mrShaderAnimCurveTU::write()" );

   if ( options->exportFilter & mrOptions::kShaderDef )
      return;

   
   MFnDependencyNode dep( nodeRef() );
   if ( written == kWritten )
   {
      written = kInProgress;
      updateChildShaders( f, dep ); 
      written = kWritten;
      return;
   }


   char oldWritten = written;

   ConnectionMap connNames;
   getConnectionNames( f, connNames, dep );

   write_curve( f, connNames );

   if ( oldWritten == kIncremental )
   {
      MRL_PUTS("incremental ");
   }


   const char* n = name.asChar();
   MRL_FPRINTF(f, "shader \"%s\"\n", n );
   TAB(1);
   MRL_FPRINTF(f, "\"%s\" (\n", shaderName.asChar() );
   TAB(2); MRL_FPRINTF(f, "\"curve\" \"%s:curve\"\n", n );
   TAB(1); MRL_PUTS(")\n");
   
   NEWLINE();
   written = kWritten;
}

#ifdef GEOSHADER_H
#include "raylib/mrShaderAnimCurveTU.inl"
#endif
