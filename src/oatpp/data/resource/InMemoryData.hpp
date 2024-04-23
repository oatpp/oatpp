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

#ifndef oatpp_data_resource_InMemoryData_hpp
#define oatpp_data_resource_InMemoryData_hpp

#include "./Resource.hpp"
#include "oatpp/data/stream/BufferStream.hpp"

namespace oatpp { namespace data { namespace resource {

class InMemoryData : public Resource {
private:

  struct DataHandle {

    oatpp::String data;

    DataHandle(const oatpp::String& pData)
      : data(pData)
    {}

  };

  struct OutputDataHandle {

    std::shared_ptr<DataHandle> dataHandle;
    oatpp::String data;
    data::stream::BufferOutputStream* stream;

    ~OutputDataHandle();

  };

private:
  std::shared_ptr<DataHandle> m_handle;
public:

  /**
   * Default constructor.
   */
  InMemoryData() = default;

  /**
   * Constructor.
   * @param fullInMemoryDataname
   */
  InMemoryData(const oatpp::String& data);

  /**
   * Open output stream to an InMemoryData. <br>
   * NOT thread-safe. <br>
   * *Note: data is committed once stream is closed.* <br>
   * *Note: stream also captures data-handle. The data won't be deleted until the stream is deleted.*
   * @return - `std::shared_ptr` to &id:oatpp::data::stream::OutputStream;.
   */
  std::shared_ptr<data::stream::OutputStream> openOutputStream() override;

  /**
   * Open input stream to an InMemoryData. <br>
   * NOT thread-safe. <br>
   * *Note: once the stream is open no subsequent writes through the output stream won't affect currently opened input streams.* <br>
   * *Note: stream also captures file-handle. The data won't be deleted until the stream is deleted.*
   * @return - `std::shared_ptr` &id:oatpp::data::stream::InputStream;.
   */
  std::shared_ptr<data::stream::InputStream> openInputStream() override;

  /**
   * Get in-memory-data.
   * @return - always returns `nullptr`.
   */
  oatpp::String getInMemoryData() override;

  /**
   * Get size of an in-memory-data.
   * @return - size of the data in bytes.
   */
  v_int64 getKnownSize() override;

  /**
   * Not applicable.
   * @return - always returns `nullptr`.
   */
  oatpp::String getLocation() override;

};

}}}

#endif //oatpp_data_resource_InMemoryData_hpp
