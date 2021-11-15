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

#include "oatpp/core/data/stream/BufferStream.hpp"
#include "oatpp/core/data/share/LazyStringMap.hpp"
#include "oatpp/core/Types.hpp"

#include <unordered_map>

namespace oatpp { namespace web { namespace mime { namespace multipart {

/**
 * Stateful parser of multipart-data stream.
 * Parser designed to work with stream-like data in order to store minimum data in the memory.
 */
class StatefulParser {
private:
  static constexpr v_int32 STATE_BOUNDARY = 0;
  static constexpr v_int32 STATE_AFTER_BOUNDARY = 1;
  static constexpr v_int32 STATE_HEADERS = 2;
  static constexpr v_int32 STATE_DATA = 3;
  static constexpr v_int32 STATE_DONE = 4;
private:
  static constexpr v_uint32 HEADERS_SECTION_END = ('\r' << 24) | ('\n' << 16) | ('\r' << 8) | ('\n');
private:
  /**
   * Typedef for headers map. Headers map key is case-insensitive.
   * For more info see &id:oatpp::data::share::LazyStringMap;.
   */
  typedef oatpp::data::share::LazyStringMultimap<oatpp::data::share::StringKeyLabelCI> Headers;
public:

  /**
   * Listener for parsed items.
   */
  class Listener {
  public:
    /**
     * Typedef for headers map. Headers map key is case-insensitive.
     * For more info see &id:oatpp::data::share::LazyStringMap;.
     */
    typedef oatpp::data::share::LazyStringMultimap<oatpp::data::share::StringKeyLabelCI> Headers;
  public:

    /**
     * Default virtual Destructor.
     */
    virtual ~Listener() = default;

    /**
     * Called on new part found in the stream.
     * Always called before `onPartData` events.
     * @param partHeaders - complete set of part headers.
     */
    virtual void onPartHeaders(const Headers& partHeaders) = 0;

    /**
     * Called on each new chunk of bytes parsed from the part body.
     * When all data of message is read, readMessage is called again with size == 0 to
     * indicate end of the part.
     * @param data - pointer to data.
     * @param size - size of the data in bytes.
     */
    virtual void onPartData(const char* data, v_buff_size size) = 0;

  };

public:

  /**
   * Async Listener for parsed items.
   */
  class AsyncListener {
  public:
    /**
     * Typedef for headers map. Headers map key is case-insensitive.
     * For more info see &id:oatpp::data::share::LazyStringMap;.
     */
    typedef oatpp::data::share::LazyStringMultimap<oatpp::data::share::StringKeyLabelCI> Headers;
  public:

    /**
     * Default virtual Destructor.
     */
    virtual ~AsyncListener() = default;

    /**
     * Called on new part found in the stream.
     * Always called before `onPartData` events.
     * @param partHeaders - complete set of part headers.
     */
    virtual async::CoroutineStarter onPartHeadersAsync(const Headers& partHeaders) = 0;

    /**
     * Called on each new chunk of bytes parsed from the part body.
     * When all data of message is read, readMessage is called again with size == 0 to
     * indicate end of the part.
     * @param data - pointer to data.
     * @param size - size of the data in bytes.
     */
    virtual async::CoroutineStarter onPartDataAsync(const char* data, v_buff_size size) = 0;

  };

private:

  class ListenerCall {
  public:

    static constexpr v_int32 CALL_NONE = 0;
    static constexpr v_int32 CALL_ON_HEADERS = 1;
    static constexpr v_int32 CALL_ON_DATA = 2;

  public:

    ListenerCall()
      : callType(CALL_NONE)
      , data(nullptr)
      , size(0)
    {}

    v_int32 callType;
    const char* data;
    v_io_size size;

    void setOnHeadersCall();
    void setOnDataCall(const char* pData, v_buff_size pSize);

    void call(StatefulParser* parser);
    async::CoroutineStarter callAsync(StatefulParser* parser);

    explicit operator bool() const;

  };

private:

  v_int32 m_state;
  v_int64 m_currPartIndex;
  v_buff_size m_currBoundaryCharIndex;
  bool m_checkForBoundary;
  bool m_finishingBoundary;
  bool m_readingBody;

  v_uint32 m_headerSectionEndAccumulator;

  oatpp::String m_firstBoundarySample;
  oatpp::String m_nextBoundarySample;

  /*
   * Headers of the part are stored in the buffer and are parsed as one chunk.
   */
  data::stream::BufferOutputStream m_headersBuffer;

  /*
   * Max length of all headers per one part.
   * Default value = 4096 bytes.
   */
  v_buff_size m_maxPartHeadersSize;

  std::shared_ptr<Listener> m_listener;
  std::shared_ptr<AsyncListener> m_asyncListener;

private:

  void parseHeaders(Headers& headers);

private:

  ListenerCall parseNext_Boundary(data::buffer::InlineWriteData& inlineData);
  void         parseNext_AfterBoundary(data::buffer::InlineWriteData& inlineData);
  ListenerCall parseNext_Headers(data::buffer::InlineWriteData& inlineData);
  ListenerCall parseNext_Data(data::buffer::InlineWriteData& inlineData);

public:

  /**
   * Constructor.
   * @param boundary - value of multipart boundary.
   * @param listener - &l:StatefulParser::Listener;.
   * @param asyncListener - &l:StatefulParser::AsyncListener;.
   */
  StatefulParser(const oatpp::String& boundary,
                 const std::shared_ptr<Listener>& listener,
                 const std::shared_ptr<AsyncListener>& asyncListener);

  /**
   * Parse next chunk of bytes.
   * @param inlineData - inline data.
   * @param action - Async Action in case Async Listener was provided in constructor.
   */
  void parseNext(data::buffer::InlineWriteData& inlineData, async::Action& action);

  /**
   * Check if parser done parsing data.
   * @return - `true` or `false`.
   */
  bool finished();

};

}}}}

#endif // oatpp_web_mime_multipart_StatefulParser_hpp
