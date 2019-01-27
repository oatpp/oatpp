cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

file(STRINGS "${CMAKE_CURRENT_LIST_DIR}/src/oatpp/core/base/Environment.hpp" OATPP_VERSION_MACRO REGEX "#define OATPP_VERSION \"[0-9]+.[0-9]+.[0-9]+\"$")
string(REGEX REPLACE "#define OATPP_VERSION \"([0-9]+.[0-9]+.[0-9]+)\"$" "\\1" oatpp_VERSION "${OATPP_VERSION_MACRO}")

###################################################################################################
## These variables are passed to oatpp-module-install.cmake script
## use these variables to configure module installation

set(OATPP_THIS_MODULE_NAME oatpp) ## name of the module (also name of folders in installation dirs)
set(OATPP_THIS_MODULE_VERSION ${oatpp_VERSION}) ## version of the module (also sufix of folders in installation dirs)
set(OATPP_THIS_MODULE_LIBRARIES oatpp oatpp-test) ## list of libraries to find when find_package is called
set(OATPP_THIS_MODULE_TARGETS oatpp oatpp-test) ## list of targets to install
set(OATPP_THIS_MODULE_DIRECTORIES oatpp oatpp-test) ## list of directories to install

###################################################################################################

project(oatpp VERSION ${OATPP_THIS_MODULE_VERSION} LANGUAGES CXX)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(OATPP_INSTALL "Create installation target for oat++" ON)
option(OATPP_BUILD_TESTS "Create test target for oat++" ON)

message("oatpp version: '${OATPP_THIS_MODULE_VERSION}'")

add_subdirectory(src)

if(OATPP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
