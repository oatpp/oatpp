/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Benedikt-Alexander Mokroß <github@bamkrs.de>
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

#include "Http2DelegatedConnectionHandler.hpp"
#include "oatpp/core/concurrency/Thread.hpp"
#include "oatpp/web/server/http2/Http2Processor.hpp"

void oatpp::web::server::http2::Http2DelegatedConnectionHandler::handleConnection(const std::shared_ptr<IOStream> &connection,
                                                                    const std::shared_ptr<const ParameterMap> &params) {

  Http2Processor::Task task(m_components, connection, &m_spawns);
  task.run();

}

void oatpp::web::server::http2::Http2DelegatedConnectionHandler::stop() {
}
