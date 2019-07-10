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

#ifndef oatpp_web_mime_multipart_StatefulParser_hpp
#define oatpp_web_mime_multipart_StatefulParser_hpp

#include "oatpp/core/data/stream/ChunkedBuffer.hpp"
#include "oatpp/core/data/share/MemoryLabel.hpp"
#include "oatpp/core/Types.hpp"

#include <unordered_map>

namespace oatpp { namespace web { namespace mime { namespace multipart {

class StatefulParser {
private:
  static constexpr v_int32 STATE_BOUNDARY = 0;
  static constexpr v_int32 STATE_AFTER_BOUNDARY = 1;
  static constexpr v_int32 STATE_HEADERS = 2;
  static constexpr v_int32 STATE_DATA = 3;
  static constexpr v_int32 STATE_DONE = 4;
private:
  static constexpr v_int32 HEADERS_SECTION_END = ('\r' << 24) | ('\n' << 16) | ('\r' << 8) | ('\n');
public:
  /**
   * Typedef for headers map. Headers map key is case-insensitive.
   * `std::unordered_map` of &id:oatpp::data::share::StringKeyLabelCI_FAST; and &id:oatpp::data::share::StringKeyLabel;.
   */
  typedef std::unordered_map<oatpp::data::share::StringKeyLabelCI_FAST, oatpp::data::share::StringKeyLabel> Headers;
public:

  class Listener {
  public:
    virtual void onPartHeaders(const oatpp::String& name, const Headers& partHeaders) = 0;
    virtual void onPartData(const oatpp::String& name, p_char8 data, oatpp::data::v_io_size size) = 0;
  };

private:

  v_int32 m_state;
  v_int32 m_currPartIndex;
  v_int32 m_currBoundaryCharIndex;
  bool m_checkForBoundary;
  bool m_finishingBoundary;
  bool m_readingBody;

  v_word32 m_headerSectionEndAccumulator;

  oatpp::String m_firstBoundarySample;
  oatpp::String m_nextBoundarySample;
  oatpp::String m_currPartName;

  /*
   * Headers of the part are stored in the buffer and are parsed as one chunk.
   */
  oatpp::data::stream::ChunkedBuffer m_headersBuffer;

private:

  void onPartHeaders(const Headers& partHeaders);
  void onPartData(p_char8 data, v_int32 size);

private:

  v_int32 parseNext_Boundary(p_char8 data, v_int32 size);
  v_int32 parseNext_AfterBoundary(p_char8 data, v_int32 size);
  v_int32 parseNext_Headers(p_char8 data, v_int32 size);
  v_int32 parseNext_Data(p_char8 data, v_int32 size);

public:

  StatefulParser(const oatpp::String& boundary)
    : m_state(STATE_BOUNDARY)
    , m_currPartIndex(0)
    , m_currBoundaryCharIndex(0)
    , m_checkForBoundary(true)
    , m_finishingBoundary(false)
    , m_readingBody(false)
    , m_headerSectionEndAccumulator(0)
    , m_firstBoundarySample("--" + boundary)
    , m_nextBoundarySample("\r\n--" + boundary)
  {}

  v_int32 parseNext(p_char8 data, v_int32 size);


};

}}}}

#endif // oatpp_web_mime_multipart_StatefulParser_hpp
