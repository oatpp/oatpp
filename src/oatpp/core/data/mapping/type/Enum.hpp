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

      AbstractPolymorphicDispatcher(bool pNotNull)
        : notNull(pNotNull)
      {}

      const bool notNull;

      virtual type::Void toInterpretation(const type::Void& enumValue) const = 0;
      virtual type::Void fromInterpretation(const type::Void& interValue) const = 0;
      virtual type::Type* getInterpretationType() const = 0;

    };

  };

  template<class T, class Interpreter>
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
  const char* nameQualifier = nullptr;
  std::unordered_map<data::share::StringKeyLabel, EnumValueInfo<T>> byName;
  std::unordered_map<v_uint64, EnumValueInfo<T>> byValue;
  std::vector<EnumValueInfo<T>> byIndex;
};

template<class T, class Interpreter>
class EnumObjectWrapper; // FWD

template<class T>
class EnumMeta {

  template<class Type, class Interpreter>
  friend class __class::Enum;

  template<class Type, class Interpreter>
  friend class EnumObjectWrapper;

public:
  typedef T EnumType;
protected:
  static EnumInfo<T>* getInfo() {
    static EnumInfo<T> info;
    return &info;
  }
};

template<class T, bool nullable>
class EnumInterpreterAsString {
public:
  template <bool N>
  using InterpreterType = EnumInterpreterAsString<T, N>;
public:
  constexpr static bool notNull = nullable;
public:
  static Void toInterpretation(const Void& enumValue);
  static Void fromInterpretation(const Void& interValue);
  static Type* getInterpretationType();
};

template<class T, bool nullable>
class EnumInterpreterAsInteger {
public:
  template <bool N>
  using InterpreterType = EnumInterpreterAsInteger<T, N>;
public:
  constexpr static bool notNull = nullable;
public:
  static Void toInterpretation(const Void& enumValue);
  static Void fromInterpretation(const Void& interValue);
  static Type* getInterpretationType();
};

template<class T, class EnumInterpreter>
class EnumObjectWrapper : public ObjectWrapper<T, __class::Enum<T, EnumInterpreter>>{
  template<class Type, class Interpreter>
  friend class EnumObjectWrapper;
public:
  typedef EnumObjectWrapper __Wrapper;
public:
  typedef typename std::underlying_type<T>::type UnderlyingEnumType;
  typedef T Z__EnumType;
  typedef __class::Enum<T, EnumInterpreter> EnumObjectClass;
  typedef EnumInterpreter Interpreter;
public:
  typedef EnumObjectWrapper<T, EnumInterpreterAsString<T, EnumInterpreter::notNull>> AsString;
  typedef EnumObjectWrapper<T, EnumInterpreterAsInteger<T, EnumInterpreter::notNull>> AsInteger;
  typedef EnumObjectWrapper<T, typename EnumInterpreter::template InterpreterType<true>> NotNull;
public:

  EnumObjectWrapper(const std::shared_ptr<T>& ptr, const type::Type* const valueType)
    : type::ObjectWrapper<T, EnumObjectClass>(ptr, valueType)
  {}

  EnumObjectWrapper() {}

  EnumObjectWrapper(std::nullptr_t) {}

  EnumObjectWrapper(const std::shared_ptr<T>& ptr)
    : type::ObjectWrapper<T, EnumObjectClass>(ptr)
  {}

  EnumObjectWrapper(std::shared_ptr<T>&& ptr)
    : type::ObjectWrapper<T, EnumObjectClass>(std::forward<std::shared_ptr<T>>(ptr))
  {}

  template<class OtherInter>
  EnumObjectWrapper(const EnumObjectWrapper<T, OtherInter>& other)
    : type::ObjectWrapper<T, EnumObjectClass>(other)
  {}

  template<class OtherInter>
  EnumObjectWrapper(EnumObjectWrapper<T, OtherInter>&& other)
    : type::ObjectWrapper<T, EnumObjectClass>(std::forward<EnumObjectWrapper>(other))
  {}

  inline EnumObjectWrapper& operator = (std::nullptr_t) {
    this->m_ptr.reset();
    return *this;
  }

  template<class OtherInter>
  inline EnumObjectWrapper& operator = (const EnumObjectWrapper<T, OtherInter>& other) {
    this->m_ptr = other.m_ptr;
    return *this;
  }

