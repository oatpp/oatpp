/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ***************************************************************************/

#include "ObjectTest.hpp"

#include "oatpp/parser/json/mapping/ObjectMapper.hpp"

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include "oatpp-test/Checker.hpp"

#include <thread>

namespace oatpp { namespace test { namespace core { namespace data { namespace mapping { namespace  type {

namespace {

#include OATPP_CODEGEN_BEGIN(DTO)

class Dto0 : public oatpp::DTO {
  DTO_INIT(Dto0, DTO)
};

class DtoA : public oatpp::DTO {

  DTO_INIT(DtoA, DTO)

  DTO_FIELD_INFO(id) {
    info->description = "identifier";
    info->pattern = "^[a-z0-9]+$";
  }
  DTO_FIELD(String, id) = "Some default id";

  DTO_HC_EQ(id)

public:

  DtoA() = default;

  DtoA(const String& pId)
    : id(pId)
  {}

};

class DtoB : public DtoA {

  DTO_INIT(DtoB, DtoA)

  DTO_FIELD_INFO(a) {
    info->description = "some field with a qualified name";
  }
  DTO_FIELD(String, a, "field-a") = "default-value";

};

class DtoC : public DtoA {

  DTO_INIT(DtoC, DtoA)

  DTO_FIELD(String, a);
  DTO_FIELD(String, b);
  DTO_FIELD(String, c);

  DTO_HC_EQ(a, b, c);

};

class DtoD : public DtoA {

  DTO_INIT(DtoD, DtoA)

  DTO_FIELD(Int32, a) = Int64(64);

};

class DtoTypeA : public oatpp::DTO {

  DTO_INIT(DtoTypeA, DTO)

  DTO_FIELD(String, fieldA) = "type-A";

};

class DtoTypeB : public oatpp::DTO {

  DTO_INIT(DtoTypeB, DTO)

  DTO_FIELD(String, fieldB) = "type-B";

};

class PolymorphicDto1 : public oatpp::DTO {

  DTO_INIT(PolymorphicDto1, DTO)

  DTO_FIELD(String, type);
  DTO_FIELD(Any, polymorph);

  DTO_FIELD_TYPE_SELECTOR(polymorph) {
    if(type == "A") return Object<DtoTypeA>::Class::getType();
    if(type == "B") return Object<DtoTypeB>::Class::getType();
    return Object<DTO>::Class::getType();
  }

};

class PolymorphicDto2 : public oatpp::DTO {

  DTO_INIT(PolymorphicDto2, DTO)

  DTO_FIELD(String, type);

  DTO_FIELD_INFO(polymorph) {
    info->description = "description";
  }
  DTO_FIELD(Any, polymorph);

  DTO_FIELD_TYPE_SELECTOR(polymorph) {
    if(type == "A") return Object<DtoTypeA>::Class::getType();
    if(type == "B") return Object<DtoTypeB>::Class::getType();
    return Object<DTO>::Class::getType();
  }

};

class PolymorphicDto3 : public oatpp::DTO {

  DTO_INIT(PolymorphicDto3, DTO)

  DTO_FIELD(String, type);
  DTO_FIELD(Any, polymorph);

