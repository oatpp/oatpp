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

#ifndef oatpp_data_Bundle_hpp
#define oatpp_data_Bundle_hpp

#include "oatpp/Types.hpp"

namespace oatpp { namespace data {

/**
 * Bundle of auxiliary data.
 */
class Bundle {
private:
  std::unordered_map<oatpp::String, oatpp::Void> m_data;
public:

  /**
   * Default constructor.
   */
  Bundle() = default;

  /**
   * Default virtual destructor.
   */
  virtual ~Bundle() = default;

  /**
   * Put data by key.
   * @param key
   * @param polymorph
   */
  void put(const oatpp::String& key, const oatpp::Void& polymorph);

  /**
   * Get data by key.
   * @tparam WrapperType
   * @param key
   * @return
   */
  template<typename WrapperType>
  WrapperType get(const oatpp::String& key) const {
    auto it = m_data.find(key);
    if(it == m_data.end()) {
      throw std::runtime_error("[oatpp::data::Bundle::get()]: "
                               "Error. Data not found for key '" + *key + "'.");
    }

    if(!WrapperType::Class::getType()->extends(it->second.getValueType())) {
      throw std::runtime_error("[oatpp::data::Bundle::get()]: Error. Type mismatch for key '" + *key +
                               "'. Stored '" +
                               std::string(it->second.getValueType()->classId.name) +
                               "' vs requested '" + std::string(WrapperType::Class::getType()->classId.name) + "'.");
    }
    return it->second.template cast<WrapperType>();
  }

  /**
   * Get map of data stored in the bundle.
   * @return
   */
  const std::unordered_map<oatpp::String, oatpp::Void>& getAll() const;

};

}}

#endif //oatpp_data_Bundle_hpp
