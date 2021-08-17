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

#include "oatpp/web/protocol/http2/Http2.hpp"
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
  typedef protocol::http2::Frame::Header::FrameType FrameType;
  typedef protocol::http2::Frame::Header FrameHeader;

private:
  static const char* TAG;

  struct ProcessingResources {

    ProcessingResources(const std::shared_ptr<processing::Components>& pComponents,
                        const std::shared_ptr<oatpp::data::stream::IOStream>& pConnection);

    ProcessingResources(const std::shared_ptr<processing::Components>& pComponents,
                        const std::shared_ptr<http2::Http2Settings>& pInSettings,
                        const std::shared_ptr<http2::Http2Settings>& pOutSettings,
                        const std::shared_ptr<oatpp::data::stream::InputStreamBufferedProxy>& pInStream,
                        const std::shared_ptr<http2::PriorityStreamScheduler>& pOutStream);
    virtual ~ProcessingResources();

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


    std::shared_ptr<http2::Http2Settings> inSettings;
    std::shared_ptr<http2::Http2Settings> outSettings;

    v_uint32 flow;
  };

  static void stop(ProcessingResources& resources);
  static ConnectionState invalidateConnection(ProcessingResources& resources);
  static ConnectionState processNextRequest(ProcessingResources& resources);
  static ConnectionState delegateToHandler(const std::shared_ptr<Http2StreamHandler> &handler,
                                          const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream,
                                          Http2Processor::ProcessingResources &resources,
                                          protocol::http2::Frame::Header &header);
  static std::shared_ptr<Http2StreamHandler> findOrCreateStream(v_uint32 ident,
                                                                ProcessingResources &resources);
  static v_io_size consumeStream(const std::shared_ptr<data::stream::InputStreamBufferedProxy> &stream, v_io_size streamPayloadLength);

  static v_io_size sendSettingsFrame(Http2Processor::ProcessingResources &resources);
  static v_io_size ackSettingsFrame(Http2Processor::ProcessingResources &resources);
  static v_io_size answerPingFrame(Http2Processor::ProcessingResources &resources);
  static v_io_size sendGoawayFrame(Http2Processor::ProcessingResources &resources,
                                   v_uint32 lastStream,
                                   v_uint32 errorCode);

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
    const std::shared_ptr<const network::ConnectionHandler::ParameterMap> m_delegationParameters;
  public:

    /**
     * Constructor.
     * @param components - &l:HttpProcessor::Components;.
     * @param connection - &id:oatpp::data::stream::IOStream;.
     * @param taskCounter - Counter to increment for every creates task
     */
    Task(const std::shared_ptr<processing::Components>& components,
         const std::shared_ptr<oatpp::data::stream::IOStream>& connection,
         std::atomic_long *taskCounter);

    /**
     * Constructor.
     * @param components - &l:HttpProcessor::Components;.
     * @param connection - &id:oatpp::data::stream::IOStream;.
     * @param taskCounter - Counter to increment for every creates task
     * @param delegationParameters - Parameter-Map from delegation (if any)
     */
    Task(const std::shared_ptr<processing::Components>& components,
         const std::shared_ptr<oatpp::data::stream::IOStream>& connection,
         std::atomic_long *taskCounter,
         const std::shared_ptr<const network::ConnectionHandler::ParameterMap> &delegationParameters);

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
