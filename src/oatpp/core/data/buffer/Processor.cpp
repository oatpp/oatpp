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

ProcessingPipeline::ProcessingPipeline(const std::vector<base::ObjectHandle<Processor>>& processors)
  : m_processors(processors)
{
  for(v_int32 i = 0; i < m_processors.size() - 1; i ++) {
    m_intermediateData.push_back(data::stream::InlineReadData());
  }
}

data::v_io_size ProcessingPipeline::suggestInputStreamReadSize() {
  return m_processors[0]->suggestInputStreamReadSize();
}

v_int32 ProcessingPipeline::iterate(data::stream::InlineReadData& dataIn,
                                    data::stream::InlineReadData& dataOut)
{

  if(dataOut.bytesLeft > 0) {
    return Error::FLUSH_DATA_OUT;
  }

  v_int32 i = 0;
  v_int32 res = Error::OK;

  while(res == Error::OK) {

    auto& p = m_processors[i];

    data::stream::InlineReadData* currDataIn = &dataIn;
    if(i > 0) {
      currDataIn = &m_intermediateData[i - 1];
    }

    data::stream::InlineReadData* currDataOut = &dataOut;
    if(i < m_intermediateData.size()) {
      currDataOut = &m_intermediateData[i];
    }

    while(res == Error::OK) {
      res = p->iterate(*currDataIn, *currDataOut);
    }

    switch (res) {
      case Error::PROVIDE_DATA_IN:
        if (i > 0) {
          i --;
          res = Error::OK;
        }
        break;


      case Error::FLUSH_DATA_OUT:
        if (i < m_processors.size() - 1) {
          i ++;
          res = Error::OK;
        }
        break;


      case Error::FINISHED:
        if (i < m_processors.size() - 1) {
          i ++;
          res = Error::OK;
        }
        break;

    }

  }

  return res;

}

}}}
