
SET( PROJECT "mrLiquid_raylib" )

SET( SHADER_NAME "gg_mrliquid" )
SET( SHADER_VERSION "${VERSION}" )

SET( SOURCES   gg_mrliquid.cpp ${SRCS} )
SET( LIBRARIES ${MAYA_MENTALRAY_LIBRARIES} ${LIBRARIES} )

REMOVE_DEFINITIONS( -DMR_NO_RAYLIB )
FIND_PACKAGE( Mentalray REQUIRED )

INCLUDE_DIRECTORIES( ${MENTALRAY_INCLUDE_DIR} ../../../mrClasses/mrClasses )
CREATE_MENTALRAY_SHADER(
  "${SHADER_NAME}" "${SHADER_VERSION}"
  "${SOURCES}" "${LIBRARIES}" ""
)

SET_SOURCE_FILES_PROPERTIES("${CMAKE_CURRENT_BINARY_DIR}/../grammar.tab.cpp" 
  GENERATED)
SET_SOURCE_FILES_PROPERTIES("${CMAKE_CURRENT_BINARY_DIR}/../fileobjgrammar.fixed.cpp" 
  GENERATED)
SET_SOURCE_FILES_PROPERTIES("${CMAKE_CURRENT_BINARY_DIR}/../lex.yy.c"                 
  GENERATED)
