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
#ifndef pluginMain_h
#define pluginMain_h
//
//! Just some macros to make registering/unregistering plugins/nodes easier
//

#define REGISTER_NAMED_CMD( name, x) \
   status = plugin.registerCommand( #name , x ## Cmd::creator, \
				   x ## Cmd::newSyntax); \
   if (!status) \
   { \
       status.perror("registerCmd " #x); \
       return status; \
   }

#define REGISTER_CMD(x) REGISTER_NAMED_CMD(x, x)


#define REGISTER_NODE(x) \
   if ( ! plugin.isNodeRegistered( #x ) ) \
   { \
     status = plugin.registerNode(#x, x##Node::id, \
				  x##Node::creator, x##Node::initialize); \
     if (!status) \
     { \
       status.perror("registerNode " #x); \
       return status; \
     } \
   }


#define REGISTER_LOCATOR(x) \
   if ( ! plugin.isNodeRegistered( #x "Locator" ) ) \
   { \
	status = plugin.registerNode( #x "Locator", x ## Locator::id, \
				      x ## Locator::creator, \
				      x ## Locator::initialize, \
				      MPxNode::kLocatorNode ); \
     if (!status) \
     { \
       status.perror("registerLocator " #x); \
       return status; \
     } \
   }


#define REGISTER_SHAPE(x) \
   if ( ! plugin.isNodeRegistered( #x "Shape" ) ) \
   { \
	status = plugin.registerShape( #x "Shape", x ## Shape::id, \
				       x ## Shape::creator, \
				       x ## Shape::initialize, \
				       x ## ShapeUI::creator ); \
     if (!status) \
     { \
       status.perror("registerShape " #x); \
       return status; \
     } \
   }

#define REGISTER_SHAPE_ADD_CALLBACK(x) \
   x ## Shape::addCallbackId = \
       MDGMessage::addNodeAddedCallback( x ## Shape::nodeAddedCallback, \
                                         #x "Shape" );

#define DEREGISTER_SHAPE_ADD_CALLBACK(x) \
       MMessage::removeCallback( x ## Shape::addCallbackId );

#define REGISTER_LOCATOR_ADD_CALLBACK(x) \
   x ## Locator::addCallbackId = \
       MDGMessage::addNodeAddedCallback( x ## Locator::nodeAddedCallback, \
                                         #x "Locator" );

#define REGISTER_NODE_ADD_CALLBACK(x) \
   x ## Node::addCallbackId = \
       MDGMessage::addNodeAddedCallback( x ## Node::nodeAddedCallback, \
                                         #x "Node" );

#define REGISTER_CONTEXT( x ) \
   status = plugin.registerContextCommand( #x "ToolContext", \
                                           x ## ContextCmd::creator ); \
   if (!status) \
   { \
      status.perror("registerContextCommand " #x); \
   }

#define REGISTER_SWATCH( a, b ) \
      status = MSwatchRenderRegister::registerSwatchRender( #a, &b ); \
      if ( status != MS::kSuccess ) \
      { \
	 LOG_ERROR("Could not register mrLiquid's swatch generator."); \
      }



#define DEREGISTER_LOCATOR_ADD_CALLBACK(x) \
       MMessage::removeCallback( x ## Locator::addCallbackId );

#define DEREGISTER_NODE_ADD_CALLBACK(x) \
       MMessage::removeCallback( x ## Node::addCallbackId );

#define DEREGISTER_CMD( x ) \
   status = plugin.deregisterCommand( #x ); \
   if (!status) \
   { \
      status.perror("deregisterCommand " #x ); \
   }

#define DEREGISTER_NODE( x ) \
   status = plugin.deregisterNode( x ## Node::id ); \
   if (!status) \
   { \
      status.perror("deregisterNode " #x); \
   }

#define DEREGISTER_LOCATOR( x ) \
   status = plugin.deregisterNode( x ## Locator::id ); \
   if (!status) \
   { \
      status.perror("deregisterLocator " #x); \
   }

#define DEREGISTER_SHAPE( x ) \
   status = plugin.deregisterNode( x ## Shape::id ); \
   if (!status) \
   { \
      status.perror("deregisterShape " #x); \
   }

#define DEREGISTER_CONTEXT( x ) \
   status = plugin.deregisterContextCommand( #x "ToolContext" ); \
   if (!status) \
   { \
      status.perror("deregisterContextCommand " #x); \
   }

#define DEREGISTER_TRANSLATOR( x ) \
      status = plugin.deregisterFileTranslator( #x ); \
      if ( status != MS::kSuccess ) \
      { \
	 status.perror("deregisterFileTranslator" #x); \
      }

#define DEREGISTER_SWATCH( a ) \
      status = MSwatchRenderRegister::unregisterSwatchRender( #a ); \
      if ( status != MS::kSuccess ) \
      { \
	 status.perror("unregisterSwatchRender" #a); \
      }


#endif
