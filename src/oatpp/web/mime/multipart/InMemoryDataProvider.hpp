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

#ifndef oatpp_web_mime_multipart_InMemoryDataProvider_hpp
#define oatpp_web_mime_multipart_InMemoryDataProvider_hpp

#include "PartReader.hpp"
#include "Reader.hpp"

namespace oatpp { namespace web { namespace mime { namespace multipart {

class InMemoryDataProvider : public PartReaderResourceProvider {
public:

  std::shared_ptr<data::resource::Resource> getResource(const std::shared_ptr<Part>& part) override;

  async::CoroutineStarter getResourceAsync(const std::shared_ptr<Part>& part,
                                           std::shared_ptr<data::resource::Resource>& resource) override;

};

/**
 * Create in-memory part reader.
 * @param maxDataSize - max-allowed size of the data. Use `-1` for infinity.
 * @return
 */
std::shared_ptr<PartReader> createInMemoryPartReader(v_io_size maxDataSize);

/**
 * Create Async in-memory part reader.
 * @param maxDataSize - max-allowed size of the data. Use `-1` for infinity.
 * @return
 */
std::shared_ptr<AsyncPartReader> createAsyncInMemoryPartReader(v_io_size maxDataSize);

}}}}

#endif //oatpp_web_mime_multipart_InMemoryDataProvider_hpp
