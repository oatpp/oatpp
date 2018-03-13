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

#include "../../../../../oatpp-lib/core/src/data/mapping/type/List.hpp"
#include "../../../../../oatpp-lib/core/src/data/mapping/type/Object.hpp"
#include "../../../../../oatpp-lib/core/src/data/mapping/type/Primitive.hpp"
#include "../../../../../oatpp-lib/core/src/data/mapping/type/Type.hpp"
#include "../../../../../oatpp-lib/core/src/data/stream/ChunkedBuffer.hpp"

#include "../../../../../oatpp-lib/core/src/parser/ParsingCaret.hpp"

#include "../../../../../oatpp-lib/core/src/collection/LinkedList.hpp"
#include "../../../../../oatpp-lib/core/src/base/String.hpp"

namespace oatpp { namespace parser { namespace json { namespace mapping {
  
class Serializer {
public:
  typedef oatpp::data::mapping::type::Type Type;
  typedef oatpp::data::mapping::type::Type::Property Property;
  typedef oatpp::data::mapping::type::Type::Properties Properties;
  
  typedef oatpp::data::mapping::type::Object Object;
  
  typedef oatpp::data::mapping::type::List<
    oatpp::data::mapping::type::AbstractSharedWrapper
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
    
  };
  
public:
  typedef oatpp::base::String String;
private:
  
  static void writeString(oatpp::data::stream::OutputStream* stream,
                          void* object,
                          Property* field);
  
  template<class T>
  static void writeSimpleData(oatpp::data::stream::OutputStream* stream,
                              void* object,
                              Property* field){
    auto value = oatpp::base::static_wrapper_cast<T>(field->get(object));
    stream->writeChar('\"');
    stream->write(field->name);
    stream->write("\": ", 3);
    if(value.isNull()){
      stream->write("null", 4);
    } else {
      stream->writeAsString(value.get()->getValue());
    }
  }
  
  static void writeObject(oatpp::data::stream::OutputStream* stream,
                          void* object,
                          Property* field);
  
  static void writeListOfString(oatpp::data::stream::OutputStream* stream,
                                AbstractList* list);
  
  template<class T>
  static void writeListOfSimpleData(oatpp::data::stream::OutputStream* stream,
                                    AbstractList* list){
    stream->writeChar('[');
    bool first = true;
    auto curr = list->getFirstNode();
    while(curr != nullptr){
      
      auto value = oatpp::base::static_wrapper_cast<T>(curr->getData());
      
      if(first){
        first = false;
      } else {
        stream->write(", ", 2);
      }
      
      if(value.isNull()){
        stream->write("null", 4);
      } else {
        stream->writeAsString(value.get()->getValue());
      }
      
      curr = curr->getNext();
    }
    
    stream->writeChar(']');
    
  }
  
  static void writeListOfObject(oatpp::data::stream::OutputStream* stream,
                                AbstractList* list,
                                const Type* const type);
  
  static void writeListOfList(oatpp::data::stream::OutputStream* stream,
                              AbstractList* list,
                              const Type* const type);
  
  static void writeListCollection(oatpp::data::stream::OutputStream* stream,
                                  AbstractList* list,
                                  const Type* const type);
  
  static void writeList(oatpp::data::stream::OutputStream* stream,
                        void* object,
                        Property* field);
  
  static void writeObject(oatpp::data::stream::OutputStream* stream,
                          const Type* const type,
                          Object* object);
  
public:
  
  static void serialize(const std::shared_ptr<oatpp::data::stream::OutputStream>& stream,
                        const oatpp::data::mapping::type::VariantWrapper& variant){
    auto type = variant.getValueType();
    if(type->name == oatpp::data::mapping::type::__class::AbstractObject::CLASS_NAME) {
      writeObject(stream.get(), type, static_cast<Object*>(variant.get()));
    } else if(type->name == oatpp::data::mapping::type::__class::AbstractList::CLASS_NAME) {
      writeListCollection(stream.get(), static_cast<AbstractList*>(variant.get()), type);
    } else {
      throw std::runtime_error("[oatpp::parser::json::mapping::Serializer::serialize()]: Unknown parameter type");
    }
  }
  
};
  
}}}}

#endif /* oatpp_parser_json_mapping_Serializer_hpp */
