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

#ifndef oatpp_web_mime_multipart_InMemoryReader_hpp
#define oatpp_web_mime_multipart_InMemoryReader_hpp

#include "Multipart.hpp"
#include "StatefulParser.hpp"
#include "oatpp/core/data/stream/ChunkedBuffer.hpp"

namespace oatpp { namespace web { namespace mime { namespace multipart {

/**
 * In memory multipart parser. <br>
 * Extends - &id:oatpp::web::mime::multipart::StatefulParser::Listener;.
 */
class InMemoryParser : public StatefulParser::Listener {
private:
  Multipart* m_multipart;
  std::shared_ptr<Part> m_currPart;
  data::stream::ChunkedBuffer m_buffer;
public:

  /**
   * Constructor.
   * @param multipart - pointer to &id:oatpp::web::mime::multipart::Multipart;.
   */
  InMemoryParser(Multipart* multipart);

  void onPartHeaders(const Headers& partHeaders) override;

  void onPartData(p_char8 data, oatpp::data::v_io_size size) override;
};

/**
 * In memory Multipart reader.
 * Extends - &id:oatpp::data::stream::WriteCallback;.
 */
class InMemoryReader : public oatpp::data::stream::WriteCallback {
private:
  StatefulParser m_parser;
public:

  /**
   * Constructor.
   * @param multipart - Multipart object to save read data to.
   */
  InMemoryReader(Multipart* multipart);

  data::v_io_size write(const void *data, data::v_io_size count) override;

};

/**
 * In memory Multipart reader.
 * Extends - &id:oatpp::data::stream::AsyncWriteCallback;.
 */
class AsyncInMemoryReader : public oatpp::data::stream::AsyncWriteCallback {
private:
  StatefulParser m_parser;
  std::shared_ptr<Multipart> m_multipart;
public:

  /**
   * Constructor.
   * @param multipart - Multipart object to save read data to.
   */
  AsyncInMemoryReader(const std::shared_ptr<Multipart>& multipart);

  oatpp::async::Action writeAsyncInline(oatpp::async::AbstractCoroutine* coroutine,
                                        oatpp::data::stream::AsyncInlineWriteData& inlineData,
                                        oatpp::async::Action&& nextAction) override;
};

}}}}

#endif //oatpp_web_mime_multipart_InMemoryReader_hpp
