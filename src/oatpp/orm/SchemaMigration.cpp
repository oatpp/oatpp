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

#include "SchemaMigration.hpp"

#include <algorithm>

namespace oatpp { namespace orm {

SchemaMigration::SchemaMigration(const base::ObjectHandle<Executor>& executor, const oatpp::String& suffix)
  : m_executor(executor)
  , m_suffix(suffix)
{}

void SchemaMigration::addText(v_int64 version, const oatpp::String& script) {
  m_scripts.push_back({version, SOURCE_TEXT, script});
}

void SchemaMigration::addFile(v_int64 version, const oatpp::String& filename) {
  m_scripts.push_back({version, SOURCE_FILE, filename});
}

void SchemaMigration::migrate() {

  if(!m_executor) {
    throw std::runtime_error("[oatpp::orm::SchemaMigration::migrate()]: Error. Executor is null.");
  }

  auto connection = m_executor->getConnection();

  if(!connection) {
    throw std::runtime_error("[oatpp::orm::SchemaMigration::migrate()]: Error. Can't connect to Database.");
  }

  v_int64 currVersion = m_executor->getSchemaVersion(m_suffix, connection);

  std::sort(m_scripts.begin(), m_scripts.end(), [](const Source& a, const Source& b) {
    return a.version < b.version;
  });

  for(auto& source : m_scripts) {

    if(source.version > currVersion) {

      oatpp::String script;

      switch (source.type) {

        case SOURCE_TEXT:
          script = source.param;
          break;

        case SOURCE_FILE:
          script = oatpp::String::loadFromFile(source.param->c_str());
          break;

        default:
          throw std::runtime_error("[oatpp::orm::SchemaMigration::migrate()]: Error. Unknown source type.");

      }

      m_executor->migrateSchema(script, source.version, m_suffix, connection);

    }

  }

}

v_int64 SchemaMigration::getSchemaVersion() {
  return m_executor->getSchemaVersion(m_suffix);
}

}}
