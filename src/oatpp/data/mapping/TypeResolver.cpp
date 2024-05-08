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

#include "TypeResolver.hpp"

namespace oatpp { namespace data { namespace mapping {

TypeResolver::TypeResolver() {

  m_knownClasses.resize(static_cast<size_t>(data::type::ClassId::getClassCount()), false);

  addKnownClasses({
    data::type::__class::String::CLASS_ID,
    data::type::__class::Any::CLASS_ID,

    data::type::__class::Int8::CLASS_ID,
    data::type::__class::UInt8::CLASS_ID,

    data::type::__class::Int16::CLASS_ID,
    data::type::__class::UInt16::CLASS_ID,

    data::type::__class::Int32::CLASS_ID,
    data::type::__class::UInt32::CLASS_ID,

    data::type::__class::Int64::CLASS_ID,
    data::type::__class::UInt64::CLASS_ID,

    data::type::__class::Float32::CLASS_ID,
    data::type::__class::Float64::CLASS_ID,
    data::type::__class::Boolean::CLASS_ID,

    data::type::__class::AbstractObject::CLASS_ID,
    data::type::__class::AbstractEnum::CLASS_ID,

    data::type::__class::AbstractVector::CLASS_ID,
    data::type::__class::AbstractList::CLASS_ID,
    data::type::__class::AbstractUnorderedSet::CLASS_ID,

    data::type::__class::AbstractPairList::CLASS_ID,
    data::type::__class::AbstractUnorderedMap::CLASS_ID
  });

}

void TypeResolver::setKnownClass(const type::ClassId& classId, bool isKnown) {
  const v_uint32 id = static_cast<v_uint32>(classId.id);
  if(id >= m_knownClasses.size()) {
    m_knownClasses.resize(id + 1, false);
  }
  m_knownClasses[id] = isKnown;
}

void TypeResolver::addKnownClasses(const std::vector<type::ClassId>& knownClasses) {
  for(const type::ClassId& id : knownClasses) {
    setKnownClass(id, true);
  }
}

bool TypeResolver::isKnownClass(const type::ClassId& classId) const {
  const v_uint32 id = static_cast<v_uint32>(classId.id);
  if(id < m_knownClasses.size()) {
    return m_knownClasses[id];
  }
  return false;
}

bool TypeResolver::isKnownType(const oatpp::Type* type) const {
  if (type != nullptr) {
    return isKnownClass(type->classId);
  }
  return false;
}

void TypeResolver::setEnabledInterpretations(const std::vector<std::string>& interpretations) {
  m_enabledInterpretations = interpretations;
}

const std::vector<std::string>& TypeResolver::getEnabledInterpretations() const {
  return m_enabledInterpretations;
}

const oatpp::Type* TypeResolver::resolveType(const oatpp::Type* type, Cache& cache) const {

  if(type == nullptr) {
    return nullptr;
  }

  if(isKnownClass(type->classId)) {
    return type;
  }

  auto it = cache.types.find(type);
  if(it != cache.types.end()) {
    return it->second;
  }

  auto interpretation = type->findInterpretation(m_enabledInterpretations);
  if(interpretation) {
    auto resolution = resolveType(interpretation->getInterpretationType(), cache);
    cache.types[type] = resolution;
    return resolution;
  }

  return nullptr;

}

type::Void TypeResolver::resolveValue(const type::Void& value, Cache& cache) const {

  if(value.getValueType() == nullptr) {
    return nullptr;
  }

  if(isKnownClass(value.getValueType()->classId)) {
    return value;
  }

  auto  typeIt = cache.values.find(value.getValueType());
  if(typeIt != cache.values.end()) {
    auto valueIt = typeIt->second.find(value);
    if(valueIt != typeIt->second.end()) {
      return valueIt->second;
    }
  }

  auto interpretation = value.getValueType()->findInterpretation(m_enabledInterpretations);
  if(interpretation) {
    auto resolution = resolveValue(interpretation->toInterpretation(value), cache);
    cache.values[value.getValueType()].insert({value, resolution});
    return resolution;
  }

  return nullptr;

}

const oatpp::Type* TypeResolver::findPropertyType(const oatpp::Type* baseType,
                                                 const std::vector<std::string>& path,
                                                 v_uint32 pathPosition,
                                                 Cache& cache) const
{

  if(isKnownType(baseType)) {
    if(pathPosition == path.size()) {
      return baseType;
    } else if(pathPosition < path.size()) {
      if(baseType->classId.id == type::__class::AbstractObject::CLASS_ID.id) {
        auto dispatcher = static_cast<const type::__class::AbstractObject::PolymorphicDispatcher*>(baseType->polymorphicDispatcher);
        const auto& map = dispatcher->getProperties()->getMap();
        auto it = map.find(path[pathPosition]);
        if(it != map.end()) {
          return findPropertyType(it->second->type, path, pathPosition + 1, cache);
        }
      }
      return nullptr;
    }
  }

  if(pathPosition > path.size()) {
    throw std::runtime_error("[oatpp::data::mapping::TypeResolver::findPropertyType()]: Error. Invalid state.");
  }

  auto resolvedType = resolveType(baseType, cache);
  if(resolvedType) {
    return findPropertyType(resolvedType, path, pathPosition, cache);
  }

  return nullptr;

}

type::Void TypeResolver::findPropertyValue(const type::Void& baseObject,
                                           const std::vector<std::string>& path,
                                           v_uint32 pathPosition,
                                           Cache& cache) const
{

  auto baseType = baseObject.getValueType();

  if(isKnownType(baseType)) {
    if(pathPosition == path.size()) {
      return baseObject;
    } else if(pathPosition < path.size()) {
      if(baseType->classId.id == type::__class::AbstractObject::CLASS_ID.id && baseObject) {
        auto dispatcher = static_cast<const type::__class::AbstractObject::PolymorphicDispatcher*>(baseType->polymorphicDispatcher);
        const auto& map = dispatcher->getProperties()->getMap();
        auto it = map.find(path[pathPosition]);
        if(it != map.end()) {
          auto property = it->second;
          return findPropertyValue(property->getAsRef(static_cast<type::BaseObject*>(baseObject.get())), path, pathPosition + 1, cache);
        }
      }
      return nullptr;
    }
  }

  if(pathPosition > path.size()) {
    throw std::runtime_error("[oatpp::data::mapping::TypeResolver::findPropertyValue()]: Error. Invalid state.");
  }

  const auto& resolution = resolveValue(baseObject, cache);
  if(resolution.getValueType()->classId.id != type::Void::Class::CLASS_ID.id) {
    return findPropertyValue(resolution, path, pathPosition, cache);
  }

  return nullptr;

}

const oatpp::Type* TypeResolver::resolveObjectPropertyType(const oatpp::Type* objectType,
                                                          const std::vector<std::string>& path,
                                                          Cache& cache) const
{
  return findPropertyType(objectType, path, 0, cache);
}

type::Void TypeResolver::resolveObjectPropertyValue(const type::Void& object,
                                                    const std::vector<std::string>& path,
                                                    Cache& cache) const
{
  return findPropertyValue(object, path, 0, cache);
}

}}}
