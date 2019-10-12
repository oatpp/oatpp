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

class LazyStringMap {
public:
  typedef StringKeyLabelCI Key;
private:
  bool m_fullyInitialized;
  std::unordered_map<Key, StringKeyLabel> m_map;
public:

  LazyStringMap()
    : m_fullyInitialized(false)
  {}

  LazyStringMap(LazyStringMap&& other)
    : m_fullyInitialized(false)
    , m_map(std::move(other.m_map))
  {}

  LazyStringMap& operator = (LazyStringMap&& other){
    m_fullyInitialized = false;
    m_map = std::move(other.m_map);
    return *this;
  }

  void put(const Key& key, const StringKeyLabel& value) {
    m_map.insert({key, value});
  }

  bool putIfNotExists(const Key& key, const StringKeyLabel& value) {

    auto it = m_map.find(key);

    if(it == m_map.end()) {
      m_map.insert({key, value});
      return true;
    }

    return false;

  }

  oatpp::String get(const Key& key) {

    auto it = m_map.find(key);

    if(it != m_map.end()) {
      it->second.captureToOwnMemory();
      return it->second.getMemoryHandle();
    }

    return nullptr;

  }

  const std::unordered_map<Key, StringKeyLabel>& getAll() {

    if(!m_fullyInitialized) {

      for(auto& pair : m_map) {
        pair.first.captureToOwnMemory();
        pair.second.captureToOwnMemory();
      }

      m_fullyInitialized = true;
    }

    return m_map;

  }

};

}}}

#endif //oatpp_data_share_LazyStringMap_hpp
