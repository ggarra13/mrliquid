
#
# Give your project a name
#
PROJECT( mrLiquid )


IF( NOT CMAKE_MODULE_PATH )
  SET( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../modules )
ENDIF( NOT CMAKE_MODULE_PATH )

#
# List additional packages (libraries) here
# for CMake to find using FindXXXXX.cmake files
# 

FIND_PACKAGE( BuildDir REQUIRED )
# FIND_PACKAGE( OpenEXR  REQUIRED )
FIND_PACKAGE( Maya     REQUIRED )
FIND_PACKAGE( Flex     REQUIRED )
FIND_PACKAGE( Bison    REQUIRED )
FIND_PACKAGE( Sed      REQUIRED )
FIND_PACKAGE( OpenGL   REQUIRED )
#FIND_PACKAGE( RLM      REQUIRED )

OPTION(USE_SFIO "Use SFIO library if available" OFF)


IF(UNIX)
  IF(USE_SFIO)
    FIND_PACKAGE( SFIO )
  ENDIF(USE_SFIO)

  # this is needed to hide rlm licensing
  #ADD_DEFINITIONS( -fvisibility=hidden )
ENDIF(UNIX)



#
# For each plugin, give it a name and version
#
SET(PLUGIN_NAME    "mrLiquid" )
SET(PLUGIN_VERSION "0.8" )


#
# On Unix, we need to hide symbols using a version script to protect licensing
#
IF(UNIX)
  IF( NOT CMAKE_BUILD_TYPE STREQUAL "Debug" )
    SET( MAYA_LINK_FLAGS "-Wl,--version-script,${CMAKE_CURRENT_SOURCE_DIR}/mrLiquid.lds -Wl,-s" )
  ENDIF( NOT CMAKE_BUILD_TYPE STREQUAL "Debug" )
ELSE(UNIX)
  IF(WIN32)
    SET( OS_LIBRARIES "ws2_32.dll" )
  ENDIF( WIN32 )
ENDIF(UNIX)



