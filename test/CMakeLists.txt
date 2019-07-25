
add_executable(oatppAllTests
        oatpp/AllTestsMain.cpp
        oatpp/core/async/LockTest.cpp
        oatpp/core/async/LockTest.hpp
        oatpp/core/base/CommandLineArgumentsTest.cpp
        oatpp/core/base/CommandLineArgumentsTest.hpp
        oatpp/core/base/RegRuleTest.cpp
        oatpp/core/base/RegRuleTest.hpp
        oatpp/core/base/collection/LinkedListTest.cpp
        oatpp/core/base/collection/LinkedListTest.hpp
        oatpp/core/base/memory/MemoryPoolTest.cpp
        oatpp/core/base/memory/MemoryPoolTest.hpp
        oatpp/core/base/memory/PerfTest.cpp
        oatpp/core/base/memory/PerfTest.hpp
        oatpp/core/data/mapping/type/TypeTest.cpp
        oatpp/core/data/mapping/type/TypeTest.hpp
        oatpp/core/data/share/MemoryLabelTest.cpp
        oatpp/core/data/share/MemoryLabelTest.hpp
        oatpp/core/data/stream/ChunkedBufferTest.cpp
        oatpp/core/data/stream/ChunkedBufferTest.hpp
        oatpp/core/parser/CaretTest.cpp
        oatpp/core/parser/CaretTest.hpp
        oatpp/encoding/Base64Test.cpp
        oatpp/encoding/Base64Test.hpp
        oatpp/encoding/UnicodeTest.cpp
        oatpp/encoding/UnicodeTest.hpp
        oatpp/network/UrlTest.cpp
        oatpp/network/UrlTest.hpp
        oatpp/network/virtual_/InterfaceTest.cpp
        oatpp/network/virtual_/InterfaceTest.hpp
        oatpp/network/virtual_/PipeTest.cpp
        oatpp/network/virtual_/PipeTest.hpp
        oatpp/parser/json/mapping/DTOMapperPerfTest.cpp
        oatpp/parser/json/mapping/DTOMapperPerfTest.hpp
        oatpp/parser/json/mapping/DTOMapperTest.cpp
        oatpp/parser/json/mapping/DTOMapperTest.hpp
        oatpp/parser/json/mapping/DeserializerTest.cpp
        oatpp/parser/json/mapping/DeserializerTest.hpp
        oatpp/web/mime/multipart/StatefulParserTest.cpp
        oatpp/web/mime/multipart/StatefulParserTest.hpp
        oatpp/web/server/api/ApiControllerTest.cpp
        oatpp/web/server/api/ApiControllerTest.hpp
        oatpp/web/FullAsyncTest.cpp
        oatpp/web/FullAsyncTest.hpp
        oatpp/web/FullTest.cpp
        oatpp/web/FullTest.hpp
        oatpp/web/app/Client.hpp
        oatpp/web/app/Controller.hpp
        oatpp/web/app/ControllerAsync.hpp
        oatpp/web/app/DTOs.hpp
        oatpp/web/FullAsyncClientTest.cpp
        oatpp/web/FullAsyncClientTest.hpp
)

target_link_libraries(oatppAllTests PRIVATE oatpp PRIVATE oatpp-test)

set_target_properties(oatppAllTests PROPERTIES
    CXX_STANDARD 11
    CXX_EXTENSIONS OFF
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(oatppAllTests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(oatppAllTests
    PRIVATE OATPP_ENABLE_ALL_TESTS_MAIN
)
add_test(oatppAllTests oatppAllTests)
