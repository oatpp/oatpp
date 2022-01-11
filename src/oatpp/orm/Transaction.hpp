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

/**
 * Database transaction.
 */
class Transaction {
private:
  base::ObjectHandle<Executor> m_executor;
  bool m_open;
private:
  provider::ResourceHandle<Connection> m_connection;
public:

  /**
   * Constructor.
   * @param executor - &id:oatpp::orm::Executor;.
   * @param connection - database connection.
   */
  Transaction(const base::ObjectHandle<Executor>& executor, const provider::ResourceHandle<Connection>& connection = nullptr);

  /**
   * Copy constructor.
   * @param other
   */
  Transaction(const Transaction& other) = delete;

  /**
   * Move constructor.
   * @param other
   */
  Transaction(Transaction&& other);

  /**
   * Virtual destructor. <br>
   * Will automatically call &l:Transaction::rollback (); if there was no prior call to &l:Transaction::commit (); method.
   */
  virtual ~Transaction();

  Transaction& operator=(const Transaction& other) = delete;
  Transaction& operator=(Transaction&& other);

  /**
   * Get the database connection associated with the transaction. <br>
   * **Note:** all database queries within the transaction MUST be called on this connection.
   * @return
   */
  provider::ResourceHandle<Connection> getConnection() const;

  /**
   * Commit transaction.
   * @return
   */
  std::shared_ptr<QueryResult> commit();

  /**
   * Rollback transaction.
   * @return
   */
  std::shared_ptr<QueryResult> rollback();

};

}}

#endif // oatpp_orm_Transaction_hpp
