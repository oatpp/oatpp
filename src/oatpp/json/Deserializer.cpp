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

#include "Deserializer.hpp"

#include "oatpp/data/stream/BufferStream.hpp"
#include "oatpp/utils/Conversion.hpp"

namespace oatpp { namespace json {

void Deserializer::deserializeNull(State& state) {
  if(state.caret->isAtText("null", true)){
    state.tree->setNull();
  } else {
    state.errorStack.push("[oatpp::json::Deserializer::deserializeNull()]: 'null' expected");
  }
}

void Deserializer::deserializeNumber(State& state) {
  if (!Utils::findDecimalSeparatorInCurrentNumber(*state.caret)) {
    state.tree->setInteger(state.caret->parseInt());
  } else {
    state.tree->setFloat(state.caret->parseFloat64());
  }
}

void Deserializer::deserializeBoolean(State& state) {
  if(state.caret->isAtText("true", true)) {
    state.tree->setPrimitive<bool>(true);
  } else if(state.caret->isAtText("false", true)) {
    state.tree->setPrimitive<bool>(false);
  } else {
    state.errorStack.push("[oatpp::json::Deserializer::deserializeBoolean()]: 'true' or 'false' expected");
  }
}

void Deserializer::deserializeString(State& state) {
  state.tree->setString(Utils::parseString(*state.caret));
}

void Deserializer::deserializeArray(State& state) {

  if(state.caret->canContinueAtChar('[', 1)) {

    state.tree->setVector(0);
    auto& vector = state.tree->getVector();

    state.caret->skipBlankChars();

    v_int64 index = 0;

    while(!state.caret->isAtChar(']') && state.caret->canContinue()){

      state.caret->skipBlankChars();

      vector.emplace_back();

      State nestedState;
      nestedState.caret = state.caret;
      nestedState.config = state.config;
      nestedState.tree = &vector[vector.size() - 1];

      deserialize(nestedState);

      if(!nestedState.errorStack.empty()) {
        state.errorStack.splice(nestedState.errorStack);
        state.errorStack.push("[oatpp::json::Deserializer::deserializeArray()]: index=" + utils::Conversion::int64ToStr(index));
        return;
      }

      state.caret->skipBlankChars();

      state.caret->canContinueAtChar(',', 1);

      index ++;

    }

    if(!state.caret->canContinueAtChar(']', 1)){
      state.errorStack.push("[oatpp::json::Deserializer::deserializeArray()]: ']' expected");
      return;
    }

  } else {
    state.errorStack.push("[oatpp::json::Deserializer::deserializeArray()]: '[' expected");
  }

}

void Deserializer::deserializeMap(State& state) {

  if(state.caret->canContinueAtChar('{', 1)) {

    state.caret->skipBlankChars();

    state.tree->setMap({});
    auto& map = state.tree->getMap();

    while (!state.caret->isAtChar('}') && state.caret->canContinue()) {

      state.caret->skipBlankChars();

      auto key = Utils::parseString(*state.caret);
      if(state.caret->hasError()){
        state.errorStack.push("[oatpp::json::Deserializer::deserializeMap()]: Item key name expected");
        return;
      }

      state.caret->skipBlankChars();
      if(!state.caret->canContinueAtChar(':', 1)){
        state.errorStack.push("[oatpp::json::Deserializer::deserializeMap()]: ':' expected");
        return;
      }

      state.caret->skipBlankChars();

      State nestedState;
      nestedState.caret = state.caret;
      nestedState.config = state.config;
      nestedState.tree = &map[key];

      deserialize(nestedState);

      if(!nestedState.errorStack.empty()) {
        state.errorStack.splice(nestedState.errorStack);
        state.errorStack.push("[oatpp::json::Deserializer::deserializeMap()]: key='" + key + "'");
        return;
      }

      state.caret->skipBlankChars();
      state.caret->canContinueAtChar(',', 1);

    }

    if(!state.caret->canContinueAtChar('}', 1)){
      state.errorStack.push("[oatpp::json::Deserializer::deserializeMap()]: '}' expected");
      return;
    }

  } else {
    state.errorStack.push("[oatpp::json::Deserializer::deserializeMap()]: '{' expected");
  }

}

void Deserializer::deserialize(State& state) {

  state.caret->skipBlankChars();

  auto c = *state.caret->getCurrData();
  switch (c) {
    case 'n':
      deserializeNull(state);
      break;
    case '{':
      deserializeMap(state);
      break;
    case '[':
      deserializeArray(state);
      break;
    case '"':
      deserializeString(state);
      break;
    case 't':
    case 'f':
      deserializeBoolean(state);
      break;
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      deserializeNumber(state);
      break;
    default:
      state.errorStack.push("[json]: Unknown character.");
      break;
  }

}

}}
