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

#ifndef oatpp_Types_hpp
#define oatpp_Types_hpp

#include "oatpp/core/data/mapping/type/Object.hpp"
#include "oatpp/core/data/mapping/type/Primitive.hpp"

namespace oatpp {

  /**
   * Mapping-Enabled String type. &id:oatpp::data::mapping::type::String; <br>
   * For `oatpp::String` methods see &id:oatpp::base::StrBuffer;
   */
  typedef oatpp::data::mapping::type::String String;

  /**
   * Mapping-Enabled 8-bits int. Can hold nullptr value. &id:oatpp::data::mapping::type::Int8;
   */
  typedef oatpp::data::mapping::type::Int8 Int8;

  /**
   * Mapping-Enabled 8-bits unsigned int. Can hold nullptr value. &id:oatpp::data::mapping::type::UInt8;
   */
  typedef oatpp::data::mapping::type::UInt8 UInt8;

  /**
   * Mapping-Enabled 16-bits int. Can hold nullptr value. &id:oatpp::data::mapping::type::Int16;
   */
  typedef oatpp::data::mapping::type::Int16 Int16;

  /**
   * Mapping-Enabled 16-bits unsigned int. Can hold nullptr value. &id:oatpp::data::mapping::type::UInt16;
   */
  typedef oatpp::data::mapping::type::UInt16 UInt16;

  /**
   * Mapping-Enabled 32-bits int. Can hold nullptr value. &id:oatpp::data::mapping::type::Int32;
   */
  typedef oatpp::data::mapping::type::Int32 Int32;

  /**
   * Mapping-Enabled 32-bits unsigned int. Can hold nullptr value. &id:oatpp::data::mapping::type::UInt32;
   */
  typedef oatpp::data::mapping::type::UInt32 UInt32;

  /**
   * Mapping-Enabled 64-bits int. Can hold nullptr value. &id:oatpp::data::mapping::type::Int64;
   */
  typedef oatpp::data::mapping::type::Int64 Int64;

  /**
   * Mapping-Enabled 64-bits unsigned int. Can hold nullptr value. &id:oatpp::data::mapping::type::UInt64;
   */
  typedef oatpp::data::mapping::type::UInt64 UInt64;

  /**
   * Mapping-Enabled 32-bits float. Can hold nullptr value. &id:oatpp::data::mapping::type::Float32;
   */
  typedef oatpp::data::mapping::type::Float32 Float32;

  /**
   * Mapping-Enabled 64-bits float (double). Can hold nullptr value. &id:oatpp::data::mapping::type::Float64;
   */
  typedef oatpp::data::mapping::type::Float64 Float64;

  /**
   * Mapping-Enabled Boolean. Can hold nullptr value. &id:oatpp::data::mapping::type::Boolean;
   */
  typedef oatpp::data::mapping::type::Boolean Boolean;
  
}

#endif /* oatpp_Types_hpp */
