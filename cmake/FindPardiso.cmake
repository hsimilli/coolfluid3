# this module looks for Pardiso library
# it will define the following values
#
# Needs environmental variables
#   PARDISO_HOME
# Sets
#   PARDISO_INCLUDE_DIR
#   PARDISO_LIBRARIES
#   CF3_HAVE_PARDISO
#

# IMPORTANT: on linux compilation with -fopenmp flag is mandatory

option( CF3_SKIP_PARDISO "Skip search for Pardiso library" OFF )

if( NOT CF3_SKIP_PARDISO )

  coolfluid_set_trial_include_path("") # clear include search path
  coolfluid_set_trial_library_path("") # clear library search path

  if(DEFINED PARDISO_HOME)
    coolfluid_add_trial_library_path(${PARDISO_HOME}     )
    coolfluid_add_trial_library_path(${PARDISO_HOME}/lib )
  endif()
  if(DEFINED ENV{PARDISO_HOME})
    coolfluid_add_trial_library_path($ENV{PARDISO_HOME}     )
    coolfluid_add_trial_library_path($ENV{PARDISO_HOME}/lib )
  endif()

  find_library(PARDISO_LIBRARY pardiso ${TRIAL_LIBRARY_PATHS} NO_DEFAULT_PATH)
  find_library(PARDISO_LIBRARY pardiso )

  list(APPEND PARDISO_LIBRARIES ${PARDISO_LIBRARY})

  list(APPEND PARDISO_LIBRARIES  ${BLAS_LIBRARIES} ${LAPACK_LIBRARIES})

  # if(MUPHYS_HAVE_IOMP5 AND APPLE)
  #   list(APPEND PARDISO_LIBRARIES ${IOMP5_LIBRARIES})
  # endif()

endif( NOT CF3_SKIP_PARDISO )

coolfluid_set_package( PACKAGE Pardiso
                       DESCRIPTION "Pardiso sparse direct solver"
                       URL "http://www.pardiso-project.org"
                       TYPE OPTIONAL
                       VARS PARDISO_LIBRARIES
                       QUIET )
