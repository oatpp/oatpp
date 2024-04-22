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

#ifndef oatpp_async_Error_hpp
#define oatpp_async_Error_hpp

#include "oatpp/core/base/Countable.hpp"
#include <string>

namespace oatpp { namespace async {

/**
 * Class to hold and communicate errors between Coroutines
 */
class Error : public std::runtime_error, public oatpp::base::Countable {
public:

  /**
   * Constructor.
   * @param what - error explanation.
   */
  explicit Error(const std::string& what);

  /**
   * Check if error belongs to specified class.
   * @tparam ErrorClass
   * @return - `true` if error is of specified class
   */
  template<class ErrorClass>
  bool is() const {
    return dynamic_cast<const ErrorClass*>(this) != nullptr;
  }

};

}}


#endif //oatpp_async_Error_hpp
