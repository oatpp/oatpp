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

/**
 * This is list of default configuration params and values which you can
 * configure in the build-time
 */

#ifndef oatpp_base_Config_hpp
#define oatpp_base_Config_hpp

/**
 * If NOT DISABLED, counting of all object of class oatpp::base::Countable is enabled
 * for debug purposes and detection of memory leaks.
 * Disable object counting for Release builds using '-D OATPP_DISABLE_ENV_OBJECT_COUNTERS' flag for better performance
 */
//#define OATPP_DISABLE_ENV_OBJECT_COUNTERS

/**
 * Define this to disable memory-pool allocations.
 * This will make oatpp::base::memory::MemoryPool, method obtain and free call new and delete directly
 */
//#define OATPP_DISABLE_POOL_ALLOCATIONS

/**
 * Predefined value for function oatpp::concurrency::Thread::getHardwareConcurrency();
 */
//#define OATPP_THREAD_HARDWARE_CONCURRENCY 4

/**
 * Number of shards of ThreadDistributedMemoryPool (Default pool for many oatpp objects)
 * Higher number reduces threads racing for resources on each shard.
 */
#ifndef OATPP_THREAD_DISTRIBUTED_MEM_POOL_SHARDS_COUNT
  #define OATPP_THREAD_DISTRIBUTED_MEM_POOL_SHARDS_COUNT 10
#endif

/**
 * Disable `thread_local` feature. <br>
 * See https://github.com/oatpp/oatpp/issues/81
 */
//#define OATPP_COMPAT_BUILD_NO_THREAD_LOCAL 1

#ifndef OATPP_FLOAT_STRING_FORMAT
  #define OATPP_FLOAT_STRING_FORMAT "%.16g"
#endif

/**
 * DISABLE logs priority V
 */
//#define OATPP_DISABLE_LOGV

/**
 * DISABLE logs priority D
 */
//#define OATPP_DISABLE_LOGD

/**
 * DISABLE logs priority I
 */
//#define OATPP_DISABLE_LOGI

/**
 * DISABLE logs priority W
 */
//#define OATPP_DISABLE_LOGW

/**
 * DISABLE logs priority E
 */
//#define OATPP_DISABLE_LOGE


#endif /* oatpp_base_Config_hpp */
