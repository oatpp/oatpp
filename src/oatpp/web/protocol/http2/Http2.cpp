/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Benedikt-Alexander Mokroß <github@bamkrs.de>
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

#include "Http2.hpp"
#include "hpack/Hpack.hpp"


namespace oatpp { namespace web { namespace protocol { namespace http2 {

Frame::FrameHeader::FrameHeader(v_int32 length, v_uint8 flags, v_uint8 type, v_uint32 streamId) {
  m_data[2] = length & 0xFF;
  m_data[1] = (length >> 8) & 0xFF;
  m_data[0] = (length >> 16) & 0xFF;
  m_data[3] = type;
  m_data[4] = flags;
  m_data[8] = streamId & 0xFF;
  m_data[7] = (streamId >> 8) & 0xFF;
  m_data[6] = (streamId >> 16) & 0xFF;
  m_data[5] = (streamId >> 24) & 0x7F;
}

std::list<Frame> Frame::createHeaderFrames(v_uint32 streamId, const Headers &hdr, v_io_size maxFrameSize) {
//  std::list<Frame> frames;
//  hpack::Hpack hp(hdr, maxFrameSize);
//  auto first = hp.GetCompressedHeaders();
//  // v_int32 length, v_uint8 flags, v_uint8 type, v_uint32 streamId, std::vector<v_uint8> payload
//  frames.emplace_back(first.size(), 0, Frame::FrameHeader::Types::HEADERS, streamId, first);
//  while (hp.HasContinuation()) {
//    frames.emplace_back(first.size(), 0, Frame::FrameHeader::Types::CONTINUATION, streamId, first);
//  }
//  return frames;
}

}}}}
