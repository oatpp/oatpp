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

#ifndef oatpp_data_share_LazyStringMap_hpp
#define oatpp_data_share_LazyStringMap_hpp

#include "./MemoryLabel.hpp"
#include "oatpp/concurrency/SpinLock.hpp"

#include <unordered_map>

namespace oatpp { namespace data { namespace share {

/**
 * Lazy String Map keeps keys, and values as memory label.
 * Once value is requested by user, the new memory block is allocated and value is copied to be stored permanently.
 * @tparam Key - one of: &id:oatpp::data::share::MemoryLabel;, &id:oatpp::data::share::StringKeyLabel;, &id:oatpp::data::share::StringKeyLabelCI;,
 * &id:oatpp::data::share::StringKeyLabelCI;.
 */
template<typename Key, typename MapType>
class LazyStringMapTemplate {
public:
  typedef oatpp::data::type::String String;
private:
  mutable concurrency::SpinLock m_lock;
  mutable bool m_fullyInitialized;
  MapType m_map;
public:

  /**
   * Constructor.
   */
  LazyStringMapTemplate()
    : m_fullyInitialized(true)
  {}

  /**
   * Copy-constructor.
   * @param other
   */
  LazyStringMapTemplate(const LazyStringMapTemplate& other) {

    std::lock_guard<concurrency::SpinLock> otherLock(other.m_lock);

    m_fullyInitialized = other.m_fullyInitialized;
    m_map = MapType(other.m_map);

  }

  /**
   * Move constructor.
   * @param other
   */
  LazyStringMapTemplate(LazyStringMapTemplate&& other) {

    std::lock_guard<concurrency::SpinLock> otherLock(other.m_lock);

    m_fullyInitialized = other.m_fullyInitialized;
    m_map = std::move(other.m_map);

  }

  LazyStringMapTemplate& operator = (const LazyStringMapTemplate& other) {

    if(this != &other) {

      std::lock_guard<concurrency::SpinLock> thisLock(m_lock);
      std::lock_guard<concurrency::SpinLock> otherLock(other.m_lock);

      m_fullyInitialized = other.m_fullyInitialized;
      m_map = MapType(other.m_map);

    }

    return *this;

  }

  LazyStringMapTemplate& operator = (LazyStringMapTemplate&& other) {

    if(this != &other) {

      std::lock_guard<concurrency::SpinLock> thisLock(m_lock);
      std::lock_guard<concurrency::SpinLock> otherLock(other.m_lock);

      m_fullyInitialized = other.m_fullyInitialized;
      m_map = std::move(other.m_map);

    }

    return *this;

  }

  /**
   * Put value to map.
   * @param key
   * @param value
   */
  void put(const Key& key, const StringKeyLabel& value) {

    std::lock_guard<concurrency::SpinLock> lock(m_lock);

    m_map.insert({key, value});
    m_fullyInitialized = false;

  }

  /**
   * Put value to map. Not thread-safe.
   * @param key
   * @param value
   */
  void put_LockFree(const Key& key, const StringKeyLabel& value) {
    m_map.insert({key, value});
    m_fullyInitialized = false;
  }

  /**
   * Put value to map if not already exists.
   * @param key
   * @param value
   * @return
   */
  bool putIfNotExists(const Key& key, const StringKeyLabel& value) {

    std::lock_guard<concurrency::SpinLock> lock(m_lock);

    auto it = m_map.find(key);

    if(it == m_map.end()) {
      m_map.insert({key, value});
      m_fullyInitialized = false;
      return true;
    }

    return false;

  }

  /**
   * Put value to map if not already exists. Not thread-safe.
   * @param key
   * @param value
   * @return
   */
  bool putIfNotExists_LockFree(const Key& key, const StringKeyLabel& value) {

    auto it = m_map.find(key);

    if(it == m_map.end()) {
      m_map.insert({key, value});
      m_fullyInitialized = false;
      return true;
    }

    return false;

  }