#
# Set these to all your .c, .cxx or .cpp files
#
SET( SOURCES 
  convertLightmapCmd.cpp
  convertLightmapSetupCmd.cpp
  cylindricalLightLocator.cpp
  discLightLocator.cpp
  mapVizShape.cpp
  MayatomrJobCmd.cpp
  # mentalClearConsoleCmd.cpp
  mentalCmd.cpp
  mentalCropTool.cpp
  mentalFileAssemblyShape.cpp
  mentalFileObjectShape.cpp
  mentalFileTranslator.cpp
  mentalIsAnimatedCmd.cpp
  mentalMaterialCmd.cpp
  mentalParseStringCmd.cpp
  mentalrayColorProfileNode.cpp
  mentalrayCurveApproxNode.cpp
  mentalrayDisplaceApproxNode.cpp
  mentalrayFramebufferNode.cpp
  mentalrayGlobalsNode.cpp
  mentalrayIblShape.cpp
  mentalrayItemsListNode.cpp
  mentalrayLightProfileNode.cpp
  mentalrayOptionsNode.cpp
  mentalrayOutputPassNode.cpp
  mentalrayPhenomenonNode.cpp
  mentalraySubdivApproxNode.cpp
  mentalraySurfaceApproxNode.cpp
  mentalrayTextNode.cpp
  mentalrayTextureNode.cpp
  mentalrayUserBufferNode.cpp
  mentalrayUserDataNode.cpp
  mentalrayVertexColorsNode.cpp
  mentalRenderLayerOverrideNode.cpp
  mentalRenderViewCmd.cpp
  mentalVisibilityCmd.cpp
  mrApproximation.cpp
  mrBakeSet.cpp
  mrCamera.cpp
  mrCustomText.cpp
  # mrEXRImage.cpp
  # mrEXRmakeTiled.cpp
  mrFactoryCmd.cpp
  mrFileAssembly.cpp
  mrFileObject.cpp
  mrFluid.cpp
  mrFramebuffer.cpp
  mrFur.cpp
  mrGlobals.cpp
  mrGroupInstance.cpp
  mrHelpers.cpp
  mrImagePlane.cpp
  mrImagePlaneSG.cpp
  mrInheritableFlags.cpp
  mrInstance.cpp
  mrInstanceDefault.cpp
  mrInstanceFileAssembly.cpp
  mrInstanceFileObject.cpp
  mrInstanceFluid.cpp
  mrInstanceFur.cpp
  mrInstanceGeoShader.cpp
  mrInstanceImagePlane.cpp
  mrInstanceObject.cpp
  mrInstanceParticles.cpp
  mrInstancePfxGeometry.cpp
  mrInstancePfxHair.cpp
  mrInstanceSwatch.cpp
  mrIPRCallbacks.cpp
  mrLanguagePython.cpp
  mrLanguageRuby.cpp
  mrvPC1.cpp
  mrlLicensing.cpp
  mrLightAmbient.cpp
  mrLightArea.cpp
  mrLight.cpp
  mrLightDefault.cpp
  mrLightDirectional.cpp
  mrLightLinks.cpp
  mrLightMapOptions.cpp
  mrLightPhotons.cpp
  mrLightPoint.cpp
  mrLightProfile.cpp
  mrLightShMap.cpp
  mrLightSpot.cpp
  mrLightVolume.cpp
  mrMemoryDbg.cpp
  mrMesh.cpp
  mrMeshSubd.cpp
  mrNode.cpp
  mrNurbsSurface.cpp
  mrObject.cpp
  mrObjectDummy.cpp
  mrObjectEmpty.cpp
  mrOptions.cpp
  mrOutput.cpp
  mrOutputPass.cpp
  mrParticles.cpp
  mrParticlesInstancer.cpp
  mrPerlinNoise.cpp
  mrPfxBase.cpp
  mrPfxGeometry.cpp
  mrPfxHair.cpp
  mrPhenomenon.cpp
  mrPipe.cpp
  mrRenderView.cpp
  mrSamplesOutputPass.cpp
  mrShaderAnimCurveTU.cpp
  mrShaderAnimCurveUU.cpp
  mrShaderCameraBGColor.cpp
  mrShader.cpp
  mrShaderFactory.cpp
  mrShaderIBLLight.cpp
  mrShaderLight.cpp
  mrShaderLightCustom.cpp
  mrShaderLightInfo.cpp
  mrShaderLightSpot.cpp
  mrShaderMayaContour.cpp
  mrShaderMaya.cpp
  mrShaderMayaFile.cpp
  mrShaderMayaFluid.cpp
  mrShaderMayaFur.cpp
  mrShaderMayaIsect.cpp
  mrShaderMayaLightMap.cpp
  mrShaderMayaParticleFile.cpp
  mrShaderMayaPfxHair.cpp
  mrShaderMayaPfxStroke.cpp
  mrShaderMayaPSD.cpp
  mrShaderMayaShadow.cpp
  mrShaderMayaUVChooser.cpp
  mrShaderRenderCone.cpp
  mrShaderRenderCube.cpp
  mrShaderRenderSphere.cpp
  mrShadingGroup.cpp
  mrShadingGroupFluid.cpp
  mrShadingGroupFur.cpp
  mrShadingGroupParticles.cpp
  mrShadingGroupPfxHair.cpp
  mrShadingGroupPfxStroke.cpp
  mrShape.cpp
  mrSocket.cpp
  mrStandalone.cpp
  mrStackTrace.cpp
  mrSubd.cpp
  mrSwatchRender.cpp
  mrTexture.cpp
  mrThread.cpp
  mrTranslator.cpp
  mrUserData.cpp
  mrUserDataObject.cpp
  mrUserDataReferenceObject.cpp
  mrUserDataVertex.cpp
  mrVertexColors.cpp
  pluginMain.cpp
  rectangularLightLocator.cpp
  sphericalLightLocator.cpp
  )

