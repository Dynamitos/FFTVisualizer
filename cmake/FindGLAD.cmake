#
# Find GLAD
#
# Try to find GLAD library.
# This module defines the following variables:
# - GLAD_INCLUDE_DIRS
# - GLAD_LIBRARIES
# - GLAD_FOUND
#
# The following variables can be set as arguments for the module.
# - GLAD_ROOT : Root library directory of GLAD
# - GLAD_USE_STATIC_LIBS : Specifies to use static version of GLAD library (Windows only)
#
# References:
# - https://github.com/progschj/OpenGL-Examples/blob/master/cmake_modules/FindGLAD.cmake
# - https://bitbucket.org/Ident8/cegui-mk2-opengl3-renderer/src/befd47200265/cmake/FindGLAD.cmake
#

# Additional modules
include(FindPackageHandleStandardArgs)

if (WIN32)
	# Find include files
	find_path(
		GLAD_INCLUDE_DIR
		NAMES glad/glad.h
		PATHS
			$ENV{PROGRAMFILES}/include
			${GLAD_ROOT}/include
		DOC "The directory where GL/GLAD.h resides")

	# Find library files
	find_library(
		GLAD_LIBRARY
		NAMES glad${CMAKE_DEBUG_POSTFIX}.lib
		PATHS
			$ENV{PROGRAMFILES}/lib
			${GLAD_ROOT}/lib
			${GLAD_ROOT}
		PATH_SUFFIXES Debug Release)

else()
	# Find include files
	find_path(
		GLAD_INCLUDE_DIR
		NAMES GLAD/GLAD3.h
		PATHS
			/usr/include
			/usr/local/include
			/sw/include
			/opt/local/include
		DOC "The directory where GL/GLAD.h resides")

	# Find library files	
	find_file(
		GLAD_LIBRARY
		NAMES libGLAD${CMAKE_DEBUG_POSTFIX}.so
		PATHS	
			/usr/lib64
			/usr/lib
			/usr/local/lib64
			/usr/local/lib
			/sw/lib
			/opt/local/lib
			${GLAD_ROOT}/src
	)
endif()

# Handle REQUIRD argument, define *_FOUND variable
find_package_handle_standard_args(GLAD DEFAULT_MSG GLAD_INCLUDE_DIR GLAD_LIBRARY)

# Define GLAD_LIBRARIES and GLAD_INCLUDE_DIRS
if (GLAD_FOUND)
	set(GLAD_LIBRARIES ${GLAD_LIBRARY})
	set(GLAD_INCLUDE_DIRS ${GLAD_INCLUDE_DIR})
endif()

# Hide some variables
mark_as_advanced(GLAD_INCLUDE_DIR GLAD_LIBRARY)