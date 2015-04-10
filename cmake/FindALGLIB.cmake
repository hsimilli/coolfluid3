
# module to look for ALGLIB library, defining the following values
# ALGLIB_INCLUDE_DIRS
# ALGLIB_LIBRARIES
# CF3_HAVE_ALGLIB
#
# uses CMake/environment variables:
#   ALGLIB_HOME

option( CF3_SKIP_ALGLIB "Skip search for ALGLIB library" OFF )
if( NOT CF3_SKIP_ALGLIB )


  if (DEFINED ALGLIB_HOME OR DEFINED ENV{ALGLIB_HOME})
    coolfluid_set_trial_include_path(${ALGLIB_HOME} $ENV{ALGLIB_HOME})
    coolfluid_set_trial_library_path(${ALGLIB_HOME} $ENV{ALGLIB_HOME})
  endif()

  find_path(ALGLIB_INCLUDE_DIRS optimization.h PATHS ${TRIAL_INCLUDE_PATHS} NO_DEFAULT_PATH)
  find_path(ALGLIB_INCLUDE_DIRS optimization.h)
  find_library(ALGLIB_LIBRARIES ALGLIB PATHS ${TRIAL_LIBRARY_PATHS} NO_DEFAULT_PATH)
  find_library(ALGLIB_LIBRARIES ALGLIB)


endif()

coolfluid_set_package(
  PACKAGE     ALGLIB
  DESCRIPTION "Cross-platform numerical analysis and data processing library (used for optimization)"
  URL         "http://www.alglib.net"
  TYPE        OPTIONAL
  VARS        ALGLIB_INCLUDE_DIRS ALGLIB_LIBRARIES )

