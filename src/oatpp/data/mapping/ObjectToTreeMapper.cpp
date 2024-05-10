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

#include "ObjectToTreeMapper.hpp"

#include "oatpp/data/stream/BufferStream.hpp"
#include "oatpp/utils/Conversion.hpp"

namespace oatpp { namespace data { namespace mapping {

ObjectToTreeMapper::ObjectToTreeMapper() {

  m_methods.resize(static_cast<size_t>(data::type::ClassId::getClassCount()), nullptr);

  setMapperMethod(data::type::__class::String::CLASS_ID, &ObjectToTreeMapper::mapString);
  setMapperMethod(data::type::__class::Tree::CLASS_ID, &ObjectToTreeMapper::mapTree);
  setMapperMethod(data::type::__class::Any::CLASS_ID, &ObjectToTreeMapper::mapAny);

  setMapperMethod(data::type::__class::Int8::CLASS_ID, &ObjectToTreeMapper::mapPrimitive<oatpp::Int8>);
  setMapperMethod(data::type::__class::UInt8::CLASS_ID, &ObjectToTreeMapper::mapPrimitive<oatpp::UInt8>);

  setMapperMethod(data::type::__class::Int16::CLASS_ID, &ObjectToTreeMapper::mapPrimitive<oatpp::Int16>);
  setMapperMethod(data::type::__class::UInt16::CLASS_ID, &ObjectToTreeMapper::mapPrimitive<oatpp::UInt16>);

  setMapperMethod(data::type::__class::Int32::CLASS_ID, &ObjectToTreeMapper::mapPrimitive<oatpp::Int32>);
  setMapperMethod(data::type::__class::UInt32::CLASS_ID, &ObjectToTreeMapper::mapPrimitive<oatpp::UInt32>);

  setMapperMethod(data::type::__class::Int64::CLASS_ID, &ObjectToTreeMapper::mapPrimitive<oatpp::Int64>);
  setMapperMethod(data::type::__class::UInt64::CLASS_ID, &ObjectToTreeMapper::mapPrimitive<oatpp::UInt64>);

  setMapperMethod(data::type::__class::Float32::CLASS_ID, &ObjectToTreeMapper::mapPrimitive<oatpp::Float32>);
  setMapperMethod(data::type::__class::Float64::CLASS_ID, &ObjectToTreeMapper::mapPrimitive<oatpp::Float64>);
  setMapperMethod(data::type::__class::Boolean::CLASS_ID, &ObjectToTreeMapper::mapPrimitive<oatpp::Boolean>);

  setMapperMethod(data::type::__class::AbstractObject::CLASS_ID, &ObjectToTreeMapper::mapObject);
  setMapperMethod(data::type::__class::AbstractEnum::CLASS_ID, &ObjectToTreeMapper::mapEnum);

  setMapperMethod(data::type::__class::AbstractVector::CLASS_ID, &ObjectToTreeMapper::mapCollection);
  setMapperMethod(data::type::__class::AbstractList::CLASS_ID, &ObjectToTreeMapper::mapCollection);
  setMapperMethod(data::type::__class::AbstractUnorderedSet::CLASS_ID, &ObjectToTreeMapper::mapCollection);

  setMapperMethod(data::type::__class::AbstractPairList::CLASS_ID, &ObjectToTreeMapper::mapMap);
  setMapperMethod(data::type::__class::AbstractUnorderedMap::CLASS_ID, &ObjectToTreeMapper::mapMap);

}

void ObjectToTreeMapper::setMapperMethod(const data::type::ClassId& classId, MapperMethod method) {
  const auto id = static_cast<v_uint32>(classId.id);
  if(id >= m_methods.size()) {
    m_methods.resize(id + 1, nullptr);
  }
  m_methods[id] = method;
}

void ObjectToTreeMapper::map(State& state, const oatpp::Void& polymorph) const
{
  auto id = static_cast<v_uint32>(polymorph.getValueType()->classId.id);
  auto& method = m_methods[id];
  if(method) {
    (*method)(this, state, polymorph);
  } else {
    auto* interpretation = polymorph.getValueType()->findInterpretation(state.config->enabledInterpretations);
    if(interpretation) {
      map(state, interpretation->toInterpretation(polymorph));
    } else {

      state.errorStack.push("[oatpp::data::mapping::ObjectToTreeMapper::map()]: "
                            "Error. No serialize method for type '" +
                            oatpp::String(polymorph.getValueType()->classId.name) + "'");

      return;
    }
  }
}

void ObjectToTreeMapper::mapString(const ObjectToTreeMapper* mapper, State& state, const oatpp::Void& polymorph) {
  if(!polymorph) {
    state.tree->setNull();
    return;
  }
  state.tree->setString(oatpp::String(std::static_pointer_cast<std::string>(polymorph.getPtr()), oatpp::String::Class::getType()));
}

void ObjectToTreeMapper::mapTree(const ObjectToTreeMapper* mapper, State& state, const oatpp::Void& polymorph) {
  if(!polymorph) {
    state.tree->setNull();
    return;
  }
  auto tree = static_cast<mapping::Tree*>(polymorph.get());
  *state.tree = *tree; // copy
}

void ObjectToTreeMapper::mapAny(const ObjectToTreeMapper* mapper, State& state, const oatpp::Void& polymorph) {
  if(!polymorph) {
    state.tree->setNull();
    return;
  }
  auto anyHandle = static_cast<data::type::AnyHandle*>(polymorph.get());
  mapper->map(state, oatpp::Void(anyHandle->ptr, anyHandle->type));
}

void ObjectToTreeMapper::mapEnum(const ObjectToTreeMapper* mapper, State& state, const oatpp::Void& polymorph) {

  auto polymorphicDispatcher = static_cast<const data::type::__class::AbstractEnum::PolymorphicDispatcher*>(
    polymorph.getValueType()->polymorphicDispatcher
  );

  data::type::EnumInterpreterError e = data::type::EnumInterpreterError::OK;
  mapper->map(state, polymorphicDispatcher->toInterpretation(polymorph, state.config->useUnqualifiedEnumNames, e));

  if(e == data::type::EnumInterpreterError::OK) {
    return;
  }

  switch(e) {
    case data::type::EnumInterpreterError::CONSTRAINT_NOT_NULL:
      state.errorStack.push("[oatpp::data::mapping::ObjectToTreeMapper::mapEnum()]: Error. Enum constraint violated - 'NotNull'.");
      break;
    case data::type::EnumInterpreterError::OK:
    case data::type::EnumInterpreterError::TYPE_MISMATCH_ENUM:
    case data::type::EnumInterpreterError::TYPE_MISMATCH_ENUM_VALUE:
    case data::type::EnumInterpreterError::ENTRY_NOT_FOUND:
    default:
      state.errorStack.push("[oatpp::data::mapping::ObjectToTreeMapper::mapEnum()]: Error. Can't serialize Enum.");
  }

}

void ObjectToTreeMapper::mapCollection(const ObjectToTreeMapper* mapper, State& state, const oatpp::Void& polymorph) {

  if(!polymorph) {
    state.tree->setNull();
    return;
  }

  auto dispatcher = static_cast<const data::type::__class::Collection::PolymorphicDispatcher*>(
    polymorph.getValueType()->polymorphicDispatcher
  );

  auto iterator = dispatcher->beginIteration(polymorph);

  state.tree->setVector(0);
  TreeChildrenOperator childrenOperator(*state.tree);
  v_int64 index = 0;

  while (!iterator->finished()) {

    const auto& value = iterator->get();

    if(value || state.config->includeNullFields || state.config->alwaysIncludeNullCollectionElements) {

      State nestedState;
      nestedState.tree = childrenOperator.putItem({});
      nestedState.config = state.config;

      mapper->map(nestedState, value);

      if(!nestedState.errorStack.empty()) {
        state.errorStack.splice(nestedState.errorStack);
        state.errorStack.push("[oatpp::data::mapping::ObjectToTreeMapper::mapCollection()]: index=" + utils::Conversion::int64ToStr(index));
        return;
      }

    }

    iterator->next();
    index ++;

  }

}

void ObjectToTreeMapper::mapMap(const ObjectToTreeMapper* mapper, State& state, const oatpp::Void& polymorph) {

  if(!polymorph) {
    state.tree->setNull();
    return;
  }

  auto dispatcher = static_cast<const data::type::__class::Map::PolymorphicDispatcher*>(
    polymorph.getValueType()->polymorphicDispatcher
  );

  auto keyType = dispatcher->getKeyType();
  if(keyType->classId != oatpp::String::Class::CLASS_ID){
    state.errorStack.push("[oatpp::data::mapping::ObjectToTreeMapper::mapMap()]: Invalid map key. Key should be String");
    return;
  }

  auto iterator = dispatcher->beginIteration(polymorph);

  if(polymorph.getValueType()->classId == data::type::__class::AbstractUnorderedMap::CLASS_ID) {
    state.tree->setMap({});
  } else if(polymorph.getValueType()->classId == data::type::__class::AbstractPairList::CLASS_ID) {
    state.tree->setPairs({});
  } else {
    state.errorStack.push("[oatpp::data::mapping::ObjectToTreeMapper::mapMap()]: Invalid polymorph type");
  }

  TreeChildrenOperator childrenOperator(*state.tree);

  while (!iterator->finished()) {
    const auto& value = iterator->getValue();
    if(value || state.config->includeNullFields || state.config->alwaysIncludeNullCollectionElements) {

      const auto& untypedKey = iterator->getKey();
      const auto& key = oatpp::String(std::static_pointer_cast<std::string>(untypedKey.getPtr()));

      State nestedState;
      nestedState.tree = childrenOperator.putPair(key, {});
      nestedState.config = state.config;

      mapper->map(nestedState, value);

      if(!nestedState.errorStack.empty()) {
        state.errorStack.splice(nestedState.errorStack);
        state.errorStack.push("[oatpp::data::mapping::ObjectToTreeMapper::mapMap()]: key='" + key + "'");
        return;
      }

    }
    iterator->next();
  }

}

void ObjectToTreeMapper::mapObject(const ObjectToTreeMapper* mapper, State& state, const oatpp::Void& polymorph) {

  if(!polymorph) {
    state.tree->setNull();
    return;
  }

  auto type = polymorph.getValueType();
  auto dispatcher = static_cast<const oatpp::data::type::__class::AbstractObject::PolymorphicDispatcher*>(
    type->polymorphicDispatcher
  );
  auto fields = dispatcher->getProperties()->getList();
  auto object = static_cast<oatpp::BaseObject*>(polymorph.get());

  state.tree->setMap({});
  TreeChildrenOperator childrenOperator(*state.tree);

  for (auto const& field : fields) {

    oatpp::Void value;
    if(field->info.typeSelector && field->type == oatpp::Any::Class::getType()) {
      const auto& any = field->get(object).cast<oatpp::Any>();
      value = any.retrieve(field->info.typeSelector->selectType(object));
    } else {
      value = field->get(object);
    }

    if(field->info.required && value == nullptr) {
      oatpp::String key;
      state.config->useUnqualifiedFieldNames ? key = field->unqualifiedName : key = field->name;
      state.errorStack.push("[oatpp::data::mapping::ObjectToTreeMapper::mapObject()]: "
                            "Error. " + std::string(type->nameQualifier) + "::"
                            + key + " is required!");
      return;
    }

    if (value || state.config->includeNullFields || (field->info.required && state.config->alwaysIncludeRequired)) {

      oatpp::String key;
      state.config->useUnqualifiedFieldNames ? key = field->unqualifiedName : key = field->name;

      State nestedState;
      nestedState.tree = childrenOperator.putPair(key, {});
      nestedState.config = state.config;

      mapper->map(nestedState, value);

      if(!nestedState.errorStack.empty()) {
        state.errorStack.splice(nestedState.errorStack);
        state.errorStack.push("[oatpp::data::mapping::ObjectToTreeMapper::mapObject()]: field='" + key + "'");
        return;
      }

    }

  }


}

}}}
