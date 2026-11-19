/***************************************************************************
 *
 * Fuzz harness: dynamic JSON tree deserialization + JSON beautifier.
 *
 * Threat model: API clients deserialize attacker-controlled JSON into
 * dynamic values (oatpp::Any / data::mapping::Tree) when no DTO type is
 * known ahead of time - the existing fuzz_mapper target only covers the
 * fixed-DTO path. The beautifier decorates serialized JSON output whose
 * string content is attacker data echoed back in responses.
 *
 * Exercised entry points (dark in the 20261117 report):
 *   - json::Deserializer into a dynamic tree (data/type/Tree.cpp 4/106,
 *     data/mapping/Tree.cpp dark remainder beyond the DTO path)
 *   - serializer of dynamic values incl. optional beautifier pass
 *     (json/Beautifier.cpp 0/107)
 *   - json::Beautifier::write driven directly with arbitrary bytes
 *     (string-content state machine: escapes, nesting, indenting)
 *
 * Expected bug classes: memory safety in scanner/escape handling, deep
 * nesting blowups (recursion depth), numeric parsing edge cases.
 ***************************************************************************/

#include "oatpp/json/ObjectMapper.hpp"
#include "oatpp/json/Beautifier.hpp"
#include "oatpp/data/stream/BufferStream.hpp"

#include <cstddef>

namespace {

void fuzzBeautifier(const uint8_t* data, std::size_t size) {
  oatpp::data::stream::BufferOutputStream outStream;
  oatpp::json::Beautifier beautifier(&outStream, "  ", "\n");
  oatpp::async::Action action;
  beautifier.write(data, static_cast<v_buff_size>(size), action);
}

} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, std::size_t size) {

  if (size == 0) {
    return 0;
  }

  try {

    oatpp::String text(reinterpret_cast<const char*>(data), size);

    {
      /* dynamic (DTO-less) deserialization into Any-backed tree */
      oatpp::json::ObjectMapper mapper;
      mapper.deserializerConfig().mapper.allowUnknownFields = size & 0x01;
      mapper.deserializerConfig().mapper.allowLexicalCasting = size & 0x02;
      mapper.deserializerConfig().mapper.useUnqualifiedEnumNames = size & 0x08;
      mapper.serializerConfig().json.useBeautifier = (size & 0x04) != 0;

      oatpp::utils::parser::Caret caret(text);
      auto value = mapper.readFromCaret<oatpp::Any>(caret);
      if (value) {
        /* re-serialize the dynamic value (optionally through the beautifier) */
        mapper.writeToString(value);
      }
    }

    fuzzBeautifier(data, size);

  } catch (...) {
    /* malformed JSON is reported via exceptions/ErrorStack; memory-safety
     * faults still surface via ASan */
  }

  return 0;

}