ADD_DEFINITIONS( -DMR_NO_RAYLIB )



#
# List directories for -I and -L options.
#
INCLUDE_DIRECTORIES(
  .  # needed for swig files
  ${MAYA_INCLUDE_DIR}
#  ${OPENEXR_INCLUDE_DIR}
  ${RLM_INCLUDE_DIR}
  )

#
# List libraries your plugin uses
#
SET( LIBRARIES
  ${MAYA_FX_LIBRARIES}
  ${MAYA_UI_LIBRARIES}
  ${MAYA_RENDER_LIBRARIES}
  ${MAYA_ANIM_LIBRARIES}
  ${MAYA_BASE_LIBRARIES}
#  ${OPENEXR_LIBRARIES}
  ${OS_LIBRARIES}
  ${OPENGL_glu_LIBRARY}
  ${OPENGL_gl_LIBRARY}
  ${RLM_LIBRARIES}
)

IF( USE_SFIO AND SFIO_FOUND )
  ADD_DEFINITIONS( -DUSE_SFIO )
  INCLUDE_DIRECTORIES( ${SFIO_INCLUDE_DIR} )
  SET(LIBRARIES ${LIBRARIES} ${SFIO_LIBRARIES} )
ENDIF( USE_SFIO AND SFIO_FOUND )

#
# Create dummy target for the parser(s)
#
ADD_CUSTOM_TARGET(miparser echo "Creating parsers")


#
# Create custom command for flex/lex (note the outputs)
#
ADD_CUSTOM_COMMAND(
   SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/grammar.lex"
   COMMAND ${FLEX_EXECUTABLE} 
   ARGS -o"${CMAKE_CURRENT_BINARY_DIR}/lex.yy.c" 
        -Pmrl_yy -8 -CFe "${CMAKE_CURRENT_SOURCE_DIR}/grammar.lex"
   TARGET miparser
   OUTPUTS "${CMAKE_CURRENT_BINARY_DIR}/lex.yy.c"
   )

#
# Create custom commands for bison/yacc (note the DEPENDS)
#
ADD_CUSTOM_COMMAND(
  SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/grammar.y"
  COMMAND ${BISON_EXECUTABLE} 
  ARGS -l -pmrl_yy  -o"${CMAKE_CURRENT_BINARY_DIR}/grammar.tab.cpp" 
             "${CMAKE_CURRENT_SOURCE_DIR}/grammar.y"
  TARGET miparser
  DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/lex.yy.c"
  OUTPUTS "${CMAKE_CURRENT_BINARY_DIR}/grammar.tab.cpp"
)

ADD_CUSTOM_COMMAND(
  SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/fileobjgrammar.y"
  COMMAND ${BISON_EXECUTABLE} 
  ARGS -d -pmrl_fileobj -l -o"${CMAKE_CURRENT_BINARY_DIR}/fileobjgrammar.cpp" 
  "${CMAKE_CURRENT_SOURCE_DIR}/fileobjgrammar.y"
  TARGET miparser
  DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/lex.yy.c"
  OUTPUTS "${CMAKE_CURRENT_BINARY_DIR}/fileobjgrammar.cpp"
)

ADD_CUSTOM_COMMAND(
  SOURCE "${CMAKE_CURRENT_BINARY_DIR}/fileobjgrammar.cpp"
  COMMAND ${SED_EXECUTABLE} 
  ARGS -e 's/YYSTYPE mrl_yylval\;/extern YYSTYPE mrl_yylval\;/' 
        < "${CMAKE_CURRENT_BINARY_DIR}/fileobjgrammar.cpp"
        > "${CMAKE_CURRENT_BINARY_DIR}/fileobjgrammar.fixed.cpp"
  TARGET miparser
  DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/fileobjgrammar.cpp"
  OUTPUTS "${CMAKE_CURRENT_BINARY_DIR}/fileobjgrammar.fixed.cpp"
)


