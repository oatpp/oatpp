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

#include "Invalidator.hpp"

#include "oatpp/core/async/Coroutine.hpp"
#include "oatpp/core/data/share/MemoryLabel.hpp"

namespace oatpp { namespace provider {

/**
 * Resource handle template.
 * @tparam T
 */
template<class T, class PTR>
struct ResourceHandleTemplate {

  /**
   * Default constructor.
   */
  ResourceHandleTemplate() = default;

  /**
   * Nullptr constructor.
   */
  ResourceHandleTemplate(std::nullptr_t) {}

  /**
   * Constructor.
   * @param resourceObject
   * @param resourceInvalidator
   */
  ResourceHandleTemplate(const PTR& resourceObject,
                         const std::shared_ptr<Invalidator<T>> &resourceInvalidator)
    : object(resourceObject), invalidator(resourceInvalidator)
  {}

  /**
   * Pointer to the resource.
   */
  PTR object;

  /**
   * Invalidator that can be used to invalidate the resource.
   */
  std::shared_ptr<Invalidator<T>> invalidator;

  inline bool operator == (std::nullptr_t) const {
    return object.get() == nullptr;
  }

  inline bool operator != (std::nullptr_t) const {
    return object.get() != nullptr;
  }

  explicit inline operator bool() const {
    return object.operator bool();
  }

  /**
   * Invalidates the resource so it can be disposed and cannot be reused anymore.
   */
  virtual void invalidate() {
    invalidator->invalidate(object);
  }

};

/**
 * Resource handle.
 * @tparam T
 */
template<class T>
struct ResourceHandle : public ResourceHandleTemplate<T, std::shared_ptr<T>> {

  /**
   * Default constructor.
   */
  ResourceHandle() = default;

  /**
   * Nullptr constructor.
   */
  ResourceHandle(std::nullptr_t) {}

  /**
   * Constructor.
   * @param resourceObject
   * @param resourceInvalidator
   */
  ResourceHandle(const std::shared_ptr<T>& resourceObject,
                 const std::shared_ptr<Invalidator<T>>& resourceInvalidator)
    : ResourceHandleTemplate<T, std::shared_ptr<T>>(resourceObject, resourceInvalidator)
  {}

};

/**
 * Weak Resource handle.
 * @tparam T
 */
template<class T>
struct WeakResourceHandle : public ResourceHandleTemplate<T, std::weak_ptr<T>> {

  /**
   * Default constructor.
   */
  WeakResourceHandle() = default;

  /**
   * Nullptr constructor.
   */
  WeakResourceHandle(std::nullptr_t) {}

  /**
   * Constructor.
   * @param resourceObject
   * @param resourceInvalidator
   */
  WeakResourceHandle(const std::weak_ptr<T>& resourceObject,
                     const std::shared_ptr<Invalidator<T>>& resourceInvalidator)
    : ResourceHandleTemplate<T, std::weak_ptr<T>>(resourceObject, resourceInvalidator)
  {}

};

/**
 * Abstract resource provider.
 * @tparam T - resource class.
 */
template <class T>
class Provider : public oatpp::base::Countable {
protected:

  void setProperty(const oatpp::String& key, const oatpp::String& value) {
    m_properties[key] = value;
  }

protected:
  std::unordered_map<data::share::StringKeyLabelCI, data::share::StringKeyLabel> m_properties;
public:

  /**
   * Default constructor.
   */
  Provider() = default;

  /**
   * Constructor.
   * @param properties
   */
  Provider(const std::unordered_map<data::share::StringKeyLabelCI, data::share::StringKeyLabel>& properties)
    : m_properties(properties)
  {}

  /**
   * Virtual destructor.
   */
  virtual ~Provider() = default;

  /**
   * Some optional properties that user might want to know. <br>
   * Note: All properties are optional and user should not rely on this.
   */
  const std::unordered_map<data::share::StringKeyLabelCI, data::share::StringKeyLabel>& getProperties() const {
    return m_properties;
  }

  /**
   * Get optional property
   */
  data::share::StringKeyLabel getProperty(const oatpp::String& key) const {
    auto it = m_properties.find(key);
    if(it == m_properties.end()) {
      return nullptr;
    }
    return it->second;
  }

  /**
   * Get resource.
   * @return - resource.
   */
  virtual ResourceHandle<T> get() = 0;

  /**
   * Get resource in Async manner.
   * @return - &id:oatpp::async::CoroutineStarterForResult; of `T`.
   */
  virtual async::CoroutineStarterForResult<const ResourceHandle<T>&> getAsync() = 0;

  /**
   * Stop provider and free associated resources.
   */
  virtual void stop() = 0;

};

}}

#endif // oatpp_provider_Provider_hpp
