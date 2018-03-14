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
  using SharedWrapper = oatpp::base::SharedWrapper<T>;
  
  template<typename T>
  using PolymorphicWrapper = oatpp::data::mapping::type::PolymorphicWrapper<T>;
  
  template<typename T>
  using TypeSharedWrapper = oatpp::data::mapping::type::SharedWrapper<T, oatpp::data::mapping::type::__class::Void>;
  
  typedef oatpp::data::mapping::type::StringSharedWrapper StringSharedWrapper;
  typedef oatpp::data::mapping::type::Int32 Int32;
  typedef oatpp::data::mapping::type::Int64 Int64;
  typedef oatpp::data::mapping::type::Float32 Float32;
  typedef oatpp::data::mapping::type::Float64 Float64;
  typedef oatpp::data::mapping::type::Boolean Boolean;
  
  template <class SharedWrapperBC, class SharedWrapperCC, class BC, class CC>
  void checkSharedWrapper(){
    {
      SharedWrapperBC reg1(new BC);
      SharedWrapperBC reg2(reg1);
      OATPP_ASSERT(!reg1.isNull());
      SharedWrapperBC reg3(std::move(reg1));
      OATPP_ASSERT(reg1.isNull());
      SharedWrapperBC reg4(BC::template create1<SharedWrapperBC>());
      SharedWrapperBC reg5(BC::template create2<SharedWrapperBC>());
    }
    
    {
      SharedWrapperBC reg1 = new BC;
      SharedWrapperBC reg2 = reg1;
      OATPP_ASSERT(!reg1.isNull());
      SharedWrapperBC reg3 = std::move(reg1);
      OATPP_ASSERT(reg1.isNull());
      SharedWrapperBC reg4 = BC::template create1<SharedWrapperBC>();
      SharedWrapperBC reg5 = BC::template create2<SharedWrapperBC>();
    }
    
    {
      SharedWrapperBC reg1 = new BC;
      SharedWrapperBC reg2;
      reg2 = reg1;
      OATPP_ASSERT(!reg1.isNull());
      SharedWrapperBC reg3;
      reg3 = std::move(reg1);
      OATPP_ASSERT(reg1.isNull());
      SharedWrapperBC reg4;
      reg4 = BC::template create1<SharedWrapperBC>();
      SharedWrapperBC reg5;
      reg5 = BC::template create2<SharedWrapperBC>();
    }
    
    //---
    
    {
      SharedWrapperCC reg1(new CC);
      SharedWrapperBC reg2(reg1);
      OATPP_ASSERT(!reg1.isNull());
      SharedWrapperBC reg3(std::move(reg1));
      OATPP_ASSERT(!reg1.isNull());
      SharedWrapperBC reg4(CC::template create1<SharedWrapperCC>());
      SharedWrapperBC reg5(CC::template create2<SharedWrapperCC>());
    }
    
    {
      SharedWrapperCC reg1 = new CC;
      SharedWrapperBC reg2 = reg1;
      OATPP_ASSERT(!reg1.isNull());
      SharedWrapperBC reg3 = std::move(reg1);
      OATPP_ASSERT(!reg1.isNull());
      SharedWrapperBC reg4 = CC::template create1<SharedWrapperCC>();
      SharedWrapperBC reg5 = CC::template create2<SharedWrapperCC>();
    }
    
    {
      SharedWrapperCC reg1 = new CC;
      SharedWrapperBC reg2;
      reg2 = reg1;
      OATPP_ASSERT(!reg1.isNull());
      SharedWrapperBC reg3;
      reg3 = std::move(reg1);
      OATPP_ASSERT(!reg1.isNull());
      SharedWrapperBC reg4;
      reg4 = CC::template create1<SharedWrapperCC>();
      SharedWrapperBC reg5;
      reg5 = CC::template create2<SharedWrapperCC>();
    }
  }
  
}

