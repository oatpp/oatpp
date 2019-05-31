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

#include "./Deserializer.hpp"

#include "oatpp/parser/json/Utils.hpp"
#include "oatpp/core/utils/ConversionUtils.hpp"

namespace oatpp { namespace parser { namespace json { namespace mapping {

void Deserializer::skipScope(oatpp::parser::Caret& caret, v_char8 charOpen, v_char8 charClose){
  
  p_char8 data = caret.getData();
  v_int32 size = caret.getDataSize();
  v_int32 pos = caret.getPosition();
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
  v_int32 size = caret.getDataSize();
  v_int32 pos = caret.getPosition();
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
  v_int32 size = caret.getDataSize();
  v_int32 pos = caret.getPosition();
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
  
Deserializer::AbstractObjectWrapper Deserializer::readStringValue(oatpp::parser::Caret& caret){
  if(caret.isAtText("null", true)){
    return AbstractObjectWrapper(String::Class::getType());
  } else {
    return AbstractObjectWrapper(oatpp::parser::json::Utils::parseString(caret).getPtr(), String::Class::getType());
  }
}

Deserializer::AbstractObjectWrapper Deserializer::readInt32Value(oatpp::parser::Caret& caret){
  if(caret.isAtText("null", true)){
    return AbstractObjectWrapper(Int32::ObjectWrapper::Class::getType());
  } else {
    return AbstractObjectWrapper(Int32::ObjectType::createAbstract((const int)caret.parseInt()), Int32::ObjectWrapper::Class::getType());
  }
}

Deserializer::AbstractObjectWrapper Deserializer::readInt64Value(oatpp::parser::Caret& caret){
  if(caret.isAtText("null", true)){
    return AbstractObjectWrapper(Int64::ObjectWrapper::Class::getType());
  } else {
    return AbstractObjectWrapper(Int64::ObjectType::createAbstract(caret.parseInt()), Int64::ObjectWrapper::Class::getType());
  }
}

Deserializer::AbstractObjectWrapper Deserializer::readFloat32Value(oatpp::parser::Caret& caret){
  if(caret.isAtText("null", true)){
    return AbstractObjectWrapper(Float32::ObjectWrapper::Class::getType());
  } else {
    return AbstractObjectWrapper(Float32::ObjectType::createAbstract(caret.parseFloat32()), Float32::ObjectWrapper::Class::getType());
  }
}

Deserializer::AbstractObjectWrapper Deserializer::readFloat64Value(oatpp::parser::Caret& caret){
  if(caret.isAtText("null", true)){
    return AbstractObjectWrapper(Float64::ObjectWrapper::Class::getType());
  } else {
    return AbstractObjectWrapper(Float64::ObjectType::createAbstract(caret.parseFloat64()), Float64::ObjectWrapper::Class::getType());
  }
}

Deserializer::AbstractObjectWrapper Deserializer::readBooleanValue(oatpp::parser::Caret& caret){
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
  
Deserializer::AbstractObjectWrapper Deserializer::readObjectValue(const Type* const type,
                                                        oatpp::parser::Caret& caret,
                                                        const std::shared_ptr<Config>& config){
  if(caret.isAtText("null", true)){
    return AbstractObjectWrapper::empty();
  } else {
    return readObject(type, caret, config);
  }
}
  
Deserializer::AbstractObjectWrapper Deserializer::readListValue(const Type* const type,
                                                      oatpp::parser::Caret& caret,
                                                      const std::shared_ptr<Config>& config){
  if(caret.isAtText("null", true)){
    return AbstractObjectWrapper::empty();
  } else {
    return readList(type, caret, config);
  }
}
  
Deserializer::AbstractObjectWrapper Deserializer::readListMapValue(const Type* const type,
                                                                   oatpp::parser::Caret& caret,
                                                                   const std::shared_ptr<Config>& config){
  if(caret.isAtText("null", true)){
    return AbstractObjectWrapper::empty();
  } else {
    return readListMap(type, caret, config);
  }
}
  
Deserializer::AbstractObjectWrapper Deserializer::readValue(const Type* const type,
                                                  oatpp::parser::Caret& caret,
                                                  const std::shared_ptr<Config>& config){
  
  auto typeName = type->name;
  if(typeName == oatpp::data::mapping::type::__class::String::CLASS_NAME){
    return readStringValue(caret);
  } else if(typeName == oatpp::data::mapping::type::__class::Int32::CLASS_NAME){
    return readInt32Value(caret);
  } else if(typeName == oatpp::data::mapping::type::__class::Int64::CLASS_NAME){
    return readInt64Value(caret);
  } else if(typeName == oatpp::data::mapping::type::__class::Float32::CLASS_NAME){
    return readFloat32Value(caret);
  } else if(typeName == oatpp::data::mapping::type::__class::Float64::CLASS_NAME){
    return readFloat64Value(caret);
  } else if(typeName == oatpp::data::mapping::type::__class::Boolean::CLASS_NAME){
    return readBooleanValue(caret);
  } else if(typeName == oatpp::data::mapping::type::__class::AbstractObject::CLASS_NAME){
    return readObjectValue(type, caret, config);
  } else if(typeName == oatpp::data::mapping::type::__class::AbstractList::CLASS_NAME){
    return readListValue(type, caret, config);
  } else if(typeName == oatpp::data::mapping::type::__class::AbstractListMap::CLASS_NAME){
    return readListMapValue(type, caret, config);
  } else {
    skipValue(caret);
  }
  
  return AbstractObjectWrapper::empty();
  
}
  
Deserializer::AbstractObjectWrapper Deserializer::readList(const Type* type,
                                                 oatpp::parser::Caret& caret,
                                                 const std::shared_ptr<Config>& config){
  
  if(caret.canContinueAtChar('[', 1)) {
    
    auto listWrapper = type->creator();
    oatpp::data::mapping::type::PolymorphicWrapper<AbstractList>
    list(std::static_pointer_cast<AbstractList>(listWrapper.getPtr()), listWrapper.valueType);
    
    Type* itemType = *type->params.begin();

    caret.skipBlankChars();
    
    while(!caret.isAtChar(']') && caret.canContinue()){
      
      caret.skipBlankChars();
      auto item = readValue(itemType, caret, config);
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
  
Deserializer::AbstractObjectWrapper Deserializer::readListMap(const Type* type,
                                                              oatpp::parser::Caret& caret,
                                                              const std::shared_ptr<Config>& config){
  
  if(caret.canContinueAtChar('{', 1)) {
    
    auto mapWrapper = type->creator();
    oatpp::data::mapping::type::PolymorphicWrapper<AbstractListMap>
    map(std::static_pointer_cast<AbstractListMap>(mapWrapper.getPtr()), mapWrapper.valueType);
    
    auto it = type->params.begin();
    Type* keyType = *it ++;
    if(keyType->name != oatpp::data::mapping::type::__class::String::CLASS_NAME){
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
      
      map->putPolymorphicItem(key, readValue(valueType, caret, config));
      
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
  
Deserializer::AbstractObjectWrapper Deserializer::readObject(const Type* type,
                                                   oatpp::parser::Caret& caret,
                                                   const std::shared_ptr<Config>& config){
  
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
        field->set(object.get(), readValue(field->type, caret, config));
        
      } else if (config->allowUnknownFields) {
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
  
}}}}
