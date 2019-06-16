
#include "oatpp/web/FullTest.hpp"
#include "oatpp/web/FullAsyncTest.hpp"
#include "oatpp/web/FullAsyncClientTest.hpp"
#include "oatpp/web/server/api/ApiControllerTest.hpp"

#include "oatpp/network/virtual_/PipeTest.hpp"
#include "oatpp/network/virtual_/InterfaceTest.hpp"
#include "oatpp/network/UrlTest.hpp"

#include "oatpp/core/data/stream/ChunkedBufferTest.hpp"
#include "oatpp/core/data/share/MemoryLabelTest.hpp"

#include "oatpp/parser/json/mapping/DeserializerTest.hpp"
#include "oatpp/parser/json/mapping/DTOMapperPerfTest.hpp"
#include "oatpp/parser/json/mapping/DTOMapperTest.hpp"

#include "oatpp/encoding/UnicodeTest.hpp"
#include "oatpp/encoding/Base64Test.hpp"

#include "oatpp/core/async/LockTest.hpp"

#include "oatpp/core/parser/CaretTest.hpp"

#include "oatpp/core/data/mapping/type/TypeTest.hpp"
#include "oatpp/core/base/collection/LinkedListTest.hpp"
#include "oatpp/core/base/memory/MemoryPoolTest.hpp"
#include "oatpp/core/base/memory/PerfTest.hpp"
#include "oatpp/core/base/CommandLineArgumentsTest.hpp"
#include "oatpp/core/base/RegRuleTest.hpp"

#include "oatpp/core/async/Coroutine.hpp"
#include "oatpp/core/Types.hpp"

#include "oatpp/core/concurrency/SpinLock.hpp"
#include "oatpp/core/base/Environment.hpp"

#include <iostream>
#include <mutex>

#ifdef OATPP_ENABLE_ALL_TESTS_MAIN
namespace {

void runTests() {

  oatpp::base::Environment::printCompilationConfig();

  OATPP_RUN_TEST(oatpp::test::base::RegRuleTest);
  OATPP_RUN_TEST(oatpp::test::base::CommandLineArgumentsTest);

  OATPP_RUN_TEST(oatpp::test::memory::MemoryPoolTest);
  OATPP_RUN_TEST(oatpp::test::memory::PerfTest);

  OATPP_RUN_TEST(oatpp::test::collection::LinkedListTest);

  OATPP_RUN_TEST(oatpp::test::core::data::share::MemoryLabelTest);
  OATPP_RUN_TEST(oatpp::test::core::data::stream::ChunkedBufferTest);
  OATPP_RUN_TEST(oatpp::test::core::data::mapping::type::TypeTest);

  OATPP_RUN_TEST(oatpp::test::async::LockTest);

  OATPP_RUN_TEST(oatpp::test::parser::CaretTest);
  OATPP_RUN_TEST(oatpp::test::parser::json::mapping::DeserializerTest);
  OATPP_RUN_TEST(oatpp::test::parser::json::mapping::DTOMapperPerfTest);
  OATPP_RUN_TEST(oatpp::test::parser::json::mapping::DTOMapperTest);

  OATPP_RUN_TEST(oatpp::test::encoding::Base64Test);
  OATPP_RUN_TEST(oatpp::test::encoding::UnicodeTest);

  OATPP_RUN_TEST(oatpp::test::network::UrlTest);
  OATPP_RUN_TEST(oatpp::test::network::virtual_::PipeTest);
  OATPP_RUN_TEST(oatpp::test::network::virtual_::InterfaceTest);

  OATPP_RUN_TEST(oatpp::test::web::server::api::ApiControllerTest);

  {

    oatpp::test::web::FullTest test_virtual(0, 1000);
    test_virtual.run();

    oatpp::test::web::FullTest test_port(8000, 10);
    test_port.run();

  }

  {

    oatpp::test::web::FullAsyncTest test_virtual(0, 1000);
    test_virtual.run();

    oatpp::test::web::FullAsyncTest test_port(8000, 10);
    test_port.run();

  }

  {

    oatpp::test::web::FullAsyncClientTest test_virtual(0, 1000);
    test_virtual.run(20);

    oatpp::test::web::FullAsyncClientTest test_port(8000, 10);
    test_port.run(1);

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
#endif
