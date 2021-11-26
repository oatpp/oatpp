#######################################################################################
## Set module properties
## all oatpp modules should have the same installation procedure
##
## installation tree:
##
## prefix/
##  |
##  |- include/oatpp-<version>/<module-name>
##   - lib/
##       |
##       |- cmake/<module-name>-<version>/
##       |    |
##       |    |- <module-name>Config.cmake
##       |     - <module-name>ConfigVersion.cmake
##       |
##        - oatpp-<version>/
##            |
##            |- lib1.a
##            |- lib2.a
##             - ...
##
######################################################################################

message("\n############################################################################")
message("## oatpp-module-install.cmake\n")

message("OATPP_THIS_MODULE_NAME=${OATPP_THIS_MODULE_NAME}")
message("OATPP_THIS_MODULE_VERSION=${OATPP_THIS_MODULE_VERSION}")
message("OATPP_THIS_MODULE_LIBRARIES=${OATPP_THIS_MODULE_LIBRARIES}")
message("OATPP_THIS_MODULE_TARGETS=${OATPP_THIS_MODULE_TARGETS}")
message("OATPP_THIS_MODULE_DIRECTORIES=${OATPP_THIS_MODULE_DIRECTORIES}")

message("\n############################################################################\n")

#######################################################################################
## Set cache variables to configure module-config.cmake.in template
## via call to configure_package_config_file

include(GNUInstallDirs)

set(OATPP_MODULE_NAME ${OATPP_THIS_MODULE_NAME} CACHE STRING "oatpp module name")
set(OATPP_MODULE_VERSION "${OATPP_THIS_MODULE_VERSION}" CACHE STRING "oatpp module version")
set(OATPP_MODULE_LIBRARIES
        "${OATPP_THIS_MODULE_LIBRARIES}" ## list libraries to find when find_package is called
        CACHE INTERNAL "oatpp module libraries"
)
set(OATPP_MODULE_LIBDIR ${CMAKE_INSTALL_LIBDIR} CACHE STRING "lib folder name")

#######################################################################################
## calc directories to install (relative to this script)
## dirs should be in ( relative ../src/<dirs>)

foreach(CURR_DIR ${OATPP_THIS_MODULE_DIRECTORIES})
    list(APPEND OATPP_DIRS_TO_INSTALL ${CMAKE_CURRENT_LIST_DIR}/../src/${CURR_DIR})
endforeach()

#######################################################################################

install(
    TARGETS ${OATPP_THIS_MODULE_TARGETS}
    EXPORT "${OATPP_MODULE_NAME}Targets"
    ARCHIVE
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/oatpp-${OATPP_MODULE_VERSION}"
        COMPONENT Devel
    LIBRARY
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/oatpp-${OATPP_MODULE_VERSION}"
        COMPONENT Library
    RUNTIME
        DESTINATION "${CMAKE_INSTALL_BINDIR}/oatpp-${OATPP_MODULE_VERSION}"
        COMPONENT Library
    INCLUDES
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/oatpp-${OATPP_MODULE_VERSION}/${OATPP_MODULE_NAME}"
)

install(DIRECTORY ${OATPP_DIRS_TO_INSTALL}
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/oatpp-${OATPP_MODULE_VERSION}/${OATPP_MODULE_NAME}"
        COMPONENT Devel
        FILES_MATCHING PATTERN "*.hpp"
)

install(EXPORT "${OATPP_MODULE_NAME}Targets"
        FILE "${OATPP_MODULE_NAME}Targets.cmake"
        NAMESPACE oatpp::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${OATPP_MODULE_NAME}-${OATPP_MODULE_VERSION}"
        COMPONENT Devel
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file("${OATPP_MODULE_NAME}ConfigVersion.cmake"
        VERSION ${OATPP_MODULE_VERSION}
        COMPATIBILITY ExactVersion ## Use exact version matching.
)

## Take module-config.cmake.in file in this direcory as a template

configure_package_config_file(
            "${CMAKE_CURRENT_LIST_DIR}/module-config.cmake.in"
            "${OATPP_MODULE_NAME}Config.cmake"
        INSTALL_DESTINATION
            "${CMAKE_INSTALL_LIBDIR}/cmake/${OATPP_MODULE_NAME}-${OATPP_MODULE_VERSION}"
        PATH_VARS
            OATPP_MODULE_NAME
            OATPP_MODULE_VERSION
            OATPP_MODULE_LIBRARIES
            OATPP_MODULE_LIBDIR
        NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

install(
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/${OATPP_MODULE_NAME}Config.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/${OATPP_MODULE_NAME}ConfigVersion.cmake"
        DESTINATION
            "${CMAKE_INSTALL_LIBDIR}/cmake/${OATPP_MODULE_NAME}-${OATPP_MODULE_VERSION}"
        COMPONENT Devel
)
