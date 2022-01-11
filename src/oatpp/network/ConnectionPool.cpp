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

#include "ConnectionPool.hpp"

namespace oatpp { namespace network {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ConnectionAcquisitionProxy

v_io_size ConnectionAcquisitionProxy::write(const void *buff, v_buff_size count, async::Action& action) {
  return _handle.object->write(buff, count, action);
}

v_io_size ConnectionAcquisitionProxy::read(void *buff, v_buff_size count, async::Action& action) {
  return _handle.object->read(buff, count, action);
}

void ConnectionAcquisitionProxy::setOutputStreamIOMode(oatpp::data::stream::IOMode ioMode) {
  return _handle.object->setOutputStreamIOMode(ioMode);
}

oatpp::data::stream::IOMode ConnectionAcquisitionProxy::getOutputStreamIOMode() {
  return _handle.object->getOutputStreamIOMode();
}

void ConnectionAcquisitionProxy::setInputStreamIOMode(oatpp::data::stream::IOMode ioMode) {
  return _handle.object->setInputStreamIOMode(ioMode);
}

oatpp::data::stream::IOMode ConnectionAcquisitionProxy::getInputStreamIOMode() {
  return _handle.object->getInputStreamIOMode();
}

oatpp::data::stream::Context& ConnectionAcquisitionProxy::getOutputStreamContext() {
  return _handle.object->getOutputStreamContext();
}

oatpp::data::stream::Context& ConnectionAcquisitionProxy::getInputStreamContext() {
  return _handle.object->getInputStreamContext();
}

}}
