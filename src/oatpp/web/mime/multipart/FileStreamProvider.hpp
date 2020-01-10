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

#ifndef oatpp_web_mime_multipart_FileStreamProvider_hpp
#define oatpp_web_mime_multipart_FileStreamProvider_hpp

#include "StreamPartReader.hpp"
#include "Reader.hpp"

namespace oatpp { namespace web { namespace mime { namespace multipart {

/**
 * Stream provider for `StreamPartReader`.
 */
class FileStreamProvider : public PartReaderStreamProvider {
public:
  /**
   * Convenience typedef for &id:oatpp::data::stream::OutputStream;.
   */
  typedef oatpp::data::stream::OutputStream OutputStream;

  /**
   * Convenience typedef for &id:oatpp::data::stream::InputStream;.
   */
  typedef oatpp::data::stream::InputStream InputStream;
private:
  oatpp::String m_filename;
public:

  /**
   * Constructor.
   * @param filename
   */
  FileStreamProvider(const oatpp::String& filename);

  /**
   * Get stream to write (save) part data in.
   * @param part
   * @return
   */
  std::shared_ptr<OutputStream> getOutputStream(const std::shared_ptr<Part>& part) override;

  /**
   * Get stream to read part data from. <br>
   * This method is called after all data has been streamed to OutputStream.
   * @param part
   * @return
   */
  std::shared_ptr<InputStream> getInputStream(const std::shared_ptr<Part>& part) override;

};

/**
 * Async stream provider for `AsyncStreamPartReader`.
 */
class AsyncFileStreamProvider : public AsyncPartReaderStreamProvider {
public:
  /**
   * Convenience typedef for &id:oatpp::data::stream::OutputStream;.
   */
  typedef oatpp::data::stream::OutputStream OutputStream;

  /**
   * Convenience typedef for &id:oatpp::data::stream::InputStream;.
   */
  typedef oatpp::data::stream::InputStream InputStream;
public:

private:
  oatpp::String m_filename;
public:

  /**
   * Constructor.
   * @param filename
   */
  AsyncFileStreamProvider(const oatpp::String& filename);

  /**
   * Get stream to write (save) part data to.
   * @param part
   * @param stream - put here pointer to obtained stream.
   * @return
   */
  async::CoroutineStarter getOutputStreamAsync(const std::shared_ptr<Part>& part,
                                               std::shared_ptr<data::stream::OutputStream>& stream) override;

  /**
   * Get stream to read part data from. <br>
   * This method is called after all data has been streamed to OutputStream.
   * @param part
   * @param stream - put here pointer to obtained stream.
   * @return
   */
  async::CoroutineStarter getInputStreamAsync(const std::shared_ptr<Part>& part,
                                              std::shared_ptr<data::stream::InputStream>& stream) override;

};

/**
 * Create file part reader. <br>
 * Reader will save part to a specified file.
 * @param filename - name of the file.
 * @param maxDataSize - max size of the received data. put `-1` for no-limit.
 * @return - `std::shared_ptr` to &id:oatpp::web::mime::multipart::PartReader;.
 */
std::shared_ptr<PartReader> createFilePartReader(const oatpp::String& filename, v_io_size maxDataSize = -1);

/**
 * Create async file part reader. <br>
 * Reader will save part to a specified file.
 * @param filename - name of the file.
 * @param maxDataSize - max size of the received data. put `-1` for no-limit.
 * @return - `std::shared_ptr` to &id:oatpp::web::mime::multipart::AsyncPartReader;.
 */
std::shared_ptr<AsyncPartReader> createAsyncFilePartReader(const oatpp::String& filename, v_io_size maxDataSize = -1);

}}}}

#endif // oatpp_web_mime_multipart_FileStreamProvider_hpp
