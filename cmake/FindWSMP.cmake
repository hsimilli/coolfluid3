
# module to look for WSMP, defining the following values
# WSMP_LIBRARIES    = where libraries can be found
# CF3_HAVE_WSMP     = set to true after finding the library

option( CF3_SKIP_WSMP "Skip search for WSMP" OFF )
if( NOT CF3_SKIP_WSMP )

  coolfluid_set_trial_include_path("") # clear include search path
  coolfluid_set_trial_library_path("") # clear library search path

  if(DEFINED WSMP_HOME)
    coolfluid_add_trial_library_path(${WSMP_HOME})
    coolfluid_add_trial_library_path(${WSMP_HOME}/lib)
  endif()
  if(DEFINED ENV{WSMP_HOME})
    coolfluid_add_trial_library_path($ENV{WSMP_HOME})
    coolfluid_add_trial_library_path($ENV{WSMP_HOME}/lib)
  endif()

  find_library(WSMP_LIBRARIES wsmp64 ${TRIAL_LIBRARY_PATHS} NO_DEFAULT_PATH)
  find_library(WSMP_LIBRARIES wsmp64)

endif()

coolfluid_set_package(
  PACKAGE     WSMP
  DESCRIPTION "Watson Sparse Matrix Package"
  URL         "http://researcher.watson.ibm.com/researcher/view_project.php?id=1426"
  TYPE        OPTIONAL
  VARS        WSMP_LIBRARIES )

