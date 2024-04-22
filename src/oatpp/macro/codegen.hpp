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

/**[info]
 * This file contains source code for `OATPP_CODEGEN_BEGIN(NAME)` and `OATPP_CODEGEN_END(NAME)` macros. <br>
 * <br>
 *
 * More about use of `OATPP_CODEGEN_BEGIN` and `OATPP_CODEGEN_BEGIN` see: <br>
 * <ul>
 *   <li>[ApiController component](https://oatpp.io/docs/components/api-controller/)</li>
 *   <li>[ApiClient component](https://oatpp.io/docs/components/api-client/)</li>
 *   <li>[Data Transfer Object(DTO) component](https://oatpp.io/docs/components/dto/)</li>
 * </ul>
 *
 */

#ifndef oatpp_macro_codegen_hpp
#define oatpp_macro_codegen_hpp

#include "./basic.hpp"

#define OATPP_CODEGEN_DEFINE_ApiClient "oatpp/codegen/ApiClient_define.hpp"
#define OATPP_CODEGEN_UNDEF_ApiClient "oatpp/codegen/ApiClient_undef.hpp"

#define OATPP_CODEGEN_DEFINE_ApiController "oatpp/codegen/ApiController_define.hpp"
#define OATPP_CODEGEN_UNDEF_ApiController "oatpp/codegen/ApiController_undef.hpp"

#define OATPP_CODEGEN_DEFINE_DbClient "oatpp/codegen/DbClient_define.hpp"
#define OATPP_CODEGEN_UNDEF_DbClient "oatpp/codegen/DbClient_undef.hpp"

#define OATPP_CODEGEN_DEFINE_DTO "oatpp/codegen/DTO_define.hpp"
#define OATPP_CODEGEN_UNDEF_DTO "oatpp/codegen/DTO_undef.hpp"

#define OATPP_CODEGEN_BEGIN(NAME) OATPP_MACRO_EXPAND(OATPP_CODEGEN_DEFINE_ ## NAME)
#define OATPP_CODEGEN_END(NAME) OATPP_MACRO_EXPAND(OATPP_CODEGEN_UNDEF_ ## NAME)

#endif /* oatpp_macro_codegen_hpp */
