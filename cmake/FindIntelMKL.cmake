
# module to look for Intel MKL libraries, defining the following values
#   INTELMKL_INCLUDE_DIRS
#   INTELMKL_LIBRARIES
#   CF3_HAVE_INTELMKL
#
# uses CMake/environment variables:
#   INTELMKL_HOME
#   IOMP5_HOME (optional)
#
# for linking line instructions, use:
# ${INTELMKL_HOME}/Documentation/en_US/mkl/mkl_link_line_advisor.htm

option( CF3_SKIP_INTELMKL "Skip search for MKL Pardiso library" OFF )
if( NOT CF3_SKIP_INTELMKL )


  # setup library search directories
  coolfluid_set_trial_include_path("")
  coolfluid_set_trial_library_path("")
  if( DEFINED INTELMKL_HOME )
    coolfluid_add_trial_include_path(${INTELMKL_HOME}/include)
    coolfluid_add_trial_library_path(${INTELMKL_HOME}/lib)
    coolfluid_add_trial_library_path(${INTELMKL_HOME}/lib/intel64)
    coolfluid_add_trial_library_path(${INTELMKL_HOME}/lib/ia32)
  endif()
  if( DEFINED ENV{INTELMKL_HOME} )
    coolfluid_add_trial_include_path($ENV{INTELMKL_HOME}/include)
    coolfluid_add_trial_library_path($ENV{INTELMKL_HOME}/lib)
    coolfluid_add_trial_library_path($ENV{INTELMKL_HOME}/lib/intel64)
    coolfluid_add_trial_library_path($ENV{INTELMKL_HOME}/lib/ia32)
  endif()
  if( DEFINED IOMP5_HOME )
    coolfluid_add_trial_library_path(${IOMP5_HOME}/lib)
    coolfluid_add_trial_library_path(${IOMP5_HOME}/lib/intel64)
    coolfluid_add_trial_library_path(${IOMP5_HOME}/lib/ia32)
  endif()
  if( DEFINED ENV{IOMP5_HOME} )
    coolfluid_add_trial_library_path($ENV{IOMP5_HOME}/lib)
    coolfluid_add_trial_library_path($ENV{IOMP5_HOME}/lib/intel64)
    coolfluid_add_trial_library_path($ENV{IOMP5_HOME}/lib/ia32)
  endif()

  # look for headers
  find_path(INTELMKL_INCLUDE_DIR mkl_pardiso.h PATHS ${TRIAL_INCLUDE_PATHS} NO_DEFAULT_PATH)
  find_path(INTELMKL_INCLUDE_DIR mkl_pardiso.h)
  list(APPEND INTELMKL_INCLUDE_DIRS ${INTELMKL_INCLUDE_DIR})

  # look for libraries
  find_library(INTELMKL_COMPUTATIONAL mkl_core ${TRIAL_LIBRARY_PATHS} NO_DEFAULT_PATH)
  if( WIN32 OR APPLE )
    find_library(INTELMKL_INTERFACE NAMES mkl_intel_c mkl_intel_lp64 PATHS ${TRIAL_LIBRARY_PATHS} NO_DEFAULT_PATH) # 32/64-bit
    find_library(INTELMKL_THREADING NAMES mkl_intel_thread           PATHS ${TRIAL_LIBRARY_PATHS} NO_DEFAULT_PATH)
  else()
    find_library(INTELMKL_INTERFACE NAMES mkl_gf mkl_gf_lp64 PATHS ${TRIAL_LIBRARY_PATHS} NO_DEFAULT_PATH) # 32/64-bit
    find_library(INTELMKL_THREADING NAMES mkl_gnu_thread     PATHS ${TRIAL_LIBRARY_PATHS} NO_DEFAULT_PATH)
  endif()
  list(APPEND INTELMKL_LIBRARIES ${INTELMKL_COMPUTATIONAL} ${INTELMKL_INTERFACE} ${INTELMKL_THREADING})

  # extra library dependencies for static linking
  if(NOT BUILD_SHARED_LIBS OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    find_library(INTELMKL_RTL NAMES iomp5 libiomp5mt PATHS ${TRIAL_LIBRARY_PATHS} NO_DEFAULT_PATH) # Linux/Windows
    list(APPEND INTELMKL_LIBRARIES ${INTELMKL_RTL})
    if(UNIX)
      find_library(PTHREAD pthread ${TRIAL_LIBRARY_PATHS})
      list(APPEND INTELMKL_LIBRARIES ${PTHREAD})
    endif()
  endif()


endif( NOT CF3_SKIP_INTELMKL )
# mark_as_advanced(INTELMKL_THREADING INTELMKL_INTERFACE INTELMKL_COMPUTATIONAL INTELMKL_RTL PTHREAD)

coolfluid_set_package(
  PACKAGE IntelMKL
  DESCRIPTION "Interface to Intel MKL iterative and direct solvers"
  URL "http://software.intel.com/en-us/intel-mkl"
  TYPE OPTIONAL
  VARS INTELMKL_LIBRARIES INTELMKL_INCLUDE_DIRS )

