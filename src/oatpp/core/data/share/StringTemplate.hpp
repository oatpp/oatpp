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

#ifndef oatpp_data_share_StringTemplate_hpp
#define oatpp_data_share_StringTemplate_hpp

#include "oatpp/core/data/stream/Stream.hpp"
#include "oatpp/core/Types.hpp"

#include <unordered_map>
#include <vector>

namespace oatpp { namespace data { namespace share {

class StringTemplate {
public:

  struct Variable {
    v_buff_size posStart;
    v_buff_size posEnd;
    oatpp::String name;
  };

private:
  oatpp::String m_text;
  std::vector<Variable> m_variables;
public:

  StringTemplate(const oatpp::String& text, std::vector<Variable>&& variables);

  void format(stream::ConsistentOutputStream* stream, const std::vector<oatpp::String>& params) const;
  void format(stream::ConsistentOutputStream* stream, const std::unordered_map<oatpp::String, oatpp::String>& params) const;

  oatpp::String format(const std::vector<oatpp::String>& params) const;
  oatpp::String format(const std::unordered_map<oatpp::String, oatpp::String>& params) const;

};

}}}

#endif // oatpp_data_share_StringTemplate_hpp
