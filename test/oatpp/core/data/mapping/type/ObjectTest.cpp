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

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

namespace oatpp { namespace test { namespace core { namespace data { namespace mapping { namespace  type {

namespace {

#include OATPP_CODEGEN_BEGIN(DTO)

class Dto0 : public oatpp::Object {
  DTO_INIT(Dto0, Object)
};

class DtoA : public oatpp::Object {

  DTO_INIT(DtoA, Object)

  DTO_FIELD(String, id);

  DTO_HC_EQ(id)

};

class DtoB : public DtoA {

  DTO_INIT(DtoB, DtoA)

  DTO_FIELD(String, a);

};

class DtoC : public DtoA {

  DTO_INIT(DtoC, DtoA)

  DTO_FIELD(String, a);
  DTO_FIELD(String, b);
  DTO_FIELD(String, c);

  DTO_HC_EQ(a, b, c);

};

#include OATPP_CODEGEN_END(DTO)

}

void ObjectTest::onRun() {

  {
    OATPP_LOGI(TAG, "Test 1...");
    DtoA::ObjectWrapper a;
    OATPP_ASSERT(!a);
    OATPP_ASSERT(a == nullptr);
    OATPP_ASSERT(a.valueType->classId.id == oatpp::data::mapping::type::__class::AbstractObject::CLASS_ID.id);
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test 2...");
    DtoA::ObjectWrapper a;
    DtoA::ObjectWrapper b;
    OATPP_ASSERT(a == b);
    OATPP_LOGI(TAG, "OK");
  }

  {
    OATPP_LOGI(TAG, "Test 3...");
    auto a = DtoA::createShared();
    DtoA::ObjectWrapper b;
    OATPP_ASSERT(a != b);
    OATPP_ASSERT(b != a);
    auto ohc = a->hashCode();
    auto whc = std::hash<DtoA::ObjectWrapper>{}(a);
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

    oatpp::UnorderedSet<DtoB> set = {a, b, c, d, e};

    OATPP_ASSERT(set->size() == 2);
    OATPP_ASSERT(set[a] == true);
    OATPP_ASSERT(set[b] == true);
    OATPP_ASSERT(set[c] == true);
    OATPP_ASSERT(set[d] == true);
    OATPP_ASSERT(set[e] == true);
    OATPP_LOGI(TAG, "OK");
  }

}

}}}}}}
