/***************************************************************************
 *
 * PoC for: Type confusion in ObjectWrapper::cast (#1102)
 *
 * Root cause: the type check in ObjectWrapper::cast() was inverted
 * (`Wrapper::Class::getType()->extends(m_valueType)` instead of
 * `m_valueType->extends(Wrapper::Class::getType())`), so a DOWN-cast
 * (base wrapper -> derived wrapper) passed the check and
 * std::static_pointer_cast reinterpreted the object, causing type confusion.
 * Additionally, the unconditional `Void` escape hatch allowed reinterpreting
 * a non-null, untyped (Void-typed) pointer as any wrapper type.
 *
 ***************************************************************************/

#include "oatpp/Types.hpp"
#include "oatpp/macro/codegen.hpp"
#include "oatpp/data/type/Any.hpp"

#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>

namespace oatpp { namespace data { namespace type {

namespace {

#include OATPP_CODEGEN_BEGIN(DTO)

class DtoBase : public oatpp::DTO {
  DTO_INIT(DtoBase, DTO)
  DTO_FIELD(Int32, baseField) = 1;
};

class DtoDerived : public DtoBase {
  DTO_INIT(DtoDerived, DtoBase)
  DTO_FIELD(String, derivedField) = "derived";
};

#include OATPP_CODEGEN_END(DTO)

int failures = 0;
#define CHECK(cond, msg) do { \
  if (cond) { printf("[PASS] %s\n", msg); } \
  else { printf("[FAIL] %s\n", msg); ++failures; } \
} while(0)

template<typename Wrapper, typename Value>
bool castThrows(const Value& v) {
  try {
    v.template cast<Wrapper>();
  } catch (const std::runtime_error&) {
    return true;
  }
  return false;
}

} // namespace

}}}

int main() {
  setvbuf(stdout, nullptr, _IONBF, 0); // keep evidence lines even if the process aborts
  using namespace oatpp::data::type;

  ///////////////////////////////////////////////////////////////////////////
  // 1. ATTACK (must throw): down-cast Object<DtoBase> -> Object<DtoDerived>.
  //    With the inverted check, DtoDerived->extends(DtoBase) was true, so the
  //    cast passed and accessing `derivedField` read memory beyond the object.
  ///////////////////////////////////////////////////////////////////////////
  {
    oatpp::Object<DtoBase> base = std::make_shared<DtoBase>();
    Void v = base; // erased, valueType == Object<DtoBase>
    CHECK((v.getValueType() == oatpp::Object<DtoBase>::Class::getType()), "fixture: stored type is Object<DtoBase>");
    CHECK(castThrows<oatpp::Object<DtoDerived>>(v), "down-cast Object<DtoBase>->Object<DtoDerived> is rejected");
  }

  ///////////////////////////////////////////////////////////////////////////
  // 2. ATTACK (must throw): cross-type cast String value -> Int32 wrapper.
  ///////////////////////////////////////////////////////////////////////////
  {
    Void v = oatpp::String("hello");
    CHECK((v.getValueType() == oatpp::String::Class::getType()), "fixture: stored type is String");
    CHECK(castThrows<oatpp::Int32>(v), "cross-cast String->Int32 is rejected");
  }

  ///////////////////////////////////////////////////////////////////////////
  // 3. ATTACK (must throw): non-null untyped Void (valueType == Void) must
  //    not be reinterpretable as a typed wrapper.
  ///////////////////////////////////////////////////////////////////////////
  {
    std::shared_ptr<void> raw = std::make_shared<std::string>("untyped bytes");
    Void v(raw); // ptr non-null, valueType == Void
    CHECK((v.getValueType() == Void::Class::getType()), "fixture: stored type is Void, ptr non-null");
    CHECK(castThrows<oatpp::Int32>(v), "cast from non-null untyped Void is rejected");
  }

  ///////////////////////////////////////////////////////////////////////////
  // 4. LEGACY (must keep working): null Void -> typed wrapper (null result).
  ///////////////////////////////////////////////////////////////////////////
  {
    Void v;
    auto i = v.cast<oatpp::Int32>();
    CHECK((i == nullptr), "cast null Void -> Int32 yields null Int32");
  }

  ///////////////////////////////////////////////////////////////////////////
  // 5. LEGACY (must keep working): type erasure and back (same type).
  ///////////////////////////////////////////////////////////////////////////
  {
    Void v = oatpp::String("test");
    CHECK((v.cast<oatpp::String>() == "test"), "erased String casts back to String");
    Void n = oatpp::Int32(32);
    CHECK((n.cast<oatpp::Int32>() == 32), "erased Int32 casts back to Int32");
  }

  ///////////////////////////////////////////////////////////////////////////
  // 6. LEGACY (must keep working): cast to Void (type erasure) stays allowed.
  ///////////////////////////////////////////////////////////////////////////
  {
    oatpp::String s = "erase me";
    auto v = s.cast<Void>();
    CHECK((v.get() != nullptr), "cast String -> Void is allowed (safe erasure)");
  }

  ///////////////////////////////////////////////////////////////////////////
  // 7. FIXED regression (must keep working): derived-to-base cast is valid.
  //    The old inverted check rejected this valid up-cast.
  ///////////////////////////////////////////////////////////////////////////
  {
    oatpp::Object<DtoDerived> d = std::make_shared<DtoDerived>();
    Void v = d; // valueType == Object<DtoDerived>
    auto b = v.cast<oatpp::Object<DtoBase>>(); // DtoDerived -> DtoBase (up-cast)
    CHECK((static_cast<bool>(b) && b->baseField == 1), "up-cast Object<DtoDerived>->Object<DtoBase> is accepted");
  }

  printf("\nRESULT: %s (%d failure(s))\n", failures == 0 ? "ALL CHECKS PASSED" : "FAILURES", failures);
  return failures == 0 ? 0 : 1;
}
