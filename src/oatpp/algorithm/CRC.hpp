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

#ifndef oatpp_algorithm_CRC_hpp
#define oatpp_algorithm_CRC_hpp

#include "oatpp/core/base/Environment.hpp"

#include "oatpp/encoding/Hex.hpp"

namespace oatpp { namespace algorithm {

/**
 * Implementation of CRC-32. Cyclic redundancy check algorithm.
 */
class CRC32 {
public:

  /**
   * Precalculated table
   */
  static const p_word32 TABLE_04C11DB7;
public:

  static v_word32 bitReverse(v_word32 poly);
  
  /**
   *  Generates v_word32 table[256] for polynomial
   */
  static p_word32 generateTable(v_word32 poly);

  /**
   * Calculate CRC32 value for buffer of defined size
   * @param buffer
   * @param size
   * @param crc
   * @param initValue
   * @param xorOut
   * @param table
   * @return - CRC32 value (v_word32)
   */
  static v_word32 calc(const void *buffer, v_int32 size, v_word32 crc = 0, v_word32 initValue = 0xFFFFFFFF, v_word32 xorOut = 0xFFFFFFFF, p_word32 table = TABLE_04C11DB7);
  
};
    
}}

#endif /* oatpp_algorithm_CRC_hpp */
