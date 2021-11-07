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

#ifndef oatpp_orm_QueryResult_hpp
#define oatpp_orm_QueryResult_hpp

#include "Connection.hpp"
#include "oatpp/core/provider/Provider.hpp"
#include "oatpp/core/Types.hpp"

namespace oatpp { namespace orm {

/**
 * Result of DB query.
 */
class QueryResult {
public:

  /**
   * Virtual destructor.
   */
  virtual ~QueryResult() = default;

  /**
   * Get DB connection associated with this result.
   * @return
   */
  virtual provider::ResourceHandle<Connection> getConnection() const = 0;

  /**
   * Check if the query was successful.
   * @return
   */
  virtual bool isSuccess() const = 0;

  /**
   * Get error message in case `isSuccess() == false`
   * @return
   */
  virtual oatpp::String getErrorMessage() const = 0;

  /**
   * Get result read position.
   * @return
   */
  virtual v_int64 getPosition() const = 0;

  /**
   * Get result entries count in the case it's known.
   * @return - `[0..N]` - in case known. `-1` - otherwise.
   */
  virtual v_int64 getKnownCount() const = 0;

  /**
   * Check if there is more data to fetch.
   * @return
   */
  virtual bool hasMoreToFetch() const = 0;

  /**
   * Fetch result entries.
   * @param resultType - wanted output type.
   * @param count - how many entries to fetch. Use `-1` to fetch all.
   * @return - `oatpp::Void`.
   */
  virtual oatpp::Void fetch(const oatpp::Type* const resultType, v_int64 count) = 0;

  /**
   * Fetch result entries.
   * @tparam Wrapper - output type.
   * @param count - how many entries to fetch. Use `-1` to fetch all.
   * @return - `Wrapper`.
   */
  template<class Wrapper>
  Wrapper fetch(v_int64 count = -1) {
    return fetch(Wrapper::Class::getType(), count).template cast<Wrapper>();
  }

};

}}

#endif // oatpp_orm_QueryResult_hpp