  template<class OtherInter>
  inline EnumObjectWrapper& operator = (EnumObjectWrapper<T, OtherInter>&& other) {
    this->m_ptr = std::forward<std::shared_ptr<T>>(other.m_ptr);
    return *this;
  }

public:

  EnumObjectWrapper(T value)
    : type::ObjectWrapper<T, EnumObjectClass>(std::make_shared<T>(value))
  {}

  EnumObjectWrapper& operator = (T value) {
    this->m_ptr = std::make_shared<T>(value);
    return *this;
  }

  T operator*() const {
    return this->m_ptr.operator*();
  }

  template<typename TP,
    typename enabled = typename std::enable_if<std::is_same<TP, std::nullptr_t>::value, void>::type
  >
  inline bool operator == (TP){
    return this->m_ptr.get() == nullptr;
  }

  template<typename TP,
    typename enabled = typename std::enable_if<std::is_same<TP, std::nullptr_t>::value, void>::type
  >
  inline bool operator != (TP){
    return this->m_ptr.get() != nullptr;
  }

  template<typename TP,
    typename enabled = typename std::enable_if<std::is_same<TP, T>::value, void>::type
  >
  inline bool operator == (TP value) const {
    if(!this->m_ptr) return false;
    return *this->m_ptr == value;
  }

  template<typename TP,
    typename enabled = typename std::enable_if<std::is_same<TP, T>::value, void>::type
  >
  inline bool operator != (TP value) const {
    if(!this->m_ptr) return true;
    return *this->m_ptr != value;
  }

  template<typename TP,
    typename enabled = typename std::enable_if<std::is_same<typename TP::Z__EnumType, Z__EnumType>::value, void>::type
  >
  inline bool operator == (const TP &other) const {
    if(this->m_ptr.get() == other.m_ptr.get()) return true;
    if(!this->m_ptr || !other.m_ptr) return false;
    return *this->m_ptr == *other.m_ptr;
  }

  template<typename TP,
    typename enabled = typename std::enable_if<std::is_same<typename TP::Z__EnumType, Z__EnumType>::value, void>::type
  >
  inline bool operator != (const TP &other) const {
    return !operator == (other);
  }

  template<typename TP,
    typename enabled = typename std::enable_if<std::is_same<TP, T>::value, void>::type
  >
  inline operator TP() const {
    return *this->m_ptr;
  }

public:

  static EnumValueInfo<T> getEntryByName(const String& name) {
    auto it = EnumMeta<T>::getInfo()->byName.find(name);
    if(it != EnumMeta<T>::getInfo()->byName.end()) {
      return it->second;
    }
    throw std::runtime_error("[oatpp::data::mapping::type::Enum::getEntryByName()]: Error. Entry not found.");
  }

  static EnumValueInfo<T> getEntryByValue(T value) {
    auto it = EnumMeta<T>::getInfo()->byValue.find(static_cast<v_uint64>(value));
    if(it != EnumMeta<T>::getInfo()->byValue.end()) {
      return it->second;
    }
    throw std::runtime_error("[oatpp::data::mapping::type::Enum::getEntryByValue()]: Error. Entry not found.");
  }

  static EnumValueInfo<T> getEntryByUnderlyingValue(UnderlyingEnumType value) {
    auto it = EnumMeta<T>::getInfo()->byValue.find(static_cast<v_uint64>(value));
    if(it != EnumMeta<T>::getInfo()->byValue.end()) {
      return it->second;
    }
    throw std::runtime_error("[oatpp::data::mapping::type::Enum::getEntryByUnderlyingValue()]: Error. Entry not found.");
  }

  static EnumValueInfo<T> getEntryByIndex(v_int32 index) {
    if(index >= 0 && index < EnumMeta<T>::getInfo()->byIndex.size()) {
      return EnumMeta<T>::getInfo()->byIndex[index];
    }
    throw std::runtime_error("[oatpp::data::mapping::type::Enum::getEntryByIndex()]: Error. Entry not found.");
  }

  static const std::vector<EnumValueInfo<T>>& getEntries() {
    return EnumMeta<T>::getInfo()->byIndex;
  }

};

template <class T>
using Enum = EnumObjectWrapper<T, EnumInterpreterAsString<T, false>>;

