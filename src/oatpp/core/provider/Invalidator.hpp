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

#ifndef oatpp_provider_Invalidator_hpp
#define oatpp_provider_Invalidator_hpp

#include "oatpp/core/base/Countable.hpp"
#include <memory>

namespace oatpp { namespace provider {

/**
 * Abstract resource invalidator.
 * @tparam T - resource class.
 */
template<class T>
class Invalidator : public oatpp::base::Countable {
public:

  /**
   * Default virtual destructor.
   */
  virtual ~Invalidator() = default;

  /**
   * Invalidate resource that was previously created by the correspondent provider. <br>
   * Use-case: if provider is pool based - you can signal that this resource should not be reused anymore.
   * @param resource
   */
  virtual void invalidate(const std::shared_ptr<T> &resource) = 0;

};

}}

#endif //oatpp_provider_Invalidator_hpp
