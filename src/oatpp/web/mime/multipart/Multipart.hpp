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

#ifndef oatpp_web_mime_multipart_Multipart_hpp
#define oatpp_web_mime_multipart_Multipart_hpp

#include "Part.hpp"
#include <list>

namespace oatpp { namespace web { namespace mime { namespace multipart {

/**
 * Typedef for headers map. Headers map key is case-insensitive.
 * For more info see &id:oatpp::data::share::LazyStringMap;.
 */
typedef oatpp::data::share::LazyStringMultimap<oatpp::data::share::StringKeyLabelCI> Headers;

/**
 * Abstract Multipart.
 */
class Multipart {
private:
  oatpp::String m_boundary;
public:

  /**
   * Constructor.
   * @param boundary - multipart boundary value.
   */
  Multipart(const oatpp::String& boundary);

  /**
   * Default virtual Destructor.
   */
  virtual ~Multipart() = default;

  /**
   * Get multipart boundary value.
   * @return - multipart boundary value.
   */
  oatpp::String getBoundary();

  /**
   * Read part-by-part from Multipart.
   * @return
   */
  virtual std::shared_ptr<Part> readNextPart(async::Action& action) = 0;

  /**
   * Write part-by-part to Multipart.
   * @param part
   */
  virtual void writeNextPart(const std::shared_ptr<Part>& part, async::Action& action) = 0;

  /**
   * Read part-by-part from Multipart. <br>
   * Call writeNextPart(...) and throw if `action.isNone() == false`.
   * @return
   */
  std::shared_ptr<Part> readNextPartSimple();

  /**
   * Write part-by-part to Multipart.
   * Call writeNextPartSimple(...) and throw if `action.isNone() == false`.
   * @param part
   */
  void writeNextPartSimple(const std::shared_ptr<Part>& part);

public:

  /**
   * Generate random boundary for Multipart object. Base64 encoded.
   * @param boundarySize - size in bytes of random vector.
   * @return - &id:oatpp::String;.
   */
  static oatpp::String generateRandomBoundary(v_int32 boundarySize = 15);

  /**
   * Parse boundary value from headers
   * @param headers
   * @return
   */
  static oatpp::String parseBoundaryFromHeaders(const Headers& requestHeaders);

};



}}}}


#endif // oatpp_web_mime_multipart_Multipart_hpp
