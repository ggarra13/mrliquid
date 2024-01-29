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
//   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#include <cmath>

#include "maya/MPlug.h"
#include "maya/MFnAttribute.h"
#include "maya/MFnAnimCurve.h"

#include "mrShaderAnimCurveUU.h"
#include "mrOptions.h"

// This is the first VALID attribute for all shader nodes,
// skipping standard stuff like message, etc.
// This value could potentially change from maya version to maya version.
static const unsigned START_ATTR = 5;

mrShaderAnimCurveUU::mrShaderAnimCurveUU( const MFnDependencyNode& fn ) :
mrShader( fn )
{
   shaderName = "maya_curve";
}


/** 
 * Main virtual routine to write shader parameters
 * 
 * @param f 
 * @param dep 
 * @param connNames 
 */
void 
mrShaderAnimCurveUU::write_shader_parameters( MRL_FILE* f,
					      MFnDependencyNode& dep,
					      const ConnectionMap& connNames )
{
   TAB(2); MRL_PUTS("\"keys\" [");

   MFnAnimCurve fn( dep.object() );

   MAngle inA, outA;
   double w;
   // float oneThird = 1.0f / 3.0f;
   
   unsigned numKeys = fn.numKeys();
   for ( unsigned i = 0; i < numKeys; ++i )
   {
      TAB(3);
      if ( i > 0 ) MRL_PUTC(',');
      MRL_PUTS("{\n");

      double value = fn.time( i ).value();
      TAB(3); MRL_FPRINTF(f, "\"time\" %g,\n", value );

      value = fn.value( i );
      TAB(3); MRL_FPRINTF(f, "\"value\" %g,\n", value );

      fn.getTangent(i, inA, w, true);
      double r = inA.asRadians();
      double x = 3.0 * cos( r ) * w;
      double y = tan( r ) * x;

      TAB(3); MRL_FPRINTF(f, "\"tanInX\" %g,\n", x );
      TAB(3); MRL_FPRINTF(f, "\"tanInY\" %g,\n", y );

      fn.getTangent( i, outA, w, false );
      r = outA.asRadians();
      x = 3.0 * cos( r ) * w;
      y = tan( r ) * x;
      TAB(3); MRL_FPRINTF(f, "\"tanOutX\" %g,\n", x );
      TAB(3); MRL_FPRINTF(f, "\"tanOutY\" %g\n", y );
      
      MRL_PUTC('}');
   }

   MRL_PUTS("\n"); TAB(2);
   MRL_PUTS("],\n");

   bool weighted = fn.isWeighted();
   TAB(2); MRL_PUTS("\"isWeighted\" ");
   if ( weighted ) MRL_PUTS("on");
   else MRL_PUTS("off");
   MRL_PUTS(",\n");

   TAB(2); MRL_FPRINTF(f, "\"preInfinity\" %d,\n", fn.preInfinityType() );
   TAB(2); MRL_FPRINTF(f, "\"postInfinity\" %d", fn.postInfinityType() );


}

void mrShaderAnimCurveUU::write_curve( MRL_FILE* f, const ConnectionMap& connNames )
{
   MFnDependencyNode dep( nodeRef() );
   char oldWritten = written;
   written = kInProgress;
   DBG2(name << ": Done with getConnection names");

   if ( oldWritten == kIncremental )
   {
      MRL_PUTS("incremental ");
   }

   MRL_FPRINTF(f, "shader \"%s:curve\"\n", name.asChar() );
   TAB(1);  MRL_PUTS( "\"maya_curve\" (\n" );
   write_shader_parameters( f, dep, connNames );
   MRL_PUTC('\n');
   TAB(1); MRL_PUTS(")\n");
}

void mrShaderAnimCurveUU::write( MRL_FILE* f )
{

   DBG( name << ": mrShaderAnimCurveUU::write()" );

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
   MRL_FPRINTF(f, "shader \"%s\"\n", name.asChar() );
   TAB(1);
   MRL_PUTS( "\"maya_scalar_curve\" (\n" );
   TAB(2);
   MString key = name + ".input";
   MRL_FPRINTF(f, "\"input\" = \"%s\",\n", connNames[key.asChar()].asChar() ); 
   MRL_FPRINTF(f, "\"curve\" \"%s:curve\"\n", name.asChar() ); 
   TAB(1); MRL_PUTS(")\n");
   
   NEWLINE();
   written = kWritten;
}


#ifdef GEOSHADER_H
#include "raylib/mrShaderAnimCurveUU.inl"
#endif
