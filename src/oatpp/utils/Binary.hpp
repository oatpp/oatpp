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

#ifndef oatpp_utils_Binary_hpp
#define oatpp_utils_Binary_hpp

#include "oatpp/Environment.hpp"

namespace oatpp { namespace utils {

/**
 * Collection of methods for binary operations and arithmetics.
 */
class Binary {
public:

  /**
   * Calculate the next power of 2. <br>
   * Example: <br>
   * `nextP2(127) = 128`, `nextP2(1025) = 2048`.
   * @return
   */
  static v_int64 nextP2(v_int64 v);

};

}}

#endif // oatpp_utils_Binary_hpp
