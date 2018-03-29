/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi, <lganzzzo@gmail.com>
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

#include "RegRuleTest.hpp"

#include "oatpp/core/data/mapping/type/Primitive.hpp"
#include "oatpp/core/base/String.hpp"

namespace oatpp { namespace test { namespace base {
  
namespace {
  
  class BaseClass : public oatpp::base::Controllable {
  public:
    
    template<class T>
    static T create1() {
      return new BaseClass();
    }
    
    template<class T>
    static T create2() {
      return T(new BaseClass());
    }
    
  };
  
  class ChildClass : public BaseClass {
  public:
    
    template<class T>
    static T create1() {
      return new ChildClass();
    }
    
    template<class T>
    static T create2() {
      return T(new ChildClass());
    }
    
  };
  
  typedef oatpp::base::String String;
  
  template<typename T>
  using PtrWrapper = oatpp::base::PtrWrapper<T>;
  
  template<typename T>
  using PolymorphicWrapper = oatpp::data::mapping::type::PolymorphicWrapper<T>;
  
  template<typename T>
  using TypePtrWrapper = oatpp::data::mapping::type::PtrWrapper<T, oatpp::data::mapping::type::__class::Void>;
  
  typedef oatpp::data::mapping::type::StringPtrWrapper StringPtrWrapper;
  typedef oatpp::data::mapping::type::Int32 Int32;
  typedef oatpp::data::mapping::type::Int64 Int64;
  typedef oatpp::data::mapping::type::Float32 Float32;
  typedef oatpp::data::mapping::type::Float64 Float64;
  typedef oatpp::data::mapping::type::Boolean Boolean;
  
}

bool RegRuleTest::onRun() {
  
  {
    String::PtrWrapper reg1("");
    String::PtrWrapper reg2(reg1);
    OATPP_ASSERT(!reg1.isNull());
    String::PtrWrapper reg3(std::move(reg1));
    OATPP_ASSERT(reg1.isNull());
    String::PtrWrapper reg4 = String::createShared(100);
  }
  
  {
    String::PtrWrapper reg1("");
    base::PtrWrapper<String> reg2(reg1);
    OATPP_ASSERT(!reg1.isNull());
    base::PtrWrapper<String> reg3(std::move(reg1));
    OATPP_ASSERT(reg1.isNull());
    base::PtrWrapper<String> reg4 = String::createShared(100) + "Leonid";
  }
  
  {
    base::PtrWrapper<String> reg1 = String::createShared(100);
    String::PtrWrapper reg2(reg1);
    OATPP_ASSERT(!reg1.isNull());
    String::PtrWrapper reg3(std::move(reg1));
    OATPP_ASSERT(reg1.isNull());
  }
  
  {
    String::PtrWrapper reg1(String::createShared(100) + "Leonid");
    StringPtrWrapper reg2(reg1);
    OATPP_ASSERT(!reg1.isNull());
    StringPtrWrapper reg3(std::move(reg1));
    OATPP_ASSERT(reg1.isNull());
    StringPtrWrapper reg4 = String::createShared(100);
  }
  
  {
    StringPtrWrapper reg1 = String::createShared(100);
    String::PtrWrapper reg2(reg1);
    OATPP_ASSERT(!reg1.isNull());
    String::PtrWrapper reg3(std::move(reg1));
    OATPP_ASSERT(reg1.isNull());
  }
  
  {
    base::PtrWrapper<String> reg1 = String::createShared(100);
    StringPtrWrapper reg2(reg1);
    OATPP_ASSERT(!reg1.isNull());
    StringPtrWrapper reg3(std::move(reg1));
    OATPP_ASSERT(reg1.isNull());
  }
  
  {
    StringPtrWrapper reg1 = String::createShared(100);
    base::PtrWrapper<String> reg2(reg1);
    OATPP_ASSERT(!reg1.isNull());
    base::PtrWrapper<String> reg3(std::move(reg1));
    OATPP_ASSERT(reg1.isNull());
  }

  return true;
}
  
}}}
