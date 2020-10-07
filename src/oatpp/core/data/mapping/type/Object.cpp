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

#include "./Object.hpp"

namespace oatpp { namespace data { namespace mapping { namespace type {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BaseObject

void BaseObject::set(v_int64 offset, const Void& value) {
  Void* property = (Void*)(((v_int64) m_basePointer) + offset);
  *property = value;
}

Void BaseObject::get(v_int64 offset) const {
  Void* property = (Void*)(((v_int64) m_basePointer) + offset);
  return *property;
}

Void& BaseObject::getAsRef(v_int64 offset) const {
  Void* property = (Void*)(((v_int64) m_basePointer) + offset);
  return *property;
}

void BaseObject::setBasePointer(void* basePointer) {
  m_basePointer = basePointer;
}

void* BaseObject::getBasePointer() const {
  return m_basePointer;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BaseObject::Properties

BaseObject::Property* BaseObject::Properties::pushBack(Property* property) {
  m_map.insert({property->name, property});
  m_list.push_back(property);
  return property;
}

void BaseObject::Properties::pushFrontAll(Properties* properties) {
  m_map.insert(properties->m_map.begin(), properties->m_map.end());
  m_list.insert(m_list.begin(), properties->m_list.begin(), properties->m_list.end());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BaseObject::Property

BaseObject::Property::Property(v_int64 pOffset, const char* pName, const Type* pType)
  : offset(pOffset)
  , name(pName)
  , type(pType)
{}

void BaseObject::Property::set(BaseObject* object, const Void& value) {
  object->set(offset, value);
}

Void BaseObject::Property::get(BaseObject* object) {
  return object->get(offset);
}

Void& BaseObject::Property::getAsRef(BaseObject* object) {
  return object->getAsRef(offset);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BaseObject::PropertyTraverser

BaseObject::PropertyTraverser::PropertyTraverser() {

  m_knownTypes.resize(data::mapping::type::ClassId::getClassCount(), false);

  setKnownClass(data::mapping::type::__class::String::CLASS_ID, true);
  setKnownClass(data::mapping::type::__class::Any::CLASS_ID, true);

  setKnownClass(data::mapping::type::__class::Int8::CLASS_ID, true);
  setKnownClass(data::mapping::type::__class::UInt8::CLASS_ID, true);

  setKnownClass(data::mapping::type::__class::Int16::CLASS_ID, true);
  setKnownClass(data::mapping::type::__class::UInt16::CLASS_ID, true);

  setKnownClass(data::mapping::type::__class::Int32::CLASS_ID, true);
  setKnownClass(data::mapping::type::__class::UInt32::CLASS_ID, true);

  setKnownClass(data::mapping::type::__class::Int64::CLASS_ID, true);
  setKnownClass(data::mapping::type::__class::UInt64::CLASS_ID, true);

  setKnownClass(data::mapping::type::__class::Float32::CLASS_ID, true);
  setKnownClass(data::mapping::type::__class::Float64::CLASS_ID, true);
  setKnownClass(data::mapping::type::__class::Boolean::CLASS_ID, true);

  setKnownClass(data::mapping::type::__class::AbstractObject::CLASS_ID, true);
  setKnownClass(data::mapping::type::__class::AbstractEnum::CLASS_ID, true);

  setKnownClass(data::mapping::type::__class::AbstractVector::CLASS_ID, true);
  setKnownClass(data::mapping::type::__class::AbstractList::CLASS_ID, true);
  setKnownClass(data::mapping::type::__class::AbstractUnorderedSet::CLASS_ID, true);

  setKnownClass(data::mapping::type::__class::AbstractPairList::CLASS_ID, true);
  setKnownClass(data::mapping::type::__class::AbstractUnorderedMap::CLASS_ID, true);

}

bool BaseObject::PropertyTraverser::isKnownType(const ClassId& classId) const {
  const v_uint32 id = classId.id;
  if(id < m_knownTypes.size()) {
    return m_knownTypes[id];
  }
  return false;
}

void BaseObject::PropertyTraverser::setKnownClass(const ClassId& classId, bool isKnown) {
  const v_uint32 id = classId.id;
  if(id < m_knownTypes.size()) {
    m_knownTypes[id] = isKnown;
  } else {
    throw std::runtime_error("[oatpp::data::mapping::type::BaseObject::PropertyTraverser::setKnownClass()]: Error. Unknown classId");
  }
}

void BaseObject::PropertyTraverser::addKnownClasses(const std::vector<ClassId>& knownClasses) {
  for(const ClassId& id : knownClasses) {
    setKnownClass(id, true);
  }
}

const Type* BaseObject::PropertyTraverser::findPropertyType(const Type* baseType,
                                                            const std::vector<std::string>& path,
                                                            v_uint32 pathPosition,
                                                            const std::vector<std::string>& allowedInterpretations) const
{
  if(isKnownType(baseType->classId)) {
    if(pathPosition == path.size()) {
      return baseType;
    } else if(pathPosition < path.size()) {
      if(baseType->classId.id == __class::AbstractObject::CLASS_ID.id) {
        auto dispatcher = static_cast<const __class::AbstractObject::PolymorphicDispatcher*>(baseType->polymorphicDispatcher);
        const auto& map = dispatcher->getProperties()->getMap();
        auto it = map.find(path[pathPosition]);
        if(it != map.end()) {
          return findPropertyType(it->second->type, path, pathPosition + 1, allowedInterpretations);
        }
      }
      return nullptr;
    }
  }

  if(pathPosition > path.size()) {
    throw std::runtime_error("[oatpp::data::mapping::type::BaseObject::PropertyTraverser::findPropertyType()]: "
                             "Error. Invalid state.");
  }

  auto* interpretation = baseType->findInterpretation(allowedInterpretations);
  if(interpretation) {
    return findPropertyType(interpretation->getInterpretationType(), path, pathPosition, allowedInterpretations);
  }

  return nullptr;
}

Void BaseObject::PropertyTraverser::findPropertyValue(const Void& baseObject,
                                                      const std::vector<std::string>& path,
                                                      v_uint32 pathPosition,
                                                      const std::vector<std::string>& allowedInterpretations) const
{
  auto baseType = baseObject.valueType;

  if(isKnownType(baseType->classId)) {
    if(pathPosition == path.size()) {
      return baseObject;
    } else if(pathPosition < path.size()) {
      if(baseType->classId.id == __class::AbstractObject::CLASS_ID.id && baseObject) {
        auto dispatcher = static_cast<const __class::AbstractObject::PolymorphicDispatcher*>(baseType->polymorphicDispatcher);
        const auto& map = dispatcher->getProperties()->getMap();
        auto it = map.find(path[pathPosition]);
        if(it != map.end()) {
          auto property = it->second;
          return findPropertyValue(property->getAsRef(static_cast<BaseObject*>(baseObject.get())), path, pathPosition + 1, allowedInterpretations);
        }
      }
      return nullptr;
    }
  }

  if(pathPosition > path.size()) {
    throw std::runtime_error("[oatpp::data::mapping::type::BaseObject::PropertyTraverser::findPropertyType()]: "
                             "Error. Invalid state.");
  }

  auto* interpretation = baseType->findInterpretation(allowedInterpretations);
  if(interpretation && baseObject) {
    return findPropertyValue(interpretation->toInterpretation(baseObject), path, pathPosition, allowedInterpretations);
  }

  return nullptr;
}

const Type* BaseObject::PropertyTraverser::findPropertyType(const Type* baseType,
                                                            const std::vector<std::string>& path,
                                                            const std::vector<std::string>& allowedInterpretations) const
{
  return findPropertyType(baseType, path, 0, allowedInterpretations);
}

Void BaseObject::PropertyTraverser::findPropertyValue(const Void& baseObject,
                                                      const std::vector<std::string>& path,
                                                      const std::vector<std::string>& allowedInterpretations) const
{
  return findPropertyValue(baseObject, path, 0, allowedInterpretations);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Object

namespace __class {

  const ClassId AbstractObject::CLASS_ID("Object");

}

BaseObject::Properties* DTO::Z__CLASS_EXTEND(BaseObject::Properties* properties, BaseObject::Properties* extensionProperties) {
  properties->pushFrontAll(extensionProperties);
  return properties;
}

oatpp::data::mapping::type::BaseObject::Properties* DTO::Z__CLASS_GET_FIELDS_MAP() {
  static data::mapping::type::BaseObject::Properties map;
  return &map;
}
  
}}}}
