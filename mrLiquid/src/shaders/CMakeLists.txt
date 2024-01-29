

PROJECT( mrl_shaders )

SET( SHADER_VERSION "0.8" )


IF(UNIX)
  IF( NOT CMAKE_BUILD_TYPE STREQUAL "Debug" )
    SET( SHADER_LINK_FLAGS "-Wl,--version-script,${CMAKE_CURRENT_SOURCE_DIR}/mrl_shaders.lds -Wl,-s" )
  ENDIF( NOT CMAKE_BUILD_TYPE STREQUAL "Debug" )
ENDIF(UNIX)


FIND_PACKAGE( BuildDir  REQUIRED )
FIND_PACKAGE( Mentalray REQUIRED )
FIND_PACKAGE( OpenEXR   REQUIRED )
FIND_PACKAGE( Maya      REQUIRED )



INCLUDE_DIRECTORIES(
  ..
  ../../../mrClasses/mrClasses
  ${OPENEXR_INCLUDE_DIR}
  ${MENTALRAY_INCLUDE_DIR}
  )



LINK_DIRECTORIES(
  ${MENTALRAY_LIBRARY_DIR}
  ${OPENEXR_LIBRARY_DIR}
  )


SET( SOURCES
  Delaunay2d.cpp
  module_exit.cpp
  # mrl_file.cpp  # unused - demo for mrl_exr_file
  mrl_furshader.cpp
  mrl_geo_hair.cpp
  mrl_geo_pdc_sprites.cpp
  mrl_geo_primitives.cpp
  mrl_hairshader.cpp
  mrl_lens.cpp
  mrl_paintfx.cpp
  mrl_particlecloud.cpp
  mrl_particlesprite.cpp
  mrl_state.cpp
  mrl_volume_isect.cpp
  mrOctree.cpp
  pdcAux.cpp

  #
  # We borrow these from mrLiquid's source...
  #
  ../mrSocket.cpp
  ../mrThread.cpp
  )

IF(UNIX)
  SET( MR_LIBRARY mrLibrary )
  SET( DEPENDENCIES "mrLibrary" )
ELSE(UNIX)
  SET( MR_LIBRARY libmrLibrary )
  SET( DEPENDENCIES "libmrLibrary" )
ENDIF(UNIX)


SET( LIBRARIES ${MAYA_MENTALRAY_LIBRARIES} ${MR_LIBRARY} )

CREATE_MENTALRAY_SHADER( 
  "mrl_shaders" "${SHADER_VERSION}"
  "${SOURCES}"
  "${LIBRARIES}"
  "${DEPENDENCIES}"
)


IF(OPENEXR_INCLUDE_DIR)

  SET( LIBRARIES ${MR_LIBRARY} ${OPENEXR_LIBRARIES} )

  SET( SOURCES 
    gg_exr_standalone.cpp
    mrl_exr_file.cpp
    )

  CREATE_MENTALRAY_SHADER( 
    "exr_shaders" "${SHADER_VERSION}"
    "${SOURCES}"
    "${LIBRARIES}"
    "${DEPENDENCIES}"
    )

ELSE(OPENEXR_INCLUDE_DIR)
  MESSAGE( "WARNING: No OpenEXR -- exr_shaders will not be created" )
ENDIF(OPENEXR_INCLUDE_DIR)


IF( SHADER_LINK_FLAGS )
  TARGET_LINK_FLAGS( exr_shaders ${SHADER_LINK_FLAGS} )
  TARGET_LINK_FLAGS( mrl_shaders ${SHADER_LINK_FLAGS} )
ENDIF( SHADER_LINK_FLAGS )

