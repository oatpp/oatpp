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

#include "TmpFileGuard.hpp"

#include "oatpp/core/data/stream/BufferStream.hpp"
#include "oatpp/encoding/Hex.hpp"
#include "oatpp/core/utils/Random.hpp"

namespace oatpp { namespace data {

oatpp::String TmpFileGuard::concatDirAndName(const oatpp::String& dir, const oatpp::String& filename) {
  if(dir && dir->size() > 0) {
    auto lastChar = dir->data()[dir->size() - 1];
    if(lastChar != '/' && lastChar != '\\') {
      return dir + "/" + filename;
    }
    return dir + filename;
  }
  return filename;
}

oatpp::String TmpFileGuard::constructRandomFilename(const oatpp::String& dir, v_int32 randomWordSizeBytes) {

  std::unique_ptr<v_char8[]> buff(new v_char8[randomWordSizeBytes]);
  utils::random::Random::randomBytes(buff.get(), randomWordSizeBytes);
  data::stream::BufferOutputStream s(randomWordSizeBytes * 2 + 4);
  encoding::Hex::encode(&s, buff.get(), randomWordSizeBytes, encoding::Hex::ALPHABET_LOWER);
  s << ".tmp";

  return concatDirAndName(dir, s.toString());

}

TmpFileGuard::TmpFileGuard(const oatpp::String& tmpDirectory)
  : m_fileName(constructRandomFilename(tmpDirectory, 8))
{}

TmpFileGuard::TmpFileGuard(const oatpp::String& tmpDirectory, const oatpp::String& tmpFileName)
  : m_fileName(concatDirAndName(tmpDirectory, tmpFileName))
{}

TmpFileGuard::TmpFileGuard(const oatpp::String& tmpDirectory, v_int32 randomWordSizeBytes)
  : m_fileName(constructRandomFilename(tmpDirectory, randomWordSizeBytes))
{}

TmpFileGuard::~TmpFileGuard() {
  std::remove(m_fileName->c_str());
}

oatpp::String TmpFileGuard::getFullFileName() {
  return m_fileName;
}

data::stream::FileOutputStream TmpFileGuard::openOutputStream() {
  return data::stream::FileOutputStream(m_fileName->c_str());
}

data::stream::FileInputStream TmpFileGuard::openInputStream() {
  return data::stream::FileInputStream(m_fileName->c_str());
}

}}
