
add_executable(oatppAllTests
        oatpp/AllTestsMain.cpp
        oatpp/core/async/LockTest.cpp
        oatpp/core/async/LockTest.hpp
        oatpp/core/base/CommandLineArgumentsTest.cpp
        oatpp/core/base/CommandLineArgumentsTest.hpp
        oatpp/core/base/LoggerTest.cpp
        oatpp/core/base/LoggerTest.hpp
        oatpp/core/data/buffer/ProcessorTest.cpp
        oatpp/core/data/buffer/ProcessorTest.hpp
        oatpp/core/data/mapping/type/AnyTest.cpp
        oatpp/core/data/mapping/type/AnyTest.hpp
        oatpp/core/data/mapping/type/EnumTest.cpp
        oatpp/core/data/mapping/type/EnumTest.hpp
        oatpp/core/data/mapping/type/InterpretationTest.cpp
        oatpp/core/data/mapping/type/InterpretationTest.hpp
        oatpp/core/data/mapping/type/ListTest.cpp
        oatpp/core/data/mapping/type/ListTest.hpp
        oatpp/core/data/mapping/type/ObjectTest.cpp
        oatpp/core/data/mapping/type/ObjectTest.hpp
        oatpp/core/data/mapping/type/ObjectWrapperTest.cpp
        oatpp/core/data/mapping/type/ObjectWrapperTest.hpp
        oatpp/core/data/mapping/type/PairListTest.cpp
        oatpp/core/data/mapping/type/PairListTest.hpp
        oatpp/core/data/mapping/type/PrimitiveTest.cpp
        oatpp/core/data/mapping/type/PrimitiveTest.hpp
        oatpp/core/data/mapping/type/StringTest.cpp
        oatpp/core/data/mapping/type/StringTest.hpp
        oatpp/core/data/mapping/type/TypeTest.cpp
        oatpp/core/data/mapping/type/TypeTest.hpp
        oatpp/core/data/mapping/type/UnorderedMapTest.cpp
        oatpp/core/data/mapping/type/UnorderedMapTest.hpp
        oatpp/core/data/mapping/type/UnorderedSetTest.cpp
        oatpp/core/data/mapping/type/UnorderedSetTest.hpp
        oatpp/core/data/mapping/type/VectorTest.cpp
        oatpp/core/data/mapping/type/VectorTest.hpp
        oatpp/core/data/mapping/TypeResolverTest.cpp
        oatpp/core/data/mapping/TypeResolverTest.hpp
        oatpp/core/data/resource/InMemoryDataTest.cpp
        oatpp/core/data/resource/InMemoryDataTest.hpp
        oatpp/core/data/share/LazyStringMapTest.cpp
        oatpp/core/data/share/LazyStringMapTest.hpp
        oatpp/core/data/share/MemoryLabelTest.cpp
        oatpp/core/data/share/MemoryLabelTest.hpp
        oatpp/core/data/share/StringTemplateTest.cpp
        oatpp/core/data/share/StringTemplateTest.hpp
        oatpp/core/data/stream/BufferStreamTest.cpp
        oatpp/core/data/stream/BufferStreamTest.hpp
        oatpp/core/parser/CaretTest.cpp
        oatpp/core/parser/CaretTest.hpp
        oatpp/core/provider/PoolTest.cpp
        oatpp/core/provider/PoolTest.hpp
        oatpp/core/provider/PoolTemplateTest.cpp
        oatpp/core/provider/PoolTemplateTest.hpp
        oatpp/encoding/Base64Test.cpp
        oatpp/encoding/Base64Test.hpp
        oatpp/encoding/UnicodeTest.cpp
        oatpp/encoding/UnicodeTest.hpp
        oatpp/network/monitor/ConnectionMonitorTest.cpp
        oatpp/network/monitor/ConnectionMonitorTest.hpp
        oatpp/network/virtual_/InterfaceTest.cpp
        oatpp/network/virtual_/InterfaceTest.hpp
        oatpp/network/virtual_/PipeTest.cpp
        oatpp/network/virtual_/PipeTest.hpp
        oatpp/network/ConnectionPoolTest.cpp
        oatpp/network/ConnectionPoolTest.hpp
        oatpp/network/UrlTest.cpp
        oatpp/network/UrlTest.hpp
        oatpp/parser/json/mapping/DTOMapperPerfTest.cpp
        oatpp/parser/json/mapping/DTOMapperPerfTest.hpp
        oatpp/parser/json/mapping/DTOMapperTest.cpp
        oatpp/parser/json/mapping/DTOMapperTest.hpp
        oatpp/parser/json/mapping/DeserializerTest.cpp
        oatpp/parser/json/mapping/DeserializerTest.hpp
        oatpp/parser/json/mapping/EnumTest.cpp
        oatpp/parser/json/mapping/EnumTest.hpp
        oatpp/parser/json/mapping/UnorderedSetTest.cpp
        oatpp/parser/json/mapping/UnorderedSetTest.hpp
        oatpp/web/protocol/http/encoding/ChunkedTest.cpp
        oatpp/web/protocol/http/encoding/ChunkedTest.hpp
        oatpp/web/mime/multipart/StatefulParserTest.cpp
        oatpp/web/mime/multipart/StatefulParserTest.hpp
        oatpp/web/server/api/ApiControllerTest.cpp
        oatpp/web/server/api/ApiControllerTest.hpp
        oatpp/web/server/handler/AuthorizationHandlerTest.cpp
        oatpp/web/server/handler/AuthorizationHandlerTest.hpp
        oatpp/web/server/HttpRouterTest.cpp
        oatpp/web/server/HttpRouterTest.hpp
        oatpp/web/server/ServerStopTest.cpp
        oatpp/web/server/ServerStopTest.hpp
        oatpp/web/ClientRetryTest.cpp
        oatpp/web/ClientRetryTest.hpp
        oatpp/web/PipelineTest.cpp
        oatpp/web/PipelineTest.hpp
        oatpp/web/PipelineAsyncTest.cpp
        oatpp/web/PipelineAsyncTest.hpp
        oatpp/web/FullAsyncTest.cpp
        oatpp/web/FullAsyncTest.hpp
        oatpp/web/FullTest.cpp
        oatpp/web/FullTest.hpp
        oatpp/web/FullAsyncClientTest.cpp
        oatpp/web/FullAsyncClientTest.hpp
        oatpp/web/app/Client.hpp
        oatpp/web/app/BearerAuthorizationController.hpp
        oatpp/web/app/BasicAuthorizationController.hpp
        oatpp/web/app/Controller.hpp
        oatpp/web/app/ControllerAsync.hpp
        oatpp/web/app/DTOs.hpp
        oatpp/web/app/ControllerWithInterceptors.hpp
        oatpp/web/app/ControllerWithInterceptorsAsync.hpp
        oatpp/web/app/ControllerWithErrorHandler.hpp
)

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
