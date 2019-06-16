
#######################################################################################################
## oatpp

add_library(oatpp
        oatpp/algorithm/CRC.cpp
        oatpp/algorithm/CRC.hpp
        oatpp/codegen/codegen_define_ApiClient_.hpp
        oatpp/codegen/codegen_define_ApiController_.hpp
        oatpp/codegen/codegen_define_DTO_.hpp
        oatpp/codegen/codegen_undef_ApiClient_.hpp
        oatpp/codegen/codegen_undef_ApiController_.hpp
        oatpp/codegen/codegen_undef_DTO_.hpp
        oatpp/core/Types.cpp
        oatpp/core/Types.hpp
        oatpp/core/async/Coroutine.cpp
        oatpp/core/async/Coroutine.hpp
        oatpp/core/async/CoroutineWaitList.cpp
        oatpp/core/async/CoroutineWaitList.hpp
        oatpp/core/async/Error.cpp
        oatpp/core/async/Error.hpp
        oatpp/core/async/Executor.cpp
        oatpp/core/async/Executor.hpp
        oatpp/core/async/Lock.cpp
        oatpp/core/async/Lock.hpp
        oatpp/core/async/Processor.cpp
        oatpp/core/async/Processor.hpp
        oatpp/core/async/worker/Worker.cpp
        oatpp/core/async/worker/Worker.hpp
        oatpp/core/async/worker/IOWorker.cpp
        oatpp/core/async/worker/IOWorker.hpp
        oatpp/core/async/worker/IOEventWorker_common.cpp
        oatpp/core/async/worker/IOEventWorker_kqueue.cpp
        oatpp/core/async/worker/IOEventWorker_epoll.cpp
        oatpp/core/async/worker/IOEventWorker.hpp
        oatpp/core/async/worker/TimerWorker.cpp
        oatpp/core/async/worker/TimerWorker.hpp
        oatpp/core/base/CommandLineArguments.cpp
        oatpp/core/base/CommandLineArguments.hpp
        oatpp/core/base/Config.hpp
        oatpp/core/base/Countable.cpp
        oatpp/core/base/Countable.hpp
        oatpp/core/base/Environment.cpp
        oatpp/core/base/Environment.hpp
        oatpp/core/base/StrBuffer.cpp
        oatpp/core/base/StrBuffer.hpp
        oatpp/core/base/memory/Allocator.cpp
        oatpp/core/base/memory/Allocator.hpp
        oatpp/core/base/memory/MemoryPool.cpp
        oatpp/core/base/memory/MemoryPool.hpp
        oatpp/core/base/memory/ObjectPool.cpp
        oatpp/core/base/memory/ObjectPool.hpp
        oatpp/core/collection/FastQueue.cpp
        oatpp/core/collection/FastQueue.hpp
        oatpp/core/collection/LinkedList.cpp
        oatpp/core/collection/LinkedList.hpp
        oatpp/core/collection/ListMap.cpp
        oatpp/core/collection/ListMap.hpp
        oatpp/core/concurrency/SpinLock.cpp
        oatpp/core/concurrency/SpinLock.hpp
        oatpp/core/concurrency/Thread.cpp
        oatpp/core/concurrency/Thread.hpp
        oatpp/core/data/IODefinitions.cpp
        oatpp/core/data/IODefinitions.hpp
        oatpp/core/data/buffer/FIFOBuffer.cpp
        oatpp/core/data/buffer/FIFOBuffer.hpp
        oatpp/core/data/buffer/IOBuffer.cpp
        oatpp/core/data/buffer/IOBuffer.hpp
        oatpp/core/data/mapping/ObjectMapper.cpp
        oatpp/core/data/mapping/ObjectMapper.hpp
        oatpp/core/data/mapping/type/List.cpp
        oatpp/core/data/mapping/type/List.hpp
        oatpp/core/data/mapping/type/ListMap.cpp
        oatpp/core/data/mapping/type/ListMap.hpp
        oatpp/core/data/mapping/type/Object.cpp
        oatpp/core/data/mapping/type/Object.hpp
        oatpp/core/data/mapping/type/Primitive.cpp
        oatpp/core/data/mapping/type/Primitive.hpp
        oatpp/core/data/mapping/type/Type.cpp
        oatpp/core/data/mapping/type/Type.hpp
        oatpp/core/data/share/MemoryLabel.cpp
        oatpp/core/data/share/MemoryLabel.hpp
        oatpp/core/data/stream/ChunkedBuffer.cpp
        oatpp/core/data/stream/ChunkedBuffer.hpp
        oatpp/core/data/stream/Delegate.cpp
        oatpp/core/data/stream/Delegate.hpp
        oatpp/core/data/stream/Stream.cpp
        oatpp/core/data/stream/Stream.hpp
        oatpp/core/data/stream/StreamBufferedProxy.cpp
        oatpp/core/data/stream/StreamBufferedProxy.hpp
        oatpp/core/macro/basic.hpp
        oatpp/core/macro/codegen.hpp
        oatpp/core/macro/component.hpp
        oatpp/core/parser/Caret.cpp
        oatpp/core/parser/Caret.hpp
        oatpp/core/parser/ParsingError.cpp
        oatpp/core/parser/ParsingError.hpp
        oatpp/core/utils/ConversionUtils.cpp
        oatpp/core/utils/ConversionUtils.hpp
        oatpp/encoding/Base64.cpp
        oatpp/encoding/Base64.hpp
        oatpp/encoding/Hex.cpp
        oatpp/encoding/Hex.hpp
        oatpp/encoding/Unicode.cpp
        oatpp/encoding/Unicode.hpp
        oatpp/network/Connection.cpp
        oatpp/network/Connection.hpp
        oatpp/network/ConnectionProvider.cpp
        oatpp/network/ConnectionProvider.hpp
        oatpp/network/Url.cpp
        oatpp/network/Url.hpp
        oatpp/network/client/SimpleTCPConnectionProvider.cpp
        oatpp/network/client/SimpleTCPConnectionProvider.hpp
        oatpp/network/server/ConnectionHandler.cpp
        oatpp/network/server/ConnectionHandler.hpp
        oatpp/network/server/Server.cpp
        oatpp/network/server/Server.hpp
        oatpp/network/server/SimpleTCPConnectionProvider.cpp
        oatpp/network/server/SimpleTCPConnectionProvider.hpp
        oatpp/network/virtual_/Interface.cpp
        oatpp/network/virtual_/Interface.hpp
        oatpp/network/virtual_/Pipe.cpp
        oatpp/network/virtual_/Pipe.hpp
        oatpp/network/virtual_/Socket.cpp
        oatpp/network/virtual_/Socket.hpp
        oatpp/network/virtual_/client/ConnectionProvider.cpp
        oatpp/network/virtual_/client/ConnectionProvider.hpp
        oatpp/network/virtual_/server/ConnectionProvider.cpp
        oatpp/network/virtual_/server/ConnectionProvider.hpp
        oatpp/parser/json/Utils.cpp
        oatpp/parser/json/Utils.hpp
        oatpp/parser/json/mapping/Deserializer.cpp
        oatpp/parser/json/mapping/Deserializer.hpp
        oatpp/parser/json/mapping/ObjectMapper.cpp
        oatpp/parser/json/mapping/ObjectMapper.hpp
        oatpp/parser/json/mapping/Serializer.cpp
        oatpp/parser/json/mapping/Serializer.hpp
        oatpp/web/client/ApiClient.cpp
        oatpp/web/client/ApiClient.hpp
        oatpp/web/client/HttpRequestExecutor.cpp
        oatpp/web/client/HttpRequestExecutor.hpp
        oatpp/web/client/RequestExecutor.cpp
        oatpp/web/client/RequestExecutor.hpp
        oatpp/web/protocol/CommunicationError.cpp
        oatpp/web/protocol/CommunicationError.hpp
        oatpp/web/protocol/http/Http.cpp
        oatpp/web/protocol/http/Http.hpp
        oatpp/web/protocol/http/incoming/BodyDecoder.cpp
        oatpp/web/protocol/http/incoming/BodyDecoder.hpp
        oatpp/web/protocol/http/incoming/Request.cpp
        oatpp/web/protocol/http/incoming/Request.hpp
        oatpp/web/protocol/http/incoming/RequestHeadersReader.cpp
        oatpp/web/protocol/http/incoming/RequestHeadersReader.hpp
        oatpp/web/protocol/http/incoming/Response.cpp
        oatpp/web/protocol/http/incoming/Response.hpp
        oatpp/web/protocol/http/incoming/ResponseHeadersReader.cpp
        oatpp/web/protocol/http/incoming/ResponseHeadersReader.hpp
        oatpp/web/protocol/http/incoming/SimpleBodyDecoder.cpp
        oatpp/web/protocol/http/incoming/SimpleBodyDecoder.hpp
        oatpp/web/protocol/http/outgoing/Body.cpp
        oatpp/web/protocol/http/outgoing/Body.hpp
        oatpp/web/protocol/http/outgoing/BufferBody.cpp
        oatpp/web/protocol/http/outgoing/BufferBody.hpp
        oatpp/web/protocol/http/outgoing/ChunkedBufferBody.cpp
        oatpp/web/protocol/http/outgoing/ChunkedBufferBody.hpp
        oatpp/web/protocol/http/outgoing/CommunicationUtils.cpp
        oatpp/web/protocol/http/outgoing/CommunicationUtils.hpp
        oatpp/web/protocol/http/outgoing/DtoBody.cpp
        oatpp/web/protocol/http/outgoing/DtoBody.hpp
        oatpp/web/protocol/http/outgoing/Request.cpp
        oatpp/web/protocol/http/outgoing/Request.hpp
        oatpp/web/protocol/http/outgoing/Response.cpp
        oatpp/web/protocol/http/outgoing/Response.hpp
        oatpp/web/protocol/http/outgoing/ResponseFactory.cpp
        oatpp/web/protocol/http/outgoing/ResponseFactory.hpp
        oatpp/web/server/AsyncHttpConnectionHandler.cpp
        oatpp/web/server/AsyncHttpConnectionHandler.hpp
        oatpp/web/server/HttpConnectionHandler.cpp
        oatpp/web/server/HttpConnectionHandler.hpp
        oatpp/web/server/HttpProcessor.cpp
        oatpp/web/server/HttpProcessor.hpp
        oatpp/web/server/HttpRequestHandler.hpp
        oatpp/web/server/HttpRouter.cpp
        oatpp/web/server/HttpRouter.hpp
        oatpp/web/server/api/ApiController.cpp
        oatpp/web/server/api/ApiController.hpp
        oatpp/web/server/api/Endpoint.cpp
        oatpp/web/server/api/Endpoint.hpp
        oatpp/web/server/handler/ErrorHandler.cpp
        oatpp/web/server/handler/ErrorHandler.hpp
        oatpp/web/server/handler/Interceptor.cpp
        oatpp/web/server/handler/Interceptor.hpp
        oatpp/web/url/mapping/Pattern.cpp
        oatpp/web/url/mapping/Pattern.hpp
        oatpp/web/url/mapping/Router.hpp
)

set_target_properties(oatpp PROPERTIES
        CXX_STANDARD 11
        CXX_EXTENSIONS OFF
        CXX_STANDARD_REQUIRED ON
)

set(CMAKE_THREAD_PREFER_PTHREAD ON)

find_package(Threads REQUIRED)

target_link_libraries(oatpp PUBLIC ${CMAKE_THREAD_LIBS_INIT})

target_include_directories(oatpp PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

#######################################################################################################
## oatpp-test

add_library(oatpp-test
        oatpp-test/Checker.cpp
        oatpp-test/Checker.hpp
        oatpp-test/UnitTest.cpp
        oatpp-test/UnitTest.hpp
        oatpp-test/web/ClientServerTestRunner.hpp
)

set_target_properties(oatpp-test PROPERTIES
        CXX_STANDARD 11
        CXX_EXTENSIONS OFF
        CXX_STANDARD_REQUIRED ON
)

target_link_libraries(oatpp-test PUBLIC oatpp)

target_include_directories(oatpp-test PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

add_dependencies(oatpp-test oatpp)

#######################################################################################################
## install targets

if(OATPP_INSTALL)
    include("../cmake/module-install.cmake")
endif()
