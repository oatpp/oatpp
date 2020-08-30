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

#include "DbClient.hpp"

#include "oatpp/core/data/stream/BufferStream.hpp"

namespace oatpp { namespace orm {

DbClient::DbClient(const std::shared_ptr<Executor>& executor)
  : m_executor(executor)
{}

void DbClient::types_putDtoFields(Executor::ParamsTypeMap& map,
                                  const Type* type,
                                  const data::share::StringKeyLabel& paramNamespace)
{

  if(type->classId.id != data::mapping::type::__class::AbstractObject::CLASS_ID.id) {
    throw std::runtime_error("[oatpp::orm::DbClient::types_putDtoFields()]: "
                             "Error. At the moment 'PARAMS_DTO' macro supports 'DTO' data-type only.");
  }

  const auto& fieldsMap = type->propertiesGetter()->getMap();

  data::stream::BufferOutputStream stream;
  stream.writeSimple(paramNamespace.getData(), paramNamespace.getSize());
  stream.writeSimple(".", 1);

  for(auto& f : fieldsMap) {

    const std::string& fname = f.first;
    auto field = f.second;

    stream.setCurrentPosition(paramNamespace.getSize() + 1);
    stream.writeSimple(fname.data(), fname.size());

    data::share::StringKeyLabel key = stream.toString();
    map.insert({ key, field->type});

  }

}

void DbClient::params_putDtoFields(std::unordered_map<oatpp::String, oatpp::Void>& params,
                                   const oatpp::Void& object,
                                   const data::share::StringKeyLabel& paramNamespace)
{

  auto type = object.valueType;

  if(type->classId.id != data::mapping::type::__class::AbstractObject::CLASS_ID.id) {
    throw std::runtime_error("[oatpp::orm::DbClient::params_putDtoFields()]: "
                             "Error. At the moment 'PARAMS_DTO' macro supports 'DTO' data-type only.");
  }

  const auto& fieldsMap = type->propertiesGetter()->getMap();

  data::stream::BufferOutputStream stream;
  stream.writeSimple(paramNamespace.getData(), paramNamespace.getSize());
  stream.writeSimple(".", 1);

  for(auto& f : fieldsMap) {

    const std::string& fname = f.first;
    auto field = f.second;

    stream.setCurrentPosition(paramNamespace.getSize() + 1);
    stream.writeSimple(fname.data(), fname.size());

    oatpp::String key = stream.toString();
    params.insert({key, field->get(object.get())});

  }

}

std::shared_ptr<Connection> DbClient::getConnection() {
  return m_executor->getConnection();
}

std::shared_ptr<QueryResult> DbClient::executeQuery(const oatpp::String& query,
                                                    const std::unordered_map<oatpp::String, oatpp::Void>& params,
                                                    const std::shared_ptr<Connection>& connection)
{
  return m_executor->execute(query, params, connection);
}

Transaction DbClient::beginTransaction(const std::shared_ptr<Connection>& connection) {
  return Transaction(m_executor, connection);
}

}}