IF( WIN32 )
  ADD_DEFINITIONS( -DYY_NO_UNISTD_H -GR ) # RTTI enabled
ENDIF( WIN32 )


# Add parsers to the list of sources

# Since grammar files may not exists yet when cmake is run, mark
# them as 'generated'.

SET( SOURCES ${SOURCES} 
  "${CMAKE_CURRENT_BINARY_DIR}/lex.yy.c" 
  "${CMAKE_CURRENT_BINARY_DIR}/grammar.tab.cpp" 
  "${CMAKE_CURRENT_BINARY_DIR}/fileobjgrammar.fixed.cpp" )

SET_SOURCE_FILES_PROPERTIES("${CMAKE_CURRENT_BINARY_DIR}/grammar.tab.cpp" 
  GENERATED)
SET_SOURCE_FILES_PROPERTIES("${CMAKE_CURRENT_BINARY_DIR}/fileobjgrammar.fixed.cpp" 
  GENERATED)
SET_SOURCE_FILES_PROPERTIES("${CMAKE_CURRENT_BINARY_DIR}/lex.yy.c"                 
  GENERATED)

SET_SOURCE_FILES_PROPERTIES( pluginMain.cpp 
  OBJECT_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/mrLiquid.lds" )

#
# Run a macro to create a plugin
#
CREATE_MAYA_PLUG_IN( 
  "${PLUGIN_NAME}" "${PLUGIN_VERSION}" 
  "${SOURCES}" "${LIBRARIES}"
)

# IF( CMAKE_BUILD_TYPE STREQUAL "Release" )
#   CREATE_MAYA_PLUG_IN( 
#     "${PLUGIN_NAME}-demo" "${PLUGIN_VERSION}" 
#     "${SOURCES}" "${LIBRARIES}" 
#     )
  
#   SET_TARGET_PROPERTIES( "${PLUGIN_NAME}-demo"
#     PROPERTIES
#     COMPILE_FLAGS "-DFILMAURA_DEMO"
#     )
# ENDIF( CMAKE_BUILD_TYPE STREQUAL "Release" )



SET( SRCS )
FOREACH( it ${SOURCES} )
  GET_FILENAME_COMPONENT( abs "${it}" ABSOLUTE )
  SET( SRCS ${SRCS} "${abs}" )
ENDFOREACH( it ${SOURCES} )

#
# Run a macro to create geoshader
#
# ADD_SUBDIRECTORY( raylib )

#
# Copy DLLs used
#
IF( NOT OPENEXR_STATIC )
  IF( WIN32 )

    FOREACH( it ${OPENEXR_LIBRARIES} )
      STRING(REPLACE ".lib" ".dll" it "${it}")
      STRING(REPLACE "lib/" "bin/" it "${it}")

      GET_FILENAME_COMPONENT( OUT ${it} NAME )

      MESSAGE( STATUS
	"> cp ${it} ${LIBRARY_OUTPUT_PATH}/maya${MAYA_VERSION}/lib/${OUT}" )
      EXECUTE_PROCESS( COMMAND ${CMAKE_COMMAND} -E
	copy "${it}" "${LIBRARY_OUTPUT_PATH}/maya${MAYA_VERSION}/lib/${OUT}"
	RESULT_VARIABLE result
	)
      IF( NOT result EQUAL 0 )
	MESSAGE( ERROR "copy command failed ${result}" )
      ENDIF( NOT result EQUAL 0 )
    ENDFOREACH( it )

  ENDIF( WIN32 )
ENDIF( NOT OPENEXR_STATIC )



#
# Copy openexr libraries to same location of .mll
#

IF(SFIO_FOUND)
  MESSAGE( STATUS "SFIO include:    ${SFIO_INCLUDE_DIR}" )
  MESSAGE( STATUS "SFIO library:    ${SFIO_LIBRARY_DIR}" )
ENDIF(SFIO_FOUND)
