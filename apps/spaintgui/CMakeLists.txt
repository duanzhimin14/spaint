#####################################
# CMakeLists.txt for apps/spaintgui #
#####################################

###########################
# Specify the target name #
###########################

SET(targetname spaintgui)

############################################################################################
# Offer a variety of options, e.g. for low power machines and those with low USB bandwidth #
############################################################################################

INCLUDE(${PROJECT_SOURCE_DIR}/cmake/OfferFocusReacquisition.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/OfferLowPowerSupport.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/OfferLowUSBBandwidthSupport.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/OfferPixelDebugging.cmake)

################################
# Specify the libraries to use #
################################

INCLUDE(${PROJECT_SOURCE_DIR}/cmake/UseALGLIB.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/UseArrayFire.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/UseBoost.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/UseCUDA.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/UseEigen.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/UseGLEW.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/UseGLUT.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/UseGraphviz.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/UseGrove.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/UseInfiniTAM.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/UseLeap.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/UseLibRoyale.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/UseLodePNG.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/UseOpenCV.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/UseOpenGL.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/UseOpenMP.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/UseOpenNI.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/UseOVR.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/UseRealSense.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/UseScoreForests.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/UseSDL.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/UseVicon.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/UseZed.cmake)

# This has to be included last since libtorch provides a tweaked version of FindCUDA.cmake that messes with the flags,
# causing build failures if other packages that require CUDA (e.g. the ZED ones) are included after it.
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/UseTorch.cmake)

#############################
# Specify the project files #
#############################

##
SET(commands_sources
commands/MarkVoxelsCommand.cpp
)

SET(commands_headers
commands/MarkVoxelsCommand.h
)

##
SET(core_sources
core/CollaborativePipeline.cpp
core/Model.cpp
core/MultiScenePipeline.cpp
core/ObjectivePipeline.cpp
core/SemanticPipeline.cpp
core/SLAMPipeline.cpp
)

SET(core_headers
core/CollaborativePipeline.h
core/Model.h
core/MultiScenePipeline.h
core/ObjectivePipeline.h
core/SemanticPipeline.h
core/SLAMPipeline.h
)

##
SET(debugging_sources)
SET(debugging_headers)

IF(BUILD_GROVE AND BUILD_SPAINTGUI_DEBUGHOOKS AND WITH_OPENCV)
  SET(debugging_sources ${debugging_sources} debugging/RelocaliserFiguresGenerator.cpp)
  SET(debugging_headers ${debugging_headers} debugging/RelocaliserFiguresGenerator.h)
ENDIF()

##
SET(renderers_sources
renderers/HeadlessRenderer.cpp
renderers/Renderer.cpp
renderers/WindowedRenderer.cpp
)

SET(renderers_headers
renderers/HeadlessRenderer.h
renderers/Renderer.h
renderers/WindowedRenderer.h
)

IF(WITH_OVR)
  SET(renderers_sources ${renderers_sources} renderers/RiftRenderer.cpp)
  SET(renderers_headers ${renderers_headers} renderers/RiftRenderer.h)
ENDIF()

##
SET(sequences_sources
sequences/Sequence.cpp
sequences/SpaintSequence.cpp
)

SET(sequences_headers
sequences/Sequence.h
sequences/SpaintSequence.h
)

##
SET(subwindows_sources
subwindows/Subwindow.cpp
subwindows/SubwindowConfiguration.cpp
)

SET(subwindows_headers
subwindows/Subwindow.h
subwindows/SubwindowConfiguration.h
)

##
SET(toplevel_sources
main.cpp
Application.cpp
CPUInstantiations.cpp
)

SET(toplevel_headers
Application.h
)

IF(WITH_CUDA)
  SET(toplevel_sources ${toplevel_sources} CUDAInstantiations.cu)
ENDIF()

#################################################################
# Collect the project files into sources, headers and templates #
#################################################################

SET(sources
${commands_sources}
${core_sources}
${debugging_sources}
${renderers_sources}
${sequences_sources}
${subwindows_sources}
${toplevel_sources}
)

SET(headers
${commands_headers}
${core_headers}
${debugging_headers}
${renderers_headers}
${sequences_headers}
${subwindows_headers}
${toplevel_headers}
)

#############################
# Specify the source groups #
#############################

SOURCE_GROUP("" FILES ${toplevel_sources} ${toplevel_headers})
SOURCE_GROUP(commands FILES ${commands_sources} ${commands_headers})
SOURCE_GROUP(core FILES ${core_sources} ${core_headers})
SOURCE_GROUP(debugging FILES ${debugging_sources} ${debugging_headers})
SOURCE_GROUP(relocalisation FILES ${relocalisation_sources} ${relocalisation_headers})
SOURCE_GROUP(renderers FILES ${renderers_sources} ${renderers_headers})
SOURCE_GROUP(sequences FILES ${sequences_sources} ${sequences_headers})
SOURCE_GROUP(subwindows FILES ${subwindows_sources} ${subwindows_headers})

##########################################
# Specify additional include directories #
##########################################

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/modules/itmx/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/modules/oglx/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/modules/orx/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/modules/rafl/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/modules/rigging/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/modules/spaint/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/modules/tvginput/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/modules/tvgutil/include)

##########################################
# Specify the target and where to put it #
##########################################

INCLUDE(${PROJECT_SOURCE_DIR}/cmake/SetCUDAAppTarget.cmake)

#################################
# Specify the libraries to link #
#################################

# Note: spaint needs to precede rafl on Linux.
TARGET_LINK_LIBRARIES(${targetname} spaint itmx oglx orx rafl rigging tvginput tvgutil)

INCLUDE(${PROJECT_SOURCE_DIR}/cmake/LinkGrove.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/LinkInfiniTAM.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/LinkSDL.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/LinkALGLIB.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/LinkArrayFire.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/LinkBoost.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/LinkCUDA.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/LinkGLEW.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/LinkGLUT.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/LinkLeap.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/LinkLibRoyale.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/LinkLodePNG.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/LinkOpenCV.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/LinkOpenGL.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/LinkOpenNI.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/LinkOVR.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/LinkRealSense.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/LinkScoreForests.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/LinkTorch.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/LinkVicon.cmake)
INCLUDE(${PROJECT_SOURCE_DIR}/cmake/LinkZed.cmake)

#########################################
# Copy resource files to the build tree #
#########################################

ADD_CUSTOM_COMMAND(TARGET ${targetname} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/apps/spaintgui/resources" "$<TARGET_FILE_DIR:${targetname}>/resources")

IF(MSVC_IDE)
  ADD_CUSTOM_COMMAND(TARGET ${targetname} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/apps/spaintgui/resources" "${PROJECT_BINARY_DIR}/apps/spaintgui/resources")
ENDIF()

#############################
# Specify things to install #
#############################

INCLUDE(${PROJECT_SOURCE_DIR}/cmake/InstallApp.cmake)
INSTALL(DIRECTORY "${PROJECT_SOURCE_DIR}/apps/spaintgui/resources" DESTINATION "bin/apps/spaintgui")
