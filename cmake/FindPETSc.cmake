
# module to look for PETSc (in a simple way...), defining the following values
# PETSC_INCLUDE_DIRS = where headers can be found
# PETSC_LIBRARIES    = where libraries can be found
# CF3_HAVE_PETSC     = set to true after finding the library

option( CF3_SKIP_PETSC "Skip search for PETSc" OFF )
if( NOT CF3_SKIP_PETSC )

  coolfluid_set_trial_include_path( ${PETSC_HOME}/include $ENV{PETSC_HOME}/include )
  coolfluid_set_trial_library_path( ${PETSC_HOME}/lib     $ENV{PETSC_HOME}/lib     )
  find_path(PETSC_INCLUDE_DIRS petsc.h ${TRIAL_INCLUDE_PATHS} NO_DEFAULT_PATH)
  find_path(PETSC_INCLUDE_DIRS petsc.h)
  find_library(PETSC_LIBRARIES petsc ${TRIAL_LIBRARY_PATHS} NO_DEFAULT_PATH)
  find_library(PETSC_LIBRARIES petsc)

  coolfluid_add_trial_include_path( ${PETSC_HOME}/conf $ENV{PETSC_HOME}/conf )
  find_path(PETSC_CONFIG PETScConfig.cmake ${TRIAL_INCLUDE_PATHS} NO_DEFAULT_PATH)
  find_path(PETSC_CONFIG PETScConfig.cmake)
  if( PETSC_CONFIG )
    include(${PETSC_CONFIG}/PETScConfig.cmake OPTIONAL)
  endif()

endif()

coolfluid_set_package(
  PACKAGE     PETSC
  DESCRIPTION "Portable, Extensible Toolkit for Scientific Computation"
  URL         "http://www.mcs.anl.gov/petsc"
  TYPE        OPTIONAL
  VARS        PETSC_INCLUDE_DIRS PETSC_LIBRARIES )

