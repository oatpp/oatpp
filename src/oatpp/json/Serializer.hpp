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

#ifndef oatpp_json_Serializer_hpp
#define oatpp_json_Serializer_hpp

#include "./Utils.hpp"
#include "./Beautifier.hpp"

#include "oatpp/data/mapping/ObjectMapper.hpp"
#include "oatpp/data/mapping/Tree.hpp"
#include "oatpp/Types.hpp"

namespace oatpp { namespace json {

/**
 * Json Serializer.
 * Serializes oatpp DTO object to json. See [Data Transfer Object(DTO) component](https://oatpp.io/docs/components/dto/).
 */
class Serializer {
public:

  /**
   * Serializer config.
   */
  class Config : public oatpp::base::Countable {
  public:

    /**
     * Include fields with value == nullptr into serialized json.
     * Field will still be included when field-info `required` is set to true and &id:alwaysIncludeRequired is set to true.
     */
    bool includeNullElements = true;

    /**
     * Use JSON Beautifier.
     */
    bool useBeautifier = false;

    /**
     * Beautifier Indent.
     */
    oatpp::String beautifierIndent = "  ";

    /**
     * Beautifier new line.
     */
    oatpp::String beautifierNewLine = "\n";

    /**
     * Escape flags.
     */
    v_uint32 escapeFlags = json::Utils::FLAG_ESCAPE_ALL;

  };

public:

  struct State {

    const Config* config;
    const data::mapping::Tree* tree;
    data::stream::ConsistentOutputStream* stream;

    data::mapping::ErrorStack errorStack;

  };

private:

  
  static void serializeString(oatpp::data::stream::ConsistentOutputStream* stream,
                              const char* data,
                              v_buff_size size,
                              v_uint32 escapeFlags);

  static void serializeNull(State& state);
  static void serializeString(State& state);
  static void serializeArray(State& state);
  static void serializeMap(State& state);
  static void serializePairs(State& state);

  static void serialize(State& state);

public:

  static void serializeToStream(data::stream::ConsistentOutputStream* stream, State& state);

};

}}

#endif /* oatpp_json_Serializer_hpp */
