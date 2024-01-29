
PROJECT( mrLibrary )

SET( SHADER_NAME "libmrLibrary" )
SET( SHADER_VERSION "0.2" )


#
# On Unix, we need to hide symbols using a version script to protect licensing
#


SET( SOURCES 
  mrCell.cpp  mrFastMath.cpp  mrLibrary.cpp  mrMemoryDbg.cpp  
  mrPerlin.cpp  mrStackTrace.cpp  mrWorley.cpp 
  )

#IF(WIN32)
#SET( SOURCES ${SOURCES} mrStackWalker.cpp )
#ENDIF(WIN32)


INCLUDE_DIRECTORIES(
  ../../include
  ../mrClasses
  ${MENTALRAY_INCLUDE_DIR}
  )

LINK_DIRECTORIES(
  ${MENTALRAY_LIBRARY_DIR}
  )

LINK_LIBRARIES( ${MENTALRAY_LIBRARIES} )


ADD_DEFINITIONS( -DMR_MEM_CHECK -DMR_LIBRARY_EXPORT )

CREATE_MENTALRAY_LIBRARY( 
  "${SHADER_NAME}" "${SHADER_VERSION}"
  "${SOURCES}"
  "${LIBRARIES}"
  )
