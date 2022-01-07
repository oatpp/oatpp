
#######################################################################################################
## oatpp

add_library(oatpp
        oatpp/algorithm/CRC.cpp
        oatpp/algorithm/CRC.hpp
        oatpp/codegen/ApiClient_define.hpp
        oatpp/codegen/ApiClient_undef.hpp
        oatpp/codegen/api_controller/base_define.hpp
        oatpp/codegen/api_controller/base_undef.hpp
        oatpp/codegen/api_controller/auth_define.hpp
        oatpp/codegen/api_controller/auth_undef.hpp
        oatpp/codegen/api_controller/bundle_define.hpp
        oatpp/codegen/api_controller/bundle_undef.hpp
        oatpp/codegen/api_controller/cors_define.hpp
        oatpp/codegen/api_controller/cors_undef.hpp
        oatpp/codegen/ApiController_define.hpp
        oatpp/codegen/ApiController_undef.hpp
        oatpp/codegen/DbClient_define.hpp
        oatpp/codegen/DbClient_undef.hpp
        oatpp/codegen/dto/base_define.hpp
        oatpp/codegen/dto/base_undef.hpp
        oatpp/codegen/dto/enum_define.hpp
        oatpp/codegen/dto/enum_undef.hpp
        oatpp/codegen/DTO_define.hpp
        oatpp/codegen/DTO_undef.hpp
        oatpp/core/Types.hpp
        oatpp/core/async/utils/FastQueue.hpp
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
        oatpp/core/async/worker/IOEventWorker_common.cpp
        oatpp/core/async/worker/IOEventWorker_kqueue.cpp
        oatpp/core/async/worker/IOEventWorker_epoll.cpp
        oatpp/core/async/worker/IOEventWorker_stub.cpp
        oatpp/core/async/worker/IOEventWorker.hpp
        oatpp/core/async/worker/IOWorker.cpp
        oatpp/core/async/worker/IOWorker.hpp
        oatpp/core/async/worker/TimerWorker.cpp
        oatpp/core/async/worker/TimerWorker.hpp
        oatpp/core/base/CommandLineArguments.cpp
        oatpp/core/base/CommandLineArguments.hpp
        oatpp/core/base/Config.hpp
        oatpp/core/base/Countable.cpp
        oatpp/core/base/Countable.hpp
        oatpp/core/base/Environment.cpp
        oatpp/core/base/Environment.hpp
        oatpp/core/base/ObjectHandle.hpp
        oatpp/core/concurrency/SpinLock.cpp
        oatpp/core/concurrency/SpinLock.hpp
        oatpp/core/concurrency/Thread.cpp
        oatpp/core/concurrency/Thread.hpp
        oatpp/core/IODefinitions.cpp
        oatpp/core/IODefinitions.hpp
        oatpp/core/data/buffer/FIFOBuffer.cpp
        oatpp/core/data/buffer/FIFOBuffer.hpp
        oatpp/core/data/buffer/IOBuffer.cpp
        oatpp/core/data/buffer/IOBuffer.hpp
        oatpp/core/data/buffer/Processor.cpp
        oatpp/core/data/buffer/Processor.hpp
        oatpp/core/data/mapping/ObjectMapper.cpp
        oatpp/core/data/mapping/ObjectMapper.hpp
        oatpp/core/data/mapping/TypeResolver.cpp
        oatpp/core/data/mapping/TypeResolver.hpp
        oatpp/core/data/mapping/type/Any.cpp
        oatpp/core/data/mapping/type/Any.hpp
        oatpp/core/data/mapping/type/Collection.hpp
        oatpp/core/data/mapping/type/Enum.cpp
        oatpp/core/data/mapping/type/Enum.hpp
        oatpp/core/data/mapping/type/List.cpp
        oatpp/core/data/mapping/type/List.hpp
        oatpp/core/data/mapping/type/Map.hpp
        oatpp/core/data/mapping/type/PairList.cpp
        oatpp/core/data/mapping/type/PairList.hpp
        oatpp/core/data/mapping/type/Object.cpp
        oatpp/core/data/mapping/type/Object.hpp
        oatpp/core/data/mapping/type/Primitive.cpp
        oatpp/core/data/mapping/type/Primitive.hpp
        oatpp/core/data/mapping/type/Type.cpp
        oatpp/core/data/mapping/type/Type.hpp
        oatpp/core/data/mapping/type/UnorderedMap.cpp
        oatpp/core/data/mapping/type/UnorderedMap.hpp
        oatpp/core/data/mapping/type/UnorderedSet.cpp
        oatpp/core/data/mapping/type/UnorderedSet.hpp
        oatpp/core/data/mapping/type/Vector.cpp
        oatpp/core/data/mapping/type/Vector.hpp
        oatpp/core/data/resource/File.cpp
        oatpp/core/data/resource/File.hpp
        oatpp/core/data/resource/InMemoryData.cpp
        oatpp/core/data/resource/InMemoryData.hpp
        oatpp/core/data/resource/Resource.hpp
        oatpp/core/data/resource/TemporaryFile.cpp
        oatpp/core/data/resource/TemporaryFile.hpp
        oatpp/core/data/share/LazyStringMap.hpp
        oatpp/core/data/share/MemoryLabel.cpp
        oatpp/core/data/share/MemoryLabel.hpp
        oatpp/core/data/share/StringTemplate.cpp
        oatpp/core/data/share/StringTemplate.hpp
        oatpp/core/data/stream/BufferStream.cpp
        oatpp/core/data/stream/BufferStream.hpp
        oatpp/core/data/stream/FIFOStream.cpp
        oatpp/core/data/stream/FIFOStream.hpp
        oatpp/core/data/stream/FileStream.cpp
        oatpp/core/data/stream/FileStream.hpp
        oatpp/core/data/stream/Stream.cpp
        oatpp/core/data/stream/Stream.hpp
        oatpp/core/data/stream/StreamBufferedProxy.cpp
        oatpp/core/data/stream/StreamBufferedProxy.hpp
        oatpp/core/data/Bundle.cpp
        oatpp/core/data/Bundle.hpp
        oatpp/core/macro/basic.hpp
        oatpp/core/macro/codegen.hpp
        oatpp/core/macro/component.hpp
        oatpp/core/parser/Caret.cpp
        oatpp/core/parser/Caret.hpp
        oatpp/core/parser/ParsingError.cpp
        oatpp/core/parser/ParsingError.hpp
        oatpp/core/provider/Invalidator.hpp
        oatpp/core/provider/Pool.hpp
        oatpp/core/provider/Provider.hpp
        oatpp/core/utils/Binary.cpp
        oatpp/core/utils/Binary.hpp
        oatpp/core/utils/ConversionUtils.cpp
        oatpp/core/utils/ConversionUtils.hpp
        oatpp/core/utils/Random.cpp
        oatpp/core/utils/Random.hpp
        oatpp/core/utils/String.cpp
        oatpp/core/utils/String.hpp
        oatpp/encoding/Base64.cpp
        oatpp/encoding/Base64.hpp
        oatpp/encoding/Hex.cpp
        oatpp/encoding/Hex.hpp
        oatpp/encoding/Unicode.cpp
        oatpp/encoding/Unicode.hpp
        oatpp/network/monitor/ConnectionInactivityChecker.cpp
        oatpp/network/monitor/ConnectionInactivityChecker.hpp
        oatpp/network/monitor/ConnectionMaxAgeChecker.cpp
        oatpp/network/monitor/ConnectionMaxAgeChecker.hpp
        oatpp/network/monitor/ConnectionMonitor.cpp
        oatpp/network/monitor/ConnectionMonitor.hpp
        oatpp/network/monitor/MetricsChecker.hpp
        oatpp/network/monitor/StatCollector.hpp
        oatpp/network/tcp/client/ConnectionProvider.cpp
        oatpp/network/tcp/client/ConnectionProvider.hpp
        oatpp/network/tcp/server/ConnectionProvider.cpp
        oatpp/network/tcp/server/ConnectionProvider.hpp
        oatpp/network/tcp/Connection.cpp
        oatpp/network/tcp/Connection.hpp
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
        oatpp/network/Address.cpp
        oatpp/network/Address.hpp
        oatpp/network/ConnectionHandler.hpp
        oatpp/network/ConnectionPool.cpp
        oatpp/network/ConnectionPool.hpp
        oatpp/network/ConnectionProvider.cpp
        oatpp/network/ConnectionProvider.hpp
        oatpp/network/ConnectionProviderSwitch.cpp
        oatpp/network/ConnectionProviderSwitch.hpp
        oatpp/network/Server.cpp
        oatpp/network/Server.hpp
        oatpp/network/Url.cpp
        oatpp/network/Url.hpp
        oatpp/orm/Connection.hpp
        oatpp/orm/DbClient.cpp
        oatpp/orm/DbClient.hpp
        oatpp/orm/Executor.cpp
        oatpp/orm/Executor.hpp
        oatpp/orm/QueryResult.cpp
        oatpp/orm/QueryResult.hpp
        oatpp/orm/SchemaMigration.cpp
        oatpp/orm/SchemaMigration.hpp
        oatpp/orm/Transaction.cpp
        oatpp/orm/Transaction.hpp
        oatpp/parser/json/Beautifier.cpp
        oatpp/parser/json/Beautifier.hpp
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
        oatpp/web/client/RetryPolicy.cpp
        oatpp/web/client/RetryPolicy.hpp
        oatpp/web/mime/multipart/FileProvider.cpp
        oatpp/web/mime/multipart/FileProvider.hpp
        oatpp/web/mime/multipart/InMemoryDataProvider.cpp
        oatpp/web/mime/multipart/InMemoryDataProvider.hpp
        oatpp/web/mime/multipart/Multipart.cpp
        oatpp/web/mime/multipart/Multipart.hpp
        oatpp/web/mime/multipart/Part.cpp
        oatpp/web/mime/multipart/Part.hpp
        oatpp/web/mime/multipart/PartList.cpp
        oatpp/web/mime/multipart/PartList.hpp
        oatpp/web/mime/multipart/PartReader.cpp
        oatpp/web/mime/multipart/PartReader.hpp
        oatpp/web/mime/multipart/Reader.cpp
        oatpp/web/mime/multipart/Reader.hpp
        oatpp/web/mime/multipart/StatefulParser.cpp
        oatpp/web/mime/multipart/StatefulParser.hpp
        oatpp/web/mime/multipart/TemporaryFileProvider.cpp
        oatpp/web/mime/multipart/TemporaryFileProvider.hpp
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
        oatpp/web/protocol/http/outgoing/MultipartBody.cpp
        oatpp/web/protocol/http/outgoing/MultipartBody.hpp
        oatpp/web/protocol/http/outgoing/StreamingBody.cpp
        oatpp/web/protocol/http/outgoing/StreamingBody.hpp
        oatpp/web/protocol/http/outgoing/Request.cpp
        oatpp/web/protocol/http/outgoing/Request.hpp
        oatpp/web/protocol/http/outgoing/Response.cpp
        oatpp/web/protocol/http/outgoing/Response.hpp
        oatpp/web/protocol/http/outgoing/ResponseFactory.cpp
        oatpp/web/protocol/http/outgoing/ResponseFactory.hpp
        oatpp/web/protocol/http/encoding/Chunked.cpp
        oatpp/web/protocol/http/encoding/Chunked.hpp
        oatpp/web/protocol/http/encoding/ProviderCollection.cpp
        oatpp/web/protocol/http/encoding/ProviderCollection.hpp
        oatpp/web/protocol/http/encoding/EncoderProvider.hpp
        oatpp/web/protocol/http/utils/CommunicationUtils.cpp
        oatpp/web/protocol/http/utils/CommunicationUtils.hpp
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
        oatpp/web/server/handler/AuthorizationHandler.cpp
        oatpp/web/server/handler/AuthorizationHandler.hpp
        oatpp/web/server/handler/ErrorHandler.cpp
        oatpp/web/server/handler/ErrorHandler.hpp
        oatpp/web/server/interceptor/AllowCorsGlobal.cpp
        oatpp/web/server/interceptor/AllowCorsGlobal.hpp
        oatpp/web/server/interceptor/RequestInterceptor.hpp
        oatpp/web/server/interceptor/ResponseInterceptor.hpp
        oatpp/web/url/mapping/Pattern.cpp
        oatpp/web/url/mapping/Pattern.hpp
        oatpp/web/url/mapping/Router.hpp
)

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
