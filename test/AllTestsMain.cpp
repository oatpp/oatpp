#include "oatpp/test/core/base/collection/LinkedListTest.hpp"
#include "oatpp/test/core/base/memory/MemoryPoolTest.hpp"
#include "oatpp/test/core/base/memory/PerfTest.hpp"
#include "oatpp/test/encoding/UnicodeTest.hpp"
#include "oatpp/test/parser/json/mapping/DeserializerTest.hpp"
#include "oatpp/test/parser/json/mapping/DTOMapperPerfTest.hpp"
#include "oatpp/test/parser/json/mapping/DTOMapperTest.hpp"

#include "oatpp/core/concurrency/SpinLock.hpp"
#include "oatpp/core/base/Environment.hpp"

#include <iostream>

namespace {

class Logger : public oatpp::base::Logger {
private:
  oatpp::concurrency::SpinLock::Atom m_atom;
public:
  
  Logger()
  : m_atom(false)
  {}
  
  void log(v_int32 priority, const std::string& tag, const std::string& message) override {
    oatpp::concurrency::SpinLock lock(m_atom);
    std::cout << tag << ":" << message << "\n";
  }
  
};

void runTests() {
  OATPP_RUN_TEST(oatpp::test::collection::LinkedListTest);
  OATPP_RUN_TEST(oatpp::test::memory::MemoryPoolTest);
  OATPP_RUN_TEST(oatpp::test::memory::PerfTest);
  OATPP_RUN_TEST(oatpp::test::encoding::UnicodeTest);
  OATPP_RUN_TEST(oatpp::test::parser::json::mapping::DeserializerTest);
  OATPP_RUN_TEST(oatpp::test::parser::json::mapping::DTOMapperPerfTest);
  OATPP_RUN_TEST(oatpp::test::parser::json::mapping::DTOMapperTest);
}
  
}

#ifdef OATPP_ENABLE_ALL_TESTS_MAIN
int main() {
  
  oatpp::base::Environment::init();
  oatpp::base::Environment::setLogger(new Logger());
  
  runTests();
  
  oatpp::base::Environment::setLogger(nullptr);
  oatpp::base::Environment::destroy();
  
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
