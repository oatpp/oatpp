/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>
 *                         Benedikt-Alexander Mokroß <bam@icognize.de>
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

#define BUNDLE(TYPE, ...) \
OATPP_MACRO_API_CONTROLLER_PARAM(OATPP_MACRO_API_CONTROLLER_BUNDLE, OATPP_MACRO_API_CONTROLLER_BUNDLE_INFO, TYPE, (__VA_ARGS__))

// BUNDLE MACRO // ------------------------------------------------------

#define OATPP_MACRO_API_CONTROLLER_BUNDLE_1(TYPE, NAME) \
TYPE NAME = __request->getBundleData<TYPE>(#NAME);

#define OATPP_MACRO_API_CONTROLLER_BUNDLE_2(TYPE, NAME, QUALIFIER) \
TYPE NAME = __request->getBundleData<TYPE>(QUALIFIER);

#define OATPP_MACRO_API_CONTROLLER_BUNDLE(TYPE, PARAM_LIST) \
OATPP_MACRO_API_CONTROLLER_MACRO_SELECTOR(OATPP_MACRO_API_CONTROLLER_BUNDLE_, TYPE, OATPP_MACRO_UNFOLD_VA_ARGS PARAM_LIST)

#define OATPP_MACRO_API_CONTROLLER_BUNDLE_INFO(TYPE, PARAM_LIST)
