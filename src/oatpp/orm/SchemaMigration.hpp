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

#ifndef oatpp_orm_SchemaMigration_hpp
#define oatpp_orm_SchemaMigration_hpp

#include "Executor.hpp"

namespace oatpp { namespace orm {

class SchemaMigration {
private:

  static constexpr v_int32 SOURCE_TEXT = 0;
  static constexpr v_int32 SOURCE_FILE = 1;

  struct Source {
    v_int64 version;
    v_int32 type;
    oatpp::String param;
  };

private:
  base::ObjectHandle<Executor> m_executor;
  oatpp::String m_suffix;
  std::vector<Source> m_scripts;
public:

  SchemaMigration(const base::ObjectHandle<Executor>& executor, const oatpp::String& suffix = nullptr);

  void addText(v_int64 version, const oatpp::String& script);
  void addFile(v_int64 version, const oatpp::String& filename);

  void migrate();

};

}}

#endif // oatpp_orm_SchemaMigration_hpp
