cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

###################################################################################################
## INCLUDES #######################################################################################
###################################################################################################

include(CheckCXXCompilerFlag)


###################################################################################################
## FUNCTIONS ######################################################################################
###################################################################################################

function(add_cxx_compiler_flags var)
  foreach(flag ${ARGN})
    string(REGEX REPLACE "[^a-zA-Z0-9]+" "_" flag_var "CXXFLAG_${flag}")
    check_cxx_compiler_flag("${flag}" ${flag_var})
    if(${flag_var})
      set(${var} "${${var}} ${flag}")
    endif()
  endforeach()
  set(${var} "${${var}}" PARENT_SCOPE)
endfunction()

function(add_compiler_flags)
  foreach(flag ${ARGN})
    add_cxx_compiler_flags(CMAKE_CXX_FLAGS ${flag})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" PARENT_SCOPE)
  endforeach()
endfunction()

function(remove_any_compiler_flags var)
  foreach(flag ${ARGN})
    string(REPLACE       "${flag}" ""  ${var} "${${var}}")
    string(REGEX REPLACE "[ \t]+"  " " ${var} "${${var}}")
  endforeach()
  set(${var} "${${var}}" PARENT_SCOPE)
endfunction()

function(remove_compiler_flags)
  foreach(flag ${ARGN})
    remove_any_compiler_flags(CMAKE_CXX_FLAGS "${flag}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" PARENT_SCOPE)
  endforeach()
endfunction()

function(save_compiler_flags)
  set(CMAKE_CXX_FLAGS_SAVED "${CMAKE_CXX_FLAGS}" CACHE STRING "Saved compiler C++ flags" FORCE)
endfunction()

function(restore_compiler_flags)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_SAVED}"       )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" PARENT_SCOPE)
endfunction()


###################################################################################################
## PROJECT ########################################################################################
###################################################################################################

file(STRINGS "${CMAKE_CURRENT_LIST_DIR}/src/oatpp/core/base/Environment.hpp" OATPP_VERSION_MACRO REGEX "#define OATPP_VERSION \"[0-9]+.[0-9]+.[0-9]+\"$")
string(REGEX REPLACE "#define OATPP_VERSION \"([0-9]+.[0-9]+.[0-9]+)\"$" "\\1" oatpp_VERSION "${OATPP_VERSION_MACRO}")

###################################################################################################
## These variables are passed to oatpp-module-install.cmake script
## use these variables to configure module installation

set(OATPP_THIS_MODULE_NAME oatpp) ## name of the module (also name of folders in installation dirs)
set(OATPP_THIS_MODULE_VERSION ${oatpp_VERSION}) ## version of the module (also sufix of folders in installation dirs)

###################################################################################################

project(oatpp VERSION ${OATPP_THIS_MODULE_VERSION} LANGUAGES CXX)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(OATPP_INSTALL "Create installation target for oat++" ON)
option(OATPP_BUILD_TESTS "Create test target for oat++" ON)
option(OATPP_LINK_TEST_LIBRARY "Link oat++ test library" ON)
option(OATPP_LINK_ATOMIC "Link atomic library for other platform than MSVC|MINGW|APPLE|FreeBSD" ON)
option(OATPP_MSVC_LINK_STATIC_RUNTIME "MSVC: Link with static runtime (/MT and /MTd)." OFF)

###################################################################################################
## COMPILATION CONFIG #############################################################################
###################################################################################################

if(OATPP_LINK_TEST_LIBRARY)
	set(OATPP_THIS_MODULE_LIBRARIES oatpp oatpp-test) ## list of libraries to find when find_package is called
	set(OATPP_THIS_MODULE_TARGETS oatpp oatpp-test) ## list of targets to install
	set(OATPP_THIS_MODULE_DIRECTORIES oatpp oatpp-test) ## list of directories to install
else()
	set(OATPP_THIS_MODULE_LIBRARIES oatpp) ## list of libraries to find when find_package is called
	set(OATPP_THIS_MODULE_TARGETS oatpp) ## list of targets to install
	set(OATPP_THIS_MODULE_DIRECTORIES oatpp) ## list of directories to install
endif()

