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
 * This file contains source code for `OATPP_CREATE_COMPONENT` and `OATPP_COMPONENT` macros which are part of
 * oatpp Dependency Injection (DI) framework. <br>
 * <br>
 * For usage examples see example-projects:
 * <ul>
 *   <li>[CRUD - example](https://github.com/oatpp/example-crud)</li>
 *   <li>[HLS Media Stream - example](https://github.com/oatpp/example-hls-media-stream)</li>
 *   <li>[Async API - example](https://github.com/oatpp/example-async-api)</li>
 * </ul>
 */

#ifndef oatpp_macro_component_hpp
#define oatpp_macro_component_hpp

#include "./basic.hpp"
#include "oatpp/core/base/Environment.hpp"

#define OATPP_MACRO_GET_COMPONENT_1(TYPE) \
(*((TYPE*) oatpp::base::Environment::getComponent(typeid(TYPE).name())))

#define OATPP_MACRO_GET_COMPONENT_2(TYPE, QUALIFIER) \
(*((TYPE*) oatpp::base::Environment::getComponent(typeid(TYPE).name(), QUALIFIER)))

#define OATPP_GET_COMPONENT(...) \
OATPP_MACRO_EXPAND(OATPP_MACRO_MACRO_SELECTOR(OATPP_MACRO_GET_COMPONENT_, (__VA_ARGS__)) (__VA_ARGS__))


#define OATPP_MACRO_COMPONENT_1(TYPE, NAME) \
TYPE& NAME = (*((TYPE*) oatpp::base::Environment::getComponent(typeid(TYPE).name())))

#define OATPP_MACRO_COMPONENT_2(TYPE, NAME, QUALIFIER) \
TYPE& NAME = (*((TYPE*) oatpp::base::Environment::getComponent(typeid(TYPE).name(), QUALIFIER)))

/**
 * Inject component. Create variable of type=TYPE and name=NAME and assign registered component to it.
 * @param TYPE - type of the component.
 * @param NAME - name of the variable.
 * @param QUALIFIER_NAME - qualifier name is needed if there are multiple components registered of the same type.
 * If there is one component registered only then TYPE info is enought to search for component.
 */
#define OATPP_COMPONENT(TYPE, ...) \
OATPP_MACRO_EXPAND(OATPP_MACRO_MACRO_SELECTOR(OATPP_MACRO_COMPONENT_, (__VA_ARGS__)) (TYPE, __VA_ARGS__))

/**
 * Create component that then can be injected in other application classes.
 * @param TYPE - type of the component.
 * @param NAME - name of the component field.
 */
#define OATPP_CREATE_COMPONENT(TYPE, NAME) \
oatpp::base::Environment::Component<TYPE> NAME = oatpp::base::Environment::Component<TYPE>

#endif /* oatpp_macro_component_hpp */
