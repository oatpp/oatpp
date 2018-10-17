cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

file(STRINGS "${CMAKE_CURRENT_LIST_DIR}/core/base/Environment.hpp" OATPP_VERSION_MACRO REGEX "#define OATPP_VERSION \"[0-9]+.[0-9]+.[0-9]+\"$")
string(REGEX REPLACE "#define OATPP_VERSION \"([0-9]+.[0-9]+.[0-9]+)\"$" "\\1" oatpp_VERSION "${OATPP_VERSION_MACRO}")

project(oatpp VERSION ${oatpp_VERSION} LANGUAGES CXX)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(OATPP_INSTALL "Create installation target for oat++" ON)
option(OATPP_BUILD_TESTS "Create test target for oat++" ON)

add_library(oatpp 
    algorithm/CRC.cpp
    algorithm/CRC.hpp
    codegen/codegen_define_ApiClient_.hpp
    codegen/codegen_define_ApiController_.hpp
    codegen/codegen_define_DTO_.hpp
    codegen/codegen_undef_ApiClient_.hpp
    codegen/codegen_undef_ApiController_.hpp
    codegen/codegen_undef_DTO_.hpp
    core/async/Coroutine.cpp
    core/async/Coroutine.hpp
    core/async/Processor.cpp
    core/async/Processor.hpp
    core/base/Config.hpp
    core/base/Controllable.cpp
    core/base/Controllable.hpp
    core/base/Environment.cpp
    core/base/Environment.hpp
    core/base/memory/Allocator.cpp
    core/base/memory/Allocator.hpp
    core/base/memory/MemoryPool.cpp
    core/base/memory/MemoryPool.hpp
    core/base/memory/ObjectPool.cpp
    core/base/memory/ObjectPool.hpp
    core/base/StrBuffer.cpp
    core/base/StrBuffer.hpp
    core/collection/FastQueue.cpp
    core/collection/FastQueue.hpp
    core/collection/LinkedList.cpp
    core/collection/LinkedList.hpp
    core/collection/ListMap.cpp
    core/collection/ListMap.hpp
    core/concurrency/Runnable.cpp
    core/concurrency/Runnable.hpp
    core/concurrency/SpinLock.cpp
    core/concurrency/SpinLock.hpp
    core/concurrency/Thread.cpp
    core/concurrency/Thread.hpp
    core/data/buffer/FIFOBuffer.cpp
    core/data/buffer/FIFOBuffer.hpp
    core/data/buffer/IOBuffer.cpp
    core/data/buffer/IOBuffer.hpp
    core/data/mapping/ObjectMapper.cpp
    core/data/mapping/ObjectMapper.hpp
    core/data/mapping/type/List.cpp
    core/data/mapping/type/List.hpp
    core/data/mapping/type/ListMap.cpp
    core/data/mapping/type/ListMap.hpp
    core/data/mapping/type/Object.cpp
    core/data/mapping/type/Object.hpp
    core/data/mapping/type/Primitive.cpp
    core/data/mapping/type/Primitive.hpp
    core/data/mapping/type/Type.cpp
    core/data/mapping/type/Type.hpp
    core/data/stream/ChunkedBuffer.cpp
    core/data/stream/ChunkedBuffer.hpp
    core/data/stream/Delegate.cpp
    core/data/stream/Delegate.hpp
    core/data/stream/Stream.cpp
    core/data/stream/Stream.hpp
    core/data/stream/StreamBufferedProxy.cpp
    core/data/stream/StreamBufferedProxy.hpp
    core/macro/basic.hpp
    core/macro/codegen.hpp
    core/macro/component.hpp
    core/os/io/Library.cpp
    core/os/io/Library.hpp
    core/parser/ParsingCaret.cpp
    core/parser/ParsingCaret.hpp
    core/Types.cpp
    core/Types.hpp
    core/utils/ConversionUtils.cpp
    core/utils/ConversionUtils.hpp
    encoding/Base64.cpp
    encoding/Base64.hpp
    encoding/Hex.cpp
    encoding/Hex.hpp
    encoding/Unicode.cpp
    encoding/Unicode.hpp
    network/client/SimpleTCPConnectionProvider.cpp
    network/client/SimpleTCPConnectionProvider.hpp
    network/Connection.cpp
    network/Connection.hpp
    network/ConnectionProvider.cpp
    network/ConnectionProvider.hpp
    network/server/ConnectionHandler.cpp
    network/server/ConnectionHandler.hpp
    network/server/Server.cpp
    network/server/Server.hpp
    network/server/SimpleTCPConnectionProvider.cpp
    network/server/SimpleTCPConnectionProvider.hpp
    network/Url.cpp
    network/Url.hpp
    network/virtual_/Pipe.cpp
    network/virtual_/Pipe.hpp
    network/virtual_/Socket.cpp
    network/virtual_/Socket.hpp
    parser/json/mapping/Deserializer.cpp
    parser/json/mapping/Deserializer.hpp
    parser/json/mapping/ObjectMapper.cpp
    parser/json/mapping/ObjectMapper.hpp
    parser/json/mapping/Serializer.cpp
    parser/json/mapping/Serializer.hpp
    parser/json/Utils.cpp
    parser/json/Utils.hpp
    web/client/ApiClient.cpp
    web/client/ApiClient.hpp
    web/client/HttpRequestExecutor.cpp
    web/client/HttpRequestExecutor.hpp
    web/client/RequestExecutor.cpp
    web/client/RequestExecutor.hpp
    web/protocol/http/Http.cpp
    web/protocol/http/Http.hpp
    web/protocol/http/incoming/BodyDecoder.cpp
    web/protocol/http/incoming/BodyDecoder.hpp
    web/protocol/http/incoming/Request.cpp
    web/protocol/http/incoming/Request.hpp
    web/protocol/http/incoming/Response.cpp
    web/protocol/http/incoming/Response.hpp
    web/protocol/http/outgoing/Body.cpp
    web/protocol/http/outgoing/Body.hpp
    web/protocol/http/outgoing/BufferBody.cpp
    web/protocol/http/outgoing/BufferBody.hpp
    web/protocol/http/outgoing/ChunkedBufferBody.cpp
    web/protocol/http/outgoing/ChunkedBufferBody.hpp
    web/protocol/http/outgoing/CommunicationUtils.cpp
    web/protocol/http/outgoing/CommunicationUtils.hpp
    web/protocol/http/outgoing/DtoBody.cpp
    web/protocol/http/outgoing/DtoBody.hpp
    web/protocol/http/outgoing/Request.cpp
    web/protocol/http/outgoing/Request.hpp
    web/protocol/http/outgoing/Response.cpp
    web/protocol/http/outgoing/Response.hpp
    web/protocol/http/outgoing/ResponseFactory.cpp
    web/protocol/http/outgoing/ResponseFactory.hpp
    web/server/api/ApiController.cpp
    web/server/api/ApiController.hpp
    web/server/api/Endpoint.cpp
    web/server/api/Endpoint.hpp
    web/server/AsyncHttpConnectionHandler.cpp
    web/server/AsyncHttpConnectionHandler.hpp
    web/server/handler/ErrorHandler.cpp
    web/server/handler/ErrorHandler.hpp
    web/server/handler/Interceptor.cpp
    web/server/handler/Interceptor.hpp
    web/server/HttpConnectionHandler.cpp
    web/server/HttpConnectionHandler.hpp
    web/server/HttpError.cpp
    web/server/HttpError.hpp
    web/server/HttpProcessor.cpp
    web/server/HttpProcessor.hpp
    web/server/HttpRouter.cpp
    web/server/HttpRouter.hpp
    web/url/mapping/Pattern.cpp
    web/url/mapping/Pattern.hpp
    web/url/mapping/Router.cpp
    web/url/mapping/Router.hpp
    web/url/mapping/Subscriber.cpp
    web/url/mapping/Subscriber.hpp
)

