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

#ifndef oatpp_parser_json_mapping_Deserializer_hpp
#define oatpp_parser_json_mapping_Deserializer_hpp

#include "oatpp/core/data/mapping/type/List.hpp"
#include "oatpp/core/data/mapping/type/Object.hpp"
#include "oatpp/core/data/mapping/type/Primitive.hpp"
#include "oatpp/core/data/mapping/type/Type.hpp"

#include "oatpp/core/parser/ParsingCaret.hpp"

#include "oatpp/core/collection/LinkedList.hpp"
#include "oatpp/core/Types.hpp"

namespace oatpp { namespace parser { namespace json { namespace mapping {
  
class Deserializer {
public:
  typedef oatpp::data::mapping::type::Type Type;
  typedef oatpp::data::mapping::type::Type::Property Property;
  typedef oatpp::data::mapping::type::Type::Properties Properties;
  
  typedef oatpp::data::mapping::type::AbstractObjectWrapper AbstractObjectWrapper;
  typedef oatpp::data::mapping::type::Object Object;
  
private:
  typedef oatpp::data::mapping::type::String String;
  typedef oatpp::data::mapping::type::Int32 Int32;
  typedef oatpp::data::mapping::type::Int64 Int64;
  typedef oatpp::data::mapping::type::Float32 Float32;
  typedef oatpp::data::mapping::type::Float64 Float64;
  typedef oatpp::data::mapping::type::Boolean Boolean;
  
  typedef oatpp::data::mapping::type::List<AbstractObjectWrapper> AbstractList;
  typedef oatpp::data::mapping::type::ListMap<String, AbstractObjectWrapper> AbstractListMap;
  
public:
  
  class Config : public oatpp::base::Controllable {
  public:
    Config()
    {}
  public:
    
    static std::shared_ptr<Config> createShared(){
      return std::make_shared<Config>();
    }
    
    bool allowUnknownFields = true;
    
  };
  
public:
  static const char* const ERROR_PARSER_OBJECT_SCOPE_OPEN;
  static const char* const ERROR_PARSER_OBJECT_SCOPE_CLOSE;
  static const char* const ERROR_PARSER_OBJECT_SCOPE_UNKNOWN_FIELD;
  static const char* const ERROR_PARSER_OBJECT_SCOPE_COLON_MISSING;
  static const char* const ERROR_PARSER_ARRAY_SCOPE_OPEN;
  static const char* const ERROR_PARSER_ARRAY_SCOPE_CLOSE;
private:
  
  static void skipScope(oatpp::parser::ParsingCaret& caret, v_char8 charOpen, v_char8 charClose);
  static void skipString(oatpp::parser::ParsingCaret& caret);
  static void skipToken(oatpp::parser::ParsingCaret& caret);
  static void skipValue(oatpp::parser::ParsingCaret& caret);
  
  static AbstractObjectWrapper readStringValue(oatpp::parser::ParsingCaret& caret);
  static AbstractObjectWrapper readInt32Value(oatpp::parser::ParsingCaret& caret);
  static AbstractObjectWrapper readInt64Value(oatpp::parser::ParsingCaret& caret);
  static AbstractObjectWrapper readFloat32Value(oatpp::parser::ParsingCaret& caret);
  static AbstractObjectWrapper readFloat64Value(oatpp::parser::ParsingCaret& caret);
  static AbstractObjectWrapper readBooleanValue(oatpp::parser::ParsingCaret& caret);
  static AbstractObjectWrapper readObjectValue(const Type* const type,
                                               oatpp::parser::ParsingCaret& caret,
                                               const std::shared_ptr<Config>& config);
  static AbstractObjectWrapper readListValue(const Type* const type,
                                             oatpp::parser::ParsingCaret& caret,
                                             const std::shared_ptr<Config>& config);
  
  static AbstractObjectWrapper readListMapValue(const Type* const type,
                                                oatpp::parser::ParsingCaret& caret,
                                                const std::shared_ptr<Config>& config);
  
  static AbstractObjectWrapper readValue(const Type* const type,
                                         oatpp::parser::ParsingCaret& caret,
                                         const std::shared_ptr<Config>& config);
  
  static AbstractObjectWrapper readList(const Type* const type,
                                        oatpp::parser::ParsingCaret& caret,
                                        const std::shared_ptr<Config>& config);
  
  static AbstractObjectWrapper readListMap(const Type* const type,
                                           oatpp::parser::ParsingCaret& caret,
                                           const std::shared_ptr<Config>& config);
  
  static AbstractObjectWrapper readObject(const Type* const type,
                                          oatpp::parser::ParsingCaret& caret,
                                          const std::shared_ptr<Config>& config);
  
public:
  
  static AbstractObjectWrapper deserialize(oatpp::parser::ParsingCaret& caret,
                                           const std::shared_ptr<Config>& config,
                                           const Type* const type) {
    
    OATPP_LOGD("test", "type->name='%s'", type->name);
    OATPP_LOGD("test", "__class='%s'", oatpp::data::mapping::type::__class::AbstractObject::CLASS_NAME);
    
    OATPP_LOGD("test", "type->name addr=%d", type->name);
    OATPP_LOGD("test", "__class addr=%d", oatpp::data::mapping::type::__class::AbstractObject::CLASS_NAME);
    
    if(type->name == oatpp::data::mapping::type::__class::AbstractObject::CLASS_NAME){
      return readObject(type, caret, config);
    } else if(type->name == oatpp::data::mapping::type::__class::AbstractList::CLASS_NAME){
      return readList(type, caret, config);
    } else if(type->name == oatpp::data::mapping::type::__class::AbstractListMap::CLASS_NAME){
      return readListMap(type, caret, config);
    }
    return AbstractObjectWrapper::empty();
  }
  
};
  
}}}}

#endif /* oatpp_parser_json_mapping_Deserializer_hpp */
