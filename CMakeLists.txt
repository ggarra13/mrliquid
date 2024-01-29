#
# This cmake file must be run under cmake 2.5 or later
#
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.7 FATAL_ERROR)

#
# Modify CMake's module path so our custom modules take precedence over
# cmake's default ones.

IF( NOT CMAKE_MODULE_PATH )
  SET( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/modules )
ENDIF( NOT CMAKE_MODULE_PATH )

#
# All CMake projects need a name.  The name is global to all
# CMakeLists.txt in subdirectories.  Once you define a project
# named "myproject", the variable "myproject_SOURCE_DIR" exists.
# This is the toplevel source directory of your project, i.e.
# wherever your topmost CMakeLists.txt resides.
#
# You can also specify support for languages other than C++, using
# additional arguments.  mrLiquid only needs C++, so we don't bother.
#
PROJECT( mrLiquid )



#
# For testing
#
#ENABLE_TESTING()



IF( UNIX )
  SET( CMAKE_SYSTEM_LIBRARY_PATH )
  INCLUDE( Platform/UnixPaths )
ENDIF( UNIX )


#
# Compile everything using MR_SSE extensions when possible
# (flag is propagated to other projects)
#
ADD_DEFINITIONS( -DMR_SSE )

#
# Turn off VC8 secure crap
#
IF( WIN32 )
  ADD_DEFINITIONS( -D_CRT_SECURE_NO_DEPRECATE ) # RTTI enabled
ENDIF( WIN32 )


#
# Link OpenEXR statically
#
SET( OPENEXR_STATIC 1 )


#
# Project's subdirectories
#
ADD_SUBDIRECTORY( tools )
ADD_SUBDIRECTORY( mrLiquid )
ADD_SUBDIRECTORY( mrClasses )


#
# For packaging it all
#
#INCLUDE( CPack )


