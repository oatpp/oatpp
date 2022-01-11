
#include "oatpp/web/ClientRetryTest.hpp"
#include "oatpp/web/FullTest.hpp"
#include "oatpp/web/FullAsyncTest.hpp"
#include "oatpp/web/FullAsyncClientTest.hpp"
#include "oatpp/web/PipelineTest.hpp"
#include "oatpp/web/PipelineAsyncTest.hpp"
#include "oatpp/web/protocol/http/encoding/ChunkedTest.hpp"
#include "oatpp/web/server/api/ApiControllerTest.hpp"
#include "oatpp/web/server/handler/AuthorizationHandlerTest.hpp"
#include "oatpp/web/server/HttpRouterTest.hpp"
#include "oatpp/web/server/ServerStopTest.hpp"
#include "oatpp/web/mime/multipart/StatefulParserTest.hpp"

#include "oatpp/network/virtual_/PipeTest.hpp"
#include "oatpp/network/virtual_/InterfaceTest.hpp"
#include "oatpp/network/UrlTest.hpp"
#include "oatpp/network/ConnectionPoolTest.hpp"
#include "oatpp/network/monitor/ConnectionMonitorTest.hpp"

#include "oatpp/parser/json/mapping/DeserializerTest.hpp"
#include "oatpp/parser/json/mapping/DTOMapperPerfTest.hpp"
#include "oatpp/parser/json/mapping/DTOMapperTest.hpp"
#include "oatpp/parser/json/mapping/EnumTest.hpp"
#include "oatpp/parser/json/mapping/UnorderedSetTest.hpp"

#include "oatpp/encoding/UnicodeTest.hpp"
#include "oatpp/encoding/Base64Test.hpp"

#include "oatpp/core/parser/CaretTest.hpp"
#include "oatpp/core/provider/PoolTest.hpp"
#include "oatpp/core/provider/PoolTemplateTest.hpp"
#include "oatpp/core/async/LockTest.hpp"

#include "oatpp/core/data/mapping/type/UnorderedMapTest.hpp"
#include "oatpp/core/data/mapping/type/PairListTest.hpp"
#include "oatpp/core/data/mapping/type/VectorTest.hpp"
#include "oatpp/core/data/mapping/type/UnorderedSetTest.hpp"
#include "oatpp/core/data/mapping/type/ListTest.hpp"
#include "oatpp/core/data/mapping/type/ObjectTest.hpp"
#include "oatpp/core/data/mapping/type/StringTest.hpp"
#include "oatpp/core/data/mapping/type/PrimitiveTest.hpp"
#include "oatpp/core/data/mapping/type/ObjectWrapperTest.hpp"
#include "oatpp/core/data/mapping/type/TypeTest.hpp"
#include "oatpp/core/data/mapping/type/AnyTest.hpp"
#include "oatpp/core/data/mapping/type/EnumTest.hpp"
#include "oatpp/core/data/mapping/type/InterpretationTest.hpp"
#include "oatpp/core/data/mapping/TypeResolverTest.hpp"

#include "oatpp/core/data/resource/InMemoryDataTest.hpp"

#include "oatpp/core/data/stream/BufferStreamTest.hpp"
#include "oatpp/core/data/share/LazyStringMapTest.hpp"
#include "oatpp/core/data/share/StringTemplateTest.hpp"
#include "oatpp/core/data/share/MemoryLabelTest.hpp"
#include "oatpp/core/data/buffer/ProcessorTest.hpp"

#include "oatpp/core/base/CommandLineArgumentsTest.hpp"
#include "oatpp/core/base/LoggerTest.hpp"

#include "oatpp/core/async/Coroutine.hpp"
#include "oatpp/core/Types.hpp"

#include "oatpp/core/base/Environment.hpp"

#include <iostream>
#include <mutex>

