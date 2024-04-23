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

#include "DbClient.hpp"

#include "oatpp/data/stream/BufferStream.hpp"

namespace oatpp { namespace orm {

DbClient::DbClient(const std::shared_ptr<Executor>& executor)
  : m_executor(executor)
  , m_typeResolver(executor->createTypeResolver())
{}

provider::ResourceHandle<Connection> DbClient::getConnection() {
  return m_executor->getConnection();
}

void DbClient::setEnabledInterpretations(const std::vector<std::string>& enabledInterpretations) {
  m_typeResolver->setEnabledInterpretations(enabledInterpretations);
}

std::shared_ptr<const data::mapping::TypeResolver> DbClient::getTypeResolver() {
  return m_typeResolver;
}

data::share::StringTemplate DbClient::parseQueryTemplate(const oatpp::String& name,
                                                         const oatpp::String& text,
                                                         const Executor::ParamsTypeMap& paramsTypeMap,
                                                         bool prepare)
{
  return m_executor->parseQueryTemplate(name, text, paramsTypeMap, prepare);
}

std::shared_ptr<QueryResult> DbClient::execute(const data::share::StringTemplate& queryTemplate,
                                               const std::unordered_map<oatpp::String, oatpp::Void>& params,
                                               const provider::ResourceHandle<Connection>& connection)
{
  return m_executor->execute(queryTemplate, params, m_typeResolver, connection);
}

std::shared_ptr<QueryResult> DbClient::executeQuery(const oatpp::String& query,
                                                    const std::unordered_map<oatpp::String, oatpp::Void>& params,
                                                    const provider::ResourceHandle<Connection>& connection)
{
  return m_executor->execute(query, params, m_typeResolver, connection);
}

Transaction DbClient::beginTransaction(const provider::ResourceHandle<Connection>& connection) {
  return Transaction(m_executor, connection);
}

}}
