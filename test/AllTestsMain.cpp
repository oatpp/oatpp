#include "oatpp/test/core/base/collection/LinkedListTest.hpp"
#include "oatpp/test/core/base/memory/MemoryPoolTest.hpp"
#include "oatpp/test/core/base/memory/PerfTest.hpp"
#include "oatpp/test/encoding/UnicodeTest.hpp"
#include "oatpp/test/parser/json/mapping/DeserializerTest.hpp"
#include "oatpp/test/parser/json/mapping/DTOMapperPerfTest.hpp"
#include "oatpp/test/parser/json/mapping/DTOMapperTest.hpp"

#ifdef OATPP_ENABLE_ALL_TESTS_MAIN
int main() {
  oatpp::base::Environment::init();
  OATPP_RUN_TEST(oatpp::test::collection::LinkedListTest);
  OATPP_RUN_TEST(oatpp::test::memory::MemoryPoolTest);
  OATPP_RUN_TEST(oatpp::test::memory::PerfTest);
  OATPP_RUN_TEST(oatpp::test::encoding::UnicodeTest);
  OATPP_RUN_TEST(oatpp::test::parser::json::mapping::DeserializerTest);
  OATPP_RUN_TEST(oatpp::test::parser::json::mapping::DTOMapperPerfTest);
  OATPP_RUN_TEST(oatpp::test::parser::json::mapping::DTOMapperTest);
  oatpp::base::Environment::destroy();
  return 0;
}
#endif
