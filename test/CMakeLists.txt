## collect all source files in folder 'oatpp' recursively (i.e. also including subfolders)
file(GLOB_RECURSE FILES_OATPP_ALL_TESTS
    oatpp/*.cpp
    oatpp/*.hpp
)
## for the sace of a better overview/browsability in the IDE,
## organize source files in a hierarchy (reflecting the actual folder structure)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/oatpp" FILES ${FILES_OATPP_ALL_TESTS})

add_executable(oatppAllTests ${FILES_OATPP_ALL_TESTS})

target_link_libraries(oatppAllTests PRIVATE oatpp PRIVATE oatpp-test)

set_target_properties(oatppAllTests PROPERTIES
    CXX_STANDARD 11
    CXX_EXTENSIONS OFF
    CXX_STANDARD_REQUIRED ON
)
if (MSVC)
    target_compile_options(oatppAllTests PRIVATE /permissive-)
endif()

target_include_directories(oatppAllTests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_test(oatppAllTests oatppAllTests)
