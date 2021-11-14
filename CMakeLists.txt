cmake_minimum_required(VERSION 3.20)
include("cmake/HunterGate.cmake")

set(CPM_DOWNLOAD_LOCATION "${CMAKE_SOURCE_DIR}/cmake/CPM.cmake")
set(CPM_DOWNLOAD_VERSION 0.32.2)

message(https://github.com/TheLartians/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake)
if (NOT (EXISTS ${CPM_DOWNLOAD_LOCATION} AND CPM_VERSION STREQUAL CPM_DOWNLOAD_VERSION))
    message(STATUS "Downloading CPM.cmake")
    file(DOWNLOAD https://github.com/TheLartians/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake ${CPM_DOWNLOAD_LOCATION})
endif ()

include(${CPM_DOWNLOAD_LOCATION})


HunterGate(
        URL "https://github.com/cpp-pm/hunter/archive/v0.23.269.tar.gz"
        SHA1 "64024b7b95b4c86d50ae05b926814448c93a70a0"
)


project(PhysarumSim)


set(CMAKE_CXX_STANDARD 20)

add_compile_definitions(GLFW_INCLUDE_NONE)
add_compile_definitions(GLM_FORCE_SWIZZLE)
add_compile_definitions(FMT_HEADER_ONLY )

find_package(OpenGL REQUIRED)

include_directories(include)
include_directories(src)
include_directories(include/stb)

find_package(GLFW3 REQUIRED)

CPMAddPackage(
        NAME pf_common
        GITHUB_REPOSITORY PetrFlajsingr/pf_common
        GIT_TAG master
)


CPMAddPackage(
        NAME pf_mainloop
        GITHUB_REPOSITORY PetrFlajsingr/pf_mainloop
        GIT_TAG master
)

CPMAddPackage(
        NAME pf_glfw
        GITHUB_REPOSITORY PetrFlajsingr/pf_glfw
        GIT_TAG master
        OPTIONS "PF_GLFW_OPENGL ON"
)
add_compile_definitions(PF_GLFW_OPENGL)

CPMAddPackage(
        NAME pf_imgui
        GITHUB_REPOSITORY PetrFlajsingr/pf_imgui
        GIT_TAG master
        OPTIONS "ENABLE_STYLES ON"
)

add_compile_definitions(PF_IMGUI_ENABLE_STYLES)

find_package(FFmpeg COMPONENTS AVCODEC AVFORMAT AVUTIL AVDEVICE REQUIRED) #add here the list of ffmpeg components required

if(FFMPEG_FOUND)
    #  FFMPEG_INCLUDE_DIRS  - Include directory necessary for using the required components headers.
    #  FFMPEG_LIBRARIES     - Link these to use the required ffmpeg components.
    #  FFMPEG_DEFINITIONS   - Compiler switches required for using the required ffmpeg components.
    message("FFMPEG_INCLUDE_DIRS = ${FFMPEG_INCLUDE_DIRS} ")
    message("FFMPEG_LIBRARIES = ${FFMPEG_LIBRARIES} ")
    message("FFMPEG_DEFINITIONS = ${FFMPEG_DEFINITIONS} ")

    include_directories(${FFMPEG_INCLUDE_DIRS})

endif()

add_library(glad src/glad.c)

file(GLOB GEGL_SOURCES src/geGL/*.cpp src/geGL/private/*.cpp)

add_executable(PhysarumSim
        src/main.cpp
        src/utils/files.h
        src/utils/files.cpp
        src/ui/UI.cpp
        src/ui/UI.h
        ${pf_imgui_SOURCE_DIR}/src/pf_imgui/backends/ImGuiGlfwOpenGLInterface.cpp
        ${pf_imgui_SOURCE_DIR}/src/pf_imgui/backends/impl/imgui_impl_glfw.cpp
        ${pf_imgui_SOURCE_DIR}/src/pf_imgui/backends/impl/imgui_impl_opengl3.cpp
        src/renderers/PhysarumRenderer.cpp src/renderers/PhysarumRenderer.h
        src/geGL/Shader.cpp
        src/geGL/Shader.h
        ${GEGL_SOURCES}
        src/simulation/PhysarumSimulator.cpp
        src/simulation/PhysarumSimulator.h
        src/simulation/Particle.h
        src/utils/rand.h
        src/simulation/generators/ParticleGenerator.h
        src/simulation/generators/RandomParticleGenerator.cpp
        src/simulation/generators/RandomParticleGenerator.h
        src/simulation/generators/PointParticleGenerator.cpp
        src/simulation/generators/PointParticleGenerator.h
        src/simulation/SimConfig.h
        src/simulation/generators/InwardCircleParticleGenerator.cpp
        src/simulation/generators/InwardCircleParticleGenerator.h
        src/simulation/generators/RandomCircleParticleGenerator.cpp
        src/simulation/generators/RandomCircleParticleGenerator.h
        src/ui/SpeciesPanel.cpp
        src/ui/SpeciesPanel.h
        src/ui/SpeciesColorPanel.cpp
        src/ui/SpeciesColorPanel.h
        src/simulation/SimConfig.cpp
        src/utils/FPSCounter.cpp
        src/utils/FPSCounter.h
        src/ui/MouseInteractionPanel.cpp
        src/ui/MouseInteractionPanel.h
        src/ui/SpeciesInteractionListbox.cpp
        src/ui/SpeciesInteractionListbox.h
        src/simulation/generators/UniformParticleGenerator.cpp
        src/simulation/generators/UniformParticleGenerator.h
        src/images/Formats.h
        src/images/save.cpp
        src/images/save.h src/ui/SimulationControlsPanel.cpp src/ui/SimulationControlsPanel.h src/GlobalThreadpool.h src/images/VideoRecorder.cpp src/images/VideoRecorder.h src/ui/RecorderPanel.cpp src/ui/RecorderPanel.h src/ui/AboutPanel.cpp src/ui/AboutPanel.h src/ui/HelpPanel.cpp src/ui/HelpPanel.h src/ui/help_data/HelpLoader.h src/ui/help_data/HelpData.h src/ui/help_data/FolderHelpLoader.cpp src/ui/help_data/FolderHelpLoader.h)


target_link_libraries(PhysarumSim
        glad
        ${OPENGL_LIBRARIES}
        pf_common::pf_common
        pf_imgui::pf_imgui
        pf_mainloop::pf_mainloop
        pf_glfw::pf_glfw
        ${FFMPEG_LIBRARIES}
        )

if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${PROJECT_SOURCE_DIR}/dlls"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>)
endif()
#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy
#        "${PROJECT_SOURCE_DIR}/config.toml"
#        $<TARGET_FILE_DIR:${PROJECT_NAME}>/config.toml)


add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/src/shaders"
        "${PROJECT_SOURCE_DIR}/resources/shaders")


#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()