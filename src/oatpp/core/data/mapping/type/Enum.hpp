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

#ifndef oatpp_data_mapping_type_Enum_hpp
#define oatpp_data_mapping_type_Enum_hpp

#include "./Primitive.hpp"
#include "oatpp/core/data/share/MemoryLabel.hpp"

#include <type_traits>
#include <unordered_map>
#include <vector>

namespace oatpp { namespace data { namespace mapping { namespace type {

namespace __class {

  class AbstractEnum {
  public:
    static const ClassId CLASS_ID;
  public:

    class AbstractPolymorphicDispatcher {
    public:

    };

  };

  template<class T>
  class Enum;

}

template<typename T>
struct EnumValueInfo {
  const T value;
  const v_int32 index;
  const data::share::StringKeyLabel name;
};

template<typename T>
struct EnumInfo {
public:
  const char* nameQualifier;
  std::unordered_map<data::share::StringKeyLabel, EnumValueInfo<T>> byName;
  std::unordered_map<v_uint64, EnumValueInfo<T>> byValue;
  std::vector<EnumValueInfo<T>> byIndex;
};

template<class T>
class Enum; // FWD

template<class T>
class EnumMeta {
  friend __class::Enum<T>;
  friend Enum<T>;
public:
  typedef T EnumType;
protected:
  static EnumInfo<T> __info;
};

template<class T>
EnumInfo<T> EnumMeta<T>::__info;

template<class T>
class Enum : public ObjectWrapper<T, __class::Enum<T>>{
public:
  typedef typename std::underlying_type<T>::type UnderlyingType;
public:

  static EnumValueInfo<T> getEntryByName(const String& name) {
    auto it = EnumMeta<T>::__info.byName.find(name);
    if(it != EnumMeta<T>::__info.byName.end()) {
      return it->second;
    }
    throw std::runtime_error("[oatpp::data::mapping::type::Enum::getEntryByName()]: Error. Entry not found.");
  }

  static EnumValueInfo<T> getEntryByValue(T value) {
    auto it = EnumMeta<T>::__info.byValue.find(static_cast<v_uint64>(value));
    if(it != EnumMeta<T>::__info.byValue.end()) {
      return it->second;
    }
    throw std::runtime_error("[oatpp::data::mapping::type::Enum::getEntryByValue()]: Error. Entry not found.");
  }

  static EnumValueInfo<T> getEntryByUnderlyingValue(UnderlyingType value) {
    auto it = EnumMeta<T>::__info.byValue.find(static_cast<v_uint64>(value));
    if(it != EnumMeta<T>::__info.byValue.end()) {
      return it->second;
    }
    throw std::runtime_error("[oatpp::data::mapping::type::Enum::getEntryByUnderlyingValue()]: Error. Entry not found.");
  }

  static EnumValueInfo<T> getEntryByIndex(v_int32 index) {
    if(index >= 0 && index < EnumMeta<T>::__info.byIndex.size()) {
      return EnumMeta<T>::__info.byIndex[index];
    }
    throw std::runtime_error("[oatpp::data::mapping::type::Enum::getEntryByIndex()]: Error. Entry not found.");
  }

  static const std::vector<EnumValueInfo<T>>& getEntries() {
    return EnumMeta<T>::__info.byIndex;
  }

};

namespace __class {

  template<class T>
  class Enum : public AbstractEnum {
  private:

    class PolymorphicDispatcher : public AbstractPolymorphicDispatcher {
    public:

    };

  private:

    static type::Void creator() {
      return type::Void(std::make_shared<T>(), getType());
    }

    static Type createType() {
      Type type(__class::AbstractEnum::CLASS_ID, type::EnumMeta<T>::__info.nameQualifier, &creator, nullptr, new PolymorphicDispatcher());
      return type;
    }

  public:

    static Type* getType() {
      static Type type = createType();
      return &type;
    }

  };

}

}}}}

#endif // oatpp_data_mapping_type_Enum_hpp
