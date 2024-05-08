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

#ifndef oatpp_json_Deserializer_hpp
#define oatpp_json_Deserializer_hpp

#include "./Utils.hpp"

#include "oatpp/data/mapping/ObjectMapper.hpp"
#include "oatpp/data/mapping/Tree.hpp"

#include "oatpp/utils/parser/Caret.hpp"
#include "oatpp/Types.hpp"

#include <vector>

namespace oatpp { namespace json {

/**
 * Json Deserializer.
 * Deserialize oatpp DTO object from json. See [Data Transfer Object(DTO) component](https://oatpp.io/docs/components/dto/).
 */
class Deserializer {
public:

  /**
   * Deserializer config.
   */
  class Config : public oatpp::base::Countable {
  public:

  };

public:

  struct State {
    const Config* config;
    data::mapping::Tree* tree;
    utils::parser::Caret* caret;
    data::mapping::ErrorStack errorStack;
  };

private:

  static void deserializeNull(State& state);
  static void deserializeNumber(State& state);
  static void deserializeBoolean(State& state);
  static void deserializeString(State& state);

  static void deserializeArray(State& state);
  static void deserializeMap(State& state);

public:

  static void deserialize(State& state);

};

}}

#endif /* oatpp_json_Deserializer_hpp */
