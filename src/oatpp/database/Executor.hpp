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

#ifndef oatpp_database_Executor_hpp
#define oatpp_database_Executor_hpp

#include "Connection.hpp"
#include "QueryResult.hpp"

#include "oatpp/core/data/mapping/type/Type.hpp"
#include "oatpp/core/data/share/StringTemplate.hpp"

namespace oatpp { namespace database {

class Executor {
public:
  typedef oatpp::data::share::StringTemplate StringTemplate;
  typedef std::unordered_map<oatpp::String, oatpp::data::mapping::type::Type*> ParamsTypeMap;
public:

  virtual std::shared_ptr<Connection> getConnection() = 0;

  virtual StringTemplate parseQueryTemplate(const oatpp::String& name,
                                            const oatpp::String& text,
                                            const ParamsTypeMap& paramsTypeMap) = 0;

  virtual QueryResult execute(const StringTemplate& queryTemplate,
                              const std::unordered_map<oatpp::String, oatpp::Void>& params,
                              const std::shared_ptr<Connection>& connection = nullptr) = 0;

};

}}

#endif // oatpp_database_Executor_hpp