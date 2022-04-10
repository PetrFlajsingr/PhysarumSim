cmake_minimum_required(VERSION 3.20)
project(PhysarumSim)


if (MSVC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.29.30129 AND CMAKE_VERSION VERSION_GREATER 3.20.3)
    set(CMAKE_CXX_STANDARD 23) # /std:c++latest - unlocks the non stable cpp20 features. For new 16.11 versions
else ()
    set(CMAKE_CXX_STANDARD 20) # /std:c++latest for msvc and -std=c++20 for everyone else.
endif ()

add_compile_definitions(GLFW_INCLUDE_NONE)
add_compile_definitions(GLM_FORCE_SWIZZLE)
#add_compile_definitions(FMT_HEADER_ONLY )

find_package(OpenGL REQUIRED)

include_directories(include)
include_directories(src)
include_directories(include/stb)

find_package(GLFW3 REQUIRED)
find_package(pf_common CONFIG REQUIRED)
find_package(magic_enum CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(range-v3 CONFIG REQUIRED)
find_package(tomlplusplus CONFIG REQUIRED)
find_package(pf_glfw CONFIG REQUIRED)
find_package(pf_mainloop CONFIG REQUIRED)
find_package(pf_imgui CONFIG REQUIRED)

add_compile_definitions(PF_GLFW_OPENGL)



if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    find_package(FFMPEG COMPONENTS AVCODEC AVFORMAT AVUTIL AVDEVICE REQUIRED) #add here the list of ffmpeg components required
    include_directories(${FFMPEG_INCLUDE_DIRS})
else ()
    find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h)
    find_library(AVCODEC_LIBRARY avcodec)

    find_path(AVFORMAT_INCLUDE_DIR libavformat/avformat.h)
    find_library(AVFORMAT_LIBRARY avformat)

    find_path(AVUTIL_INCLUDE_DIR libavutil/avutil.h)
    find_library(AVUTIL_LIBRARY avutil)

    find_path(AVDEVICE_INCLUDE_DIR libavdevice/avdevice.h)
    find_library(AVDEVICE_LIBRARY avdevice)

    find_path(SWSCALE_INCLUDE_DIR libswscale/swscale.h)
    find_library(SWSCALE_LIBRARY swscale)

    find_path(SWRESAMPLE_INCLUDE_DIR libswresample/swresample.h)
    find_library(SWRESAMPLE_LIBRARY swresample)
endif ()

add_library(glad src/glad.c)

file(GLOB GEGL_SOURCES include/geGL/*.cpp include/geGL/private/*.cpp)

add_executable(PhysarumSim
        src/main.cpp
        src/utils/files.h
        src/utils/files.cpp
        src/ui/UI.cpp
        src/ui/UI.h
        src/imgui/ImGuiGlfwOpenGLInterface.cpp
        src/imgui/impl/imgui_impl_glfw.cpp
        src/imgui/impl/imgui_impl_opengl3.cpp
        src/renderers/PhysarumRenderer.cpp src/renderers/PhysarumRenderer.h
        include/geGL/Shader.cpp
        include/geGL/Shader.h
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
        src/images/save.h src/ui/SimulationControlsPanel.cpp
        src/images/VideoRecorder.cpp
        src/ui/RecorderPanel.cpp
        src/ui/AboutPanel.cpp
        src/ui/HelpPanel.cpp
        src/ui/help_data/FolderHelpLoader.cpp
        src/ui/about_data/AboutData.h src/ui/about_data/AboutDataLoader.h src/ui/about_data/FolderAboutDataLoader.cpp src/ui/about_data/FolderAboutDataLoader.h)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_link_libraries(PhysarumSim
            glad
            ${OPENGL_LIBRARIES}
            pf_common::pf_common
            pf_imgui::pf_imgui
            pf_mainloop::pf_mainloop
            pf_glfw::pf_glfw
            ${FFMPEG_LIBRARIES}
            )
else ()
    target_include_directories(PhysarumSim PRIVATE ${AVCODEC_INCLUDE_DIR} ${AVFORMAT_INCLUDE_DIR} ${AVUTIL_INCLUDE_DIR} ${AVDEVICE_INCLUDE_DIR} ${SWSCALE_INCLUDE_DIR} ${SWRESAMPLE_INCLUDE_DIR})
    target_link_libraries(PhysarumSim
            glad
            ${OPENGL_LIBRARIES}
            pf_common::pf_common
            pf_imgui::pf_imgui
            pf_mainloop::pf_mainloop
            pf_glfw::pf_glfw
            ${AVCODEC_LIBRARY} ${AVFORMAT_LIBRARY} ${AVUTIL_LIBRARY} ${AVDEVICE_LIBRARY} ${SWSCALE_LIBRARY} ${SWRESAMPLE_LIBRARY}
            )
endif ()


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