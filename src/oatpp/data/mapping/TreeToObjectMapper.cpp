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

namespace oatpp { namespace data { namespace mapping {

TreeToObjectMapper::TreeToObjectMapper() {

  m_methods.resize(static_cast<size_t>(data::type::ClassId::getClassCount()), nullptr);

  setMapperMethod(data::type::__class::String::CLASS_ID, &TreeToObjectMapper::mapString);

  setMapperMethod(data::type::__class::Tree::CLASS_ID, &TreeToObjectMapper::mapTree);
  setMapperMethod(data::type::__class::Any::CLASS_ID, &TreeToObjectMapper::mapAny);

  setMapperMethod(data::type::__class::Int8::CLASS_ID, &TreeToObjectMapper::mapPrimitive<oatpp::Int8>);
  setMapperMethod(data::type::__class::UInt8::CLASS_ID, &TreeToObjectMapper::mapPrimitive<oatpp::UInt8>);

  setMapperMethod(data::type::__class::Int16::CLASS_ID, &TreeToObjectMapper::mapPrimitive<oatpp::Int16>);
  setMapperMethod(data::type::__class::UInt16::CLASS_ID, &TreeToObjectMapper::mapPrimitive<oatpp::UInt16>);

  setMapperMethod(data::type::__class::Int32::CLASS_ID, &TreeToObjectMapper::mapPrimitive<oatpp::Int32>);
  setMapperMethod(data::type::__class::UInt32::CLASS_ID, &TreeToObjectMapper::mapPrimitive<oatpp::UInt32>);

  setMapperMethod(data::type::__class::Int64::CLASS_ID, &TreeToObjectMapper::mapPrimitive<oatpp::Int64>);
  setMapperMethod(data::type::__class::UInt64::CLASS_ID, &TreeToObjectMapper::mapPrimitive<oatpp::UInt64>);

  setMapperMethod(data::type::__class::Float32::CLASS_ID, &TreeToObjectMapper::mapPrimitive<oatpp::Float32>);
  setMapperMethod(data::type::__class::Float64::CLASS_ID, &TreeToObjectMapper::mapPrimitive<oatpp::Float64>);
  setMapperMethod(data::type::__class::Boolean::CLASS_ID, &TreeToObjectMapper::mapPrimitive<oatpp::Boolean>);

  setMapperMethod(data::type::__class::AbstractObject::CLASS_ID, &TreeToObjectMapper::mapObject);
  setMapperMethod(data::type::__class::AbstractEnum::CLASS_ID, &TreeToObjectMapper::mapEnum);

  setMapperMethod(data::type::__class::AbstractVector::CLASS_ID, &TreeToObjectMapper::mapCollection);
  setMapperMethod(data::type::__class::AbstractList::CLASS_ID, &TreeToObjectMapper::mapCollection);
  setMapperMethod(data::type::__class::AbstractUnorderedSet::CLASS_ID, &TreeToObjectMapper::mapCollection);

  setMapperMethod(data::type::__class::AbstractPairList::CLASS_ID, &TreeToObjectMapper::mapMap);
  setMapperMethod(data::type::__class::AbstractUnorderedMap::CLASS_ID, &TreeToObjectMapper::mapMap);

}

void TreeToObjectMapper::setMapperMethod(const data::type::ClassId& classId, MapperMethod method) {
  const auto id = static_cast<v_uint32>(classId.id);
  if(id >= m_methods.size()) {
    m_methods.resize(id + 1, nullptr);
  }
  m_methods[id] = method;
}

TreeToObjectMapper::GuessedPrimitiveType TreeToObjectMapper::guessedPrimitiveType(const oatpp::String& text) {

  if(text->empty()) {
    return GuessedPrimitiveType::NOT_PRIMITIVE;
  }

  bool number = true;
  bool fraction = false;

  auto data = text->data();

  for(v_buff_usize i = 0; i < text->size(); i ++) {

    auto c = data[i];

    if(c == '-' || c == ' ') {
      continue;
    }

    bool digit = c >= '0' && c <= '9';
    bool dot = c == '.' || c == ',';

    if(dot) {
      fraction = true;
    } else if(!digit) {
      number = false;
      break;
    }

  }

  if(number) {
    if(fraction) {
      return GuessedPrimitiveType::FLOAT;
    }
    return GuessedPrimitiveType::INT;
  }

  auto labelCI = data::share::StringKeyLabelCI(text);

  if(labelCI == "true") {
    return GuessedPrimitiveType::BOOL_TRUE;
  }

  if(labelCI == "false") {
    return GuessedPrimitiveType::BOOL_FALSE;
  }

  return GuessedPrimitiveType::NOT_PRIMITIVE;

}

oatpp::Void TreeToObjectMapper::map(State& state, const Type* type) const {
  auto id = static_cast<v_uint32>(type->classId.id);
  auto& method = m_methods[id];
  if(method) {
    return (*method)(this, state, type);
  } else {

    auto* interpretation = type->findInterpretation(state.config->enabledInterpretations);
    if(interpretation) {
      return interpretation->fromInterpretation(map(state, interpretation->getInterpretationType()));
    }

    state.errorStack.push("[oatpp::data::mapping::TreeToObjectMapper::map()]: "
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
    case Tree::Type::PAIRS: return Fields<oatpp::Any>::Class::getType();

    default: return nullptr;

  }

}

oatpp::Void TreeToObjectMapper::mapString(const TreeToObjectMapper* mapper, State& state, const Type* type) {

  (void) mapper;
  (void) type;

  if(state.tree->isString()) {
    return state.tree->getString();
  }

  if(state.tree->isNull()){
    return oatpp::Void(String::Class::getType());
  }

  if(state.config->allowLexicalCasting) {

    data::stream::BufferOutputStream ss(128);

    switch (state.tree->getType()) {

      case Tree::Type::UNDEFINED:
      case Tree::Type::NULL_VALUE:
        break;

      case Tree::Type::INTEGER: {
        ss << state.tree->getInteger();
        return ss.toString();
      }
      case Tree::Type::FLOAT: {
        ss << state.tree->getFloat();
        return ss.toString();
      }

      case Tree::Type::BOOL: {
        ss << state.tree->getPrimitive<bool>();
        return ss.toString();
      }
      case Tree::Type::INT_8: {
        ss << state.tree->getPrimitive<v_int8>();
        return ss.toString();
      }
      case Tree::Type::UINT_8: {
        ss << state.tree->getPrimitive<v_uint8>();
        return ss.toString();
      }
      case Tree::Type::INT_16: {
        ss << state.tree->getPrimitive<v_int16>();
        return ss.toString();
      }
      case Tree::Type::UINT_16: {
        ss << state.tree->getPrimitive<v_uint16>();
        return ss.toString();
      }
      case Tree::Type::INT_32: {
        ss << state.tree->getPrimitive<v_int32>();
        return ss.toString();
      }
      case Tree::Type::UINT_32: {
        ss << state.tree->getPrimitive<v_uint32>();
        return ss.toString();
      }
      case Tree::Type::INT_64: {
        ss << state.tree->getPrimitive<v_int64>();
        return ss.toString();
      }
      case Tree::Type::UINT_64: {
        ss << state.tree->getPrimitive<v_uint64>();
        return ss.toString();
      }
      case Tree::Type::FLOAT_32: {
        ss << state.tree->getPrimitive<v_float32>();
        return ss.toString();
      }
      case Tree::Type::FLOAT_64: {
        ss << state.tree->getPrimitive<v_float64>();
        return ss.toString();
      }

      case Tree::Type::STRING:
      case Tree::Type::VECTOR:
      case Tree::Type::MAP:
      case Tree::Type::PAIRS:

      default:
        break;

    }

  }

  state.errorStack.push("[oatpp::data::mapping::TreeToObjectMapper::mapString()]: Node is NOT a STRING");
  return nullptr;

}

oatpp::Void TreeToObjectMapper::mapTree(const TreeToObjectMapper* mapper, State& state, const Type* type) {
  (void) type;
  (void) mapper;
  return oatpp::Tree(std::make_shared<mapping::Tree>(*state.tree), oatpp::Tree::Class::getType());
}

oatpp::Void TreeToObjectMapper::mapAny(const TreeToObjectMapper* mapper, State& state, const Type* type) {
  (void) type;
  if(state.tree->isNull()){
    return oatpp::Void(Any::Class::getType());
  } else {
    const Type* const fieldType = guessType(*state.tree);
    if(fieldType != nullptr) {
      auto fieldValue = mapper->map(state, fieldType);
      auto anyHandle = std::make_shared<data::type::AnyHandle>(fieldValue.getPtr(), fieldValue.getValueType());
      return oatpp::Void(anyHandle, Any::Class::getType());
    }
  }
  return oatpp::Void(Any::Class::getType());
}

oatpp::Void TreeToObjectMapper::mapEnum(const TreeToObjectMapper* mapper, State& state, const Type* type) {

  auto polymorphicDispatcher = static_cast<const data::type::__class::AbstractEnum::PolymorphicDispatcher*>(
    type->polymorphicDispatcher
  );

  data::type::EnumInterpreterError e = data::type::EnumInterpreterError::OK;
  const auto& value = mapper->map(state, polymorphicDispatcher->getInterpretationType());
  if(!state.errorStack.empty()) {
    state.errorStack.push("[oatpp::data::mapping::TreeToObjectMapper::mapEnum()]");
    return nullptr;
  }
  const auto& result = polymorphicDispatcher->fromInterpretation(value, state.config->useUnqualifiedEnumNames, e);

  if(e == data::type::EnumInterpreterError::OK) {
    return result;
  }

  switch(e) {
    case data::type::EnumInterpreterError::CONSTRAINT_NOT_NULL:
      state.errorStack.push("[oatpp::data::mapping::TreeToObjectMapper::mapEnum()]: Error. Enum constraint violated - 'NotNull'.");
      break;
    case data::type::EnumInterpreterError::OK:
    case data::type::EnumInterpreterError::TYPE_MISMATCH_ENUM:
    case data::type::EnumInterpreterError::TYPE_MISMATCH_ENUM_VALUE:
    case data::type::EnumInterpreterError::ENTRY_NOT_FOUND:
    default:
      state.errorStack.push("[oatpp::data::mapping::TreeToObjectMapper::mapEnum()]: Error. Can't map Enum.");
  }

  return nullptr;

}

oatpp::Void TreeToObjectMapper::mapCollection(const TreeToObjectMapper* mapper, State& state, const Type* type) {

  if(state.tree->isNull()){
    return oatpp::Void(type);
  }

  auto dispatcher = static_cast<const data::type::__class::Collection::PolymorphicDispatcher*>(type->polymorphicDispatcher);
  auto collection = dispatcher->createObject();

  auto itemType = dispatcher->getItemType();

  const TreeChildrenOperator childrenOperator(*state.tree);
  v_uint64 childrenCount = childrenOperator.size();

  State nestedState;
  nestedState.config = state.config;

  for(v_uint64 index = 0; index < childrenCount; index ++) {

    nestedState.tree = childrenOperator.getItem(index);
    auto item = mapper->map(nestedState, itemType);

    if(!nestedState.errorStack.empty()) {
      state.errorStack.splice(nestedState.errorStack);
      state.errorStack.push("[oatpp::data::mapping::TreeToObjectMapper::mapCollection()]: index=" + utils::Conversion::uint64ToStr(index));
      return nullptr;
    }

    dispatcher->addItem(collection, item);

  }

  return collection;

}

oatpp::Void TreeToObjectMapper::mapMap(const TreeToObjectMapper* mapper, State& state, const Type* type) {

  if(state.tree->isNull()){
    return oatpp::Void(type);
  }

  auto dispatcher = static_cast<const data::type::__class::Map::PolymorphicDispatcher*>(type->polymorphicDispatcher);
  auto map = dispatcher->createObject();

  auto keyType = dispatcher->getKeyType();
  if(keyType->classId != oatpp::String::Class::CLASS_ID){
    state.errorStack.push("[oatpp::data::mapping::TreeToObjectMapper::mapMap()]: Invalid map key. Key should be String");
    return nullptr;
  }
  auto valueType = dispatcher->getValueType();

  const TreeChildrenOperator childrenOperator(*state.tree);
  v_uint64 childrenCount = childrenOperator.size();

  State nestedState;
  nestedState.config = state.config;

  for(v_uint64 i = 0; i < childrenCount; i ++) {

    const auto& pair = childrenOperator.getPair(i);

    nestedState.tree = pair.second;

    auto item = mapper->map(nestedState, valueType);

    if(!nestedState.errorStack.empty()) {
      state.errorStack.splice(nestedState.errorStack);
      state.errorStack.push("[oatpp::data::mapping::TreeToObjectMapper::mapMap()]: key='" + pair.first + "'");
      return nullptr;
    }

    dispatcher->addItem(map, pair.first, item);

  }

  return map;

}

oatpp::Void TreeToObjectMapper::mapObject(const TreeToObjectMapper* mapper, State& state, const Type* type) {

  if(state.tree->isNull()){
    return oatpp::Void(type);
  }

  auto dispatcher = static_cast<const oatpp::data::type::__class::AbstractObject::PolymorphicDispatcher*>(type->polymorphicDispatcher);
  auto object = dispatcher->createObject();
  const std::unordered_map<std::string, BaseObject::Property*>* fieldsMap;

  if(state.config->useUnqualifiedFieldNames) {
    fieldsMap = std::addressof(dispatcher->getProperties()->getUnqualifiedMap());
  } else {
    fieldsMap = std::addressof(dispatcher->getProperties()->getMap());
  }

  std::vector<std::pair<oatpp::BaseObject::Property*, const Tree*>> polymorphs;

  const TreeChildrenOperator childrenOperator(*state.tree);
  v_uint64 childrenCount = childrenOperator.size();

  for(v_uint64 i = 0; i < childrenCount; i ++) {

    const auto& pair = childrenOperator.getPair(i);

    auto fieldIterator = fieldsMap->find(pair.first);
    if(fieldIterator != fieldsMap->end()){

      auto field = fieldIterator->second;

      if(field->info.typeSelector && field->type == oatpp::Any::Class::getType()) {
        polymorphs.emplace_back(field, pair.second); // store polymorphs for later processing.
      } else {

        State nestedState;
        nestedState.tree = pair.second;
        nestedState.config = state.config;

        auto value = mapper->map(nestedState, field->type);

        if(!nestedState.errorStack.empty()) {
          state.errorStack.splice(nestedState.errorStack);
          state.errorStack.push("[oatpp::data::mapping::TreeToObjectMapper::mapObject()]: field='" + pair.first + "'");
          return nullptr;
        }

        if(field->info.required && value == nullptr) {
          state.errorStack.push("[oatpp::data::mapping::TreeToObjectMapper::mapObject()]: Error. " +
                                oatpp::String(type->nameQualifier) + "::" +
                                oatpp::String(field->name) + " is required!");
          return nullptr;
        }
        field->set(static_cast<oatpp::BaseObject *>(object.get()), value);
      }

    } else if (!state.config->allowUnknownFields) {
      state.errorStack.push("[oatpp::data::mapping::TreeToObjectMapper::mapObject()]: Error. Unknown field '" + pair.first + "'");
      return nullptr;
    }

  }

  for(auto& p : polymorphs) {
    auto selectedType = p.first->info.typeSelector->selectType(static_cast<oatpp::BaseObject *>(object.get()));

    State nestedState;
    nestedState.tree = p.second;
    nestedState.config = state.config;

    auto value = mapper->map(nestedState, selectedType);

    if(!nestedState.errorStack.empty()) {
      state.errorStack.splice(nestedState.errorStack);
      state.errorStack.push("[oatpp::data::mapping::TreeToObjectMapper::mapObject()]: field='" + oatpp::String(p.first->name) + "'");
      return nullptr;
    }

    if(p.first->info.required && value == nullptr) {
      state.errorStack.push("[oatpp::data::mapping::TreeToObjectMapper::mapObject()]: Error. " +
                            oatpp::String(type->nameQualifier) + "::" +
                            oatpp::String(p.first->name) + " is required!");
      return nullptr;
    }

    oatpp::Any any(value);
    p.first->set(static_cast<oatpp::BaseObject *>(object.get()), oatpp::Void(any.getPtr(), p.first->type));

  }

  return object;

}

}}}
