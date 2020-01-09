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

#ifndef oatpp_web_protocol_CommunicationError_hpp
#define oatpp_web_protocol_CommunicationError_hpp

#include "oatpp/core/IODefinitions.hpp"

namespace oatpp { namespace web { namespace protocol {

/**
 * Communication Error
 */
class CommunicationError : public std::runtime_error {
private:
  oatpp::v_io_size m_ioStatus;
  oatpp::String m_message;
public:

  /**
   * Constructor.
   * @param ioStatus - I/O error. See &id:oatpp::v_io_size;.
   * @param message - error message.
   */
  CommunicationError(oatpp::v_io_size ioStatus, const oatpp::String& message);

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
 * Protocol Error Info.
 */
template<class Status>
struct ProtocolErrorInfo {

  /**
   * Default Constructor.
   */
  ProtocolErrorInfo()
    : ioStatus(0)
  {}

  /**
   * Constructor.
   * @param pIOStatus - I/O level error. See &id:oatpp::v_io_size;.
   * @param pStatus - configurable arbitrary data type.
   */
  ProtocolErrorInfo(oatpp::v_io_size pIOStatus, const Status& pStatus)
    : ioStatus(pIOStatus)
    , status(pStatus)
  {}

  /**
   * Get I/O level error.
   */
  oatpp::v_io_size ioStatus;

  /**
   * Configurable arbitrary data type.
   */
  Status status;

};

/**
 * Protocol Error template.
 * @tparam Status - arbitrary data type.
 */
template<class Status>
class ProtocolError : public CommunicationError {
public:
  /**
   * Cenvenience typedef for ProtocolErrorInfo
   */
  typedef ProtocolErrorInfo<Status> Info;
private:
  Info m_info;
public:

  /**
   * Constructor.
   * @param info - &l:ProtocolError::Info ;.
   * @param message - error message.
   */
  ProtocolError(const Info& info, const oatpp::String& message)
    : CommunicationError(info.ioStatus, message)
    , m_info(info)
  {}

  /**
   * Get error info.
   * @return - error info.
   */
  Info getInfo() {
    return m_info;
  }
  
};


/**
 * Protocol Error template.
 * @tparam Status - arbitrary data type.
 */
template<class Status>
class AsyncProtocolError : public oatpp::AsyncIOError {
public:
  /**
   * Cenvenience typedef for ProtocolErrorInfo
   */
  typedef ProtocolErrorInfo<Status> Info;
private:
  Info m_info;
  oatpp::String m_message;
public:

  /**
   * Constructor.
   * @param info - &l:ProtocolError::Info ;.
   * @param message - error message.
   */
  AsyncProtocolError(const Info& info, const oatpp::String& message)
    : oatpp::AsyncIOError("AsyncProtocolError", info.ioStatus)
    , m_info(info)
    , m_message(message)
  {}

  /**
   * Error message.
   * @return - error message.
   */
  oatpp::String getMessage() {
    return m_message;
  }

  /**
   * Get error info.
   * @return - error info.
   */
  Info getInfo() {
    return m_info;
  }

};
  
}}}

#endif /* oatpp_web_protocol_CommunicationError_hpp */
