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

#include "Serializer.hpp"

#include "./Utils.hpp"
#include "oatpp/data/stream/BufferStream.hpp"
#include "oatpp/utils/Conversion.hpp"

namespace oatpp { namespace json {

void Serializer::serializeString(data::stream::ConsistentOutputStream* stream, const char* data, v_buff_size size, v_uint32 escapeFlags) {
  auto encodedValue = Utils::escapeString(data, size, escapeFlags);
  stream->writeCharSimple('\"');
  stream->writeSimple(encodedValue);
  stream->writeCharSimple('\"');
}

void Serializer::serializeNull(State& state) {
  state.stream->writeSimple("null");
}

void Serializer::serializeString(State& state) {
  const auto& str = state.tree->getString();
  serializeString(state.stream, str->data(), static_cast<v_buff_size>(str->size()), state.config->escapeFlags);
}

void Serializer::serializeArray(State& state) {

  state.stream->writeCharSimple('[');

  State nestedState;
  nestedState.stream = state.stream;
  nestedState.config = state.config;

  auto& vector = state.tree->getVector();

  v_int64 index = 0;
  for(auto& tree : vector) {

    nestedState.tree = &tree;

    if(!tree.isNull() || state.config->includeNullElements) {

      if(index > 0) state.stream->writeSimple(",", 1);

      serialize(nestedState);

      if(!nestedState.errorStack.empty()) {
        state.errorStack.splice(nestedState.errorStack);
        state.errorStack.push("[oatpp::json::Serializer::serializeArray()]: index=" + utils::Conversion::int64ToStr(index));
        return;
      }
    }

    index ++;

  }

  state.stream->writeCharSimple(']');

}

void Serializer::serializeMap(State& state) {

  state.stream->writeCharSimple('{');

  State nestedState;
  nestedState.stream = state.stream;
  nestedState.config = state.config;

  auto& map = state.tree->getMap();
  auto mapSize = map.size();

  for(v_uint64 index = 0; index < mapSize; index ++) {

    const auto& pair = map[index];

    nestedState.tree = &pair.second.get();

    if(!nestedState.tree->isNull() || state.config->includeNullElements) {

      if(index > 0) state.stream->writeSimple(",", 1);

      const auto& str = pair.first;
      serializeString(state.stream, str->data(), static_cast<v_buff_size>(str->size()), state.config->escapeFlags);
      state.stream->writeCharSimple(':');

      serialize(nestedState);

      if(!nestedState.errorStack.empty()) {
        state.errorStack.splice(nestedState.errorStack);
        state.errorStack.push("[oatpp::json::Serializer::serializeMap()]: key='" + pair.first + "'");
        return;
      }
    }

  }

  state.stream->writeCharSimple('}');

}

void Serializer::serializePairs(State& state) {

  state.stream->writeCharSimple('{');

  State nestedState;
  nestedState.stream = state.stream;
  nestedState.config = state.config;

  auto& map = state.tree->getPairs();
  auto mapSize = map.size();

  for(v_uint64 index = 0; index < mapSize; index ++) {

    const auto& pair = map[index];

    nestedState.tree = &pair.second;

    if(!nestedState.tree->isNull() || state.config->includeNullElements) {

      if(index > 0) state.stream->writeSimple(",", 1);

      const auto& str = pair.first;
      serializeString(state.stream, str->data(), static_cast<v_buff_size>(str->size()), state.config->escapeFlags);
      state.stream->writeCharSimple(':');

      serialize(nestedState);

      if(!nestedState.errorStack.empty()) {
        state.errorStack.splice(nestedState.errorStack);
        state.errorStack.push("[oatpp::json::Serializer::serializePairs()]: key='" + pair.first + "'");
        return;
      }
    }

  }

  state.stream->writeCharSimple('}');

}

void Serializer::serialize(State& state) {

  switch (state.tree->getType()) {

    case data::mapping::Tree::Type::UNDEFINED:
      state.errorStack.push("[oatpp::json::Serializer::serialize()]: "
                            "UNDEFINED tree node is NOT serializable. To fix: set node value.");
      return;
    case data::mapping::Tree::Type::NULL_VALUE: serializeNull(state); return;

    case data::mapping::Tree::Type::INTEGER: state.stream->writeAsString(state.tree->getInteger()); return;
    case data::mapping::Tree::Type::FLOAT: state.stream->writeAsString(state.tree->getFloat()); return;

    case data::mapping::Tree::Type::BOOL:  state.stream->writeAsString(state.tree->getPrimitive<bool>()); return;

    case data::mapping::Tree::Type::INT_8: state.stream->writeAsString(state.tree->getPrimitive<v_int8>()); return;
    case data::mapping::Tree::Type::UINT_8: state.stream->writeAsString(state.tree->getPrimitive<v_uint8>()); return;
    case data::mapping::Tree::Type::INT_16: state.stream->writeAsString(state.tree->getPrimitive<v_int16>()); return;
    case data::mapping::Tree::Type::UINT_16: state.stream->writeAsString(state.tree->getPrimitive<v_uint16>()); return;
    case data::mapping::Tree::Type::INT_32: state.stream->writeAsString(state.tree->getPrimitive<v_int32>()); return;
    case data::mapping::Tree::Type::UINT_32: state.stream->writeAsString(state.tree->getPrimitive<v_uint32>()); return;
    case data::mapping::Tree::Type::INT_64: state.stream->writeAsString(state.tree->getPrimitive<v_int64>()); return;
    case data::mapping::Tree::Type::UINT_64: state.stream->writeAsString(state.tree->getPrimitive<v_uint64>()); return;

    case data::mapping::Tree::Type::FLOAT_32: state.stream->writeAsString(state.tree->getPrimitive<v_float32>()); return;
    case data::mapping::Tree::Type::FLOAT_64: state.stream->writeAsString(state.tree->getPrimitive<v_float64>()); return;

    case data::mapping::Tree::Type::STRING: serializeString(state); return;
    case data::mapping::Tree::Type::VECTOR: serializeArray(state); return;
    case data::mapping::Tree::Type::MAP: serializeMap(state); return;
    case data::mapping::Tree::Type::PAIRS: serializePairs(state); return;

    default:
      break;

  }

  state.errorStack.push("[oatpp::json::Serializer::serialize()]: Unknown node type");

}

void Serializer::serializeToStream(data::stream::ConsistentOutputStream* stream, State& state) {

  if(state.config->useBeautifier) {

    json::Beautifier beautifier(stream, "  ", "\n");

    State beautifulState;
    beautifulState.stream = &beautifier;
    beautifulState.tree = state.tree;
    beautifulState.config = state.config;
    serialize(beautifulState);

    state.errorStack = std::move(beautifulState.errorStack);

  } else {
    state.stream = stream;
    serialize(state);
  }

}

}}