bool RegRuleTest::onRun() {
  
  //checkSharedWrapper<SharedWrapper<BaseClass>, SharedWrapper<ChildClass>, BaseClass, ChildClass>();
  
  {
    String::SharedWrapper reg1("");
    String::SharedWrapper reg2(reg1);
    OATPP_ASSERT(!reg1.isNull());
    String::SharedWrapper reg3(std::move(reg1));
    OATPP_ASSERT(reg1.isNull());
    String::SharedWrapper reg4 = String::createShared(100);
  }
  
  {
    String::SharedWrapper reg1("");
    base::SharedWrapper<String> reg2(reg1);
    OATPP_ASSERT(!reg1.isNull());
    base::SharedWrapper<String> reg3(std::move(reg1));
    OATPP_ASSERT(reg1.isNull());
    base::SharedWrapper<String> reg4 = String::createShared(100) + "Leonid";
  }
  
  {
    base::SharedWrapper<String> reg1 = String::createShared(100);
    String::SharedWrapper reg2(reg1);
    OATPP_ASSERT(!reg1.isNull());
    String::SharedWrapper reg3(std::move(reg1));
    OATPP_ASSERT(reg1.isNull());
  }
  
  {
    String::SharedWrapper reg1(String::createShared(100) + "Leonid");
    StringSharedWrapper reg2(reg1);
    OATPP_ASSERT(!reg1.isNull());
    StringSharedWrapper reg3(std::move(reg1));
    OATPP_ASSERT(reg1.isNull());
    StringSharedWrapper reg4 = String::createShared(100);
  }
  
  {
    StringSharedWrapper reg1 = String::createShared(100);
    String::SharedWrapper reg2(reg1);
    OATPP_ASSERT(!reg1.isNull());
    String::SharedWrapper reg3(std::move(reg1));
    OATPP_ASSERT(reg1.isNull());
  }
  
  {
    base::SharedWrapper<String> reg1 = String::createShared(100);
    StringSharedWrapper reg2(reg1);
    OATPP_ASSERT(!reg1.isNull());
    StringSharedWrapper reg3(std::move(reg1));
    OATPP_ASSERT(reg1.isNull());
  }
  
  {
    StringSharedWrapper reg1 = String::createShared(100);
    base::SharedWrapper<String> reg2(reg1);
    OATPP_ASSERT(!reg1.isNull());
    base::SharedWrapper<String> reg3(std::move(reg1));
    OATPP_ASSERT(reg1.isNull());
  }

  
  //checkSharedWrapper<PolymorphicSharedWrapper<BaseClass>, PolymorphicSharedWrapper<ChildClass>, BaseClass, ChildClass>();
  
  /*
  {
    SharedWrapper<BaseClass> reg1(new BaseClass);
    SharedWrapper<BaseClass> reg2(reg1);
    OATPP_ASSERT(!reg1.isNull());
    SharedWrapper<BaseClass> reg3(std::move(reg1));
    OATPP_ASSERT(reg1.isNull());
    SharedWrapper<BaseClass> reg4(BaseClass::create1<SharedWrapper<BaseClass>>());
    SharedWrapper<BaseClass> reg5(BaseClass::create2<SharedWrapper<BaseClass>>());
  }
  
  {
    SharedWrapper<BaseClass> reg1 = new BaseClass;
    SharedWrapper<BaseClass> reg2 = reg1;
    OATPP_ASSERT(!reg1.isNull());
    SharedWrapper<BaseClass> reg3 = std::move(reg1);
    OATPP_ASSERT(reg1.isNull());
    SharedWrapper<BaseClass> reg4 = BaseClass::create1<SharedWrapper<BaseClass>>();
    SharedWrapper<BaseClass> reg5 = BaseClass::create2<SharedWrapper<BaseClass>>();
  }
  
  {
    SharedWrapper<BaseClass> reg1 = new BaseClass;
    SharedWrapper<BaseClass> reg2;
    reg2 = reg1;
    OATPP_ASSERT(!reg1.isNull());
    SharedWrapper<BaseClass> reg3;
    reg3 = std::move(reg1);
    OATPP_ASSERT(reg1.isNull());
    SharedWrapper<BaseClass> reg4;
    reg4 = BaseClass::create1<SharedWrapper<BaseClass>>();
    SharedWrapper<BaseClass> reg5;
    reg5 = BaseClass::create2<SharedWrapper<BaseClass>>();
  }
  
  //---
  
  {
    SharedWrapper<ChildClass> reg1(new ChildClass);
    SharedWrapper<BaseClass> reg2(reg1);
    OATPP_ASSERT(!reg1.isNull());
    SharedWrapper<BaseClass> reg3(std::move(reg1));
    OATPP_ASSERT(!reg1.isNull());
    SharedWrapper<BaseClass> reg4(ChildClass::create1<SharedWrapper<ChildClass>>());
    SharedWrapper<BaseClass> reg5(ChildClass::create2<SharedWrapper<ChildClass>>());
  }
  
  {
    SharedWrapper<ChildClass> reg1 = new ChildClass;
    SharedWrapper<BaseClass> reg2 = reg1;
    OATPP_ASSERT(!reg1.isNull());
    SharedWrapper<BaseClass> reg3 = std::move(reg1);
    OATPP_ASSERT(!reg1.isNull());
    SharedWrapper<BaseClass> reg4 = ChildClass::create1<SharedWrapper<ChildClass>>();
    SharedWrapper<BaseClass> reg5 = ChildClass::create2<SharedWrapper<ChildClass>>();
  }
  
  {
    SharedWrapper<ChildClass> reg1 = new ChildClass;
    SharedWrapper<BaseClass> reg2;
    reg2 = reg1;
    OATPP_ASSERT(!reg1.isNull());
    SharedWrapper<BaseClass> reg3;
    reg3 = std::move(reg1);
    OATPP_ASSERT(!reg1.isNull());
    SharedWrapper<BaseClass> reg4;
    reg4 = ChildClass::create1<SharedWrapper<ChildClass>>();
    SharedWrapper<BaseClass> reg5;
    reg5 = ChildClass::create2<SharedWrapper<ChildClass>>();
  }
  */
  return true;
}
  
}}}
