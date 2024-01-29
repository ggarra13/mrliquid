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

#ifndef mrFactoryCmd_h
#define mrFactoryCmd_h

#include <vector>
#include <maya/MPxCommand.h>

class  MString;

struct mrShaderParameter;
struct mrShaderDeclaration;


//! Auxiliary class used to store mrFactory shader parameter information
struct mrFactoryShaderParameter
{
     enum Type
     {
     kBoolean,
     kColor,
     kScalar,
     kInteger,
     kLight,
     kTransform,
     kString,
     kVector,
     kVectorTexture,
     kColorTexture,
     kScalarTexture,
     kLightProfile,
     kGeometry,
     kMaterial,
     kShader,
     kData,
     kUnknown
     };

     mrFactoryShaderParameter( const mrShaderParameter* b );
     ~mrFactoryShaderParameter();
     
     const char* getType();
     
     MString   name;
     MString   help;
     char    type;
     
   private:
     mrFactoryShaderParameter() {};
};


//! Auxiliary class used to store mrFactory shader information
struct mrFactoryShaderInfo
{
     static void factory( const mrShaderDeclaration* s );
     static void clear();
     
     char* name;
     char* help;
     
     typedef std::vector< mrFactoryShaderParameter* > ParameterList;
     ParameterList params;

     ~mrFactoryShaderInfo();
     
   protected:
     void addAttributes( const mrShaderParameter* p );
     mrFactoryShaderInfo( const mrShaderDeclaration* s );
};



class MArgList;
class MSyntax;

//! MAIN CLASS DECLARATION FOR THE MEL COMMAND: mrFactory
class mrFactoryCmd : public MPxCommand
{
   public:
     mrFactoryCmd();
     virtual ~mrFactoryCmd();
     static void *creator();
     static MSyntax newSyntax();
     bool isUndoable() const;
     MStatus doIt(const MArgList&);

   protected:
     MString getAttrType( const MString& node, const MString& attr );
     MString     getHelp( const MString& node, const MString& attr );
};



#endif // mrFactoryCmd_h
