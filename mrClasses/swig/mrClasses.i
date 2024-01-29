/**
 * @file   mrClasses.i
 * @author Gonzalo Garramuno
 * @date   Mon Jul 10 23:23:05 2006
 * 
 * @brief  Main File for Swig Wrapping.  Tested under SWIG 1.3.29
 * 
 */

%module(directors="1",dirprot="1") "mr"

%nodefaultctor;


%{
#include "shader.h"
#include "geoshader.h"

  extern "C" {
  struct mi_undefined_lock {
  };
  }

%}

// Catch ruby exceptions in directors
%feature("director:except") {
 throw Swig::DirectorMethodException($error);
}


%include "shader.i"
%include "geoshader.i"

