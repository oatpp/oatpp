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

#ifndef oatpp_orm_Transaction_hpp
#define oatpp_orm_Transaction_hpp

#include "Executor.hpp"

namespace oatpp { namespace orm {

class Transaction {
private:
  std::shared_ptr<Executor> m_executor;
  bool m_open;
private:
  std::shared_ptr<Connection> m_connection;
public:

  Transaction(const std::shared_ptr<Executor>& executor, const std::shared_ptr<Connection>& connection = nullptr);
  Transaction(const Transaction& other) = delete;
  Transaction(Transaction&& other);

  ~Transaction();

  Transaction& operator=(const Transaction& other) = delete;
  Transaction& operator=(Transaction&& other);

  std::shared_ptr<Connection> getConnection() const;

  std::shared_ptr<QueryResult> commit();
  std::shared_ptr<QueryResult> rollback();

};

}}

#endif // oatpp_orm_Transaction_hpp
