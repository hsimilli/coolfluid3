
# module to look for levmar library, defining the following values
#   LEVMAR_INCLUDE_DIRS
#   LEVMAR_LIBRARIES
#   CF3_HAVE_LEVMAR
#
# uses CMake/environment variables:
#   LEVMAR_HOME

option( CF3_SKIP_LEVMAR "Skip search for levmar library" OFF )
if( NOT CF3_SKIP_LEVMAR )


  if (DEFINED LEVMAR_HOME OR DEFINED ENV{LEVMAR_HOME})
    coolfluid_set_trial_include_path(${LEVMAR_HOME} $ENV{LEVMAR_HOME})
    coolfluid_set_trial_library_path(${LEVMAR_HOME} $ENV{LEVMAR_HOME})
  endif()

  find_path(LEVMAR_INCLUDE_DIR levmar.h PATHS ${TRIAL_INCLUDE_PATHS} NO_DEFAULT_PATH)
  find_path(LEVMAR_INCLUDE_DIR levmar.h)
  find_library(LEVMAR_LIBRARY NAMES levmar PATHS ${TRIAL_LIBRARY_PATHS} NO_DEFAULT_PATH)
  find_library(LEVMAR_LIBRARY NAMES levmar)
  list(APPEND LEVMAR_INCLUDE_DIRS ${LEVMAR_INCLUDE_DIR})
  list(APPEND LEVMAR_LIBRARIES ${LEVMAR_LIBRARY} ${LAPACK_LIBRARIES})


endif()

coolfluid_set_package(
  PACKAGE     levmar
  DESCRIPTION "Levenberg-Marquardt optimization library, small-scale dense implementation"
  URL         "http://users.ics.forth.gr/~lourakis/levmar"
  TYPE        OPTIONAL
  VARS        LEVMAR_LIBRARIES LEVMAR_INCLUDE_DIRS )

