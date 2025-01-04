#include "oatpp/json/ObjectMapper.hpp"
#include "oatpp/macro/codegen.hpp"

typedef oatpp::utils::parser::Caret ParsingCaret;
typedef oatpp::json::Serializer Serializer;
typedef oatpp::json::Deserializer Deserializer;

#include OATPP_CODEGEN_BEGIN(DTO)

ENUM(TestEnum, v_int32,
  VALUE(A, 1, "a"),
  VALUE(B, 2, "b"),
  VALUE(C, 2, "c")
)

class Empty : public oatpp::DTO {
  DTO_INIT(Empty, DTO)
};

class Test1 : public oatpp::DTO {
  DTO_INIT(Test1, DTO)
  DTO_FIELD(Int8, i8);
  DTO_FIELD(UInt8, u8);
  DTO_FIELD(Int16, i16);
  DTO_FIELD(UInt16, u16);
  DTO_FIELD(Int32, i32);
  DTO_FIELD(UInt32, u32);
  DTO_FIELD(Int64, i64);
  DTO_FIELD(UInt64, u64);
  DTO_FIELD(Float32, f32);
  DTO_FIELD(Float64, f64);
  DTO_FIELD(Boolean, b);
  DTO_FIELD(String, s);
  DTO_FIELD(Enum<TestEnum>, e);
  DTO_FIELD(List<Float64>, l);
  DTO_FIELD(Vector<Int8>, v);
  DTO_FIELD(Fields<UInt32>, f);
  DTO_FIELD(UnorderedFields<UInt8>, uf);
  DTO_FIELD(UnorderedSet<Int64>, us);
  DTO_FIELD(Object<Empty>, obj, "o");
  DTO_FIELD(Any, a);
};

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  oatpp::String input(reinterpret_cast<const char*>(data), size);

  oatpp::json::ObjectMapper mapper;
  mapper.deserializerConfig().mapper.allowUnknownFields = size & 0x01;
  mapper.deserializerConfig().mapper.allowLexicalCasting = size & 0x02;
  mapper.deserializerConfig().mapper.useUnqualifiedFieldNames = size & 0x04;
  mapper.deserializerConfig().mapper.useUnqualifiedEnumNames = size & 0x08;

  try {
    const auto dto = mapper.readFromString<oatpp::Object<Test1>>(input);
    mapper.writeToString(dto);
  } catch(...) {}

  return 0;
}
