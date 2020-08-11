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

#include "oatpp/core/Types.hpp"

namespace oatpp { namespace orm {

class QueryResult {
public:

  virtual ~QueryResult() = default;

  virtual bool isSuccess() const = 0;

  virtual v_int64 position() const = 0;

  virtual v_int64 count() const = 0;

  virtual void fetch(oatpp::Void& polymorph, v_int64 count) = 0;

  template<class Wrapper>
  Wrapper fetch(v_int64 count) {
    oatpp::Void polymorph(Wrapper::Class::getType());
    fetch(polymorph, count);
    return polymorph.template staticCast<Wrapper>();
  }

  template<class Wrapper>
  Wrapper fetch() {
    return fetch<Wrapper>(this->count());
  }

};

}}

#endif // oatpp_orm_QueryResult_hpp
