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

#ifndef oatpp_web_server_http2_HttpProcessor_hpp
#define oatpp_web_server_http2_HttpProcessor_hpp

#include "oatpp/web/server/HttpRouter.hpp"

#include "oatpp/web/server/interceptor/RequestInterceptor.hpp"
#include "oatpp/web/server/interceptor/ResponseInterceptor.hpp"
#include "oatpp/web/server/handler/ErrorHandler.hpp"

#include "oatpp/web/protocol/http/encoding/ProviderCollection.hpp"

#include "oatpp/web/protocol/http/incoming/RequestHeadersReader.hpp"
#include "oatpp/web/protocol/http/incoming/Request.hpp"

#include "oatpp/web/protocol/http/outgoing/Response.hpp"
#include "oatpp/web/protocol/http/utils/CommunicationUtils.hpp"

#include "oatpp/core/data/stream/StreamBufferedProxy.hpp"
#include "oatpp/core/async/Processor.hpp"

#include "oatpp/web/server/http2/Http2StreamHandler.hpp"
#include "oatpp/web/server/http2/Http2ProcessingComponents.hpp"
#include "oatpp/web/server/http2/PriorityStreamScheduler.hpp"
#include "oatpp/web/server/http2/Http2Settings.hpp"

namespace oatpp { namespace web { namespace server { namespace http2 {

/**
 * HttpProcessor. Helper class to handle HTTP processing.
 */
class Http2Processor {
public:
  typedef web::protocol::http::incoming::RequestHeadersReader RequestHeadersReader;
  typedef protocol::http::utils::CommunicationUtils::ConnectionState ConnectionState;

private:
  static const char* TAG;

  enum FrameType : v_uint8 {
    DATA = 0x00,
    HEADERS = 0x01,
    PRIORITY = 0x02,
    RST_STREAM = 0x03,
    SETTINGS = 0x04,
    PUSH_PROMISE = 0x05,
    PING = 0x06,
    GOAWAY = 0x07,
    WINDOW_UPDATE = 0x08,
    CONTINUATION = 0x09
  };

  struct ProcessingResources {

    ProcessingResources(const std::shared_ptr<processing::Components>& pComponents,
                        const std::shared_ptr<oatpp::data::stream::IOStream>& pConnection);

    std::shared_ptr<processing::Components> components;
    std::shared_ptr<oatpp::data::stream::IOStream> connection;
    std::shared_ptr<oatpp::data::stream::InputStreamBufferedProxy> inStream;
    std::shared_ptr<http2::PriorityStreamScheduler> outStream;

    /**
     * Collection of all streams in an ordered map
     */
    std::map<v_uint32, std::shared_ptr<Http2StreamHandler>> h2streams;

    /**
     * For now here, should be provided with a kind of factory via components
     */
    std::shared_ptr<web::protocol::http2::hpack::Hpack> hpack;


    std::shared_ptr<http2::Http2Settings> settings;

    v_uint32 flow;
  };

  static ConnectionState processNextRequest(ProcessingResources& resources);
  static Http2Processor::ConnectionState processNextRequest(const std::shared_ptr<Http2StreamHandler> &handler,
                                                            Http2Processor::ProcessingResources &resources,
                                                            FrameType type,
                                                            v_uint8 flags,
                                                            const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream,
                                                            v_io_size streamPayloadLength);
  static std::shared_ptr<Http2StreamHandler> findOrCreateStream(v_uint32 ident,
                                                                ProcessingResources &resources);
  static v_io_size consumeStream(const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream, v_io_size streamPayloadLength);

public:

  /**
   * Connection serving task. <br>
   * Usege example: <br>
   * `std::thread thread(&HttpProcessor::Task::run, HttpProcessor::Task(components, connection));`
   */
  class Task : public base::Countable {
  private:
    std::shared_ptr<processing::Components> m_components;
    std::shared_ptr<oatpp::data::stream::IOStream> m_connection;
    std::atomic_long *m_counter;
  public:

    /**
     * Constructor.
     * @param components - &l:HttpProcessor::Components;.
     * @param connection - &id:oatpp::data::stream::IOStream;.
     */
    Task(const std::shared_ptr<processing::Components>& components,
         const std::shared_ptr<oatpp::data::stream::IOStream>& connection,
         std::atomic_long *taskCounter);

    /**
     * Copy-Constructor to correctly count tasks.
     */
    Task(const Task &copy);

    /**
     * Copy-Assignment to correctly count tasks.
     * @param t - Task to copy
     * @return
     */
    Task &operator=(const Task &t);

    /**
     * Move-Constructor to correclty count tasks;
     */
     Task(Task &&move);

     /**
      * Move-Assignment to correctly count tasks.
      * @param t
      * @return
      */
    Task &operator=(Task &&t);

    /**
     * Destructor, needed for counting.
     */
    ~Task() override;

  public:

    /**
     * Run loop.
     */
    void run();

  };

};
  
}}}}

#endif /* oatpp_web_server_http2_HttpProcessor_hpp */
