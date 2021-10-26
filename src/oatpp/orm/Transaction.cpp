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

#include "Transaction.hpp"

namespace oatpp { namespace orm {

Transaction::Transaction(const base::ObjectHandle<Executor>& executor, const provider::ResourceHandle<Connection>& connection)
  : m_executor(executor)
  , m_open(true)
{
  auto res = executor->begin(connection);
  if(res->isSuccess()) {
    m_connection = res->getConnection();
  } else {
    m_open = false;
    throw std::runtime_error("[oatpp::orm::Transaction::Transaction()]: "
                             "Error. Can't begin transaction - " + *res->getErrorMessage());
  }
}

Transaction::Transaction(Transaction&& other)
  : m_executor(std::move(other.m_executor))
  , m_open(other.m_open)
  , m_connection(std::move(other.m_connection))
{
  other.m_open = false;
}

Transaction::~Transaction() {
  if(m_open) {
    try {
      rollback();
    } catch (...) {
      // DO NOTHING
    }
  }
}

Transaction& Transaction::operator=(Transaction&& other) {

  m_executor = std::move(other.m_executor);
  m_open = other.m_open;
  m_connection = std::move(other.m_connection);

  other.m_open = false;

  return *this;

}

provider::ResourceHandle<Connection> Transaction::getConnection() const {
  return m_connection;
}

std::shared_ptr<QueryResult> Transaction::commit() {
  if(m_open) {
    m_open = false;
    return m_executor->commit(m_connection);
  } else {
    throw std::runtime_error("[oatpp::orm::Transaction::commit()]: Error. The transaction is already closed.");
  }
}

std::shared_ptr<QueryResult> Transaction::rollback() {
  if(m_open) {
    m_open = false;
    return m_executor->rollback(m_connection);
  } else {
    throw std::runtime_error("[oatpp::orm::Transaction::commit()]: Error. The transaction is already closed.");
  }
}

}}
