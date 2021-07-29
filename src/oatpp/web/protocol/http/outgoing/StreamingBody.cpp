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

#include "StreamingBody.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace outgoing {

StreamingBody::StreamingBody(const std::shared_ptr<data::stream::ReadCallback>& readCallback)
  : m_readCallback(readCallback)
{}

v_io_size StreamingBody::read(void *buffer, v_buff_size count, async::Action& action) {
  return m_readCallback->read(buffer, count, action);
}

void StreamingBody::declareHeaders(Headers& headers) {
  (void) headers;
  // DO NOTHING
}

p_char8 StreamingBody::getKnownData() {
  return nullptr;
}


v_int64 StreamingBody::getKnownSize() {
  return -1;
}

}}}}}