option(OATPP_DISABLE_ENV_OBJECT_COUNTERS "Disable object counting for Release builds for better performance" OFF)
option(OATPP_DISABLE_POOL_ALLOCATIONS "This will make oatpp::base::memory::MemoryPool, method obtain and free call new and delete directly" OFF)

set(OATPP_THREAD_HARDWARE_CONCURRENCY "AUTO" CACHE STRING "Predefined value for function oatpp::concurrency::Thread::getHardwareConcurrency()")

option(OATPP_COMPAT_BUILD_NO_THREAD_LOCAL "Disable 'thread_local' feature" OFF)
option(OATPP_COMPAT_BUILD_NO_SET_AFFINITY "No 'pthread_setaffinity_np' method" OFF)

option(OATPP_DISABLE_LOGV "DISABLE logs priority V" OFF)
option(OATPP_DISABLE_LOGD "DISABLE logs priority D" OFF)
option(OATPP_DISABLE_LOGI "DISABLE logs priority I" OFF)
option(OATPP_DISABLE_LOGW "DISABLE logs priority W" OFF)
option(OATPP_DISABLE_LOGE "DISABLE logs priority E" OFF)

## Print config ##################################################################################

message("\n############################################################################")
message("## oatpp module compilation config:\n")

message("OATPP_DISABLE_ENV_OBJECT_COUNTERS=${OATPP_DISABLE_ENV_OBJECT_COUNTERS}")
message("OATPP_THREAD_HARDWARE_CONCURRENCY=${OATPP_THREAD_HARDWARE_CONCURRENCY}")
message("OATPP_COMPAT_BUILD_NO_THREAD_LOCAL=${OATPP_COMPAT_BUILD_NO_THREAD_LOCAL}")

## Set definitions ###############################################################################

if(OATPP_DISABLE_ENV_OBJECT_COUNTERS)
    add_definitions(-DOATPP_DISABLE_ENV_OBJECT_COUNTERS)
endif()

if(OATPP_DISABLE_POOL_ALLOCATIONS)
    add_definitions (-DOATPP_DISABLE_POOL_ALLOCATIONS)
    message("WARNING: OATPP_DISABLE_POOL_ALLOCATIONS option is deprecated and has no effect.")
endif()

set(AUTO_VALUE AUTO)
if(NOT OATPP_THREAD_HARDWARE_CONCURRENCY STREQUAL AUTO_VALUE)
    add_definitions (-DOATPP_THREAD_HARDWARE_CONCURRENCY=${OATPP_THREAD_HARDWARE_CONCURRENCY})
endif()

if(OATPP_THREAD_DISTRIBUTED_MEM_POOL_SHARDS_COUNT)
    add_definitions (-DOATPP_THREAD_DISTRIBUTED_MEM_POOL_SHARDS_COUNT=${OATPP_THREAD_DISTRIBUTED_MEM_POOL_SHARDS_COUNT})
    message("WARNING: OATPP_THREAD_DISTRIBUTED_MEM_POOL_SHARDS_COUNT option is deprecated and has no effect.")
endif()

if(OATPP_COMPAT_BUILD_NO_THREAD_LOCAL)
    add_definitions(-DOATPP_COMPAT_BUILD_NO_THREAD_LOCAL)
endif()

if(OATPP_COMPAT_BUILD_NO_SET_AFFINITY)
    add_definitions(-DOATPP_COMPAT_BUILD_NO_SET_AFFINITY)
endif()

if(OATPP_DISABLE_LOGV)
    add_definitions(-DOATPP_DISABLE_LOGV)
endif()

if(OATPP_DISABLE_LOGD)
    add_definitions(-DOATPP_DISABLE_LOGD)
endif()

if(OATPP_DISABLE_LOGI)
    add_definitions(-DOATPP_DISABLE_LOGI)
endif()

if(OATPP_DISABLE_LOGW)
    add_definitions(-DOATPP_DISABLE_LOGW)
endif()

if(OATPP_DISABLE_LOGE)
    add_definitions(-DOATPP_DISABLE_LOGE)
endif()

if(CMAKE_COMPILER_IS_GNUCXX AND ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 5.0)
    add_definitions(-DOATPP_DISABLE_STD_PUT_TIME)
