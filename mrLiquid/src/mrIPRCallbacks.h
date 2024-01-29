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

#ifndef mrIPRCallbacks_h
#define mrIPRCallbacks_h

#include <vector>

#include "maya/MTime.h"
#include "maya/MDagPath.h"
#include "maya/MDagPathArray.h"
#include "maya/MDagMessage.h"
#include "maya/MNodeMessage.h"

class mrTranslator;

class mrIPRCallbacks
{
public:
  typedef std::vector< mrIPRCallbacks* > mrIPRCallbacksList;

protected:
  typedef std::vector< MCallbackId > CallbackList;

public:
  mrIPRCallbacks( mrTranslator* translator, const int port = 6500 );
  ~mrIPRCallbacks();

  int  running();
  void stop();
  void unpause();

  void delete_nodes();
  void update_scene();
  void update_attr_callbacks();

  void add_all_callbacks();
  void add_global_callbacks();
  void add_delete_callbacks();
  void add_file_callbacks();
  void add_attr_callbacks();
  void add_attr_callback( MObject& node );
  void add_time_callback();
  void add_idle_callback();

  void remove_all_callbacks();
  void remove_global_callbacks();
  void remove_attr_callbacks();
  void remove_attr_callback( MObject& node );
  void remove_time_callback();
  void remove_idle_callback();

  bool has_callback( MObject& node );


  void port( int p )          { m_port = p; }
  int  port()                 { return m_port; }
  mrTranslator* translator()  { return m_translator; }

  static void pause_all();
  static void unpause_all();

protected:
  mrTranslator* m_translator;

  MCallbackId  idleId;
  MCallbackId  timeId;
  CallbackList ids;
  CallbackList attrs;
  
  int           m_port;

protected:
  static mrIPRCallbacksList callbackList;
};


#endif // mrIPRCallbacks_h
