cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)

# Handle superbuild first
option (USE_SUPERBUILD "Whether or not a superbuild should be invoked" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(EXTERNAL_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/external)
set(GLM_ROOT ${EXTERNAL_ROOT}/glm)
set(GLFW_ROOT ${EXTERNAL_ROOT}/glfw)
set(GLAD_ROOT ${EXTERNAL_ROOT}/glad)
set(STB_ROOT ${EXTERNAL_ROOT}/stb)
set(JSON_ROOT ${EXTERNAL_ROOT}/json)
set(SDL_ROOT ${EXTERNAL_ROOT}/sdl)
set(FFmpeg_ROOT ${EXTERNAL_ROOT}/ffmpeg)
set(OPENAL_ROOT ${EXTERNAL_ROOT}/openal)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/)

if (USE_SUPERBUILD)
  project (SUPERBUILD NONE)
  # execute the superbuild (this script will be invoked again without the
  # USE_SUPERBUILD option this time)
  include (cmake/SuperBuild.cmake)
  return()                      # stop processing this file further
else()
  project (MusicVisualizer)
  endif()
  
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  #Workaround for vs, because it places artifacts into an additional subfolder
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR})
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR})
  
  
if(UNIX)
  find_package(Threads REQUIRED)
endif()
find_package(GLFW REQUIRED)
find_package(GLM REQUIRED)
find_package(GLAD REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenAL REQUIRED)
find_package(JSON REQUIRED)
find_package(STB REQUIRED)
find_package(SDL REQUIRED)
find_package(FFmpeg REQUIRED avcodec avdevice avfilter avformat avutil swscale swresample)

add_executable(Visualizer "")

target_include_directories(Visualizer PRIVATE ${GLM_INCLUDE_DIRS})
target_include_directories(Visualizer PRIVATE ${GLFW_INCLUDE_DIRS})
target_include_directories(Visualizer PRIVATE ${GLAD_INCLUDE_DIRS})
target_include_directories(Visualizer PRIVATE ${OPENGL_INCLUDE_DIRS})
target_include_directories(Visualizer PRIVATE ${OPENAL_INCLUDE_DIRS})
target_include_directories(Visualizer PRIVATE ${JSON_INCLUDE_DIRS})
target_include_directories(Visualizer PRIVATE ${STB_INCLUDE_DIRS})
target_include_directories(Visualizer PRIVATE ${SDL_INCLUDE_DIRS})
target_include_directories(Visualizer PRIVATE ${FFmpeg_INCLUDE_DIRS})
target_include_directories(Visualizer PRIVATE src/)

target_link_libraries(Visualizer ${GLFW_LIBRARIES})
target_link_libraries(Visualizer ${OPENGL_LIBRARIES})
target_link_libraries(Visualizer ${OPENAL_LIBRARIES})
target_link_libraries(Visualizer ${GLAD_LIBRARIES})
target_link_libraries(Visualizer ${SDL_LIBRARIES})
target_link_libraries(Visualizer ${FFmpeg_LIBRARIES})
if(UNIX)
  target_link_libraries(Visualizer Threads::Threads)
  target_link_libraries(Visualizer dl)
elseif(WIN32)
  target_link_libraries(Visualizer winmm)
  target_link_libraries(Visualizer Setupapi)
  target_link_libraries(Visualizer version)
  target_link_libraries(Visualizer imm32)
endif()


add_subdirectory(src/)

if(MSVC)
  set(_CRT_SECURE_NO_WARNINGS)
  target_compile_options(Visualizer PRIVATE /Zi /MP /DEBUG)
else()
  target_compile_options(Visualizer PRIVATE -g -Wall -Wextra -Wno-delete-incomplete -pedantic -fcoroutines)
endif()


if(WIN32)
  add_custom_target(copy-binaries ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${FFmpeg_ROOT}/bin $<TARGET_FILE_DIR:Visualizer>
    )
else()
  add_custom_target(copy-binaries ALL COMMAND "")
endif()


add_custom_target(MusicVisualizer ALL
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:Visualizer>
  COMMAND 
	DEPENDS Visualizer copy-binaries)

  