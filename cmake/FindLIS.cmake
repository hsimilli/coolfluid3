
# module to look for LIS library, defining the following values
#   LIS_INCLUDE_DIRS
#   LIS_LIBRARIES
#   CF3_HAVE_LIS
#
# uses CMake/environment variables:
#   LIS_HOME

option( CF3_SKIP_LIS "Skip search for LIS library" OFF )
if( NOT CF3_SKIP_LIS )


  if (DEFINED LIS_HOME OR DEFINED ENV{LIS_HOME})
    coolfluid_set_trial_include_path(${LIS_HOME}/include $ENV{LIS_HOME}/include)
    coolfluid_set_trial_library_path(${LIS_HOME}/lib     $ENV{LIS_HOME}/lib)
  endif()

  find_path(LIS_INCLUDE_DIR lis.h PATHS ${TRIAL_INCLUDE_PATHS} NO_DEFAULT_PATH)
  find_path(LIS_INCLUDE_DIR lis.h)
  find_library(LIS_LIBRARY NAMES lis PATHS ${TRIAL_LIBRARY_PATHS} NO_DEFAULT_PATH)
  find_library(LIS_LIBRARY NAMES lis)
  list(APPEND LIS_INCLUDE_DIRS ${LIS_INCLUDE_DIR})
  list(APPEND LIS_LIBRARIES ${LIS_LIBRARY})


endif()

coolfluid_set_package(
  PACKAGE     LIS
  DESCRIPTION "Library of Iterative Solvers for linear systems"
  URL         "http://www.ssisc.org/lis/"
  TYPE        OPTIONAL
  VARS        LIS_LIBRARIES LIS_INCLUDE_DIRS )

