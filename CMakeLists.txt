cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

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

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")

include(cmake/msvc-runtime.cmake)
configure_msvc_runtime()

add_subdirectory(src)

if(OATPP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()


include(cpack.cmake)

# This must always be last!
include( CPack )
