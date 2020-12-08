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
 * This file contains "undefs" for ApiClient code generating macro. <br>
 * Usage:<br>
 *
 * ```cpp
 * #include OATPP_CODEGEN_BEGIN(ApiClient)
 * ...
 * // Generated API-Calls.
 * ...
 * #include OATPP_CODEGEN_END(ApiClient)
 * ```
 *
 *
 * *For details see:*
 * <ul>
 *   <li>[ApiClient component](https://oatpp.io/docs/components/api-client/)</li>
 *   <li>&id:oatpp::web::client::ApiClient;</li>
 * </ul>
 */

#undef OATPP_MACRO_API_CLIENT_PARAM_MACRO
#undef OATPP_MACRO_API_CLIENT_PARAM_TYPE
#undef OATPP_MACRO_API_CLIENT_PARAM_NAME
#undef OATPP_MACRO_API_CLIENT_PARAM_TYPE_STR
#undef OATPP_MACRO_API_CLIENT_PARAM_NAME_STR
#undef OATPP_MACRO_API_CLIENT_PARAM

#undef HEADER
#undef PATH
#undef QUERY
#undef BODY
#undef BODY_DTO
#undef BODY_STRING
#undef AUTHORIZATION
#undef AUTHORIZATION_BASIC
//

#undef OATPP_MACRO_API_CLIENT_MACRO_SELECTOR

// INIT

#undef API_CLIENT_INIT

// HEADER MACRO

#undef OATPP_MACRO_API_CLIENT_HEADER_1
#undef OATPP_MACRO_API_CLIENT_HEADER_2

#undef OATPP_MACRO_API_CLIENT_HEADER

// PATH MACRO

#undef OATPP_MACRO_API_CLIENT_PATH_1
#undef OATPP_MACRO_API_CLIENT_PATH_2

#undef OATPP_MACRO_API_CLIENT_PATH

// QUERY MACRO

#undef OATPP_MACRO_API_CLIENT_QUERY_1
#undef OATPP_MACRO_API_CLIENT_QUERY_2

#undef OATPP_MACRO_API_CLIENT_QUERY

// BODY MACRO

#undef OATPP_MACRO_API_CLIENT_BODY

// BODY_DTO MACRO

#undef OATPP_MACRO_API_CLIENT_BODY_DTO

// BODY_STRING MACRO

#undef OATPP_MACRO_API_CLIENT_BODY_STRING

// AUTHORIZATION MACRO

#undef OATPP_MACRO_API_CLIENT_AUTHORIZATION_2

#undef OATPP_MACRO_API_CLIENT_AUTHORIZATION

// AUTHORIZATION_BASIC MACRO

#undef OATPP_MACRO_API_CLIENT_AUTHORIZATION_BASIC_1

#undef OATPP_MACRO_API_CLIENT_AUTHORIZATION_BASIC

// FOR EACH

#undef OATPP_MACRO_API_CLIENT_PARAM_DECL
#undef OATPP_MACRO_API_CLIENT_PARAM_PUT

// API_CALL MACRO

#undef OATPP_API_CALL_0
#undef OATPP_API_CALL_1

#undef OATPP_API_CALL_MACRO_0
#undef OATPP_API_CALL_MACRO_1

#undef API_CALL

// API_CALL_ASYNC MACRO

#undef OATPP_API_CALL_ASYNC_0
#undef OATPP_API_CALL_ASYNC_1

#undef OATPP_API_CALL_ASYNC_MACRO_0
#undef OATPP_API_CALL_ASYNC_MACRO_1

#undef API_CALL_ASYNC

#undef API_CALL_HEADERS
