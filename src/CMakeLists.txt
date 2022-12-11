
#######################################################################################################
## oatpp

## collect all source files in folder 'oatpp' recursively (i.e. also including subfolders)
file(GLOB_RECURSE FILES_OATPP
    oatpp/*.cpp
    oatpp/*.hpp
)
## for the sace of a better overview/browsability in the IDE,
## organize source files in a hierarchy (reflecting the actual folder structure)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/oatpp" FILES ${FILES_OATPP})

add_library(oatpp ${FILES_OATPP})

set_target_properties(oatpp PROPERTIES
        CXX_STANDARD 11
        CXX_EXTENSIONS OFF
        CXX_STANDARD_REQUIRED ON
)
if (MSVC)
    target_compile_options(oatpp PRIVATE /permissive-)
endif()

set(CMAKE_THREAD_PREFER_PTHREAD ON)

find_package(Threads REQUIRED)

#######################################################################################################
## Link external libraries

SET(OATPP_ADD_LINK_LIBS "")

if(MSVC OR MINGW)
        SET(OATPP_ADD_LINK_LIBS wsock32 ws2_32)
elseif(NOT APPLE AND NOT ${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
        if(OATPP_LINK_ATOMIC)
                SET(OATPP_ADD_LINK_LIBS atomic)
        endif()
endif()

message("OATPP_ADD_LINK_LIBS=${OATPP_ADD_LINK_LIBS}")

target_link_libraries(oatpp PUBLIC ${CMAKE_THREAD_LIBS_INIT}
        ${OATPP_ADD_LINK_LIBS}
)

target_include_directories(oatpp PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

#######################################################################################################
## oatpp-test

if(OATPP_LINK_TEST_LIBRARY)
    file(GLOB_RECURSE FILES_OATPP_TEST
        oatpp-test/*.cpp
        oatpp-test/*.hpp
    )
    
    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/oatpp-test" FILES ${FILES_OATPP_TEST})

    add_library(oatpp-test ${FILES_OATPP_TEST})

	set_target_properties(oatpp-test PROPERTIES
			CXX_STANDARD 11
			CXX_EXTENSIONS OFF
			CXX_STANDARD_REQUIRED ON
	)
	if (MSVC)
		target_compile_options(oatpp-test PRIVATE /permissive-)
	endif()

	target_link_libraries(oatpp-test PUBLIC oatpp)

	target_include_directories(oatpp-test PUBLIC
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	)

	add_dependencies(oatpp-test oatpp)

endif()

#######################################################################################################
## Install targets

if(OATPP_INSTALL)
    include("../cmake/module-install.cmake")
endif()
