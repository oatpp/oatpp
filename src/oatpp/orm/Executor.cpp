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

#include "Executor.hpp"

namespace oatpp { namespace orm {

Executor::Executor()
  : m_defaultTypeResolver(std::make_shared<data::mapping::TypeResolver>())
{}

std::shared_ptr<const data::mapping::TypeResolver> Executor::getDefaultTypeResolver() {
  return m_defaultTypeResolver;
}

std::shared_ptr<QueryResult> Executor::execute(const oatpp::String& query,
                                               const std::unordered_map<oatpp::String, oatpp::Void>& params,
                                               const std::shared_ptr<const data::mapping::TypeResolver>& typeResolver,
                                               const provider::ResourceHandle<Connection>& connection)
{
  const auto& qt = parseQueryTemplate(nullptr, query, ParamsTypeMap(), false);
  return execute(qt, params, typeResolver, connection);
}

}}
