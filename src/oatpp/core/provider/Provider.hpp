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

#ifndef oatpp_provider_Provider_hpp
#define oatpp_provider_Provider_hpp

#include "oatpp/core/async/Coroutine.hpp"

namespace oatpp { namespace provider {

/**
 * Abstract resource provider.
 * @tparam T - resource class.
 */
template <class T>
class Provider {
public:

  /**
   * Virtual destructor.
   */
  virtual ~Provider() = default;

  /**
   * Get resource.
   * @return - resource.
   */
  virtual std::shared_ptr<T> get() = 0;

  /**
   * Get resource in Async manner.
   * @return - &id:oatpp::async::CoroutineStarterForResult; of `T`.
   */
  virtual async::CoroutineStarterForResult<const std::shared_ptr<T>&> getAsync() = 0;

  /**
   * Invalidate resource that was previously created by this provider. <br>
   * Use-case: if provider is pool based - you can signal that this resource should not be reused anymore.
   * @param resource
   */
  virtual void invalidate(const std::shared_ptr<T>& resource) = 0;

  /**
   * Stop provider and free associated resources.
   */
  virtual void stop() = 0;

};

}}

#endif // oatpp_provider_Provider_hpp
