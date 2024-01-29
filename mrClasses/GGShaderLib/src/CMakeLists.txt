

PROJECT( gg_shaders )

SET( SHADER_VERSION "0.2" )

IF(UNIX)
  IF( NOT CMAKE_BUILD_TYPE STREQUAL "Debug" )
    SET( SHADER_LINK_FLAGS "-Wl,--version-script,${CMAKE_CURRENT_SOURCE_DIR}/mray_shaders.lds -Wl,-s" )
  ENDIF( NOT CMAKE_BUILD_TYPE STREQUAL "Debug" )
ENDIF(UNIX)


INCLUDE_DIRECTORIES(
  ../../../include
  ../../mrClasses
  ${MENTALRAY_INCLUDE_DIR}
  )

LINK_DIRECTORIES(
  ../../mrLibrary
)


LINK_DIRECTORIES(
  ${MENTALRAY_LIBRARY_DIR}
  )


SET( SOURCES 
  gg_ambientocclusion.cpp
  gg_cellnoise.cpp
  # gg_comp.cpp
  gg_noise.cpp
  gg_noises.cpp
  gg_parallaxbump.cpp
  gg_perlin.cpp
  # gg_quicktime.cpp
  gg_reflectionocclusion.cpp
  gg_rgbdisplacement.cpp
  gg_showinfo.cpp
  gg_spherical.cpp
  # gg_tonemap.cpp
  gg_tracegroup.cpp
  gg_worley.cpp
  )

IF(UNIX)
  SET( LIBRARIES mrLibrary )
  SET( DEPENDENCIES "mrLibrary" )
ELSE(UNIX)
  SET( LIBRARIES libmrLibrary )
  SET( DEPENDENCIES "libmrLibrary" )
ENDIF(UNIX)


CREATE_MENTALRAY_SHADER( 
  "mray_shaders" "${SHADER_VERSION}"
  "${SOURCES}" "${LIBRARIES}"
  "${DEPENDENCIES}"
  )



SET( SOURCES
  # gg_buffers.cpp  # obsolete
  gg_fakefur.cpp
  gg_geo_pfxhair.cpp
  gg_lambert.cpp
  gg_orennayar.cpp
  gg_pfxhair.cpp
  gg_phong.cpp
  gg_skin.cpp
  gg_strauss.cpp
  gg_shaderstate.cpp
  gg_threads.cpp
  gg_velvet.cpp
)

# CREATE_MENTALRAY_SHADER( 
#   "nonpd_shaders" "${SHADER_VERSION}"
#   "${SOURCES}"
#   "${MAYA_LIBRARIES}"
#   "${DEPENDENCIES}"
#   )

#
# SET( SOURCES
#   maya_ambientlight.cpp  
#   maya_spotlight.cpp
# )
#
# CREATE_MENTALRAY_SHADER( 
#   "maya_shaders" "${SHADER_VERSION}"
#   "${SOURCES}"
#   "${MAYA_LIBRARIES}"
#   "${DEPENDENCIES}"
#   )
#

IF( SHADER_LINK_FLAGS )
  TARGET_LINK_FLAGS( mray_shaders ${SHADER_LINK_FLAGS} )
ENDIF( SHADER_LINK_FLAGS )
