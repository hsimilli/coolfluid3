# - Try to find Dlib
#
# Once done this will define
#
#  DLIB_INCLUDE_DIRS - the dlib include directory
#  CF3_HAVE_DLIB

coolfluid_set_trial_include_path("") # clear include search path
coolfluid_set_trial_library_path("") # clear library search path

# path to dlib sources
if(DEFINED DLIB_HOME)
  coolfluid_add_trial_include_path( ${DLIB_HOME} )
endif()

find_path( DLIB_INCLUDE_DIR dlib/matrix.h PATHS ${TRIAL_INCLUDE_PATHS}  NO_DEFAULT_PATH )
find_path( DLIB_INCLUDE_DIR dlib/matrix.h )
list(APPEND DLIB_INCLUDE_DIRS ${DLIB_INCLUDE_DIR})

mark_as_advanced(DLIB_INCLUDE_DIR DLIB_INCLUDE_DIRS)

coolfluid_set_package( PACKAGE Dlib
                       DESCRIPTION "Dlib"
                       URL "http://dlib.net"
                       VARS DLIB_INCLUDE_DIRS
                       QUIET )