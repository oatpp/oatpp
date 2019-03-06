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

#include "RegRuleTest.hpp"

#include "oatpp/core/data/mapping/type/Primitive.hpp"
#include "oatpp/core/Types.hpp"

#include <unordered_map>

namespace oatpp { namespace test { namespace base {
  
namespace {
  
  class BaseClass : public oatpp::base::Countable {
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
  
  typedef oatpp::String String;
  
  template<typename T>
  using ObjectWrapper = oatpp::data::mapping::type::PolymorphicWrapper<T>;
  
  template<typename T>
  using PolymorphicWrapper = oatpp::data::mapping::type::PolymorphicWrapper<T>;
  
  template<typename T>
  using TypeObjectWrapper = oatpp::data::mapping::type::ObjectWrapper<T, oatpp::data::mapping::type::__class::Void>;
  
  typedef oatpp::data::mapping::type::Int32 Int32;
  typedef oatpp::data::mapping::type::Int64 Int64;
  typedef oatpp::data::mapping::type::Float32 Float32;
  typedef oatpp::data::mapping::type::Float64 Float64;
  typedef oatpp::data::mapping::type::Boolean Boolean;
  
}

void RegRuleTest::onRun() {
  
  {
    String reg1("");
    String reg2(reg1);
    OATPP_ASSERT(reg1);
    String reg3(std::move(reg1));
    OATPP_ASSERT(!reg1);
    String reg4 = String(100);
  }
  
  {
    String reg1("");
    String reg2(reg1);
    OATPP_ASSERT(reg1);
    String reg3(std::move(reg1));
    OATPP_ASSERT(!reg1);
    String reg4 = String(100) + "Leonid";
  }
  
  {
    String reg1 = String(100);
    String reg2(reg1);
    OATPP_ASSERT(reg1);
    String reg3(std::move(reg1));
    OATPP_ASSERT(!reg1);
  }
  
  {
    String reg1(String(100) + "Leonid");
    String reg2(reg1);
    OATPP_ASSERT(reg1);
    String reg3(std::move(reg1));
    OATPP_ASSERT(!reg1);
    String reg4 = String(100);
  }
  
  {
    String reg1 = String(100);
    String reg2(reg1);
    OATPP_ASSERT(reg1);
    String reg3(std::move(reg1));
    OATPP_ASSERT(!reg1);
  }
  
  {
    String reg1 = String(100);
    String reg2(reg1);
    OATPP_ASSERT(reg1);
    String reg3(std::move(reg1));
    OATPP_ASSERT(!reg1);
  }
  
  {
    String reg1 = String(100);
    String reg2(reg1);
    OATPP_ASSERT(reg1);
    String reg3(std::move(reg1));
    OATPP_ASSERT(!reg1);
  }

  std::unordered_map<String, String> map;
  
  map["str_1"] = "val_1";
  map["str_2"] = "val_2";
  map["str_3"] = "val_3";
  
  OATPP_ASSERT(map.find("str_1")->second == "val_1");
  OATPP_ASSERT(map.find("str_2")->second == "val_2");
  OATPP_ASSERT(map.find("str_3")->second == "val_3");

}
  
}}}