set(CMAKE_THREAD_PREFER_PTHREAD ON)
find_package(Threads REQUIRED)
target_link_libraries(oatpp PUBLIC ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(oatpp PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
set_target_properties(oatpp PROPERTIES
    CXX_STANDARD 11
    CXX_EXTENSIONS OFF
    CXX_STANDARD_REQUIRED ON
)
add_library(oatpp::oatpp ALIAS oatpp)

if(OATPP_INSTALL)
    include(GNUInstallDirs)

    install(TARGETS oatpp
        EXPORT oatpp-targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
    install(DIRECTORY algorithm codegen core encoding network parser web
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/oatpp
        FILES_MATCHING PATTERN "*.hpp"
    )
    install(EXPORT oatpp-targets
        FILE oatppTargets.cmake
        NAMESPACE oatpp::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/oatpp
    )
    include(CMakePackageConfigHelpers)
    string(CONCAT oatppConfig_cmake_in
        "@PACKAGE_INIT@\n"
        "\n"
        "set_and_check(oatpp_INCLUDE_DIRS \"\$\{PACKAGE_PREFIX_DIR\}/include\")\n"
        "if(NOT TARGET oatpp::oatpp)\n"
        "    include(\"\$\{CMAKE_CURRENT_LIST_DIR\}/oatppTargets.cmake\")\n"
        "endif()\n"
        "set(oatpp_LIBRARIES \"oatpp::oatpp\")\n"
    )
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/oatppConfig.cmake.in" "${oatppConfig_cmake_in}")

    configure_package_config_file(${CMAKE_CURRENT_BINARY_DIR}/oatppConfig.cmake.in
        oatppConfig.cmake
        INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/oatpp"
        NO_CHECK_REQUIRED_COMPONENTS_MACRO
    )
    write_basic_package_version_file(oatppConfigVersion.cmake
        VERSION ${oatpp_VERSION}
        COMPATIBILITY SameMajorVersion
    )
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/oatppConfig.cmake" "${CMAKE_CURRENT_BINARY_DIR}/oatppConfigVersion.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/oatpp"
    )
endif()

if(OATPP_BUILD_TESTS)
    enable_testing()

    add_executable(oatppAllTests
        test/AllTestsMain.cpp
        test/Checker.cpp
        test/Checker.hpp
        test/UnitTest.cpp
        test/UnitTest.hpp
        test/core/base/collection/LinkedListTest.cpp
        test/core/base/collection/LinkedListTest.hpp
        test/core/base/memory/MemoryPoolTest.cpp
        test/core/base/memory/MemoryPoolTest.hpp
        test/core/base/memory/PerfTest.cpp
        test/core/base/memory/PerfTest.hpp
        test/core/base/RegRuleTest.cpp
        test/core/base/RegRuleTest.hpp
        test/core/data/mapping/type/TypeTest.cpp
        test/core/data/mapping/type/TypeTest.hpp
        test/encoding/UnicodeTest.cpp
        test/encoding/UnicodeTest.hpp
        test/encoding/Base64Test.cpp
        test/encoding/Base64Test.hpp
        test/parser/json/mapping/DeserializerTest.cpp
        test/parser/json/mapping/DeserializerTest.hpp
        test/parser/json/mapping/DTOMapperPerfTest.cpp
        test/parser/json/mapping/DTOMapperPerfTest.hpp
        test/parser/json/mapping/DTOMapperTest.cpp
        test/parser/json/mapping/DTOMapperTest.hpp
    )
    target_link_libraries(oatppAllTests PRIVATE oatpp)
    set_target_properties(oatppAllTests PROPERTIES
        CXX_STANDARD 11
        CXX_EXTENSIONS OFF
        CXX_STANDARD_REQUIRED ON
    )
    target_compile_definitions(oatppAllTests
        PRIVATE OATPP_ENABLE_ALL_TESTS_MAIN
    )
    add_test(oatppAllTests oatppAllTests)
endif()
