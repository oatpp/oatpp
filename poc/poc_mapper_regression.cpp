// Regression: mapper round-trip must be unaffected by Property owner validation
#include "oatpp/json/ObjectMapper.hpp"
#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"
#include <cstdio>
#include <thread>
#include <vector>
#include <atomic>

namespace {
#include OATPP_CODEGEN_BEGIN(DTO)

class DtoTypeA : public oatpp::DTO {
  DTO_INIT(DtoTypeA, DTO)
  DTO_FIELD(oatpp::String, fieldA) = "type-A";
};

class DtoTypeB : public oatpp::DTO {
  DTO_INIT(DtoTypeB, DTO)
  DTO_FIELD(oatpp::String, fieldB) = "type-B";
};

class PolymorphicDto : public oatpp::DTO {
  DTO_INIT(PolymorphicDto, DTO)
  DTO_FIELD(oatpp::String, type);
  DTO_FIELD(oatpp::Any, polymorph);
  DTO_FIELD_TYPE_SELECTOR(polymorph) {
    if(type == "A") return Object<DtoTypeA>::Class::getType();
    if(type == "B") return Object<DtoTypeB>::Class::getType();
    return Object<DTO>::Class::getType();
  }
};

class NestedDto : public oatpp::DTO {
  DTO_INIT(NestedDto, DTO)
  DTO_FIELD(oatpp::String, name);
  DTO_FIELD(oatpp::Int32, count);
};

class ContainerDto : public oatpp::DTO {
  DTO_INIT(ContainerDto, DTO)
  DTO_FIELD(oatpp::String, id);
  DTO_FIELD(oatpp::List<oatpp::Object<NestedDto>>, items);
  DTO_FIELD(oatpp::Fields<oatpp::String>, labels);
  DTO_FIELD(oatpp::String, note, "note-qualified") = "q";
};

class ChildDto : public NestedDto {
  DTO_INIT(ChildDto, NestedDto)
  DTO_FIELD(oatpp::String, extra);
};

#include OATPP_CODEGEN_END(DTO)
}

static int failures = 0;
#define CHECK(cond, msg) do { \
  if (cond) { printf("[PASS] %s\n", msg); } \
  else { printf("[FAIL] %s\n", msg); ++failures; } \
} while(0)

int main() {

  oatpp::json::ObjectMapper mapper;

  // 1. plain DTO round-trip
  {
    auto dto = ContainerDto::createShared();
    dto->id = "c-1";
    dto->items = oatpp::List<oatpp::Object<NestedDto>>::createShared();
    auto item1 = NestedDto::createShared();
    item1->name = "n1";
    item1->count = oatpp::Int32(1);
    dto->items->push_back(item1);
    auto item2 = NestedDto::createShared();
    item2->name = "n2";
    item2->count = oatpp::Int32(2);
    dto->items->push_back(item2);
    dto->labels = oatpp::Fields<oatpp::String>::createShared();
    dto->labels->push_back({"k", "v"});
    dto->note = oatpp::String("n");

    auto json = mapper.writeToString(dto);
    auto clone = mapper.readFromString<oatpp::Object<ContainerDto>>(json);
    auto json2 = mapper.writeToString(clone);

    CHECK(json == json2, "plain DTO json round-trip identical");
    CHECK(clone->id == oatpp::String("c-1"), "field 'id' survived");
    CHECK(clone->items->size() == 2, "list size survived");
    CHECK(clone->items[0]->name == oatpp::String("n1"), "nested dto field survived");
    CHECK(clone->note == oatpp::String("n"), "qualified-name field survived (FIELD_2 macro)");
    CHECK(json->find("note-qualified") != std::string::npos, "qualified field serialized under qualifier name");
  }

  // 2. polymorphic (Any + type selector) round-trip
  {
    auto dto = PolymorphicDto::createShared();
    dto->type = "A";
    dto->polymorph = DtoTypeA::createShared();

    auto json = mapper.writeToString(dto);
    auto clone = mapper.readFromString<oatpp::Object<PolymorphicDto>>(json);
    auto json2 = mapper.writeToString(clone);

    CHECK(json == json2, "polymorphic DTO json round-trip identical");
    CHECK(clone->polymorph, "Any field restored");

    auto retrieved = clone->polymorph.retrieve<oatpp::Object<DtoTypeA>>();
    CHECK(retrieved && retrieved->fieldA == oatpp::String("type-A"), "Any field content restored");
  }

  // 3. inherited-DTO round-trip (base-declared property via mapper)
  {
    auto dto = ChildDto::createShared();
    dto->name = "base-field";
    dto->count = oatpp::Int32(7);
    dto->extra = "child-field";

    auto json = mapper.writeToString(dto);
    auto clone = mapper.readFromString<oatpp::Object<ChildDto>>(json);
    auto json2 = mapper.writeToString(clone);

    CHECK(json == json2, "inherited DTO json round-trip identical");
    CHECK(clone->name == oatpp::String("base-field"), "base-declared field survived");
    CHECK(clone->extra == oatpp::String("child-field"), "own field survived");
  }

  // 4. cross-type property access via mapper-resolved property must throw, not corrupt
  {
    auto a = DtoTypeA::createShared();
    auto propB = oatpp::Object<DtoTypeB>::getPropertiesMap().at("fieldB");
    bool thrown = false;
    try {
      propB->set(a.get(), oatpp::String("evil"));
    } catch (const std::runtime_error&) {
      thrown = true;
    }
    CHECK(thrown, "cross-type set still rejected in mapper-dep module");
  }

  // 5. concurrent createShared stress (property singleton + owner type init in parallel)
  {
    std::vector<std::thread> threads;
    std::atomic<int> errs(0);
    for(int t = 0; t < 16; t++) {
      threads.emplace_back([&]{
        for(int i = 0; i < 200; i++) {
          auto a = DtoTypeA::createShared();
          auto c = ChildDto::createShared();
          auto p = PolymorphicDto::createShared();
          p->type = "B";
          p->polymorph = DtoTypeB::createShared();
          (void)a; (void)c; (void)p;
        }
      });
    }
    for(auto& t : threads) t.join();
    CHECK(true, "concurrent DTO init stress completed");
  }

  printf(failures == 0 ? "\nALL CHECKS PASSED\n" : "\n%d CHECK(S) FAILED\n", failures);
  return failures == 0 ? 0 : 1;
}
