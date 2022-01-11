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

#include "Socket.hpp"

namespace oatpp { namespace network { namespace virtual_ {

Socket::Socket(const std::shared_ptr<Pipe>& pipeIn, const std::shared_ptr<Pipe>& pipeOut)
  : m_pipeIn(pipeIn)
  , m_pipeOut(pipeOut)
{}

Socket::~Socket() {
  close();
}

std::shared_ptr<Socket> Socket::createShared(const std::shared_ptr<Pipe>& pipeIn, const std::shared_ptr<Pipe>& pipeOut) {
  return std::make_shared<Socket>(pipeIn, pipeOut);
}

void Socket::setMaxAvailableToReadWrtie(v_io_size maxToRead, v_io_size maxToWrite) {
  m_pipeIn->getReader()->setMaxAvailableToRead(maxToRead);
  m_pipeOut->getWriter()->setMaxAvailableToWrite(maxToWrite);
}
  
v_io_size Socket::read(void *data, v_buff_size count, async::Action& action) {
  return m_pipeIn->getReader()->read(data, count, action);
}

v_io_size Socket::write(const void *data, v_buff_size count, async::Action& action) {
  return m_pipeOut->getWriter()->write(data, count, action);
}

void Socket::setOutputStreamIOMode(oatpp::data::stream::IOMode ioMode) {
  m_pipeOut->getWriter()->setOutputStreamIOMode(ioMode);
}

oatpp::data::stream::IOMode Socket::getOutputStreamIOMode() {
  return m_pipeOut->getWriter()->getOutputStreamIOMode();
}

void Socket::setInputStreamIOMode(oatpp::data::stream::IOMode ioMode) {
  m_pipeIn->getReader()->setInputStreamIOMode(ioMode);
}

oatpp::data::stream::IOMode Socket::getInputStreamIOMode() {
  return m_pipeIn->getReader()->getInputStreamIOMode();
}

oatpp::data::stream::Context& Socket::getOutputStreamContext() {
  return m_pipeOut->getWriter()->getOutputStreamContext();
}


oatpp::data::stream::Context& Socket::getInputStreamContext() {
  return m_pipeIn->getReader()->getInputStreamContext();
}

void Socket::close() {
  m_pipeIn->close();
  m_pipeOut->close();
}
  
}}}
