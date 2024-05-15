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

#include "ObjectWrapperTest.hpp"
#include "oatpp/Types.hpp"

namespace oatpp { namespace data { namespace  type {

namespace {

  template<class T, class Clazz = oatpp::data::type::__class::Void>
  using TestWrapper = oatpp::data::type::ObjectWrapper<T, Clazz>;

}

void ObjectWrapperTest::onRun() {

  {
    OATPP_LOGi(TAG, "Check default valueType is assigned (default tparam Clazz)...")
    TestWrapper<std::string> pw;
    OATPP_ASSERT(!pw)
    OATPP_ASSERT(pw == nullptr)
    OATPP_ASSERT(pw.getValueType() == oatpp::data::type::__class::Void::getType())
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "Check default valueType is assigned (specified tparam Clazz)...")
    TestWrapper<std::string, oatpp::data::type::__class::String> pw;
    OATPP_ASSERT(!pw)
    OATPP_ASSERT(pw == nullptr)
    OATPP_ASSERT(pw.getValueType() == oatpp::data::type::__class::String::getType())
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "Check valueType is assigned from constructor...")
    TestWrapper<std::string> pw(oatpp::data::type::__class::String::getType());
    OATPP_ASSERT(!pw)
    OATPP_ASSERT(pw == nullptr)
    OATPP_ASSERT(pw.getValueType() == oatpp::data::type::__class::String::getType())
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "Check valueType is assigned from copy constructor...")
    TestWrapper<std::string> pw1(oatpp::data::type::__class::String::getType());
    TestWrapper<std::string> pw2(pw1);
    OATPP_ASSERT(pw2.getValueType() == oatpp::data::type::__class::String::getType())
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "Check valueType is assigned from move constructor...")
    TestWrapper<std::string> pw1(oatpp::data::type::__class::String::getType());
    TestWrapper<std::string> pw2(std::move(pw1));
    OATPP_ASSERT(pw2.getValueType() == oatpp::data::type::__class::String::getType())
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "Check valueType is NOT assigned from copy-assign operator...")
    TestWrapper<std::string> pw1(oatpp::data::type::__class::String::getType());
    TestWrapper<std::string> pw2;
    bool throws = false;
    try {
      pw2 = pw1;
    } catch (std::runtime_error&) {
      throws = true;
    }
    OATPP_ASSERT(pw2.getValueType() == oatpp::data::type::__class::Void::getType())
    OATPP_ASSERT(throws)
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "Check valueType is NOT assigned from move-assign operator...")
    TestWrapper<std::string> pw1(oatpp::data::type::__class::String::getType());
    TestWrapper<std::string> pw2;
    bool throws = false;
    try {
      pw2 = std::move(pw1);
    } catch (std::runtime_error&) {
      throws = true;
    }
    OATPP_ASSERT(pw2.getValueType() == oatpp::data::type::__class::Void::getType())
    OATPP_ASSERT(throws)
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "Check copy-assign operator. Check == operator...")
    TestWrapper<std::string> pw1;
    OATPP_ASSERT(!pw1)
    OATPP_ASSERT(pw1 == nullptr)
    OATPP_ASSERT(pw1.getValueType() == oatpp::data::type::__class::Void::getType())

    TestWrapper<std::string> pw2 = std::make_shared<std::string>("Hello!");
    OATPP_ASSERT(pw2)
    OATPP_ASSERT(pw2 != nullptr)
    OATPP_ASSERT(pw2.getValueType() == oatpp::data::type::__class::Void::getType())

    pw1 = pw2;

    OATPP_ASSERT(pw1)
    OATPP_ASSERT(pw1 != nullptr)

    OATPP_ASSERT(pw2)
    OATPP_ASSERT(pw2 != nullptr)

    OATPP_ASSERT(pw1 == pw2)
    OATPP_ASSERT(pw1.get() == pw2.get())
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "Check != operator...")
    TestWrapper<std::string, oatpp::data::type::__class::String> pw1(std::make_shared<std::string>("Hello!"));
    OATPP_ASSERT(pw1)
    OATPP_ASSERT(pw1 != nullptr)
    OATPP_ASSERT(pw1.getValueType() == oatpp::data::type::__class::String::getType())

    TestWrapper<std::string, oatpp::data::type::__class::String> pw2(std::make_shared<std::string>("Hello!"));
    OATPP_ASSERT(pw2)
    OATPP_ASSERT(pw2 != nullptr)
    OATPP_ASSERT(pw2.getValueType() == oatpp::data::type::__class::String::getType())

    OATPP_ASSERT(pw1 != pw2)
    OATPP_ASSERT(pw1.get() != pw2.get())
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "Check move-assign operator. Check != operator...")
    TestWrapper<std::string> pw1;
    OATPP_ASSERT(!pw1)
    OATPP_ASSERT(pw1 == nullptr)
    OATPP_ASSERT(pw1.getValueType() == oatpp::data::type::__class::Void::getType())

    TestWrapper<std::string> pw2 = std::make_shared<std::string>("Hello!");
    OATPP_ASSERT(pw2)
    OATPP_ASSERT(pw2 != nullptr)
    OATPP_ASSERT(pw2.getValueType() == oatpp::data::type::__class::Void::getType())

    pw1 = std::move(pw2);

    OATPP_ASSERT(pw1)
    OATPP_ASSERT(pw1 != nullptr)

    OATPP_ASSERT(!pw2)
    OATPP_ASSERT(pw2 == nullptr)

    OATPP_ASSERT(pw1 != pw2)
    OATPP_ASSERT(pw1.get() != pw2.get())
    OATPP_LOGi(TAG, "OK")
  }

  {
    OATPP_LOGi(TAG, "Check oatpp::Void type reassigned")

    oatpp::Void v;
    v = oatpp::String("test");

    OATPP_ASSERT(v.getValueType() == oatpp::String::Class::getType())

    v = oatpp::Int32(32);

    OATPP_ASSERT(v.getValueType() == oatpp::Int32::Class::getType())

    oatpp::Int32 i = v.cast<oatpp::Int32>();

    OATPP_ASSERT(i.getValueType() == oatpp::Int32::Class::getType())
    OATPP_ASSERT(i == 32)

  }

}

}}}
