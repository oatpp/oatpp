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

#ifndef oatpp_data_resource_Resource_hpp
#define oatpp_data_resource_Resource_hpp

#include "oatpp/core/data/stream/Stream.hpp"

namespace oatpp { namespace data { namespace resource {

/**
 * Abstract data resource
 */
class Resource : public oatpp::base::Countable {
public:

  /**
   * virtual destructor.
   */
  virtual ~Resource() = default;

  /**
   * Open output stream.
   * @return
   */
  virtual std::shared_ptr<data::stream::OutputStream> openOutputStream() = 0;

  /**
   * Open input stream.
   * @return
   */
  virtual std::shared_ptr<data::stream::InputStream> openInputStream() = 0;

  /**
   * Get in-memory data if applicable.
   * @return - `&id:oatpp::String;` or `nullptr` if not applicable.
   */
  virtual oatpp::String getInMemoryData() = 0;

  /**
   * Get known data size if applicable.
   * @return - known size of the data. `-1` - if size is unknown.
   */
  virtual v_int64 getKnownSize() = 0;

  /**
   * Get resource location if applicable. <br>
   * location can be for example a file name.
   * @return - `&id:oatpp::String;` or `nullptr` if not applicable.
   */
  virtual oatpp::String getLocation() = 0;

};

}}}

#endif //oatpp_data_resource_Resource_hpp
