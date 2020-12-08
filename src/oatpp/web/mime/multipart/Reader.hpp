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

#ifndef oatpp_web_mime_multipart_Reader_hpp
#define oatpp_web_mime_multipart_Reader_hpp

#include "Multipart.hpp"
#include "StatefulParser.hpp"

#include <unordered_map>

namespace oatpp { namespace web { namespace mime { namespace multipart {

/**
 * Abstract read handler of multipart parts.
 */
class PartReader {
public:

  /**
   * Default virtual destructor.
   */
  virtual ~PartReader() = default;

  /**
   * Called when new part headers are parsed and part object is created.
   * @param part
   */
  virtual void onNewPart(const std::shared_ptr<Part>& part) = 0;

  /**
   * Called on each new chunk of data is parsed for the multipart-part. <br>
   * When all data is read, called again with `data == nullptr && size == 0` to indicate end of the part.
   * @param part
   * @param data - pointer to buffer containing chunk data.
   * @param size - size of the buffer.
   */
  virtual void onPartData(const std::shared_ptr<Part>& part, p_char8 data, oatpp::v_io_size size) = 0;

};

/**
 * Abstract Async read handler of multipart parts.
 */
class AsyncPartReader {
public:

  /**
   * Default virtual destructor.
   */
  virtual ~AsyncPartReader() = default;

  /**
   * Called when new part headers are parsed and part object is created.
   * @param part
   * @return - &id:oatpp::async::CoroutineStarter;.
   */
  virtual async::CoroutineStarter onNewPartAsync(const std::shared_ptr<Part>& part) = 0;

  /**
   * Called on each new chunk of data is parsed for the multipart-part. <br>
   * When all data is read, called again with `data == nullptr && size == 0` to indicate end of the part.
   * @param part
   * @param data - pointer to buffer containing chunk data.
   * @param size - size of the buffer.
   * @return - &id:oatpp::async::CoroutineStarter;.
   */
  virtual async::CoroutineStarter onPartDataAsync(const std::shared_ptr<Part>& part, p_char8 data, oatpp::v_io_size size) = 0;

};

class Reader; // FWD

/**
 * Map of part readers. `<part-name> --> <part-reader>`.
 */
typedef std::unordered_map<oatpp::String, std::shared_ptr<PartReader>> PartReadersMap;

/**
 * In memory multipart parser. <br>
 * Extends - &id:oatpp::web::mime::multipart::StatefulParser::Listener;.
 */
class PartsParser : public StatefulParser::Listener {
  friend Reader;
private:
  PartReadersMap m_readers;
  std::shared_ptr<PartReader> m_defaultReader;
  std::shared_ptr<PartReader> m_currReader;
  Multipart* m_multipart;
  std::shared_ptr<Part> m_currPart;
public:

  /**
   * Constructor.
   * @param multipart - pointer to &id:oatpp::web::mime::multipart::Multipart;.
   */
  PartsParser(Multipart* multipart);

  PartsParser(Multipart* multipart, const PartReadersMap& readersMap);

  void onPartHeaders(const Headers& partHeaders) override;

  void onPartData(p_char8 data, v_buff_size size) override;

  void setPartReader(const oatpp::String& partName, const std::shared_ptr<PartReader>& reader);

  void setDefaultPartReader(const std::shared_ptr<PartReader>& reader);

};

class AsyncReader; // FWD

/**
 * Map of async part readers. `<part-name> --> <part-reader>`.
 */
typedef std::unordered_map<oatpp::String, std::shared_ptr<AsyncPartReader>> AsyncPartReadersMap;

/**
 * Async In memory multipart parser. <br>
 * Extends - &id:oatpp::web::mime::multipart::StatefulParser::AsyncListener;.
 */
class AsyncPartsParser : public StatefulParser::AsyncListener {
  friend AsyncReader;
private:
  async::CoroutineStarter onPartDone(const std::shared_ptr<Part>& part);
private:
  AsyncPartReadersMap m_readers;
  std::shared_ptr<AsyncPartReader> m_defaultReader;
  std::shared_ptr<AsyncPartReader> m_currReader;
  Multipart* m_multipart;
  std::shared_ptr<Part> m_currPart;
public:

  /**
   * Constructor.
   * @param multipart - pointer to &id:oatpp::web::mime::multipart::Multipart;.
   */
  AsyncPartsParser(Multipart* multipart);

  AsyncPartsParser(Multipart* multipart, const AsyncPartReadersMap& readersMap);

  async::CoroutineStarter onPartHeadersAsync(const Headers& partHeaders) override;

  async::CoroutineStarter onPartDataAsync(p_char8 data, v_buff_size size) override;

  void setPartReader(const oatpp::String& partName, const std::shared_ptr<AsyncPartReader>& reader);

  void setDefaultPartReader(const std::shared_ptr<AsyncPartReader>& reader);

};

/**
 * In memory Multipart reader.
 * Extends - &id:oatpp::data::stream::WriteCallback;.
 */
class Reader : public oatpp::data::stream::WriteCallback {
private:
  std::shared_ptr<PartsParser> m_partsParser;
  StatefulParser m_parser;
public:

  /**
   * Constructor.
   * @param multipart - Multipart object to save read data to.
   */
  Reader(Multipart* multipart);

  v_io_size write(const void *data, v_buff_size count, async::Action& action) override;

  /**
   * Set named part reader. <br>
   * Part with the `name == partName` will be read using the specified `reader`.
   * @param partName - name of the part to read.
   * @param reader - reader to read part with.
   */
  void setPartReader(const oatpp::String& partName, const std::shared_ptr<PartReader>& reader);

  /**
   * Set default reader for parts. <br>
   * `setPartReader` has precedence.
   * @param reader
   */
  void setDefaultPartReader(const std::shared_ptr<PartReader>& reader);

};

/**
 * In memory Multipart reader.
 * Extends - &id:oatpp::data::stream::WriteCallback;.
 */
class AsyncReader : public oatpp::data::stream::WriteCallback {
private:
  std::shared_ptr<AsyncPartsParser> m_partsParser;
  StatefulParser m_parser;
  std::shared_ptr<Multipart> m_multipart;
public:

  /**
   * Constructor.
   * @param multipart - Multipart object to save read data to.
   */
  AsyncReader(const std::shared_ptr<Multipart>& multipart);

  v_io_size write(const void *data, v_buff_size count, async::Action& action) override;

  /**
   * Set named part reader. <br>
   * Part with the `name == partName` will be read using the specified `reader`.
   * @param partName - name of the part to read.
   * @param reader - reader to read part with.
   */
  void setPartReader(const oatpp::String& partName, const std::shared_ptr<AsyncPartReader>& reader);

  /**
   * Set default reader for parts. <br>
   * `setPartReader` has precedence.
   * @param reader
   */
  void setDefaultPartReader(const std::shared_ptr<AsyncPartReader>& reader);

};

}}}}

#endif // oatpp_web_mime_multipart_Reader_hpp
