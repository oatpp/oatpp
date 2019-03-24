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
 * This file contains "undefs" for DTO code generating macro. <br>
 * Usage:<br>
 *
 * ```cpp
 * #include OATPP_CODEGEN_BEGIN(DTO)
 * ...
 * // Generated Endpoints.
 * ...
 * #include OATPP_CODEGEN_END(DTO)
 * ```
 *
 *
 * *For details see:*
 * <ul>
 *   <li>[Data Transfer Object(DTO) component](https://oatpp.io/docs/components/dto/)</li>
 *   <li>&id:oatpp::data::mapping::type::Object;</li>
 * </ul>
 */

#undef DTO_INIT

// Fields

#undef OATPP_MACRO_DTO_FIELD_0

#undef OATPP_MACRO_DTO_FIELD_1

#undef OATPP_MACRO_DTO_FIELD_
#undef OATPP_MACRO_DTO_FIELD__
#undef OATPP_MACRO_DTO_FIELD___

#undef DTO_FIELD
