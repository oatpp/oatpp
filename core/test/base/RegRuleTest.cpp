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

#include "../../src/data/mapping/type/Primitive.hpp"
#include "../../src/base/String.hpp"

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
  
  template <class PtrWrapperBC, class PtrWrapperCC, class BC, class CC>
  void checkPtrWrapper(){
    {
      PtrWrapperBC reg1(new BC);
      PtrWrapperBC reg2(reg1);
      OATPP_ASSERT(!reg1.isNull());
      PtrWrapperBC reg3(std::move(reg1));
      OATPP_ASSERT(reg1.isNull());
      PtrWrapperBC reg4(BC::template create1<PtrWrapperBC>());
      PtrWrapperBC reg5(BC::template create2<PtrWrapperBC>());
    }
    
    {
      PtrWrapperBC reg1 = new BC;
      PtrWrapperBC reg2 = reg1;
      OATPP_ASSERT(!reg1.isNull());
      PtrWrapperBC reg3 = std::move(reg1);
      OATPP_ASSERT(reg1.isNull());
      PtrWrapperBC reg4 = BC::template create1<PtrWrapperBC>();
      PtrWrapperBC reg5 = BC::template create2<PtrWrapperBC>();
    }
    
    {
      PtrWrapperBC reg1 = new BC;
      PtrWrapperBC reg2;
      reg2 = reg1;
      OATPP_ASSERT(!reg1.isNull());
      PtrWrapperBC reg3;
      reg3 = std::move(reg1);
      OATPP_ASSERT(reg1.isNull());
      PtrWrapperBC reg4;
      reg4 = BC::template create1<PtrWrapperBC>();
      PtrWrapperBC reg5;
      reg5 = BC::template create2<PtrWrapperBC>();
    }
    
    //---
    
    {
      PtrWrapperCC reg1(new CC);
      PtrWrapperBC reg2(reg1);
      OATPP_ASSERT(!reg1.isNull());
      PtrWrapperBC reg3(std::move(reg1));
      OATPP_ASSERT(!reg1.isNull());
      PtrWrapperBC reg4(CC::template create1<PtrWrapperCC>());
      PtrWrapperBC reg5(CC::template create2<PtrWrapperCC>());
    }
    
    {
      PtrWrapperCC reg1 = new CC;
      PtrWrapperBC reg2 = reg1;
      OATPP_ASSERT(!reg1.isNull());
      PtrWrapperBC reg3 = std::move(reg1);
      OATPP_ASSERT(!reg1.isNull());
      PtrWrapperBC reg4 = CC::template create1<PtrWrapperCC>();
      PtrWrapperBC reg5 = CC::template create2<PtrWrapperCC>();
    }
    
    {
      PtrWrapperCC reg1 = new CC;
      PtrWrapperBC reg2;
      reg2 = reg1;
      OATPP_ASSERT(!reg1.isNull());
      PtrWrapperBC reg3;
      reg3 = std::move(reg1);
      OATPP_ASSERT(!reg1.isNull());
      PtrWrapperBC reg4;
      reg4 = CC::template create1<PtrWrapperCC>();
      PtrWrapperBC reg5;
      reg5 = CC::template create2<PtrWrapperCC>();
    }
  }
  
}

bool RegRuleTest::onRun() {
  
  //checkPtrWrapper<PtrWrapper<BaseClass>, PtrWrapper<ChildClass>, BaseClass, ChildClass>();
  
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

  
  //checkPtrWrapper<PolymorphicPtrWrapper<BaseClass>, PolymorphicPtrWrapper<ChildClass>, BaseClass, ChildClass>();
  
  /*
  {
    PtrWrapper<BaseClass> reg1(new BaseClass);
    PtrWrapper<BaseClass> reg2(reg1);
    OATPP_ASSERT(!reg1.isNull());
    PtrWrapper<BaseClass> reg3(std::move(reg1));
    OATPP_ASSERT(reg1.isNull());
    PtrWrapper<BaseClass> reg4(BaseClass::create1<PtrWrapper<BaseClass>>());
    PtrWrapper<BaseClass> reg5(BaseClass::create2<PtrWrapper<BaseClass>>());
  }
  
  {
    PtrWrapper<BaseClass> reg1 = new BaseClass;
    PtrWrapper<BaseClass> reg2 = reg1;
    OATPP_ASSERT(!reg1.isNull());
    PtrWrapper<BaseClass> reg3 = std::move(reg1);
    OATPP_ASSERT(reg1.isNull());
    PtrWrapper<BaseClass> reg4 = BaseClass::create1<PtrWrapper<BaseClass>>();
    PtrWrapper<BaseClass> reg5 = BaseClass::create2<PtrWrapper<BaseClass>>();
  }
  
  {
    PtrWrapper<BaseClass> reg1 = new BaseClass;
    PtrWrapper<BaseClass> reg2;
    reg2 = reg1;
    OATPP_ASSERT(!reg1.isNull());
    PtrWrapper<BaseClass> reg3;
    reg3 = std::move(reg1);
    OATPP_ASSERT(reg1.isNull());
    PtrWrapper<BaseClass> reg4;
    reg4 = BaseClass::create1<PtrWrapper<BaseClass>>();
    PtrWrapper<BaseClass> reg5;
    reg5 = BaseClass::create2<PtrWrapper<BaseClass>>();
  }
  
  //---
  
  {
    PtrWrapper<ChildClass> reg1(new ChildClass);
    PtrWrapper<BaseClass> reg2(reg1);
    OATPP_ASSERT(!reg1.isNull());
    PtrWrapper<BaseClass> reg3(std::move(reg1));
    OATPP_ASSERT(!reg1.isNull());
    PtrWrapper<BaseClass> reg4(ChildClass::create1<PtrWrapper<ChildClass>>());
    PtrWrapper<BaseClass> reg5(ChildClass::create2<PtrWrapper<ChildClass>>());
  }
  
  {
    PtrWrapper<ChildClass> reg1 = new ChildClass;
    PtrWrapper<BaseClass> reg2 = reg1;
    OATPP_ASSERT(!reg1.isNull());
    PtrWrapper<BaseClass> reg3 = std::move(reg1);
    OATPP_ASSERT(!reg1.isNull());
    PtrWrapper<BaseClass> reg4 = ChildClass::create1<PtrWrapper<ChildClass>>();
    PtrWrapper<BaseClass> reg5 = ChildClass::create2<PtrWrapper<ChildClass>>();
  }
  
  {
    PtrWrapper<ChildClass> reg1 = new ChildClass;
    PtrWrapper<BaseClass> reg2;
    reg2 = reg1;
    OATPP_ASSERT(!reg1.isNull());
    PtrWrapper<BaseClass> reg3;
    reg3 = std::move(reg1);
    OATPP_ASSERT(!reg1.isNull());
    PtrWrapper<BaseClass> reg4;
    reg4 = ChildClass::create1<PtrWrapper<ChildClass>>();
    PtrWrapper<BaseClass> reg5;
    reg5 = ChildClass::create2<PtrWrapper<ChildClass>>();
  }
  */
  return true;
}
  
}}}