namespace {

void runTests() {

  oatpp::base::Environment::printCompilationConfig();

  OATPP_LOGD("Tests", "coroutine size=%d", sizeof(oatpp::async::AbstractCoroutine));
  OATPP_LOGD("Tests", "action size=%d", sizeof(oatpp::async::Action));
  OATPP_LOGD("Tests", "class count=%d", oatpp::data::mapping::type::ClassId::getClassCount());

  auto names = oatpp::data::mapping::type::ClassId::getRegisteredClassNames();
  v_int32 i = 0;
  for(auto& name : names) {
    OATPP_LOGD("CLASS", "%d --> '%s'", i, name);
    i ++;
  }

  OATPP_RUN_TEST(oatpp::test::base::CommandLineArgumentsTest);
  OATPP_RUN_TEST(oatpp::test::base::LoggerTest);

  OATPP_RUN_TEST(oatpp::test::core::data::share::MemoryLabelTest);
  OATPP_RUN_TEST(oatpp::test::core::data::share::LazyStringMapTest);
  OATPP_RUN_TEST(oatpp::test::core::data::share::StringTemplateTest);

  OATPP_RUN_TEST(oatpp::test::core::data::buffer::ProcessorTest);

  OATPP_RUN_TEST(oatpp::test::core::data::stream::BufferStreamTest);

  OATPP_RUN_TEST(oatpp::test::core::data::mapping::type::ObjectWrapperTest);
  OATPP_RUN_TEST(oatpp::test::core::data::mapping::type::TypeTest);

  OATPP_RUN_TEST(oatpp::test::core::data::mapping::type::StringTest);

  OATPP_RUN_TEST(oatpp::test::core::data::mapping::type::PrimitiveTest);
  OATPP_RUN_TEST(oatpp::test::core::data::mapping::type::ListTest);
  OATPP_RUN_TEST(oatpp::test::core::data::mapping::type::VectorTest);
  OATPP_RUN_TEST(oatpp::test::core::data::mapping::type::UnorderedSetTest);
  OATPP_RUN_TEST(oatpp::test::core::data::mapping::type::PairListTest);
  OATPP_RUN_TEST(oatpp::test::core::data::mapping::type::UnorderedMapTest);
  OATPP_RUN_TEST(oatpp::test::core::data::mapping::type::AnyTest);
  OATPP_RUN_TEST(oatpp::test::core::data::mapping::type::EnumTest);

  OATPP_RUN_TEST(oatpp::test::core::data::mapping::type::ObjectTest);

  OATPP_RUN_TEST(oatpp::test::core::data::mapping::type::InterpretationTest);
  OATPP_RUN_TEST(oatpp::test::core::data::mapping::TypeResolverTest);

  OATPP_RUN_TEST(oatpp::test::core::data::resource::InMemoryDataTest);

  OATPP_RUN_TEST(oatpp::test::async::LockTest);
  OATPP_RUN_TEST(oatpp::test::parser::CaretTest);

  OATPP_RUN_TEST(oatpp::test::core::provider::PoolTest);
  OATPP_RUN_TEST(oatpp::test::core::provider::PoolTemplateTest);

  OATPP_RUN_TEST(oatpp::test::parser::json::mapping::EnumTest);

  OATPP_RUN_TEST(oatpp::test::parser::json::mapping::UnorderedSetTest);

  OATPP_RUN_TEST(oatpp::test::parser::json::mapping::DeserializerTest);
  OATPP_RUN_TEST(oatpp::test::parser::json::mapping::DTOMapperPerfTest);
  OATPP_RUN_TEST(oatpp::test::parser::json::mapping::DTOMapperTest);

  OATPP_RUN_TEST(oatpp::test::encoding::Base64Test);
  OATPP_RUN_TEST(oatpp::test::encoding::UnicodeTest);

  OATPP_RUN_TEST(oatpp::test::network::UrlTest);
  OATPP_RUN_TEST(oatpp::test::network::ConnectionPoolTest);
  OATPP_RUN_TEST(oatpp::test::network::monitor::ConnectionMonitorTest);
  OATPP_RUN_TEST(oatpp::test::network::virtual_::PipeTest);
  OATPP_RUN_TEST(oatpp::test::network::virtual_::InterfaceTest);

  OATPP_RUN_TEST(oatpp::test::web::protocol::http::encoding::ChunkedTest);

  OATPP_RUN_TEST(oatpp::test::web::mime::multipart::StatefulParserTest);

  OATPP_RUN_TEST(oatpp::test::web::server::HttpRouterTest);
  OATPP_RUN_TEST(oatpp::test::web::server::api::ApiControllerTest);
  OATPP_RUN_TEST(oatpp::test::web::server::handler::AuthorizationHandlerTest);

  {

    oatpp::test::web::server::ServerStopTest test_virtual(0);
    test_virtual.run();

    oatpp::test::web::server::ServerStopTest test_port(8000);
    test_port.run();

  }

  {

    oatpp::test::web::PipelineTest test_virtual(0, 3000);
    test_virtual.run();

    oatpp::test::web::PipelineTest test_port(8000, 3000);
    test_port.run();

  }

  {

    oatpp::test::web::PipelineAsyncTest test_virtual(0, 3000);
    test_virtual.run();

    oatpp::test::web::PipelineAsyncTest test_port(8000, 3000);
    test_port.run();

  }

  {

    oatpp::test::web::FullTest test_virtual(0, 1000);
    test_virtual.run();

    oatpp::test::web::FullTest test_port(8000, 5);
    test_port.run();

  }

  {

    oatpp::test::web::FullAsyncTest test_virtual(0, 1000);
    test_virtual.run();

    oatpp::test::web::FullAsyncTest test_port(8000, 5);
    test_port.run();

  }

  {

    oatpp::test::web::FullAsyncClientTest test_virtual(0, 1000);
    test_virtual.run(20);

    oatpp::test::web::FullAsyncClientTest test_port(8000, 5);
    test_port.run(1);

  }

  {

    oatpp::test::web::ClientRetryTest test_virtual(0);
    test_virtual.run();

    oatpp::test::web::ClientRetryTest test_port(8000);
    test_port.run();

  }

}

}

int main() {

  oatpp::base::Environment::init();

  runTests();

  /* Print how much objects were created during app running, and what have left-probably leaked */
  /* Disable object counting for release builds using '-D OATPP_DISABLE_ENV_OBJECT_COUNTERS' flag for better performance */
  std::cout << "\nEnvironment:\n";
  std::cout << "objectsCount = " << oatpp::base::Environment::getObjectsCount() << "\n";
  std::cout << "objectsCreated = " << oatpp::base::Environment::getObjectsCreated() << "\n\n";

  OATPP_ASSERT(oatpp::base::Environment::getObjectsCount() == 0);

  oatpp::base::Environment::destroy();

  return 0;
}

