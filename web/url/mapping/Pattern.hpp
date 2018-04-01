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

#ifndef network_url_Pattern_hpp
#define network_url_Pattern_hpp

#include "oatpp/core/collection/ListMap.hpp"
#include "oatpp/core/collection/LinkedList.hpp"

#include "oatpp/core/base/String.hpp"

#include "oatpp/core/base/PtrWrapper.hpp"
#include "oatpp/core/base/Controllable.hpp"
#include "oatpp/core/base/Environment.hpp"

namespace oatpp { namespace web { namespace url { namespace mapping {
  
class Pattern : public base::Controllable{
public:
  
  class MatchMap : public base::Controllable{
  public:
    typedef oatpp::collection::ListMap<base::String::PtrWrapper, base::String::PtrWrapper> Variables;
  protected:
    MatchMap(const std::shared_ptr<Variables>& vars, const std::shared_ptr<base::String>& urlTail)
      : variables(vars)
      , tail(urlTail)
    {}
  public:
    
    static std::shared_ptr<MatchMap> createShared(const std::shared_ptr<Variables>& vars, const std::shared_ptr<base::String>& tail){
      return std::shared_ptr<MatchMap>(new MatchMap(vars, tail));
    }
    
    const Variables::Entry* getVariable(const base::String::PtrWrapper& key){
      return variables->find(key);
    }
    
    const std::shared_ptr<Variables> variables;
    const base::String::PtrWrapper tail;
    
  };
  
private:
  
  class Part : public base::Controllable{
  protected:
    Part(const char* pFunction, const std::shared_ptr<base::String>& pText)
      : function(pFunction)
      , text(pText)
    {}
  public:
    
    static const char* FUNCTION_CONST;
    static const char* FUNCTION_VAR;
    static const char* FUNCTION_ANY_END;
    
    static std::shared_ptr<Part> createShared(const char* function, const std::shared_ptr<base::String>& text){
      return std::shared_ptr<Part>(new Part(function, text));
    }
    
    const char* function;
    const std::shared_ptr<base::String> text;
    
  };
  
private:
  std::shared_ptr<oatpp::collection::LinkedList<std::shared_ptr<Part>>> m_parts;
protected:
  Pattern()
    : m_parts(oatpp::collection::LinkedList<std::shared_ptr<Part>>::createShared())
  {}
public:
  
  static std::shared_ptr<Pattern> createShared(){
    return std::shared_ptr<Pattern>(new Pattern());
  }
  
  static std::shared_ptr<Pattern> parse(p_char8 data, v_int32 size);
  static std::shared_ptr<Pattern> parse(const char* data);
  static std::shared_ptr<Pattern> parse(const base::String::PtrWrapper& data);
  
  std::shared_ptr<MatchMap> match(p_char8 url, v_int32 size);
  std::shared_ptr<MatchMap> match(const char* url);
  std::shared_ptr<MatchMap> match(const base::String::PtrWrapper& url);
  
  std::shared_ptr<oatpp::base::String> toString();
  
};
  
}}}}

#endif /* Pattern_hpp */
