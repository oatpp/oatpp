/***************************************************************************
 *
 * PoC for: OOB write in `BaseObject::Property::set` (issue #1104)
 *
 * A `Property` stores a field offset but (before the fix) not the class it
 * was declared on. Applying a wide DTO's last property to a small,
 * unrelated DTO object wrote past the end of the heap allocation.
 *
 * After the fix each property stores its owner type and
 * `set`/`get`/`getAsRef` verify the object's runtime type (or one of its
 * base types) against the owner, throwing `std::runtime_error` instead of
 * corrupting memory.
 *
 ***************************************************************************/

#include "oatpp/Types.hpp"
#include "oatpp/macro/codegen.hpp"

#include <cstdio>
#include <stdexcept>

namespace {

#include OATPP_CODEGEN_BEGIN(DTO)

class Tiny : public oatpp::DTO {
  DTO_INIT(Tiny, DTO)
  DTO_FIELD(oatpp::String, t0);
};

class Wide : public oatpp::DTO {
  DTO_INIT(Wide, DTO)
  DTO_FIELD(oatpp::String, w0);
  DTO_FIELD(oatpp::String, w1);
  DTO_FIELD(oatpp::String, w2);
  DTO_FIELD(oatpp::String, w3);
  DTO_FIELD(oatpp::String, w4);
  DTO_FIELD(oatpp::String, w5);
};

class Base : public oatpp::DTO {
  DTO_INIT(Base, DTO)
  DTO_FIELD(oatpp::String, b0);
};

class Derived : public Base {
  DTO_INIT(Derived, Base)
  DTO_FIELD(oatpp::String, d0);
};

#include OATPP_CODEGEN_END(DTO)

}

static int failures = 0;
#define CHECK(cond, msg) do { \
  if (cond) { printf("[PASS] %s\n", msg); } \
  else { printf("[FAIL] %s\n", msg); ++failures; } \
} while(0)

int main() {

  printf("sizeof(Tiny)=%zu sizeof(Wide)=%zu\n", sizeof(Tiny), sizeof(Wide));

  // === PoC: apply Wide::w5 (last field) to a Tiny object - must be rejected ===
  {
    auto tiny = Tiny::createShared();
    auto w5 = oatpp::Object<Wide>::getPropertiesMap().at("w5");

    bool thrown = false;
    try {
      printf("applying Wide::w5 to a Tiny object ...\n");
      w5->set(tiny.get(),
              oatpp::Void(oatpp::String("x").getPtr(), oatpp::String::Class::getType()));
    } catch (const std::runtime_error& e) {
      thrown = true;
      printf("[*] caught: %s\n", e.what());
    }
    CHECK(thrown, "Property::set of unrelated type throws std::runtime_error");
    CHECK(tiny->t0 == nullptr, "Tiny object untouched (no OOB write, no corruption)");
  }

  // === get / getAsRef must be guarded the same way ===
  {
    auto tiny = Tiny::createShared();
    auto w5 = oatpp::Object<Wide>::getPropertiesMap().at("w5");

    bool thrownGet = false;
    try { w5->get(tiny.get()); }
    catch (const std::runtime_error&) { thrownGet = true; }
    CHECK(thrownGet, "Property::get of unrelated type throws");

    bool thrownRef = false;
    try { (void)w5->getAsRef(tiny.get()); }
    catch (const std::runtime_error&) { thrownRef = true; }
    CHECK(thrownRef, "Property::getAsRef of unrelated type throws");
  }

  // === legit usages must keep working: own property ===
  {
    auto wide = Wide::createShared();
    auto w5 = oatpp::Object<Wide>::getPropertiesMap().at("w5");
    w5->set(wide.get(),
            oatpp::Void(oatpp::String("x").getPtr(), oatpp::String::Class::getType()));
    CHECK(wide->w5 == oatpp::String("x"), "set own property works");

    auto value = w5->get(wide.get());
    CHECK(value.getValueType() == oatpp::String::Class::getType(), "get own property works");

    w5->getAsRef(wide.get()) = oatpp::String("y");
    CHECK(wide->w5 == oatpp::String("y"), "getAsRef own property works");
  }

  // === legit: base-declared property applied to a derived object ===
  {
    auto derived = Derived::createShared();
    auto b0 = oatpp::Object<Derived>::getPropertiesMap().at("b0"); // declared on Base
    b0->set(derived.get(),
            oatpp::Void(oatpp::String("base").getPtr(), oatpp::String::Class::getType()));
    CHECK(derived->b0 == oatpp::String("base"), "base-declared property on derived object works");
  }

  // === legit: DTOWrapper::operator[] goes through the same guarded path ===
  {
    auto wide = Wide::createShared();
    wide["w3"] = oatpp::String("via-operator");
    CHECK(wide->w3 == oatpp::String("via-operator"), "operator[] still works");
  }

  // === stress: repeated guard hits and legit accesses interleaved ===
  {
    auto tiny = Tiny::createShared();
    auto w5 = oatpp::Object<Wide>::getPropertiesMap().at("w5");
    int rejected = 0;
    for(int i = 0; i < 1000; i++) {
      try {
        w5->set(tiny.get(),
                oatpp::Void(oatpp::String("x").getPtr(), oatpp::String::Class::getType()));
      } catch (const std::runtime_error&) {
        ++rejected;
      }
      tiny->t0 = oatpp::String("ok");
    }
    CHECK(rejected == 1000, "stress: 1000/1000 cross-type accesses rejected");
    CHECK(tiny->t0 == oatpp::String("ok"), "stress: legit field access unaffected");
  }

  printf(failures == 0 ? "\nALL CHECKS PASSED\n" : "\n%d CHECK(S) FAILED\n", failures);
  return failures == 0 ? 0 : 1;
}
