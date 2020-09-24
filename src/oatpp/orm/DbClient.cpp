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

#include "oatpp/core/data/stream/BufferStream.hpp"

namespace oatpp { namespace orm {

DbClient::DbClient(const std::shared_ptr<Executor>& executor)
  : m_executor(executor)
{}

std::shared_ptr<Connection> DbClient::getConnection() {
  return m_executor->getConnection();
}

std::shared_ptr<QueryResult> DbClient::executeQuery(const oatpp::String& query,
                                                    const std::unordered_map<oatpp::String, oatpp::Void>& params,
                                                    const std::shared_ptr<Connection>& connection)
{
  return m_executor->execute(query, params, connection);
}

Transaction DbClient::beginTransaction(const std::shared_ptr<Connection>& connection) {
  return Transaction(m_executor, connection);
}

}}
