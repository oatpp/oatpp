/**
 * Author: Benedikt-Alexander Mokroß <mokross@gessler.de>
 * Date: 8/6/21
 * Copyright (c) 2021 Gessler GmbH. All rights reserved.
 */

#include "ConnectionHandler.hpp"
#include "oatpp/core/concurrency/Thread.hpp"
#include "oatpp/web/server/http2/Http2Processor.hpp"

void oatpp::web::server::http2::ConnectionHandler::handleConnection(const std::shared_ptr<IOStream> &connection,
                                                                    const std::shared_ptr<const ParameterMap> &params) {
  (void)params;

  if (m_continue.load()) {

    connection->setOutputStreamIOMode(oatpp::data::stream::IOMode::BLOCKING);
    connection->setInputStreamIOMode(oatpp::data::stream::IOMode::BLOCKING);

    /* Create working thread */
    std::thread thread(&Http2Processor::Task::run, std::move(Http2Processor::Task(m_components, connection, &m_spawns)));

    /* Get hardware concurrency -1 in order to have 1cpu free of workers. */
    v_int32 concurrency = oatpp::concurrency::getHardwareConcurrency();
    if (concurrency > 1) {
      concurrency -= 1;
    }

    /* Set thread affinity group CPUs [0..cpu_count - 1]. Leave one cpu free of workers */
    oatpp::concurrency::setThreadAffinityToCpuRange(thread.native_handle(),
                                                    0,
                                                    concurrency - 1 /* -1 because 0-based index */);

    thread.detach();
  }
}
