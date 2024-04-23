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

#include "Url.hpp"

#include "oatpp/data/stream/BufferStream.hpp"

namespace oatpp { namespace encoding {

Url::Config::Config() {

  disallowCharRange(0, 255);

  allowCharRange('0', '9');
  allowCharRange('a', 'z');
  allowCharRange('A', 'Z');

  allowChar('-');
  allowChar('.');
  allowChar('_');
  allowChar('~');

}

void Url::Config::allowChar(v_char8 c) {
  allowedChars[c] = true;
}

void Url::Config::allowCharRange(v_char8 from, v_char8 to) {
  for(v_int32 i = from; i <= to; i++) {
    allowedChars[i] = true;
  }
}

void Url::Config::disallowChar(v_char8 c) {
  allowedChars[c] = false;
}

void Url::Config::disallowCharRange(v_char8 from, v_char8 to) {
  for(v_int32 i = from; i <= to; i++) {
    allowedChars[i] = false;
  }
}

void Url::encode(data::stream::ConsistentOutputStream *stream, const void *data, v_buff_size size, const Config& config) {

  auto pdata = reinterpret_cast<const char*>(data);

  for(v_buff_size i = 0; i < size; i++) {
    v_char8 c = static_cast<v_char8>(pdata[i]);
    if(config.allowedChars[c]) {
      stream->writeCharSimple(c);
    } else if(c == ' ' && config.spaceToPlus) {
      stream->writeCharSimple('+');
    } else {
        stream->writeCharSimple('%');
        Hex::encode(stream, pdata + i, 1, config.hexAlphabet);
    }
  }

}

void Url::decode(data::stream::ConsistentOutputStream* stream, const void* data, v_buff_size size) {

  auto pdata = reinterpret_cast<const char*>(data);
  v_buff_size i = 0;

  while (i < size) {

    v_char8 c = static_cast<v_char8>(pdata[i]);
    if(c == '%') {
      if(size - i > 1) {
        Hex::decode(stream, pdata + i + 1, 2);
        i += 3;
      } else {
        break;
      }
    } else if (c == '+') {
      stream->writeCharSimple(' ');
      i ++;
    } else {
        stream->writeCharSimple(c);
        i ++;
    }

  }

}

oatpp::String Url::encode(const oatpp::String& data, const Config& config) {
  if(!data) return nullptr;
  data::stream::BufferOutputStream stream(static_cast<v_buff_size>(data->size() * 3));
  encode(&stream, data->data(), static_cast<v_buff_size>(data->size()), config);
  return stream.toString();
}

oatpp::String Url::decode(const oatpp::String& data) {
  if(!data) return nullptr;
  data::stream::BufferOutputStream stream(static_cast<v_buff_size>(data->size()));
  decode(&stream, data->data(), static_cast<v_buff_size>(data->size()));
  return stream.toString();
}

}}
