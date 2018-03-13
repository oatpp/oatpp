/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi, <lganzzzo@gmail.com>
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

#ifndef oatpp_macro_component_hpp
#define oatpp_macro_component_hpp

#include "../base/Environment.hpp"
#include "./basic.hpp"

#define OATPP_MACRO_GET_COMPONENT_0(TYPE, LIST) \
(*((TYPE*) oatpp::base::Environment::getComponent(typeid(TYPE).name())))

#define OATPP_MACRO_GET_COMPONENT_1(TYPE, LIST) \
(*((TYPE*) oatpp::base::Environment::getComponent(typeid(TYPE).name(), OATPP_MACRO_FIRSTARG LIST)))

#define OATPP_MACRO_GET_COMPONENT_(X, TYPE, LIST) \
OATPP_MACRO_GET_COMPONENT_##X(TYPE, LIST)

#define OATPP_MACRO_GET_COMPONENT__(X, TYPE, LIST) \
OATPP_MACRO_GET_COMPONENT_(X, TYPE, LIST)

#define OATPP_MACRO_GET_COMPONENT___(TYPE, LIST) \
OATPP_MACRO_GET_COMPONENT__(OATPP_MACRO_HAS_ARGS LIST, TYPE, LIST)

#define OATPP_GET_COMPONENT(TYPE, ...) \
OATPP_MACRO_GET_COMPONENT___(TYPE, (__VA_ARGS__))


#define OATPP_MACRO_COMPONENT_0(TYPE, NAME, LIST) \
TYPE& NAME = (*((TYPE*) oatpp::base::Environment::getComponent(typeid(TYPE).name())))

#define OATPP_MACRO_COMPONENT_1(TYPE, NAME, LIST) \
TYPE& NAME = (*((TYPE*) oatpp::base::Environment::getComponent(typeid(TYPE).name(), OATPP_MACRO_FIRSTARG LIST)))

#define OATPP_MACRO_COMPONENT_(X, TYPE, NAME, LIST) \
OATPP_MACRO_COMPONENT_##X(TYPE, NAME, LIST)

#define OATPP_MACRO_COMPONENT__(X, TYPE, NAME, LIST) \
OATPP_MACRO_COMPONENT_(X, TYPE, NAME, LIST)

#define OATPP_MACRO_COMPONENT___(TYPE, NAME, LIST) \
OATPP_MACRO_COMPONENT__(OATPP_MACRO_HAS_ARGS LIST, TYPE, NAME, LIST)

#define OATPP_COMPONENT(TYPE, NAME, ...) \
OATPP_MACRO_COMPONENT___(TYPE, NAME, (__VA_ARGS__))


#define OATPP_CREATE_COMPONENT(TYPE, NAME) \
oatpp::base::Environment::Component<TYPE> NAME = oatpp::base::Environment::Component<TYPE>

#endif /* oatpp_macro_component_hpp */