template<class T, bool nullable>
Void EnumInterpreterAsString<T, nullable>::toInterpretation(const Void& enumValue) {
  typedef EnumObjectWrapper<T, EnumInterpreterAsString<T, nullable>> EnumOW;

  if(enumValue.valueType != EnumOW::Class::getType()) {
    throw std::runtime_error("[oatpp::data::mapping::type::EnumInterpreterAsString::toInterpretation()]: Error. Enum type doesn't match.");
  }

  const auto& ow = enumValue.staticCast<EnumOW>();
  const auto& entry = EnumOW::getEntryByValue(*ow);
  return entry.name.toString();
}

template<class T, bool nullable>
Void EnumInterpreterAsString<T, nullable>::fromInterpretation(const Void& interValue) {
  typedef EnumObjectWrapper<T, EnumInterpreterAsString<T, nullable>> EnumOW;

  if(interValue.valueType != String::Class::getType()) {
    throw std::runtime_error("[oatpp::data::mapping::type::EnumInterpreterAsString::fromInterpretation()]: Error. Interpretation must be a String.");
  }

  const auto& entry = EnumObjectWrapper<T, EnumInterpreterAsString<T, nullable>>::getEntryByName(interValue.staticCast<String>());
  return EnumOW(entry.value);
}

template<class T, bool nullable>
Type* EnumInterpreterAsString<T, nullable>::getInterpretationType() {
  return String::Class::getType();
}

template<class T, bool nullable>
Void EnumInterpreterAsInteger<T, nullable>::toInterpretation(const Void& enumValue) {

  typedef EnumObjectWrapper<T, EnumInterpreterAsInteger<T, nullable>> EnumOW;
  typedef typename std::underlying_type<T>::type EnumUT;
  typedef typename ObjectWrapperByUnderlyingType<EnumUT>::ObjectWrapper UTOW;

  if(enumValue.valueType != EnumOW::Class::getType()) {
    throw std::runtime_error("[oatpp::data::mapping::type::EnumInterpreterAsInteger::toInterpretation()]: Error. Enum type doesn't match.");
  }

  const auto& ow = enumValue.staticCast<EnumOW>();
  return UTOW(static_cast<EnumUT>(*ow));

}

template<class T, bool nullable>
Void EnumInterpreterAsInteger<T, nullable>::fromInterpretation(const Void& interValue) {
  typedef EnumObjectWrapper<T, EnumInterpreterAsInteger<T, nullable>> EnumOW;

  typedef typename std::underlying_type<T>::type EnumUT;
  typedef typename ObjectWrapperByUnderlyingType<EnumUT>::ObjectWrapper OW;

  if(interValue.valueType != OW::Class::getType()) {
    throw std::runtime_error("[oatpp::data::mapping::type::EnumInterpreterAsInteger::fromInterpretation()]: Error. Interpretation value type doesn't match.");
  }

  const auto& entry = EnumObjectWrapper<T, EnumInterpreterAsInteger<T, nullable>>::getEntryByUnderlyingValue(interValue.staticCast<OW>());
  return EnumOW(entry.value);
}

template<class T, bool nullable>
Type* EnumInterpreterAsInteger<T, nullable>::getInterpretationType() {
  typedef typename std::underlying_type<T>::type EnumUT;
  return ObjectWrapperByUnderlyingType<EnumUT>::ObjectWrapper::Class::getType();
}

namespace __class {

  template<class T, class Interpreter>
  class Enum : public AbstractEnum {
  private:

    class PolymorphicDispatcher : public AbstractPolymorphicDispatcher {
    public:
      PolymorphicDispatcher()
        : AbstractPolymorphicDispatcher(Interpreter::notNull)
      {}

      type::Void toInterpretation(const type::Void& enumValue) const override {
        return Interpreter::toInterpretation(enumValue);
      }

      type::Void fromInterpretation(const type::Void& interValue) const override {
        return Interpreter::fromInterpretation(interValue);
      }

      type::Type* getInterpretationType() const override {
        return Interpreter::getInterpretationType();
      }

    };

  private:

    static type::Void creator() {
      return type::Void(std::make_shared<T>(), getType());
    }

    static Type createType() {
      Type type(__class::AbstractEnum::CLASS_ID, type::EnumMeta<T>::getInfo()->nameQualifier, &creator, nullptr, new PolymorphicDispatcher());
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
