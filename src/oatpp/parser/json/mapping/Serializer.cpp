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

#include "Serializer.hpp"

#include "oatpp/parser/json/Utils.hpp"
#include "oatpp/core/data/mapping/type/Any.hpp"

namespace oatpp { namespace parser { namespace json { namespace mapping {

Serializer::Serializer(const std::shared_ptr<Config>& config)
  : m_config(config)
{

  m_methods.resize(data::mapping::type::ClassId::getClassCount(), nullptr);

  setSerializerMethod(data::mapping::type::__class::String::CLASS_ID, &Serializer::serializeString);
  setSerializerMethod(data::mapping::type::__class::Any::CLASS_ID, &Serializer::serializeAny);

  setSerializerMethod(data::mapping::type::__class::Int8::CLASS_ID, &Serializer::serializePrimitive<oatpp::Int8>);
  setSerializerMethod(data::mapping::type::__class::UInt8::CLASS_ID, &Serializer::serializePrimitive<oatpp::UInt8>);

  setSerializerMethod(data::mapping::type::__class::Int16::CLASS_ID, &Serializer::serializePrimitive<oatpp::Int16>);
  setSerializerMethod(data::mapping::type::__class::UInt16::CLASS_ID, &Serializer::serializePrimitive<oatpp::UInt16>);

  setSerializerMethod(data::mapping::type::__class::Int32::CLASS_ID, &Serializer::serializePrimitive<oatpp::Int32>);
  setSerializerMethod(data::mapping::type::__class::UInt32::CLASS_ID, &Serializer::serializePrimitive<oatpp::UInt32>);

  setSerializerMethod(data::mapping::type::__class::Int64::CLASS_ID, &Serializer::serializePrimitive<oatpp::Int64>);
  setSerializerMethod(data::mapping::type::__class::UInt64::CLASS_ID, &Serializer::serializePrimitive<oatpp::UInt64>);

  setSerializerMethod(data::mapping::type::__class::Float32::CLASS_ID, &Serializer::serializePrimitive<oatpp::Float32>);
  setSerializerMethod(data::mapping::type::__class::Float64::CLASS_ID, &Serializer::serializePrimitive<oatpp::Float64>);
  setSerializerMethod(data::mapping::type::__class::Boolean::CLASS_ID, &Serializer::serializePrimitive<oatpp::Boolean>);

  setSerializerMethod(data::mapping::type::__class::AbstractObject::CLASS_ID, &Serializer::serializeObject);
  setSerializerMethod(data::mapping::type::__class::AbstractEnum::CLASS_ID, &Serializer::serializeEnum);

  setSerializerMethod(data::mapping::type::__class::AbstractVector::CLASS_ID, &Serializer::serializeCollection);
  setSerializerMethod(data::mapping::type::__class::AbstractList::CLASS_ID, &Serializer::serializeCollection);
  setSerializerMethod(data::mapping::type::__class::AbstractUnorderedSet::CLASS_ID, &Serializer::serializeCollection);

  setSerializerMethod(data::mapping::type::__class::AbstractPairList::CLASS_ID, &Serializer::serializeMap);
  setSerializerMethod(data::mapping::type::__class::AbstractUnorderedMap::CLASS_ID, &Serializer::serializeMap);

}

void Serializer::setSerializerMethod(const data::mapping::type::ClassId& classId, SerializerMethod method) {
  const v_uint32 id = classId.id;
  if(id >= m_methods.size()) {
    m_methods.resize(id + 1, nullptr);
  }
  m_methods[id] = method;
}

void Serializer::serializeString(data::stream::ConsistentOutputStream* stream, const char* data, v_buff_size size, v_uint32 escapeFlags) {
  auto encodedValue = Utils::escapeString(data, size, escapeFlags);
  stream->writeCharSimple('\"');
  stream->writeSimple(encodedValue);
  stream->writeCharSimple('\"');
}

void Serializer::serializeString(Serializer* serializer,
                                 data::stream::ConsistentOutputStream* stream,
                                 const oatpp::Void& polymorph)
{

  if(!polymorph) {
    stream->writeSimple("null", 4);
    return;
  }

  auto str = static_cast<std::string*>(polymorph.get());

  serializeString(stream, str->data(), str->size(), serializer->m_config->escapeFlags);

}

void Serializer::serializeAny(Serializer* serializer,
                              data::stream::ConsistentOutputStream* stream,
                              const oatpp::Void& polymorph)
{

  if(!polymorph) {
    stream->writeSimple("null", 4);
    return;
  }

  auto anyHandle = static_cast<data::mapping::type::AnyHandle*>(polymorph.get());
  serializer->serialize(stream, oatpp::Void(anyHandle->ptr, anyHandle->type));

}

void Serializer::serializeEnum(Serializer* serializer,
                               data::stream::ConsistentOutputStream* stream,
                               const oatpp::Void& polymorph)
{
  auto polymorphicDispatcher = static_cast<const data::mapping::type::__class::AbstractEnum::PolymorphicDispatcher*>(
    polymorph.getValueType()->polymorphicDispatcher
  );

  data::mapping::type::EnumInterpreterError e = data::mapping::type::EnumInterpreterError::OK;
  serializer->serialize(stream, polymorphicDispatcher->toInterpretation(polymorph, e));

  if(e == data::mapping::type::EnumInterpreterError::OK) {
    return;
  }

  switch(e) {
    case data::mapping::type::EnumInterpreterError::CONSTRAINT_NOT_NULL:
      throw std::runtime_error("[oatpp::parser::json::mapping::Serializer::serializeEnum()]: Error. Enum constraint violated - 'NotNull'.");
    default:
      throw std::runtime_error("[oatpp::parser::json::mapping::Serializer::serializeEnum()]: Error. Can't serialize Enum.");
  }

}

void Serializer::serializeCollection(Serializer* serializer,
                                     data::stream::ConsistentOutputStream* stream,
                                     const oatpp::Void& polymorph)
{

  if(!polymorph) {
    stream->writeSimple("null", 4);
    return;
  }

  auto dispatcher = static_cast<const data::mapping::type::__class::Collection::PolymorphicDispatcher*>(
    polymorph.getValueType()->polymorphicDispatcher
  );

  stream->writeCharSimple('[');
  bool first = true;

  auto iterator = dispatcher->beginIteration(polymorph);

  while (!iterator->finished()) {
    const auto& value = iterator->get();
    if(value || serializer->getConfig()->includeNullFields || serializer->getConfig()->alwaysIncludeNullCollectionElements) {
      (first) ? first = false : stream->writeSimple(",", 1);
      serializer->serialize(stream, value);
    }
    iterator->next();
  }

  stream->writeCharSimple(']');

}

void Serializer::serializeMap(Serializer* serializer,
                              data::stream::ConsistentOutputStream* stream,
                              const oatpp::Void& polymorph)
{

  if(!polymorph) {
    stream->writeSimple("null", 4);
    return;
  }

  auto dispatcher = static_cast<const data::mapping::type::__class::Map::PolymorphicDispatcher*>(
    polymorph.getValueType()->polymorphicDispatcher
  );

  auto keyType = dispatcher->getKeyType();
  if(keyType->classId != oatpp::String::Class::CLASS_ID){
    throw std::runtime_error("[oatpp::parser::json::mapping::Serializer::serializeMap()]: Invalid json map key. Key should be String");
  }

  stream->writeCharSimple('{');
  bool first = true;

  auto iterator = dispatcher->beginIteration(polymorph);

  while (!iterator->finished()) {
    const auto& value = iterator->getValue();
    if(value || serializer->m_config->includeNullFields || serializer->m_config->alwaysIncludeNullCollectionElements) {
      (first) ? first = false : stream->writeSimple(",", 1);
      const auto& untypedKey = iterator->getKey();
      const auto& key = oatpp::String(std::static_pointer_cast<std::string>(untypedKey.getPtr()));
      serializeString(stream, key->data(), key->size(), serializer->m_config->escapeFlags);
      stream->writeSimple(":", 1);
      serializer->serialize(stream, value);
    }
    iterator->next();
  }

  stream->writeCharSimple('}');

}

void Serializer::serializeObject(Serializer* serializer,
                                  data::stream::ConsistentOutputStream* stream,
                                  const oatpp::Void& polymorph)
{

  if(!polymorph) {
    stream->writeSimple("null", 4);
    return;
  }

  stream->writeCharSimple('{');

  bool first = true;
  auto dispatcher = static_cast<const oatpp::data::mapping::type::__class::AbstractObject::PolymorphicDispatcher*>(
    polymorph.getValueType()->polymorphicDispatcher
  );
  auto fields = dispatcher->getProperties()->getList();
  auto object = static_cast<oatpp::BaseObject*>(polymorph.get());
  auto config = serializer->m_config;

  for (auto const& field : fields) {

    oatpp::Void value;
    if(field->info.typeSelector && field->type == oatpp::Any::Class::getType()) {
      const auto& any = field->get(object).cast<oatpp::Any>();
      value = any.retrieve(field->info.typeSelector->selectType(object));
    } else {
      value = field->get(object);
    }

    if (value || config->includeNullFields || (field->info.required && config->alwaysIncludeRequired)) {
      (first) ? first = false : stream->writeSimple(",", 1);
      serializeString(stream, field->name, std::strlen(field->name), serializer->m_config->escapeFlags);
      stream->writeSimple(":", 1);
      serializer->serialize(stream, value);
    }

  }

  stream->writeCharSimple('}');

}

void Serializer::serialize(data::stream::ConsistentOutputStream* stream,
                           const oatpp::Void& polymorph)
{
  auto id = polymorph.getValueType()->classId.id;
  auto& method = m_methods[id];
  if(method) {
    (*method)(this, stream, polymorph);
  } else {

    auto* interpretation = polymorph.getValueType()->findInterpretation(m_config->enabledInterpretations);
    if(interpretation) {
      serialize(stream, interpretation->toInterpretation(polymorph));
    } else {
      throw std::runtime_error("[oatpp::parser::json::mapping::Serializer::serialize()]: "
                               "Error. No serialize method for type '" +
                               std::string(polymorph.getValueType()->classId.name) + "'");
    }

  }
}

void Serializer::serializeToStream(data::stream::ConsistentOutputStream* stream,
                                   const oatpp::Void& polymorph)
{
  if(m_config->useBeautifier) {
    json::Beautifier beautifier(stream, "  ", "\n");
    serialize(&beautifier, polymorph);
  } else {
    serialize(stream, polymorph);
  }
}

const std::shared_ptr<Serializer::Config>& Serializer::getConfig() {
  return m_config;
}

}}}}
