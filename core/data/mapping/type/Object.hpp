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

#ifndef oatpp_data_type_Object_hpp
#define oatpp_data_type_Object_hpp

#include "./Type.hpp"

#include "./Primitive.hpp"
#include "./List.hpp"

#include "oatpp/core/base/memory/ObjectPool.hpp"
#include "oatpp/core/base/Controllable.hpp"
#include "oatpp/core/base/String.hpp"
#include "oatpp/core/base/PtrWrapper.hpp"

namespace oatpp { namespace data { namespace mapping { namespace type {
  
namespace __class {
  
  class AbstractObject {
  public:
    constexpr static const char* const CLASS_NAME = "Object";
  };
  
  template<class T>
  class Object : public AbstractObject {
  public:
    
    static Type* getType(){
      static Type* type = static_cast<Type*>(T::Z__CLASS_GET_TYPE());
      return type;
    }
    
  };
  
}
  
class Object : public oatpp::base::Controllable {
public:
  typedef oatpp::data::mapping::type::StringPtrWrapper String;
  typedef oatpp::data::mapping::type::Int32::PtrWrapper Int32;
  typedef oatpp::data::mapping::type::Int64::PtrWrapper Int64;
  typedef oatpp::data::mapping::type::Float32::PtrWrapper Float32;
  typedef oatpp::data::mapping::type::Float64::PtrWrapper Float64;
  typedef oatpp::data::mapping::type::Boolean::PtrWrapper Boolean;
  template <class T>
  using List = oatpp::data::mapping::type::List<T>;
protected:
  static Type::Properties* Z__CLASS_EXTEND(Type::Properties* map, Type::Properties* extensionMap) {
    map->insert(extensionMap->begin(), extensionMap->end());
    return extensionMap;
  }
public:
  
  static oatpp::data::mapping::type::Type::Properties* Z__CLASS_GET_FIELDS_MAP(){
    static oatpp::data::mapping::type::Type::Properties map;
    return &map;
  }
  
};
  
}}}}

#endif /* oatpp_data_type_Object_hpp */