endif()

message("\n############################################################################\n")

###################################################################################################

message("oatpp version: '${OATPP_THIS_MODULE_VERSION}'")

###################################################################################################
## COMPILER FLAGS #################################################################################
###################################################################################################

#
# Generic flags
#
add_compiler_flags("-Wall")
add_compiler_flags("-Wextra")
add_compiler_flags("-g3")

#
# C/C++ Warning Flags
# See https://github.com/vadz/gcc-warnings-tools for gcc flags and versions
#
add_compiler_flags("-Wcast-align")                                         # gcc 4.6.0
#add_compiler_flags("-Wconversion")                                         # gcc 4.6.0
#add_compiler_flags("-Wdouble-promotion")                                   # gcc 4.6.0
add_compiler_flags("-Wformat=2")                                           # gcc 4.6.0
add_compiler_flags("-Winvalid-pch")                                        # gcc 4.6.0
add_compiler_flags("-Wmissing-declarations")                               # gcc 4.6.0
add_compiler_flags("-Wmissing-format-attribute")                           # gcc 4.6.0
add_compiler_flags("-Wmissing-include-dirs")                               # gcc 4.6.0
add_compiler_flags("-Wpointer-arith")                                      # gcc 4.6.0
add_compiler_flags("-Wredundant-decls")                                    # gcc 4.6.0
#add_compiler_flags("-Wshadow")                                             # gcc 4.6.0
#add_compiler_flags("-Wsign-conversion")                                    # gcc 4.6.0
#add_compiler_flags("-Wsuggest-attribute=noreturn")                         # gcc 4.6.0
add_compiler_flags("-Wswitch-default")                                     # gcc 4.6.0
add_compiler_flags("-Wswitch-enum")                                        # gcc 4.6.0
add_compiler_flags("-Wtype-limits")                                         # gcc 4.6.0
add_compiler_flags("-Wundef")                                              # gcc 4.6.0
add_compiler_flags("-Wuninitialized")                                       # gcc 4.6.0
add_compiler_flags("-Wunknown-pragmas")                                     # gcc 4.6.0
add_compiler_flags("-Wunsafe-loop-optimizations")                           # gcc 4.6.0
add_compiler_flags("-Wunused-but-set-parameter")                            # gcc 4.6.0
add_compiler_flags("-Wunused-but-set-variable")                             # gcc 4.6.0
add_compiler_flags("-Wunused-function")                                     # gcc 4.6.0
add_compiler_flags("-Wunused")                                              # gcc 4.6.0
add_compiler_flags("-Wunused-label")                                        # gcc 4.6.0
add_compiler_flags("-Wunused-macros")                                       # gcc 4.6.0
#add_compiler_flags("-Wunused-parameter")                                    # gcc 4.6.0
add_compiler_flags("-Wunused-result")                                       # gcc 4.6.0
add_compiler_flags("-Wunused-value")                                        # gcc 4.6.0
#add_compiler_flags("-Wunused-variable")                                     # gcc 4.6.0

add_compiler_flags("-Wunused-local-typedefs")                               # gcc 4.7.0

#add_compiler_flags("-Wsuggest-attribute=format")                           # gcc 4.8.0

add_compiler_flags("-Wformat-signedness")                                  # gcc 5.1.0

add_compiler_flags("-Wduplicated-cond")                                    # gcc 6.1.0
#add_compiler_flags("-Wlogical-op")                                         # gcc 6.1.0
add_compiler_flags("-Wnull-dereference")                                   # gcc 6.1.0

add_compiler_flags("-Wduplicated-branches")                                # gcc 7.1.0
add_compiler_flags("-Wformat-overflow=2")                                  # gcc 7.1.0
add_compiler_flags("-Wformat-truncation=2")                                # gcc 7.1.0

#add_compiler_flags("-Wcast-align=strict")                                  # gcc 8.1.0
#add_compiler_flags("-Wsuggest-attribute=cold")                             # gcc 8.1.0
add_compiler_flags("-Wsuggest-attribute=malloc")                           # gcc 8.1.0

#add_compiler_flags("-Warith-conversion")                                   # gcc 10.1.0

