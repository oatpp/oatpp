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

#ifndef oatpp_network_Address_hpp
#define oatpp_network_Address_hpp

#include "oatpp/Types.hpp"

namespace oatpp { namespace network {

/**
 * Network address.
 */
class Address {
public:

  /**
   * Address family.
   */
  enum Family : v_int32 {

    /**
     * IPv4.
     */
    IP_4 = 0,

    /**
     * IPv6.
     */
    IP_6 = 1,

    /**
     * Unspecified.
     */
    UNSPEC = 2
  };

public:

  /**
   * Constructor.
   * @param pHost
   * @param pPort
   * @param pFamily
   */
  Address(const oatpp::String& pHost, v_uint16 pPort, Family pFamily = UNSPEC);

  /**
   * Host name without schema and port. Ex.: "oatpp.io", "127.0.0.1", "localhost".
   */
  oatpp::String host;

  /**
   * Port.
   */
  v_uint16 port;

  /**
   * Family &l:Address::Family;.
   */
  Family family;

};

}}

#endif // oatpp_network_Address_hpp
