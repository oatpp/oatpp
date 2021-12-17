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

#include "Deserializer.hpp"

#include "oatpp/core/utils/ConversionUtils.hpp"

namespace oatpp { namespace parser { namespace json { namespace mapping {

Deserializer::Deserializer(const std::shared_ptr<Config>& config)
  : m_config(config)
{

  m_methods.resize(data::mapping::type::ClassId::getClassCount(), nullptr);

  setDeserializerMethod(data::mapping::type::__class::String::CLASS_ID, &Deserializer::deserializeString);
  setDeserializerMethod(data::mapping::type::__class::Any::CLASS_ID, &Deserializer::deserializeAny);

  setDeserializerMethod(data::mapping::type::__class::Int8::CLASS_ID, &Deserializer::deserializeInt<oatpp::Int8>);
  setDeserializerMethod(data::mapping::type::__class::UInt8::CLASS_ID, &Deserializer::deserializeUInt<oatpp::UInt8>);

  setDeserializerMethod(data::mapping::type::__class::Int16::CLASS_ID, &Deserializer::deserializeInt<oatpp::Int16>);
  setDeserializerMethod(data::mapping::type::__class::UInt16::CLASS_ID, &Deserializer::deserializeUInt<oatpp::UInt16>);

  setDeserializerMethod(data::mapping::type::__class::Int32::CLASS_ID, &Deserializer::deserializeInt<oatpp::Int32>);
  setDeserializerMethod(data::mapping::type::__class::UInt32::CLASS_ID, &Deserializer::deserializeUInt<oatpp::UInt32>);

  setDeserializerMethod(data::mapping::type::__class::Int64::CLASS_ID, &Deserializer::deserializeInt<oatpp::Int64>);
  setDeserializerMethod(data::mapping::type::__class::UInt64::CLASS_ID, &Deserializer::deserializeUInt<oatpp::UInt64>);

  setDeserializerMethod(data::mapping::type::__class::Float32::CLASS_ID, &Deserializer::deserializeFloat32);
  setDeserializerMethod(data::mapping::type::__class::Float64::CLASS_ID, &Deserializer::deserializeFloat64);
  setDeserializerMethod(data::mapping::type::__class::Boolean::CLASS_ID, &Deserializer::deserializeBoolean);

  setDeserializerMethod(data::mapping::type::__class::AbstractObject::CLASS_ID, &Deserializer::deserializeObject);
  setDeserializerMethod(data::mapping::type::__class::AbstractEnum::CLASS_ID, &Deserializer::deserializeEnum);

  setDeserializerMethod(data::mapping::type::__class::AbstractVector::CLASS_ID, &Deserializer::deserializeCollection);
  setDeserializerMethod(data::mapping::type::__class::AbstractList::CLASS_ID, &Deserializer::deserializeCollection);
  setDeserializerMethod(data::mapping::type::__class::AbstractUnorderedSet::CLASS_ID, &Deserializer::deserializeCollection);

  setDeserializerMethod(data::mapping::type::__class::AbstractPairList::CLASS_ID, &Deserializer::deserializeMap);
  setDeserializerMethod(data::mapping::type::__class::AbstractUnorderedMap::CLASS_ID, &Deserializer::deserializeMap);

}

void Deserializer::setDeserializerMethod(const data::mapping::type::ClassId& classId, DeserializerMethod method) {
  const v_uint32 id = classId.id;
  if(id >= m_methods.size()) {
    m_methods.resize(id + 1, nullptr);
  }
  m_methods[id] = method;
}

void Deserializer::skipScope(oatpp::parser::Caret& caret, v_char8 charOpen, v_char8 charClose){

  const char* data = caret.getData();
  v_buff_size size = caret.getDataSize();
  v_buff_size pos = caret.getPosition();
  v_int32 scopeCounter = 0;

  bool isInString = false;

  while(pos < size){
    v_char8 a = data[pos];
    if(a == charOpen){
      if(!isInString){
        scopeCounter ++;
      }
    } else if(a == charClose){
      if(!isInString){
        scopeCounter --;
        if(scopeCounter == 0){
          caret.setPosition(pos + 1);
          return;
        }
      }
    } else if(a == '"') {
      isInString = !isInString;
    } else if(a == '\\'){
      pos ++;
    }

    pos ++;

  }
}

void Deserializer::skipString(oatpp::parser::Caret& caret){
  const char* data = caret.getData();
  v_buff_size size = caret.getDataSize();
  v_buff_size pos = caret.getPosition();
  v_int32 scopeCounter = 0;
  while(pos < size){
    v_char8 a = data[pos];
    if(a == '"'){
      scopeCounter ++;
      if(scopeCounter == 2) {
        caret.setPosition(pos + 1);
        return;
      }
    } else if(a == '\\'){
      pos ++;
    }
    pos ++;
  }
}

void Deserializer::skipToken(oatpp::parser::Caret& caret){
  const char* data = caret.getData();
  v_buff_size size = caret.getDataSize();
  v_buff_size pos = caret.getPosition();
  while(pos < size){
    v_char8 a = data[pos];
    if(a == ' ' || a == '\t' || a == '\n' || a == '\r' || a == '\b' || a == '\f' ||
       a == '}' || a == ',' || a == ']') {
      caret.setPosition(pos);
      return;
    }
    pos ++;
  }
}

void Deserializer::skipValue(oatpp::parser::Caret& caret){
  if(caret.isAtChar('{')){
    skipScope(caret, '{', '}');
  } else if(caret.isAtChar('[')){
    skipScope(caret, '[', ']');
  } else if(caret.isAtChar('"')){
    skipString(caret);
  } else {
    skipToken(caret);
  }
}

oatpp::Void Deserializer::deserializeFloat32(Deserializer* deserializer, parser::Caret& caret, const Type* const type) {

  (void) deserializer;
  (void) type;

  if(caret.isAtText("null", true)){
    return oatpp::Void(Float32::Class::getType());
  } else {
    return Float32(caret.parseFloat32());
  }
}

oatpp::Void Deserializer::deserializeFloat64(Deserializer* deserializer, parser::Caret& caret, const Type* const type) {

  (void) deserializer;
  (void) type;

  if(caret.isAtText("null", true)){
    return oatpp::Void(Float64::Class::getType());
  } else {
    return Float64(caret.parseFloat64());
  }

}

oatpp::Void Deserializer::deserializeBoolean(Deserializer* deserializer, parser::Caret& caret, const Type* const type) {

  (void) deserializer;
  (void) type;

  if(caret.isAtText("null", true)){
    return oatpp::Void(Boolean::Class::getType());
  } else {
    if(caret.isAtText("true", true)) {
      return Boolean(true);
    } else if(caret.isAtText("false", true)) {
      return Boolean(false);
    } else {
      caret.setError("[oatpp::parser::json::mapping::Deserializer::readBooleanValue()]: Error. 'true' or 'false' - expected.", ERROR_CODE_VALUE_BOOLEAN);
      return oatpp::Void(Boolean::Class::getType());
    }
  }

}

oatpp::Void Deserializer::deserializeString(Deserializer* deserializer, parser::Caret& caret, const Type* const type) {

  (void) deserializer;
  (void) type;

  if(caret.isAtText("null", true)){
    return oatpp::Void(String::Class::getType());
  } else {
    return oatpp::Void(oatpp::parser::json::Utils::parseString(caret).getPtr(), String::Class::getType());
  }
}

const data::mapping::type::Type* Deserializer::guessNumberType(oatpp::parser::Caret& caret) {
  if (!Utils::findDecimalSeparatorInCurrentNumber(caret)) {
    if (*caret.getCurrData() == '-') {
      return Int64::Class::getType();
    } else {
      return UInt64::Class::getType();
    }
  }
  return Float64::Class::getType();
}

const data::mapping::type::Type* Deserializer::guessType(oatpp::parser::Caret& caret) {
  {
    parser::Caret::StateSaveGuard stateGuard(caret);
    v_char8 c = *caret.getCurrData();
    switch (c) {
      case '"':
        return String::Class::getType();
      case '{':
        return oatpp::Fields<Any>::Class::getType();
      case '[':
        return oatpp::List<Any>::Class::getType();
      case 't':
        if(caret.isAtText("true")) return Boolean::Class::getType();
        break;
      case 'f':
        if(caret.isAtText("false")) return Boolean::Class::getType();
        break;
      default:
        if (c == '-' || caret.isAtDigitChar()) {
          return guessNumberType(caret);
        }
    }
  }
  caret.setError("[oatpp::parser::json::mapping::Deserializer::guessType()]: Error. Can't guess type for oatpp::Any.");
  return nullptr;
}

oatpp::Void Deserializer::deserializeAny(Deserializer* deserializer, parser::Caret& caret, const Type* const type) {
  (void) type;
  if(caret.isAtText("null", true)){
    return oatpp::Void(Any::Class::getType());
  } else {
    const Type* const fieldType = guessType(caret);
    if(fieldType != nullptr) {
      auto fieldValue = deserializer->deserialize(caret, fieldType);
      auto anyHandle = std::make_shared<data::mapping::type::AnyHandle>(fieldValue.getPtr(), fieldValue.getValueType());
      return oatpp::Void(anyHandle, Any::Class::getType());
    }
  }
  return oatpp::Void(Any::Class::getType());
}

oatpp::Void Deserializer::deserializeEnum(Deserializer* deserializer, parser::Caret& caret, const Type* const type) {

  auto polymorphicDispatcher = static_cast<const data::mapping::type::__class::AbstractEnum::PolymorphicDispatcher*>(
    type->polymorphicDispatcher
  );

  data::mapping::type::EnumInterpreterError e = data::mapping::type::EnumInterpreterError::OK;
  const auto& value = deserializer->deserialize(caret, polymorphicDispatcher->getInterpretationType());
  if(caret.hasError()) {
    return nullptr;
  }
  const auto& result = polymorphicDispatcher->fromInterpretation(value, e);

  if(e == data::mapping::type::EnumInterpreterError::OK) {
    return result;
  }

  switch(e) {
    case data::mapping::type::EnumInterpreterError::CONSTRAINT_NOT_NULL:
      caret.setError("[oatpp::parser::json::mapping::Deserializer::deserializeEnum()]: Error. Enum constraint violated - 'NotNull'.");
      break;
    default:
      caret.setError("[oatpp::parser::json::mapping::Deserializer::deserializeEnum()]: Error. Can't deserialize Enum.");
  }

  return nullptr;

}

oatpp::Void Deserializer::deserializeCollection(Deserializer* deserializer, parser::Caret& caret, const Type* type) {

  if(caret.isAtText("null", true)){
    return oatpp::Void(type);
  }

  if(caret.canContinueAtChar('[', 1)) {

    auto dispatcher = static_cast<const data::mapping::type::__class::Collection::PolymorphicDispatcher*>(type->polymorphicDispatcher);
    auto collection = dispatcher->createObject();

    auto itemType = dispatcher->getItemType();

    caret.skipBlankChars();

    while(!caret.isAtChar(']') && caret.canContinue()){

      caret.skipBlankChars();
      auto item = deserializer->deserialize(caret, itemType);
      if(caret.hasError()){
        return nullptr;
      }

      dispatcher->addItem(collection, item);
      caret.skipBlankChars();

      caret.canContinueAtChar(',', 1);

    }

    if(!caret.canContinueAtChar(']', 1)){
      if(!caret.hasError()){
        caret.setError("[oatpp::parser::json::mapping::Deserializer::deserializeCollection()]: Error. ']' - expected", ERROR_CODE_ARRAY_SCOPE_CLOSE);
      }
      return nullptr;
    };

    return collection;

  } else {
    caret.setError("[oatpp::parser::json::mapping::Deserializer::deserializeCollection()]: Error. '[' - expected", ERROR_CODE_ARRAY_SCOPE_OPEN);
    return nullptr;
  }

}

oatpp::Void Deserializer::deserializeMap(Deserializer* deserializer, parser::Caret& caret, const Type* const type) {

  if(caret.isAtText("null", true)){
    return oatpp::Void(type);
  }

  if(caret.canContinueAtChar('{', 1)) {

    auto dispatcher = static_cast<const data::mapping::type::__class::Map::PolymorphicDispatcher*>(type->polymorphicDispatcher);
    auto map = dispatcher->createObject();

    auto keyType = dispatcher->getKeyType();
    if(keyType->classId != oatpp::String::Class::CLASS_ID){
      throw std::runtime_error("[oatpp::parser::json::mapping::Deserializer::deserializeMap()]: Invalid json map key. Key should be String");
    }
    auto valueType = dispatcher->getValueType();

    caret.skipBlankChars();

    while (!caret.isAtChar('}') && caret.canContinue()) {

      caret.skipBlankChars();
      auto key = Utils::parseString(caret);
      if(caret.hasError()){
        return nullptr;
      }

      caret.skipBlankChars();
      if(!caret.canContinueAtChar(':', 1)){
        caret.setError("[oatpp::parser::json::mapping::Deserializer::deserializeMap()]: Error. ':' - expected", ERROR_CODE_OBJECT_SCOPE_COLON_MISSING);
        return nullptr;
      }

      caret.skipBlankChars();

      auto item = deserializer->deserialize(caret, valueType);
      if(caret.hasError()){
        return nullptr;
      }
      dispatcher->addItem(map, key, item);

      caret.skipBlankChars();
      caret.canContinueAtChar(',', 1);

    }

    if(!caret.canContinueAtChar('}', 1)){
      if(!caret.hasError()){
        caret.setError("[oatpp::parser::json::mapping::Deserializer::deserializeMap()]: Error. '}' - expected", ERROR_CODE_OBJECT_SCOPE_CLOSE);
      }
      return nullptr;
    }

    return map;

  } else {
    caret.setError("[oatpp::parser::json::mapping::Deserializer::deserializeMap()]: Error. '{' - expected", ERROR_CODE_OBJECT_SCOPE_OPEN);
  }

  return nullptr;

}

oatpp::Void Deserializer::deserializeObject(Deserializer* deserializer, parser::Caret& caret, const Type* const type) {

  if(caret.isAtText("null", true)){
    return oatpp::Void(type);
  }

  if(caret.canContinueAtChar('{', 1)) {

    auto dispatcher = static_cast<const oatpp::data::mapping::type::__class::AbstractObject::PolymorphicDispatcher*>(type->polymorphicDispatcher);
    auto object = dispatcher->createObject();
    const auto& fieldsMap = dispatcher->getProperties()->getMap();

    caret.skipBlankChars();

    std::vector<std::pair<oatpp::BaseObject::Property*, oatpp::String>> polymorphs;
    while (!caret.isAtChar('}') && caret.canContinue()) {

      caret.skipBlankChars();
      auto key = Utils::parseStringToStdString(caret);
      if(caret.hasError()){
        return nullptr;
      }

      auto fieldIterator = fieldsMap.find(key);
      if(fieldIterator != fieldsMap.end()){

        caret.skipBlankChars();
        if(!caret.canContinueAtChar(':', 1)){
          caret.setError("[oatpp::parser::json::mapping::Deserializer::readObject()]: Error. ':' - expected", ERROR_CODE_OBJECT_SCOPE_COLON_MISSING);
          return nullptr;
        }

        caret.skipBlankChars();

        auto field = fieldIterator->second;

        if(field->info.typeSelector && field->type == oatpp::Any::Class::getType()) {
          auto label = caret.putLabel();
          skipValue(caret);
          polymorphs.emplace_back(field, label.toString()); // store polymorphs for later processing.
        } else {
          field->set(static_cast<oatpp::BaseObject *>(object.get()), deserializer->deserialize(caret, field->type));
        }

      } else if (deserializer->getConfig()->allowUnknownFields) {
        caret.skipBlankChars();
        if(!caret.canContinueAtChar(':', 1)){
          caret.setError("[oatpp::parser::json::mapping::Deserializer::readObject()/if(config->allowUnknownFields){}]: Error. ':' - expected", ERROR_CODE_OBJECT_SCOPE_COLON_MISSING);
          return nullptr;
        }
        caret.skipBlankChars();
        skipValue(caret);
      } else {
        caret.setError("[oatpp::parser::json::mapping::Deserializer::readObject()]: Error. Unknown field", ERROR_CODE_OBJECT_SCOPE_UNKNOWN_FIELD);
        return nullptr;
      }

      caret.skipBlankChars();
      caret.canContinueAtChar(',', 1);

    }

    if(!caret.canContinueAtChar('}', 1)){
      if(!caret.hasError()){
        caret.setError("[oatpp::parser::json::mapping::Deserializer::readObject()]: Error. '}' - expected", ERROR_CODE_OBJECT_SCOPE_CLOSE);
      }
      return nullptr;
    }

    for(auto& p : polymorphs) {
      parser::Caret polyCaret(p.second);
      auto selectedType = p.first->info.typeSelector->selectType(static_cast<oatpp::BaseObject *>(object.get()));
      auto value = deserializer->deserialize(polyCaret, selectedType);
      oatpp::Any any(value);
      p.first->set(static_cast<oatpp::BaseObject *>(object.get()), oatpp::Void(any.getPtr(), p.first->type));
    }

    return object;

  } else {
    caret.setError("[oatpp::parser::json::mapping::Deserializer::readObject()]: Error. '{' - expected", ERROR_CODE_OBJECT_SCOPE_OPEN);
  }

  return nullptr;

}

oatpp::Void Deserializer::deserialize(parser::Caret& caret, const Type* const type) {
  auto id = type->classId.id;
  auto& method = m_methods[id];
  if(method) {
    return (*method)(this, caret, type);
  } else {

    auto* interpretation = type->findInterpretation(m_config->enabledInterpretations);
    if(interpretation) {
      return interpretation->fromInterpretation(deserialize(caret, interpretation->getInterpretationType()));
    }

    throw std::runtime_error("[oatpp::parser::json::mapping::Deserializer::deserialize()]: "
                             "Error. No deserialize method for type '" + std::string(type->classId.name) + "'");
  }
}

const std::shared_ptr<Deserializer::Config>& Deserializer::getConfig() {
  return m_config;
}

}}}}
