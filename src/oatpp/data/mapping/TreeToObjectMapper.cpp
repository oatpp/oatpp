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

#include "TreeToObjectMapper.hpp"

#include "oatpp/data/stream/BufferStream.hpp"
#include "oatpp/utils/Conversion.hpp"

namespace oatpp { namespace data { namespace mapping {

oatpp::String TreeToObjectMapper::MappingState::errorStacktrace() const {
  stream::BufferOutputStream ss;
  for(auto& s : errorStack) {
    ss << s << "\n";
  }
  return ss.toString();
}

TreeToObjectMapper::TreeToObjectMapper() {

  m_methods.resize(static_cast<size_t>(data::mapping::type::ClassId::getClassCount()), nullptr);

  setMapperMethod(data::mapping::type::__class::String::CLASS_ID, &TreeToObjectMapper::mapString);
  setMapperMethod(data::mapping::type::__class::Any::CLASS_ID, &TreeToObjectMapper::mapAny);

  setMapperMethod(data::mapping::type::__class::Int8::CLASS_ID, &TreeToObjectMapper::mapPrimitive<oatpp::Int8>);
  setMapperMethod(data::mapping::type::__class::UInt8::CLASS_ID, &TreeToObjectMapper::mapPrimitive<oatpp::UInt8>);

  setMapperMethod(data::mapping::type::__class::Int16::CLASS_ID, &TreeToObjectMapper::mapPrimitive<oatpp::Int16>);
  setMapperMethod(data::mapping::type::__class::UInt16::CLASS_ID, &TreeToObjectMapper::mapPrimitive<oatpp::UInt16>);

  setMapperMethod(data::mapping::type::__class::Int32::CLASS_ID, &TreeToObjectMapper::mapPrimitive<oatpp::Int32>);
  setMapperMethod(data::mapping::type::__class::UInt32::CLASS_ID, &TreeToObjectMapper::mapPrimitive<oatpp::UInt32>);

  setMapperMethod(data::mapping::type::__class::Int64::CLASS_ID, &TreeToObjectMapper::mapPrimitive<oatpp::Int64>);
  setMapperMethod(data::mapping::type::__class::UInt64::CLASS_ID, &TreeToObjectMapper::mapPrimitive<oatpp::UInt64>);

  setMapperMethod(data::mapping::type::__class::Float32::CLASS_ID, &TreeToObjectMapper::mapPrimitive<oatpp::Float32>);
  setMapperMethod(data::mapping::type::__class::Float64::CLASS_ID, &TreeToObjectMapper::mapPrimitive<oatpp::Float64>);
  setMapperMethod(data::mapping::type::__class::Boolean::CLASS_ID, &TreeToObjectMapper::mapPrimitive<oatpp::Boolean>);

  setMapperMethod(data::mapping::type::__class::AbstractObject::CLASS_ID, &TreeToObjectMapper::mapObject);
  setMapperMethod(data::mapping::type::__class::AbstractEnum::CLASS_ID, &TreeToObjectMapper::mapEnum);

  setMapperMethod(data::mapping::type::__class::AbstractVector::CLASS_ID, &TreeToObjectMapper::mapCollection);
  setMapperMethod(data::mapping::type::__class::AbstractList::CLASS_ID, &TreeToObjectMapper::mapCollection);
  setMapperMethod(data::mapping::type::__class::AbstractUnorderedSet::CLASS_ID, &TreeToObjectMapper::mapCollection);

  setMapperMethod(data::mapping::type::__class::AbstractPairList::CLASS_ID, &TreeToObjectMapper::mapMap);
  setMapperMethod(data::mapping::type::__class::AbstractUnorderedMap::CLASS_ID, &TreeToObjectMapper::mapMap);

}

void TreeToObjectMapper::setMapperMethod(const data::mapping::type::ClassId& classId, MapperMethod method) {
  const auto id = static_cast<v_uint32>(classId.id);
  if(id >= m_methods.size()) {
    m_methods.resize(id + 1, nullptr);
  }
  m_methods[id] = method;
}

oatpp::Void TreeToObjectMapper::map(MappingState& state, const Type* const type) const {
  auto id = static_cast<v_uint32>(type->classId.id);
  auto& method = m_methods[id];
  if(method) {
    return (*method)(this, state, type);
  } else {

    auto* interpretation = type->findInterpretation(state.config->enabledInterpretations);
    if(interpretation) {
      return interpretation->fromInterpretation(map(state, interpretation->getInterpretationType()));
    }

    state.errorStack.emplace_back("[oatpp::data::TreeToObjectMapper::map()]: "
                                  "Error. No map method for type '" + std::string(type->classId.name) + "'");
    return nullptr;

  }
}

const Type* TreeToObjectMapper::guessType(const Tree& node) {

  switch (node.getType()) {

    case Tree::Type::UNDEFINED:
    case Tree::Type::NULL_VALUE: return nullptr;

    case Tree::Type::INTEGER: return Int64::Class::getType();
    case Tree::Type::FLOAT: return Float64::Class::getType();

    case Tree::Type::BOOL: return Boolean::Class::getType();
    case Tree::Type::INT_8: return Int8::Class::getType();
    case Tree::Type::UINT_8: return UInt8::Class::getType();
    case Tree::Type::INT_16: return Int16::Class::getType();
    case Tree::Type::UINT_16: return UInt16::Class::getType();
    case Tree::Type::INT_32: return Int32::Class::getType();
    case Tree::Type::UINT_32: return UInt32::Class::getType();
    case Tree::Type::INT_64: return Int64::Class::getType();
    case Tree::Type::UINT_64: return UInt64::Class::getType();
    case Tree::Type::FLOAT_32: return Float32::Class::getType();
    case Tree::Type::FLOAT_64: return Float64::Class::getType();

    case Tree::Type::STRING: return String::Class::getType();
    case Tree::Type::VECTOR: return Vector<oatpp::Any>::Class::getType();
    case Tree::Type::MAP: return Fields<oatpp::Any>::Class::getType();

    default: return nullptr;

  }

}

oatpp::Void TreeToObjectMapper::mapString(const TreeToObjectMapper* mapper, MappingState& state, const Type* const type) {

  (void) mapper;
  (void) type;

  if(state.tree->isNull()){
    return oatpp::Void(String::Class::getType());
  }

  if(state.tree->getType() == Tree::Type::STRING) {
    return state.tree->getString();
  }

  state.errorStack.emplace_back("[oatpp::data::TreeToObjectMapper::mapString()]: Node is NOT a STRING");
  return nullptr;

}

oatpp::Void TreeToObjectMapper::mapAny(const TreeToObjectMapper* mapper, MappingState& state, const Type* const type) {
  (void) type;
  if(state.tree->isNull()){
    return oatpp::Void(Any::Class::getType());
  } else {
    const Type* const fieldType = guessType(*state.tree);
    if(fieldType != nullptr) {
      auto fieldValue = mapper->map(state, fieldType);
      auto anyHandle = std::make_shared<data::mapping::type::AnyHandle>(fieldValue.getPtr(), fieldValue.getValueType());
      return oatpp::Void(anyHandle, Any::Class::getType());
    }
  }
  return oatpp::Void(Any::Class::getType());
}

oatpp::Void TreeToObjectMapper::mapEnum(const TreeToObjectMapper* mapper, MappingState& state, const Type* const type) {

  auto polymorphicDispatcher = static_cast<const data::mapping::type::__class::AbstractEnum::PolymorphicDispatcher*>(
    type->polymorphicDispatcher
  );

  data::mapping::type::EnumInterpreterError e = data::mapping::type::EnumInterpreterError::OK;
  const auto& value = mapper->map(state, polymorphicDispatcher->getInterpretationType());
  if(!state.errorStack.empty()) {
    state.errorStack.emplace_back("[oatpp::data::TreeToObjectMapper::mapEnum()]");
    return nullptr;
  }
  const auto& result = polymorphicDispatcher->fromInterpretation(value, e);

  if(e == data::mapping::type::EnumInterpreterError::OK) {
    return result;
  }

  switch(e) {
    case data::mapping::type::EnumInterpreterError::CONSTRAINT_NOT_NULL:
      state.errorStack.emplace_back("[oatpp::data::TreeToObjectMapper::mapEnum()]: Error. Enum constraint violated - 'NotNull'.");
      break;
    case data::mapping::type::EnumInterpreterError::OK:
    case data::mapping::type::EnumInterpreterError::TYPE_MISMATCH_ENUM:
    case data::mapping::type::EnumInterpreterError::TYPE_MISMATCH_ENUM_VALUE:
    case data::mapping::type::EnumInterpreterError::ENTRY_NOT_FOUND:
    default:
      state.errorStack.emplace_back("[oatpp::data::TreeToObjectMapper::mapEnum()]: Error. Can't map Enum.");
  }

  return nullptr;

}

oatpp::Void TreeToObjectMapper::mapCollection(const TreeToObjectMapper* mapper, MappingState& state, const Type* const type) {

  if(state.tree->isNull()){
    return oatpp::Void(type);
  }

  if(state.tree->getType() != Tree::Type::VECTOR) {
    state.errorStack.emplace_back("[oatpp::data::TreeToObjectMapper::mapCollection()]: Node is NOT a VECTOR.");
    return nullptr;
  }

  auto dispatcher = static_cast<const data::mapping::type::__class::Collection::PolymorphicDispatcher*>(type->polymorphicDispatcher);
  auto collection = dispatcher->createObject();

  auto itemType = dispatcher->getItemType();

  const auto& vector = state.tree->getVector();

  v_int64 index = 0;

  for(const auto& node : vector) {

    MappingState nestedState;
    nestedState.tree = &node;
    nestedState.config = state.config;

    auto item = mapper->map(nestedState, itemType);

    if(!nestedState.errorStack.empty()) {
      state.errorStack.splice(state.errorStack.end(), nestedState.errorStack);
      state.errorStack.emplace_back("[oatpp::data::TreeToObjectMapper::mapCollection()]: index=" + utils::Conversion::int64ToStr(index));
      return nullptr;
    }

    dispatcher->addItem(collection, item);

    index ++;

  }

  return collection;

}

oatpp::Void TreeToObjectMapper::mapMap(const TreeToObjectMapper* mapper, MappingState& state, const Type* const type) {

  if(state.tree->isNull()){
    return oatpp::Void(type);
  }

  if(state.tree->getType() != Tree::Type::MAP) {
    state.errorStack.emplace_back("[oatpp::data::TreeToObjectMapper::mapMap()]: Node is NOT a MAP.");
    return nullptr;
  }

  auto dispatcher = static_cast<const data::mapping::type::__class::Map::PolymorphicDispatcher*>(type->polymorphicDispatcher);
  auto map = dispatcher->createObject();

  auto keyType = dispatcher->getKeyType();
  if(keyType->classId != oatpp::String::Class::CLASS_ID){
    state.errorStack.emplace_back("[oatpp::data::TreeToObjectMapper::mapMap()]: Invalid map key. Key should be String");
    return nullptr;
  }
  auto valueType = dispatcher->getValueType();

  const auto& treeMap = state.tree->getMap();
  auto treeMapSize = treeMap.size();

  for(v_uint64 i = 0; i < treeMapSize; i ++) {

    const auto& node = treeMap[i];

    MappingState nestedState;
    nestedState.tree = &node.second.get();
    nestedState.config = state.config;

    auto item = mapper->map(nestedState, valueType);

    if(!nestedState.errorStack.empty()) {
      state.errorStack.splice(state.errorStack.end(), nestedState.errorStack);
      state.errorStack.emplace_back("[oatpp::data::TreeToObjectMapper::mapMap()]: key='" + node.first + "'");
      return nullptr;
    }

    dispatcher->addItem(map, node.first, item);

  }

  return map;

}

oatpp::Void TreeToObjectMapper::mapObject(const TreeToObjectMapper* mapper, MappingState& state, const Type* const type) {

  if(state.tree->isNull()){
    return oatpp::Void(type);
  }

  if(state.tree->getType() != Tree::Type::MAP) {
    state.errorStack.emplace_back("[oatpp::data::TreeToObjectMapper::mapObject()]: Node is NOT a MAP.");
    return nullptr;
  }

  auto dispatcher = static_cast<const oatpp::data::mapping::type::__class::AbstractObject::PolymorphicDispatcher*>(type->polymorphicDispatcher);
  auto object = dispatcher->createObject();
  const auto& fieldsMap = dispatcher->getProperties()->getMap();

  std::vector<std::pair<oatpp::BaseObject::Property*, const Tree*>> polymorphs;

  const auto& treeMap = state.tree->getMap();
  auto treeMapSize = treeMap.size();

  for(v_uint64 i = 0; i < treeMapSize; i ++) {

    const auto& node = treeMap[i];

    auto fieldIterator = fieldsMap.find(node.first);
    if(fieldIterator != fieldsMap.end()){

      auto field = fieldIterator->second;

      if(field->info.typeSelector && field->type == oatpp::Any::Class::getType()) {
        polymorphs.emplace_back(field, &node.second.get()); // store polymorphs for later processing.
      } else {

        MappingState nestedState;
        nestedState.tree = &node.second.get();
        nestedState.config = state.config;

        auto value = mapper->map(nestedState, field->type);

        if(!nestedState.errorStack.empty()) {
          state.errorStack.splice(state.errorStack.end(), nestedState.errorStack);
          state.errorStack.emplace_back("[oatpp::data::TreeToObjectMapper::mapObject()]: field='" + node.first + "'");
          return nullptr;
        }

        if(field->info.required && value == nullptr) {
          state.errorStack.emplace_back("[oatpp::data::TreeToObjectMapper::mapObject()]: Error. "
                                        + oatpp::String(type->nameQualifier) + "::"
                                        + oatpp::String(field->name) + " is required!");
          return nullptr;
        }
        field->set(static_cast<oatpp::BaseObject *>(object.get()), value);
      }

    } else if (!state.config->allowUnknownFields) {
      state.errorStack.emplace_back("[oatpp::data::TreeToObjectMapper::mapObject()]: Error. Unknown field '" + node.first + "'");
      return nullptr;
    }

  }

  for(auto& p : polymorphs) {
    auto selectedType = p.first->info.typeSelector->selectType(static_cast<oatpp::BaseObject *>(object.get()));

    MappingState nestedState;
    nestedState.tree = p.second;
    nestedState.config = state.config;

    auto value = mapper->map(nestedState, selectedType);

    if(!nestedState.errorStack.empty()) {
      state.errorStack.splice(state.errorStack.end(), nestedState.errorStack);
      state.errorStack.emplace_back("[oatpp::data::TreeToObjectMapper::mapObject()]: field='" + oatpp::String(p.first->name) + "'");
      return nullptr;
    }

    if(p.first->info.required && value == nullptr) {
      state.errorStack.emplace_back("[oatpp::data::TreeToObjectMapper::mapObject()]: Error. "
                                    + oatpp::String(type->nameQualifier) + "::"
                                    + oatpp::String(p.first->name) + " is required!");
      return nullptr;
    }

    oatpp::Any any(value);
    p.first->set(static_cast<oatpp::BaseObject *>(object.get()), oatpp::Void(any.getPtr(), p.first->type));

  }

  return object;

}

}}}
