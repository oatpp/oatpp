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

#include "InMemoryDataProvider.hpp"

#include "oatpp/data/resource/InMemoryData.hpp"

namespace oatpp { namespace web { namespace mime { namespace multipart {

std::shared_ptr<data::resource::Resource> InMemoryDataProvider::getResource(const std::shared_ptr<Part>& part) {
  (void)part;
  return std::make_shared<data::resource::InMemoryData>();
}

async::CoroutineStarter InMemoryDataProvider::getResourceAsync(const std::shared_ptr<Part>& part,
                                                               std::shared_ptr<data::resource::Resource>& resource)
{
  (void)part;
  resource = std::make_shared<data::resource::InMemoryData>();
  return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Other functions

std::shared_ptr<PartReader> createInMemoryPartReader(v_io_size maxDataSize) {
  auto provider = std::make_shared<InMemoryDataProvider>();
  auto reader = std::make_shared<StreamPartReader>(provider, maxDataSize);
  return reader;
}

std::shared_ptr<AsyncPartReader> createAsyncInMemoryPartReader(v_io_size maxDataSize) {
  auto provider = std::make_shared<InMemoryDataProvider>();
  auto reader = std::make_shared<AsyncStreamPartReader>(provider, maxDataSize);
  return reader;
}

}}}}
