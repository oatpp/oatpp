/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>
 *                         Benedikt-Alexander Mokroß <bamkrs@github.com>
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

#ifndef oatpp_network_ConnectionHandler_hpp
#define oatpp_network_ConnectionHandler_hpp

#include "oatpp/core/data/stream/Stream.hpp"
#include <unordered_map>

namespace oatpp { namespace network {

/**
 * Abstract ConnectionHandler.
 */
class ConnectionHandler {
public:
  /**
   * Convenience typedef for &id:oatpp::data::stream::IOStream;.
   */
  typedef oatpp::data::stream::IOStream IOStream;

  /**
   * Convenience typedef for accompanying parameters of connection handling.
   */
  typedef std::unordered_map<oatpp::String, oatpp::String> ParameterMap;
public:

  /**
   * Virtual Destructor.
   */
  virtual ~ConnectionHandler() = default;

  /**
   * Handle provided connection.
   * @param connection - see &id:oatpp::data::stream::IOStream;.
   * @param params - accompanying parameters.
   */
  virtual void handleConnection(const std::shared_ptr<IOStream>& connection, const std::shared_ptr<const ParameterMap>& params) = 0;

  /**
   * Stop all threads here
   */
  virtual void stop() = 0;
};


/**
 * Connection Error
 */
class ConnectionError : public std::runtime_error {
 private:
  oatpp::v_io_size m_ioStatus;
  oatpp::String m_message;
 public:

  /**
   * Constructor.
   * @param ioStatus - I/O error. See &id:oatpp::v_io_size;.
   * @param message - error message.
   */
  ConnectionError(oatpp::v_io_size ioStatus, const oatpp::String& message);

  /**
   * Get I/O error. See &id:oatpp::v_io_size;.
   * @return &id:oatpp::v_io_size;.
   */
  oatpp::v_io_size getIOStatus();

  /**
   * Get error message.
   * @return - error message.
   */
  oatpp::String& getMessage();

};

/**
 * Communication Error
 */
class FatalConnectionError : public ConnectionError {
 public:

  /**
   * Constructor.
   * @param ioStatus - I/O error. See &id:oatpp::v_io_size;.
   * @param message - error message.
   */
  FatalConnectionError(oatpp::v_io_size ioStatus, const oatpp::String& message);
};
  
}}

#endif /* oatpp_network_ConnectionHandler_hpp */