  /**
   * Erases all occurrences of key and replaces them with a new entry
   * @param key
   * @param value
   * @return - true if an entry was replaced, false if entry was only inserted.
   */
  bool putOrReplace(const Key& key, const StringKeyLabel& value) {

    std::lock_guard<concurrency::SpinLock> lock(m_lock);

    bool needsErase = m_map.find(key) != m_map.end();
    if (needsErase) {
      m_map.erase(key);
    }
    m_map.insert({key, value});
    m_fullyInitialized = false;

    return needsErase;

  }

  /**
   * Erases all occurrences of key and replaces them with a new entry. Not thread-safe.
   * @param key
   * @param value
   * @return - `true` if an entry was replaced, `false` if entry was only inserted.
   */
  bool putOrReplace_LockFree(const Key& key, const StringKeyLabel& value) {

    bool needsErase = m_map.find(key) != m_map.end();
    if (needsErase) {
      m_map.erase(key);
    }
    m_map.insert({key, value});
    m_fullyInitialized = false;

    return needsErase;

  }

  /**
   * Get value as &id:oatpp::String;.
   * @param key
   * @return
   */
  String get(const Key& key) const {

    std::lock_guard<concurrency::SpinLock> lock(m_lock);

    auto it = m_map.find(key);

    if(it != m_map.end()) {
      it->second.captureToOwnMemory();
      return it->second.getMemoryHandle();
    }

    return nullptr;

  }

  /**
   * Get value as a memory label.
   * @tparam T - one of: &id:oatpp::data::share::MemoryLabel;, &id:oatpp::data::share::StringKeyLabel;, &id:oatpp::data::share::StringKeyLabelCI;.
   * @param key
   * @return
   */
  template<class T>
  T getAsMemoryLabel(const Key& key) const {

    std::lock_guard<concurrency::SpinLock> lock(m_lock);

    auto it = m_map.find(key);

    if(it != m_map.end()) {
      it->second.captureToOwnMemory();
      const auto& label = it->second;
      return T(label.getMemoryHandle(), reinterpret_cast<const char*>(label.getData()), label.getSize());
    }

    return T(nullptr, nullptr, 0);

  }

  /**
   * Get value as a memory label without allocating memory for value.
   * @tparam T - one of: &id:oatpp::data::share::MemoryLabel;, &id:oatpp::data::share::StringKeyLabel;, &id:oatpp::data::share::StringKeyLabelCI;,
   * * &id:oatpp::data::share::StringKeyLabelCI;.
   * @param key
   * @return
   */
  template<class T>
  T getAsMemoryLabel_Unsafe(const Key& key) const {

    std::lock_guard<concurrency::SpinLock> lock(m_lock);

    auto it = m_map.find(key);

    if(it != m_map.end()) {
      const auto& label = it->second;
      return T(label.getMemoryHandle(), reinterpret_cast<const char*>(label.getData()), label.getSize());
    }

    return T(nullptr, nullptr, 0);

  }

  /**
   * Get map of all values.
   * @return
   */
  const MapType& getAll() const {

    std::lock_guard<concurrency::SpinLock> lock(m_lock);

    if(!m_fullyInitialized) {

      for(auto& pair : m_map) {
        pair.first.captureToOwnMemory();
        pair.second.captureToOwnMemory();
      }

      m_fullyInitialized = true;
    }

    return m_map;

  }

  /**
   * Get map of all values without allocating memory for those keys/values.
   * @return
   */
  const MapType& getAll_Unsafe() const {
    return m_map;
  }

  /**
   * Get number of entries in the map.
   * @return
   */
  v_int32 getSize() const {
    std::lock_guard<concurrency::SpinLock> lock(m_lock);
    return static_cast<v_int32>(m_map.size());
  }

};

/**
 * Convenience template for &l:LazyStringMapTemplate;. Based on `std::unordered_map`.
 */
template<typename Key, typename Value = StringKeyLabel>
using LazyStringMap = LazyStringMapTemplate<Key, std::unordered_map<Key, Value>>;

/**
 * Convenience template for &l:LazyStringMapTemplate;. Based on `std::unordered_map`.
 */
template<typename Key, typename Value = StringKeyLabel>
using LazyStringMultimap = LazyStringMapTemplate<Key, std::unordered_multimap<Key, Value>>;

}}}

#endif //oatpp_data_share_LazyStringMap_hpp
