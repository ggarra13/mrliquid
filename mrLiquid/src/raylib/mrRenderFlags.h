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

#ifndef mrRenderFlags_h
#define mrRenderFlags_h

static const int miRENDER_PREPROC = 1;
static const int miRENDER_OBJ_DELETE = 2;
static const int miRENDER_SHADOWMAP = 4;
static const int miRENDER_DISPLAY = 8;
static const int miRENDER_RENDER  = 16;
static const int miRENDER_FB_SAVE = 32;
static const int miRENDER_FB_RESTORE = 64;
static const int miRENDER_FB_DELETE  = 128;
static const int miRENDER_OUTPUTSH = 256;
static const int miRENDER_POSTPROC = 512;
static const int miRENDER_IMAGE_DELETE = 1024;
static const int miRENDER_CHECK = 2048;
static const int miRENDER_DUMP = 4096;
static const int miRENDER_ACCEL_DELETE = 8192;
static const int miRENDER_LIGHTMAP = 16384;
static const int miRENDER_REINHERIT = 32768;
static const int miRENDER_DEFAULT   = 0xFFFFFF;

#endif // mrRenderFlags_h

