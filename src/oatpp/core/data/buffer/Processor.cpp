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

#include "Processor.hpp"

namespace oatpp { namespace data{ namespace buffer {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// InlineReadData

InlineReadData::InlineReadData()
  : currBufferPtr(nullptr)
  , bytesLeft(0)
{}

InlineReadData::InlineReadData(void* data, v_buff_size size)
  : currBufferPtr(data)
  , bytesLeft(size)
{}

void InlineReadData::set(void* data, v_buff_size size) {
  currBufferPtr = data;
  bytesLeft = size;
}

void InlineReadData::inc(v_buff_size amount) {
  currBufferPtr = &((p_char8) currBufferPtr)[amount];
  bytesLeft -= amount;
}

void InlineReadData::setEof() {
  currBufferPtr = &((p_char8) currBufferPtr)[bytesLeft];
  bytesLeft = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// InlineWriteData

InlineWriteData::InlineWriteData()
  : currBufferPtr(nullptr)
  , bytesLeft(0)
{}

InlineWriteData::InlineWriteData(const void* data, v_buff_size size)
  : currBufferPtr(data)
  , bytesLeft(size)
{}

void InlineWriteData::set(const void* data, v_buff_size size) {
  currBufferPtr = data;
  bytesLeft = size;
}

void InlineWriteData::inc(v_buff_size amount) {
  currBufferPtr = &((p_char8) currBufferPtr)[amount];
  bytesLeft -= amount;
}

void InlineWriteData::setEof() {
  currBufferPtr = &((p_char8) currBufferPtr)[bytesLeft];
  bytesLeft = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ProcessingPipeline

ProcessingPipeline::ProcessingPipeline(const std::vector<base::ObjectHandle<Processor>>& processors)
  : m_processors(processors), m_intermediateData(processors.size() - 1)
{
}

v_io_size ProcessingPipeline::suggestInputStreamReadSize() {
  return m_processors[0]->suggestInputStreamReadSize();
}

v_int32 ProcessingPipeline::iterate(data::buffer::InlineReadData& dataIn,
                                    data::buffer::InlineReadData& dataOut)
{

  if(dataOut.bytesLeft > 0) {
    return Error::FLUSH_DATA_OUT;
  }

  v_int32 i = 0;
  v_int32 res = Error::OK;

  while(res == Error::OK) {

    auto& p = m_processors[i];

    data::buffer::InlineReadData* currDataIn = &dataIn;
    if(i > 0) {
      currDataIn = &m_intermediateData[i - 1];
    }

    data::buffer::InlineReadData* currDataOut = &dataOut;
    if(i < (v_int32) m_intermediateData.size()) {
      currDataOut = &m_intermediateData[i];
    }

    while(res == Error::OK) {
      res = p->iterate(*currDataIn, *currDataOut);
    }

    const v_int32 numOfProcessors = v_int32(m_processors.size());

    switch (res) {
      case Error::PROVIDE_DATA_IN:
        if (i > 0) {
          i --;
          res = Error::OK;
        }
        break;


      case Error::FLUSH_DATA_OUT:
        if (i < numOfProcessors - 1) {
          i ++;
          res = Error::OK;
        }
        break;


      case Error::FINISHED:
        if (i < numOfProcessors - 1) {
          i ++;
          res = Error::OK;
        }
        break;

    }

  }

  return res;

}

}}}
