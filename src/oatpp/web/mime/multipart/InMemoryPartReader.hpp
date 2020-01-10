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

#ifndef oatpp_web_mime_multipart_InMemoryPartReader_hpp
#define oatpp_web_mime_multipart_InMemoryPartReader_hpp

#include "Reader.hpp"

namespace oatpp { namespace web { namespace mime { namespace multipart {

/**
 * In memory part reader.
 */
class InMemoryPartReader : public PartReader {
private:
  static const char* const TAG_NAME;
private:
  oatpp::v_io_size m_maxDataSize;
public:

  /**
   * Constructor.
   * @param maxDataSize
   */
  InMemoryPartReader(v_io_size maxDataSize = 64 * 1024);

  /**
   * Called when new part headers are parsed and part object is created.
   * @param part
   */
  void onNewPart(const std::shared_ptr<Part>& part) override;

  /**
   * Called on each new chunk of data is parsed for the multipart-part. <br>
   * When all data is read, called again with `data == nullptr && size == 0` to indicate end of the part.
   * @param part
   * @param data - pointer to buffer containing chunk data.
   * @param size - size of the buffer.
   */
  void onPartData(const std::shared_ptr<Part>& part, p_char8 data, oatpp::v_io_size size) override;

};

/**
 * Async in memory part reader.
 */
class AsyncInMemoryPartReader : public AsyncPartReader {
private:
  static const char* const TAG_NAME;
private:
  oatpp::v_io_size m_maxDataSize;
public:

  /**
   * Constructor.
   * @param maxDataSize
   */
  AsyncInMemoryPartReader(v_io_size maxDataSize = 64 * 1024);

  /**
   * Called when new part headers are parsed and part object is created.
   * @param part
   * @return - &id:oatpp::async::CoroutineStarter;.
   */
  async::CoroutineStarter onNewPartAsync(const std::shared_ptr<Part>& part) override;

  /**
   * Called on each new chunk of data is parsed for the multipart-part. <br>
   * When all data is read, called again with `data == nullptr && size == 0` to indicate end of the part.
   * @param part
   * @param data - pointer to buffer containing chunk data.
   * @param size - size of the buffer.
   * @return - &id:oatpp::async::CoroutineStarter;.
   */
  async::CoroutineStarter onPartDataAsync(const std::shared_ptr<Part>& part, p_char8 data, oatpp::v_io_size size) override;

};

/**
 * Create in-memory part reader. <br>
 * @param maxDataSize - max size of the received data.
 * @return - `std::shared_ptr` to &id:oatpp::web::mime::multipart::PartReader;.
 */
std::shared_ptr<PartReader> createInMemoryPartReader(v_io_size maxDataSize = 64 * 1024);

/**
 * Create in-memory part reader. <br>
 * @param maxDataSize - max size of the received data.
 * @return - `std::shared_ptr` to &id:oatpp::web::mime::multipart::AsyncPartReader;.
 */
std::shared_ptr<AsyncPartReader> createAsyncInMemoryPartReader(v_io_size maxDataSize = 64 * 1024);

}}}}

#endif // oatpp_web_mime_multipart_InMemoryPartReader_hpp
