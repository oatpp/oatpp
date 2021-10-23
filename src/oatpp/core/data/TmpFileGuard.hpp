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

#ifndef oatpp_data_TmpFileGuard_hpp
#define oatpp_data_TmpFileGuard_hpp

#include "oatpp/core/data/stream/FileStream.hpp"
#include "oatpp/core/Types.hpp"

namespace oatpp { namespace data {

class TmpFileGuard {
private:
  static oatpp::String concatDirAndName(const oatpp::String& dir, const oatpp::String& filename);
  static oatpp::String constructRandomFilename(const oatpp::String& dir, v_int32 randomWordSizeBytes);
private:
  oatpp::String m_fileName;
public:

  TmpFileGuard(const oatpp::String& tmpDirectory);
  TmpFileGuard(const oatpp::String& tmpDirectory, const oatpp::String& tmpFileName);
  TmpFileGuard(const oatpp::String& tmpDirectory, v_int32 randomWordSizeBytes);

  virtual ~TmpFileGuard();

  oatpp::String getFullFileName();

  data::stream::FileOutputStream openOutputStream();
  data::stream::FileInputStream openInputStream();

};

}}

#endif //oatpp_data_TmpFileGuard_hpp
