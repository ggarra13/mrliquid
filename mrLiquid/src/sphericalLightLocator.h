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
#ifndef sphericalLightLocator_h
#define sphericalLightLocator_h

#if defined(_WIN32) || defined(_WIN64)
#include "winsock2.h"
#include "windows.h"
#endif


#include "maya/MPxLocatorNode.h"

#if defined(OSMac_)
#   if defined(OSMac_MachO_)
#      include "AGL/glu.h"
#   else
#      include <glu.h>
#   endif
#else
#  include "GL/glu.h"
#endif


class sphericalLightLocator : public MPxLocatorNode
{
   public:
     sphericalLightLocator();
     virtual ~sphericalLightLocator();

     //! main GL draw function
     virtual void draw( M3dView & view, const MDagPath & path, 
			M3dView::DisplayStyle style,
			M3dView::DisplayStatus status );

     //! always true
     virtual bool            isBounded() const;
     //! boundingBox calculation
     virtual MBoundingBox    boundingBox() const; 

  
     static  void *          creator();
     static  MStatus         initialize();
     
     static MTypeId	          id;    //! ID of plugin
     
   protected:
     static GLUnurbsObj*  glu;
     static unsigned      num;
};


#endif // sphericalLightLocator_h
