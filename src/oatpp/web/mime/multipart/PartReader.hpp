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

#ifndef oatpp_web_mime_multipart_PartReader_hpp
#define oatpp_web_mime_multipart_PartReader_hpp

#include "./Multipart.hpp"
#include "oatpp/core/data/stream/Stream.hpp"

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
  virtual void onPartData(const std::shared_ptr<Part>& part, const char* data, oatpp::v_io_size size) = 0;

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
  virtual async::CoroutineStarter onPartDataAsync(const std::shared_ptr<Part>& part, const char* data, oatpp::v_io_size size) = 0;

};

/**
 * Resource provider for `StreamPartReader`.
 */
class PartReaderResourceProvider {
public:

  /**
   * Default virtual destructor.
   */
  virtual ~PartReaderResourceProvider() = default;

  /**
   * Get data resource to write (save) part data in.
   * @param part
   * @return
   */
  virtual std::shared_ptr<data::resource::Resource> getResource(const std::shared_ptr<Part>& part) = 0;

  /**
   * Get data resource to write (save) part data in.
   * @param part
   * @param resource - put here pointer to obtained resource.
   * @return
   */
  virtual async::CoroutineStarter getResourceAsync(const std::shared_ptr<Part>& part,
                                                   std::shared_ptr<data::resource::Resource>& resource) = 0;

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
    std::shared_ptr<data::resource::Resource> resource;
    std::shared_ptr<data::stream::OutputStream> outputStream;
  };

private:
  std::shared_ptr<PartReaderResourceProvider> m_resourceProvider;
  v_io_size m_maxDataSize;
public:

  /**
   * Constructor.
   * @param resourceProvider
   * @param maxDataSize - use `-1` for no limit.
   */
  StreamPartReader(const std::shared_ptr<PartReaderResourceProvider>& resourceProvider, v_io_size maxDataSize = -1);

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
  void onPartData(const std::shared_ptr<Part>& part, const char* data, oatpp::v_io_size size) override;

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
    std::shared_ptr<data::resource::Resource> resource;
    std::shared_ptr<data::stream::OutputStream> outputStream;
  };

private:
  std::shared_ptr<PartReaderResourceProvider> m_resourceProvider;
  v_io_size m_maxDataSize;
public:

  /**
   * Constructor.
   * @param resourceProvider
   * @param maxDataSize - use `-1` for no limit.
   */
  AsyncStreamPartReader(const std::shared_ptr<PartReaderResourceProvider>& resourceProvider, v_io_size maxDataSize = -1);

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
  async::CoroutineStarter onPartDataAsync(const std::shared_ptr<Part>& part, const char* data, oatpp::v_io_size size) override;

};

}}}}

#endif //oatpp_web_mime_multipart_PartReader_hpp
