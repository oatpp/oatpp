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

#ifndef oatpp_data_buffer_Processor_hpp
#define oatpp_data_buffer_Processor_hpp

#include "oatpp/core/data/stream/Stream.hpp"
#include "oatpp/core/base/ObjectHandle.hpp"

#include <vector>

namespace oatpp { namespace data { namespace buffer {

/**
 * Buffer processor.
 * Note: all processors are considered to stateful.
 */
class Processor {
public:

  /**
   * Enum of processing errors.
   */
  enum Error : v_int32 {

    /**
     * No error.
     */
    OK = 0,

    /**
     * Caller must set fields of `dataIn` parameter.
     */
    PROVIDE_DATA_IN = 1,

    /**
     * Caller must read all the data from the `dataOut`.
     */
    FLUSH_DATA_OUT = 2,

    /**
     * Processing is finished.
     */
    FINISHED = 3

    //*********************************************//
    // Other values are processor-specific errors. //
    //*********************************************//
  };

public:

  /**
   * Default virtual destructor.
   */
  virtual ~Processor() = default;

  /**
   * If the client is using the input stream to read data and push it to the processor,
   * the client MAY ask the processor for a suggested read size.
   * @return - suggested read size.
   */
  virtual data::v_io_size suggestInputStreamReadSize() = 0;

  /**
   * Process data.
   * @param dataIn - data provided by client to processor. Input data. &id:data::stream::InlineReadData;.
   * Set `dataIn` buffer pointer to `nullptr` to designate the end of input.
   * @param dataOut - data provided to client by processor. Output data. &id:data::stream::InlineReadData;.
   * @return - &l:Processor::Error;.
   */
  virtual v_int32 iterate(data::stream::InlineReadData& dataIn,
                          data::stream::InlineReadData& dataOut) = 0;

};

/**
 * Pipeline of buffer processors.
 */
class ProcessingPipeline : public Processor {
private:
  std::vector<base::ObjectHandle<Processor>> m_processors;
  std::vector<data::stream::InlineReadData> m_intermediateData;
public:

  /**
   * Constructor.
   * @param m_processors - the array of processors defining the pipeline.
   */
  ProcessingPipeline(const std::vector<base::ObjectHandle<Processor>>& m_processors);

  /**
   * If the client is using the input stream to read data and push it to the processor,
   * the client MAY ask the processor for a suggested read size.
   * @return - suggested read size.
   */
  data::v_io_size suggestInputStreamReadSize() override;

  /**
   * Process data.
   * @param dataIn - data provided by client to processor. Input data. &id:data::stream::InlineReadData;.
   * Set `dataIn` buffer pointer to `nullptr` to designate the end of input.
   * @param dataOut - data provided to client by processor. Output data. &id:data::stream::InlineReadData;.
   * @return - &l:Processor::Error;.
   */
  v_int32 iterate(data::stream::InlineReadData& dataIn,
                  data::stream::InlineReadData& dataOut) override;

};

}}}

#endif // oatpp_data_buffer_Processor_hpp
