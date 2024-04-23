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

#ifndef oatpp_web_protocol_http_outgoing_MultipartBody_hpp
#define oatpp_web_protocol_http_outgoing_MultipartBody_hpp

#include "./Body.hpp"

#include "oatpp/web/mime/multipart/Multipart.hpp"
#include "oatpp/data/stream/BufferStream.hpp"

namespace oatpp { namespace web { namespace protocol { namespace http { namespace outgoing {

class MultipartBody : public oatpp::base::Countable, public Body {
public:

  /**
   * Convenience typedef for &id:oatpp::web::mime::multipart::Multipart;.
   */
  typedef oatpp::web::mime::multipart::Multipart Multipart;

  /**
   * Convenience typedef for &id:oatpp::web::mime::multipart::Part;.
   */
  typedef oatpp::web::mime::multipart::Part Part;
private:

  /*
   * Convenience typedef for &id:oatpp::data::stream::ReadCallback;.
   */
  typedef oatpp::data::stream::ReadCallback ReadCallback;

private:

  static constexpr v_int32 STATE_BOUNDARY = 0;
  static constexpr v_int32 STATE_HEADERS = 1;
  static constexpr v_int32 STATE_BODY = 2;
  static constexpr v_int32 STATE_INC_PART = 3;
  static constexpr v_int32 STATE_ROUND = 4; // number of possible states. used to round the state.
  static constexpr v_int32 STATE_FINISHED = 5;

private:

  class PartIterator {
  private:
    std::shared_ptr<Multipart> m_multipart;
    std::shared_ptr<Part> m_part;
    std::shared_ptr<data::stream::InputStream> m_partInputStream;
    bool m_isFirst;
    bool m_initialized;
  public:

    PartIterator(const std::shared_ptr<Multipart>& multipart)
      : m_multipart(multipart)
      , m_part(nullptr)
      , m_partInputStream(nullptr)
      , m_isFirst(true)
      , m_initialized(false)
    {}

    void init(async::Action& action) {
      if(!m_initialized) {
        m_part = m_multipart->readNextPart(action);
        m_partInputStream = nullptr;
        if(m_part) {
          auto payload = m_part->getPayload();
          if (payload) {
            m_partInputStream = payload->openInputStream();
          }
        }
        m_initialized = true;
      }
    }

    void inc(async::Action& action) {
      m_part = m_multipart->readNextPart(action);
      m_partInputStream = nullptr;
      if(m_part) {
        auto payload = m_part->getPayload();
        if (payload) {
          m_partInputStream = payload->openInputStream();
        }
      }
      m_isFirst = false;
    }

    bool finished() {
      return m_part == nullptr;
    }

    bool isFirst() {
      return m_isFirst;
    }

    std::shared_ptr<Part> get() {
      return m_part;
    }

    std::shared_ptr<data::stream::InputStream> getPartInputStream() {
      return m_partInputStream;
    }

  };

private:

  static v_io_size readBoundary(const std::shared_ptr<Multipart>& multipart,
                                PartIterator& iterator,
                                data::stream::BufferInputStream& readStream,
                                void *buffer,
                                v_buff_size count);

  static v_io_size readHeaders(const std::shared_ptr<Multipart>& multipart,
                               PartIterator& iterator,
                               data::stream::BufferInputStream& readStream,
                               void *buffer,
                               v_buff_size count);

private:
  std::shared_ptr<Multipart> m_multipart;
  oatpp::String m_contentType;
private:
  PartIterator m_iterator;
  v_int32 m_state;
  oatpp::data::stream::BufferInputStream m_readStream;
  bool m_flushParts;
private:
  v_io_size readBody(void *buffer, v_buff_size count, async::Action& action);
  v_io_size incPart(async::Action& action);
public:

  /**
   * Constructor.
   * @param multipart - multipart object.
   * @param contentType - type of the multipart. Default value = `"multipart/form-data"`.
   * @param flushParts - flush data part by part.
   */
  MultipartBody(const std::shared_ptr<Multipart>& multipart,
                const oatpp::String& contentType = "multipart/form-data",
                bool flushParts = false);

  /**
   * Read operation callback.
   * @param buffer - pointer to buffer.
   * @param count - size of the buffer in bytes.
   * @param action - async specific action. If action is NOT &id:oatpp::async::Action::TYPE_NONE;, then
   * caller MUST return this action on coroutine iteration.
   * @return - actual number of bytes written to buffer. 0 - to indicate end-of-file.
   */
  v_io_size read(void *buffer, v_buff_size count, async::Action& action) override;

  /**
   * Declare `Transfer-Encoding: chunked`, `Content-Type: multipart/<type>` header.
   * @param headers - &id:oatpp::web::protocol::http::Headers;.
   */
  void declareHeaders(Headers& headers) override;

  /**
   * Pointer to the body known data.
   * @return - `p_char8`.
   */
  p_char8 getKnownData() override;

  /**
   * Always returns `-1` - as body size is unknown.
   * @return - `-1`. `v_buff_size`.
   */
  v_int64 getKnownSize() override;

};

}}}}}

#endif // oatpp_web_protocol_http_outgoing_MultipartBody_hpp
