include (ExternalProject)

set(DEPENDENCIES)
set(EXTRA_CMAKE_ARGS)

#--------------GLFW------------------------------
list(APPEND DEPENDENCIES glfw)
set(ENKITS_BUILD_EXAMPLES OFF CACHE BOOL "Build basic example applications" )
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL  "GLFW lib only" )
set(GLFW_BUILD_TESTS OFF CACHE BOOL  "GLFW lib only" )
set(GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" )
set(GLFW_BUILD_INSTALL OFF CACHE BOOL  "GLFW lib only" )
set(GLFW_USE_STATIC_LIBS ON CACHE BOOL "GLFW lib only")

add_subdirectory(${GLFW_ROOT} ${GLFW_ROOT})

#--------------SDL------------------------------
list(APPEND DEPENDENCIES SDL2-static)

add_subdirectory(${SDL_ROOT} ${SDL_ROOT}/build)

find_library(SDL_LIBRARY
    NAMES SDL2${CMAKE_DEBUG_POSTFIX}.lib
    PATHS
        ${SDL_ROOT}/build
    PATH_SUFFIXES Debug)
#--------------GLAD------------------------------
list(APPEND DEPENDENCIES glad)

add_subdirectory(${GLAD_ROOT} ${GLAD_ROOT})

#--------------------JSON------------------
list(APPEND DEPENDENCIES nlohmann_json)
set(JSON_MultipleHeaders ON CACHE INTERNAL "")
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")

add_subdirectory(${JSON_ROOT} ${JSON_ROOT})

#--------------OPENAL---------------------------
#list(APPEND DEPENDENCIES OpenAL)

#add_subdirectory(${OPENAL_ROOT} ${OPENAL_ROOT})


#-----------------MusicVisualizer--------------------
ExternalProject_Add(MusicVisualizer
    DEPENDS ${DEPENDENCIES}
    SOURCE_DIR ${PROJECT_SOURCE_DIR}
    PREFIX ${CMAKE_BINARY_DIR}
    BINARY_DIR ${CMAKE_BINARY_DIR}
    CMAKE_ARGS -DUSE_SUPERBUILD=OFF ${EXTRA_CMAKE_ARGS}
    INSTALL_COMMAND "")
