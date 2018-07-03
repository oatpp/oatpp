/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi, <lganzzzo@gmail.com>
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

#ifndef oatpp_parser_json_mapping_Serializer_hpp
#define oatpp_parser_json_mapping_Serializer_hpp

#include "oatpp/core/data/mapping/type/List.hpp"
#include "oatpp/core/data/mapping/type/Object.hpp"
#include "oatpp/core/data/mapping/type/Primitive.hpp"
#include "oatpp/core/data/mapping/type/Type.hpp"
#include "oatpp/core/data/stream/ChunkedBuffer.hpp"

#include "oatpp/core/parser/ParsingCaret.hpp"

#include "oatpp/core/collection/LinkedList.hpp"
#include "oatpp/core/Types.hpp"

namespace oatpp { namespace parser { namespace json { namespace mapping {
  
class Serializer {
public:
  typedef oatpp::data::mapping::type::Type Type;
  typedef oatpp::data::mapping::type::Type::Property Property;
  typedef oatpp::data::mapping::type::Type::Properties Properties;
  
  typedef oatpp::data::mapping::type::Object Object;
  
  typedef oatpp::data::mapping::type::List<
    oatpp::data::mapping::type::AbstractObjectWrapper
  > AbstractList;
public:
  
  class Config : public oatpp::base::Controllable {
  protected:
    Config()
    {}
  public:
    
    static std::shared_ptr<Config> createShared(){
      return std::shared_ptr<Config>(new Config());
    }
    
    bool includeNullFields = true;
    
  };
  
public:
  typedef oatpp::String String;
private:
  
  static void writeString(oatpp::data::stream::OutputStream* stream,
                          void* object,
                          Property* field,
                          const std::shared_ptr<Config>& config);
  
  template<class T>
  static void writeSimpleData(oatpp::data::stream::OutputStream* stream,
                              void* object,
                              Property* field,
                              const std::shared_ptr<Config>& config){
    auto value = oatpp::data::mapping::type::static_wrapper_cast<T>(field->get(object));
    if(value) {
      stream->writeChar('\"');
      stream->write(field->name);
      stream->write("\": ", 3);
      stream->writeAsString(value.get()->getValue());
    } else if(config->includeNullFields) {
      stream->writeChar('\"');
      stream->write(field->name);
      stream->write("\": null", 7);
    }
  }
  
  static void writeObject(oatpp::data::stream::OutputStream* stream,
                          void* object,
                          Property* field,
                          const std::shared_ptr<Config>& config);
  
  static void writeListOfString(oatpp::data::stream::OutputStream* stream,
                                AbstractList* list,
                                const std::shared_ptr<Config>& config);
  
  template<class T>
  static void writeListOfSimpleData(oatpp::data::stream::OutputStream* stream,
                                    AbstractList* list,
                                    const std::shared_ptr<Config>& config){
    stream->writeChar('[');
    bool first = true;
    auto curr = list->getFirstNode();
    while(curr != nullptr){
      
      auto value = oatpp::data::mapping::type::static_wrapper_cast<T>(curr->getData());
      if(value) {
        (first) ? first = false : stream->write(", ", 2);
        stream->writeAsString(value.get()->getValue());
      } else if(config->includeNullFields) {
        (first) ? first = false : stream->write(", ", 2);
        stream->write("null", 4);
      }
      
      curr = curr->getNext();
      
    }
    
    stream->writeChar(']');
    
  }
  
  static void writeListOfObject(oatpp::data::stream::OutputStream* stream,
                                AbstractList* list,
                                const Type* const type,
                                const std::shared_ptr<Config>& config);
  
  static void writeListOfList(oatpp::data::stream::OutputStream* stream,
                              AbstractList* list,
                              const Type* const type,
                              const std::shared_ptr<Config>& config);
  
  static void writeListCollection(oatpp::data::stream::OutputStream* stream,
                                  AbstractList* list,
                                  const Type* const type,
                                  const std::shared_ptr<Config>& config);
  
  static void writeList(oatpp::data::stream::OutputStream* stream,
                        void* object,
                        Property* field,
                        const std::shared_ptr<Config>& config);
  
  static void writeObject(oatpp::data::stream::OutputStream* stream,
                          const Type* const type,
                          Object* object,
                          const std::shared_ptr<Config>& config);
  
public:
  
  static void serialize(const std::shared_ptr<oatpp::data::stream::OutputStream>& stream,
                        const oatpp::data::mapping::type::AbstractObjectWrapper& object,
                        const std::shared_ptr<Config>& config){
    auto type = object.valueType;
    if(type->name == oatpp::data::mapping::type::__class::AbstractObject::CLASS_NAME) {
      writeObject(stream.get(), type, static_cast<Object*>(object.get()), config);
    } else if(type->name == oatpp::data::mapping::type::__class::AbstractList::CLASS_NAME) {
      writeListCollection(stream.get(), static_cast<AbstractList*>(object.get()), type, config);
    } else {
      throw std::runtime_error("[oatpp::parser::json::mapping::Serializer::serialize()]: Unknown parameter type");
    }
  }
  
};
  
}}}}

#endif /* oatpp_parser_json_mapping_Serializer_hpp */