add_compiler_flags("-Wctad-maybe-unsupported")                             # gcc 11.1.0
add_compiler_flags("-Wdeprecated-enum-enum-conversion")                    # gcc 11.1.0
add_compiler_flags("-Wdeprecated-enum-float-conversion")                   # gcc 11.1.0
add_compiler_flags("-Winvalid-imported-macros")                            # gcc 11.1.0
add_compiler_flags("-Wrange-loop-construct")                               # gcc 11.1.0

add_compiler_flags("-Warray-compare")                                      # gcc 12.1.0
add_compiler_flags("-Wbidi-chars=unpaired,ucn")                            # gcc 12.1.0
add_compiler_flags("-Winfinite-recursion")                                 # gcc 12.1.0
add_compiler_flags("-Wopenacc-parallelism")                                # gcc 12.1.0
add_compiler_flags("-ftrivial-auto-var-init=zero")                         # gcc 12.1.0
add_compiler_flags("-Wtrivial-auto-var-init")                              # gcc 12.1.0

#
# C++ Warning Flags
# See https://github.com/vadz/gcc-warnings-tools for gcc flags and versions
#
#add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wctor-dtor-privacy")              # gcc 4.6.0
add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wnoexcept")                       # gcc 4.6.0
#add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wold-style-cast")                 # gcc 4.6.0
add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Woverloaded-virtual")             # gcc 4.6.0
add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wsign-promo")                     # gcc 4.6.0
add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wstrict-null-sentinel")           # gcc 4.6.0

add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wzero-as-null-pointer-constant")  # gcc 4.7.0

#add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wuseless-cast")                   # gcc 4.8.0

add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wconditionally-supported")        # gcc 4.9.0

add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wsuggest-override")               # gcc 5.1.0

add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wnoexcept-type")                  # gcc 7.1.0

add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wextra-semi")                     # gcc 8.1.0

add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wcomma-subscript")                # gcc 10.1.0
add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wmismatched-tags")                # gcc 10.1.0
add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wredundant-tags")                 # gcc 10.1.0
add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-Wvolatile")                       # gcc 10.1.0

#
# Allow the linker to remove unused data and functions
#
add_compiler_flags("-fdata-sections")
add_compiler_flags("-ffunction-sections")
add_compiler_flags("-fno-common")
if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
  add_compiler_flags("-Wl,--gc-sections")
endif (CMAKE_CXX_COMPILER_ID MATCHES GNU)

#
# Hardening flags (see https://developers.redhat.com/blog/2018/03/21/compiler-and-linker-flags-gcc)
#
add_compiler_flags("-fasynchronous-unwind-tables")
add_compiler_flags("-fexceptions")
add_compiler_flags("-fstack-clash-protection")
add_compiler_flags("-fstack-protector-strong")
add_compiler_flags("-grecord-gcc-switches")
add_compiler_flags("-fcf-protection")
add_compiler_flags("-pipe")
add_compiler_flags("-Wformat=2 -Werror=format-security -Wno-format-nonliteral")
add_compiler_flags("-fPIE")
if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
  add_compiler_flags("-Wl,-z,defs")
  add_compiler_flags("-Wl,-z,now")
  add_compiler_flags("-Wl,-z,relro")
endif (CMAKE_CXX_COMPILER_ID MATCHES GNU)

#
# Disable some warnings
#
add_compiler_flags("-Wno-dangling-reference")
add_compiler_flags("-Wno-missing-field-initializers")
add_compiler_flags("-Wno-pessimizing-move")
add_compiler_flags("-Wno-sign-compare")
add_compiler_flags("-Wno-suggest-attribute=format")
add_compiler_flags("-Wno-unused-parameter")
add_compiler_flags("-Wno-unused-variable")

#
# Sanitize flags
#
#add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-fsanitize=address")
#add_cxx_compiler_flags(CMAKE_CXX_FLAGS "-fsanitize=thread")

include(cmake/msvc-runtime.cmake)
configure_msvc_runtime()

include(cmake/project.cmake)

add_subdirectory(src)

if(OATPP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()


include(cpack.cmake)

# This must always be last!
include( CPack )
