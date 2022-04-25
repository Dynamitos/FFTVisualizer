#
# Find OPENAL
#
# Try to find OPENAL library.
# This module defines the following variables:
# - OPENAL_INCLUDE_DIRS
# - OPENAL_LIBRARIES
# - OPENAL_FOUND
#
# The following variables can be set as arguments for the module.
# - OPENAL_ROOT : Root library directory of OPENAL
# - OPENAL_USE_STATIC_LIBS : Specifies to use static version of OPENAL library (Windows only)
#
# References:
# - https://github.com/progschj/OpenGL-Examples/blob/master/cmake_modules/FindOPENAL.cmake
# - https://bitbucket.org/Ident8/cegui-mk2-opengl3-renderer/src/befd47200265/cmake/FindOPENAL.cmake
#

# Additional modules
include(FindPackageHandleStandardArgs)

if (WIN32)
	# Find include files
	find_path(
		OPENAL_INCLUDE_DIR
		NAMES AL/al.h
		PATHS
			$ENV{PROGRAMFILES}/include
			${OPENAL_ROOT}
		DOC "The directory where AL/al.h resides")

	# Find library files
	find_library(
		OPENAL_LIBRARY
		NAMES al32.lib
		PATHS
			$ENV{PROGRAMFILES}/lib
			${OPENAL_ROOT}/lib)

else()
	# Find include files
	find_path(
		OPENAL_INCLUDE_DIR
		NAMES OPENAL/OPENAL3.h
		PATHS
			/usr/include
			/usr/local/include
			/sw/include
			/opt/local/include
		DOC "The directory where GL/OPENAL.h resides")

	# Find library files	
	find_file(
		OPENAL_LIBRARY
		NAMES libOPENAL${CMAKE_DEBUG_POSTFIX}.so
		PATHS	
			/usr/lib64
			/usr/lib
			/usr/local/lib64
			/usr/local/lib
			/sw/lib
			/opt/local/lib
			${OPENAL_ROOT}/src
	)
endif()

# Handle REQUIRD argument, define *_FOUND variable
find_package_handle_standard_args(OpenAL DEFAULT_MSG OPENAL_INCLUDE_DIR OPENAL_LIBRARY)

# Define OPENAL_LIBRARIES and OPENAL_INCLUDE_DIRS
if (OPENAL_FOUND)
	set(OPENAL_LIBRARIES ${OPENAL_LIBRARY})
	set(OPENAL_INCLUDE_DIRS ${OPENAL_INCLUDE_DIR})
endif()

# Hide some variables
mark_as_advanced(OPENAL_INCLUDE_DIR OPENAL_LIBRARY)