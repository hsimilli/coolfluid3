
# module to look for sparseLM library, defining the following values
#   SPARSELM_INCLUDE_DIRS
#   SPARSELM_LIBRARIES
#   CF3_HAVE_SPARSELM
#
# uses CMake/environment variables:
#   SPARSELM_HOME

option( CF3_SKIP_SPARSELM "Skip search for sparseLM library" OFF )
if( NOT CF3_SKIP_SPARSELM )


  if (DEFINED SPARSELM_HOME OR DEFINED ENV{SPARSELM_HOME})
    coolfluid_set_trial_include_path(${SPARSELM_HOME} $ENV{SPARSELM_HOME})
    coolfluid_set_trial_library_path(${SPARSELM_HOME} $ENV{SPARSELM_HOME})
  endif()

  find_path(SPARSELM_INCLUDE_DIR splm.h PATHS ${TRIAL_INCLUDE_PATHS} NO_DEFAULT_PATH)
  find_path(SPARSELM_INCLUDE_DIR splm.h)
  find_library(SPARSELM_LIBRARY NAMES splm PATHS ${TRIAL_LIBRARY_PATHS} NO_DEFAULT_PATH)
  find_library(SPARSELM_LIBRARY NAMES splm)
  list(APPEND SPARSELM_INCLUDE_DIRS ${SPARSELM_INCLUDE_DIR})
  list(APPEND SPARSELM_LIBRARIES ${SPARSELM_LIBRARY})


endif()

coolfluid_set_package(
  PACKAGE     sparseLM
  DESCRIPTION "Levenberg-Marquardt optimization library, large-scale sparse implementation"
  URL         "http://users.ics.forth.gr/~lourakis/sparseLM"
  TYPE        OPTIONAL
  VARS        SPARSELM_LIBRARIES SPARSELM_INCLUDE_DIRS )

