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

#ifndef oatpp_orm_DbClient_hpp
#define oatpp_orm_DbClient_hpp

#include "Executor.hpp"
#include "Transaction.hpp"

#include "oatpp/core/data/stream/Stream.hpp"
#include "oatpp/core/Types.hpp"

namespace oatpp { namespace orm {

/**
 * Database client.
 */
class DbClient {
private:
  std::shared_ptr<Executor> m_executor;
protected:
  std::shared_ptr<data::mapping::TypeResolver> m_typeResolver;
public:

  /**
   * Constructor.
   * @param executor - &id:oatpp::orm::Executor;.
   */
  DbClient(const std::shared_ptr<Executor>& executor);

  /**
   * Virtual destructor.
   */
  virtual ~DbClient() = default;

  /**
   * Get database connection.
   * @return
   */
  provider::ResourceHandle<Connection> getConnection();

  /**
   * Set enabled type interpretations.
   * @param enabledInterpretations
   */
  void setEnabledInterpretations(const std::vector<std::string>& enabledInterpretations);

  /**
   * Get type resolver.
   * @return - &id:oatpp::data::mapping::TypeResolver;.
   */
  std::shared_ptr<const data::mapping::TypeResolver> getTypeResolver();

  /**
   * Parse query template.
   * @param name - template name.
   * @param text - template text.
   * @param paramsTypeMap - template parameter types.
   * @param prepare - `true` if the query should use prepared statement, `false` otherwise.
   * @return - &id:oatpp::data::share::StringTemplate;.
   */
  data::share::StringTemplate parseQueryTemplate(const oatpp::String& name,
                                                 const oatpp::String& text,
                                                 const Executor::ParamsTypeMap& paramsTypeMap,
                                                 bool prepare = false);

  /**
   * Execute query using template.
   * @param queryTemplate - a query template obtained in a prior call to &l:DbClient::parseQueryTemplate (); method.
   * @param params - query parameters.
   * @param connection - database connection.
   * @return - &id:oatpp::orm::QueryResult;.
   */
  std::shared_ptr<QueryResult> execute(const data::share::StringTemplate& queryTemplate,
                                       const std::unordered_map<oatpp::String, oatpp::Void>& params,
                                       const provider::ResourceHandle<Connection>& connection = nullptr);

  /**
   * Execute arbitrary query.
   * @param query - query text.
   * @param params - query parameters.
   * @param connection - database connection.
   * @return - &id:oatpp::orm::QueryResult;.
   */
  std::shared_ptr<QueryResult> executeQuery(const oatpp::String& query,
                                            const std::unordered_map<oatpp::String, oatpp::Void>& params,
                                            const provider::ResourceHandle<Connection>& connection = nullptr);

  /**
   * Begin database transaction.
   * @param connection - database connection.
   * @return - &id:oatpp::orm::Transaction;.
   */
  Transaction beginTransaction(const provider::ResourceHandle<Connection>& connection = nullptr);

};

}}

#endif // oatpp_orm_DbClient_hpp
