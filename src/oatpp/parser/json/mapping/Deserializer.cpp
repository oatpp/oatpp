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

#include "oatpp/parser/json/Utils.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"

namespace oatpp { namespace parser { namespace json { namespace mapping {

Deserializer::Deserializer(const std::shared_ptr<Config>& config)
  : m_config(config)
{

  m_methods.resize(data::mapping::type::ClassId::getClassCount(), nullptr);

  setDeserializerMethod(oatpp::data::mapping::type::__class::String::CLASS_ID, &Deserializer::deserializeString);

  setDeserializerMethod(oatpp::data::mapping::type::__class::Int8::CLASS_ID, &Deserializer::deserializeInt<oatpp::Int8>);
  setDeserializerMethod(oatpp::data::mapping::type::__class::UInt8::CLASS_ID, &Deserializer::deserializeUInt<oatpp::UInt8>);

  setDeserializerMethod(oatpp::data::mapping::type::__class::Int16::CLASS_ID, &Deserializer::deserializeInt<oatpp::Int16>);
  setDeserializerMethod(oatpp::data::mapping::type::__class::UInt16::CLASS_ID, &Deserializer::deserializeUInt<oatpp::UInt16>);

  setDeserializerMethod(oatpp::data::mapping::type::__class::Int32::CLASS_ID, &Deserializer::deserializeInt<oatpp::Int32>);
  setDeserializerMethod(oatpp::data::mapping::type::__class::UInt32::CLASS_ID, &Deserializer::deserializeUInt<oatpp::UInt32>);

  setDeserializerMethod(oatpp::data::mapping::type::__class::Int64::CLASS_ID, &Deserializer::deserializeInt<oatpp::Int64>);
  setDeserializerMethod(oatpp::data::mapping::type::__class::UInt64::CLASS_ID, &Deserializer::deserializeUInt<oatpp::UInt64>);

  setDeserializerMethod(oatpp::data::mapping::type::__class::Float32::CLASS_ID, &Deserializer::deserializeFloat32);
  setDeserializerMethod(oatpp::data::mapping::type::__class::Float64::CLASS_ID, &Deserializer::deserializeFloat64);
  setDeserializerMethod(oatpp::data::mapping::type::__class::Boolean::CLASS_ID, &Deserializer::deserializeBoolean);

  setDeserializerMethod(oatpp::data::mapping::type::__class::AbstractList::CLASS_ID, &Deserializer::deserializeList);
  setDeserializerMethod(oatpp::data::mapping::type::__class::AbstractListMap::CLASS_ID, &Deserializer::deserializeFieldsMap);
  setDeserializerMethod(oatpp::data::mapping::type::__class::AbstractObject::CLASS_ID, &Deserializer::deserializeObject);

}

void Deserializer::setDeserializerMethod(const data::mapping::type::ClassId& classId, DeserializerMethod method) {
  auto id = classId.id;
  if(id < m_methods.size()) {
    m_methods[id] = method;
  } else {
    throw std::runtime_error("[oatpp::parser::json::mapping::Deserializer::setDeserializerMethod()]: Error. Unknown classId");
  }
}

void Deserializer::skipScope(oatpp::parser::Caret& caret, v_char8 charOpen, v_char8 charClose){

  p_char8 data = caret.getData();
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
  p_char8 data = caret.getData();
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
  p_char8 data = caret.getData();
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

data::mapping::type::AbstractObjectWrapper Deserializer::deserializeFloat32(Deserializer* deserializer,
                                                                             parser::Caret& caret,
                                                                             const Type* const type)
{

  (void) deserializer;
  (void) type;

  if(caret.isAtText("null", true)){
    return AbstractObjectWrapper(Float32::ObjectWrapper::Class::getType());
  } else {
    return AbstractObjectWrapper(Float32::ObjectType::createAbstract(caret.parseFloat32()), Float32::ObjectWrapper::Class::getType());
  }
}

data::mapping::type::AbstractObjectWrapper Deserializer::deserializeFloat64(Deserializer* deserializer,
                                                                             parser::Caret& caret,
                                                                             const Type* const type)
{

  (void) deserializer;
  (void) type;

  if(caret.isAtText("null", true)){
    return AbstractObjectWrapper(Float64::ObjectWrapper::Class::getType());
  } else {
    return AbstractObjectWrapper(Float64::ObjectType::createAbstract(caret.parseFloat64()), Float64::ObjectWrapper::Class::getType());
  }

}

data::mapping::type::AbstractObjectWrapper Deserializer::deserializeBoolean(Deserializer* deserializer,
                                                                             parser::Caret& caret,
                                                                             const Type* const type)
{

  (void) deserializer;
  (void) type;

  if(caret.isAtText("null", true)){
    return AbstractObjectWrapper(Boolean::ObjectWrapper::Class::getType());
  } else {
    if(caret.isAtText("true", true)) {
      return AbstractObjectWrapper(Boolean::ObjectType::createAbstract(true), Boolean::ObjectWrapper::Class::getType());
    } else if(caret.isAtText("false", true)) {
      return AbstractObjectWrapper(Boolean::ObjectType::createAbstract(false), Boolean::ObjectWrapper::Class::getType());
    } else {
      caret.setError("[oatpp::parser::json::mapping::Deserializer::readBooleanValue()]: Error. 'true' or 'false' - expected.", ERROR_CODE_VALUE_BOOLEAN);
      return AbstractObjectWrapper(Boolean::ObjectWrapper::Class::getType());
    }
  }

}

data::mapping::type::AbstractObjectWrapper Deserializer::deserializeString(Deserializer* deserializer,
                                                                            parser::Caret& caret,
                                                                            const Type* const type)
{

  (void) deserializer;
  (void) type;

  if(caret.isAtText("null", true)){
    return AbstractObjectWrapper(String::Class::getType());
  } else {
    return AbstractObjectWrapper(oatpp::parser::json::Utils::parseString(caret).getPtr(), String::Class::getType());
  }
}

data::mapping::type::AbstractObjectWrapper Deserializer::deserializeList(Deserializer* deserializer,
                                                                          parser::Caret& caret,
                                                                          const Type* const type)
{

  if(caret.isAtText("null", true)){
    return AbstractObjectWrapper(type);
  }

  if(caret.canContinueAtChar('[', 1)) {

    auto listWrapper = type->creator();
    oatpp::data::mapping::type::PolymorphicWrapper<AbstractList>
      list(std::static_pointer_cast<AbstractList>(listWrapper.getPtr()), listWrapper.valueType);

    Type* itemType = *type->params.begin();

    caret.skipBlankChars();

    while(!caret.isAtChar(']') && caret.canContinue()){

      caret.skipBlankChars();
      auto item = deserializer->deserialize(caret, itemType);
      if(caret.hasError()){
        return AbstractObjectWrapper::empty();
      }

      list->addPolymorphicItem(item);
      caret.skipBlankChars();

      caret.canContinueAtChar(',', 1);

    }

    if(!caret.canContinueAtChar(']', 1)){
      if(!caret.hasError()){
        caret.setError("[oatpp::parser::json::mapping::Deserializer::readList()]: Error. ']' - expected", ERROR_CODE_ARRAY_SCOPE_CLOSE);
      }
      return AbstractObjectWrapper::empty();
    };

    return AbstractObjectWrapper(list.getPtr(), list.valueType);
  } else {
    caret.setError("[oatpp::parser::json::mapping::Deserializer::readList()]: Error. '[' - expected", ERROR_CODE_ARRAY_SCOPE_OPEN);
    return AbstractObjectWrapper::empty();
  }

}

data::mapping::type::AbstractObjectWrapper Deserializer::deserializeFieldsMap(Deserializer* deserializer,
                                                                               parser::Caret& caret,
                                                                               const Type* const type)
{

  if(caret.isAtText("null", true)){
    return AbstractObjectWrapper(type);
  }

  if(caret.canContinueAtChar('{', 1)) {

    auto mapWrapper = type->creator();
    oatpp::data::mapping::type::PolymorphicWrapper<AbstractFieldsMap>
      map(std::static_pointer_cast<AbstractFieldsMap>(mapWrapper.getPtr()), mapWrapper.valueType);

    auto it = type->params.begin();
    Type* keyType = *it ++;
    if(keyType->classId.id != oatpp::data::mapping::type::__class::String::CLASS_ID.id){
      throw std::runtime_error("[oatpp::parser::json::mapping::Deserializer::readListMap()]: Invalid json map key. Key should be String");
    }
    Type* valueType = *it;

    caret.skipBlankChars();

    while (!caret.isAtChar('}') && caret.canContinue()) {

      caret.skipBlankChars();
      auto key = Utils::parseString(caret);
      if(caret.hasError()){
        return AbstractObjectWrapper::empty();
      }

      caret.skipBlankChars();
      if(!caret.canContinueAtChar(':', 1)){
        caret.setError("[oatpp::parser::json::mapping::Deserializer::readListMap()]: Error. ':' - expected", ERROR_CODE_OBJECT_SCOPE_COLON_MISSING);
        return AbstractObjectWrapper::empty();
      }

      caret.skipBlankChars();

      map->putPolymorphicItem(key, deserializer->deserialize(caret, valueType));

      caret.skipBlankChars();
      caret.canContinueAtChar(',', 1);

    }

    if(!caret.canContinueAtChar('}', 1)){
      if(!caret.hasError()){
        caret.setError("[oatpp::parser::json::mapping::Deserializer::readListMap()]: Error. '}' - expected", ERROR_CODE_OBJECT_SCOPE_CLOSE);
      }
      return AbstractObjectWrapper::empty();
    }

    return AbstractObjectWrapper(map.getPtr(), map.valueType);

  } else {
    caret.setError("[oatpp::parser::json::mapping::Deserializer::readListMap()]: Error. '{' - expected", ERROR_CODE_OBJECT_SCOPE_OPEN);
  }

  return AbstractObjectWrapper::empty();

}

data::mapping::type::AbstractObjectWrapper Deserializer::deserializeObject(Deserializer* deserializer,
                                                                            parser::Caret& caret,
                                                                            const Type* const type)
{

  if(caret.isAtText("null", true)){
    return AbstractObjectWrapper(type);
  }

  if(caret.canContinueAtChar('{', 1)) {

    auto object = type->creator();
    const auto& fieldsMap = type->properties->getMap();

    caret.skipBlankChars();

    while (!caret.isAtChar('}') && caret.canContinue()) {

      caret.skipBlankChars();
      auto key = Utils::parseStringToStdString(caret);
      if(caret.hasError()){
        return AbstractObjectWrapper::empty();
      }

      auto fieldIterator = fieldsMap.find(key);
      if(fieldIterator != fieldsMap.end()){

        caret.skipBlankChars();
        if(!caret.canContinueAtChar(':', 1)){
          caret.setError("[oatpp::parser::json::mapping::Deserializer::readObject()]: Error. ':' - expected", ERROR_CODE_OBJECT_SCOPE_COLON_MISSING);
          return AbstractObjectWrapper::empty();
        }

        caret.skipBlankChars();

        auto field = fieldIterator->second;
        field->set(object.get(), deserializer->deserialize(caret, field->type));

      } else if (deserializer->getConfig()->allowUnknownFields) {
        caret.skipBlankChars();
        if(!caret.canContinueAtChar(':', 1)){
          caret.setError("[oatpp::parser::json::mapping::Deserializer::readObject()/if(config->allowUnknownFields){}]: Error. ':' - expected", ERROR_CODE_OBJECT_SCOPE_COLON_MISSING);
          return AbstractObjectWrapper::empty();
        }
        caret.skipBlankChars();
        skipValue(caret);
      } else {
        caret.setError("[oatpp::parser::json::mapping::Deserializer::readObject()]: Error. Unknown field", ERROR_CODE_OBJECT_SCOPE_UNKNOWN_FIELD);
        return AbstractObjectWrapper::empty();
      }

      caret.skipBlankChars();
      caret.canContinueAtChar(',', 1);

    }

    if(!caret.canContinueAtChar('}', 1)){
      if(!caret.hasError()){
        caret.setError("[oatpp::parser::json::mapping::Deserializer::readObject()]: Error. '}' - expected", ERROR_CODE_OBJECT_SCOPE_CLOSE);
      }
      return AbstractObjectWrapper::empty();
    }

    return object;

  } else {
    caret.setError("[oatpp::parser::json::mapping::Deserializer::readObject()]: Error. '{' - expected", ERROR_CODE_OBJECT_SCOPE_OPEN);
  }

  return AbstractObjectWrapper::empty();

}

data::mapping::type::AbstractObjectWrapper Deserializer::deserialize(parser::Caret& caret, const Type* const type) {
  auto id = type->classId.id;
  auto& method = m_methods[id];
  if(method) {
    return (*method)(this, caret, type);
  } else {
    throw std::runtime_error("[oatpp::parser::json::mapping::Deserializer::deserialize()]: "
                             "Error. No deserialize method for type '" + std::string(type->classId.name) + "'");
  }
}

const std::shared_ptr<Deserializer::Config>& Deserializer::getConfig() {
  return m_config;
}

}}}}
