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

#ifndef oatpp_db_Client_hpp
#define oatpp_db_Client_hpp

#include "Executor.hpp"

#include "oatpp/core/data/stream/Stream.hpp"
#include "oatpp/core/Types.hpp"

#include <unordered_map>

namespace oatpp { namespace db {

class Client {
private:
  std::shared_ptr<Executor> m_executor;
public:

  Client(const std::shared_ptr<Executor>& executor);
  virtual ~Client() = default;

//  QUERY(getUserById,
//        "SELECT * FROM user WHERE userId=:userId",
//        PARAM(String, userId))
//
//  QUERY(deleteUserById,
//        "DELETE FROM user WHERE userId=:userId",
//        PARAM(String, userId))

  oatpp::data::share::StringTemplate Z_QUERY_TEMPLATE_getUserById =
    m_executor->parseQueryTemplate("SELECT * FROM user WHERE userId=:userId");

  oatpp::db::QueryResult getUserById(const oatpp::String& userId) {
    std::unordered_map<oatpp::String, oatpp::Any> __params;
    __params["userId"] = userId;
    return m_executor->execute(Z_QUERY_TEMPLATE_getUserById, __params);
  }



};

}}

#endif // oatpp_db_Client_hpp
