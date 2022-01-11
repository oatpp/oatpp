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

#ifndef oatpp_base_Countable
#define oatpp_base_Countable

#include <memory>
#include "./Environment.hpp"

namespace oatpp { namespace base{

/**
 * Class instantiations of which can be counted.
 */
class Countable {
public:
  /**
   * Constructor. Increment counter calling &id:oatpp::base::Environment::incObjects;.
   */
  Countable();

  /**
   * Copy constructor. Increment counter calling &id:oatpp::base::Environment::incObjects;.
   * @param other
   */
  Countable(const Countable& other);

  /**
   * Virtual destructor. Decrement counter calling &id:oatpp::base::Environment::decObjects;.
   */
  virtual ~Countable();

  Countable& operator = (Countable&) = default;


};
  
}}

#endif /* oatpp_base_Countable */