  DTO_FIELD_TYPE_SELECTOR(polymorph) {
    if(type == "str") return String::Class::getType();
    if(type == "int") return Int32::Class::getType();
    return Void::Class::getType();
  }

};

#include OATPP_CODEGEN_END(DTO)

void runDtoInitializations() {
  for(v_int32 i = 0; i < 1000; i ++) {
    auto dto = DtoB::createShared();
  }
}

void runDtoInitializetionsInThreads() {

  std::list<std::thread> threads;
  for(v_int32 i = 0; i < 500; i++) {
    threads.push_back(std::thread(runDtoInitializations));
  }

  for(auto& t : threads) {
    t.join();
  }

}

}

void ObjectTest::onRun() {

  {
    oatpp::test::PerformanceChecker timer("DTO - Initializations.");
    runDtoInitializetionsInThreads();
  }

  {
    auto dto = DtoA::createShared("id1");
    OATPP_ASSERT(dto->id == "id1");
  }

  {
    OATPP_LOGI(TAG, "Test Meta 1...");

    auto type = Object<DtoA>::Class::getType();
    auto dispatcher = static_cast<const oatpp::data::mapping::type::__class::AbstractObject::PolymorphicDispatcher*>(type->polymorphicDispatcher);
    const auto& propsMap = dispatcher->getProperties()->getMap();

    OATPP_ASSERT(propsMap.size() == 1);

    auto it = propsMap.find("id");
    OATPP_ASSERT(it != propsMap.end());
    OATPP_ASSERT(it->second->info.description == "identifier");
    OATPP_ASSERT(it->second->info.pattern == "^[a-z0-9]+$");

    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test Meta 2...");

    auto type = Object<DtoB>::Class::getType();
    auto dispatcher = static_cast<const oatpp::data::mapping::type::__class::AbstractObject::PolymorphicDispatcher*>(type->polymorphicDispatcher);
    const auto& propsMap = dispatcher->getProperties()->getMap();

    OATPP_ASSERT(propsMap.size() == 2);

    {
      auto it = propsMap.find("id");
      OATPP_ASSERT("id" && it != propsMap.end());
      OATPP_ASSERT(it->second->info.description == "identifier");
    }

    {
      auto it = propsMap.find("field-a");
      OATPP_ASSERT("field-a" && it != propsMap.end());
      OATPP_ASSERT(it->second->info.description == "some field with a qualified name");
    }

    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test 1...");
    Object<DtoA> a;
    OATPP_ASSERT(!a);
    OATPP_ASSERT(a == nullptr);
    OATPP_ASSERT(a.getValueType()->classId.id == oatpp::data::mapping::type::__class::AbstractObject::CLASS_ID.id);
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test 2...");
    Object<DtoA> a;
    Object<DtoA> b;
    OATPP_ASSERT(a == b);
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test 3...");
    auto a = DtoA::createShared();
    Object<DtoA> b;
    OATPP_ASSERT(a != b);
    OATPP_ASSERT(b != a);
    auto ohc = a->hashCode();
    auto whc = std::hash<oatpp::Object<DtoA>>{}(a);
    OATPP_ASSERT(ohc == whc);
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test 4...");
    auto a = Dto0::createShared();
    auto b = Dto0::createShared();
    OATPP_ASSERT(a != b);
    OATPP_ASSERT(a->hashCode() != b->hashCode());
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test 5...");
    auto a = DtoA::createShared();
    auto b = DtoA::createShared();

    OATPP_ASSERT(a == b);
    OATPP_ASSERT(a->hashCode() == b->hashCode());

    a->id = "hello";

    OATPP_ASSERT(a != b);
    OATPP_ASSERT(a->hashCode() != b->hashCode());

    b->id = "hello";

    OATPP_ASSERT(a == b);
    OATPP_ASSERT(a->hashCode() == b->hashCode());
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test 6...");
    auto a = DtoB::createShared();
    auto b = DtoB::createShared();

    OATPP_ASSERT(a->a == "default-value");
    OATPP_ASSERT(b->a == "default-value");

    a->a = "value1"; // value that is ignored in HC & EQ
    a->a = "value2"; // value that is ignored in HC & EQ

    OATPP_ASSERT(a == b);
    OATPP_ASSERT(a->hashCode() == b->hashCode());

    a->id = "hello";

    OATPP_ASSERT(a != b);
    OATPP_ASSERT(a->hashCode() != b->hashCode());

    b->id = "hello";

    OATPP_ASSERT(a == b);
    OATPP_ASSERT(a->hashCode() == b->hashCode());
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test 7...");
    auto a = DtoC::createShared();
    auto b = DtoC::createShared();

    a->id = "1";
    b->id = "2";

    OATPP_ASSERT(a != b);
    OATPP_ASSERT(a->hashCode() != b->hashCode());

    a->id = "2";

    OATPP_ASSERT(a == b);
    OATPP_ASSERT(a->hashCode() == b->hashCode());

    a->c = "a";

    OATPP_ASSERT(a != b);
    OATPP_ASSERT(a->hashCode() != b->hashCode());

    b->c = "a";

    OATPP_ASSERT(a == b);
    OATPP_ASSERT(a->hashCode() == b->hashCode());
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test 8...");
    auto a = DtoB::createShared();
    auto b = DtoB::createShared();
    auto c = DtoB::createShared();
    auto d = DtoB::createShared();
    auto e = DtoB::createShared();

    a->a = "1";
    b->a = "2";
    c->a = "3";
    d->a = "4";
    e->a = "5";

    a->id = "1";
    e->id = "1";

    oatpp::UnorderedSet<oatpp::Object<DtoB>> set = {a, b, c, d, e};

    OATPP_ASSERT(set->size() == 2);
    OATPP_ASSERT(set[a] == true);
    OATPP_ASSERT(set[b] == true);
    OATPP_ASSERT(set[c] == true);
    OATPP_ASSERT(set[d] == true);
    OATPP_ASSERT(set[e] == true);
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test 9...");
    auto dto = DtoD::createShared();
    OATPP_ASSERT(dto->a.getValueType() == oatpp::Int32::Class::getType());
    OATPP_ASSERT(dto->a);
    OATPP_ASSERT(dto->a == 64);
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test 10...");
    OATPP_ASSERT(oatpp::Object<DtoA>::Class::getType()->extends(oatpp::Object<oatpp::DTO>::Class::getType()))
    OATPP_ASSERT(oatpp::Object<DtoA>::Class::getType()->extends(oatpp::Object<DtoA>::Class::getType()))
    OATPP_ASSERT(oatpp::Object<DtoB>::Class::getType()->extends(oatpp::Object<DtoA>::Class::getType()))
    OATPP_ASSERT(oatpp::Object<DtoB>::Class::getType()->extends(oatpp::Object<oatpp::DTO>::Class::getType()))
    OATPP_ASSERT(oatpp::Object<DtoC>::Class::getType()->extends(oatpp::Object<DtoA>::Class::getType()))
    OATPP_ASSERT(oatpp::Object<DtoD>::Class::getType()->extends(oatpp::Object<DtoA>::Class::getType()))
    OATPP_ASSERT(!oatpp::Object<DtoC>::Class::getType()->extends(oatpp::Object<DtoB>::Class::getType()))
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test 11...");

    auto map = oatpp::Object<PolymorphicDto1>::getPropertiesMap();
    auto p = map["polymorph"];

    OATPP_ASSERT(p->info.description == "");
    OATPP_ASSERT(p->info.typeSelector != nullptr);

    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test 12...");

    auto map = oatpp::Object<PolymorphicDto2>::getPropertiesMap();
    auto p = map["polymorph"];

    OATPP_ASSERT(p->info.description == "description");
    OATPP_ASSERT(p->info.typeSelector != nullptr);

    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test 13...");

    auto dto = oatpp::Object<PolymorphicDto2>::createShared();
    OATPP_ASSERT(dto->type == nullptr)
    OATPP_ASSERT(dto->type.getValueType() == oatpp::String::Class::getType())
    OATPP_ASSERT(dto["type"] == nullptr)
    OATPP_ASSERT(dto["type"].getValueType() == oatpp::String::Class::getType())

    dto["type"] = oatpp::String("hello");
    OATPP_ASSERT(dto->type == "hello");
    OATPP_ASSERT(dto->type.getValueType() == oatpp::String::Class::getType())

    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test 14...");

    auto dto = oatpp::Object<PolymorphicDto2>::createShared();
    bool thrown = false;

    try{
      dto["type"] = oatpp::Int32(32);
    } catch(std::runtime_error e) {
      OATPP_LOGD(TAG, "error='%s'", e.what());
      thrown = true;
    }

    OATPP_ASSERT(thrown)

    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test 15...");

    auto dto = oatpp::Object<PolymorphicDto2>::createShared();
    bool thrown = false;

    try{
      dto["non-existing"];
    } catch(std::out_of_range e) {
      OATPP_LOGD(TAG, "error='%s'", e.what());
      thrown = true;
    }

    OATPP_ASSERT(thrown)

    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test 16...");

    oatpp::parser::json::mapping::ObjectMapper mapper;

    auto dto = PolymorphicDto1::createShared();

    dto->type = "A";
    dto->polymorph = DtoTypeA::createShared();

    OATPP_ASSERT(dto->polymorph.getValueType() == oatpp::Any::Class::getType())

    auto json = mapper.writeToString(dto);
    OATPP_LOGD(TAG, "json0='%s'", json->c_str())

    auto dtoClone = mapper.readFromString<oatpp::Object<PolymorphicDto1>>(json);

    auto jsonClone = mapper.writeToString(dtoClone);
    OATPP_LOGD(TAG, "json1='%s'", jsonClone->c_str())

    OATPP_ASSERT(json == jsonClone)

    OATPP_ASSERT(dtoClone->polymorph)
    OATPP_ASSERT(dtoClone->polymorph.getValueType() == oatpp::Any::Class::getType())

    auto polymorphClone = dtoClone->polymorph.retrieve<oatpp::Object<DtoTypeA>>();

    OATPP_ASSERT(polymorphClone->fieldA == "type-A")

    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test 17...");

    oatpp::parser::json::mapping::ObjectMapper mapper;

    auto dto = PolymorphicDto3::createShared();

    dto->type = "str";
    dto->polymorph = oatpp::String("Hello World!");

    OATPP_ASSERT(dto->polymorph.getValueType() == oatpp::Any::Class::getType())

    auto json = mapper.writeToString(dto);
    OATPP_LOGD(TAG, "json0='%s'", json->c_str())

    auto dtoClone = mapper.readFromString<oatpp::Object<PolymorphicDto3>>(json);

    auto jsonClone = mapper.writeToString(dtoClone);
    OATPP_LOGD(TAG, "json1='%s'", jsonClone->c_str())

    OATPP_ASSERT(json == jsonClone)

    OATPP_ASSERT(dtoClone->polymorph)
    OATPP_ASSERT(dtoClone->polymorph.getValueType() == oatpp::Any::Class::getType())

    auto polymorphClone = dtoClone->polymorph.retrieve<oatpp::String>();

    OATPP_ASSERT(polymorphClone == "Hello World!")

    OATPP_LOGI(TAG, "OK");
  }

}

}}}}}}
