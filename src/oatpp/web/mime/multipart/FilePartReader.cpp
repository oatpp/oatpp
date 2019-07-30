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

#include "FilePartReader.hpp"

namespace oatpp { namespace web { namespace mime { namespace multipart {

void FilePartReader::onNewPart(const std::shared_ptr<Part>& part) {

}

void FilePartReader::onPartData(const std::shared_ptr<Part>& part, p_char8 data, oatpp::data::v_io_size size) {

}

async::CoroutineStarter AsyncFilePartReader::onNewPartAsync(const std::shared_ptr<Part>& part) {
  return nullptr;
}

async::CoroutineStarter AsyncFilePartReader::onPartDataAsync(const std::shared_ptr<Part>& part, p_char8 data, oatpp::data::v_io_size size) {
  return nullptr;
}

}}}}
