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
#include <unordered_map>

namespace oatpp { namespace data { namespace share {

/**
 * Lazy String Map keeps keys, and values as memory label.
 * Once value is requested by user, the new memory block is allocated and value is copied to be stored permanently.
 * @tparam Key - one of: &id:oatpp::data::share::MemoryLabel;, &id:oatpp::data::share::StringKeyLabel;, &id:oatpp::data::share::StringKeyLabelCI;,
 * &id:oatpp::data::share::StringKeyLabelCI_FAST;.
 */
template<class Key>
class LazyStringMap {
private:
  mutable bool m_fullyInitialized;
  std::unordered_map<Key, StringKeyLabel> m_map;
public:

  /**
   * Constructor.
   */
  LazyStringMap()
    : m_fullyInitialized(false)
  {}

  /**
   * Default copy-constructor.
   * @param other
   */
  LazyStringMap(const LazyStringMap& other) = default;

  /**
   * Move constructor.
   * @param other
   */
  LazyStringMap(LazyStringMap&& other)
    : m_fullyInitialized(false)
    , m_map(std::move(other.m_map))
  {}

  LazyStringMap& operator = (LazyStringMap& other) = default;

  LazyStringMap& operator = (LazyStringMap&& other){
    m_fullyInitialized = false;
    m_map = std::move(other.m_map);
    return *this;
  }

  /**
   * Put value to map.
   * @param key
   * @param value
   */
  void put(const Key& key, const StringKeyLabel& value) {
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

    auto it = m_map.find(key);

    if(it == m_map.end()) {
      m_map.insert({key, value});
      m_fullyInitialized = false;
      return true;
    }

    return false;

  }

  /**
   * Get value as &id:oatpp::String;.
   * @param key
   * @return
   */
  oatpp::String get(const Key& key) const {

    auto it = m_map.find(key);

    if(it != m_map.end()) {
      it->second.captureToOwnMemory();
      return it->second.getMemoryHandle();
    }

    return nullptr;

  }

  /**
   * Get value as a memory label.
   * @tparam T - one of: &id:oatpp::data::share::MemoryLabel;, &id:oatpp::data::share::StringKeyLabel;, &id:oatpp::data::share::StringKeyLabelCI;,
 * &id:oatpp::data::share::StringKeyLabelCI_FAST;.
   * @param key
   * @return
   */
  template<class T>
  T getAsMemoryLabel(const Key& key) const {

    auto it = m_map.find(key);

    if(it != m_map.end()) {
      it->second.captureToOwnMemory();
      const auto& label = it->second;
      return T(label.getMemoryHandle(), label.getData(), label.getSize());
    }

    return T(nullptr, nullptr, 0);

  }

  /**
   * Get value as a memory label without allocating memory for value.
   * @tparam T - one of: &id:oatpp::data::share::MemoryLabel;, &id:oatpp::data::share::StringKeyLabel;, &id:oatpp::data::share::StringKeyLabelCI;,
 * &id:oatpp::data::share::StringKeyLabelCI_FAST;.
   * @param key
   * @return
   */
  template<class T>
  T getAsMemoryLabel_Unsafe(const Key& key) const {

    auto it = m_map.find(key);

    if(it != m_map.end()) {
      const auto& label = it->second;
      return T(label.getMemoryHandle(), label.getData(), label.getSize());
    }

    return T(nullptr, nullptr, 0);

  }

  /**
   * Get map of all values.
   * @return
   */
  const std::unordered_map<Key, StringKeyLabel>& getAll() const {

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
  const std::unordered_map<Key, StringKeyLabel>& getAll_Unsafe() const {
    return m_map;
  }

  /**
   * Get number of entries in the map.
   * @return
   */
  v_int32 getSize() const {
    return (v_int32) m_map.size();
  }

};

}}}

#endif //oatpp_data_share_LazyStringMap_hpp
