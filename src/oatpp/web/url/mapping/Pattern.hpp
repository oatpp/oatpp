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

#ifndef oatpp_web_url_mapping_Pattern_hpp
#define oatpp_web_url_mapping_Pattern_hpp

#include "oatpp/data/share/MemoryLabel.hpp"

#include "oatpp/utils/parser/Caret.hpp"

#include <list>
#include <unordered_map>

namespace oatpp { namespace web { namespace url { namespace mapping {
  
class Pattern : public base::Countable{
private:
  typedef oatpp::data::share::StringKeyLabel StringKeyLabel;
public:
  
  class MatchMap {
    friend Pattern;
  public:
    typedef std::unordered_map<StringKeyLabel, StringKeyLabel> Variables;
  private:
    Variables m_variables;
    StringKeyLabel m_tail;
  public:
    
    MatchMap() {}
    
    MatchMap(const Variables& vars, const StringKeyLabel& urlTail)
      : m_variables(vars)
      , m_tail(urlTail)
    {}
    
    oatpp::String getVariable(const StringKeyLabel& key) const {
      auto it = m_variables.find(key);
      if(it != m_variables.end()) {
        return it->second.toString();
      }
      return nullptr;
    }
    
    oatpp::String getTail() const {
      return m_tail.toString();
    }

    const Variables& getVariables() const {
      return m_variables;
    }
    
  };
  
private:
  
  class Part : public base::Countable{
  public:
    Part(const char* pFunction, const oatpp::String& pText)
      : function(pFunction)
      , text(pText)
    {}
  public:
    
    static const char* FUNCTION_CONST;
    static const char* FUNCTION_VAR;
    static const char* FUNCTION_ANY_END;
    
    static std::shared_ptr<Part> createShared(const char* function, const oatpp::String& text){
      return std::make_shared<Part>(function, text);
    }
    
    const char* function;
    const oatpp::String text;
    
  };
  
private:
  std::shared_ptr<std::list<std::shared_ptr<Part>>> m_parts{std::make_shared<std::list<std::shared_ptr<Part>>>()};
private:
  v_char8 findSysChar(oatpp::utils::parser::Caret& caret);
public:
  
  static std::shared_ptr<Pattern> createShared(){
    return std::make_shared<Pattern>();
  }
  
  static std::shared_ptr<Pattern> parse(p_char8 data, v_buff_size size);
  static std::shared_ptr<Pattern> parse(const char* data);
  static std::shared_ptr<Pattern> parse(const oatpp::String& data);
  
  bool match(const StringKeyLabel& url, MatchMap& matchMap);
  
  oatpp::String toString();
  
};
  
}}}}

#endif /* oatpp_web_url_mapping_Pattern_hpp */
