#include "oatpp/json/ObjectMapper.hpp"
#include "oatpp/macro/codegen.hpp"

typedef oatpp::utils::parser::Caret ParsingCaret;
typedef oatpp::json::Serializer Serializer;
typedef oatpp::json::Deserializer Deserializer;

#include OATPP_CODEGEN_BEGIN(DTO)

class EmptyDto : public oatpp::DTO {
  DTO_INIT(EmptyDto, DTO)
};

class Test1 : public oatpp::DTO {
  DTO_INIT(Test1, DTO)
  DTO_FIELD(String, strF);
};

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  oatpp::String input(reinterpret_cast<const char*>(data), size);
  oatpp::json::ObjectMapper mapper;
  try {
    mapper.readFromString<oatpp::Object<Test1>>(input);
  } catch(...) {}

  return 0;
}
