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

#ifndef oatpp_web_mime_multipart_StreamPartReader_hpp
#define oatpp_web_mime_multipart_StreamPartReader_hpp

#include "Reader.hpp"

#include "oatpp/core/data/stream/Stream.hpp"

namespace oatpp { namespace web { namespace mime { namespace multipart {

/**
 * Stream provider for `StreamPartReader`.
 */
class PartReaderStreamProvider {
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

  /**
   * Default virtual destructor.
   */
  virtual ~PartReaderStreamProvider() = default;

  /**
   * Get stream to write (save) part data in.
   * @param part
   * @return
   */
  virtual std::shared_ptr<OutputStream> getOutputStream(const std::shared_ptr<Part>& part) = 0;

  /**
   * Get stream to read part data from. <br>
   * This method is called after all data has been streamed to OutputStream.
   * @param part
   * @return
   */
  virtual std::shared_ptr<InputStream> getInputStream(const std::shared_ptr<Part>& part) = 0;

};

/**
 * Async stream provider for `AsyncStreamPartReader`.
 */
class AsyncPartReaderStreamProvider {
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

  /**
   * Default virtual destructor.
   */
  virtual ~AsyncPartReaderStreamProvider() = default;

  /**
   * Get stream to write (save) part data to.
   * @param part
   * @param stream - put here pointer to obtained stream.
   * @return
   */
  virtual async::CoroutineStarter getOutputStreamAsync(const std::shared_ptr<Part>& part,
                                                       std::shared_ptr<data::stream::OutputStream>& stream) = 0;

  /**
   * Get stream to read part data from. <br>
   * This method is called after all data has been streamed to OutputStream.
   * @param part
   * @param stream - put here pointer to obtained stream.
   * @return
   */
  virtual async::CoroutineStarter getInputStreamAsync(const std::shared_ptr<Part>& part,
                                                      std::shared_ptr<data::stream::InputStream>& stream) = 0;

};

/**
 * Part reader used in order to stream part data.
 */
class StreamPartReader : public PartReader {
private:
  static const char* const TAG_NAME;
private:

  class TagObject : public oatpp::base::Countable {
  public:
    v_io_size size = 0;
    std::shared_ptr<oatpp::data::stream::OutputStream> outputStream;
  };

private:
  std::shared_ptr<PartReaderStreamProvider> m_streamProvider;
  v_io_size m_maxDataSize;
public:

  /**
   * Constructor.
   * @param streamProvider
   * @param maxDataSize - use `-1` for no limit.
   */
  StreamPartReader(const std::shared_ptr<PartReaderStreamProvider>& streamProvider,
                   v_io_size maxDataSize = -1);

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
 * Async part reader used in order to stream part data in Asynchronous manner.
 */
class AsyncStreamPartReader : public AsyncPartReader {
private:
  static const char* const TAG_NAME;
private:

  class TagObject : public oatpp::base::Countable {
  public:
    v_io_size size = 0;
    std::shared_ptr<oatpp::data::stream::OutputStream> outputStream;
  };

private:
  async::CoroutineStarter onPartDone(const std::shared_ptr<Part>& part);
private:
  std::shared_ptr<AsyncPartReaderStreamProvider> m_streamProvider;
  v_io_size m_maxDataSize;
public:

  /**
   * Constructor.
   * @param streamProvider
   * @param maxDataSize - use `-1` for no limit.
   */
  AsyncStreamPartReader(const std::shared_ptr<AsyncPartReaderStreamProvider>& streamProvider,
                        v_io_size maxDataSize = -1);

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

}}}}


#endif // oatpp_web_mime_multipart_StreamPartReader_hpp
